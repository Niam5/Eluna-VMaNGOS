/*
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

#ifndef __SPELL_DEFINES_H
#define __SPELL_DEFINES_H

#include "Common.h"


enum SpellTarget
{
    TARGET_NONE                                        = 0,
    TARGET_UNIT_CASTER                                 = 1,
    TARGET_UNIT_ENEMY_NEAR_CASTER                      = 2,
    TARGET_UNIT_FRIEND_NEAR_CASTER                     = 3,
    TARGET_UNIT_NEAR_CASTER                            = 4,
    TARGET_UNIT_CASTER_PET                             = 5,
    TARGET_UNIT_ENEMY                                  = 6,
    TARGET_ENUM_UNITS_SCRIPT_AOE_AT_SRC_LOC            = 7,
    TARGET_ENUM_UNITS_SCRIPT_AOE_AT_DEST_LOC           = 8,
    TARGET_LOCATION_CASTER_HOME_BIND                   = 9,
    TARGET_LOCATION_CASTER_DIVINE_BIND_NYI             = 10,
    TARGET_PLAYER_NYI                                  = 11,
    TARGET_PLAYER_NEAR_CASTER_NYI                      = 12,
    TARGET_PLAYER_ENEMY_NYI                            = 13,
    TARGET_PLAYER_FRIEND_NYI                           = 14,
    TARGET_ENUM_UNITS_ENEMY_AOE_AT_SRC_LOC             = 15,
    TARGET_ENUM_UNITS_ENEMY_AOE_AT_DEST_LOC            = 16,
    TARGET_LOCATION_DATABASE                           = 17,
    TARGET_LOCATION_CASTER_DEST                        = 18,
    TARGET_UNK_19                                      = 19,
    TARGET_ENUM_UNITS_PARTY_WITHIN_CASTER_RANGE        = 20,
    TARGET_UNIT_FRIEND                                 = 21,
    TARGET_LOCATION_CASTER_SRC                         = 22,
    TARGET_GAMEOBJECT                                  = 23,
    TARGET_ENUM_UNITS_ENEMY_IN_CONE_24                 = 24,
    TARGET_UNIT                                        = 25,
    TARGET_LOCKED                                      = 26,
    TARGET_UNIT_CASTER_MASTER                          = 27,
    TARGET_ENUM_UNITS_ENEMY_AOE_AT_DYNOBJ_LOC          = 28,
    TARGET_ENUM_UNITS_FRIEND_AOE_AT_DYNOBJ_LOC         = 29,
    TARGET_ENUM_UNITS_FRIEND_AOE_AT_SRC_LOC            = 30,
    TARGET_ENUM_UNITS_FRIEND_AOE_AT_DEST_LOC           = 31,
    TARGET_LOCATION_UNIT_MINION_POSITION               = 32,
    TARGET_ENUM_UNITS_PARTY_AOE_AT_SRC_LOC             = 33,
    TARGET_ENUM_UNITS_PARTY_AOE_AT_DEST_LOC            = 34,
    TARGET_UNIT_PARTY                                  = 35,
    TARGET_ENUM_UNITS_ENEMY_WITHIN_CASTER_RANGE        = 36, // TODO: only used with dest-effects - reinvestigate naming
    TARGET_UNIT_FRIEND_AND_PARTY                       = 37,
    TARGET_UNIT_SCRIPT_NEAR_CASTER                     = 38,
    TARGET_LOCATION_CASTER_FISHING_SPOT                = 39,
    TARGET_GAMEOBJECT_SCRIPT_NEAR_CASTER               = 40,
    TARGET_LOCATION_CASTER_FRONT_RIGHT                 = 41,
    TARGET_LOCATION_CASTER_BACK_RIGHT                  = 42,
    TARGET_LOCATION_CASTER_BACK_LEFT                   = 43,
    TARGET_LOCATION_CASTER_FRONT_LEFT                  = 44,
    TARGET_UNIT_FRIEND_CHAIN_HEAL                      = 45,
    TARGET_LOCATION_SCRIPT_NEAR_CASTER                 = 46,
    TARGET_LOCATION_CASTER_FRONT                       = 47,
    TARGET_LOCATION_CASTER_BACK                        = 48,
    TARGET_LOCATION_CASTER_LEFT                        = 49,
    TARGET_LOCATION_CASTER_RIGHT                       = 50,
    TARGET_ENUM_GAMEOBJECTS_SCRIPT_AOE_AT_SRC_LOC      = 51,
    TARGET_ENUM_GAMEOBJECTS_SCRIPT_AOE_AT_DEST_LOC     = 52,
    TARGET_LOCATION_CASTER_TARGET_POSITION             = 53,
    TARGET_ENUM_UNITS_ENEMY_IN_CONE_54                 = 54,
    TARGET_LOCATION_CASTER_FRONT_LEAP                  = 55,
    TARGET_ENUM_UNITS_RAID_WITHIN_CASTER_RANGE         = 56,
    TARGET_UNIT_RAID                                   = 57,
    TARGET_UNIT_RAID_NEAR_CASTER                       = 58,
    TARGET_ENUM_UNITS_FRIEND_IN_CONE                   = 59,
    TARGET_ENUM_UNITS_SCRIPT_IN_CONE_60                = 60,
    TARGET_UNIT_RAID_AND_CLASS                         = 61,
    TARGET_PLAYER_RAID_NYI                             = 62,
    TARGET_LOCATION_UNIT_POSITION                      = 63,

    MAX_SPELL_TARGETS
};

enum SpellMissInfo
{
    SPELL_MISS_NONE                    = 0,
    SPELL_MISS_MISS                    = 1,
    SPELL_MISS_RESIST                  = 2,
    SPELL_MISS_DODGE                   = 3,
    SPELL_MISS_PARRY                   = 4,
    SPELL_MISS_BLOCK                   = 5,
    SPELL_MISS_EVADE                   = 6,
    SPELL_MISS_IMMUNE                  = 7,
    SPELL_MISS_IMMUNE2                 = 8,
    SPELL_MISS_DEFLECT                 = 9,
    SPELL_MISS_ABSORB                  = 10,
    SPELL_MISS_REFLECT                 = 11
};

enum SpellHitType
{
    SPELL_HIT_TYPE_CRIT_DEBUG           = 0x01,
    SPELL_HIT_TYPE_CRIT                 = 0x02,
    SPELL_HIT_TYPE_HIT_DEBUG            = 0x04,
    SPELL_HIT_TYPE_SPLIT                = 0x08,
    SPELL_HIT_TYPE_VICTIM_IS_ATTACKER   = 0x10,
    SPELL_HIT_TYPE_ATTACK_TABLE_DEBUG   = 0x20
};

enum SpellDmgClass
{
    SPELL_DAMAGE_CLASS_NONE     = 0,
    SPELL_DAMAGE_CLASS_MAGIC    = 1,
    SPELL_DAMAGE_CLASS_MELEE    = 2,
    SPELL_DAMAGE_CLASS_RANGED   = 3
};

enum SpellPreventionType
{
    SPELL_PREVENTION_TYPE_NONE      = 0,
    SPELL_PREVENTION_TYPE_SILENCE   = 1,
    SPELL_PREVENTION_TYPE_PACIFY    = 2
};

// Indexes from SpellRange.dbc, listed only special and used in code
enum SpellRangeIndex
{
    SPELL_RANGE_IDX_SELF_ONLY = 1,                          // 0.0
    SPELL_RANGE_IDX_COMBAT    = 2,                          // often ~5.5 (but infact dynamic melee combat range)
    SPELL_RANGE_IDX_ANYWHERE  = 13,                         // 500000 (anywhere)
};

enum SpellEffects
{
    SPELL_EFFECT_NONE                      = 0,
    SPELL_EFFECT_INSTAKILL                 = 1,
    SPELL_EFFECT_SCHOOL_DAMAGE             = 2,
    SPELL_EFFECT_DUMMY                     = 3,
    SPELL_EFFECT_PORTAL_TELEPORT           = 4,
    SPELL_EFFECT_TELEPORT_UNITS            = 5,
    SPELL_EFFECT_APPLY_AURA                = 6,
    SPELL_EFFECT_ENVIRONMENTAL_DAMAGE      = 7,
    SPELL_EFFECT_POWER_DRAIN               = 8,
    SPELL_EFFECT_HEALTH_LEECH              = 9,
    SPELL_EFFECT_HEAL                      = 10,
    SPELL_EFFECT_BIND                      = 11,
    SPELL_EFFECT_PORTAL                    = 12,
    SPELL_EFFECT_RITUAL_BASE               = 13,
    SPELL_EFFECT_RITUAL_SPECIALIZE         = 14,
    SPELL_EFFECT_RITUAL_ACTIVATE_PORTAL    = 15,
    SPELL_EFFECT_QUEST_COMPLETE            = 16,
    SPELL_EFFECT_WEAPON_DAMAGE_NOSCHOOL    = 17,
    SPELL_EFFECT_RESURRECT                 = 18,
    SPELL_EFFECT_ADD_EXTRA_ATTACKS         = 19,
    SPELL_EFFECT_DODGE                     = 20,
    SPELL_EFFECT_EVADE                     = 21,
    SPELL_EFFECT_PARRY                     = 22,
    SPELL_EFFECT_BLOCK                     = 23,
    SPELL_EFFECT_CREATE_ITEM               = 24,
    SPELL_EFFECT_WEAPON                    = 25,
    SPELL_EFFECT_DEFENSE                   = 26,
    SPELL_EFFECT_PERSISTENT_AREA_AURA      = 27,
    SPELL_EFFECT_SUMMON                    = 28,
    SPELL_EFFECT_LEAP                      = 29,
    SPELL_EFFECT_ENERGIZE                  = 30,
    SPELL_EFFECT_WEAPON_PERCENT_DAMAGE     = 31,
    SPELL_EFFECT_TRIGGER_MISSILE           = 32,
    SPELL_EFFECT_OPEN_LOCK                 = 33,
    SPELL_EFFECT_SUMMON_CHANGE_ITEM        = 34,
    SPELL_EFFECT_APPLY_AREA_AURA_PARTY     = 35,
    SPELL_EFFECT_LEARN_SPELL               = 36,
    SPELL_EFFECT_SPELL_DEFENSE             = 37,
    SPELL_EFFECT_DISPEL                    = 38,
    SPELL_EFFECT_LANGUAGE                  = 39,
    SPELL_EFFECT_DUAL_WIELD                = 40,
    SPELL_EFFECT_SUMMON_WILD               = 41,
    SPELL_EFFECT_SUMMON_GUARDIAN           = 42,
    SPELL_EFFECT_TELEPORT_UNITS_FACE_CASTER= 43,
    SPELL_EFFECT_SKILL_STEP                = 44,
    SPELL_EFFECT_ADD_HONOR                 = 45,
    SPELL_EFFECT_SPAWN                     = 46,
    SPELL_EFFECT_TRADE_SKILL               = 47,
    SPELL_EFFECT_STEALTH                   = 48,
    SPELL_EFFECT_DETECT                    = 49,
    SPELL_EFFECT_TRANS_DOOR                = 50,
    SPELL_EFFECT_FORCE_CRITICAL_HIT        = 51,
    SPELL_EFFECT_GUARANTEE_HIT             = 52,
    SPELL_EFFECT_ENCHANT_ITEM              = 53,
    SPELL_EFFECT_ENCHANT_ITEM_TEMPORARY    = 54,
    SPELL_EFFECT_TAMECREATURE              = 55,
    SPELL_EFFECT_SUMMON_PET                = 56,
    SPELL_EFFECT_LEARN_PET_SPELL           = 57,
    SPELL_EFFECT_WEAPON_DAMAGE             = 58,
    SPELL_EFFECT_OPEN_LOCK_ITEM            = 59,
    SPELL_EFFECT_PROFICIENCY               = 60,
    SPELL_EFFECT_SEND_EVENT                = 61,
    SPELL_EFFECT_POWER_BURN                = 62,
    SPELL_EFFECT_THREAT                    = 63,
    SPELL_EFFECT_TRIGGER_SPELL             = 64,
    SPELL_EFFECT_HEALTH_FUNNEL             = 65,
    SPELL_EFFECT_POWER_FUNNEL              = 66,
    SPELL_EFFECT_HEAL_MAX_HEALTH           = 67,
    SPELL_EFFECT_INTERRUPT_CAST            = 68,
    SPELL_EFFECT_DISTRACT                  = 69,
    SPELL_EFFECT_PULL                      = 70,
    SPELL_EFFECT_PICKPOCKET                = 71,
    SPELL_EFFECT_ADD_FARSIGHT              = 72,
    SPELL_EFFECT_SUMMON_POSSESSED          = 73,
    SPELL_EFFECT_SUMMON_TOTEM              = 74,
    SPELL_EFFECT_HEAL_MECHANICAL           = 75,
    SPELL_EFFECT_SUMMON_OBJECT_WILD        = 76,
    SPELL_EFFECT_SCRIPT_EFFECT             = 77,
    SPELL_EFFECT_ATTACK                    = 78,
    SPELL_EFFECT_SANCTUARY                 = 79,
    SPELL_EFFECT_ADD_COMBO_POINTS          = 80,
    SPELL_EFFECT_CREATE_HOUSE              = 81,
    SPELL_EFFECT_BIND_SIGHT                = 82,
    SPELL_EFFECT_DUEL                      = 83,
    SPELL_EFFECT_STUCK                     = 84,
    SPELL_EFFECT_SUMMON_PLAYER             = 85,
    SPELL_EFFECT_ACTIVATE_OBJECT           = 86,
    SPELL_EFFECT_SUMMON_TOTEM_SLOT1        = 87,
    SPELL_EFFECT_SUMMON_TOTEM_SLOT2        = 88,
    SPELL_EFFECT_SUMMON_TOTEM_SLOT3        = 89,
    SPELL_EFFECT_SUMMON_TOTEM_SLOT4        = 90,
    SPELL_EFFECT_THREAT_ALL                = 91,
    SPELL_EFFECT_ENCHANT_HELD_ITEM         = 92,
    SPELL_EFFECT_SUMMON_PHANTASM           = 93,
    SPELL_EFFECT_SELF_RESURRECT            = 94,
    SPELL_EFFECT_SKINNING                  = 95,
    SPELL_EFFECT_CHARGE                    = 96,
    SPELL_EFFECT_SUMMON_CRITTER            = 97,
    SPELL_EFFECT_KNOCK_BACK                = 98,
    SPELL_EFFECT_DISENCHANT                = 99,
    SPELL_EFFECT_INEBRIATE                 = 100,
    SPELL_EFFECT_FEED_PET                  = 101,
    SPELL_EFFECT_DISMISS_PET               = 102,
    SPELL_EFFECT_REPUTATION                = 103,
    SPELL_EFFECT_SUMMON_OBJECT_SLOT1       = 104,
    SPELL_EFFECT_SUMMON_OBJECT_SLOT2       = 105,
    SPELL_EFFECT_SUMMON_OBJECT_SLOT3       = 106,
    SPELL_EFFECT_SUMMON_OBJECT_SLOT4       = 107,
    SPELL_EFFECT_DISPEL_MECHANIC           = 108,
    SPELL_EFFECT_SUMMON_DEAD_PET           = 109,
    SPELL_EFFECT_DESTROY_ALL_TOTEMS        = 110,
    SPELL_EFFECT_DURABILITY_DAMAGE         = 111,
    SPELL_EFFECT_SUMMON_DEMON              = 112,
    SPELL_EFFECT_RESURRECT_NEW             = 113,
    SPELL_EFFECT_ATTACK_ME                 = 114,
    SPELL_EFFECT_DURABILITY_DAMAGE_PCT     = 115,
    SPELL_EFFECT_SKIN_PLAYER_CORPSE        = 116,
    SPELL_EFFECT_SPIRIT_HEAL               = 117,
    SPELL_EFFECT_SKILL                     = 118,
    SPELL_EFFECT_APPLY_AREA_AURA_PET       = 119,
    SPELL_EFFECT_TELEPORT_GRAVEYARD        = 120,
    SPELL_EFFECT_NORMALIZED_WEAPON_DMG     = 121,
    SPELL_EFFECT_122                       = 122,
    SPELL_EFFECT_SEND_TAXI                 = 123,
    SPELL_EFFECT_PLAYER_PULL               = 124,
    SPELL_EFFECT_MODIFY_THREAT_PERCENT     = 125,
    SPELL_EFFECT_126                       = 126,
    SPELL_EFFECT_127                       = 127,
    // Effets "backportes" depuis MaNGOS BC+.
    SPELL_EFFECT_APPLY_AREA_AURA_FRIEND    = 128,
    SPELL_EFFECT_APPLY_AREA_AURA_ENEMY     = 129,
    // Custom
    SPELL_EFFECT_DESPAWN_OBJECT            = 130,
    SPELL_EFFECT_NOSTALRIUS                = 131,
    SPELL_EFFECT_APPLY_AREA_AURA_RAID      = 132,
    SPELL_EFFECT_APPLY_AREA_AURA_OWNER     = 133,
    TOTAL_SPELL_EFFECTS                    = 134
};

enum SpellCastResult
{
    SPELL_FAILED_AFFECTING_COMBAT               = 0x00, // You are in combat
    SPELL_FAILED_ALREADY_AT_FULL_HEALTH               , // You are already at full Health.
    SPELL_FAILED_ALREADY_AT_FULL_POWER                , // You are already at full %s.
    SPELL_FAILED_ALREADY_BEING_TAMED                  , // That creature is already being tamed
    SPELL_FAILED_ALREADY_HAVE_CHARM                   , // You already control a charmed creature
    SPELL_FAILED_ALREADY_HAVE_SUMMON                  , // You already control a summoned creature
    SPELL_FAILED_ALREADY_OPEN                         , // Already open
    SPELL_FAILED_AURA_BOUNCED                         , // A more powerful spell is already active
    SPELL_FAILED_AUTOTRACK_INTERRUPTED                , // Message is hidden/unused
    SPELL_FAILED_BAD_IMPLICIT_TARGETS                 , // You have no target.
    SPELL_FAILED_BAD_TARGETS                          , // Invalid target
    SPELL_FAILED_CANT_BE_CHARMED                      , // Target can't be charmed
    SPELL_FAILED_CANT_BE_DISENCHANTED                 , // Item cannot be disenchanted
#if SUPPORTED_CLIENT_BUILD >= CLIENT_BUILD_1_12_1
    SPELL_FAILED_CANT_BE_PROSPECTED                   , // There are no gems in this
#endif
    SPELL_FAILED_CANT_CAST_ON_TAPPED                  , // Target is tapped
    SPELL_FAILED_CANT_DUEL_WHILE_INVISIBLE            , // You can't start a duel while invisible
    SPELL_FAILED_CANT_DUEL_WHILE_STEALTHED            , // You can't start a duel while stealthed
    SPELL_FAILED_CANT_STEALTH                         , // You are too close to enemies
    SPELL_FAILED_CASTER_AURASTATE                     , // You can't do that yet
    SPELL_FAILED_CASTER_DEAD                          , // You are dead
#if SUPPORTED_CLIENT_BUILD >= CLIENT_BUILD_1_11_2
    SPELL_FAILED_CHARMED                              , // Can't do that while charmed
#endif
    SPELL_FAILED_CHEST_IN_USE                         , // That is already being used
    SPELL_FAILED_CONFUSED                             , // Can't do that while confused
    SPELL_FAILED_DONT_REPORT                          , // Message is hidden/unused
    SPELL_FAILED_EQUIPPED_ITEM                        , // Must have the proper item equipped
    SPELL_FAILED_EQUIPPED_ITEM_CLASS                  , // Must have a %s equipped
    SPELL_FAILED_EQUIPPED_ITEM_CLASS_MAINHAND         , // Must have a %s equipped in the main hand
#if SUPPORTED_CLIENT_BUILD >= CLIENT_BUILD_1_10_2
    SPELL_FAILED_EQUIPPED_ITEM_CLASS_OFFHAND          , // Must have a %s equipped in the offhand
#endif
    SPELL_FAILED_ERROR                                , // Internal error
    SPELL_FAILED_FIZZLE                               , // Fizzled
    SPELL_FAILED_FLEEING                              , // Can't do that while fleeing
    SPELL_FAILED_FOOD_LOWLEVEL                        , // That food's level is not high enough for your pet
    SPELL_FAILED_HIGHLEVEL                            , // Target is too high level
    SPELL_FAILED_HUNGER_SATIATED                      , // Message is hidden/unused
    SPELL_FAILED_IMMUNE                               , // Immune
    SPELL_FAILED_INTERRUPTED                          , // Interrupted
    SPELL_FAILED_INTERRUPTED_COMBAT                   , // Interrupted
    SPELL_FAILED_ITEM_ALREADY_ENCHANTED               , // Item is already enchanted
    SPELL_FAILED_ITEM_GONE                            , // Item is gone
    SPELL_FAILED_ITEM_NOT_FOUND                       , // Tried to enchant an item that didn't exist
    SPELL_FAILED_ITEM_NOT_READY                       , // Item is not ready yet.
    SPELL_FAILED_LEVEL_REQUIREMENT                    , // You are not high enough level
    SPELL_FAILED_LINE_OF_SIGHT                        , // Target not in line of sight
    SPELL_FAILED_LOWLEVEL                             , // Target is too low level
    SPELL_FAILED_LOW_CASTLEVEL                        , // Skill not high enough
    SPELL_FAILED_MAINHAND_EMPTY                       , // Your weapon hand is empty
    SPELL_FAILED_MOVING                               , // Can't do that while moving
    SPELL_FAILED_NEED_AMMO                            , // Ammo needs to be in the paper doll ammo slot before it can be fired
    SPELL_FAILED_NEED_AMMO_POUCH                      , // Requires: %s
    SPELL_FAILED_NEED_EXOTIC_AMMO                     , // Requires exotic ammo: %s
    SPELL_FAILED_NOPATH                               , // No path available
    SPELL_FAILED_NOT_BEHIND                           , // You must be behind your target
    SPELL_FAILED_NOT_FISHABLE                         , // Your cast didn't land in fishable water
    SPELL_FAILED_NOT_HERE                             , // You can't use that here
    SPELL_FAILED_NOT_INFRONT                          , // You must be in front of your target
    SPELL_FAILED_NOT_IN_CONTROL                       , // You are not in control of your actions
    SPELL_FAILED_NOT_KNOWN                            , // Spell not learned
    SPELL_FAILED_NOT_MOUNTED                          , // You are mounted
    SPELL_FAILED_NOT_ON_TAXI                          , // You are in flight
    SPELL_FAILED_NOT_ON_TRANSPORT                     , // You are on a transport
    SPELL_FAILED_NOT_READY                            , // Spell is not ready yet.
    SPELL_FAILED_NOT_SHAPESHIFT                       , // You are in shapeshift form
    SPELL_FAILED_NOT_STANDING                         , // You must be standing to do that
    SPELL_FAILED_NOT_TRADEABLE                        , // You can only use this on an object you own
    SPELL_FAILED_NOT_TRADING                          , // Tried to enchant a trade item, but not trading
    SPELL_FAILED_NOT_UNSHEATHED                       , // You have to be unsheathed to do that!
    SPELL_FAILED_NOT_WHILE_GHOST                      , // Can't cast as ghost
    SPELL_FAILED_NO_AMMO                              , // Out of ammo
    SPELL_FAILED_NO_CHARGES_REMAIN                    , // No charges remain
#if SUPPORTED_CLIENT_BUILD >= CLIENT_BUILD_1_10_2
    SPELL_FAILED_NO_CHAMPION                          , // You haven't selected a champion
#endif
    SPELL_FAILED_NO_COMBO_POINTS                      , // That ability requires combo points
    SPELL_FAILED_NO_DUELING                           , // Dueling isn't allowed here
    SPELL_FAILED_NO_ENDURANCE                         , // Not enough endurance
    SPELL_FAILED_NO_FISH                              , // There aren't any fish here
    SPELL_FAILED_NO_ITEMS_WHILE_SHAPESHIFTED          , // Can't use items while shapeshifted
    SPELL_FAILED_NO_MOUNTS_ALLOWED                    , // You can't mount here
    SPELL_FAILED_NO_PET                               , // You do not have a pet
    SPELL_FAILED_NO_POWER                             , // Dynamic pre-defined messages, no args: Not enough mana, Not enough rage, etc
#if SUPPORTED_CLIENT_BUILD >= CLIENT_BUILD_1_10_2
    SPELL_FAILED_NOTHING_TO_DISPEL                    , // Nothing to dispel
#endif
#if SUPPORTED_CLIENT_BUILD >= CLIENT_BUILD_1_12_1
    SPELL_FAILED_NOTHING_TO_STEAL                     , // Nothing to steal
#endif
    SPELL_FAILED_ONLY_ABOVEWATER                      , // Cannot use while swimming
    SPELL_FAILED_ONLY_DAYTIME                         , // Can only use during the day
    SPELL_FAILED_ONLY_INDOORS                         , // Can only use indoors
    SPELL_FAILED_ONLY_MOUNTED                         , // Can only use while mounted
    SPELL_FAILED_ONLY_NIGHTTIME                       , // Can only use during the night
    SPELL_FAILED_ONLY_OUTDOORS                        , // Can only use outside
    SPELL_FAILED_ONLY_SHAPESHIFT                      , // Must be in %s
    SPELL_FAILED_ONLY_STEALTHED                       , // You must be in stealth mode
    SPELL_FAILED_ONLY_UNDERWATER                      , // Can only use while swimming
    SPELL_FAILED_OUT_OF_RANGE                         , // Out of range.
    SPELL_FAILED_PACIFIED                             , // Can't use that ability while pacified
    SPELL_FAILED_POSSESSED                            , // You are possessed
    SPELL_FAILED_REAGENTS                             , // Message is hidden/unused, supposedly implemented client-side only
    SPELL_FAILED_REQUIRES_AREA                        , // You need to be in %s
    SPELL_FAILED_REQUIRES_SPELL_FOCUS                 , // Requires %s
    SPELL_FAILED_ROOTED                               , // You are unable to move
    SPELL_FAILED_SILENCED                             , // Can't do that while silenced
    SPELL_FAILED_SPELL_IN_PROGRESS                    , // Another action is in progress
    SPELL_FAILED_SPELL_LEARNED                        , // You have already learned the spell
    SPELL_FAILED_SPELL_UNAVAILABLE                    , // The spell is not available to you
    SPELL_FAILED_STUNNED                              , // Can't do that while stunned
    SPELL_FAILED_TARGETS_DEAD                         , // Your target is dead
    SPELL_FAILED_TARGET_AFFECTING_COMBAT              , // Target is in combat
    SPELL_FAILED_TARGET_AURASTATE                     , // You can't do that yet
    SPELL_FAILED_TARGET_DUELING                       , // Target is currently dueling
    SPELL_FAILED_TARGET_ENEMY                         , // Target is hostile
    SPELL_FAILED_TARGET_ENRAGED                       , // Target is too enraged to be charmed
    SPELL_FAILED_TARGET_FRIENDLY                      , // Target is friendly
    SPELL_FAILED_TARGET_IN_COMBAT                     , // The target can't be in combat
    SPELL_FAILED_TARGET_IS_PLAYER                     , // Can't target players
    SPELL_FAILED_TARGET_NOT_DEAD                      , // Target is alive
    SPELL_FAILED_TARGET_NOT_IN_PARTY                  , // Target is not in your party
    SPELL_FAILED_TARGET_NOT_LOOTED                    , // Creature must be looted first
    SPELL_FAILED_TARGET_NOT_PLAYER                    , // Target is not a player
    SPELL_FAILED_TARGET_NO_POCKETS                    , // No pockets to pick
    SPELL_FAILED_TARGET_NO_WEAPONS                    , // Target has no weapons equipped
    SPELL_FAILED_TARGET_UNSKINNABLE                   , // Creature is not skinnable
    SPELL_FAILED_THIRST_SATIATED                      , // Message is hidden/unused
    SPELL_FAILED_TOO_CLOSE                            , // Target too close
    SPELL_FAILED_TOO_MANY_OF_ITEM                     , // You have too many of that item already
    SPELL_FAILED_TOTEMS                               , // Message is hidden/unused, supposedly implemented client-side only
    SPELL_FAILED_TRAINING_POINTS                      , // Not enough training points
    SPELL_FAILED_TRY_AGAIN                            , // Failed attempt
    SPELL_FAILED_UNIT_NOT_BEHIND                      , // Target needs to be behind you
    SPELL_FAILED_UNIT_NOT_INFRONT                     , // Target needs to be in front of you
    SPELL_FAILED_WRONG_PET_FOOD                       , // Your pet doesn't like that food
    SPELL_FAILED_NOT_WHILE_FATIGUED                   , // Can't cast while fatigued
    SPELL_FAILED_TARGET_NOT_IN_INSTANCE               , // Target must be in this instance
    SPELL_FAILED_NOT_WHILE_TRADING                    , // Can't cast while trading
    SPELL_FAILED_TARGET_NOT_IN_RAID                   , // Target is not in your party or raid group
    SPELL_FAILED_DISENCHANT_WHILE_LOOTING             , // Cannot disenchant while looting
#if SUPPORTED_CLIENT_BUILD >= CLIENT_BUILD_1_12_1
    SPELL_FAILED_PROSPECT_WHILE_LOOTING               , // Cannot prospect while looting
    SPELL_FAILED_PROSPECT_NEED_MORE                   , // Message is hidden/unused, supposedly implemented client-side only
#endif
    SPELL_FAILED_TARGET_FREEFORALL                    , // Target is currently in free-for-all PvP combat
    SPELL_FAILED_NO_EDIBLE_CORPSES                    , // There are no nearby corpses to eat
    SPELL_FAILED_ONLY_BATTLEGROUNDS                   , // Can only use in battlegrounds
#if SUPPORTED_CLIENT_BUILD > CLIENT_BUILD_1_6_1
    SPELL_FAILED_TARGET_NOT_GHOST                     , // Target is not a ghost
    SPELL_FAILED_TOO_MANY_SKILLS                      , // Your pet can't learn any more skills
#endif
#if SUPPORTED_CLIENT_BUILD > CLIENT_BUILD_1_7_1
    SPELL_FAILED_TRANSFORM_UNUSABLE                   , // You can't use the new item
    SPELL_FAILED_WRONG_WEATHER                        , // The weather isn't right for that
    SPELL_FAILED_DAMAGE_IMMUNE                        , // You can't do that while you are immune
    SPELL_FAILED_PREVENTED_BY_MECHANIC                , // Can't do that while %s
    SPELL_FAILED_PLAY_TIME                            , // Maximum play time exceeded
#endif
#if SUPPORTED_CLIENT_BUILD >= CLIENT_BUILD_1_9_4
    SPELL_FAILED_REPUTATION                           , // Your reputation isn't high enough
#endif
#if SUPPORTED_CLIENT_BUILD >= CLIENT_BUILD_1_12_1
    SPELL_FAILED_MIN_SKILL                            , // Your skill is not high enough.  Requires %s (%d).
#endif
    SPELL_FAILED_UNKNOWN                              , // Generic out of bounds response:  Unknown reason

    SPELL_CAST_OK                               = 0xFF  // custom value, must not be send to client
};

enum SpellInterruptFlags
{
    SPELL_INTERRUPT_FLAG_MOVEMENT        = 0x01, // Movement
    SPELL_INTERRUPT_FLAG_DAMAGE_PUSHBACK = 0x02, // Dmg Pushback(Player)
    SPELL_INTERRUPT_FLAG_STUN            = 0x04, // Stun
    SPELL_INTERRUPT_FLAG_COMBAT          = 0x08, // Combat
    SPELL_INTERRUPT_FLAG_DAMAGE_CANCELS  = 0x10, // Dmg Cancels(Player)
};

enum SpellAuraInterruptFlags
{
    AURA_INTERRUPT_HOSTILE_ACTION_RECEIVED_CANCELS  = 0x00000001,   // 0    on being hit by spell from hostile caster
    AURA_INTERRUPT_DAMAGE_CANCELS                   = 0x00000002,   // 1
    AURA_INTERRUPT_ACTION_CANCELS                   = 0x00000004,   // 2    removed at the beginning of an action (spellcast, ability use etc...)
    AURA_INTERRUPT_MOVING_CANCELS                   = 0x00000008,   // 3
    AURA_INTERRUPT_TURNING_CANCELS                  = 0x00000010,   // 4
    AURA_INTERRUPT_ANIM_CANCELS                     = 0x00000020,   // 5    used by Feign Death
    AURA_INTERRUPT_DISMOUNT_CANCELS                 = 0x00000040,   // 6 
    AURA_INTERRUPT_UNDER_WATER_CANCELS              = 0x00000080,   // 7    removed by entering water
    AURA_INTERRUPT_ABOVE_WATER_CANCELS              = 0x00000100,   // 8    removed by leaving water
    AURA_INTERRUPT_SHEATHING_CANCELS                = 0x00000200,   // 9
    AURA_INTERRUPT_INTERACTING_CANCELS              = 0x00000400,   // 10   on interact with NPC (gossip, trainer, stable pet ...)
    AURA_INTERRUPT_LOOTING_CANCELS                  = 0x00000800,   // 11   on interact with GO or looting
    AURA_INTERRUPT_ATTACKING_CANCELS                = 0x00001000,   // 12
    AURA_INTERRUPT_ITEM_USE_CANCELS                 = 0x00002000,   // 13
    AURA_INTERRUPT_DAMAGE_CHANNEL_DURATION          = 0x00004000,   // 14   only assigned in channel flags
    AURA_INTERRUPT_SHAPESHIFTING_CANCELS            = 0x00008000,   // 15
    AURA_INTERRUPT_ACTION_CANCELS_LATE              = 0x00010000,   // 16   removed at the completion of an action (spellcast, ability use etc...)
    AURA_INTERRUPT_MOUNT_CANCELS                    = 0x00020000,   // 17
    AURA_INTERRUPT_STANDING_CANCELS                 = 0x00040000,   // 18   used by food and drink mostly and sleep/Fake Death like
    AURA_INTERRUPT_LEAVE_WORLD_CANCELS              = 0x00080000,   // 19
    AURA_INTERRUPT_STEALTH_INVIS_CANCELS            = 0x00100000,   // 20
    AURA_INTERRUPT_INVULNERABILITY_BUFF_CANCELS     = 0x00200000,   // 21
    AURA_INTERRUPT_ENTER_WORLD_CANCELS              = 0x00400000,   // 22
};

enum SpellModOp
{
    SPELLMOD_DAMAGE                 = 0,
    SPELLMOD_DURATION               = 1,
    SPELLMOD_THREAT                 = 2,
    SPELLMOD_ATTACK_POWER           = 3,
    SPELLMOD_CHARGES                = 4,
    SPELLMOD_RANGE                  = 5,
    SPELLMOD_RADIUS                 = 6,
    SPELLMOD_CRITICAL_CHANCE        = 7,
    SPELLMOD_ALL_EFFECTS            = 8,
    SPELLMOD_NOT_LOSE_CASTING_TIME  = 9,
    SPELLMOD_CASTING_TIME           = 10,
    SPELLMOD_COOLDOWN               = 11,
    SPELLMOD_SPEED                  = 12,
    SPELLMOD_COST                   = 14,
    SPELLMOD_CRIT_DAMAGE_BONUS      = 15,
    SPELLMOD_RESIST_MISS_CHANCE     = 16,
    SPELLMOD_JUMP_TARGETS           = 17,
    SPELLMOD_CHANCE_OF_SUCCESS      = 18,                   // Only used with SPELL_AURA_ADD_FLAT_MODIFIER and affects proc spells
    SPELLMOD_ACTIVATION_TIME        = 19,
    SPELLMOD_EFFECT_PAST_FIRST      = 20,
    SPELLMOD_GLOBAL_COOLDOWN        = 21,
    SPELLMOD_DOT                    = 22,
    SPELLMOD_HASTE                  = 23,
    SPELLMOD_SPELL_BONUS_DAMAGE     = 24,
    SPELLMOD_MULTIPLE_VALUE         = 27,
    SPELLMOD_RESIST_DISPEL_CHANCE   = 28,
    MAX_SPELLMOD                    = 29,
};

// Note: SPELLMOD_* values is aura types in fact
enum SpellModType
{
    SPELLMOD_TYPE_NONE    = 0,
    SPELLMOD_FLAT         = 107,                            // SPELL_AURA_ADD_FLAT_MODIFIER
    SPELLMOD_PCT          = 108                             // SPELL_AURA_ADD_PCT_MODIFIER
};

// Spell aura states
enum AuraState
{   // (C) used in caster aura state     (T) used in target aura state
    AURA_STATE_DEFENSE                      = 1,            // C   |
    AURA_STATE_HEALTHLESS_20_PERCENT        = 2,            // C T |
    AURA_STATE_BERSERKING                   = 3,            // C   |
    AURA_STATE_FROZEN                       = 4,            //     | frozen target (but not used for any spells in 1.12.1 at client side)
    AURA_STATE_JUDGEMENT                    = 5,            // C   |
    //AURA_STATE_UNKNOWN6                   = 6,            //     | not used
    AURA_STATE_HUNTER_PARRY                 = 7,            // C   |
    AURA_STATE_ROGUE_ATTACK_FROM_STEALTH    = 8,            // C   | FIX ME: not implemented yet!
    // Custom aura states - not based on spell data:
    AURA_STATE_HEALTHLESS_15_PERCENT        = 9,
    AURA_STATE_HEALTHLESS_10_PERCENT        = 10,
    AURA_STATE_HEALTHLESS_5_PERCENT         = 11,
};

// Spell mechanics
enum Mechanics
{
    MECHANIC_NONE             = 0,
    MECHANIC_CHARM            = 1,
    MECHANIC_DISORIENTED      = 2,
    MECHANIC_DISARM           = 3,
    MECHANIC_DISTRACT         = 4,
    MECHANIC_FEAR             = 5,
    MECHANIC_FUMBLE           = 6,
    MECHANIC_ROOT             = 7,
    MECHANIC_PACIFY           = 8,                          // 0 spells use this mechanic
    MECHANIC_SILENCE          = 9,
    MECHANIC_SLEEP            = 10,
    MECHANIC_SNARE            = 11,
    MECHANIC_STUN             = 12,
    MECHANIC_FREEZE           = 13,
    MECHANIC_KNOCKOUT         = 14,
    MECHANIC_BLEED            = 15,
    MECHANIC_BANDAGE          = 16,
    MECHANIC_POLYMORPH        = 17,
    MECHANIC_BANISH           = 18,
    MECHANIC_SHIELD           = 19,
    MECHANIC_SHACKLE          = 20,
    MECHANIC_MOUNT            = 21,
    MECHANIC_PERSUADE         = 22,                         // 0 spells use this mechanic
    MECHANIC_TURN             = 23,
    MECHANIC_HORROR           = 24,
    MECHANIC_INVULNERABILITY  = 25,
    MECHANIC_INTERRUPT        = 26,
    MECHANIC_DAZE             = 27,
    MECHANIC_DISCOVERY        = 28,
    MECHANIC_IMMUNE_SHIELD    = 29,                         // Divine (Blessing) Shield/Protection and Ice Block
    MECHANIC_SAPPED           = 30,

    // Custom
    MECHANIC_SLOW_CAST_SPEED  = 31                          // Curse of Tongues
};

#define FIRST_MECHANIC          1
#define MAX_MECHANIC            31

#define IMMUNE_TO_ROOT_AND_SNARE_MASK ( \
    (1<<(MECHANIC_ROOT-1))|(1<<(MECHANIC_SNARE-1)))

#define IMMUNE_TO_ROOT_AND_STUN_MASK ( \
    (1<<(MECHANIC_ROOT-1))|(1<<(MECHANIC_STUN-1)))

#define CONFUSED_MECHANIC_MASK ( \
    (1<<(MECHANIC_DISORIENTED-1))|(1<<(MECHANIC_POLYMORPH-1)))

// Daze and all croud control spells except polymorph are not removed
#define MECHANIC_NOT_REMOVED_BY_SHAPESHIFT ( \
    (1<<(MECHANIC_CHARM -1))|(1<<(MECHANIC_DISORIENTED-1))|(1<<(MECHANIC_FEAR  -1))| \
    (1<<(MECHANIC_PACIFY-1))|(1<<(MECHANIC_STUN       -1))|(1<<(MECHANIC_FREEZE-1))| \
    (1<<(MECHANIC_BANISH-1))|(1<<(MECHANIC_SHACKLE    -1))|(1<<(MECHANIC_HORROR-1))| \
    (1<<(MECHANIC_TURN  -1))|(1<<(MECHANIC_DAZE       -1))|(1<<(MECHANIC_SAPPED-1)))

// Spell dispell type
enum DispelType
{
    DISPEL_NONE         = 0,
    DISPEL_MAGIC        = 1,
    DISPEL_CURSE        = 2,
    DISPEL_DISEASE      = 3,
    DISPEL_POISON       = 4,
    DISPEL_STEALTH      = 5,
    DISPEL_INVISIBILITY = 6,
    DISPEL_ALL          = 7,
    DISPEL_SPE_NPC_ONLY = 8,
    DISPEL_ENRAGE       = 9,
    DISPEL_ZG_TICKET    = 10
};

#define DISPEL_ALL_MASK ( (1<<DISPEL_MAGIC) | (1<<DISPEL_CURSE) | (1<<DISPEL_DISEASE) | (1<<DISPEL_POISON) )

// To all Immune system,if target has immunes,
// some spell that related to ImmuneToDispel or ImmuneToSchool or ImmuneToDamage type can't cast to it,
// some spell_effects that related to ImmuneToEffect<effect>(only this effect in the spell) can't cast to it,
// some aura(related to Mechanics or ImmuneToState<aura>) can't apply to it.
enum SpellImmunity
{
    IMMUNITY_EFFECT                = 0,                     // enum SpellEffects
    IMMUNITY_STATE                 = 1,                     // enum AuraType
    IMMUNITY_SCHOOL                = 2,                     // enum SpellSchoolMask
    IMMUNITY_DAMAGE                = 3,                     // enum SpellSchoolMask
    IMMUNITY_DISPEL                = 4,                     // enum DispelType
    IMMUNITY_MECHANIC              = 5                      // enum Mechanics
};

#define MAX_SPELL_IMMUNITY           6

/**
 * The different spell schools that are available, used in both damage calculation
 * and spell casting to decide what should be affected, the SPELL_SCHOOL_NORMAL
 * is the armor, others should be self explanatory.
 *
 * Note that these are the values to use for changing ie, the armor via a
 * Modifier, and it is the Modifier::m_miscValue that should be set.
 */
