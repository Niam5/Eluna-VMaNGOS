/*
 * Copyright (C) 2005-2011 MaNGOS <http://getmangos.com/>
 * Copyright (C) 2009-2011 MaNGOSZero <https://github.com/mangos/zero>
 * Copyright (C) 2011-2016 Nostalrius <https://nostalrius.org>
 * Copyright (C) 2016-2017 Elysium Project <https://github.com/elysium-project>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef MANGOS_OBJECT_GUID_H
#define MANGOS_OBJECT_GUID_H

#include <functional>
#include <queue>
#include <unordered_set>

#include "Common.h"
#include "ByteBuffer.h"
#include "Policies/ThreadingModel.h"

enum TypeID
{
    TYPEID_OBJECT        = 0,
    TYPEID_ITEM          = 1,
    TYPEID_CONTAINER     = 2,
    TYPEID_UNIT          = 3,
    TYPEID_PLAYER        = 4,
    TYPEID_GAMEOBJECT    = 5,
    TYPEID_DYNAMICOBJECT = 6,
    TYPEID_CORPSE        = 7
};

#define MAX_TYPE_ID        8

enum TypeMask
{
    TYPEMASK_OBJECT         = 0x0001,
    TYPEMASK_ITEM           = 0x0002,
    TYPEMASK_CONTAINER      = 0x0004,
    TYPEMASK_UNIT           = 0x0008,                       // players also have it
    TYPEMASK_PLAYER         = 0x0010,
    TYPEMASK_GAMEOBJECT     = 0x0020,
    TYPEMASK_DYNAMICOBJECT  = 0x0040,
    TYPEMASK_CORPSE         = 0x0080,

    // used combinations in Player::GetObjectByTypeMask (TYPEMASK_UNIT case ignore players in call)
    TYPEMASK_CREATURE_OR_GAMEOBJECT = TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT,
    TYPEMASK_CREATURE_GAMEOBJECT_OR_ITEM = TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT | TYPEMASK_ITEM,
    TYPEMASK_CREATURE_GAMEOBJECT_PLAYER_OR_ITEM = TYPEMASK_UNIT | TYPEMASK_GAMEOBJECT | TYPEMASK_ITEM | TYPEMASK_PLAYER,

    TYPEMASK_WORLDOBJECT = TYPEMASK_UNIT | TYPEMASK_PLAYER | TYPEMASK_GAMEOBJECT | TYPEMASK_DYNAMICOBJECT | TYPEMASK_CORPSE,
};

enum HighGuid
{
    HIGHGUID_ITEM           = 0x4000,                       // blizz 4000
    HIGHGUID_CONTAINER      = 0x4000,                       // blizz 4000
    HIGHGUID_PLAYER         = 0x0000,                       // blizz 0000
    HIGHGUID_GAMEOBJECT     = 0xF110,                       // blizz F110
    HIGHGUID_TRANSPORT      = 0xF120,                       // blizz F120 (for GAMEOBJECT_TYPE_TRANSPORT)
    HIGHGUID_UNIT           = 0xF130,                       // blizz F130
    HIGHGUID_PET            = 0xF140,                       // blizz F140
    HIGHGUID_DYNAMICOBJECT  = 0xF100,                       // blizz F100
    HIGHGUID_CORPSE         = 0xF101,                       // blizz F100
    HIGHGUID_MO_TRANSPORT   = 0x1FC0,                       // blizz 1FC0 (for GAMEOBJECT_TYPE_MO_TRANSPORT)
};

// NOSTALRIUS : Code supprime par MaNGOS. Eviter de l'utiliser.
#define GUID_HIPART(x)   (uint32)((uint64(x) >> 48) & 0x0000FFFF)
// We have different low and middle part size for different guid types
#define _GUID_LOPART_2(x) (uint32)(uint64(x)         & UI64LIT(0x00000000FFFFFFFF))
#define _GUID_LOPART_3(x) (uint32)(uint64(x)         & UI64LIT(0x0000000000FFFFFF))

// Pour les codes TrinityCore
#define IS_EMPTY_GUID(g)       (g == 0)
#define IS_CREATURE_GUID(g)    (GUID_HIPART(g) == HIGHGUID_UNIT)
#define IS_PET_GUID(g)         (GUID_HIPART(g) == HIGHGUID_PET)
#define IS_PLAYER_GUID(g)      (GUID_HIPART(g) == HIGHGUID_PLAYER && g != 0)
#define IS_UNIT_GUID(g)        (IS_CREATURE_GUID(g) || IS_PLAYER_GUID(g))

inline bool IsGuidHaveEnPart(uint64 const& guid)
{
    switch(GUID_HIPART(guid))
    {
        case HIGHGUID_ITEM:
        case HIGHGUID_PLAYER:
        case HIGHGUID_DYNAMICOBJECT:
        case HIGHGUID_CORPSE:
        case HIGHGUID_MO_TRANSPORT:
            return false;
        case HIGHGUID_GAMEOBJECT:
        case HIGHGUID_TRANSPORT:
        case HIGHGUID_UNIT:
        case HIGHGUID_PET:
        default:
            return true;
    }
}
#define GUID_LOPART(x) (IsGuidHaveEnPart(x) ? _GUID_LOPART_3(x) : _GUID_LOPART_2(x))
// FIN Nostalrius.


class ObjectGuid;
class PackedGuid;

struct PackedGuidReader
{
    explicit PackedGuidReader(ObjectGuid& guid) : m_guidPtr(&guid) {}
    ObjectGuid* m_guidPtr;
};

class ObjectGuid
{
    public:                                                 // constructors
        ObjectGuid() : m_guid(0) {}
        ObjectGuid(uint64 const& guid) : m_guid(guid) {}    // temporary allowed implicit cast, really bad in connection with operator uint64()
        ObjectGuid(HighGuid hi, uint32 entry, uint32 counter) : m_guid(counter ? uint64(counter) | (uint64(entry) << 24) | (uint64(hi) << 48) : 0) {}
        ObjectGuid(HighGuid hi, uint32 counter) : m_guid(counter ? uint64(counter) | (uint64(hi) << 48) : 0) {}

    //private:
        explicit ObjectGuid(uint32 const& lowGuid) : m_guid(lowGuid) {} // Besoin dans OutdoorPvP par exemple
        operator uint64() const { return m_guid; }
    private:
        ObjectGuid(HighGuid, uint32, uint64 counter);       // no implementation, used for catch wrong type assign
        ObjectGuid(HighGuid, uint64 counter);               // no implementation, used for catch wrong type assign

    public:                                                 // modifiers
        PackedGuidReader ReadAsPacked() { return PackedGuidReader(*this); }

        void Set(uint64 const& guid);
        void Clear() { m_guid = 0; }

        PackedGuid WriteAsPacked() const;
    public:                                                 // accessors
        uint64 const& GetRawValue() const { return m_guid; }
        static HighGuid GetHigh(uint64 guid) { return HighGuid((guid >> 48) & 0x0000FFFF); }
        static void ClampPlayerGuid(uint64& value);
        HighGuid GetHigh() const { return GetHigh(m_guid); }
        uint32   GetEntry() const { return HasEntry() ? uint32((m_guid >> 24) & UI64LIT(0x0000000000FFFFFF)) : 0; }
        uint32   GetCounter()  const
        {
            return GetCounter(m_guid, HasEntry());
        }

        static uint32 GetCounter(uint64 guid, bool hasEntry)
        {
            return hasEntry
                ? uint32(guid & UI64LIT(0x0000000000FFFFFF))
                : uint32(guid & UI64LIT(0x00000000FFFFFFFF));
        }

        static uint32 GetMaxCounter(HighGuid high)
        {
            return HasEntry(high)
                ? uint32(0x00FFFFFF)
                : uint32(0xFFFFFFFF);
        }

        uint32 GetMaxCounter() const { return GetMaxCounter(GetHigh()); }

        bool IsEmpty()         const { return m_guid == 0; }
        bool IsCreature()      const { return GetHigh() == HIGHGUID_UNIT; }
        bool IsPet()           const { return GetHigh() == HIGHGUID_PET; }
        bool IsCreatureOrPet() const { return IsCreature() || IsPet(); }
        bool IsAnyTypeCreature() const { return IsCreature() || IsPet(); }
        bool IsPlayer()        const { return !IsEmpty() && GetHigh() == HIGHGUID_PLAYER; }
        bool IsUnit()          const { return IsAnyTypeCreature() || IsPlayer(); }
        bool IsItem()          const { return GetHigh() == HIGHGUID_ITEM; }
        bool IsGameObject()    const { return GetHigh() == HIGHGUID_GAMEOBJECT; }
        bool IsDynamicObject() const { return GetHigh() == HIGHGUID_DYNAMICOBJECT; }
        bool IsCorpse()        const { return GetHigh() == HIGHGUID_CORPSE; }
        bool IsTransport()     const { return GetHigh() == HIGHGUID_TRANSPORT; }
        bool IsMOTransport()   const { return GetHigh() == HIGHGUID_MO_TRANSPORT; }

        static TypeID GetTypeId(HighGuid high)
        {
            switch(high)
            {
                case HIGHGUID_ITEM:         return TYPEID_ITEM;
                //case HIGHGUID_CONTAINER:    return TYPEID_CONTAINER; HIGHGUID_CONTAINER==HIGHGUID_ITEM currently
                case HIGHGUID_UNIT:         return TYPEID_UNIT;
                case HIGHGUID_PET:          return TYPEID_UNIT;
                case HIGHGUID_PLAYER:       return TYPEID_PLAYER;
                case HIGHGUID_GAMEOBJECT:   return TYPEID_GAMEOBJECT;
                case HIGHGUID_DYNAMICOBJECT:return TYPEID_DYNAMICOBJECT;
                case HIGHGUID_CORPSE:       return TYPEID_CORPSE;
                case HIGHGUID_MO_TRANSPORT: return TYPEID_GAMEOBJECT;
                // unknown
                default:                    return TYPEID_OBJECT;
            }
        }

        TypeID GetTypeId() const { return GetTypeId(GetHigh()); }

        bool operator! () const { return IsEmpty(); }
        bool operator== (ObjectGuid const& guid) const { return GetRawValue() == guid.GetRawValue(); }
        bool operator!= (ObjectGuid const& guid) const { return GetRawValue() != guid.GetRawValue(); }
        bool operator< (ObjectGuid const& guid) const { return GetRawValue() < guid.GetRawValue(); }

    public:                                                 // accessors - for debug
        static char const* GetTypeName(HighGuid high);
        char const* GetTypeName() const { return !IsEmpty() ? GetTypeName(GetHigh()) : "None"; }
        std::string GetString() const;

    private:                                                // internal functions
        static bool HasEntry(HighGuid high)
        {
            switch(high)
            {
                case HIGHGUID_ITEM:
                case HIGHGUID_PLAYER:
                case HIGHGUID_DYNAMICOBJECT:
                case HIGHGUID_CORPSE:
                case HIGHGUID_MO_TRANSPORT:
                    return false;
                case HIGHGUID_GAMEOBJECT:
                case HIGHGUID_TRANSPORT:
                case HIGHGUID_UNIT:
                case HIGHGUID_PET:
                default:
                    return true;
            }
        }

        bool HasEntry() const { return HasEntry(GetHigh()); }

    private:                                                // fields
        uint64 m_guid;
};

template <>
struct std::hash<ObjectGuid>
{
    std::size_t operator()(ObjectGuid const& k) const
    {
        return std::hash<uint64>()(k.GetRawValue());
    }
};

typedef std::unordered_set<ObjectGuid> ObjectGuidSet;
typedef std::list<ObjectGuid> GuidList;

//minimum buffer size for packed guid is 9 bytes
#define PACKED_GUID_MIN_BUFFER_SIZE 9

class PackedGuid
{
    friend ByteBuffer& operator<< (ByteBuffer& buf, PackedGuid const& guid);

    public:                                                 // constructors
        explicit PackedGuid() : m_packedGuid(PACKED_GUID_MIN_BUFFER_SIZE) { m_packedGuid.appendPackGUID(0); }
        explicit PackedGuid(uint64 const& guid) : m_packedGuid(PACKED_GUID_MIN_BUFFER_SIZE) { m_packedGuid.appendPackGUID(guid); }
        explicit PackedGuid(ObjectGuid const& guid) : m_packedGuid(PACKED_GUID_MIN_BUFFER_SIZE) { m_packedGuid.appendPackGUID(guid.GetRawValue()); }

    public:                                                 // modifiers
        void Set(uint64 const& guid) { m_packedGuid.wpos(0); m_packedGuid.appendPackGUID(guid); }
        void Set(ObjectGuid const& guid) { m_packedGuid.wpos(0); m_packedGuid.appendPackGUID(guid.GetRawValue()); }

    public:                                                 // accessors
        size_t size() const { return m_packedGuid.size(); }

    private:                                                // fields
        ByteBuffer m_packedGuid;
};

template<HighGuid high>
class ObjectGuidGenerator
{
    public:                                                 // constructors
        explicit ObjectGuidGenerator(uint32 start = 1) : m_nextGuid(start) {}

    public:                                                 // modifiers
        void SetMaxUsedGuid(uint32 val, char const* guidType);
        uint32 Generate();
        void GenerateRange(uint32& first, uint32& last);

    public:                                                 // accessors
        uint32 GetNextAfterMaxUsed() const { return m_nextGuid; }
        void FreeGuid(uint32 guid) { m_freedGuids.push(guid); }

    private:                                                // fields
        uint32 m_nextGuid;
        std::queue<uint32> m_freedGuids;
};

template<HighGuid high>
class ObjectSafeGuidGenerator : public ObjectGuidGenerator<high>
{
public:
    uint32 Generate()
    {
        Guard _g(lock);
        return ObjectGuidGenerator<high>::Generate();
    }
    void FreeGuid(uint32 g)
    {
        Guard _g(lock);
        ObjectGuidGenerator<high>::FreeGuid(g);
    }
    void GenerateRange(uint32& first, uint32& last)
    {
        Guard _g(lock);
        ObjectGuidGenerator<high>::GenerateRange(first, last);
    }
protected:
    using LockType = std::mutex;
    using Guard = MaNGOS::GeneralLock<LockType>;
    LockType lock;
};

ByteBuffer& operator<< (ByteBuffer& buf, ObjectGuid const& guid);
ByteBuffer& operator>> (ByteBuffer& buf, ObjectGuid&       guid);

ByteBuffer& operator<< (ByteBuffer& buf, PackedGuid const& guid);
ByteBuffer& operator>> (ByteBuffer& buf, PackedGuidReader const& guid);

inline PackedGuid ObjectGuid::WriteAsPacked() const { return PackedGuid(*this); }

#endif
