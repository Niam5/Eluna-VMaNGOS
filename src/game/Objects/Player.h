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

#ifndef _PLAYER_H
#define _PLAYER_H

#include "Common.h"
#include "Unit.h"
#include "Database/DatabaseEnv.h"
#include "GroupReference.h"
#include "MapReference.h"
#include "WorldSession.h"
#include "Pet.h"
#include "Util.h"                                           // for Tokens typedef
#include "ReputationMgr.h"
#include "BattleGroundDefines.h"
#include "SharedDefines.h"
#include "GameObjectDefines.h"
#include "SpellMgr.h"
#include "HonorMgr.h"
#include "PlayerTaxi.h"
#include "MirrorTimer.h"

#include <string>
#include <utility>
#include <vector>
#include <functional>
#include <shared_mutex>

struct Mail;
struct ItemPrototype;
struct AuraSaveStruct;
struct SpellModifier;
class Group;
class Channel;
class Creature;
class PlayerMenu;
class UpdateMask;
class SpellCastTargets;
class PlayerSocial;
class TradeData;
class DungeonPersistentState;
class Spell;
class Item;
class ZoneScript;
class PlayerAI;
class PlayerBroadcaster;
class BattleGround;

#define PLAYER_MAX_SKILLS           127
#define PLAYER_EXPLORED_ZONES_SIZE  64
#define CORPSE_REPOP_TIME (6 * MINUTE * IN_MILLISECONDS)

enum EnvironmentFlags
{
    ENVIRONMENT_FLAG_NONE           = 0x00,
    ENVIRONMENT_FLAG_IN_WATER       = 0x01,                     // Swimming or standing in water
    ENVIRONMENT_FLAG_IN_MAGMA       = 0x02,                     // Swimming or standing in magma
    ENVIRONMENT_FLAG_IN_SLIME       = 0x04,                     // Swimming or standing in slime
    ENVIRONMENT_FLAG_HIGH_SEA       = 0x08,                     // Anywhere inside deep water area
    ENVIRONMENT_FLAG_UNDERWATER     = 0x10,                     // Swimming fully submerged in any liquid
    ENVIRONMENT_FLAG_HIGH_LIQUID    = 0x20,                     // In any liquid deep enough to be able to swim
    ENVIRONMENT_FLAG_LIQUID         = 0x40,                     // Anywhere indide area with any liquid

    ENVIRONMENT_MASK_LIQUID_HAZARD  = (ENVIRONMENT_FLAG_IN_MAGMA | ENVIRONMENT_FLAG_IN_SLIME),
    ENVIRONMENT_MASK_IN_LIQUID      = (ENVIRONMENT_FLAG_IN_WATER | ENVIRONMENT_MASK_LIQUID_HAZARD),
    ENVIRONMENT_MASK_LIQUID_FLAGS   = (ENVIRONMENT_FLAG_UNDERWATER | ENVIRONMENT_MASK_IN_LIQUID | ENVIRONMENT_FLAG_HIGH_SEA | ENVIRONMENT_FLAG_LIQUID | ENVIRONMENT_FLAG_HIGH_LIQUID),
};

enum BuyBankSlotResult
{
    ERR_BANKSLOT_FAILED_TOO_MANY    = 0,
    ERR_BANKSLOT_INSUFFICIENT_FUNDS = 1,
    ERR_BANKSLOT_NOTBANKER          = 2,
    ERR_BANKSLOT_OK                 = 3
};

enum PlayerSpellState
{
    PLAYERSPELL_UNCHANGED = 0,
    PLAYERSPELL_CHANGED   = 1,
    PLAYERSPELL_NEW       = 2,
    PLAYERSPELL_REMOVED   = 3
};

struct PlayerSpell
{
    PlayerSpellState state : 8;
    bool active            : 1;                             // show in spellbook
    bool dependent         : 1;                             // learned as result another spell learn, skill grow, quest reward, etc
    bool disabled          : 1;                             // first rank has been learned in result talent learn but currently talent unlearned, save max learned ranks
};

typedef std::unordered_map<uint32, PlayerSpell> PlayerSpellMap;

typedef std::list<SpellModifier*> SpellModList;

enum TrainerSpellState
{
    TRAINER_SPELL_GREEN = 0,
    TRAINER_SPELL_RED   = 1,
    TRAINER_SPELL_GRAY  = 2,
    TRAINER_SPELL_GREEN_DISABLED = 10                       // custom value, not send to client: formally green but learn not allowed
};

enum ActionButtonUpdateState
{
    ACTIONBUTTON_UNCHANGED = 0,
    ACTIONBUTTON_CHANGED   = 1,
    ACTIONBUTTON_NEW       = 2,
    ACTIONBUTTON_DELETED   = 3
};

enum ActionButtonType
{
    ACTION_BUTTON_SPELL     = 0x00,
    ACTION_BUTTON_C         = 0x01,                         // click?
    ACTION_BUTTON_MACRO     = 0x40,
    ACTION_BUTTON_CMACRO    = ACTION_BUTTON_C | ACTION_BUTTON_MACRO,
    ACTION_BUTTON_ITEM      = 0x80
};

#define ACTION_BUTTON_ACTION(X) (uint32(X) & 0x00FFFFFF)
#define ACTION_BUTTON_TYPE(X)   ((uint32(X) & 0xFF000000) >> 24)
#define MAX_ACTION_BUTTON_ACTION_VALUE (0x00FFFFFF+1)

struct ActionButton
{
    uint32 packedData = 0;
    ActionButtonUpdateState uState = ACTIONBUTTON_NEW;

    // helpers
    ActionButtonType GetType() const { return ActionButtonType(ACTION_BUTTON_TYPE(packedData)); }
    uint32 GetAction() const { return ACTION_BUTTON_ACTION(packedData); }
    void SetActionAndType(uint32 action, ActionButtonType type)
    {
        uint32 newData = action | (uint32(type) << 24);
        if (newData != packedData || uState == ACTIONBUTTON_DELETED)
        {
            packedData = newData;
            if (uState != ACTIONBUTTON_NEW)
                uState = ACTIONBUTTON_CHANGED;
        }
    }
};

#define  MAX_ACTION_BUTTONS 120   // TBC 132 checked in 2.3.0

struct PlayerCreateInfoItem
{
    PlayerCreateInfoItem(uint32 id, uint32 amount) : item_id(id), item_amount(amount) {}

    uint32 item_id;
    uint32 item_amount;
};

typedef std::vector<PlayerCreateInfoItem> PlayerCreateInfoItems;

struct PlayerClassLevelInfo
{
    uint16 basehealth = 0;
    uint16 basemana = 0;
};

struct PlayerClassInfo
{
    PlayerClassLevelInfo* levelInfo = nullptr;                        //[level-1] 0..MaxPlayerLevel-1
};

struct PlayerLevelInfo
{
    uint8 stats[MAX_STATS] = { 0 };
};

typedef std::vector<uint32> PlayerCreateInfoSpells;

struct PlayerCreateInfoAction
{
    PlayerCreateInfoAction() = default;
    PlayerCreateInfoAction(uint8 _button, uint32 _action, uint8 _type) : button(_button), type(_type), action(_action) {}

    uint8 button = 0;
    uint8 type = 0;
    uint32 action = 0;
};

typedef std::vector<PlayerCreateInfoAction> PlayerCreateInfoActions;

struct PlayerInfo
{
    uint32 mapId = 0;
    uint32 areaId = 0;
    float positionX = 0.0f;
    float positionY = 0.0f;
    float positionZ = 0.0f;
    float orientation = 0.0f;
    uint16 displayId_m = 0;
    uint16 displayId_f = 0;
    PlayerCreateInfoItems item;
    PlayerCreateInfoSpells spell;
    PlayerCreateInfoActions action;

    PlayerLevelInfo* levelInfo = nullptr;                             //[level-1] 0..MaxPlayerLevel-1
};

struct PvPInfo
{
    bool inPvPCombat = false;
    bool inPvPEnforcedArea = false;
    bool inPvPCapturePoint = false;
    bool isPvPFlagCarrier = false;
    uint32 timerPvPRemaining = 0;
    uint32 timerPvPContestedRemaining = 0;
};

struct DuelInfo
{
    Player* initiator = nullptr;
    Player* opponent = nullptr;
    time_t startTimer = 0;
    time_t startTime = 0;
    time_t outOfBound = 0;
    uint32 transportGuid = 0;
    bool finished = false;
};

struct Areas
{
    uint32 areaID = 0;
    uint32 areaFlag = 0;
    float x1 = 0.0f;
    float x2 = 0.0f;
    float y1 = 0.0f;
    float y2 = 0.0f;
};

struct EnchantDuration
{
    EnchantDuration() = default;
    EnchantDuration(Item* _item, EnchantmentSlot _slot, uint32 _leftduration) : item(_item), slot(_slot), leftduration(_leftduration) { MANGOS_ASSERT(item); };

    Item* item = nullptr;
    EnchantmentSlot slot = MAX_ENCHANTMENT_SLOT;
    uint32 leftduration = 0;
};

typedef std::list<EnchantDuration> EnchantDurationList;
typedef std::list<Item*> ItemDurationList;

enum RaidGroupError
{
    ERR_RAID_GROUP_REQUIRED = 1,
    ERR_RAID_GROUP_FULL     = 2
};

enum DrunkenState
{
    DRUNKEN_SOBER   = 0,
    DRUNKEN_TIPSY   = 1,
    DRUNKEN_DRUNK   = 2,
    DRUNKEN_SMASHED = 3
};

#define MAX_DRUNKEN   4

enum PlayerFlags
{
    PLAYER_FLAGS_NONE                   = 0x00000000,
    PLAYER_FLAGS_GROUP_LEADER           = 0x00000001,
    PLAYER_FLAGS_AFK                    = 0x00000002,
    PLAYER_FLAGS_DND                    = 0x00000004,
    PLAYER_FLAGS_GM                     = 0x00000008,
    PLAYER_FLAGS_GHOST                  = 0x00000010,
    PLAYER_FLAGS_RESTING                = 0x00000020,
    PLAYER_FLAGS_UNK7                   = 0x00000040,       // admin?
    PLAYER_FLAGS_FFA_PVP                = 0x00000080,
    PLAYER_FLAGS_CONTESTED_PVP          = 0x00000100,       // Player has been involved in a PvP combat and will be attacked by contested guards
    PLAYER_FLAGS_PVP_DESIRED            = 0x00000200,       // Stores player's permanent PvP flag preference
    PLAYER_FLAGS_HIDE_HELM              = 0x00000400,
    PLAYER_FLAGS_HIDE_CLOAK             = 0x00000800,
#if SUPPORTED_CLIENT_BUILD < CLIENT_BUILD_1_6_1
    PLAYER_FLAGS_CAN_SELF_RESURRECT     = 0x00001000,
#endif
    PLAYER_FLAGS_PARTIAL_PLAY_TIME      = 0x00001000,       // played long time
    PLAYER_FLAGS_NO_PLAY_TIME           = 0x00002000,       // played too long time
    PLAYER_FLAGS_UNK15                  = 0x00004000,
    PLAYER_FLAGS_UNK16                  = 0x00008000,       // strange visual effect (2.0.1), looks like PLAYER_FLAGS_GHOST flag
    PLAYER_FLAGS_SANCTUARY              = 0x00010000,       // player entered sanctuary
    PLAYER_FLAGS_TAXI_BENCHMARK         = 0x00020000,       // taxi benchmark mode (on/off) (2.0.1)
    PLAYER_FLAGS_PVP_TIMER              = 0x00040000,       // 3.0.2, pvp timer active (after you disable pvp manually)
};

enum PlayerBytesOffsets
{
    PLAYER_BYTES_OFFSET_SKIN_ID         = 0,
    PLAYER_BYTES_OFFSET_FACE_ID         = 1,
    PLAYER_BYTES_OFFSET_HAIR_STYLE_ID   = 2,
    PLAYER_BYTES_OFFSET_HAIR_COLOR_ID   = 3
};

enum PlayerBytes2Offsets
{
    PLAYER_BYTES_2_OFFSET_FACIAL_STYLE   = 0,
    PLAYER_BYTES_2_OFFSET_UNK1           = 1,
    PLAYER_BYTES_2_OFFSET_BANK_BAG_SLOTS = 2,
    PLAYER_BYTES_2_OFFSET_REST_STATE     = 3
};

enum PlayerBytes3Offsets
{
    PLAYER_BYTES_3_OFFSET_GENDER_AND_INEBRIATION = 0, // uint16, 1 bit for gender, rest for drunk state
    PLAYER_BYTES_3_OFFSET_CITY_PROTECTOR_TITLE   = 2, // race id
    PLAYER_BYTES_3_OFFSET_HONOR_RANK             = 3
};

enum PlayerFieldBytesOffsets
{
    PLAYER_FIELD_BYTES_OFFSET_FLAGS              = 0,
    PLAYER_FIELD_BYTES_OFFSET_COMBO_POINTS       = 1,
    PLAYER_FIELD_BYTES_OFFSET_ACTION_BARS        = 2,
    PLAYER_FIELD_BYTES_OFFSET_HIGHEST_HONOR_RANK = 3
};

enum PlayerFieldBytes2Offsets
{
    PLAYER_FIELD_BYTES_2_OFFSET_HONOR_RANK_BAR = 0,
    PLAYER_FIELD_BYTES_2_OFFSET_FLAGS          = 1,
    PLAYER_FIELD_BYTES_2_OFFSET_UNK2           = 2,
    PLAYER_FIELD_BYTES_2_OFFSET_UNK3           = 3
};

// used in (PLAYER_FIELD_BYTES, 0) byte values
enum PlayerFieldByteFlags
{
    PLAYER_FIELD_BYTE_TRACK_STEALTHED   = 0x02,
    PLAYER_FIELD_BYTE_RELEASE_TIMER     = 0x08,             // Display time till auto release spirit
    PLAYER_FIELD_BYTE_NO_RELEASE_WINDOW = 0x10              // Display no "release spirit" window at all
};

// used in byte (PLAYER_FIELD_BYTES2,1) values
enum PlayerFieldByte2Flags
{
    PLAYER_FIELD_BYTE2_NONE              = 0x00,
    PLAYER_FIELD_BYTE2_DETECT_AMORE      = 0x01,            // SPELL_AURA_DETECT_AMORE
    PLAYER_FIELD_BYTE2_STEALTH           = 0x20,
    PLAYER_FIELD_BYTE2_INVISIBILITY_GLOW = 0x40
};

#if SUPPORTED_CLIENT_BUILD <= CLIENT_BUILD_1_8_4
enum WhoListPartyStatus
{
    WHO_PARTY_STATUS_NOT_IN_PARTY = 0x0,
    WHO_PARTY_STATUS_IN_PARTY     = 0x1,
    WHO_PARTY_STATUS_LFG          = 0x2
};
#endif

enum ActivateTaxiReplies
{
    ERR_TAXIOK                      = 0,
    ERR_TAXIUNSPECIFIEDSERVERERROR  = 1,
    ERR_TAXINOSUCHPATH              = 2,
    ERR_TAXINOTENOUGHMONEY          = 3,
    ERR_TAXITOOFARAWAY              = 4,
    ERR_TAXINOVENDORNEARBY          = 5,
    ERR_TAXINOTVISITED              = 6,
    ERR_TAXIPLAYERBUSY              = 7,
    ERR_TAXIPLAYERALREADYMOUNTED    = 8,
    ERR_TAXIPLAYERSHAPESHIFTED      = 9,
    ERR_TAXIPLAYERMOVING            = 10,
    ERR_TAXISAMENODE                = 11,
    ERR_TAXINOTSTANDING             = 12
};