enum SpellSchools
{
    SPELL_SCHOOL_NORMAL                 = 0,            // Physical, Armor
    SPELL_SCHOOL_HOLY                   = 1,
    SPELL_SCHOOL_FIRE                   = 2,
    SPELL_SCHOOL_NATURE                 = 3,
    SPELL_SCHOOL_FROST                  = 4,
    SPELL_SCHOOL_SHADOW                 = 5,
    SPELL_SCHOOL_ARCANE                 = 6
};

#define MAX_SPELL_SCHOOL                  7

/**
 * A bitmask of the available SpellSchools. Used for convenience
 */
enum SpellSchoolMask
{
    SPELL_SCHOOL_MASK_NONE    = 0x00,                       // not exist
    SPELL_SCHOOL_MASK_NORMAL  = (1 << SPELL_SCHOOL_NORMAL), // PHYSICAL (Armor)
    SPELL_SCHOOL_MASK_HOLY    = (1 << SPELL_SCHOOL_HOLY  ),
    SPELL_SCHOOL_MASK_FIRE    = (1 << SPELL_SCHOOL_FIRE  ),
    SPELL_SCHOOL_MASK_NATURE  = (1 << SPELL_SCHOOL_NATURE),
    SPELL_SCHOOL_MASK_FROST   = (1 << SPELL_SCHOOL_FROST ),
    SPELL_SCHOOL_MASK_SHADOW  = (1 << SPELL_SCHOOL_SHADOW),
    SPELL_SCHOOL_MASK_ARCANE  = (1 << SPELL_SCHOOL_ARCANE),

