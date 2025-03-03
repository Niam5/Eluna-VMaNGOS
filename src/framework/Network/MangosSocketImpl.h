#include <ace/Message_Block.h>
#include <ace/OS_NS_string.h>
#include <ace/OS_NS_unistd.h>
#include <ace/os_include/arpa/os_inet.h>
#include <ace/os_include/netinet/os_tcp.h>
#include <ace/os_include/sys/os_types.h>
#include <ace/os_include/sys/os_socket.h>
#include <ace/OS_NS_string.h>
#include <ace/Reactor.h>

#include "MangosSocket.h"
#include "Common.h"

#include "Util.h"
#include "WorldPacket.h"
#include "SharedDefines.h"
#include "ByteBuffer.h"
#include "Database/DatabaseEnv.h"
#include "WorldSession.h"
#include "Log.h"
#include "DBCStores.h"


template <typename SessionType, typename SocketName, typename Crypt>
MangosSocket<SessionType, SocketName, Crypt>::MangosSocket() :
    WorldHandler(),
    m_createTime(ACE_OS::gettimeofday()),
    m_lastPingTime(ACE_Time_Value::zero),
    m_overSpeedPings(0),
    m_session(0),
    m_recvWPct(0),
    m_recvPct(),
    m_header(sizeof(ClientPktHeader)),
    m_outBuffer(0),
    m_outBufferSize(65536),
    m_outActive(false),
    m_seed(static_cast<uint32>(rand32())),
    m_isServerSocket(true)
{
    reference_counting_policy().value(ACE_Event_Handler::Reference_Counting_Policy::ENABLED);
}

template <typename SessionType, typename SocketName, typename Crypt>
MangosSocket<SessionType, SocketName, Crypt>::~MangosSocket(void)
{
    delete m_recvWPct;

    if (m_outBuffer)
        m_outBuffer->release();

    closing_ = true;

    peer().close();

    WorldPacket* pct;
    while (m_packetQueue.dequeue_head(pct) == 0)
        delete pct;
}

template <typename SessionType, typename SocketName, typename Crypt>
void MangosSocket<SessionType, SocketName, Crypt>::CloseSocket(void)
{
    {
        GuardType lock(m_outBufferLock);

        if (closing_)
            return;

        closing_ = true;
        peer().close_writer();
    }

    {
        GuardType lock(m_sessionLock);

        m_session = nullptr;
    }
}

template <typename SessionType, typename SocketName, typename Crypt>
int MangosSocket<SessionType, SocketName, Crypt>::SendPacket(WorldPacket const& pct)
{
    GuardType lock(m_outBufferLock);

    if (closing_)
        return -1;

    if (((SocketName*)this)->iSendPacket(pct) == -1)
    {
        WorldPacket* npct;

        ACE_NEW_RETURN(npct, WorldPacket(pct), -1);

        // NOTE maybe check of the size of the queue can be good ?
        // to make it bounded instead of unbounded
        if (m_packetQueue.enqueue_tail(npct) == -1)
        {
            delete npct;
            sLog.Out(LOG_BASIC, LOG_LVL_ERROR, "MangosSocket<SessionType, SocketName, Crypt>::SendPacket: m_packetQueue.enqueue_tail failed");
            return -1;
        }
    }

    return 0;
}

template <typename SessionType, typename SocketName, typename Crypt>
int MangosSocket<SessionType, SocketName, Crypt>::open(void *a)
{
    ACE_UNUSED_ARG(a);

    // Prevent double call to this func.
    if (m_outBuffer)
        return -1;

    // This will also prevent the socket from being Updated
    // while we are initializing it.
    m_outActive = true;

    // Hook for the manager.
    if (((SocketName*)this)->OnSocketOpen() == -1)
        return -1;

    // Allocate the buffer.
    ACE_NEW_RETURN(m_outBuffer, ACE_Message_Block(m_outBufferSize), -1);

    // Store peer address.
    ACE_INET_Addr remote_addr;

    if (peer().get_remote_addr(remote_addr) == -1)
    {
        sLog.Out(LOG_BASIC, LOG_LVL_ERROR, "WorldSocket::open: peer ().get_remote_addr errno = %s", ACE_OS::strerror(errno));
        return -1;
    }

    m_address = remote_addr.get_host_addr();

    if (((SocketName*)this)->SendStartupPacket() == -1)
        return -1;

    // Register with ACE Reactor
    if (reactor()->register_handler(this, ACE_Event_Handler::READ_MASK | ACE_Event_Handler::WRITE_MASK) == -1)
    {
        sLog.Out(LOG_BASIC, LOG_LVL_ERROR, "WorldSocket::open: unable to register client handler errno = %s", ACE_OS::strerror(errno));
        return -1;
    }

    // reactor takes care of the socket from now on
    remove_reference();

    return 0;
}