// 2^n values
enum AtLoginFlags
{
    AT_LOGIN_NONE              = 0x00,
    AT_LOGIN_RENAME            = 0x01,
    AT_LOGIN_RESET_SPELLS      = 0x02,
    AT_LOGIN_RESET_TALENTS     = 0x04,
    //AT_LOGIN_CUSTOMIZE         = 0x08, -- used in post-3.x
    //AT_LOGIN_RESET_PET_TALENTS = 0x10, -- used in post-3.x
    AT_LOGIN_FIRST             = 0x20,
};

enum PlayerCheatOptions : uint16
{
    PLAYER_CHEAT_FLY               = 0x001,
    PLAYER_CHEAT_NO_COOLDOWN       = 0x002,
    PLAYER_CHEAT_NO_CAST_TIME      = 0x004,
    PLAYER_CHEAT_NO_POWER          = 0x008,
    PLAYER_CHEAT_DEBUFF_IMMUNITY   = 0x010,
    PLAYER_CHEAT_ALWAYS_CRIT       = 0x020,
    PLAYER_CHEAT_NO_CHECK_CAST     = 0x040,
    PLAYER_CHEAT_ALWAYS_PROC       = 0x080,
    PLAYER_CHEAT_TRIGGER_PASS      = 0x100,
    PLAYER_CHEAT_IGNORE_TRIGGERS   = 0x200,
    PLAYER_CHEAT_DEBUG_TARGET_INFO = 0x400,
    PLAYER_CHEAT_FIXED_Z           = 0x800,
};

typedef std::map<uint32, QuestStatusData> QuestStatusMap;

enum QuestSlotOffsets
{
    QUEST_ID_OFFSET           = 0,
    QUEST_COUNT_STATE_OFFSET  = 1,                          // including counters 6bits+6bits+6bits+6bits + state 8bits
    QUEST_TIME_OFFSET         = 2
};

#define MAX_QUEST_OFFSET 3

enum QuestSlotStateMask
{
    QUEST_STATE_NONE     = 0x0000,
    QUEST_STATE_COMPLETE = 0x0001,
    QUEST_STATE_FAIL     = 0x0002
};

enum SkillUpdateState
{
    SKILL_UNCHANGED     = 0,
    SKILL_CHANGED       = 1,
    SKILL_NEW           = 2,
    SKILL_DELETED       = 3
};

struct SkillStatusData
{
    SkillStatusData(uint8 _pos, SkillUpdateState _uState) : pos(_pos), uState(_uState) {}
    uint8 pos;
    SkillUpdateState uState;
};

typedef std::unordered_map<uint32, SkillStatusData> SkillStatusMap;

enum PlayerSlots
{
    // first slot for item stored (in any way in player m_items data)
    PLAYER_SLOT_START           = 0,
    // last+1 slot for item stored (in any way in player m_items data)
    PLAYER_SLOT_END             = 118,
    PLAYER_SLOTS_COUNT          = (PLAYER_SLOT_END - PLAYER_SLOT_START)
};

#define INVENTORY_SLOT_BAG_0    255

enum EquipmentSlots                                         // 19 slots
{
    EQUIPMENT_SLOT_START        = 0,
    EQUIPMENT_SLOT_HEAD         = 0,
    EQUIPMENT_SLOT_NECK         = 1,
    EQUIPMENT_SLOT_SHOULDERS    = 2,
    EQUIPMENT_SLOT_BODY         = 3,
    EQUIPMENT_SLOT_CHEST        = 4,
    EQUIPMENT_SLOT_WAIST        = 5,
    EQUIPMENT_SLOT_LEGS         = 6,
    EQUIPMENT_SLOT_FEET         = 7,
    EQUIPMENT_SLOT_WRISTS       = 8,
    EQUIPMENT_SLOT_HANDS        = 9,
    EQUIPMENT_SLOT_FINGER1      = 10,
    EQUIPMENT_SLOT_FINGER2      = 11,
    EQUIPMENT_SLOT_TRINKET1     = 12,
    EQUIPMENT_SLOT_TRINKET2     = 13,
    EQUIPMENT_SLOT_BACK         = 14,
    EQUIPMENT_SLOT_MAINHAND     = 15,
    EQUIPMENT_SLOT_OFFHAND      = 16,
    EQUIPMENT_SLOT_RANGED       = 17,
    EQUIPMENT_SLOT_TABARD       = 18,
    EQUIPMENT_SLOT_END          = 19
};

enum InventorySlots                                         // 4 slots
{
    INVENTORY_SLOT_BAG_START    = 19,
    INVENTORY_SLOT_BAG_END      = 23
};

enum InventoryPackSlots                                     // 16 slots
{
    INVENTORY_SLOT_ITEM_START   = 23,
    INVENTORY_SLOT_ITEM_END     = 39
};

enum BankItemSlots                                          // 28 slots
{
    BANK_SLOT_ITEM_START        = 39,
    BANK_SLOT_ITEM_END          = 63
};

enum BankBagSlots                                           // 6 slots
{
    BANK_SLOT_BAG_START         = 63,
    BANK_SLOT_BAG_END           = 69
};

enum BuyBackSlots                                           // 12 slots after 1.8, only one prior
{
    // stored in m_buybackitems
    BUYBACK_SLOT_START          = 69,
#if SUPPORTED_CLIENT_BUILD > CLIENT_BUILD_1_7_1
    BUYBACK_SLOT_END            = 81
#else
    BUYBACK_SLOT_END            = 70
#endif
};

enum KeyRingSlots                                           // 32 slots
{
    KEYRING_SLOT_START          = 81,
    KEYRING_SLOT_END            = 97
};

#define MAX_KEYRING_SLOTS 32

struct ItemPosCount
{
    ItemPosCount(uint16 _pos, uint8 _count) : pos(_pos), count(_count) {}
    bool isContainedIn(std::vector<ItemPosCount> const& vec) const;
    uint16 pos;
    uint8 count;
};
typedef std::vector<ItemPosCount> ItemPosCountVec;

enum TransferAbortReason
{
    TRANSFER_ABORT_MAX_PLAYERS                  = 0x01,     // Transfer Aborted: instance is full
    TRANSFER_ABORT_NOT_FOUND                    = 0x02,     // Transfer Aborted: instance not found
    TRANSFER_ABORT_TOO_MANY_INSTANCES           = 0x03,     // You have entered too many instances recently.
    TRANSFER_ABORT_SILENTLY                     = 0x04,     // no message shown; the same effect give values above 5
    TRANSFER_ABORT_ZONE_IN_COMBAT               = 0x05,     // Unable to zone in while an encounter is in progress.
};

enum InstanceResetWarningType
{
    RAID_INSTANCE_WARNING_HOURS     = 1,                    // WARNING! %s is scheduled to reset in %d hour(s).
    RAID_INSTANCE_WARNING_MIN       = 2,                    // WARNING! %s is scheduled to reset in %d minute(s)!
    RAID_INSTANCE_WARNING_MIN_SOON  = 3,                    // WARNING! %s is scheduled to reset in %d minute(s). Please exit the zone or you will be returned to your bind location!
    RAID_INSTANCE_WELCOME           = 4                     // Welcome to %s. This raid instance is scheduled to reset in %s.
};

enum RestType
{
    REST_TYPE_NO        = 0,
    REST_TYPE_IN_TAVERN = 1,
    REST_TYPE_IN_CITY   = 2
};

enum DuelCompleteType
{
    DUEL_INTERRUPTED = 0,
    DUEL_WON         = 1,
    DUEL_FLED        = 2
};

// Type of environmental damages
enum EnvironmentalDamageType
{
    DAMAGE_EXHAUSTED = 0,
    DAMAGE_DROWNING  = 1,
    DAMAGE_FALL      = 2,
    DAMAGE_LAVA      = 3,
    DAMAGE_SLIME     = 4,
    DAMAGE_FIRE      = 5,
    DAMAGE_FALL_TO_VOID = 6                                 // custom case for fall without durability loss
};

enum PlayedTimeIndex
{
    PLAYED_TIME_TOTAL = 0,
    PLAYED_TIME_LEVEL = 1
};

#define MAX_PLAYED_TIME_INDEX 2

// used at player loading query list preparing, and later result selection
enum PlayerLoginQueryIndex
{
    PLAYER_LOGIN_QUERY_LOADFROM,
    PLAYER_LOGIN_QUERY_LOADGROUP,
    PLAYER_LOGIN_QUERY_LOADBOUNDINSTANCES,
    PLAYER_LOGIN_QUERY_LOADAURAS,
    PLAYER_LOGIN_QUERY_LOADSPELLS,
    PLAYER_LOGIN_QUERY_LOADQUESTSTATUS,
    PLAYER_LOGIN_QUERY_LOADHONORCP,
    PLAYER_LOGIN_QUERY_LOADREPUTATION,
    PLAYER_LOGIN_QUERY_LOADINVENTORY,
    PLAYER_LOGIN_QUERY_LOADITEMLOOT,
    PLAYER_LOGIN_QUERY_LOADACTIONS,
    PLAYER_LOGIN_QUERY_LOADSOCIALLIST,
    PLAYER_LOGIN_QUERY_LOADHOMEBIND,
    PLAYER_LOGIN_QUERY_LOADSPELLCOOLDOWNS,
    PLAYER_LOGIN_QUERY_LOADGUILD,
    PLAYER_LOGIN_QUERY_LOADBGDATA,
    PLAYER_LOGIN_QUERY_LOADACCOUNTDATA,
    PLAYER_LOGIN_QUERY_LOADSKILLS,
    PLAYER_LOGIN_QUERY_LOADMAILS,
    PLAYER_LOGIN_QUERY_LOADMAILEDITEMS,
    PLAYER_LOGIN_QUERY_BATTLEGROUND_DATA,
    PLAYER_LOGIN_QUERY_FORGOTTEN_SKILLS,

    MAX_PLAYER_LOGIN_QUERY
};

enum PlayerDelayedOperations
{
    DELAYED_SAVE_PLAYER           = 0x01,
    DELAYED_RESURRECT_PLAYER      = 0x02,
    DELAYED_SPELL_CAST_DESERTER   = 0x04,
    DELAYED_CAST_HONORLESS_TARGET = 0x08,
    DELAYED_END
};

enum ReputationSource
{
    REPUTATION_SOURCE_KILL,
    REPUTATION_SOURCE_QUEST,
    REPUTATION_SOURCE_SPELL
};

// Player summoning auto-decline time (in secs)
#define MAX_PLAYER_SUMMON_DELAY (2*MINUTE)
#define MAX_MONEY_AMOUNT (0x7FFFFFFF-1)
#define MAX_TRIAL_MONEY_AMOUNT 100000

struct InstancePlayerBind
{
    DungeonPersistentState* state;
    bool perm;
    /* permanent PlayerInstanceBinds are created in Raid instances for players
       that aren't already permanently bound when they are inside when a boss is killed
       or when they enter an instance that the group leader is permanently bound to. */
    InstancePlayerBind() : state(nullptr), perm(false) {}
};

#define MAX_INSTANCE_PER_ACCOUNT_PER_HOUR 5

enum PlayerRestState
{
    REST_STATE_RESTED           = 0x01,
    REST_STATE_NORMAL           = 0x02,
    REST_STATE_RAF_LINKED       = 0x04                      // Exact use unknown
};

struct ResurrectionData
{
    ObjectGuid resurrectorGuid;
    WorldLocation location;
    uint32 instanceId = 0;
    uint32 health = 0;
    uint32 mana = 0;
};

// Holder for BattleGround data
struct BGData
{
    uint32 bgInstanceID = 0;                                // This variable is set to bg->m_InstanceID, saved
                                                            // when player is teleported to BG - (it is battleground's GUID)
    BattleGroundTypeId bgTypeID = BATTLEGROUND_TYPE_NONE;

    std::set<uint32>   bgAfkReporter;
    uint8              bgAfkReportedCount = 0;
    time_t             bgAfkReportedTimer = 0;

    Team bgTeam = TEAM_NONE;                                // What side the player will be added to, saved

    WorldLocation joinPos;                                  // From where player entered BG, saved

    bool m_needSave = false;                                // true, if saved to DB fields modified after prev. save (marked as "saved" above)
};

struct CinematicWaypointEntry
{
        uint32 cinematic_id = 0;
        uint32 time = 0;
        Position position;
};

#define MAX_RACIAL_SPELLS 10

struct RacialSpells
{
    uint8 raceId = 0;
    uint32 spells[MAX_RACIAL_SPELLS] = { 0 };
};

class MovementAnticheat;

struct ScheduledTeleportData
{
    ScheduledTeleportData() = default;
    ScheduledTeleportData(uint32 mapid, float x, float y, float z, float o,
        uint32 options, std::function<void()> recover_)
        : targetMapId(mapid), x(x), y(y), z(z),
          orientation(o), options(options), recover(std::move(recover_)) {};

    uint32 targetMapId = 0;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float orientation = 0.0f;

    uint32 options = 0;

    std::function<void()> recover = std::function<void()>();
};

class Player final: public Unit
{
    friend class WorldSession;
    friend void Item::AddToUpdateQueueOf(Player* player);
    friend void Item::RemoveFromUpdateQueueOf(Player* player);
    public:
        explicit Player (WorldSession* session);
        ~Player() override;

        void CleanupsBeforeDelete() override;

        // Initializes a new Player object that was not loaded from the database.
        bool Create(uint32 guidlow, std::string const& name, uint8 race, uint8 class_, uint8 gender, uint8 skin, uint8 face, uint8 hairStyle, uint8 hairColor, uint8 facialHair);
        void Update(uint32 update_diff, uint32 time) override;
        static bool BuildEnumData(const std::unique_ptr<QueryResult>& result,  WorldPacket* pData);

        /**
         * @brief Can only be called from Master server (or ASSERT will fail)
         * @return player social structure.
         */
        PlayerSocial* GetSocial() const;
        /**
         * @brief Can only be called from Master server (or returns nullptr)
         * @return player social structure - or nullptr if no MasterPlayer
         */
        PlayerSocial* FindSocial() const;

        // 0 = own auction, -1 = enemy auction, 1 = goblin auction
        int GetAuctionAccessMode() const { return m_ExtraFlags & PLAYER_EXTRA_AUCTION_ENEMY ? -1 : (m_ExtraFlags & PLAYER_EXTRA_AUCTION_NEUTRAL ? 1 : 0); }
        void SetAuctionAccessMode(int state)
        {
            m_ExtraFlags &= ~(PLAYER_EXTRA_AUCTION_ENEMY | PLAYER_EXTRA_AUCTION_NEUTRAL);

            if (state < 0)
                m_ExtraFlags |= PLAYER_EXTRA_AUCTION_ENEMY;
            else if (state > 0)
                m_ExtraFlags |= PLAYER_EXTRA_AUCTION_NEUTRAL;
        }

        /*********************************************************/
        /***                 GAMEMASTER SYSTEM                 ***/
        /*********************************************************/