    // unions

    // 124, not include normal and holy damage
    SPELL_SCHOOL_MASK_SPELL   = ( SPELL_SCHOOL_MASK_FIRE   |
                                  SPELL_SCHOOL_MASK_NATURE | SPELL_SCHOOL_MASK_FROST  |
                                  SPELL_SCHOOL_MASK_SHADOW | SPELL_SCHOOL_MASK_ARCANE ),
    // 126
    SPELL_SCHOOL_MASK_MAGIC   = ( SPELL_SCHOOL_MASK_HOLY | SPELL_SCHOOL_MASK_SPELL ),

    // 127
    SPELL_SCHOOL_MASK_ALL     = ( SPELL_SCHOOL_MASK_NORMAL | SPELL_SCHOOL_MASK_MAGIC )
};

#define SPELL_SCHOOL_MASK_MAGIC                            \
    ( SPELL_SCHOOL_MASK_HOLY | SPELL_SCHOOL_MASK_FIRE | SPELL_SCHOOL_MASK_NATURE |  \
      SPELL_SCHOOL_MASK_FROST | SPELL_SCHOOL_MASK_SHADOW | \
      SPELL_SCHOOL_MASK_ARCANE )

// converts school value into schoolmask missing in 1.12 dbc
inline SpellSchoolMask GetSchoolMask(uint32 school)
{
    return SpellSchoolMask(1 << school);
}