template <typename SessionType, typename SocketName, typename Crypt>
int MangosSocket<SessionType, SocketName, Crypt>::close(int)
{
    shutdown();

    closing_ = true;

    remove_reference();

    return 0;
}

template <typename SessionType, typename SocketName, typename Crypt>
int MangosSocket<SessionType, SocketName, Crypt>::handle_input(ACE_HANDLE)
{
    if (closing_)
        return -1;

    switch (handle_input_missing_data())
    {
        case -1 :
        {
            if ((errno == EWOULDBLOCK) ||
                    (errno == EAGAIN))
            {
                return Update();                            // interesting line ,isn't it ?
            }

            sLog.Out(LOG_BASIC, LOG_LVL_DEBUG, "WorldSocket::handle_input: Peer error closing connection errno = %s", ACE_OS::strerror(errno));

            errno = ECONNRESET;
            return -1;
        }
        case 0:
        {
            sLog.Out(LOG_BASIC, LOG_LVL_DEBUG, "WorldSocket::handle_input: Peer has closed connection");

            errno = ECONNRESET;
            return -1;
        }
        case 1:
            return 1;
        default:
            return Update();                                // another interesting line ;)
    }

    ACE_NOTREACHED(return -1);
}

template <typename SessionType, typename SocketName, typename Crypt>
int MangosSocket<SessionType, SocketName, Crypt>::handle_output(ACE_HANDLE)
{
    GuardType lock(m_outBufferLock);

    if (closing_)
        return -1;

    const size_t send_len = m_outBuffer->length();

    if (send_len == 0)
        return cancel_wakeup_output(lock);

#ifdef MSG_NOSIGNAL
    ssize_t n = peer().send(m_outBuffer->rd_ptr(), send_len, MSG_NOSIGNAL);
#else
    ssize_t n = peer().send(m_outBuffer->rd_ptr(), send_len);
#endif // MSG_NOSIGNAL

    if (n == 0)
        return -1;
    else if (n == -1)
    {
#ifdef _WIN32
        if (WSAGetLastError() == WSAEWOULDBLOCK)
            return schedule_wakeup_output(lock);
#endif

        if (errno == EWOULDBLOCK || errno == EAGAIN)
            return schedule_wakeup_output(lock);

        return -1;
    }
    else if (n < (ssize_t)send_len) //now n > 0
    {
        m_outBuffer->rd_ptr(static_cast<size_t>(n));

        // move the data to the base of the buffer
        m_outBuffer->crunch();

        return schedule_wakeup_output(lock);
    }
    else //now n == send_len
    {
        m_outBuffer->reset();

        if (!iFlushPacketQueue())
            return cancel_wakeup_output(lock);
        else
            return schedule_wakeup_output(lock);
    }

    ACE_NOTREACHED(return 0);
}

template <typename SessionType, typename SocketName, typename Crypt>
int MangosSocket<SessionType, SocketName, Crypt>::handle_close(ACE_HANDLE h, ACE_Reactor_Mask)
{
    // Critical section
    {
        GuardType lock(m_outBufferLock);

        closing_ = true;

        if (h == ACE_INVALID_HANDLE)
            peer().close_writer();
    }

    // Critical section
    {
        GuardType lock(m_sessionLock);

        m_session = nullptr;
    }

    reactor()->remove_handler(this, ACE_Event_Handler::DONT_CALL | ACE_Event_Handler::ALL_EVENTS_MASK);
    return 0;
}