    private:
        uint32 m_gmInvisibilityLevel;
        uint32 m_currentTicketCounter;
        uint16 m_cheatOptions;
    public:
        bool IsAcceptTickets() const { return GetSession()->GetSecurity() >= SEC_GAMEMASTER && (m_ExtraFlags & PLAYER_EXTRA_GM_ACCEPT_TICKETS); }
        void SetAcceptTicket(bool on) { if (on) m_ExtraFlags |= PLAYER_EXTRA_GM_ACCEPT_TICKETS; else m_ExtraFlags &= ~PLAYER_EXTRA_GM_ACCEPT_TICKETS; }
        bool IsGameMaster() const { return m_ExtraFlags & PLAYER_EXTRA_GM_ON; }
        void SetGameMaster(bool on, bool notify = false);
        bool IsGMChat() const { return GetSession()->GetSecurity() >= SEC_MODERATOR && (m_ExtraFlags & PLAYER_EXTRA_GM_CHAT); }
        void SetGMChat(bool on, bool notify = false);
        bool IsTaxiCheater() const { return m_ExtraFlags & PLAYER_EXTRA_TAXICHEAT; }
        void SetTaxiCheater(bool on) { if (on) m_ExtraFlags |= PLAYER_EXTRA_TAXICHEAT; else m_ExtraFlags &= ~PLAYER_EXTRA_TAXICHEAT; }
        void SetPvPDeath(bool on) { if (on) m_ExtraFlags |= PLAYER_EXTRA_PVP_DEATH; else m_ExtraFlags &= ~PLAYER_EXTRA_PVP_DEATH; }
        bool IsGMVisible() const { return !(m_ExtraFlags & PLAYER_EXTRA_GM_INVISIBLE); }
        void SetGMVisible(bool on, bool notify = false);
        
        void SetCheatFly(bool on, bool notify = false);
        void SetCheatFixedZ(bool on, bool notify = false);
        void SetCheatGod(bool on, bool notify = false);
        void SetCheatNoCooldown(bool on, bool notify = false);
        void SetCheatInstantCast(bool on, bool notify = false);
        void SetCheatNoPowerCost(bool on, bool notify = false);
        void SetCheatDebuffImmunity(bool on, bool notify = false);
        void SetCheatAlwaysCrit(bool on, bool notify = false);
        void SetCheatNoCastCheck(bool on, bool notify = false);
        void SetCheatAlwaysProc(bool on, bool notify = false);
        void SetCheatTriggerPass(bool on, bool notify = false);
        void SetCheatIgnoreTriggers(bool on, bool notify = false);
        void SetCheatDebugTargetInfo(bool on, bool notify = false);
        uint16 GetCheatOptions() const { return m_cheatOptions; }
        bool HasCheatOption(PlayerCheatOptions o) const { return (m_cheatOptions & o); }
        void EnableCheatOption(PlayerCheatOptions o)    { m_cheatOptions |= o; }
        void RemoveCheatOption(PlayerCheatOptions o)    { m_cheatOptions &= (~o); }
        void SetCheatOption(PlayerCheatOptions o, bool on)
        {
            if (on)
                EnableCheatOption(o);
            else
                RemoveCheatOption(o);
        }

        uint32 GetGMInvisibilityLevel() const { return m_gmInvisibilityLevel; }
        void SetGMInvisibilityLevel(uint32 level) { m_gmInvisibilityLevel = level; }
        uint32 GetGMTicketCounter() const { return m_currentTicketCounter; }
        void SetGMTicketCounter(uint32 counter) { m_currentTicketCounter = counter; }

        /*********************************************************/
        /***                    STORAGE SYSTEM                 ***/
        /*********************************************************/

    private:
        ObjectGuid m_lootGuid;
        Item* m_items[PLAYER_SLOTS_COUNT];
        uint32 m_currentBuybackSlot;

        std::vector<Item*> m_itemUpdateQueue;
        bool m_itemUpdateQueueBlocked;
        EnchantDurationList m_enchantDuration;
        ItemDurationList m_itemDuration;

        TradeData* m_trade;
        uint32 m_weaponProficiency;
        uint32 m_armorProficiency;

        // internal common parts for CanStore/StoreItem functions
        InventoryResult _CanStoreItem_InSpecificSlot(uint8 bag, uint8 slot, ItemPosCountVec& dest, ItemPrototype const* pProto, uint32& count, bool swap, Item const* pSrcItem) const;
        InventoryResult _CanStoreItem_InBag(uint8 bag, ItemPosCountVec& dest, ItemPrototype const* pProto, uint32& count, bool merge, bool non_specialized, Item const* pSrcItem, uint8 skip_bag, uint8 skip_slot) const;
        InventoryResult _CanStoreItem_InInventorySlots(uint8 slot_begin, uint8 slot_end, ItemPosCountVec& dest, ItemPrototype const* pProto, uint32& count, bool merge, Item const* pSrcItem, uint8 skip_bag, uint8 skip_slot) const;
        Item* _StoreItem(uint16 pos, Item* pItem, uint32 count, bool clone, bool update);
        InventoryResult CanTakeMoreSimilarItems(Item const* pItem) const { return _CanTakeMoreSimilarItems(pItem->GetEntry(), pItem->GetCount(), pItem); }
        InventoryResult CanTakeMoreSimilarItems(uint32 entry, uint32 count) const { return _CanTakeMoreSimilarItems(entry, count, nullptr); }
        InventoryResult CanEquipUniqueItem(ItemPrototype const* itemProto, uint8 except_slot = NULL_SLOT) const;
        InventoryResult _CanTakeMoreSimilarItems(uint32 entry, uint32 count, Item const* pItem, uint32* no_space_count = nullptr) const;
        InventoryResult _CanStoreItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, uint32 entry, uint32 count, Item const* pItem = nullptr, bool swap = false, uint32* no_space_count = nullptr) const;
        void ApplyEquipCooldown(Item const* pItem);
        bool CheckAmmoCompatibility(ItemPrototype const* ammo_proto) const;
        void QuickEquipItem(uint16 pos, Item* pItem);
        void VisualizeItem(uint8 slot, Item* pItem);
        void SetVisibleItemSlot(uint8 slot, Item const* pItem);
        // in trade, guild bank, mail....
        void RemoveItemDependentAurasAndCasts(Item const* pItem);
        void UpdateEnchantTime(uint32 time);
        void UpdateItemDuration(uint32 time, bool realtimeonly = false);
        void AddEnchantmentDurations(Item* item);
        void RemoveEnchantmentDurations(Item* item);
        void RemoveAllEnchantments(EnchantmentSlot slot);
        void AddEnchantmentDuration(Item* item, EnchantmentSlot slot, uint32 duration);
        void SendEnchantmentDurations() const;
        void BuildEnchantmentLog(WorldPacket& data, ObjectGuid casterGuid, uint32 itemId, uint32 spellId, bool showAffiliation) const;
        void AddItemDurations(Item* item);
        void RemoveItemDurations(Item const* item);
        void SendItemDurations() const;
        uint32 CountFreeInventorySlots() const;
    public:
        Item* AddItem(uint32 itemId, uint32 count = 1);
        void InterruptSpellsWithCastItem(Item const* item);
        uint8 FindEquipSlot(ItemPrototype const* proto, uint32 slot, bool swap) const;
        uint32 GetItemCount(uint32 item, bool inBankAlso = false, Item const* skipItem = nullptr) const;
        Item* GetItemByGuid(ObjectGuid guid) const;

#ifdef ENABLE_ELUNA
        Item* GetItemByEntry(uint32 item) const;            // only for special cases
#endif

        Item* GetItemByPos(uint16 pos) const;
        Item* GetItemByPos(uint8 bag, uint8 slot) const;
        Item* GetWeaponForAttack(WeaponAttackType attackType) const { return GetWeaponForAttack(attackType,false,false); }
        Item* GetWeaponForAttack(WeaponAttackType attackType, bool nonbroken, bool useable) const;
        Item* GetWeaponForParry() const;
        static uint32 GetAttackBySlot(uint8 slot);        // MAX_ATTACK if not weapon slot
        uint32 GetHighestKnownArmorProficiency() const;
        std::vector<Item*>& GetItemUpdateQueue() { return m_itemUpdateQueue; }
        static bool IsMainHandPos(uint16 pos) { return uint8(pos >> 8) == INVENTORY_SLOT_BAG_0 && uint8(pos & 255) == EQUIPMENT_SLOT_MAINHAND; }
        static bool IsInventoryPos(uint16 pos) { return IsInventoryPos(pos >> 8, pos & 255); }
        static bool IsInventoryPos(uint8 bag, uint8 slot);
        static bool IsEquipmentPos(uint16 pos) { return IsEquipmentPos(pos >> 8, pos & 255); }
        static bool IsEquipmentPos(uint8 bag, uint8 slot);
        static bool IsBagPos(uint16 pos);
        static bool IsBankPos(uint16 pos) { return IsBankPos(pos >> 8, pos & 255); }
        static bool IsBankPos(uint8 bag, uint8 slot);
        bool IsValidPos(uint16 pos, bool explicit_pos) const { return IsValidPos(pos >> 8, pos & 255, explicit_pos); }
        bool IsValidPos(uint8 bag, uint8 slot, bool explicit_pos) const;
        uint8 GetBankBagSlotCount() const { return GetByteValue(PLAYER_BYTES_2, PLAYER_BYTES_2_OFFSET_BANK_BAG_SLOTS); }
        void SetBankBagSlotCount(uint8 count) { SetByteValue(PLAYER_BYTES_2, PLAYER_BYTES_2_OFFSET_BANK_BAG_SLOTS, count); }
        bool HasItemCount(uint32 item, uint32 count = 1, bool inBankAlso = false) const;
        bool HasItemFitToSpellReqirements(SpellEntry const* spellInfo, Item const* ignoreItem = nullptr) const;
        bool HasItemWithIdEquipped(uint32 item, uint32 count = 1, uint8 except_slot = NULL_SLOT) const;
        InventoryResult CanStoreNewItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, uint32 item, uint32 count, uint32* no_space_count = nullptr) const
        {
            return _CanStoreItem(bag, slot, dest, item, count, nullptr, false, no_space_count);
        }
        InventoryResult CanStoreItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, Item* pItem, bool swap = false) const
        {
            if (!pItem)
                return EQUIP_ERR_ITEM_NOT_FOUND;
            uint32 count = pItem->GetCount();
            return _CanStoreItem(bag, slot, dest, pItem->GetEntry(), count, pItem, swap, nullptr);
        }
        InventoryResult CanStoreItems(Item** pItem,int count) const;
        InventoryResult CanEquipNewItem(uint8 slot, uint16& dest, uint32 item, bool swap) const;
        InventoryResult CanEquipItem(uint8 slot, uint16& dest, Item const* pItem, bool swap, bool not_loading = true) const;
        InventoryResult CanEquipItem(uint8 slot, uint16& dest, ItemPrototype const* pProto, Item const* pItem = nullptr, bool swap = false, bool not_loading = true) const;
        InventoryResult CanUnequipItems(uint32 item, uint32 count) const;
        InventoryResult CanUnequipItem(uint16 src, bool swap) const;
        InventoryResult CanBankItem(uint8 bag, uint8 slot, ItemPosCountVec& dest, Item const* pItem, bool swap, bool not_loading = true) const;
        InventoryResult CanUseItem(Item const* pItem, bool not_loading = true) const;
        InventoryResult CanUseItem(ItemPrototype const* pItem, bool not_loading = true) const;
        InventoryResult CanUseAmmo(uint32 item) const;
        Item* StoreNewItem(ItemPosCountVec const& pos, uint32 item, bool update, int32 randomPropertyId = 0);
        Item* StoreItem(ItemPosCountVec const& pos, Item* pItem, bool update);
        Item* EquipNewItem(uint16 pos, uint32 item, bool update);
        Item* EquipItem(uint16 pos, Item* pItem, bool update);
        void AutoUnequipWeaponsIfNeed();
        void AutoUnequipOffhandIfNeed();
        void AutoUnequipItemFromSlot(uint32 slot);
        void SatisfyItemRequirements(ItemPrototype const* pItem);
        void AddStartingItems();
        bool StoreNewItemInBestSlots(uint32 itemId, uint32 amount, uint32 enchantId = 0);
        Item* StoreNewItemInInventorySlot(uint32 itemId, uint32 amount);
        void AutoStoreLoot(Loot& loot, bool broadcast = false, uint8 bag = NULL_BAG, uint8 slot = NULL_SLOT);
        void SetAmmo(uint32 item);
        void RemoveAmmo();
        void SendProficiency(ItemClass itemClass, uint32 itemSubclassMask) const;
        Item* BankItem(ItemPosCountVec const& dest, Item* pItem, bool update)
        {
            return StoreItem(dest, pItem, update);
        }
        void RemoveItem(uint8 bag, uint8 slot, bool update);
        void MoveItemFromInventory(uint8 bag, uint8 slot, bool update);
        // in trade, auction, guild bank, mail....
        void MoveItemToInventory(ItemPosCountVec const& dest, Item* pItem, bool update, bool in_characterInventoryDB = false);
        void DestroyItem(uint8 bag, uint8 slot, bool update);
        void DestroyItemCount(uint32 item, uint32 count, bool update, bool unequip_check = false, bool check_bank = false);
        void DestroyItemCount(Item* item, uint32& count, bool update);
        /**
         * @brief Destroys equipped item $itemId and updates the Player
         * @param itemId
         * @return true if an item got deleted
         */
        bool DestroyEquippedItem(uint32 itemId);
        void DestroyConjuredItems(bool update);
        void DestroyZoneLimitedItem(bool update, uint32 new_zone);
        void SplitItem(uint16 src, uint16 dst, uint32 count);
        void SwapItem(uint16 src, uint16 dst);
        void AddItemToBuyBackSlot(Item* pItem, uint32 money, ObjectGuid vendorGuid);
        Item* GetItemFromBuyBackSlot(uint32 slot);
        void RemoveItemFromBuyBackSlot(uint32 slot, bool del);
#if SUPPORTED_CLIENT_BUILD > CLIENT_BUILD_1_7_1
        uint32 GetBuyBackItemPrice(uint32 slot) const { return GetUInt32Value(PLAYER_FIELD_BUYBACK_PRICE_1 + slot - BUYBACK_SLOT_START); }
#else
        uint32 GetBuyBackItemPrice(uint32 /*slot*/) const { return GetUInt32Value(PLAYER_FIELD_BUYBACK_PRICE); }
#endif

#if SUPPORTED_CLIENT_BUILD > CLIENT_BUILD_1_10_2
        uint32 GetMaxKeyringSize() const { return GetLevel() < 40 ? 4 : (GetLevel() < 50 ? 8 : 12); }
#else
        uint32 GetMaxKeyringSize() const { return 0; }
#endif
        void SendEquipError(InventoryResult msg, Item const* pItem, Item const* pItem2 = nullptr, uint32 itemid = 0) const;
        void SendBuyError(BuyResult msg, Creature const* pCreature, uint32 item, uint32 param) const;
        void SendSellError(SellResult msg, Creature const* pCreature, ObjectGuid itemGuid, uint32 param) const;
        void SendOpenContainer() const;
        void AddWeaponProficiency(uint32 newflag) { m_weaponProficiency |= newflag; }
        void AddArmorProficiency(uint32 newflag) { m_armorProficiency |= newflag; }
        uint32 GetWeaponProficiency() const { return m_weaponProficiency; }
        uint32 GetArmorProficiency() const { return m_armorProficiency; }
        bool IsTwoHandUsed() const
        {
            Item* mainItem = GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
            return mainItem && mainItem->GetProto()->InventoryType == INVTYPE_2HWEAPON;
        }
        void SendNewItem(Item const* item, uint32 count, bool received, bool created, bool broadcast = false, bool showInChat = true);
        bool BuyItemFromVendor(ObjectGuid vendorGuid, uint32 item, uint8 count, uint8 bag, uint8 slot);
        void OnReceivedItem(Item const* item);

        float GetReputationPriceDiscount(Creature const* pCreature, bool taxi = false) const;

        Player* GetTrader() const;
        TradeData* GetTradeData() const { return m_trade; }
        void TradeCancel(bool sendback, TradeStatus status = TRADE_STATUS_TRADE_CANCELED);

        uint32 GetMoney() const { return GetUInt32Value(PLAYER_FIELD_COINAGE); }
        void LogModifyMoney(int32 d, char const* type, ObjectGuid fromGuid = ObjectGuid(), uint32 data = 0);
        uint32 GetMaxMoney() const;