inline SpellSchools GetFirstSchoolInMask(SpellSchoolMask mask)
{
    for(int i = 0; i < MAX_SPELL_SCHOOL; ++i)
        if (mask & (1 << i))
            return SpellSchools(i);

    return SPELL_SCHOOL_NORMAL;
}

enum SpellVisualKit
{
    SPELL_VISUAL_KIT_FOOD           = 406,
    SPELL_VISUAL_KIT_DRINK          = 438
};


// ***********************************
// Spell Attributes definitions
// ***********************************
enum SpellAttributes
{
    SPELL_ATTR_PROC_FAILURE_BURNS_CHARGE        = 0x00000001,            // 0
    SPELL_ATTR_USES_RANGED_SLOT                 = 0x00000002,            // 1 All ranged abilites have this flag
    SPELL_ATTR_ON_NEXT_SWING_NO_DAMAGE          = 0x00000004,            // 2
    SPELL_ATTR_NEED_EXOTIC_AMMO                 = 0x00000008,            // 3 Vanilla only attribute removed in TBC
    SPELL_ATTR_IS_ABILITY                       = 0x00000010,            // 4 Client puts 'ability' instead of 'spell' in game strings for these spells
    SPELL_ATTR_IS_TRADESKILL                    = 0x00000020,            // 5 Trade spells, will be added by client to a sublist of profession spell
    SPELL_ATTR_PASSIVE                          = 0x00000040,            // 6
    SPELL_ATTR_DO_NOT_DISPLAY                   = 0x00000080,            // 7 Spells with this attribute are not visible in spellbook or aura bar
    SPELL_ATTR_DO_NOT_LOG                       = 0x00000100,            // 8 This attributes controls whether spell appears in combat logs
    SPELL_ATTR_HELD_ITEM_ONLY                   = 0x00000200,            // 9 Client automatically selects item from mainhand slot as a cast target
    SPELL_ATTR_ON_NEXT_SWING                    = 0x00000400,            // 10
    SPELL_ATTR_WEARER_CASTS_PROC_TRIGGER        = 0x00000800,            // 11
    SPELL_ATTR_DAYTIME_ONLY                     = 0x00001000,            // 12 Only useable at daytime, not set in 2.4.2
    SPELL_ATTR_NIGHT_ONLY                       = 0x00002000,            // 13 Only useable at night, not set in 2.4.2
    SPELL_ATTR_ONLY_INDOORS                     = 0x00004000,            // 14 Only useable indoors, not set in 2.4.2
    SPELL_ATTR_ONLY_OUTDOORS                    = 0x00008000,            // 15 Only useable outdoors
    SPELL_ATTR_NOT_SHAPESHIFT                   = 0x00010000,            // 16 Not while shapeshifted
    SPELL_ATTR_ONLY_STEALTHED                   = 0x00020000,            // 17 Must be in stealth
    SPELL_ATTR_DO_NOT_SHEATH                    = 0x00040000,            // 18 Client won't hide unit weapons in sheath on cast/channel
    SPELL_ATTR_SCALES_WITH_CREATURE_LEVEL       = 0x00080000,            // 19 Spell damage depends on caster level
    SPELL_ATTR_CANCELS_AUTO_ATTACK_COMBAT       = 0x00100000,            // 20 Stop attack after use this spell (and not begin attack if use)
    SPELL_ATTR_NO_ACTIVE_DEFENSE                = 0x00200000,            // 21 Cannot be dodged/parried/blocked
    SPELL_ATTR_TRACK_TARGET_IN_CAST_PLAYER_ONLY = 0x00400000,            // 22 SetTrackingTarget
    SPELL_ATTR_ALLOW_CAST_WHILE_DEAD            = 0x00800000,            // 23 Castable while dead
    SPELL_ATTR_ALLOW_WHILE_MOUNTED              = 0x01000000,            // 24 Castable while mounted
    SPELL_ATTR_COOLDOWN_ON_EVENT                = 0x02000000,            // 25 Activate and start cooldown after aura fade or remove summoned creature or go
    SPELL_ATTR_AURA_IS_DEBUFF                   = 0x04000000,            // 26 Almost all negative spells have it
    SPELL_ATTR_ALLOW_WHILE_SITTING              = 0x08000000,            // 27 Castable while sitting
    SPELL_ATTR_NOT_IN_COMBAT_ONLY_PEACEFUL      = 0x10000000,            // 28 Cannot be used in combat
    SPELL_ATTR_NO_IMMUNITIES                    = 0x20000000,            // 29 Unaffected by invulnerability
    SPELL_ATTR_HEARTBEAT_RESIST                 = 0x40000000,            // 30 Chance for spell effects to break early (heartbeat resist)
#if SUPPORTED_CLIENT_BUILD > CLIENT_BUILD_1_6_1
    SPELL_ATTR_NO_AURA_CANCEL                   = 0x80000000             // 31 Positive aura can't be canceled
#else
    SPELL_ATTR_CAN_BREAK_ON_DAMAGE              = 0x80000000             // 31 Taking damage has chance to remove the aura
#endif
};