template <typename SessionType, typename SocketName, typename Crypt>
int MangosSocket<SessionType, SocketName, Crypt>::Update(void)
{
    if (closing_)
        return -1;
    
    if (!m_session && m_isServerSocket && ((ACE_OS::gettimeofday() - m_createTime) > ACE_Time_Value(10)))
    {
        sLog.Out(LOG_BASIC, LOG_LVL_DETAIL, "Disconnecting idle connection from %s.", m_address.c_str());
        CloseSocket();
        return -1;
    }

    if (m_outActive || m_outBuffer->length() == 0)
        return 0;

    return handle_output(get_handle());
}

template <typename SessionType, typename SocketName, typename Crypt>
int MangosSocket<SessionType, SocketName, Crypt>::handle_input_header(void)
{
    MANGOS_ASSERT(m_recvWPct == nullptr);

    MANGOS_ASSERT(m_header.length() == sizeof(ClientPktHeader));

    m_crypt.DecryptRecv((uint8*) m_header.rd_ptr(), sizeof(ClientPktHeader));

    ClientPktHeader& header = *((ClientPktHeader*) m_header.rd_ptr());

    EndianConvertReverse(header.size);
    EndianConvert(header.cmd);

    if ((header.size < 4) || (header.size > 10240) || (header.cmd  > 10240))
    {
        sLog.Out(LOG_BASIC, LOG_LVL_ERROR, "WorldSocket::handle_input_header: client %s sent malformed packet size = %d , cmd = %d",
                    GetRemoteAddress().c_str(), header.size, header.cmd);

        errno = EINVAL;
        return -1;
    }

    header.size -= 4;

    ACE_NEW_RETURN(m_recvWPct, WorldPacket((uint16) header.cmd, header.size), -1);

    if (header.size > 0)
    {
        m_recvWPct->resize(header.size);
        m_recvPct.base((char*) m_recvWPct->contents(), m_recvWPct->size());
    }
    else
        MANGOS_ASSERT(m_recvPct.space() == 0);

    return 0;
}

template <typename SessionType, typename SocketName, typename Crypt>
int MangosSocket<SessionType, SocketName, Crypt>::handle_input_payload(void)
{
    // set errno properly here on error !!!
    // now have a header and payload

    MANGOS_ASSERT(m_recvPct.space() == 0);
    MANGOS_ASSERT(m_header.space() == 0);
    MANGOS_ASSERT(m_recvWPct != nullptr);

    const int ret = ((SocketName*)this)->ProcessIncoming(m_recvWPct);

    m_recvPct.base(nullptr, 0);
    m_recvPct.reset();
    m_recvWPct = nullptr;

    m_header.reset();

    if (ret == -1)
        errno = EINVAL;

    return ret;
}

template <typename SessionType, typename SocketName, typename Crypt>
int MangosSocket<SessionType, SocketName, Crypt>::handle_input_missing_data(void)
{
    char buf [4096];

    ACE_Data_Block db(sizeof(buf),
                      ACE_Message_Block::MB_DATA,
                      buf,
                      0,
                      0,
                      ACE_Message_Block::DONT_DELETE,
                      0);

    ACE_Message_Block message_block(&db,
                                    ACE_Message_Block::DONT_DELETE,
                                    0);

    const size_t recv_size = message_block.space();

    const ssize_t n = peer().recv(message_block.wr_ptr(),
                                  recv_size);

    if (n <= 0)
        return (int)n;

    message_block.wr_ptr(n);

    while (message_block.length() > 0)
    {
        if (m_header.space() > 0)
        {
            //need to receive the header
            const size_t to_header = (message_block.length() > m_header.space() ? m_header.space() : message_block.length());
            m_header.copy(message_block.rd_ptr(), to_header);
            message_block.rd_ptr(to_header);

            if (m_header.space() > 0)
            {
                // Couldn't receive the whole header this time.
                MANGOS_ASSERT(message_block.length() == 0);
                errno = EWOULDBLOCK;
                return -1;
            }

            // We just received nice new header
            if (handle_input_header() == -1)
            {
                MANGOS_ASSERT((errno != EWOULDBLOCK) && (errno != EAGAIN));
                return -1;
            }
        }

        // Its possible on some error situations that this happens
        // for example on closing when epoll receives more chunked data and stuff
        // hope this is not hack ,as proper m_recvWPct is asserted around
        if (!m_recvWPct)
        {
            sLog.Out(LOG_BASIC, LOG_LVL_ERROR, "Forcing close on input m_recvWPct = nullptr");
            errno = EINVAL;
            return -1;
        }

        // We have full read header, now check the data payload
        if (m_recvPct.space() > 0)
        {
            //need more data in the payload
            const size_t to_data = (message_block.length() > m_recvPct.space() ? m_recvPct.space() : message_block.length());
            m_recvPct.copy(message_block.rd_ptr(), to_data);
            message_block.rd_ptr(to_data);

            if (m_recvPct.space() > 0)
            {
                // Couldn't receive the whole data this time.
                MANGOS_ASSERT(message_block.length() == 0);
                errno = EWOULDBLOCK;
                return -1;
            }
        }

        //just received fresh new payload
        if (handle_input_payload() == -1)
        {
            MANGOS_ASSERT((errno != EWOULDBLOCK) && (errno != EAGAIN));
            return -1;
        }
    }

    return size_t(n) == recv_size ? 1 : 2;
}