#ifdef ENABLE_ELUNA
        void ModifyMoney(int32 d);       
#else
        void ModifyMoney(int32 d)
        {
            if (d < 0)
                SetMoney(GetMoney() > uint32(-d) ? GetMoney() + d : 0);
            else
                SetMoney((uint32)std::min<uint64>(uint64(GetMoney()) + uint64(d), GetMaxMoney()));
        }
#endif
        void LootMoney(int32 g, Loot* loot);
        std::string GetShortDescription() const; // "player:guid [username:accountId@IP]"

        void SetMoney(uint32 value)
        {
            SetUInt32Value(PLAYER_FIELD_COINAGE, value);
            MoneyChanged(value);
        }

        ObjectGuid const& GetLootGuid() const { return m_lootGuid; }
        void SetLootGuid(ObjectGuid const& guid) { m_lootGuid = guid; }
        void RemovedInsignia(Player* looterPlr, Corpse* corpse);
        void SendLoot(ObjectGuid guid, LootType lootType, Player const* pVictim = nullptr);
        void SendLootRelease(ObjectGuid guid) const;
        void SendLootError(ObjectGuid guid, LootError error) const;
        void SendNotifyLootItemRemoved(uint8 lootSlot) const;
        void SendNotifyLootMoneyRemoved() const;
        void SendLootMoneyNotify(uint32 amount) const;
        bool IsAllowedToLoot(Creature const* creature);
        float GetMaxLootDistance(Unit const* pUnit) const;

        void SendEnchantmentLog(ObjectGuid casterGuid, uint32 itemId, uint32 spellId) const;
        void ApplyEnchantment(Item* item,EnchantmentSlot slot,bool apply, bool apply_dur = true, bool ignore_condition = false);
        void ApplyEnchantment(Item* item,bool apply);

        void DurabilityLossAll(double percent, bool inventory);
        void DurabilityLoss(Item* item, double percent);
        void DurabilityPointsLossAll(int32 points, bool inventory);
        void DurabilityPointsLoss(Item* item, int32 points);
        void DurabilityPointLossForEquipSlot(EquipmentSlots slot);
        uint32 DurabilityRepairAll(bool cost, float discountMod);
        uint32 DurabilityRepair(uint16 pos, bool cost, float discountMod);

        /*********************************************************/
        /***                    GOSSIP SYSTEM                  ***/
        /*********************************************************/

    public:
        void PrepareGossipMenu(WorldObject* pSource, uint32 menuId = 0);
        void SendPreparedGossip(WorldObject* pSource);
        void OnGossipSelect(WorldObject* pSource, uint32 gossipListId);

        uint32 GetGossipTextId(uint32 menuId, WorldObject* pSource);
        static uint32 GetGossipTextId(WorldObject const* pSource);
        PlayerMenu* PlayerTalkClass;

        /*********************************************************/
        /***                    QUEST SYSTEM                   ***/
        /*********************************************************/

    private:
        // We allow only one timed quest active at the same time. Below can then be simple value instead of set.
        typedef std::set<uint32> QuestSet;
        QuestSet m_timedquests;

        ObjectGuid m_dividerGuid;
        uint32 m_ingametime;
        QuestStatusMap mQuestStatus;
        void AdjustQuestReqItemCount(Quest const* pQuest, QuestStatusData& questStatusData);
        bool CanGiveQuestSourceItemIfNeed(Quest const* pQuest, ItemPosCountVec* dest = nullptr) const;
        void GiveQuestSourceItemIfNeed(Quest const* pQuest);

        uint16 FindQuestSlot(uint32 quest_id) const;
        uint32 GetQuestSlotQuestId(uint16 slot) const { return GetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot * MAX_QUEST_OFFSET + QUEST_ID_OFFSET); }
        void SetQuestSlot(uint16 slot, uint32 quest_id, uint32 timer = 0)
        {
            SetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot*MAX_QUEST_OFFSET + QUEST_ID_OFFSET, quest_id);
            SetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot*MAX_QUEST_OFFSET + QUEST_COUNT_STATE_OFFSET, 0);
            SetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot*MAX_QUEST_OFFSET + QUEST_TIME_OFFSET, timer);
        }
        void SetQuestSlotCounter(uint16 slot, uint8 counter, uint8 count)
        {
            uint32 val = GetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot*MAX_QUEST_OFFSET + QUEST_COUNT_STATE_OFFSET);
            val &= ~(0x3F << (counter * 6));
            val |= ((uint32)count << (counter * 6));
            SetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot*MAX_QUEST_OFFSET + QUEST_COUNT_STATE_OFFSET, val);
        }
        void SetQuestSlotState(uint16 slot, uint8 state) { SetByteFlag(PLAYER_QUEST_LOG_1_1 + slot*MAX_QUEST_OFFSET + QUEST_COUNT_STATE_OFFSET, 3, state); }
        void RemoveQuestSlotState(uint16 slot, uint8 state) { RemoveByteFlag(PLAYER_QUEST_LOG_1_1 + slot*MAX_QUEST_OFFSET + QUEST_COUNT_STATE_OFFSET, 3, state); }
        void SetQuestSlotTimer(uint16 slot, uint32 timer) { SetUInt32Value(PLAYER_QUEST_LOG_1_1 + slot*MAX_QUEST_OFFSET + QUEST_TIME_OFFSET, timer); }
    public:
        uint32 GetQuestLevelForPlayer(Quest const* pQuest) const { return pQuest && (pQuest->GetQuestLevel() > 0) ? pQuest->GetQuestLevel() : GetLevel(); }
        void PrepareQuestMenu(ObjectGuid guid, uint32 exceptQuestId = 0);
        void SendPreparedQuest(ObjectGuid guid);
        bool IsActiveQuest(uint32 quest_id) const;        // can be taken or taken
        bool IsCurrentQuest(uint32 quest_id, uint8 completedOrNot = 0) const;
        Quest const* GetNextQuest(ObjectGuid guid, Quest const* pQuest);
        bool CanSeeStartQuest(Quest const* pQuest) const;
        bool CanTakeQuest(Quest const* pQuest, bool msg, bool skipStatusCheck = false) const;
        bool CanAddQuest(Quest const* pQuest, bool msg) const;
        bool CanCompleteQuest(uint32 quest_id) const;
        bool CanCompleteRepeatableQuest(Quest const* pQuest) const;
        bool CanRewardQuest(Quest const* pQuest, bool msg) const;
        bool CanRewardQuest(Quest const* pQuest, uint32 reward, bool msg) const;
        void AddQuest(Quest const* pQuest, Object* questGiver);
        void FullQuestComplete(uint32 questId); // Equivalent to .quest complete
        void CompleteQuest(uint32 quest_id);
        void RemoveQuest(uint32 quest_id);
        void RemoveQuestAtSlot(uint32 slot);
        void IncompleteQuest(uint32 quest_id);
        void RewardQuest(Quest const* pQuest, uint32 reward, WorldObject* questGiver, bool announce = true);
        void FailQuest(uint32 quest_id);
        bool SatisfyQuestSkill(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestCondition(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestLevel(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestLog(bool msg) const;
        bool SatisfyQuestPreviousQuest(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestBreadcrumbQuest(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestDependentBreadcrumbQuests(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestClass(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestRace(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestReputation(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestStatus(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestTimed(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestExclusiveGroup(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestNextChain(Quest const* qInfo, bool msg) const;
        bool SatisfyQuestPrevChain(Quest const* qInfo, bool msg) const;
        bool TakeOrReplaceQuestStartItems(uint32 quest_id, bool msg, bool giveQuestStartItem);
        bool GetQuestRewardStatus(uint32 quest_id) const;
        QuestStatusData const* GetQuestStatusData(uint32 quest_id) const;
        QuestStatus GetQuestStatus(uint32 quest_id) const;
        void SetQuestStatus(uint32 quest_id, QuestStatus status);

        void SwapQuestSlot(uint16 slot1,uint16 slot2)
        {
            for (int i = 0; i < MAX_QUEST_OFFSET ; ++i)
            {
                uint32 temp1 = GetUInt32Value(PLAYER_QUEST_LOG_1_1 + MAX_QUEST_OFFSET *slot1 + i);
                uint32 temp2 = GetUInt32Value(PLAYER_QUEST_LOG_1_1 + MAX_QUEST_OFFSET *slot2 + i);

                SetUInt32Value(PLAYER_QUEST_LOG_1_1 + MAX_QUEST_OFFSET *slot1 + i, temp2);
                SetUInt32Value(PLAYER_QUEST_LOG_1_1 + MAX_QUEST_OFFSET *slot2 + i, temp1);
            }
        }

        bool IsAtGroupRewardDistance(WorldObject const* pRewardSource) const;
        void RewardSinglePlayerAtKill(Unit const* pVictim);
        void RewardPlayerAndGroupAtEvent(uint32 creatureId, WorldObject const* pRewardSource);
        void RewardPlayerAndGroupAtCast(WorldObject const* pRewardSource, uint32 spellid = 0);
        void AreaExploredOrEventHappens(uint32 questId);
        void GroupEventHappens(uint32 questId, WorldObject const* pEventObject);
        void GroupEventFailHappens(uint32 questId);
        void ItemAddedQuestCheck(uint32 entry, uint32 count);
        void ItemRemovedQuestCheck(uint32 entry, uint32 count);
        void KilledMonster(CreatureInfo const* cInfo, ObjectGuid guid);
        void KilledMonsterCredit(uint32 entry, ObjectGuid guid = ObjectGuid());
        void CastedCreatureOrGO(uint32 entry, ObjectGuid guid, uint32 spellId, bool originalCaster = true);
        void TalkedToCreature(uint32 entry, ObjectGuid guid);
        void MoneyChanged(uint32 value);
        void ReputationChanged(FactionEntry const* factionEntry);
        bool HasQuestForItem(uint32 itemid) const;
        bool HasQuestForGO(int32 GOId) const;
        void UpdateForQuestWorldObjects();
        bool CanShareQuest(uint32 quest_id) const;
        QuestStatusMap& getQuestStatusMap() { return mQuestStatus; };

        void SendQuestCompleteEvent(uint32 quest_id) const;
        void SendQuestReward(Quest const* pQuest, uint32 XP) const;
        void SendQuestFailed(uint32 quest_id) const;
        void SendQuestFailedAtTaker(uint32 quest_id, uint32 reason = INVALIDREASON_DONT_HAVE_REQ) const;
        void SendQuestTimerFailed(uint32 quest_id) const;
        void SendCanTakeQuestResponse(uint32 msg) const;
        void SendQuestConfirmAccept(Quest const* pQuest, Player const* pReceiver) const;
        void SendPushToPartyResponse(Player const* pPlayer, uint8 msg) const;
        void SendQuestUpdateAddItem(Quest const* pQuest, uint32 item_idx, uint32 current, uint32 count);
        void SendQuestUpdateAddCreatureOrGo(Quest const* pQuest, ObjectGuid guid, uint32 creatureOrGO_idx, uint32 count);

        ObjectGuid GetDividerGuid() const { return m_dividerGuid; }
        void SetDividerGuid(ObjectGuid guid) { m_dividerGuid = guid; }
        void ClearDividerGuid() { m_dividerGuid.Clear(); }

        uint32 GetInGameTime() const { return m_ingametime; }
        void SetInGameTime(uint32 time) { m_ingametime = time; }

        void AddTimedQuest(uint32 quest_id) { m_timedquests.insert(quest_id); }
        void RemoveTimedQuest(uint32 quest_id) { m_timedquests.erase(quest_id); }

        /*********************************************************/
        /***                   LOAD SYSTEM                     ***/
        /*********************************************************/

    private:
        void LoadAura(AuraSaveStruct& saveStruct, uint32 timediff);
        bool SaveAura(SpellAuraHolder const* holder, AuraSaveStruct& saveStruct);
        void _LoadAuras(std::unique_ptr<QueryResult> result, uint32 timediff);
        void _LoadBoundInstances(std::unique_ptr<QueryResult> result);
        bool _LoadInventory(std::unique_ptr<QueryResult> result, uint32 timediff, bool& hasEpicMount);
        void _LoadItemLoot(std::unique_ptr<QueryResult> result);
        void _LoadQuestStatus(std::unique_ptr<QueryResult> result);
        void _LoadGroup(std::unique_ptr<QueryResult> result);
        void _LoadSkills(std::unique_ptr<QueryResult> result);
#if SUPPORTED_CLIENT_BUILD > CLIENT_BUILD_1_10_2
        void _LoadForgottenSkills(std::unique_ptr<QueryResult> result);
#endif
        void LoadSkillsFromFields();
        void _LoadSpells(std::unique_ptr<QueryResult> result);
        bool _LoadHomeBind(std::unique_ptr<QueryResult> result);
        void _LoadBGData(std::unique_ptr<QueryResult> result);
        void _LoadIntoDataField(char const* data, uint32 startOffset, uint32 count);
        void _LoadGuild(std::unique_ptr<QueryResult> result);
        uint32 m_atLoginFlags;
    public:
        bool LoadFromDB(ObjectGuid guid, SqlQueryHolder* holder);
        void SendPacketsAtRelogin();
        static uint32 GetZoneIdFromDB(ObjectGuid guid);
        static uint32 GetLevelFromDB(ObjectGuid guid);
        void Initialize(uint32 guid);
        static bool LoadPositionFromDB(ObjectGuid guid, uint32& mapid, float& x,float& y,float& z,float& o, bool& in_flight);
        void LoadCorpse();
        void LoadPet();
        bool HasAtLoginFlag(AtLoginFlags f) const { return m_atLoginFlags & f; }
        void SetAtLoginFlag(AtLoginFlags f) { m_atLoginFlags |= f; }
        void RemoveAtLoginFlag(AtLoginFlags f, bool in_db_also = false);
        static bool ValidateAppearance(uint8 race, uint8 class_, uint8 gender, uint8 hairID, uint8 hairColor, uint8 faceID, uint8 facialHair, uint8 skinColor, bool create = false);

        /*********************************************************/
        /***                   SAVE SYSTEM                     ***/
        /*********************************************************/
        
    private:
        void _SaveAuras();
        void _SaveInventory();
        void _SaveQuestStatus();
        void _SaveSkills();
        void _SaveSpells();
        void _SaveBGData();
        void _SaveStats();
        uint32 m_nextSave;
        bool m_saveDisabled; // used for temporary bots and faction change
    public:
        // Saves a new character directly in the database, without creating a Player object in memory.
        static bool SaveNewPlayer(WorldSession* session, uint32 guidlow, std::string const& name, uint8 raceId, uint8 classId, uint8 gender, uint8 skin, uint8 face, uint8 hairStyle, uint8 hairColor, uint8 facialHair);
        void SaveToDB(bool online = true, bool force = false);
        void SaveInventoryAndGoldToDB();                    // fast save function for item/money cheating preventing
        void SaveGoldToDB();
        static void SavePositionInDB(ObjectGuid guid, uint32 mapId, float x, float y, float z, float o, uint32 zone);

        static void DeleteFromDB(ObjectGuid playerGuid, uint32 accountId, bool updateRealmChars = true, bool deleteFinally = false);
        static void DeleteOldCharacters();
        static void DeleteOldCharacters(uint32 keepDays);

        uint32 GetSaveTimer() const { return m_nextSave; }
        void   SetSaveTimer(uint32 timer) { m_nextSave = timer; }
        bool   IsSavingDisabled() const { return m_saveDisabled; }

        /*********************************************************/
        /***                    PET SYSTEM                     ***/
        /*********************************************************/

    private:
        ObjectGuid m_miniPetGuid;
        uint32 m_temporaryUnsummonedPetNumber;
        void SendPetSkillWipeConfirm() const;
    public:
        void SendPetTameFailure(PetTameFailureReason reason) const;
        void PetSpellInitialize();
        void CharmSpellInitialize() const;
        void PossessSpellInitialize();
        void RemovePetActionBar() const;

        // Take possession of a new spawned creature
        Creature* SummonPossessedMinion(uint32 creatureId, uint32 spellId, float x, float y, float z, float ang, uint32 duration);
        void UnsummonPossessedMinion();

        uint32 m_stableSlots;
        uint32 m_petEntry;
        uint32 m_petSpell;

        void RemovePet(PetSaveMode mode);
        void RemoveMiniPet();
        Pet* GetMiniPet() const override;
        void AutoReSummonPet();

        // use only in Pet::Unsummon/Spell::DoSummon
        void _SetMiniPet(Pet const* pet) { m_miniPetGuid = pet ? pet->GetObjectGuid() : ObjectGuid(); }

        // Temporarily removed pet cache
        uint32 GetTemporaryUnsummonedPetNumber() const { return m_temporaryUnsummonedPetNumber; }
        void SetTemporaryUnsummonedPetNumber(uint32 petNumber) { m_temporaryUnsummonedPetNumber = petNumber; }
        void UnsummonPetTemporaryIfAny();
        void ResummonPetTemporaryUnSummonedIfAny();
        bool IsPetNeedBeTemporaryUnsummoned() const;
        
        /*********************************************************/
        /***                   SPELL SYSTEM                    ***/
        /*********************************************************/

    private:
        PlayerSpellMap m_spells;
        float m_auraBaseMod[BASEMOD_END][MOD_END];
        SpellModList m_spellMods[MAX_SPELLMOD];
        uint32 m_lastFromClientCastedSpellID;
        std::map<uint32, ItemSetEffect> m_itemSetEffects;
#if SUPPORTED_CLIENT_BUILD < CLIENT_BUILD_1_6_1
        uint32 m_resurrectionSpellId;
#endif
        bool IsNeedCastPassiveLikeSpellAtLearn(SpellEntry const* spellInfo) const;
        void SendInitialSpells() const;
        bool AddSpell(uint32 spellId, bool active, bool learning, bool dependent, bool disabled);
    public:
        bool HasSpell(uint32 spell) const override;
        bool HasActiveSpell(uint32 spell) const;            // show in spellbook
        TrainerSpellState GetTrainerSpellState(TrainerSpell const* pTrainerSpell) const;
        bool IsSpellFitByClassAndRace(uint32 spellId, uint32* pReqlevel = nullptr) const;
        bool IsImmuneToSpellEffect(SpellEntry const* spellInfo, SpellEffectIndex index, bool castOnSelf) const override;
        void SendSpellRemoved(uint32 spellId) const;
        void LearnSpell(uint32 spellId, bool dependent, bool talent = false);
        void RemoveSpell(uint32 spellId, bool disabled = false, bool learn_low_rank = true);
        void ResetSpells();
        void LearnDefaultSpells();
        void LearnQuestRewardedSpells();
        void LearnQuestRewardedSpells(Quest const* quest);
        void LearnSpellHighRank(uint32 spellid);
        uint32 GetSpellRank(SpellEntry const* spellInfo) const final;
        void SendChannelUpdate(uint32 time) const;

        void CastItemCombatSpell(Unit* Target, WeaponAttackType attType);
        void CastItemUseSpell(Item* item, SpellCastTargets const& targets);

        ItemSetEffect* GetItemSetEffect(uint32 setId);
        ItemSetEffect* AddItemSetEffect(uint32 setId);
        void RemoveItemSetEffect(uint32 setId);

        // needed by vanish and improved sap
        void CastHighestStealthRank();

        PlayerSpellMap const& GetSpellMap() const { return m_spells; }
        PlayerSpellMap      & GetSpellMap() { return m_spells; }

        void AddSpellMod(SpellModifier* mod, bool apply);
        void SendSpellMod(SpellModifier const* mod) const;
        bool HasInstantCastingSpellMod(SpellEntry const* spellInfo) const;
        bool IsAffectedBySpellmod(SpellEntry const* spellInfo, SpellModifier const* mod, Spell const* spell = nullptr) const;
        template <class T> T ApplySpellMod(uint32 spellId, SpellModOp op, T &basevalue, Spell* spell = nullptr);
        SpellModifier* GetSpellMod(SpellModOp op, uint32 spellId) const;
        void RemoveSpellMods(Spell* spell);
        void RestoreSpellMods(Spell* spell, uint32 ownerAuraId = 0, Aura* aura = nullptr);
        void RestoreAllSpellMods(uint32 ownerAuraId = 0, Aura* aura = nullptr);
        void DropModCharge(SpellModifier* mod, Spell* spell);

        // cooldown system
        void AddGCD(SpellEntry const& spellEntry, uint32 forcedDuration = 0, bool updateClient = false) final;
        void AddCooldown(SpellEntry const& spellEntry, ItemPrototype const* itemProto = nullptr, bool permanent = false, uint32 forcedDuration = 0) final;
        void RemoveSpellCooldown(SpellEntry const& spellEntry, bool updateClient = true) final;
        void RemoveSpellCategoryCooldown(uint32 category, bool updateClient = true) final;
        void RemoveAllCooldowns(bool sendOnly = false) final;
        void LockOutSpells(SpellSchoolMask schoolMask, uint32 duration) final;
        void RemoveSpellLockout(SpellSchoolMask spellSchoolMask, std::set<uint32>* spellAlreadySent = nullptr);
        void SendClearCooldown(uint32 spellId, Unit const* target) const;
        void SendClearAllCooldowns(Unit const* target) const;
        void SendSpellCooldown(uint32 spellId, uint32 cooldown, ObjectGuid target) const;
        void _LoadSpellCooldowns(std::unique_ptr<QueryResult> result);
        void _SaveSpellCooldowns() const;

        template <typename F>
        void RemoveSomeCooldown(F check)
        {
            auto spellCDItr = m_cooldownMap.begin();
            while (spellCDItr != m_cooldownMap.end())
            {
                SpellEntry const* entry = sSpellMgr.GetSpellEntry(spellCDItr->first);
                if (entry && check(*entry))
                {
                    SendClearCooldown(spellCDItr->first, this);
                    spellCDItr = m_cooldownMap.erase(spellCDItr);
                }
                else
                    ++spellCDItr;
            }
        }

        /*********************************************************/
        /***                   TALENT SYSTEM                   ***/
        /*********************************************************/

    private:
        mutable uint32 m_resetTalentsMultiplier;
        time_t m_resetTalentsTime;
        uint32 m_usedTalentCount;

        void UpdateFreeTalentPoints(bool resetIfNeed = true);

        void UpdateResetTalentsMultiplier() const;
        uint32 CalculateTalentsPoints() const;
        void SendTalentWipeConfirm(ObjectGuid guid) const;
    public:
		uint32 GetResetTalentsCost() const;
        uint32 GetFreeTalentPoints() const { return GetUInt32Value(PLAYER_CHARACTER_POINTS1); }
        void SetFreeTalentPoints(uint32 points) { SetUInt32Value(PLAYER_CHARACTER_POINTS1, points); }
        bool ResetTalents(bool noCost = false);
        void InitTalentForLevel();
        bool LearnTalent(uint32 talentId, uint32 talentRank);

        /*********************************************************/
        /***                    STAT SYSTEM                    ***/
        /*********************************************************/

    private:
        float m_modManaRegen;
        float m_modManaRegenInterrupt;
        float m_carryHealthRegen;
        ObjectGuid m_comboTargetGuid;
        int8 m_comboPoints;
        uint32 m_weaponChangeTimer;
        bool m_canParry;
        bool m_canBlock;
        bool m_canDualWield;
        float m_ammoDPS;
        float m_personalXpRate;
        uint32 m_foodEmoteTimer;

        void RegenerateAll();
        void Regenerate(Powers power);
        void RegenerateHealth();
        void HandleFoodEmotes(uint32 diff);

        static float GetHealthBonusFromStamina(float stamina);
        static float GetManaBonusFromIntellect(float intellect);
        float GetMeleeCritFromAgility() const;
        float GetDodgeFromAgility() const;
        void InitStatBuffMods()
        {
            for (int i = STAT_STRENGTH; i < MAX_STATS; ++i) SetFloatValue(PLAYER_FIELD_POSSTAT0 + i, 0);
            for (int i = STAT_STRENGTH; i < MAX_STATS; ++i) SetFloatValue(PLAYER_FIELD_NEGSTAT0 + i, 0);
        }
        void _ApplyAllStatBonuses();
        void _RemoveAllStatBonuses();
        void _ApplyItemMods(Item* item, uint8 slot, bool apply);
        void _RemoveAllItemMods();
        void _ApplyAllItemMods();
        void _ApplyItemBonuses(ItemPrototype const* proto, uint8 slot, bool apply);
        void _ApplyAmmoBonuses();
    public:
        void SetPersonalXpRate(float rate) { if (rate >= 0) m_personalXpRate = rate; }
        float GetPersonalXpRate() const { return m_personalXpRate; }
        void GiveXP(uint32 xp, Unit const* victim);
        void GiveLevel(uint32 level);
        void InitStatsForLevel(bool reapplyMods = false);

        void RewardRage(uint32 damage, bool attacker);
        uint8 GetComboPoints() const { return m_comboPoints; }
        ObjectGuid const& GetComboTargetGuid() const { return m_comboTargetGuid; }

        void AddComboPoints(Unit* target, int8 count);
        void ClearComboPoints();
        void SetComboPoints();

        bool UpdateStats(Stats stat) override;
        bool UpdateAllStats() override;
        void UpdateResistances(uint32 school) override;
        void UpdateArmor() override;
        void UpdateMaxHealth() override;
        void UpdateMaxPower(Powers power) override;
        void UpdateManaRegen() override;
        void UpdateAttackPowerAndDamage(bool ranged = false) override;
        void UpdateDamagePhysical(WeaponAttackType attType) override;
        void UpdateSpellDamageAndHealingBonus();
        void UpdateDefenseBonusesMod();
        void UpdateBlockPercentage();
        void UpdateCritPercentage(WeaponAttackType attType);
        void UpdateAllCritPercentages();
        void UpdateParryPercentage();
        void UpdateDodgePercentage();
        void CalculateMinMaxDamage(WeaponAttackType attType, bool normalized, float& min_damage, float& max_damage, uint8 index = 0) const;
        float GetWeaponBasedAuraModifier(WeaponAttackType attType, AuraType auraType) const final;

        uint32 GetShieldBlockValue() const override;                 // overwrite Unit version (virtual)
        bool CanParry() const { return m_canParry; }
        void SetCanParry(bool value);
        bool CanBlock() const { return m_canBlock; }
        void SetCanBlock(bool value);
        bool CanDualWield() const { return m_canDualWield; }
        void SetCanDualWield(bool value) { m_canDualWield = value; }

        void ApplyStatBuffMod(Stats stat, float val, bool apply) { ApplyModSignedFloatValue((val > 0 ? PLAYER_FIELD_POSSTAT0 + stat : PLAYER_FIELD_NEGSTAT0 + stat), val, apply); }
        void ApplyStatPercentBuffMod(Stats stat, float val, bool apply)
        {
            ApplyPercentModFloatValue(PLAYER_FIELD_POSSTAT0 + stat, val, apply);
            ApplyPercentModFloatValue(PLAYER_FIELD_NEGSTAT0 + stat, val, apply);
        }
        float GetPosStat(Stats stat) const { return GetFloatValue(PLAYER_FIELD_POSSTAT0 + stat); }
        float GetNegStat(Stats stat) const { return GetFloatValue(PLAYER_FIELD_NEGSTAT0 + stat); }
        float GetResistanceBuffMods(SpellSchools school, bool positive) const { return GetFloatValue(positive ? PLAYER_FIELD_RESISTANCEBUFFMODSPOSITIVE + school : PLAYER_FIELD_RESISTANCEBUFFMODSNEGATIVE + school); }
        void SetResistanceBuffMods(SpellSchools school, bool positive, float val) { SetFloatValue(positive ? PLAYER_FIELD_RESISTANCEBUFFMODSPOSITIVE + school : PLAYER_FIELD_RESISTANCEBUFFMODSNEGATIVE + school, val); }
        void ApplyResistanceBuffModsMod(SpellSchools school, bool positive, float val, bool apply) { ApplyModSignedFloatValue(positive ? PLAYER_FIELD_RESISTANCEBUFFMODSPOSITIVE + school : PLAYER_FIELD_RESISTANCEBUFFMODSNEGATIVE + school, val, apply); }
        void ApplyResistanceBuffModsPercentMod(SpellSchools school, bool positive, float val, bool apply) { ApplyPercentModFloatValue(positive ? PLAYER_FIELD_RESISTANCEBUFFMODSPOSITIVE + school : PLAYER_FIELD_RESISTANCEBUFFMODSNEGATIVE + school, val, apply); }

        float GetAmmoDPS() const { return m_ammoDPS; }
        void SetRegularAttackTime(bool resetTimer = true);
        void SetBaseModValue(BaseModGroup modGroup, BaseModType modType, float value) { m_auraBaseMod[modGroup][modType] = value; }
        void HandleBaseModValue(BaseModGroup modGroup, BaseModType modType, float amount, bool apply);
        float GetBaseModValue(BaseModGroup modGroup, BaseModType modType) const;
        float GetTotalBaseModValue(BaseModGroup modGroup) const;
        float GetTotalPercentageModValue(BaseModGroup modGroup) const { return m_auraBaseMod[modGroup][FLAT_MOD] + m_auraBaseMod[modGroup][PCT_MOD]; }

        void _ApplyWeaponDependentAuraMods(Item const* item, WeaponAttackType attackType, bool apply);
        void _ApplyWeaponDependentAuraCritMod(Item const* item, WeaponAttackType attackType, Aura* aura, bool apply);
        void _ApplyWeaponDependentAuraDamageMod(Item const* item, WeaponAttackType attackType, Aura* aura, bool apply);

        void InitDataForForm(bool reapplyMods = false);
        void ApplyItemEquipSpell(Item* item, bool apply, bool formChange = false);
        void ApplyEquipSpell(SpellEntry const* spellInfo, Item* item, bool apply, bool formChange = false);
        void UpdateEquipSpellsAtFormChange();
        void outDebugStatsValues() const;

#ifdef ENABLE_ELUNA
		float GetHealthBonusFromStamina() const { return GetHealthBonusFromStamina(GetStat(STAT_STAMINA)); };
		float GetManaBonusFromIntellect() const { return GetManaBonusFromIntellect(GetStat(STAT_INTELLECT)); };
#endif

        /*********************************************************/
        /***                   SKILLS SYSTEM                   ***/
        /*********************************************************/

    private:
        void InitPrimaryProfessions();
        void UpdateSkillTrainedSpells(uint16 id, uint16 currVal);                                   // learns/unlearns spells dependent on a skill
        void UpdateSpellTrainedSkills(uint32 spellId, bool apply);                                  // learns/unlearns skills dependent on a spell
        void UpdateOldRidingSkillToNew(bool hasEpicMount);
        void UpdateSkillsForLevel();
        SkillStatusMap m_skillStatusMap;
#if SUPPORTED_CLIENT_BUILD > CLIENT_BUILD_1_10_2
        std::unordered_map<uint16, uint16> m_forgottenSkills;
#endif
    public:
        uint32 GetFreePrimaryProfessionPoints() const { return GetUInt32Value(PLAYER_CHARACTER_POINTS2); }
        void SetFreePrimaryProfessions(uint16 profs) { SetUInt32Value(PLAYER_CHARACTER_POINTS2, profs); }

        bool UpdateSkill(uint32 skill_id, uint32 step);
        bool UpdateSkillPro(uint16 SkillId, int32 Chance, uint32 step);

        bool UpdateCraftSkill(uint32 spellid);
        bool UpdateGatherSkill(uint32 SkillId, uint32 SkillValue, uint32 RedLevel, uint32 Multiplicator = 1);
        bool UpdateFishingSkill();

        uint32 GetBaseDefenseSkillValue() const { return GetSkillValuePure(SKILL_DEFENSE); }
        uint32 GetBaseWeaponSkillValue(WeaponAttackType attType) const;

        void UpdateCombatSkills(Unit const* pVictim, WeaponAttackType attType, bool defence);

        void SetSkill(uint16 id, uint16 currVal, uint16 maxVal, uint16 step = 0);
        uint16 GetSkill(uint16 id, bool bonusPerm, bool bonusTemp, bool max = false) const;
        inline uint16 GetSkillValue(uint16 id) const { return GetSkill(id, true, true); }           // skill value + perm. bonus + temp bonus
        inline uint16 GetSkillValueBase(uint16 id) const { return GetSkill(id, true, false); }      // skill value + perm. bonus
        inline uint16 GetSkillValuePure(uint16 id) const { return GetSkill(id, false, false); }     // skill value
        inline uint16 GetSkillMax(uint16 id) const { return GetSkill(id, true, true, true); }       // skill max + perm. bonus + temp bonus
        inline uint16 GetSkillMaxPure(uint16 id) const { return GetSkill(id, false, false, true); } // skill max
        bool ModifySkillBonus(uint16 id, int16 diff, bool permanent = false);
        int16 GetSkillBonus(uint16 id, bool permanent = false) const;
        inline int16 GetSkillBonusPermanent(uint16 id) const { return GetSkillBonus(id, true); }    // skill perm. bonus
        inline int16 GetSkillBonusTemporary(uint16 id) const { return GetSkillBonus(id); }          // skill temp bonus
        bool HasSkill(uint16 id) const;

        void UpdateSkillsToMaxSkillsForLevel();             // for .levelup

        /*********************************************************/
        /***                  LOCATION SYSTEM                  ***/
        /*********************************************************/

    private:
        uint32 m_zoneUpdateId;
        uint32 m_zoneUpdateTimer;
        uint32 m_areaUpdateId;

        // Current teleport data
        WorldLocation m_teleportDest;
        uint32 m_teleportOptions;
        std::function<void()> m_teleportRecover;
        std::function<void()> m_teleportRecoverDelayed;
        bool m_semaphoreTeleportNear;
        bool m_semaphoreTeleportFar;
        bool m_pendingFarTeleport;

        uint32 m_delayedOperations;
        bool m_bCanDelayTeleport;
        bool m_bHasDelayedTeleport;
        bool m_bHasBeenAliveAtDelayedTeleport;
        uint32 m_areaCheckTimer; // Trigger call to UpdateTerainEnvironmentFlags/CheckAreaExploreAndOutdoor

        // to fix an 1.12 client problem with transports
        // sometimes they need a refresh before being usable
        bool m_justBoarded;
        void SetJustBoarded(bool hasBoarded) { m_justBoarded = hasBoarded; }
        bool HasJustBoarded() const { return m_justBoarded; }

        void SetCanDelayTeleport(bool setting) { m_bCanDelayTeleport = setting; }
        bool IsHasDelayedTeleport() const
        {
            // we should not execute delayed teleports for now dead players but has been alive at teleport
            // because we don't want player's ghost teleported from graveyard
            return m_bHasDelayedTeleport && (IsAlive() || !m_bHasBeenAliveAtDelayedTeleport);
        }

        bool SetDelayedTeleportFlagIfCan()
        {
            m_bHasDelayedTeleport = m_bCanDelayTeleport;
            m_bHasBeenAliveAtDelayedTeleport = IsAlive();
            return m_bHasDelayedTeleport;
        }

        void ScheduleDelayedOperation(uint32 operation)
        {
            if (operation < DELAYED_END)
                m_delayedOperations |= operation;
        }

        Unit* m_mover;
        GridReference<Player> m_gridRef;
        MapReference m_mapRef;

        // highest position we started falling from
        float m_fallStartZ;

        // Recall position
        uint32 m_recallMap;
        float  m_recallX;
        float  m_recallY;
        float  m_recallZ;
        float  m_recallO;

        time_t m_summon_expire;
        uint32 m_summon_mapid;
        float  m_summon_x;
        float  m_summon_y;
        float  m_summon_z;

        Camera m_camera;
        ObjectGuid m_pendingCameraUpdate;
        uint32 m_cameraUpdateTimer;
        float m_longSightRange;
        uint32 m_longSightSpell;

        // Homebind coordinates
        WorldLocation m_homebind;
        uint16 m_homebindAreaId;

        // knockback/jumping states
        bool m_launched;

        LiquidTypeEntry const* m_lastLiquid;
        uint8 m_isunderwater;

        void UpdateZoneDependentAuras();
        void UpdateAreaDependentAuras();                    // subzones
        void UpdateTerainEnvironmentFlags();
        void CheckAreaExploreAndOutdoor();
    public:
        void AddToWorld() override;
        void RemoveFromWorld() override;

        /* Switch from instanceId of same map.
        * Assumes that you can enter the map.
        * Should be called in a thread-safe environnement (not in map update for example !)
        */
        bool SwitchInstance(uint32 newInstanceId);
        bool TeleportTo(uint32 mapId, float x, float y, float z, float orientation, uint32 options = 0, std::function<void()> recover = std::function<void()>());
        template <class T>
        bool TeleportTo(T const& loc, uint32 options = 0, std::function<void()> recover = std::function<void()>())
        {
            return TeleportTo(loc.mapId, loc.x, loc.y, loc.z, loc.o, options, recover);
        }

        // _NOT_ thread-safe. Must be executed by the map manager after map updates, since we
        // remove objects from the map
        bool ExecuteTeleportFar(ScheduledTeleportData* data);
        void SendNewWorld();
        void HandleReturnOnTeleportFail(WorldLocation const& oldLoc);

        bool TeleportToBGEntryPoint();
        void RestorePendingTeleport();

        void UpdateZone(uint32 newZone,uint32 newArea);
        void UpdateArea(uint32 newArea);
        uint32 GetCachedZoneId() const { return m_zoneUpdateId; }
        uint32 GetCachedAreaId() const { return m_areaUpdateId; }

        GridReference<Player>& GetGridRef() { return m_gridRef; }
        MapReference &GetMapRef() { return m_mapRef; }

        bool SetPosition(float x, float y, float z, float orientation, bool teleport = false);
        void SetBindPoint(ObjectGuid guid) const;

        WorldLocation& GetTeleportDest() { return m_teleportDest; }
        bool IsBeingTeleported() const { return m_semaphoreTeleportNear || m_semaphoreTeleportFar || m_pendingFarTeleport; }
        bool IsBeingTeleportedNear() const { return m_semaphoreTeleportNear; }
        bool IsBeingTeleportedFar() const { return m_semaphoreTeleportFar; }
        void SetSemaphoreTeleportNear(bool semphsetting);
        void SetSemaphoreTeleportFar(bool semphsetting);
        void SetPendingFarTeleport(bool pending) { m_pendingFarTeleport = pending; }
        void ExecuteTeleportNear();
        void ProcessDelayedOperations();

        bool HasMovementFlag(MovementFlags f) const;        // for script access to m_movementInfo.HasMovementFlag
        void UpdateFallInformationIfNeed(MovementInfo const& minfo, uint16 opcode);
        void SetFallInformation(float fallStartZ)
        {
            m_fallStartZ = fallStartZ;

        }
        void HandleFall(MovementInfo const& movementInfo);
        bool IsFalling() const { return m_fallStartZ != 0; }

        bool IsControlledByOwnClient() const { return m_session->GetClientMoverGuid() == GetObjectGuid(); }
        void SetClientControl(Unit const* target, uint8 allowMove) const;
        void SetMover(Unit* target) { m_mover = target ? target : this; }
        Unit* GetMover() const { return m_mover; } // can never be null
        Unit* GetConfirmedMover() const; // only returns mover confirmed by client, can be null
        bool IsSelfMover() const { return m_mover == this; } // normal case for player not controlling other unit
        bool IsOutdoorOnTransport() const;

        ObjectGuid const& GetFarSightGuid() const { return GetGuidValue(PLAYER_FARSIGHT); }

        void SaveRecallPosition();
        void GetRecallPosition(uint32& map, float& x, float& y, float& z, float& o) const
        {
            map = m_recallMap;
            x = m_recallX;
            y = m_recallY;
            z = m_recallZ;
            o = m_recallO;
        };

        void SetHomebindToLocation(WorldLocation const& loc, uint32 areaId);
        void RelocateToHomebind() { SetLocationMapId(m_homebind.mapId); Relocate(m_homebind.x, m_homebind.y, m_homebind.z); }
        bool TeleportToHomebind(uint32 options = 0, bool hearthCooldown = true);

        // currently visible objects at player client
        ObjectGuidSet m_visibleGUIDs;
        mutable std::shared_timed_mutex m_visibleGUIDs_lock;
        std::map<ObjectGuid, bool> m_visibleGobjQuestActivated;
        mutable std::mutex m_visibleGobjsQuestAct_lock;

        bool IsInVisibleList(WorldObject const* u) const;
        bool IsInVisibleList_Unsafe(WorldObject const* u) const { return this == u || m_visibleGUIDs.find(u->GetObjectGuid()) != m_visibleGUIDs.end(); }
        bool IsVisibleInGridForPlayer(Player const* pl) const override;
        bool IsVisibleGloballyFor(Player const* viewer) const;
        void UpdateVisibilityOf(WorldObject const* viewPoint, WorldObject* target);
        template<class T>
        void UpdateVisibilityOf(WorldObject const* viewPoint, T* target, UpdateData& data, std::set<WorldObject*>& visibleNow);
        void LeaveCombatWithFarAwayCreatures();

        Camera& GetCamera() { return m_camera; }
        void ScheduleCameraUpdate(ObjectGuid guid);

        uint32 GetLongSight() const { return m_longSightSpell; }
        void SetLongSight(Aura const* aura = nullptr);
        void UpdateLongSight() const;

        bool CanWalk() const override { return true; }
        bool CanSwim() const override { return true; }
        bool CanFly() const override { return IsFlying(); }

        void SetFly(bool enable) override;

        // Anti undermap
        void SaveNoUndermapPosition(float x, float y, float z, float o)
        {
            m_lastSafePosition.x = x;
            m_lastSafePosition.y = y;
            m_lastSafePosition.z = z + 2.0f;
            m_lastSafePosition.o = 0;
            m_undermapPosValid = true;
        }
        bool UndermapRecall()
        {
            if (!m_undermapPosValid || IsBeingTeleported())
                return false;
            if (GetDistance2d(m_lastSafePosition) > 100.0f)
            {
                m_undermapPosValid = false;
                return false;
            }
            NearTeleportTo(m_lastSafePosition, TELE_TO_NOT_LEAVE_COMBAT | TELE_TO_NOT_UNSUMMON_PET);
            m_undermapPosValid = false;
            return true;
        }
        Position m_lastSafePosition;
        bool  m_undermapPosValid;

        uint32 GetHomeBindMap() const { return m_homebind.mapId; }
        uint16 GetHomeBindAreaId() const { return m_homebindAreaId; }

        void SendSummonRequest(ObjectGuid summonerGuid, uint32 mapId, uint32 zoneId, float x, float y, float z);
        void SetSummonPoint(uint32 mapid, float x, float y, float z)
        {
            m_summon_expire = time(nullptr) + MAX_PLAYER_SUMMON_DELAY;
            m_summon_mapid = mapid;
            m_summon_x = x;
            m_summon_y = y;
            m_summon_z = z;
        }
        void SummonIfPossible(bool agree);
        void SetTransport(GenericTransport* t) override;
        void DismountCheck();

        // knockback/jumping states
        bool IsLaunched() const { return m_launched; }
        void SetLaunched(bool apply) { m_launched = apply; }

        void SendRaidGroupOnlyError(uint32 timer, RaidGroupError error) const;
        void SendInitialPacketsBeforeAddToMap();
        void SendInitialPacketsAfterAddToMap(bool login = true);

        /*********************************************************/
        /***              ENVIRONMENTAL SYSTEM                 ***/
        /*********************************************************/

    protected: 
        uint8 m_environmentFlags = ENVIRONMENT_FLAG_NONE;
        float m_environmentBreathingMultiplier = 1.0f;
        MirrorTimer m_mirrorTimers[MirrorTimer::NUM_TIMERS] = { MirrorTimer::FATIGUE, MirrorTimer::BREATH, MirrorTimer::FEIGNDEATH, MirrorTimer::ENVIRONMENTAL };

        void SetEnvironmentFlags(EnvironmentFlags flags, bool apply);

        void SendMirrorTimerStart(uint32 type, uint32 remaining, uint32 duration, int32 scale, bool paused = false, uint32 spellId = 0);
        void SendMirrorTimerStop(uint32 type);
        void SendMirrorTimerPause(uint32 type, bool state);

        void FreezeMirrorTimers(bool state);
        void UpdateMirrorTimers(uint32 diff, bool send = true);

        bool CheckMirrorTimerActivation(MirrorTimer::Type timer) const;
        bool CheckMirrorTimerDeactivation(MirrorTimer::Type timer) const;

        void OnMirrorTimerExpirationPulse(MirrorTimer::Type timer);

        uint32 GetMirrorTimerMaxDuration(MirrorTimer::Type timer) const;
        SpellAuraHolder const* GetMirrorTimerBuff(MirrorTimer::Type timer) const;
    public:
        bool IsUnderwater() const override { return (m_environmentFlags & ENVIRONMENT_FLAG_UNDERWATER); }
        bool IsInWater() const override { return (m_environmentFlags & ENVIRONMENT_FLAG_IN_WATER); }
        inline bool IsInMagma() const { return (m_environmentFlags & ENVIRONMENT_FLAG_IN_MAGMA); }
        inline bool IsInSlime() const { return (m_environmentFlags & ENVIRONMENT_FLAG_IN_SLIME); }
        inline bool IsInHighSea() const { return (m_environmentFlags & ENVIRONMENT_FLAG_HIGH_SEA); }
        inline bool IsInHighLiquid() const { return (m_environmentFlags & ENVIRONMENT_FLAG_HIGH_LIQUID); }

        inline uint32 GetWaterBreathingInterval() const;
        void SetWaterBreathingIntervalMultiplier(float multiplier);

        void SendMirrorTimers(bool forced = false);

        uint32 EnvironmentalDamage(EnvironmentalDamageType type, uint32 damage);

        /*********************************************************/
        /***                    REST SYSTEM                    ***/
        /*********************************************************/

    private:
        uint32 m_restTime;
        time_t m_timeInnEnter;
        uint32 m_innTriggerId;
        float m_restBonus;
        RestType m_restType;
        void UpdateInnerTime(time_t time) { m_timeInnEnter = time; }
    public:
        /**
        * \brief: compute rest bonus
        * \param: time_t timePassed > time from last check
        * \param: bool offline      > is the player was offline?
        * \param: bool inRestPlace  > if it was offline, is the player was in city/tavern/inn?
        * \returns: float
        **/
        float ComputeRest(time_t timePassed, bool offline = false, bool inRestPlace = false);

        float GetRestBonus() const { return m_restBonus; }
        void SetRestBonus(float rest_bonus_new);
        RestType GetRestType() const { return m_restType; }
        void SetRestType(RestType restType, uint32 areaTriggerId = 0);
        time_t GetTimeInnEnter() const { return m_timeInnEnter; }
        bool IsRested() const { return GetRestTime() >= 10 * IN_MILLISECONDS; }
        uint32 GetXPRestBonus(uint32 xp);
        uint32 GetRestTime() const { return m_restTime; }
        void SetRestTime(uint32 v) { m_restTime = v; }

        /*********************************************************/
        /***                    TAXI SYSTEM                    ***/
        /*********************************************************/
        
    private:
        PlayerTaxi m_taxi;
    public:
        PlayerTaxi& GetTaxi() { return m_taxi; }
        PlayerTaxi const& GetTaxi() const { return m_taxi; }
        void InitTaxiNodes() { m_taxi.InitTaxiNodes(GetRace(), GetLevel()); }
        bool ActivateTaxiPathTo(std::vector<uint32> const& nodes, Creature const* npc = nullptr, uint32 spellid = 0, bool nocheck = false);
        bool ActivateTaxiPathTo(uint32 taxi_path_id, uint32 spellid = 0, bool nocheck = false);
        void TaxiStepFinished(bool lastPointReached);
        void ContinueTaxiFlight() const;

        /*********************************************************/
        /***                 CINEMATIC SYSTEM                  ***/
        /*********************************************************/

    private:
        uint32 m_currentCinematicEntry;
        Position m_cinematicStartPos;
        uint32 m_cinematicLastCheck;
        uint32 m_cinematicElapsedTime;
        void UpdateCinematic(uint32 diff);
    public:
        void SendCinematicStart(uint32 CinematicSequenceId);
        uint32 GetCurrentCinematicEntry() const { return m_currentCinematicEntry; }

        void CinematicEnd();
        void CinematicStart(uint32 id);

        /*********************************************************/
        /***                   COMBAT SYSTEM                   ***/
        /*********************************************************/

    private:
        AutoAttackCheckResult m_swingErrorMsg;
        int32 m_cannotBeDetectedTimer;

        // Stealth detection system
        void HandleStealthedUnitsDetection();
    public:
        AutoAttackCheckResult GetLastSwingErrorMsg() const { return m_swingErrorMsg; }
        void SetSwingErrorMsg(AutoAttackCheckResult val) { m_swingErrorMsg = val; }
        void SendAttackSwingCantAttack() const;
        void SendAttackSwingCancelAttack() const;
        void SendAttackSwingDeadTarget() const;
        void SendAttackSwingNotStanding() const;
        void SendAttackSwingNotInRange() const;
        void SendAttackSwingBadFacingAttack() const;
        void SendAutoRepeatCancel() const;
        void SendFeignDeathResisted() const;
        void SendExplorationExperience(uint32 Area, uint32 Experience) const;
        void SendFactionAtWar(uint32 reputationId, bool apply) const;
        AutoAttackCheckResult CanAutoAttackTarget(Unit const*) const override;

        // Cannot be detected by creature (Should be tested in AI::MoveInLineOfSight)
        void SetCannotBeDetectedTimer(uint32 milliseconds) { m_cannotBeDetectedTimer = milliseconds; };
        bool CanBeDetected() const override { return m_cannotBeDetectedTimer <= 0; }

        // PlayerAI management
        PlayerAI* m_AI;
        PlayerAI* AI() { return m_AI; }
        void SetAI(PlayerAI* otherAI) { m_AI = otherAI; }
        void SetControlledBy(Unit* pWho);
        void RemoveAI();
        void RemoveTemporaryAI(); // will restore player bot AI if needed
        void ModPossessPet(Pet* pPet, bool apply, AuraRemoveMode m_removeMode = AURA_REMOVE_BY_DEFAULT);

        void SetDeathState(DeathState s) override;                   // overwrite Unit::SetDeathState

        /*********************************************************/
        /***                  SESSION SYSTEM                   ***/
        /*********************************************************/

    private:
        WorldSession* m_session;
        uint32 m_skippedUpdateTime;
        time_t m_createTime;
        time_t m_loginTime;
        time_t m_lastTick;
        uint32 m_playedTime[MAX_PLAYED_TIME_INDEX];
    public:
        WorldSession* GetSession() const { return m_session; }
        void SetSession(WorldSession* s);
        bool IsBot() const { return m_session->GetBot() != nullptr; }

        void BuildCreateUpdateBlockForPlayer(UpdateData& data, Player* target) const override;
        void DestroyForPlayer(Player const* target) const override;
        void SendLogXPGain(uint32 givenXP, Unit const* victim, uint32 restXP) const;

        void SendMessageToSet(WorldPacket* data, bool self) const override;
        void SendMessageToSetInRange(WorldPacket* data, float fist, bool self) const override;
        void SendMessageToSetInRange(WorldPacket* data, float dist, bool self, bool own_team_only) const;
        void SendInitWorldStates(uint32 zone) const;
        void SendUpdateWorldState(uint32 state, uint32 value) const;
        void SendDirectMessage(WorldPacket* data) const;

        uint32 GetTotalPlayedTime() const { return m_playedTime[PLAYED_TIME_TOTAL]; }
        uint32 GetLevelPlayedTime() const { return m_playedTime[PLAYED_TIME_LEVEL]; }

        void AddSkippedUpdateTime(uint32 t) { m_skippedUpdateTime += t; }
        uint32 GetSkippedUpdateTime() const { return m_skippedUpdateTime; }
        void ResetSkippedUpdateTime() { m_skippedUpdateTime = 0; }

        /*********************************************************/
        /***                 MISC GAME SYSTEMS                 ***/
        /*********************************************************/

    private:
        bool m_isStandUpScheduled;
        uint32 m_detectInvisibilityTimer;
        uint32 m_ExtraFlags;
        ObjectGuid m_curSelectionGuid;
        ResurrectionData m_resurrectData;
        uint32 m_drunkTimer;
        uint16 m_drunk;
        void HandleSobering();
        uint32 m_deathTimer;
        time_t m_deathExpireTime;
        bool m_repopAtGraveyardPending;
        ObjectGuid m_selectedGobj; // For GM commands
        ObjectGuid m_escortingGuid;
        ObjectGuid m_currentBankerGuid;

        void SendMountResult(UnitMountResult result) const;
        void SendDismountResult(UnitDismountResult result) const;
        void UpdateCorpseReclaimDelay();
    public:
        void ScheduleStandUp();
        bool IsStandUpScheduled() const { return m_isStandUpScheduled; }
        void ClearScheduledStandUp() { m_isStandUpScheduled = false; }
        UnitMountResult Mount(uint32 mount, uint32 spellId = 0) override;
        UnitDismountResult Unmount(bool from_aura = false) override;

        bool CanUseBank(ObjectGuid bankerGUID = ObjectGuid()) const;
        bool CanInteractWithQuestGiver(Object const* questGiver) const;
        Creature* FindNearestInteractableNpcWithFlag(uint32 npcFlags) const;
        Creature* GetNPCIfCanInteractWith(ObjectGuid guid, uint32 npcFlagMask) const;
        bool CanInteractWithNPC(Creature const* pCreature, uint32 npcFlagMask) const;
        GameObject* GetGameObjectIfCanInteractWith(ObjectGuid guid, uint32 goType = MAX_GAMEOBJECT_TYPE) const;
        bool CanInteractWithGameObject(GameObject const* pGo, uint32 goType = MAX_GAMEOBJECT_TYPE) const;
        bool CanSeeHealthOf(Unit const* pTarget) const;
        bool CanSeeSpecialInfoOf(Unit const* pTarget) const;

        ObjectGuid const& GetSelectedGobj() const { return m_selectedGobj; }
        void SetSelectedGobj(ObjectGuid guid) { m_selectedGobj = guid; }
        ObjectGuid const& GetSelectionGuid() const { return m_curSelectionGuid; }
        void SetSelectionGuid(ObjectGuid guid) { m_curSelectionGuid = guid; SetTargetGuid(guid); }
        Unit* GetSelectedUnit() const;
        Creature* GetSelectedCreature() const;
        Player* GetSelectedPlayer() const;
        Object* GetObjectByTypeMask(ObjectGuid guid, TypeMask typeMask);

        void SetResurrectRequestData(ObjectGuid guid, uint16 mapId, uint32 instanceId, float x, float y, float z, float o, uint32 health, uint32 mana)
        {
            m_resurrectData.resurrectorGuid = guid;
            m_resurrectData.location.mapId = mapId;
            m_resurrectData.location.x = x;
            m_resurrectData.location.y = y;
            m_resurrectData.location.z = z;
            m_resurrectData.location.o = o;
            m_resurrectData.instanceId = instanceId;
            m_resurrectData.health = health;
            m_resurrectData.mana = mana;
        }
        void ClearResurrectRequestData() { SetResurrectRequestData(ObjectGuid(), 0, 0, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0); }
        bool IsRessurectRequestedBy(ObjectGuid guid) const { return m_resurrectData.resurrectorGuid == guid; }
        bool IsRessurectRequested() const { return !m_resurrectData.resurrectorGuid.IsEmpty(); }
        ObjectGuid const& GetResurrector() const { return m_resurrectData.resurrectorGuid; }
        void ResurectUsingRequestData();

        static bool IsActionButtonDataValid(uint8 button, uint32 action, uint8 type, Player const* player);
        void RefreshBitsForVisibleUnits(UpdateMask* mask, uint32 objectTypeMask = TYPEMASK_UNIT);

        Corpse* GetCorpse() const;
        void SpawnCorpseBones();
        Corpse* CreateCorpse();
        void KillPlayer();
#if SUPPORTED_CLIENT_BUILD < CLIENT_BUILD_1_6_1
        uint32 GetResurrectionSpellId() const { return m_resurrectionSpellId; }
        void SetResurrectionSpellId(uint32 resurrectionSpellId) { m_resurrectionSpellId = resurrectionSpellId; }
#endif
        uint32 SelectResurrectionSpellId() const;
        void ResurrectPlayer(float restore_percent, bool applySickness = false);
        void BuildPlayerRepop();
        void RepopAtGraveyard();
        void ScheduleRepopAtGraveyard();

        // Nostalrius : Phasing
        void SetWorldMask(uint32 newMask) override;

        void RemoveDelayedOperation(uint32 operation)
        {
            m_delayedOperations &= ~operation;
        }
        inline bool HasScheduledEvent() const { return m_Events.HasScheduledEvent(); }

        void SetEscortingGuid(ObjectGuid const& guid) { m_escortingGuid = guid; }
        ObjectGuid const& GetEscortingGuid() const { return m_escortingGuid; }

        void SetDrunkValue(uint16 newDrunkValue, uint32 itemid = 0);
        uint16 GetDrunkValue() const { return m_drunk; }
        static DrunkenState GetDrunkenstateByValue(uint16 value);

        uint32 GetDeathTimer() const { return m_deathTimer; }
        uint32 GetCorpseReclaimDelay(bool pvp) const;
        void SendCorpseReclaimDelay(bool load = false) const;

        /*********************************************************/
        /***                    CHAT SYSTEM                    ***/
        /*********************************************************/

    private:
        typedef std::list<Channel*> JoinedChannelsList;
        JoinedChannelsList m_channels;
        void UpdateLocalChannels(uint32 newZone);
        std::string m_name;
        uint64 m_knownLanguagesMask;
    public:
        void JoinedChannel(Channel* c);
        void LeftChannel(Channel* c);
        void CleanupChannels();
        void LeaveLFGChannel();

        bool IsAllowedWhisperFrom(ObjectGuid guid) const;
        bool IsEnabledWhisperRestriction() const { return m_ExtraFlags & PLAYER_EXTRA_WHISP_RESTRICTION; }
        void SetWhisperRestriction(bool on) { if (on) m_ExtraFlags |= PLAYER_EXTRA_WHISP_RESTRICTION; else m_ExtraFlags &= ~PLAYER_EXTRA_WHISP_RESTRICTION; }

        bool IsAcceptWhispers() const { return m_ExtraFlags & PLAYER_EXTRA_ACCEPT_WHISPERS; }
        void SetAcceptWhispers(bool on) { if (on) m_ExtraFlags |= PLAYER_EXTRA_ACCEPT_WHISPERS; else m_ExtraFlags &= ~PLAYER_EXTRA_ACCEPT_WHISPERS; }
        uint32 GetExtraFlags() const { return m_ExtraFlags; }

        bool ToggleAFK();
        bool ToggleDND();
        bool IsAFK() const { return HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_AFK); }
        bool IsDND() const { return HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_DND); }
        uint8 GetChatTag() const;

        char const* GetName() const final { return m_name.c_str(); }
        void SetName(std::string const& newname) { m_name = newname; }

        float GetYellRange() const;
        void Say(char const* text, uint32 const language) const;
        void Yell(char const* text, uint32 const language) const;
        void TextEmote(char const* text) const;

        void LearnLanguage(uint64 languageId) { m_knownLanguagesMask |= (1llu << languageId); }
        void RemoveLanguage(uint64 languageId) { m_knownLanguagesMask &= ~(1llu << languageId);}
        bool KnowsLanguage(uint64 languageId) const { return (m_knownLanguagesMask & (1llu << languageId)) != 0; }

#ifdef ENABLE_ELUNA
        void Whisper(const std::string& text, const uint32 language, ObjectGuid receiver);
		void RemoveAllSpellCooldown();
#endif
        /*********************************************************/
        /***                   FACTION SYSTEM                  ***/
        /*********************************************************/

    private:
        Team m_team;
        ReputationMgr  m_reputationMgr;
        std::set<uint32> m_temporaryAtWarFactions;
    public:
        static Team TeamForRace(uint8 race);
        Team GetTeam() const final { return m_team; }
        TeamId GetTeamId() const { return m_team == ALLIANCE ? TEAM_ALLIANCE : TEAM_HORDE; }
        static uint32 GetFactionForRace(uint8 race);
        void SetFactionForRace(uint8 race);

        ReputationMgr&       GetReputationMgr() { return m_reputationMgr; }
        ReputationMgr const& GetReputationMgr() const { return m_reputationMgr; }
        ReputationRank GetReputationRank(uint32 faction_id) const;
        void RewardReputation(Unit const* pVictim, float rate);
        void RewardReputation(Quest const* pQuest);
        int32 CalculateReputationGain(ReputationSource source, int32 rep, int32 faction, uint32 creatureOrQuestLevel = 0, bool noAuraBonus = false);
        void SetTemporaryAtWarWithFaction(uint32 factionId) { m_temporaryAtWarFactions.insert(factionId); }
        void ClearTemporaryWarWithFactions();

        bool ChangeRace(uint8 newRace);
        bool ChangeItemsForRace(uint8 oldRace, uint8 newRace);
        bool ChangeReputationsForRace(uint8 oldRace, uint8 newRace);
        bool ChangeQuestsForRace(uint8 oldRace, uint8 newRace);
        bool ConvertSpell(uint32 oldSpellId, uint32 newSpellId);
        bool ChangeSpellsForRace(uint8 oldRace, uint8 newRace);

        /*********************************************************/
        /***                    PVP SYSTEM                     ***/
        /*********************************************************/

    private:
        HonorMgr  m_honorMgr;
        void UpdatePvPFlagTimer(uint32 diff);
        void UpdatePvPContestedFlagTimer(uint32 diff);
    public:
        PvPInfo pvpInfo;
        void UpdatePvP(bool state, bool overriding = false);
        void UpdatePvPContested(bool state, bool overriding = false);

        bool IsPvPDesired() const { return HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_PVP_DESIRED); }
        void SetPvPDesired(bool state);
        bool IsFFAPvP() const { return HasFlag(PLAYER_FLAGS, PLAYER_FLAGS_FFA_PVP); }
        void SetFFAPvP(bool state);
        bool IsInInterFactionMode() const;

        // todo: -maybe move UpdateDuelFlag+DuelComplete to independent DuelHandler.
        DuelInfo* m_duel;
        bool IsInDuelWith(Player const* player) const { return m_duel && m_duel->opponent == player && m_duel->startTime != 0; }
        void UpdateDuelFlag(time_t currTime);
        void CheckDuelDistance(time_t currTime);
        void DuelComplete(DuelCompleteType type);
        void SendDuelCountdown(uint32 counter) const;

        void RewardHonor(Unit const* pVictim, uint32 groupSize);
        void RewardHonorOnDeath();
        bool IsHonorOrXPTarget(Unit const* pVictim) const;
        bool IsCityProtector() const;
        void SetCityTitle();
        void RemoveCityTitle();

        HonorMgr&       GetHonorMgr() { return m_honorMgr; }
        HonorMgr const& GetHonorMgr() const { return m_honorMgr; }

        /*********************************************************/
        /***               OUTDOOR PVP SYSTEM                  ***/
        /*********************************************************/

    public:
        ZoneScript* GetZoneScript() const override;
        // returns true if the player is in active state for outdoor pvp objective capturing, false otherwise
        bool IsOutdoorPvPActive() const;

        /*********************************************************/
        /***               BATTLEGROUND SYSTEM                 ***/
        /*********************************************************/

    protected:
        /*
        this is an array of BG queues (BgTypeIDs) in which is player
        */
        struct BgBattleGroundQueueID_Rec
        {
            BattleGroundQueueTypeId bgQueueTypeId;
            uint32 invitedToInstance;
        };

        BgBattleGroundQueueID_Rec m_bgBattleGroundQueueID[PLAYER_MAX_BATTLEGROUND_QUEUES];
        BGData                    m_bgData;
    public:
        bool InBattleGround()       const                { return m_bgData.bgInstanceID != 0; }
        uint32 GetBattleGroundId()  const                { return m_bgData.bgInstanceID; }
        BattleGroundTypeId GetBattleGroundTypeId() const { return m_bgData.bgTypeID; }
        BattleGround* GetBattleGround() const;

        static uint32 GetMinLevelForBattleGroundBracketId(BattleGroundBracketId bracket_id, BattleGroundTypeId bgTypeId);
        static uint32 GetMaxLevelForBattleGroundBracketId(BattleGroundBracketId bracket_id, BattleGroundTypeId bgTypeId);
        static BattleGroundBracketId GetBattleGroundBracketIdFromLevel(BattleGroundTypeId bgTypeId, uint32 level);
        BattleGroundBracketId GetBattleGroundBracketIdFromLevel(BattleGroundTypeId bgTypeId) const;

        bool InBattleGroundQueue() const
        {
            for (const auto& i : m_bgBattleGroundQueueID)
                if (i.bgQueueTypeId != BATTLEGROUND_QUEUE_NONE)
                    return true;
            return false;
        }

        BattleGroundQueueTypeId GetQueuedBattleground() const
        {
            for (const auto& i : m_bgBattleGroundQueueID)
                if (i.bgQueueTypeId != BATTLEGROUND_QUEUE_NONE)
                    return i.bgQueueTypeId;
            return BATTLEGROUND_QUEUE_NONE;
        }

        BattleGroundQueueTypeId GetBattleGroundQueueTypeId(uint32 index) const { return m_bgBattleGroundQueueID[index].bgQueueTypeId; }
        uint32 GetBattleGroundQueueIndex(BattleGroundQueueTypeId bgQueueTypeId) const
        {
            for (int i=0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
                if (m_bgBattleGroundQueueID[i].bgQueueTypeId == bgQueueTypeId)
                    return i;
            return PLAYER_MAX_BATTLEGROUND_QUEUES;
        }
        bool IsInvitedForBattleGroundQueueType(BattleGroundQueueTypeId bgQueueTypeId) const
        {
            for (const auto& i : m_bgBattleGroundQueueID)
                if (i.bgQueueTypeId == bgQueueTypeId)
                    return i.invitedToInstance != 0;
            return false;
        }
        bool InBattleGroundQueueForBattleGroundQueueType(BattleGroundQueueTypeId bgQueueTypeId) const
        {
            return GetBattleGroundQueueIndex(bgQueueTypeId) < PLAYER_MAX_BATTLEGROUND_QUEUES;
        }

        void SetBattleGroundId(uint32 val, BattleGroundTypeId bgTypeId)
        {
            m_bgData.bgInstanceID = val;
            m_bgData.bgTypeID = bgTypeId;
            m_bgData.m_needSave = true;
        }
        uint32 AddBattleGroundQueueId(BattleGroundQueueTypeId val)
        {
            for (int i=0; i < PLAYER_MAX_BATTLEGROUND_QUEUES; ++i)
            {
                if (m_bgBattleGroundQueueID[i].bgQueueTypeId == BATTLEGROUND_QUEUE_NONE || m_bgBattleGroundQueueID[i].bgQueueTypeId == val)
                {
                    m_bgBattleGroundQueueID[i].bgQueueTypeId = val;
                    m_bgBattleGroundQueueID[i].invitedToInstance = 0;
                    return i;
                }
            }
            return PLAYER_MAX_BATTLEGROUND_QUEUES;
        }
        bool HasFreeBattleGroundQueueId() const;
        void RemoveBattleGroundQueueId(BattleGroundQueueTypeId val)
        {
            for (auto& i : m_bgBattleGroundQueueID)
            {
                if (i.bgQueueTypeId == val)
                {
                    i.bgQueueTypeId = BATTLEGROUND_QUEUE_NONE;
                    i.invitedToInstance = 0;
                    return;
                }
            }
        }
        void SetInviteForBattleGroundQueueType(BattleGroundQueueTypeId bgQueueTypeId, uint32 instanceId)
        {
            for (auto& i : m_bgBattleGroundQueueID)
                if (i.bgQueueTypeId == bgQueueTypeId)
                    i.invitedToInstance = instanceId;
        }
        bool IsInvitedForBattleGroundInstance(uint32 instanceId) const
        {
            for (const auto& i : m_bgBattleGroundQueueID)
                if (i.invitedToInstance == instanceId)
                    return true;
            return false;
        }
        WorldLocation const& GetBattleGroundEntryPoint() const { return m_bgData.joinPos; }
        void SetBattleGroundEntryPoint(uint32 mapId, float x, float y, float z, float orientation);
        void SetBattleGroundEntryPoint(Player const* leader = nullptr, bool queuedAtBGPortal = false);

        void SetBGTeam(Team team) { m_bgData.bgTeam = team; m_bgData.m_needSave = true; }
        Team GetBGTeam() const { return m_bgData.bgTeam ? m_bgData.bgTeam : GetTeam(); }

        void LeaveBattleground(bool teleportToEntryPoint = true);
        bool CanJoinToBattleground() const;

        bool GetBGAccessByLevel(BattleGroundTypeId bgTypeId) const;
        bool CanUseBattleGroundObject() const;

        /*********************************************************/
        /***                  ANTICHEAT SYSTEM                 ***/
        /*********************************************************/

    public:
        bool CanSpeak() const;
        bool FallGround(uint8 fallMode);

        // Anticheat
        MovementAnticheat* GetCheatData() const { return m_session->GetCheatData(); }
        void OnDisconnected();
        void RelocateToLastClientPosition();
        void GetSafePosition(float &x, float &y, float &z, GenericTransport const* onTransport = nullptr) const override;

        /*********************************************************/
        /***                 PACKET BROADCASTER                ***/
        /*********************************************************/

    public:
        std::shared_ptr<PlayerBroadcaster> m_broadcaster;
        void DeletePacketBroadcaster();
        void CreatePacketBroadcaster();
        std::shared_ptr<PlayerBroadcaster> GetPacketBroadcaster() const { return m_broadcaster; }

        /*********************************************************/
        /***                 INSTANCE SYSTEM                   ***/
        /*********************************************************/

    public:
        typedef std::unordered_map< uint32 /*mapId*/, InstancePlayerBind > BoundInstancesMap;

    private:
        bool   m_enableInstanceSwitch;
        bool   m_smartInstanceRebind;
        uint32 m_homebindTimer;

        void ResetInstance(InstanceResetMethod method, BoundInstancesMap::iterator& itr);
    public:
        void SendTransferAborted(uint8 reason) const;
        void SendInstanceResetWarning(uint32 mapId, uint32 resetTime) const;
        
        void ResetInstances(InstanceResetMethod method);
        void ResetPersonalInstanceOnLeaveDungeon(uint32 mapId);
        void SendResetInstanceSuccess(uint32 MapId) const;
        void SendResetInstanceFailed(uint32 reason, uint32 MapId) const;
        void SendResetFailedNotify();
        bool CheckInstanceCount(uint32 instanceId) const;
        void AddInstanceEnterTime(uint32 instanceId, time_t enterTime) const;

        void UpdateHomebindTime(uint32 time);
        bool m_instanceValid;
        // permanent binds and solo binds
        BoundInstancesMap m_boundInstances;
        mutable std::mutex m_boundInstancesMutex;
        InstancePlayerBind* GetBoundInstance(uint32 mapId);
        BoundInstancesMap& GetBoundInstances() { return m_boundInstances; }
        void UnbindInstance(uint32 mapId, bool unload = false);
        void UnbindInstance(BoundInstancesMap::iterator &itr, bool unload = false);
        InstancePlayerBind* BindToInstance(DungeonPersistentState* save, bool permanent, bool load = false);
        void SendRaidInfo() const;
        void SendSavedInstances() const;
        static void ConvertInstancesToGroup(Player* player, Group* group = nullptr, ObjectGuid player_guid = ObjectGuid());
        DungeonPersistentState* GetBoundInstanceSaveForSelfOrGroup(uint32 mapId);
        void SetAutoInstanceSwitch(bool v) { m_enableInstanceSwitch = v; }
        bool GetSmartInstanceBindingMode() const { return m_smartInstanceRebind; }
        void SetSmartInstanceBindingMode(bool smartRebinding) { m_smartInstanceRebind = smartRebinding; }

        /*********************************************************/
        /***                   GROUP SYSTEM                    ***/
        /*********************************************************/

    private:
        GroupReference m_group;
        GroupReference m_originalGroup;
        Group* m_groupInvite;
        uint32 m_groupUpdateMask;
        uint64 m_auraUpdateMask;
        uint32 m_LFGAreaId;
    public:
        Group* GetGroupInvite() const { return m_groupInvite; }
        void SetGroupInvite(Group* group) { m_groupInvite = group; }
        Group* GetGroup() { return m_group.getTarget(); }
        Group const* GetGroup() const { return (Group const*)m_group.getTarget(); }
        GroupReference& GetGroupRef() { return m_group; }
        void SetGroup(Group* group, int8 subgroup = -1);
        uint8 GetSubGroup() const { return m_group.getSubGroup(); }
        uint32 GetGroupUpdateFlag() const { return m_groupUpdateMask; }
        void SetGroupUpdateFlag(uint32 flag) { m_groupUpdateMask |= flag; }
        uint64 const& GetAuraUpdateMask() const { return m_auraUpdateMask; }
        void SetAuraUpdateSlot(uint8 slot) { m_auraUpdateMask |= (uint64(1) << slot); }
        void SetAuraUpdateMask(uint64 mask) { m_auraUpdateMask = mask; }
        Player* GetNextRandomRaidMember(float radius);
        PartyResult CanUninviteFromGroup(ObjectGuid uninvitedGuid) const;
        void UpdateGroupLeaderFlag(bool const remove = false);
        bool IsGroupVisibleFor(Player const* p) const;
        bool IsInSameGroupWith(Player const* p) const;
        bool IsInSameRaidWith(Player const* p) const { return p == this || (GetGroup() != nullptr && GetGroup() == p->GetGroup()); }
        void UninviteFromGroup();
        static void RemoveFromGroup(Group* group, ObjectGuid guid);
        void RemoveFromGroup() { RemoveFromGroup(GetGroup(), GetObjectGuid()); }
        void SendUpdateToOutOfRangeGroupMembers();
        void SendDestroyGroupMembers(bool includingSelf = false);

#if SUPPORTED_CLIENT_BUILD <= CLIENT_BUILD_1_8_4
        uint32 GetWhoListPartyStatus() const;
#endif

        // LFG
        void SetLFGAreaId(uint32 areaId) { m_LFGAreaId = areaId; }
        uint32 GetLFGAreaId() const { return m_LFGAreaId; }
        bool IsInLFG() const { return m_LFGAreaId > 0; }

        // BattleGround Group System
        void SetBattleGroundRaid(Group* group, int8 subgroup = -1);
        void RemoveFromBattleGroundRaid();
        Group* GetOriginalGroup() const { return m_originalGroup.getTarget(); }
        GroupReference& GetOriginalGroupRef() { return m_originalGroup; }
        uint8 GetOriginalSubGroup() const { return m_originalGroup.getSubGroup(); }
        void SetOriginalGroup(Group* group, int8 subgroup = -1);

        /*********************************************************/
        /***                   GUILD SYSTEM                    ***/
        /*********************************************************/

    private:
        uint32 m_guildIdInvited;
    public:
        void SetInGuild(uint32 GuildId) { SetUInt32Value(PLAYER_GUILDID, GuildId); }
        void SetRank(uint32 rankId) { SetUInt32Value(PLAYER_GUILDRANK, rankId); }
        void SetGuildIdInvited(uint32 GuildId) { m_guildIdInvited = GuildId; }
        uint32 GetGuildId() const { return GetUInt32Value(PLAYER_GUILDID); }
        uint32 GetRank() const { return GetUInt32Value(PLAYER_GUILDRANK); }
        static uint32 GetGuildIdFromDB(ObjectGuid guid);
        static uint32 GetRankFromDB(ObjectGuid guid);
        int GetGuildIdInvited() const { return m_guildIdInvited; }
        static void RemovePetitionsAndSigns(ObjectGuid guid, uint32 exceptPetitionId = 0);
};

inline Player* Object::ToPlayer()
{
    return IsPlayer() ? static_cast<Player*>(this) : nullptr;
}

inline Player const* Object::ToPlayer() const
{
    return IsPlayer() ? static_cast<Player const*>(this) : nullptr;
}

inline Player* ToPlayer(Object* object)
{
    return object && object->IsPlayer() ? static_cast<Player*>(object) : nullptr;
}

inline Player const* ToPlayer(Object const* object)
{
    return object && object->IsPlayer() ? static_cast<Player const*>(object) : nullptr;
}

void AddItemsSetItem(Player*player,Item* item);
void RemoveItemsSetItem(Player*player,ItemPrototype const* proto);

#endif