enum SpellAttributesEx
{
    SPELL_ATTR_EX_DISMISS_PET_FIRST             = 0x00000001,            // 0 For spells without this flag client doesn't allow to summon pet if caster has a pet
    SPELL_ATTR_EX_USE_ALL_MANA                  = 0x00000002,            // 1 Use all power (Only paladin Lay of Hands and Bunyanize)
    SPELL_ATTR_EX_IS_CHANNELED                  = 0x00000004,            // 2
    SPELL_ATTR_EX_NO_REDIRECTION                = 0x00000008,            // 3
    SPELL_ATTR_EX_NO_SKILL_INCREASE             = 0x00000010,            // 4 Only assigned to stealth spells for some reason
    SPELL_ATTR_EX_ALLOW_WHILE_STEALTHED         = 0x00000020,            // 5 Does not break stealth
    SPELL_ATTR_EX_IS_SELF_CHANNELED             = 0x00000040,            // 6
    SPELL_ATTR_EX_NO_REFLECTION                 = 0x00000080,            // 7
    SPELL_ATTR_EX_ONLY_PEACEFUL_TARGETS         = 0x00000100,            // 8 Target must not be in combat
    SPELL_ATTR_EX_INITIATES_COMBAT              = 0x00000200,            // 9 Enables Auto-Attack
    SPELL_ATTR_EX_NO_THREAT                     = 0x00000400,            // 10
    SPELL_ATTR_EX_AURA_UNIQUE                   = 0x00000800,            // 11
    SPELL_ATTR_EX_FAILURE_BREAKS_STEALTH        = 0x00001000,            // 12
    SPELL_ATTR_EX_TOGGLE_FARSIGHT               = 0x00002000,            // 13
    SPELL_ATTR_EX_TRACK_TARGET_IN_CHANNEL       = 0x00004000,            // 14 Client automatically forces player to face target when channeling
    SPELL_ATTR_EX_IMMUNITY_PURGES_EFFECT        = 0x00008000,            // 15 Remove auras on immunity
    SPELL_ATTR_EX_IMMUNITY_TO_HOSTILE_AND_FRIENDLY_EFFECTS = 0x00010000, // 16 Aura that provides immunity prevents positive effects too
    SPELL_ATTR_EX_NO_AUTOCAST_AI                = 0x00020000,            // 17
    SPELL_ATTR_EX_PREVENTS_ANIM                 = 0x00040000,            // 18 Stun, polymorph, daze, sleep
    SPELL_ATTR_EX_EXCLUDE_CASTER                = 0x00080000,            // 19
    SPELL_ATTR_EX_FINISHING_MOVE_DAMAGE         = 0x00100000,            // 20 Uses combo points
    SPELL_ATTR_EX_THREAT_ONLY_ON_MISS           = 0x00200000,            // 21
    SPELL_ATTR_EX_FINISHING_MOVE_DURATION       = 0x00400000,            // 22 Uses combo points (in 4.x not required combo point target selected)
    SPELL_ATTR_EX_IGNORE_CASTER_AND_TARGET_RESTRICTIONS = 0x00800000,    // 23 Skips all cast checks, moved to AttributesEx3 after 1.10 (100% correlation)
    SPELL_ATTR_EX_SPECIAL_SKILLUP               = 0x01000000,            // 24 Only fishing spells
    SPELL_ATTR_EX_UNK25                         = 0x02000000,            // 25 Different in vanilla
    SPELL_ATTR_EX_REQUIRE_ALL_TARGETS           = 0x04000000,            // 26
    SPELL_ATTR_EX_DISCOUNT_POWER_ON_MISS        = 0x08000000,            // 27 All these spells refund power on parry or deflect
    SPELL_ATTR_EX_NO_AURA_ICON                  = 0x10000000,            // 28 Client doesn't display these spells in aura bar
    SPELL_ATTR_EX_NAME_IN_CHANNEL_BAR           = 0x20000000,            // 29 Spell name is displayed in cast bar instead of 'channeling' text
    SPELL_ATTR_EX_COMBO_ON_BLOCK                = 0x40000000,            // 30 Overpower
    SPELL_ATTR_EX_CAST_WHEN_LEARNED             = 0x80000000             // 31
};