template <typename SessionType, typename SocketName, typename Crypt>
int MangosSocket<SessionType, SocketName, Crypt>::cancel_wakeup_output(GuardType& g)
{
    if (!m_outActive)
        return 0;

    m_outActive = false;

    g.unlock();

    if (reactor()->cancel_wakeup
            (this, ACE_Event_Handler::WRITE_MASK) == -1)
    {
        // would be good to store errno from reactor with errno guard
        sLog.Out(LOG_BASIC, LOG_LVL_ERROR, "MangosSocket<SessionType, SocketName, Crypt>::cancel_wakeup_output");
        return -1;
    }

    return 0;
}

template <typename SessionType, typename SocketName, typename Crypt>
int MangosSocket<SessionType, SocketName, Crypt>::schedule_wakeup_output(GuardType& g)
{
    if (m_outActive)
        return 0;

    m_outActive = true;

    g.unlock();

    if (reactor()->schedule_wakeup
            (this, ACE_Event_Handler::WRITE_MASK) == -1)
    {
        sLog.Out(LOG_BASIC, LOG_LVL_ERROR, "MangosSocket<SessionType, SocketName, Crypt>::schedule_wakeup_output");
        return -1;
    }

    return 0;
}

template <typename SessionType, typename SocketName, typename Crypt>
int MangosSocket<SessionType, SocketName, Crypt>::iSendPacket(WorldPacket const& pct)
{
    if (m_outBuffer->space() < pct.size() + sizeof(ServerPktHeader))
    {
        errno = ENOBUFS;
        return -1;
    }

    ServerPktHeader header;

    header.cmd = pct.GetOpcode();

    header.size = (uint16) pct.size() + 2;

    EndianConvertReverse(header.size);
    EndianConvert(header.cmd);

    m_crypt.EncryptSend((uint8*) & header, sizeof(header));

    if (m_outBuffer->copy((char*) & header, sizeof(header)) == -1)
        ACE_ASSERT(false);

    if (!pct.empty())
        if (m_outBuffer->copy((char*) pct.contents(), pct.size()) == -1)
            ACE_ASSERT(false);

    return 0;
}

template <typename SessionType, typename SocketName, typename Crypt>
bool MangosSocket<SessionType, SocketName, Crypt>::iFlushPacketQueue()
{
    WorldPacket *pct;
    bool haveone = false;

    while (m_packetQueue.dequeue_head(pct) == 0)
    {
        if (((SocketName*)this)->iSendPacket(*pct) == -1)
        {
            if (m_packetQueue.enqueue_head(pct) == -1)
            {
                delete pct;
                sLog.Out(LOG_BASIC, LOG_LVL_ERROR, "MangosSocket<SessionType, SocketName, Crypt>::iFlushPacketQueue m_packetQueue->enqueue_head");
                return false;
            }

            break;
        }
        else
        {
            haveone = true;
            delete pct;
        }
    }

    return haveone;
}