enum SpellAttributesEx2
{
    SPELL_ATTR_EX2_ALLOW_DEAD_TARGET            = 0x00000001,            // 0 Can target dead unit or corpse
    SPELL_ATTR_EX2_NO_SHAPESHIFT_UI             = 0x00000002,            // 1
    SPELL_ATTR_EX2_IGNORE_LINE_OF_SIGHT         = 0x00000004,            // 2
    SPELL_ATTR_EX2_ALLOW_LOW_LEVEL_BUFF         = 0x00000008,            // 3
    SPELL_ATTR_EX2_USE_SHAPESHIFT_BAR           = 0x00000010,            // 4 Client displays icon in stance bar when learned, even if not shapeshift
    SPELL_ATTR_EX2_AUTO_REPEAT                  = 0x00000020,            // 5
    SPELL_ATTR_EX2_CANNOT_CAST_ON_TAPPED        = 0x00000040,            // 6 Target must be tapped by caster
    SPELL_ATTR_EX2_DO_NOT_REPORT_SPELL_FAILURE  = 0x00000080,            // 7
    SPELL_ATTR_EX2_UNK8                         = 0x00000100,            // 8 Unused
    SPELL_ATTR_EX2_UNK9                         = 0x00000200,            // 9 Unused
    SPELL_ATTR_EX2_SPECIAL_TAMING_FLAG          = 0x00000400,            // 10
    SPELL_ATTR_EX2_NO_TARGET_PER_SECOND_COSTS   = 0x00000800,            // 11
    SPELL_ATTR_EX2_CHAIN_FROM_CASTER            = 0x00001000,            // 12
    SPELL_ATTR_EX2_ENCHANT_OWN_ITEM_ONLY        = 0x00002000,            // 13
    SPELL_ATTR_EX2_ALLOW_WHILE_INVISIBLE        = 0x00004000,            // 14
    SPELL_ATTR_EX2_ENABLE_AFTER_PARRY           = 0x00008000,            // 15 Deprecated in patch 1.8 and moved to CasterAuraState
    SPELL_ATTR_EX2_NO_ACTIVE_PETS               = 0x00010000,            // 16
    SPELL_ATTR_EX2_DO_NOT_RESET_COMBAT_TIMERS   = 0x00020000,            // 17 Don't reset timers for melee autoattacks (swings) or ranged autoattacks (autoshoots)
    SPELL_ATTR_EX2_REQ_DEAD_PET                 = 0x00040000,            // 18 Only Revive pet has it
    SPELL_ATTR_EX2_ALLOW_WHILE_NOT_SHAPESHIFTED = 0x00080000,            // 19 Does not necessary need shapeshift (pre-3.x not have passive spells with this attribute)
    SPELL_ATTR_EX2_INITIATE_COMBAT_POST_CAST    = 0x00100000,            // 20 Client will send CMSG_ATTACK_SWING after SMSG_SPELL_GO
    SPELL_ATTR_EX2_FAIL_ON_ALL_TARGETS_IMMUNE   = 0x00200000,            // 21 For ice blocks, pala immunity buffs, priest absorb shields
    SPELL_ATTR_EX2_NO_INITIAL_THREAT            = 0x00400000,            // 22
    SPELL_ATTR_EX2_PROC_COOLDOWN_ON_FAILURE     = 0x00800000,            // 23
    SPELL_ATTR_EX2_ITEM_CAST_WITH_OWNER_SKILL   = 0x01000000,            // 24 NYI
    SPELL_ATTR_EX2_DONT_BLOCK_MANA_REGEN        = 0x02000000,            // 25
    SPELL_ATTR_EX2_NO_SCHOOL_IMMUNITIES         = 0x04000000,            // 26
    SPELL_ATTR_EX2_IGNORE_WEAPONSKILL           = 0x08000000,            // 27 NYI (only fishing has it)
    SPELL_ATTR_EX2_NOT_AN_ACTION                = 0x10000000,            // 28
    SPELL_ATTR_EX2_CANT_CRIT                    = 0x20000000,            // 29
    SPELL_ATTR_EX2_ACTIVE_THREAT                = 0x40000000,            // 30 Caster is put in combat for 5.5 seconds on cast at enemy unit
    SPELL_ATTR_EX2_RETAIN_ITEM_CAST             = 0x80000000             // 31 Food or Drink Buff (like Well Fed)
};

enum SpellAttributesEx3
{
    SPELL_ATTR_EX3_PVP_ENABLING                 = 0x00000001,            // 0 Spell landed counts as hostile action against enemy even if it doesn't trigger combat state, propagates PvP flags
    SPELL_ATTR_EX3_NO_PROC_EQUIP_REQUIREMENT    = 0x00000002,            // 1
    // Attributes below first added in patch 1.3.
    SPELL_ATTR_EX3_NO_CASTING_BAR_TEXT          = 0x00000004,            // 2
    SPELL_ATTR_EX3_COMPLETELY_BLOCKED           = 0x00000008,            // 3 All effects prevented on block
    SPELL_ATTR_EX3_NO_RES_TIMER                 = 0x00000010,            // 4 Corpse reclaim delay does not apply to accepting resurrection (only Rebirth has it)
    SPELL_ATTR_EX3_NO_DURABILITY_LOSS           = 0x00000020,            // 5
    // Attributes below first added in patch 1.5.
    SPELL_ATTR_EX3_NO_AVOIDANCE                 = 0x00000040,            // 6 Persistent Area Aura not removed on leaving radius
    SPELL_ATTR_EX3_DOT_STACKING_RULE            = 0x00000080,            // 7 Create a separate (de)buff stack for each caster
    // Attributes below first added in patch 1.6.
    SPELL_ATTR_EX3_ONLY_ON_PLAYER               = 0x00000100,            // 8 Can target only players
    SPELL_ATTR_EX3_NOT_A_PROC                   = 0x00000200,            // 9 Aura periodic trigger is not evaluated as triggered
    SPELL_ATTR_EX3_REQUIRES_MAIN_HAND_WEAPON    = 0x00000400,            // 10
    SPELL_ATTR_EX3_ONLY_BATTLEGROUNDS           = 0x00000800,            // 11
    // Attributes below first added in patch 1.7.
    SPELL_ATTR_EX3_ONLY_ON_GHOSTS               = 0x00001000,            // 12
    SPELL_ATTR_EX3_HIDE_CHANNEL_BAR             = 0x00002000,            // 13 Client will not display channeling bar
    // Attributes below first added in patch 1.9.
    SPELL_ATTR_EX3_HIDE_IN_RAID_FILTER          = 0x00004000,            // 14 Only "Honorless Target" has this flag
    SPELL_ATTR_EX3_NORMAL_RANGED_ATTACK         = 0x00008000,            // 15 Spells with this attribute are processed as ranged attacks in client
    SPELL_ATTR_EX3_SUPPRESS_CASTER_PROCS        = 0x00010000,            // 16
    SPELL_ATTR_EX3_SUPPRESS_TARGET_PROCS        = 0x00020000,            // 17
    SPELL_ATTR_EX3_ALWAYS_HIT                   = 0x00040000,            // 18 Spell should always hit its target
    SPELL_ATTR_EX3_INSTANT_TARGET_PROCS         = 0x00080000,            // 19 Related to spell batching
    SPELL_ATTR_EX3_ALLOW_AURA_WHILE_DEAD        = 0x00100000,            // 20 Death persistent spells
    SPELL_ATTR_EX3_ONLY_PROC_OUTDOORS           = 0x00200000,            // 21
    SPELL_ATTR_EX3_CASTING_CANCELS_AUTOREPEAT   = 0x00400000,            // 22 NYI (only Shoot with Wand has it)
    SPELL_ATTR_EX3_NO_DAMAGE_HISTORY            = 0x00800000,            // 23 NYI
    // Attributes below first added in patch 1.10.
    SPELL_ATTR_EX3_REQUIRES_OFFHAND_WEAPON      = 0x01000000,            // 24
    SPELL_ATTR_EX3_TREAT_AS_PERIODIC            = 0x02000000,            // 25 Does not cause spell pushback
    SPELL_ATTR_EX3_CAN_PROC_FROM_PROCS          = 0x04000000,            // 26 Auras with this attribute can proc off procced spells (periodic triggers etc)
    SPELL_ATTR_EX3_ONLY_PROC_ON_CASTER          = 0x08000000,            // 27
    SPELL_ATTR_EX3_IGNORE_CASTER_AND_TARGET_RESTRICTIONS = 0x10000000,   // 28 Skips all cast checks, moved from AttributesEx after 1.10 (100% correlation)
    // Attributes below first added in patch 1.11.
    SPELL_ATTR_EX3_IGNORE_CASTER_MODIFIERS      = 0x20000000,            // 29
    SPELL_ATTR_EX3_DO_NOT_DISPLAY_RANGE         = 0x40000000,            // 30
    SPELL_ATTR_EX3_NOT_ON_AOE_IMMUNE            = 0x80000000             // 31
};

enum SpellAttributesEx4
{
    // Attributes below first added in patch 1.12.
    SPELL_ATTR_EX4_IGNORE_RESISTANCES           = 0x00000001,            // 0 From TC 3.3.5, but not present in 1.12 native DBCs. Add it with spell_mod to prevent a spell from being resisted.
    SPELL_ATTR_EX4_CLASS_TRIGGER_ONLY_ON_TARGET = 0x00000002,            // 1
    SPELL_ATTR_EX4_AURA_EXPIRES_OFFLINE         = 0x00000004,            // 2 Aura continues to expire while player is offline
    SPELL_ATTR_EX4_NO_HELPFUL_THREAT            = 0x00000008,            // 3
    SPELL_ATTR_EX4_NO_HARMFUL_THREAT            = 0x00000010,            // 4
    SPELL_ATTR_EX4_ALLOW_CLIENT_TARGETING       = 0x00000020,            // 5 NYI
    SPELL_ATTR_EX4_CANNOT_BE_STOLEN             = 0x00000040,            // 6 Unused
    SPELL_ATTR_EX4_CAN_CAST_WHILE_CASTING       = 0x00000080,            // 7 NYI (does not seem to work client side either)
    SPELL_ATTR_EX4_IGNORE_DAMAGE_TAKEN_MODIFIERS= 0x00000100,            // 8
    SPELL_ATTR_EX4_COMBAT_FEEDBACK_WHEN_USABLE  = 0x00000200,            // 9 Initially disabled / Trigger activate from event (Execute, Riposte, Deep Freeze...)
};

// Custom flags assigned in the db
enum SpellAttributesCustom
{
    SPELL_CUSTOM_NONE                       = 0x000,
    SPELL_CUSTOM_ALLOW_STACK_BETWEEN_CASTER = 0x001,     // For example 'Siphon Soul' must be able to stack between the warlocks on a mob
    SPELL_CUSTOM_NEGATIVE                   = 0x002,
    SPELL_CUSTOM_POSITIVE                   = 0x004,
    SPELL_CUSTOM_CHAN_NO_DIST_LIMIT         = 0x008,
    SPELL_CUSTOM_FIXED_DAMAGE               = 0x010,     // Not affected by damage/healing done bonus
    SPELL_CUSTOM_IGNORE_ARMOR               = 0x020,
    SPELL_CUSTOM_BEHIND_TARGET              = 0x040,     // For spells that require the caster to be behind the target
    SPELL_CUSTOM_FACE_TARGET                = 0x080,     // For spells that require the target to be in front of the caster
    SPELL_CUSTOM_SINGLE_TARGET_AURA         = 0x100,     // Aura applied by spell can only be on 1 target at a time
    SPELL_CUSTOM_AURA_APPLY_BREAKS_STEALTH  = 0x200,     // Stealth is removed when this aura is applied
    SPELL_CUSTOM_NOT_REMOVED_ON_EVADE       = 0x400,     // Aura persists after creature evades
    SPELL_CUSTOM_SEND_CHANNEL_VISUAL        = 0x800,     // Will periodically send the channeling spell visual kit
    SPELL_CUSTOM_SEPARATE_AURA_PER_CASTER   = 0x1000,    // Each caster has his own aura slot, instead of replacing others
};

// Custom flags assigned by the core based on spell template data
enum SpellAttributesInternal
{
    SPELL_INTERNAL_APPLIES_AURA             = 0x00001,
    SPELL_INTERNAL_APPLIES_PERIODIC_AURA    = 0x00002,
    SPELL_INTERNAL_PASSIVE_STACK_WITH_RANKS = 0x00004,
    SPELL_INTERNAL_POSITIVE                 = 0x00008,
    SPELL_INTERNAL_HEAL                     = 0x00010,
    SPELL_INTERNAL_DIRECT_DAMAGE            = 0x00020,
    SPELL_INTERNAL_CASTER_SOURCE_TARGETS    = 0x00040,
    SPELL_INTERNAL_AOE                      = 0x00080,
    SPELL_INTERNAL_AOE_AURA                 = 0x00100,
    SPELL_INTERNAL_DISMOUNT                 = 0x00200,
    SPELL_INTERNAL_CHARM                    = 0x00400,
    SPELL_INTERNAL_REFLECTABLE              = 0x00800,
    SPELL_INTERNAL_CUSTOM                   = 0x01000,
    SPELL_INTERNAL_DELAYABLE_EFFECTS        = 0x02000,
    SPELL_INTERNAL_BINARY                   = 0x04000,
    SPELL_INTERNAL_NON_PERIODIC_DISPEL      = 0x08000,
    SPELL_INTERNAL_PVE_HEARTBEAT            = 0x10000,
    SPELL_INTERNAL_CROWD_CONTROL            = 0x20000,
};

// Spell proc event related declarations
enum ProcFlags
{
    PROC_FLAG_NONE                           = 0x00000000,

    PROC_FLAG_HEARTBEAT                      = 0x00000001,   // 00 On Tick
    PROC_FLAG_KILL                           = 0x00000002,   // 01 Kill target (in most cases need XP/Honor reward, see Unit::IsTriggeredAtSpellProcEvent for additinoal check)

    PROC_FLAG_DEAL_MELEE_SWING               = 0x00000004,   // 02 Successful melee auto attack
    PROC_FLAG_TAKE_MELEE_SWING               = 0x00000008,   // 03 Taken damage from melee auto attack hit

    PROC_FLAG_DEAL_MELEE_ABILITY             = 0x00000010,   // 04 Successful attack by Spell that use melee weapon
    PROC_FLAG_TAKE_MELEE_ABILITY             = 0x00000020,   // 05 Taken damage by Spell that use melee weapon

    PROC_FLAG_DEAL_RANGED_ATTACK             = 0x00000040,   // 06 Successful Ranged auto attack
    PROC_FLAG_TAKE_RANGED_ATTACK             = 0x00000080,   // 07 Taken damage from ranged auto attack

    PROC_FLAG_DEAL_RANGED_ABILITY            = 0x00000100,   // 08 Successful Ranged attack by Spell that use ranged weapon
    PROC_FLAG_TAKE_RANGED_ABILITY            = 0x00000200,   // 09 Taken damage by Spell that use ranged weapon

    PROC_FLAG_DEAL_HELPFUL_ABILITY           = 0x00000400,   // 10 Successful positive spell cast (no damage class, or not a healing spell)
    PROC_FLAG_TAKE_HELPFUL_ABILITY           = 0x00000800,   // 11 Taken positive spell (no damage class)

    PROC_FLAG_DEAL_HARMFUL_ABILITY           = 0x00001000,   // 12 Successful negative spell cast (no damage class)
    PROC_FLAG_TAKE_HARMFUL_ABILITY           = 0x00002000,   // 13 Taken negative damage (no damage class)

    PROC_FLAG_DEAL_HELPFUL_SPELL             = 0x00004000,   // 14 Successful cast positive spell (by default only on healing (direct and periodic))
    PROC_FLAG_TAKE_HELPFUL_SPELL             = 0x00008000,   // 15 Taken positive spell hit (by default only on healing (direct and periodic))

    PROC_FLAG_DEAL_HARMFUL_SPELL             = 0x00010000,   // 16 Successful negative spell cast (by default only on damage)
    PROC_FLAG_TAKE_HARMFUL_SPELL             = 0x00020000,   // 17 Taken negative spell (by default only on damage)

    PROC_FLAG_DEAL_HARMFUL_PERIODIC          = 0x00040000,   // 18 Successful do periodic (damage / healing, determined by PROC_EX_PERIODIC_POSITIVE or negative if no procEx)
    PROC_FLAG_TAKE_HARMFUL_PERIODIC          = 0x00080000,   // 19 Taken spell periodic (damage / healing, determined by PROC_EX_PERIODIC_POSITIVE or negative if no procEx)

    PROC_FLAG_TAKEN_ANY_DAMAGE               = 0x00100000,   // 20 Taken any damage
    PROC_FLAG_ON_TRAP_ACTIVATION             = 0x00200000,   // 21 On trap activation (Deal Helpful Periodic in db2)

    PROC_FLAG_MAIN_HAND_WEAPON_SWING         = 0x00400000,   // 22 Taken off-hand melee attacks (no spells have it)
    PROC_FLAG_OFF_HAND_WEAPON_SWING          = 0x00800000,   // 23 Successful off-hand melee attacks  (no spells have it)
};

#define MELEE_BASED_TRIGGER_MASK (PROC_FLAG_DEAL_MELEE_SWING        | \
                                  PROC_FLAG_TAKE_MELEE_SWING        | \
                                  PROC_FLAG_DEAL_MELEE_ABILITY      | \
                                  PROC_FLAG_TAKE_MELEE_ABILITY      | \
                                  PROC_FLAG_DEAL_RANGED_ATTACK      | \
                                  PROC_FLAG_TAKE_RANGED_ATTACK      | \
                                  PROC_FLAG_DEAL_RANGED_ABILITY     | \
                                  PROC_FLAG_TAKE_RANGED_ABILITY)

#define NEGATIVE_TRIGGER_MASK (MELEE_BASED_TRIGGER_MASK             | \
                               PROC_FLAG_DEAL_HARMFUL_ABILITY       | \
                               PROC_FLAG_TAKE_HARMFUL_ABILITY       | \
                               PROC_FLAG_DEAL_HARMFUL_SPELL         | \
                               PROC_FLAG_TAKE_HARMFUL_SPELL)

enum ProcFlagsEx
{
    PROC_EX_NONE                = 0x0000000,                // If none can tigger on Hit/Crit only (passive spells MUST defined by SpellFamily flag)
    PROC_EX_NORMAL_HIT          = 0x0000001,                // If set only from normal hit (only damage spells)
    PROC_EX_CRITICAL_HIT        = 0x0000002,
    PROC_EX_MISS                = 0x0000004,
    PROC_EX_RESIST              = 0x0000008,
    PROC_EX_DODGE               = 0x0000010,
    PROC_EX_PARRY               = 0x0000020,
    PROC_EX_BLOCK               = 0x0000040,
    PROC_EX_EVADE               = 0x0000080,
    PROC_EX_IMMUNE              = 0x0000100,
    PROC_EX_DEFLECT             = 0x0000200,
    PROC_EX_ABSORB              = 0x0000400,
    PROC_EX_REFLECT             = 0x0000800,
    PROC_EX_INTERRUPT           = 0x0001000,                // Melee hit result can be Interrupt (not used)
    PROC_EX_RESERVED1           = 0x0002000,
    PROC_EX_RESERVED2           = 0x0004000,
    PROC_EX_RESERVED3           = 0x0008000,
    PROC_EX_EX_TRIGGER_ALWAYS   = 0x0010000,                // If set trigger always ( no matter another flags) used for drop charges
    PROC_EX_NO_PERIODIC         = 0x0020000,                // Will never proc if periodic proc flag present
    PROC_EX_PERIODIC_POSITIVE   = 0x0040000,                // For periodic heal
    PROC_EX_CAST_END            = 0x0080000,                // Procs on end of cast only
};

#define PROX_EX_NO_DAMAGE_MASK (PROC_EX_MISS    | \
                                PROC_EX_RESIST  | \
                                PROC_EX_DODGE   | \
                                PROC_EX_PARRY   | \
                                PROC_EX_BLOCK   | \
                                PROC_EX_EVADE   | \
                                PROC_EX_IMMUNE  | \
                                PROC_EX_DEFLECT | \
                                PROC_EX_ABSORB  | \
                                PROC_EX_REFLECT)

// original names, do not edit
enum SpellCategories
{
    SPELLCATEGORY_DEFAULT = 1,
    SPELLCATEGORY_DIRECT_DAMAGE_SPELL = 2,
    SPELLCATEGORY_ITEM_COMBAT_CONSUMABLE_POTION = 4,
    SPELLCATEGORY_ITEM_FOOD = 11,
    SPELLCATEGORY_HEALING_SPELL = 12,
    SPELLCATEGORY_QUICK_BUFF_RESIST_SPELL = 17,
    SPELLCATEGORY_DAMAGE_OVER_TIME_SPELL = 18,
    SPELLCATEGORY_QUICK_DAMAGE_SPELL = 19,
    SPELLCATEGORY_INVULNERABILITY_OTHER = 20,
    SPELLCATEGORY_QUICK_BUFF_SPELL = 21,
    SPELLCATEGORY_QUICK_DEBUFF_SPELL = 22,
    SPELLCATEGORY_SUMMONING = 23,
    SPELLCATEGORY_ITEM_COMBAT_CONSUMABLE_AGGRESSIVE = 24,
    SPELLCATEGORY_QUICK_HEAL_SPELL = 25,
    SPELLCATEGORY_RESURRECTION_FULL = 26,
    SPELLCATEGORY_ITEM_SCROLL = 27,
    SPELLCATEGORY_ITEM_QUICK_BUFF = 28,
    SPELLCATEGORY_ITEM_DEBUFF = 29,
    SPELLCATEGORY_ITEM_HEALING = 30,
    SPELLCATEGORY_CONJURE_SHORT = 31,
    SPELLCATEGORY_STUN = 32,
    SPELLCATEGORY_MEZ = 33,
    SPELLCATEGORY_ROOT = 34,
    SPELLCATEGORY_DIRECT_DAMAGE_AE_SPELL = 35,
    SPELLCATEGORY_DEBUFF_SPELL = 36,
    SPELLCATEGORY_INVULNERABILITY = 37,
    SPELLCATEGORY_AURA = 38,
    SPELLCATEGORY_SHAPESHIFT = 39,
    SPELLCATEGORY_MELEE_GENERIC = 40,
    SPELLCATEGORY_CRITICAL = 41,
    SPELLCATEGORY_SNARE = 42,
    SPELLCATEGORY_SHOUT = 43,
    SPELLCATEGORY_SPEED = 44,
    SPELLCATEGORY_TOTEM_STONECLAW = 45,
    SPELLCATEGORY_HEALING_GROUP_SPELL = 46,
    SPELLCATEGORY_COMBAT_STATES = 47,
    SPELLCATEGORY_DIRECT_DAMAGE_AE_ABILITY = 49,
    SPELLCATEGORY_DIRECT_DAMAGE_AECONE_ABILITY = 50,
    SPELLCATEGORY_QUICK_DEBUFF_DPS_SPELL = 51,
    SPELLCATEGORY_QUICK_DEBUFF_DR_SPELL = 52,
    SPELLCATEGORY_QUICK_BUFF_DR_SPELL = 54,
    SPELLCATEGORY_QUICK_BUFF_DPS_SPELL = 55,
    SPELLCATEGORY_INSTANT_HEAL_SPELL = 56,
    SPELLCATEGORY_QUICK_HEAL_GROUP_SPELL = 57,
    SPELLCATEGORY_INSTANT_HEAL_GROUP_SPELL = 58,
    SPELLCATEGORY_ITEM_DRINK = 59,
    SPELLCATEGORY_INVULNERABILITY_TEMP = 60,
    SPELLCATEGORY_ENERGIZE_GROUP_SPELL = 61,
    SPELLCATEGORY_ENERGIZE_SPELL = 62,
    SPELLCATEGORY_BIG_DIRECT_DAMAGE_SPELL = 63,
    SPELLCATEGORY_MINIMAP_SPECIAL = 64,
    SPELLCATEGORY_MELEE_SPECIAL = 65,
    SPELLCATEGORY_DODGE_MANEUVER = 66,
    SPELLCATEGORY_BLOCK_MANEUVER = 67,
    SPELLCATEGORY_PARRY_MANEUVER = 68,
    SPELLCATEGORY_DIRECT_DAMAGE_AEPERSISTENT_SPELL = 72,
    SPELLCATEGORY_MARTIAL_ARTS_GENERIC = 73,
    SPELLCATEGORY_MARTIAL_ARTS_SPECIAL = 74,
    SPELLCATEGORY_DETECT = 75,
    SPELLCATEGORY_SHOOT_THROW = 76,
    SPELLCATEGORY_TRADE_HERBALISM = 77,
    SPELLCATEGORY_TRADE_MINING = 78,
    SPELLCATEGORY_ITEM_POTION_NONCOMBAT = 79,
    SPELLCATEGORY_TAUNT_DETAUNT = 82,
    SPELLCATEGORY_TAMING = 83,
    SPELLCATEGORY_TAUNT_AE = 84,
    SPELLCATEGORY_DIRECT_DAMAGE_AECHAIN_ABILITY = 85,
    SPELLCATEGORY_PET = 86,
    SPELLCATEGORY_CONJURE_LONG = 87,
    SPELLCATEGORY_SILENCE = 88,
    SPELLCATEGORY_PORTAL = 89,
    SPELLCATEGORY_CHARM = 93,
    SPELLCATEGORY_ITEM_SUMMONING = 94,
    SPELLCATEGORY_RACIAL_ABILITY = 95,
    SPELLCATEGORY_RACIAL_ABILITY_2 = 96,
    SPELLCATEGORY_SECONDARY_SURVIVAL = 97,
    SPELLCATEGORY_BIG_DIRECT_DAMAGE_SPELL_2 = 98,
    SPELLCATEGORY_INSTANT_SPELL = 99,
    SPELLCATEGORY_ITEM_MANA_GEM = 100,
    SPELLCATEGORY_FELHUNTER = 101,
    SPELLCATEGORY_ITEM_LONG_BUFF = 102,
    SPELLCATEGORY_ITEM_EPIC = 103,
    SPELLCATEGORY_TOTEM_HEALING = 104,
    SPELLCATEGORY_TOTEM_SERPENT = 105,
    SPELLCATEGORY_TOTEM_SLOWING = 106,
    SPELLCATEGORY_TOTEM_MANA = 107,
    SPELLCATEGORY_TOTEM_INVISIBILITY = 108,
    SPELLCATEGORY_MELEE_DISARM = 109,
    SPELLCATEGORY_DISCIPLINE = 132,
    SPELLCATEGORY_GLOBAL = 133,
    SPELLCATEGORY_ITEM_BANDAGE = 150,
    SPELLCATEGORY_FINISHING_MOVE_WEAPONSCALED = 170,
    SPELLCATEGORY_BANISH = 190,
    SPELLCATEGORY_TOTEM_GROUNDING = 230,
    SPELLCATEGORY_BLAST_WAVE = 250,
    SPELLCATEGORY_KIDNEY_SHOT = 270,
    SPELLCATEGORY_PYROBLAST_REUSE = 290,
    SPELLCATEGORY_TRANSMUTE_ALCHEMY = 310,
    SPELLCATEGORY_MOUNT = 330,
    SPELLCATEGORY_INNER_RAGE = 350,
    SPELLCATEGORY_RANGED_WEAPON = 351,
    SPELLCATEGORY_LIGHTNING_SHIELD = 371,
    SPELLCATEGORY_QUEST_FELCURSE = 391,
    SPELLCATEGORY_TRAP = 411,
    SPELLCATEGORY_HOLY_NOVA = 431,
    SPELLCATEGORY_HOLY_FIRE = 451,
    SPELLCATEGORY_ICE_BARRIER = 471,
    SPELLCATEGORY_ASTRAL_RECALL = 511,
    SPELLCATEGORY_NATURES_GRASP = 531, 
    SPELLCATEGORY_AURA_OF_THE_PIOUS = 551,
    SPELLCATEGORY_HURRICANE = 571,
    SPELLCATEGORY_TOTEM_MANA_TIDE = 591,
    SPELLCATEGORY_WINGS_OF_HOPE = 611,
    SPELLCATEGORY_SOUL_FIRE = 631,
    SPELLCATEGORY_DEATH_COIL = 633,
    SPELLCATEGORY_HOWL_OF_TERROR = 634,
    SPELLCATEGORY_SHADOWBURN = 651,
    SPELLCATEGORY_DESPERATE_PRAYER = 671,
    SPELLCATEGORY_CONFLAGRATE = 672,
    SPELLCATEGORY_DEVOURING_PLAGUE = 691,
    SPELLCATEGORY_SUMMON_INFERNAL = 731,
    SPELLCATEGORY_TREE_FORM = 751,
    SPELLCATEGORY_ITEM_SALT_SHAKER = 791,
    SPELLCATEGORY_DIVINE_INTERVENTION = 811,
    SPELLCATEGORY_SOULSTONE = 831,
    SPELLCATEGORY_RESTORATION = 851,
    SPELLCATEGORY_SHADOWMELD = 871,
    SPELLCATEGORY_INTERCEPT = 872,
    SPELLCATEGORY_WHIRLWIND = 891,
    SPELLCATEGORY_HOLY_SHOCK = 892,
    SPELLCATEGORY_DISTRACTING_SHOT = 911,
    SPELLCATEGORY_HOLY_SHIELD = 931,
    SPELLCATEGORY_CONSECRATION = 932,
    SPELLCATEGORY_PVP_BATTLEFIELD_ITEM_LONG_30_MINS = 951,
    SPELLCATEGORY_MORTAL_STRIKE = 971,
    SPELLCATEGORY_ITEM_SNOWMASTER = 991,
    SPELLCATEGORY_FRENZIED_HEALING = 1011,
    SPELLCATEGORY_ITEM_HALF_HOUR = 1031,
    SPELLCATEGORY_ITEM_JUMPER_CABLES = 1051,
    SPELLCATEGORY_ITEM_HATCH_JUBLING = 1071,
    SPELLCATEGORY_BATTLEGROUNDS_RECALL = 1091,
    SPELLCATEGORY_WYVERN_STING = 1111,
    SPELLCATEGORY_HAMMER_OF_VENGEANCE = 1131,
    SPELLCATEGORY_INTIMIDATION = 1132,
    SPELLCATEGORY_FAERIE_FIRE_FERAL = 1133,
    SPELLCATEGORY_RIPOSTE = 1134,
    SPELLCATEGORY_COUNTERATTACK = 1135,
    SPELLCATEGORY_HOLIDAY_FIREWORK_ROCKETS = 1136,
    SPELLCATEGORY_HOLIDAY_FIRECRACKER = 1137,
    SPELLCATEGORY_RC_WEAPONS = 1138,
    SPELLCATEGORY_ITEM_QUEST_10_MINUTES = 1139,
    SPELLCATEGORY_ITEM_QUEST_1_MIN = 1140,
    SPELLCATEGORY_ITEM_BURST_TRINKET = 1141,
    SPELLCATEGORY_HOLIDAY_VALENTINE_PERFUME_COLOGNE = 1142,
    SPELLCATEGORY_ITEM_TARGET_DUMMY = 1143,
    SPELLCATEGORY_PRIEST_RACIAL = 1144,
    SPELLCATEGORY_LIGHTWELL = 1145,
    SPELLCATEGORY_QUEST_1_HOUR = 1149,
    SPELLCATEGORY_SHADOWTHUNDERSTRIKE = 1150,
    SPELLCATEGORY_TALENT_DPS = 1151,
    SPELLCATEGORY_CREATURE_SPECIAL = 1152,
    SPELLCATEGORY_ITEM_COMBAT_CONSUMABLE_NONAGGRESSIVE = 1153,
    SPELLCATEGORY_ARATHI_BASIN_TRINKET = 1155,
    SPELLCATEGORY_CREATURE_SPECIAL_2 = 1159,
    SPELLCATEGORY_ITEM_PRIEST_EPIC_STAFF = 1160,
    SPELLCATEGORY_REINCARNATION = 1161
};

// Spell clasification
enum SpellSpecific
{
    SPELL_NORMAL            = 0,
    SPELL_SEAL              = 1,
    SPELL_BLESSING          = 2,
    SPELL_AURA              = 3,
    SPELL_STING             = 4,
    SPELL_CURSE             = 5,
    SPELL_ASPECT            = 6,
    SPELL_TRACKER           = 7,
    SPELL_WARLOCK_ARMOR     = 8,
    SPELL_MAGE_ARMOR        = 9,
    SPELL_ELEMENTAL_SHIELD  = 10,
    SPELL_MAGE_POLYMORPH    = 11,
    SPELL_POSITIVE_SHOUT    = 12,
    SPELL_JUDGEMENT         = 13,
    SPELL_BATTLE_ELIXIR     = 14,
    SPELL_GUARDIAN_ELIXIR   = 15,
    SPELL_FLASK_ELIXIR      = 16,
    //SPELL_PRESENCE          = 17,                         // used in 3.x
    //SPELL_HAND              = 18,                         // used in 3.x
    SPELL_WELL_FED          = 19,
    SPELL_FOOD              = 20,
    SPELL_DRINK             = 21,
    SPELL_FOOD_AND_DRINK    = 22,
    SPELL_NEGATIVE_HASTE    = 23,
    SPELL_SNARE             = 24,
};

#endif
