/*
 * Copyright (C) 2005-2012 MaNGOS <http://getmangos.com/>
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

#include "Common.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Player.h"
#include "Unit.h"
#include "Spell.h"
#include "SpellAuras.h"
#include "Totem.h"
#include "Creature.h"
#include "Formulas.h"
#include "CreatureAI.h"
#include "Util.h"

pAuraProcHandler AuraProcHandler[TOTAL_AURAS]=
{
    &Unit::HandleNULLProc,                                  //  0 SPELL_AURA_NONE
    &Unit::HandleNULLProc,                                  //  1 SPELL_AURA_BIND_SIGHT
    &Unit::HandleNULLProc,                                  //  2 SPELL_AURA_MOD_POSSESS
    &Unit::HandleNULLProc,                                  //  3 SPELL_AURA_PERIODIC_DAMAGE
    &Unit::HandleDummyAuraProc,                             //  4 SPELL_AURA_DUMMY
    &Unit::HandleRemoveByDamageProc,                        //  5 SPELL_AURA_MOD_CONFUSE
    &Unit::HandleNULLProc,                                  //  6 SPELL_AURA_MOD_CHARM
    &Unit::HandleRemoveByDamageProc,                        //  7 SPELL_AURA_MOD_FEAR
    &Unit::HandleNULLProc,                                  //  8 SPELL_AURA_PERIODIC_HEAL
    &Unit::HandleNULLProc,                                  //  9 SPELL_AURA_MOD_ATTACKSPEED
    &Unit::HandleNULLProc,                                  // 10 SPELL_AURA_MOD_THREAT
    &Unit::HandleNULLProc,                                  // 11 SPELL_AURA_MOD_TAUNT
    &Unit::HandleRemoveByDamageProc,                        // 12 SPELL_AURA_MOD_STUN
    &Unit::HandleNULLProc,                                  // 13 SPELL_AURA_MOD_DAMAGE_DONE
    &Unit::HandleNULLProc,                                  // 14 SPELL_AURA_MOD_DAMAGE_TAKEN
    &Unit::HandleNULLProc,                                  // 15 SPELL_AURA_DAMAGE_SHIELD
    &Unit::HandleNULLProc,                                  // 16 SPELL_AURA_MOD_STEALTH
    &Unit::HandleNULLProc,                                  // 17 SPELL_AURA_MOD_STEALTH_DETECT
    &Unit::HandleNULLProc,                                  // 18 SPELL_AURA_MOD_INVISIBILITY
    &Unit::HandleNULLProc,                                  // 19 SPELL_AURA_MOD_INVISIBILITY_DETECTION
    &Unit::HandleNULLProc,                                  // 20 SPELL_AURA_OBS_MOD_HEALTH
    &Unit::HandleNULLProc,                                  // 21 SPELL_AURA_OBS_MOD_ENERGY
    &Unit::HandleModResistanceAuraProc,                     // 22 SPELL_AURA_MOD_RESISTANCE
    &Unit::HandleNULLProc,                                  // 23 SPELL_AURA_PERIODIC_TRIGGER_SPELL
    &Unit::HandleNULLProc,                                  // 24 SPELL_AURA_PERIODIC_ENERGIZE
    &Unit::HandleNULLProc,                                  // 25 SPELL_AURA_MOD_PACIFY
    &Unit::HandleRemoveByDamageProc,                        // 26 SPELL_AURA_MOD_ROOT
    &Unit::HandleNULLProc,                                  // 27 SPELL_AURA_MOD_SILENCE
    &Unit::HandleNULLProc,                                  // 28 SPELL_AURA_REFLECT_SPELLS
    &Unit::HandleNULLProc,                                  // 29 SPELL_AURA_MOD_STAT
    &Unit::HandleNULLProc,                                  // 30 SPELL_AURA_MOD_SKILL
    &Unit::HandleIncreaseSpeedAuraProc,                     // 31 SPELL_AURA_MOD_INCREASE_SPEED
    &Unit::HandleNULLProc,                                  // 32 SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED
    &Unit::HandleNULLProc,                                  // 33 SPELL_AURA_MOD_DECREASE_SPEED
    &Unit::HandleNULLProc,                                  // 34 SPELL_AURA_MOD_INCREASE_HEALTH
    &Unit::HandleNULLProc,                                  // 35 SPELL_AURA_MOD_INCREASE_ENERGY
    &Unit::HandleNULLProc,                                  // 36 SPELL_AURA_MOD_SHAPESHIFT
    &Unit::HandleNULLProc,                                  // 37 SPELL_AURA_EFFECT_IMMUNITY
    &Unit::HandleNULLProc,                                  // 38 SPELL_AURA_STATE_IMMUNITY
    &Unit::HandleNULLProc,                                  // 39 SPELL_AURA_SCHOOL_IMMUNITY
    &Unit::HandleNULLProc,                                  // 40 SPELL_AURA_DAMAGE_IMMUNITY
    &Unit::HandleNULLProc,                                  // 41 SPELL_AURA_DISPEL_IMMUNITY
    &Unit::HandleProcTriggerSpellAuraProc,                  // 42 SPELL_AURA_PROC_TRIGGER_SPELL
    &Unit::HandleProcTriggerDamageAuraProc,                 // 43 SPELL_AURA_PROC_TRIGGER_DAMAGE
    &Unit::HandleNULLProc,                                  // 44 SPELL_AURA_TRACK_CREATURES
    &Unit::HandleNULLProc,                                  // 45 SPELL_AURA_TRACK_RESOURCES
    &Unit::HandleNULLProc,                                  // 46 SPELL_AURA_46 4 spells in 4.3.4
    &Unit::HandleNULLProc,                                  // 47 SPELL_AURA_MOD_PARRY_PERCENT
    &Unit::HandleNULLProc,                                  // 48 SPELL_AURA_48 4 spells in 4.3.4 Napalm (area damage spell with additional delayed damage effect)
    &Unit::HandleNULLProc,                                  // 49 SPELL_AURA_MOD_DODGE_PERCENT
    &Unit::HandleNULLProc,                                  // 50 SPELL_AURA_MOD_CRITICAL_HEALING_AMOUNT
    &Unit::HandleNULLProc,                                  // 51 SPELL_AURA_MOD_BLOCK_CHANCE_PERCENT
    &Unit::HandleNULLProc,                                  // 52 SPELL_AURA_MOD_CRIT_PERCENT
    &Unit::HandleNULLProc,                                  // 53 SPELL_AURA_PERIODIC_LEECH
    &Unit::HandleNULLProc,                                  // 54 SPELL_AURA_MOD_HIT_CHANCE
    &Unit::HandleNULLProc,                                  // 55 SPELL_AURA_MOD_SPELL_HIT_CHANCE
    &Unit::HandleRemoveByDamageProc,                        // 56 SPELL_AURA_TRANSFORM
    &Unit::HandleSpellCritChanceAuraProc,                   // 57 SPELL_AURA_MOD_SPELL_CRIT_CHANCE
    &Unit::HandleNULLProc,                                  // 58 SPELL_AURA_MOD_INCREASE_SWIM_SPEED
    &Unit::HandleNULLProc,                                  // 59 SPELL_AURA_MOD_DAMAGE_DONE_CREATURE
    &Unit::HandleRemoveByDamageProc,                        // 60 SPELL_AURA_MOD_PACIFY_SILENCE
    &Unit::HandleNULLProc,                                  // 61 SPELL_AURA_MOD_SCALE
    &Unit::HandleNULLProc,                                  // 62 SPELL_AURA_PERIODIC_HEALTH_FUNNEL
    &Unit::HandleNULLProc,                                  // 63 unused (3.0.8a-4.3.4) old SPELL_AURA_PERIODIC_MANA_FUNNEL
    &Unit::HandleNULLProc,                                  // 64 SPELL_AURA_PERIODIC_MANA_LEECH
    &Unit::HandleModCastingSpeedNotStackAuraProc,           // 65 SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK
    &Unit::HandleNULLProc,                                  // 66 SPELL_AURA_FEIGN_DEATH
    &Unit::HandleNULLProc,                                  // 67 SPELL_AURA_MOD_DISARM
    &Unit::HandleNULLProc,                                  // 68 SPELL_AURA_MOD_STALKED
    &Unit::HandleNULLProc,                                  // 69 SPELL_AURA_SCHOOL_ABSORB
    &Unit::HandleNULLProc,                                  // 70 SPELL_AURA_EXTRA_ATTACKS      Useless, used by only one spell 41560 that has only visual effect (3.2.2a)
    &Unit::HandleNULLProc,                                  // 71 SPELL_AURA_MOD_SPELL_CRIT_CHANCE_SCHOOL
    &Unit::HandleModPowerCostSchoolAuraProc,                // 72 SPELL_AURA_MOD_POWER_COST_SCHOOL_PCT
    &Unit::HandleModPowerCostSchoolAuraProc,                // 73 SPELL_AURA_MOD_POWER_COST_SCHOOL
    &Unit::HandleReflectSpellsSchoolAuraProc,               // 74 SPELL_AURA_REFLECT_SPELLS_SCHOOL
    &Unit::HandleNULLProc,                                  // 75 SPELL_AURA_MOD_LANGUAGE
    &Unit::HandleNULLProc,                                  // 76 SPELL_AURA_FAR_SIGHT
    &Unit::HandleMechanicImmuneResistanceAuraProc,          // 77 SPELL_AURA_MECHANIC_IMMUNITY
    &Unit::HandleNULLProc,                                  // 78 SPELL_AURA_MOUNTED
    &Unit::HandleModDamagePercentDoneAuraProc,              // 79 SPELL_AURA_MOD_DAMAGE_PERCENT_DONE
    &Unit::HandleNULLProc,                                  // 80 SPELL_AURA_MOD_PERCENT_STAT
    &Unit::HandleNULLProc,                                  // 81 SPELL_AURA_SPLIT_DAMAGE_PCT
    &Unit::HandleNULLProc,                                  // 82 SPELL_AURA_WATER_BREATHING
    &Unit::HandleNULLProc,                                  // 83 SPELL_AURA_MOD_BASE_RESISTANCE
    &Unit::HandleNULLProc,                                  // 84 SPELL_AURA_MOD_REGEN
    &Unit::HandleCantTrigger,                               // 85 SPELL_AURA_MOD_POWER_REGEN
    &Unit::HandleNULLProc,                                  // 86 SPELL_AURA_CHANNEL_DEATH_ITEM
    &Unit::HandleNULLProc,                                  // 87 SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN
    &Unit::HandleNULLProc,                                  // 88 SPELL_AURA_MOD_HEALTH_REGEN_PERCENT
    &Unit::HandleNULLProc,                                  // 89 SPELL_AURA_PERIODIC_DAMAGE_PERCENT
    &Unit::HandleNULLProc,                                  // 90 unused (3.0.8a-4.3.4) old SPELL_AURA_MOD_RESIST_CHANCE
    &Unit::HandleNULLProc,                                  // 91 SPELL_AURA_MOD_DETECT_RANGE
    &Unit::HandleNULLProc,                                  // 92 SPELL_AURA_PREVENTS_FLEEING
    &Unit::HandleNULLProc,                                  // 93 SPELL_AURA_MOD_UNATTACKABLE
    &Unit::HandleNULLProc,                                  // 94 SPELL_AURA_INTERRUPT_REGEN
    &Unit::HandleNULLProc,                                  // 95 SPELL_AURA_GHOST
    &Unit::HandleSpellMagnetAuraProc,                       // 96 SPELL_AURA_SPELL_MAGNET
    &Unit::HandleManaShieldAuraProc,                        // 97 SPELL_AURA_MANA_SHIELD
    &Unit::HandleNULLProc,                                  // 98 SPELL_AURA_MOD_SKILL_TALENT
    &Unit::HandleNULLProc,                                  // 99 SPELL_AURA_MOD_ATTACK_POWER
    &Unit::HandleNULLProc,                                  //100 SPELL_AURA_AURAS_VISIBLE obsolete 3.x? all player can see all auras now, but still have 2 spells including GM-spell (1852,2855)
    &Unit::HandleNULLProc,                                  //101 SPELL_AURA_MOD_RESISTANCE_PCT
    &Unit::HandleNULLProc,                                  //102 SPELL_AURA_MOD_MELEE_ATTACK_POWER_VERSUS
    &Unit::HandleNULLProc,                                  //103 SPELL_AURA_MOD_TOTAL_THREAT
    &Unit::HandleNULLProc,                                  //104 SPELL_AURA_WATER_WALK
    &Unit::HandleNULLProc,                                  //105 SPELL_AURA_FEATHER_FALL
    &Unit::HandleNULLProc,                                  //106 SPELL_AURA_HOVER
    &Unit::HandleAddFlatModifierAuraProc,                   //107 SPELL_AURA_ADD_FLAT_MODIFIER
    &Unit::HandleAddPctModifierAuraProc,                    //108 SPELL_AURA_ADD_PCT_MODIFIER
    &Unit::HandleNULLProc,                                  //109 SPELL_AURA_ADD_TARGET_TRIGGER
    &Unit::HandleNULLProc,                                  //110 SPELL_AURA_MOD_POWER_REGEN_PERCENT
    &Unit::HandleNULLProc,                                  //111 SPELL_AURA_ADD_CASTER_HIT_TRIGGER
    &Unit::HandleOverrideClassScriptAuraProc,               //112 SPELL_AURA_OVERRIDE_CLASS_SCRIPTS
    &Unit::HandleNULLProc,                                  //113 SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN
    &Unit::HandleNULLProc,                                  //114 SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN_PCT
    &Unit::HandleNULLProc,                                  //115 SPELL_AURA_MOD_HEALING
    &Unit::HandleNULLProc,                                  //116 SPELL_AURA_MOD_REGEN_DURING_COMBAT
    &Unit::HandleMechanicImmuneResistanceAuraProc,          //117 SPELL_AURA_MOD_MECHANIC_RESISTANCE
    &Unit::HandleNULLProc,                                  //118 SPELL_AURA_MOD_HEALING_PCT
    &Unit::HandleNULLProc,                                  //119 unused (3.0.8a-4.3.4) old SPELL_AURA_SHARE_PET_TRACKING
    &Unit::HandleNULLProc,                                  //120 SPELL_AURA_UNTRACKABLE
    &Unit::HandleNULLProc,                                  //121 SPELL_AURA_EMPATHY
    &Unit::HandleNULLProc,                                  //122 SPELL_AURA_MOD_OFFHAND_DAMAGE_PCT
    &Unit::HandleNULLProc,                                  //123 SPELL_AURA_MOD_TARGET_RESISTANCE
    &Unit::HandleNULLProc,                                  //124 SPELL_AURA_MOD_RANGED_ATTACK_POWER
    &Unit::HandleNULLProc,                                  //125 SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN
    &Unit::HandleNULLProc,                                  //126 SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN_PCT
    &Unit::HandleNULLProc,                                  //127 SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS
    &Unit::HandleNULLProc,                                  //128 SPELL_AURA_MOD_POSSESS_PET
    &Unit::HandleNULLProc,                                  //129 SPELL_AURA_MOD_SPEED_ALWAYS
    &Unit::HandleNULLProc,                                  //130 SPELL_AURA_MOD_MOUNTED_SPEED_ALWAYS
    &Unit::HandleNULLProc,                                  //131 SPELL_AURA_MOD_RANGED_ATTACK_POWER_VERSUS
    &Unit::HandleNULLProc,                                  //132 SPELL_AURA_MOD_INCREASE_ENERGY_PERCENT
    &Unit::HandleNULLProc,                                  //133 SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT
    &Unit::HandleNULLProc,                                  //134 SPELL_AURA_MOD_MANA_REGEN_INTERRUPT
    &Unit::HandleNULLProc,                                  //135 SPELL_AURA_MOD_HEALING_DONE
    &Unit::HandleNULLProc,                                  //136 SPELL_AURA_MOD_HEALING_DONE_PERCENT
    &Unit::HandleNULLProc,                                  //137 SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE
    &Unit::HandleHasteAuraProc,                             //138 SPELL_AURA_MOD_MELEE_HASTE
    &Unit::HandleNULLProc,                                  //139 SPELL_AURA_FORCE_REACTION
    &Unit::HandleNULLProc,                                  //140 SPELL_AURA_MOD_RANGED_HASTE
    &Unit::HandleNULLProc,                                  //141 SPELL_AURA_MOD_RANGED_AMMO_HASTE
    &Unit::HandleNULLProc,                                  //142 SPELL_AURA_MOD_BASE_RESISTANCE_PCT
    &Unit::HandleNULLProc,                                  //143 SPELL_AURA_MOD_RESISTANCE_EXCLUSIVE
    &Unit::HandleNULLProc,                                  //144 SPELL_AURA_SAFE_FALL
    &Unit::HandleNULLProc,                                  //145 SPELL_AURA_MOD_PET_TALENT_POINTS
    &Unit::HandleNULLProc,                                  //146 SPELL_AURA_ALLOW_TAME_PET_TYPE
    &Unit::HandleNULLProc,                                  //147 SPELL_AURA_MECHANIC_IMMUNITY_MASK
    &Unit::HandleNULLProc,                                  //148 SPELL_AURA_RETAIN_COMBO_POINTS
    &Unit::HandleCantTrigger,                               //149 SPELL_AURA_REDUCE_PUSHBACK
    &Unit::HandleNULLProc,                                  //150 SPELL_AURA_MOD_SHIELD_BLOCKDAMAGE
    &Unit::HandleNULLProc,                                  //151 SPELL_AURA_TRACK_STEALTHED
    &Unit::HandleNULLProc,                                  //152 SPELL_AURA_MOD_DETECTED_RANGE
    &Unit::HandleNULLProc,                                  //153 old SPELL_AURA_SPLIT_DAMAGE_FLAT
    &Unit::HandleNULLProc,                                  //154 SPELL_AURA_MOD_STEALTH_LEVEL
    &Unit::HandleNULLProc,                                  //155 SPELL_AURA_MOD_WATER_BREATHING
    &Unit::HandleNULLProc,                                  //156 SPELL_AURA_MOD_REPUTATION_GAIN
    &Unit::HandleNULLProc,                                  //157 SPELL_AURA_PET_DAMAGE_MULTI (single test like spell 20782, also single for 214 aura)
    &Unit::HandleNULLProc,                                  //158 SPELL_AURA_MOD_SHIELD_BLOCKVALUE
    &Unit::HandleNULLProc,                                  //159 SPELL_AURA_NO_PVP_CREDIT
    &Unit::HandleNULLProc,                                  //160 old SPELL_AURA_MOD_AOE_AVOIDANCE
    &Unit::HandleNULLProc,                                  //161 SPELL_AURA_MOD_HEALTH_REGEN_IN_COMBAT
    &Unit::HandleNULLProc,                                  //162 SPELL_AURA_POWER_BURN_ENERGY
    &Unit::HandleNULLProc,                                  //163 SPELL_AURA_MOD_CRIT_DAMAGE_BONUS
    &Unit::HandleNULLProc,                                  //164 2 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //165 SPELL_AURA_MELEE_ATTACK_POWER_ATTACKER_BONUS
    &Unit::HandleNULLProc,                                  //166 SPELL_AURA_MOD_ATTACK_POWER_PCT
    &Unit::HandleNULLProc,                                  //167 SPELL_AURA_MOD_RANGED_ATTACK_POWER_PCT
    &Unit::HandleNULLProc,                                  //168 SPELL_AURA_MOD_DAMAGE_DONE_VERSUS
    &Unit::HandleNULLProc,                                  //169 old SPELL_AURA_MOD_CRIT_PERCENT_VERSUS
    &Unit::HandleNULLProc,                                  //170 SPELL_AURA_DETECT_AMORE       different spells that ignore transformation effects
    &Unit::HandleNULLProc,                                  //171 SPELL_AURA_MOD_SPEED_NOT_STACK
    &Unit::HandleNULLProc,                                  //172 SPELL_AURA_MOD_MOUNTED_SPEED_NOT_STACK
    &Unit::HandleNULLProc,                                  //173 unused (3.0.8a-4.3.4) no spells, old SPELL_AURA_ALLOW_CHAMPION_SPELLS  only for Proclaim Champion spell
    &Unit::HandleNULLProc,                                  //174 SPELL_AURA_MOD_SPELL_DAMAGE_OF_STAT_PERCENT
    &Unit::HandleNULLProc,                                  //175 SPELL_AURA_MOD_SPELL_HEALING_OF_STAT_PERCENT
    &Unit::HandleNULLProc,                                  //176 SPELL_AURA_SPIRIT_OF_REDEMPTION   only for Spirit of Redemption spell, die at aura end
    &Unit::HandleNULLProc,                                  //177 SPELL_AURA_AOE_CHARM (22 spells)
    &Unit::HandleNULLProc,                                  //178 old SPELL_AURA_MOD_DEBUFF_RESISTANCE
    &Unit::HandleNULLProc,                                  //179 SPELL_AURA_MOD_ATTACKER_SPELL_CRIT_CHANCE
    &Unit::HandleNULLProc,                                  //180 SPELL_AURA_MOD_FLAT_SPELL_DAMAGE_VERSUS
    &Unit::HandleNULLProc,                                  //181 unused (3.0.8a-4.3.4) old SPELL_AURA_MOD_FLAT_SPELL_CRIT_DAMAGE_VERSUS
    &Unit::HandleNULLProc,                                  //182 SPELL_AURA_MOD_RESISTANCE_OF_STAT_PERCENT
    &Unit::HandleNULLProc,                                  //183 SPELL_AURA_MOD_CRITICAL_THREAT only used in 28746
    &Unit::HandleNULLProc,                                  //184 SPELL_AURA_MOD_ATTACKER_MELEE_HIT_CHANCE
    &Unit::HandleNULLProc,                                  //185 SPELL_AURA_MOD_ATTACKER_RANGED_HIT_CHANCE
    &Unit::HandleNULLProc,                                  //186 SPELL_AURA_MOD_ATTACKER_SPELL_HIT_CHANCE
    &Unit::HandleNULLProc,                                  //187 SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_CHANCE
    &Unit::HandleNULLProc,                                  //188 SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_CHANCE
    &Unit::HandleModRating,                                 //189 SPELL_AURA_MOD_RATING
    &Unit::HandleNULLProc,                                  //190 SPELL_AURA_MOD_FACTION_REPUTATION_GAIN
    &Unit::HandleNULLProc,                                  //191 SPELL_AURA_USE_NORMAL_MOVEMENT_SPEED
    &Unit::HandleNULLProc,                                  //192 SPELL_AURA_HASTE_MELEE
    &Unit::HandleHasteAllProc,                              //193 SPELL_AURA_HASTE_ALL (in fact combat (any type attack) speed pct)
    &Unit::HandleNULLProc,                                  //194 SPELL_AURA_MOD_IGNORE_ABSORB_SCHOOL
    &Unit::HandleNULLProc,                                  //195 SPELL_AURA_MOD_IGNORE_ABSORB_FOR_SPELL
    &Unit::HandleNULLProc,                                  //196 SPELL_AURA_MOD_COOLDOWN (single spell 24818 in 3.2.2a)
    &Unit::HandleNULLProc,                                  //197 SPELL_AURA_MOD_ATTACKER_SPELL_AND_WEAPON_CRIT_CHANCEe
    &Unit::HandleNULLProc,                                  //198 unused (3.0.8a-4.3.4) old SPELL_AURA_MOD_ALL_WEAPON_SKILLS
    &Unit::HandleNULLProc,                                  //199 old SPELL_AURA_MOD_INCREASES_SPELL_PCT_TO_HIT
    &Unit::HandleNULLProc,                                  //200 SPELL_AURA_MOD_KILL_XP_PCT
    &Unit::HandleNULLProc,                                  //201 SPELL_AURA_FLY                             this aura enable flight mode...
    &Unit::HandleNULLProc,                                  //202 SPELL_AURA_CANNOT_BE_DODGED
    &Unit::HandleNULLProc,                                  //203 SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_DAMAGE
    &Unit::HandleNULLProc,                                  //204 SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_DAMAGE
    &Unit::HandleNULLProc,                                  //205 SPELL_AURA_MOD_ATTACKER_SPELL_CRIT_DAMAGE
    &Unit::HandleNULLProc,                                  //206 SPELL_AURA_MOD_FLIGHT_SPEED
    &Unit::HandleNULLProc,                                  //207 SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED
    &Unit::HandleNULLProc,                                  //208 SPELL_AURA_MOD_FLIGHT_SPEED_STACKING
    &Unit::HandleNULLProc,                                  //209 SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED_STACKING
    &Unit::HandleNULLProc,                                  //210 SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACKING
    &Unit::HandleNULLProc,                                  //211 SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED_NOT_STACKING
    &Unit::HandleNULLProc,                                  //212 old SPELL_AURA_MOD_RANGED_ATTACK_POWER_OF_STAT_PERCENT
    &Unit::HandleNULLProc,                                  //213 SPELL_AURA_MOD_RAGE_FROM_DAMAGE_DEALT implemented in Player::RewardRage
    &Unit::HandleNULLProc,                                  //214 Tamed Pet Passive (single test like spell 20782, also single for 157 aura)
    &Unit::HandleNULLProc,                                  //215 SPELL_AURA_ARENA_PREPARATION
    &Unit::HandleNULLProc,                                  //216 SPELL_AURA_HASTE_SPELLS
    &Unit::HandleNULLProc,                                  //217 SPELL_AURA_MOD_MELEE_HASTE_2
    &Unit::HandleNULLProc,                                  //218 SPELL_AURA_HASTE_RANGED
    &Unit::HandleNULLProc,                                  //219 SPELL_AURA_MOD_MANA_REGEN_FROM_STAT
    &Unit::HandleNULLProc,                                  //220 SPELL_AURA_MOD_RATING_FROM_STAT
    &Unit::HandleNULLProc,                                  //221 SPELL_AURA_IGNORED 5 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //222 2 spells in 4.3.4 Prayer of Mending
    &Unit::HandleNULLProc,                                  //223 dummy code (cast damage spell to attacker) and another dummy (jump to another nearby raid member)
    &Unit::HandleNULLProc,                                  //224 unused (3.0.8a-4.3.4)
    &Unit::HandleMendingAuraProc,                           //225 SPELL_AURA_PRAYER_OF_MENDING
    &Unit::HandlePeriodicDummyAuraProc,                     //226 SPELL_AURA_PERIODIC_DUMMY
    &Unit::HandleNULLProc,                                  //227 SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE
    &Unit::HandleNULLProc,                                  //228 SPELL_AURA_DETECT_STEALTH
    &Unit::HandleNULLProc,                                  //229 SPELL_AURA_MOD_AOE_DAMAGE_AVOIDANCE
    &Unit::HandleNULLProc,                                  //230 Commanding Shout
    &Unit::HandleProcTriggerSpellAuraProc,                  //231 SPELL_AURA_PROC_TRIGGER_SPELL_WITH_VALUE
    &Unit::HandleNULLProc,                                  //232 SPELL_AURA_MECHANIC_DURATION_MOD
    &Unit::HandleNULLProc,                                  //233 set model id to the one of the creature with id m_modifier.m_miscvalue
    &Unit::HandleNULLProc,                                  //234 SPELL_AURA_MECHANIC_DURATION_MOD_NOT_STACK
    &Unit::HandleNULLProc,                                  //235 SPELL_AURA_MOD_DISPEL_RESIST
    &Unit::HandleNULLProc,                                  //236 SPELL_AURA_CONTROL_VEHICLE
    &Unit::HandleNULLProc,                                  //237 SPELL_AURA_MOD_SPELL_DAMAGE_OF_ATTACK_POWER
    &Unit::HandleNULLProc,                                  //238 SPELL_AURA_MOD_SPELL_HEALING_OF_ATTACK_POWER
    &Unit::HandleNULLProc,                                  //239 SPELL_AURA_MOD_SCALE_2 only in Noggenfogger Elixir (16595) before 2.3.0 aura 61
    &Unit::HandleNULLProc,                                  //240 SPELL_AURA_MOD_EXPERTISE
    &Unit::HandleNULLProc,                                  //241 Forces the player to move forward
    &Unit::HandleNULLProc,                                  //242 SPELL_AURA_MOD_SPELL_DAMAGE_FROM_HEALING (only 2 test spels in 3.2.2a)
    &Unit::HandleNULLProc,                                  //243 faction reaction override spells
    &Unit::HandleNULLProc,                                  //244 SPELL_AURA_COMPREHEND_LANGUAGE
    &Unit::HandleNULLProc,                                  //245 SPELL_AURA_MOD_DURATION_OF_MAGIC_EFFECTS
    &Unit::HandleNULLProc,                                  //246 SPELL_AURA_MOD_DURATION_OF_EFFECTS_BY_DISPEL
    &Unit::HandleNULLProc,                                  //247 target to become a clone of the caster
    &Unit::HandleNULLProc,                                  //248 SPELL_AURA_MOD_COMBAT_RESULT_CHANCE
    &Unit::HandleNULLProc,                                  //249 SPELL_AURA_CONVERT_RUNE
    &Unit::HandleNULLProc,                                  //250 SPELL_AURA_MOD_INCREASE_HEALTH_2
    &Unit::HandleNULLProc,                                  //251 SPELL_AURA_MOD_ENEMY_DODGE
    &Unit::HandleNULLProc,                                  //252 SPELL_AURA_SLOW_ALL
    &Unit::HandleNULLProc,                                  //253 SPELL_AURA_MOD_BLOCK_CRIT_CHANCE
    &Unit::HandleNULLProc,                                  //254 SPELL_AURA_MOD_DISARM_OFFHAND disarm Shield
    &Unit::HandleNULLProc,                                  //255 SPELL_AURA_MOD_MECHANIC_DAMAGE_TAKEN_PERCENT
    &Unit::HandleNULLProc,                                  //256 SPELL_AURA_NO_REAGENT_USE Use SpellClassMask for spell select
    &Unit::HandleNULLProc,                                  //257 SPELL_AURA_MOD_TARGET_RESIST_BY_SPELL_CLASS Use SpellClassMask for spell select
    &Unit::HandleNULLProc,                                  //258 SPELL_AURA_MOD_SPELL_VISUAL
    &Unit::HandleNULLProc,                                  //259 corrupt healing over time spell
    &Unit::HandleNULLProc,                                  //260 SPELL_AURA_SCREEN_EFFECT (miscvalue = id in ScreenEffect.dbc) not required any code
    &Unit::HandleNULLProc,                                  //261 SPELL_AURA_PHASE undetectable invisibility?
    &Unit::HandleIgnoreUnitStateAuraProc,                   //262 SPELL_AURA_IGNORE_UNIT_STATE
    &Unit::HandleNULLProc,                                  //263 SPELL_AURA_ALLOW_ONLY_ABILITY player can use only abilities set in SpellClassMask
    &Unit::HandleNULLProc,                                  //264 1 spell in 4.3.4 Deterrence
    &Unit::HandleNULLProc,                                  //265 unused (3.0.8a-4.3.4)
    &Unit::HandleNULLProc,                                  //266 unused (3.0.8a-4.3.4)
    &Unit::HandleNULLProc,                                  //267 SPELL_AURA_MOD_IMMUNE_AURA_APPLY_SCHOOL
    &Unit::HandleNULLProc,                                  //268 SPELL_AURA_MOD_ATTACK_POWER_OF_STAT_PERCENT
    &Unit::HandleNULLProc,                                  //269 SPELL_AURA_MOD_IGNORE_DAMAGE_REDUCTION_SCHOOL
    &Unit::HandleNULLProc,                                  //270 SPELL_AURA_MOD_IGNORE_TARGET_RESIST (unused in 3.2.2a)
    &Unit::HandleModDamageFromCasterAuraProc,               //271 SPELL_AURA_MOD_DAMAGE_FROM_CASTER
    &Unit::HandleCantTrigger,                               //272 SPELL_AURA_IGNORE_MELEE_RESET (unclear use for aura, it used in (3.2.2a...3.3.0) in single spell 53817 that spellmode stacked and charged spell expected to be drop as stack
    &Unit::HandleNULLProc,                                  //273 SPELL_AURA_X_RAY (client side implementation)
    &Unit::HandleNULLProc,                                  //274 proc free shot?
    &Unit::HandleNULLProc,                                  //275 SPELL_AURA_MOD_IGNORE_SHAPESHIFT Use SpellClassMask for spell select
    &Unit::HandleNULLProc,                                  //276 SPELL_AURA_MOD_DAMAGE_DONE_BY_MECHANIC
    &Unit::HandleNULLProc,                                  //277 old SPELL_AURA_MOD_MAX_AFFECTED_TARGETS
    &Unit::HandleNULLProc,                                  //278 SPELL_AURA_MOD_DISARM_RANGED disarm ranged weapon
    &Unit::HandleNULLProc,                                  //279 SPELL_AURA_INITIALIZE_IMAGES 9 spells in 4.3.4 visual effects?
    &Unit::HandleNULLProc,                                  //280 old SPELL_AURA_MOD_TARGET_ARMOR_PCT
    &Unit::HandleNULLProc,                                  //281 SPELL_AURA_MOD_GUILD_REPUTATION_GAIN
    &Unit::HandleNULLProc,                                  //282 SPELL_AURA_INCREASE_BASE_HEALTH_PERCENT
    &Unit::HandleNULLProc,                                  //283 SPELL_AURA_MOD_HEALING_RECEIVED
    &Unit::HandleNULLProc,                                  //284 146 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //285 SPELL_AURA_MOD_ATTACK_POWER_OF_ARMOR
    &Unit::HandleNULLProc,                                  //286 SPELL_AURA_ABILITY_PERIODIC_CRIT
    &Unit::HandleNULLProc,                                  //287 SPELL_AURA_DEFLECT_SPELLS
    &Unit::HandleNULLProc,                                  //288 increase parry/deflect, prevent attack (single spell used 67801)
    &Unit::HandleNULLProc,                                  //289 1 spell in 4.3.4 Mortality
    &Unit::HandleNULLProc,                                  //290 SPELL_AURA_MOD_ALL_CRIT_CHANCE
    &Unit::HandleNULLProc,                                  //291 SPELL_AURA_MOD_QUEST_XP_PCT
    &Unit::HandleNULLProc,                                  //292 call stabled pet
    &Unit::HandleNULLProc,                                  //293 3 spells
    &Unit::HandleNULLProc,                                  //294 2 spells, possible prevent mana regen
    &Unit::HandleNULLProc,                                  //295 unused (4.3.4)
    &Unit::HandleNULLProc,                                  //SPELL_AURA_SET_VEHICLE_ID 296 62 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //297 14 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //298 6 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //299 unused (3.2.2a-4.3.4)
    &Unit::HandleNULLProc,                                  //300 21 spells (share damage?)
    &Unit::HandleNULLProc,                                  //301 SPELL_AURA_HEAL_ABSORB 31 spells
    &Unit::HandleNULLProc,                                  //302 unused (3.2.2a-4.3.4)
    &Unit::HandleNULLProc,                                  //303 AURA_STATE_BLEEDING 35 spells increases damage done vs. aurastate
    &Unit::HandleNULLProc,                                  //304 SPELL_AURA_FAKE_INEBRIATE
    &Unit::HandleNULLProc,                                  //305 SPELL_AURA_MOD_MINIMUM_SPEED
    &Unit::HandleNULLProc,                                  //306 0 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //307 0 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //308 SPELL_AURA_MOD_CRIT_FROM_CASTER
    &Unit::HandleNULLProc,                                  //309 0 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //310 SPELL_AURA_MOD_PET_AOE_DAMAGE_AVOIDANCE
    &Unit::HandleNULLProc,                                  //311 2 spells in 4.3.4 some kind of stun effect
    &Unit::HandleNULLProc,                                  //312 37 spells in 4.3.4 some cosmetic auras
    &Unit::HandleNULLProc,                                  //313  0 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //314  SPELL_AURA_PREVENT_RESURRECTION 2 spells int 4.3.4 prevents ressurection ?
    &Unit::HandleNULLProc,                                  //315 SPELL_AURA_UNDERWATER_WALKING 4 spells in 4.3.4 underwater walking
    &Unit::HandleNULLProc,                                  //316 old SPELL_AURA_MOD_PERIODIC_HASTE 0 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //317 SPELL_AURA_MOD_INCREASE_SPELL_POWER_PCT 13 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //318 SPELL_AURA_MASTERY 12 spells in 4.3
    &Unit::HandleNULLProc,                                  //319 SPELL_AURA_MOD_MELEE_HASTE_3 47 spells in 4.3.4
    &Unit::HandleModRangedHasteAuraProc,                    //320 SPELL_AURA_MOD_RANGED_HASTE_2 5 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //321 1 spells in 4.3 Hex
    &Unit::HandleNULLProc,                                  //322 SPELL_AURA_INTERFERE_TARGETING 6 spells in 4.3
    &Unit::HandleNULLProc,                                  //323 0 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //324 2 spells in 4.3.4 test spells
    &Unit::HandleNULLProc,                                  //325 0 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //326 SPELL_AURA_PHASE_2 24 spells in 4.3.4 new phase auras
    &Unit::HandleNULLProc,                                  //327 0 spells in 4.3.4
    &Unit::HandleAuraProcOnPowerAmount,                     //328 SPELL_AURA_PROC_ON_TARGET_AMOUNT 2 spells in 4.3.4 Eclipse Mastery Driver Passive
    &Unit::HandleNULLProc,                                  //329 SPELL_AURA_MOD_RUNIC_POWER_GAIN 3 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //330 SPELL_AURA_ALLOW_CAST_WHILE_MOVING 16 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //331 SPELL_AURA_MOD_WEATHER 10 spells in 4.3.4
    &Unit::HandleSpellAuraOverrideActionbarSpellsProc,      //332 SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS 16 spells in 4.3.4
    &Unit::HandleSpellAuraOverrideActionbarSpellsProc,      //333 SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS_2 10 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //334 SPELL_AURA_BLIND_SIGHT 2 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //335 SPELL_AURA_SEE_WHILE_INVISIBLE implemented in Unit::isVisibleForOrDetect 5 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //336 SPELL_AURA_FLIGHT_RESTRICTIONS 8 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //337 SPELL_AURA_MOD_VENDOR_PRICE 1 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //338 SPELL_AURA_MOD_DURABILITY_LOSS 3 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //339 SPELL_AURA_MOD_SKILLCHANCE 1 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //340 SPELL_AURA_MOD_RESURRECTED_HEALTH_BY_GUILD_MEMBER 1 spells in 4.3.4, implemented in Spell::EffectResurrect and Spell::EffectResurrectNew
    &Unit::HandleNULLProc,                                  //341 SPELL_AURA_MOD_SPELL_CATEGORY_COOLDOWN 2 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //342 SPELL_AURA_MOD_ATTACKSPEED_2 17 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //343 SPELL_AURA_MOD_ALL_DAMAGE_FROM_CASTER 2 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //344 SPELL_AURA_MOD_AUTOATTACK_DAMAGE 6 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //345 SPELL_AURA_MOD_IGNORE_ARMOR_PCT 3 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //346 SPELL_AURA_ALT_POWER_INDICATOR 32 spells in 4.3.4 shows progressbar-like ui?
    &Unit::HandleNULLProc,                                  //347 SPELL_AURA_MOD_CD_FROM_HASTE 2 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //348 SPELL_AURA_MOD_MONEY_TO_GUILD_BANK 2 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //349 SPELL_AURA_MOD_CURRENCY_GAIN 16 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //350 SPELL_AURA_MOD_ITEM_LOOT 1 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //351 SPELL_AURA_MOD_CURRENCY_LOOT 8 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //352 SPELL_AURA_ALLOW_WORGEN_TRANSFORM 1 spells in 4.3.4 enables worgen<>human form switches
    &Unit::HandleNULLProc,                                  //353 SPELL_AURA_CAMOUFLAGE 3 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //354 SPELL_AURA_MOD_HEALING_DONE_FROM_PCT_HEALTH 2 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //355 0 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //356 SPELL_AURA_MOD_DAMAGE_DONE_FROM_PCT_POWER 2 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //357 1 spells in 4.3.4 Enable Tower Assault Unit Frame
    &Unit::HandleNULLProc,                                  //358 SPELL_AURA_WORGEN_TRANSFORM 5 spells in 4.3.4 forces client-side transformation to worgen form
    &Unit::HandleNULLProc,                                  //359 1 spells in 4.3.4 healing done vs aurastate?
    &Unit::HandleNULLProc,                                  //360 SPELL_AURA_PROC_DUPLICATE_SPELLS 2 spells in 4.3.4
    &Unit::HandleProcTriggerSpellAuraProc,                  //361 SPELL_AURA_PROC_TRIGGER_SPELL_2 4 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //362 0 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //363 1 spells in 4.3.4 Throw Totem
    &Unit::HandleNULLProc,                                  //364 0 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //365 1 spells in 4.3.4 Max Far Clip Plane
    &Unit::HandleNULLProc,                                  //366 SPELL_AURA_OVERRIDE_SPELL_POWER_BY_AP_PCT 1 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //367 2 spells in 4.3.4 test spells
    &Unit::HandleNULLProc,                                  //368 0 spells in 4.3.4
    &Unit::HandleNULLProc,                                  //369 5 spells in 4.3.4 darkmoon faire related
    &Unit::HandleNULLProc                                   //370 1 spells in 4.3.4 Fair Far Clip
};

bool Unit::IsTriggeredAtSpellProcEvent(Unit *pVictim, SpellAuraHolder* holder, SpellEntry const* procSpell, uint32 procFlag, uint32 procExtra, WeaponAttackType attType, bool isVictim, SpellProcEventEntry const*& spellProcEvent )
{
    SpellEntry const* spellProto = holder->GetSpellProto ();

    // Get proc Event Entry
    spellProcEvent = sSpellMgr.GetSpellProcEvent(spellProto->Id);

    // Get EventProcFlag
    uint32 EventProcFlag;
    if (spellProcEvent && spellProcEvent->procFlags)        // if exist get custom spellProcEvent->procFlags
        EventProcFlag = spellProcEvent->procFlags;
    else
        EventProcFlag = spellProto->GetProcFlags();         // else get from spell proto
    // Continue if no trigger exist
    if (!EventProcFlag)
        return false;

    // Check spellProcEvent data requirements
    if(!SpellMgr::IsSpellProcEventCanTriggeredBy(spellProcEvent, EventProcFlag, procSpell, procFlag, procExtra))
        return false;

    // In most cases req get honor or XP from kill
    if (EventProcFlag & PROC_FLAG_KILL && GetTypeId() == TYPEID_PLAYER)
    {
        bool allow = ((Player*)this)->isHonorOrXPTarget(pVictim);
        // Shadow Word: Death - can trigger from every kill
        if (holder->GetId() == 32409)
            allow = true;
        if (!allow)
            return false;
    }
    // Aura added by spell can`t trigger from self (prevent drop charges/do triggers)
    // But except periodic triggers (can triggered from self)
    if (procSpell && procSpell->Id == spellProto->Id && !(EventProcFlag & PROC_FLAG_ON_TAKE_PERIODIC))
        return false;

    // Check if current equipment allows aura to proc
    if (!isVictim && GetTypeId() == TYPEID_PLAYER)
    {
        if (SpellEquippedItemsEntry const* eqItems = spellProto->GetSpellEquippedItems())
        {
            if (eqItems->EquippedItemClass == ITEM_CLASS_WEAPON)
            {
                Item *item = NULL;
                if (attType == BASE_ATTACK)
                    item = ((Player*)this)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND);
                else if (attType == OFF_ATTACK)
                    item = ((Player*)this)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
                else
                    item = ((Player*)this)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED);

                if (!item || item->IsBroken() || !CanUseEquippedWeapon(attType) || item->GetProto()->Class != ITEM_CLASS_WEAPON || !((1<<item->GetProto()->SubClass) & eqItems->EquippedItemSubClassMask))
                    return false;
            }
            else if(eqItems->EquippedItemClass == ITEM_CLASS_ARMOR)
            {
                // Check if player is wearing shield
                Item *item = ((Player*)this)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND);
                if (!item || item->IsBroken() || !CanUseEquippedWeapon(OFF_ATTACK) || item->GetProto()->Class != ITEM_CLASS_ARMOR || !((1<<item->GetProto()->SubClass) & eqItems->EquippedItemSubClassMask))
                    return false;
            }
        }
    }
    // Get chance from spell
    float chance = (float)spellProto->GetProcChance();

    // If in spellProcEvent exist custom chance, chance = spellProcEvent->customChance;
    if(spellProcEvent && spellProcEvent->customChance)
        chance = spellProcEvent->customChance;

    // If PPM exist calculate chance from PPM
    if (spellProcEvent && spellProcEvent->ppmRate != 0)
    {
        if (!isVictim)
        {
            uint32 WeaponSpeed = GetAttackTime(attType);
            chance = GetPPMProcChance(WeaponSpeed, spellProcEvent->ppmRate);
        }
        else
        {
            uint32 WeaponSpeed = pVictim->GetAttackTime(attType);
            chance = pVictim->GetPPMProcChance(WeaponSpeed, spellProcEvent->ppmRate);
        }

        if (Player* modOwner = GetSpellModOwner())
            modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_FREQUENCY_OF_SUCCESS, chance);
    }

    // Apply chance modifier aura
    if(Player* modOwner = GetSpellModOwner())
        modOwner->ApplySpellMod(spellProto->Id, SPELLMOD_CHANCE_OF_SUCCESS, chance);

    // Earthliving procs more often with Blessing of the Eternals on low hp targets
    if (spellProto->Id == 52007 && pVictim->GetHealthPercent() < 35.0f)
    {
        // Search Blessing of the Eternals
        Unit::AuraList const& dummyAuras = GetAurasByType(SPELL_AURA_DUMMY);
        for (Unit::AuraList::const_iterator itr = dummyAuras.begin(); itr != dummyAuras.end(); ++itr)
            if ((*itr)->GetSpellProto()->SpellIconID == 3157 && (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_SHAMAN)
            {
                chance += (*itr)->GetModifier()->m_amount;
                break;
            }
    }

    return roll_chance_f(chance);
}

SpellAuraProcResult Unit::HandleHasteAuraProc(Unit *pVictim, uint32 damage, uint32 absorb, Aura* triggeredByAura, SpellEntry const * /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 cooldown)
{
    SpellEntry const *hasteSpell = triggeredByAura->GetSpellProto();

    Item* castItem = triggeredByAura->GetCastItemGuid() && GetTypeId()==TYPEID_PLAYER
        ? ((Player*)this)->GetItemByGuid(triggeredByAura->GetCastItemGuid()) : NULL;

    uint32 triggered_spell_id = 0;
    Unit* target = pVictim;
    int32 basepoints0 = 0;

    switch(hasteSpell->GetSpellFamilyName())
    {
        case SPELLFAMILY_ROGUE:
        {
            switch(hasteSpell->Id)
            {
                // Blade Flurry
                case 13877:
                case 33735:
                {
                    target = SelectRandomUnfriendlyTarget(pVictim);
                    if(!target)
                        return SPELL_AURA_PROC_FAILED;
                    basepoints0 = damage;
                    triggered_spell_id = 22482;
                    break;
                }
            }
            break;
        }
    }

    // processed charge only counting case
    if(!triggered_spell_id)
        return SPELL_AURA_PROC_OK;

    SpellEntry const* triggerEntry = sSpellStore.LookupEntry(triggered_spell_id);

    if(!triggerEntry)
    {
        ERROR_LOG("Unit::HandleHasteAuraProc: Spell %u have nonexistent triggered spell %u",hasteSpell->Id,triggered_spell_id);
        return SPELL_AURA_PROC_FAILED;
    }

    // default case
    if(!target || target!=this && !target->isAlive())
        return SPELL_AURA_PROC_FAILED;

    if( cooldown && GetTypeId()==TYPEID_PLAYER && ((Player*)this)->HasSpellCooldown(triggered_spell_id))
        return SPELL_AURA_PROC_FAILED;

    if(basepoints0)
        CastCustomSpell(target,triggered_spell_id,&basepoints0,NULL,NULL,true,castItem,triggeredByAura);
    else
        CastSpell(target,triggered_spell_id,true,castItem,triggeredByAura);

    if( cooldown && GetTypeId()==TYPEID_PLAYER )
        ((Player*)this)->AddSpellCooldown(triggered_spell_id,0,time(NULL) + cooldown);

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleSpellCritChanceAuraProc(Unit *pVictim, uint32 /*damage*/, uint32 /*absorb*/, Aura* triggeredByAura, SpellEntry const * procSpell, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 cooldown)
{
    if (!procSpell)
        return SPELL_AURA_PROC_FAILED;

    SpellEntry const *triggeredByAuraSpell = triggeredByAura->GetSpellProto();

    Item* castItem = triggeredByAura->GetCastItemGuid() && GetTypeId()==TYPEID_PLAYER
        ? ((Player*)this)->GetItemByGuid(triggeredByAura->GetCastItemGuid()) : NULL;

    uint32 triggered_spell_id = 0;
    Unit* target = pVictim;
    int32 basepoints0 = 0;

    switch(triggeredByAuraSpell->GetSpellFamilyName())
    {
        case SPELLFAMILY_GENERIC:
        {
            // Astral Alignment
            if (triggeredByAura->GetId() == 90164)
            {
                Modifier* mod = triggeredByAura->GetModifier();
                mod -= triggeredByAura->GetSpellEffect()->CalculateSimpleValue();
                if (mod > 0)
                {
                    triggeredByAura->GetHolder()->SendAuraUpdate(false);
                    return SPELL_AURA_PROC_FAILED;
                }
            }
            break;
        }
        case SPELLFAMILY_MAGE:
        {
            switch(triggeredByAuraSpell->Id)
            {
                // Focus Magic
                case 54646:
                {
                    Unit* caster = triggeredByAura->GetCaster();
                    if(!caster)
                        return SPELL_AURA_PROC_FAILED;

                    triggered_spell_id = 54648;
                    target = caster;
                    break;
                }
            }
        }
    }

    // processed charge only counting case
    if(!triggered_spell_id)
        return SPELL_AURA_PROC_OK;

    SpellEntry const* triggerEntry = sSpellStore.LookupEntry(triggered_spell_id);

    if(!triggerEntry)
    {
        ERROR_LOG("Unit::HandleHasteAuraProc: Spell %u have nonexistent triggered spell %u",triggeredByAuraSpell->Id,triggered_spell_id);
        return SPELL_AURA_PROC_FAILED;
    }

    // default case
    if(!target || target!=this && !target->isAlive())
        return SPELL_AURA_PROC_FAILED;

    if( cooldown && GetTypeId()==TYPEID_PLAYER && ((Player*)this)->HasSpellCooldown(triggered_spell_id))
        return SPELL_AURA_PROC_FAILED;

    if(basepoints0)
        CastCustomSpell(target,triggered_spell_id,&basepoints0,NULL,NULL,true,castItem,triggeredByAura);
    else
        CastSpell(target,triggered_spell_id,true,castItem,triggeredByAura);

    if( cooldown && GetTypeId()==TYPEID_PLAYER )
        ((Player*)this)->AddSpellCooldown(triggered_spell_id,0,time(NULL) + cooldown);

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleDummyAuraProc(Unit *pVictim, uint32 damage, uint32 absorb, Aura* triggeredByAura, SpellEntry const * procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown)
{
    SpellEntry const *dummySpell = triggeredByAura->GetSpellProto();
    SpellEffectIndex effIndex = triggeredByAura->GetEffIndex();
    SpellEffectEntry const* dummySpellEffect = dummySpell->GetSpellEffect(effIndex);
    SpellClassOptionsEntry const* dummyClassOptions = dummySpell->GetSpellClassOptions();
    SpellClassOptionsEntry const* procClassOptions = procSpell ? procSpell->GetSpellClassOptions() : NULL;
    int32  triggerAmount = triggeredByAura->GetModifier()->m_amount;

    Item* castItem = triggeredByAura->GetCastItemGuid() && GetTypeId()==TYPEID_PLAYER
        ? ((Player*)this)->GetItemByGuid(triggeredByAura->GetCastItemGuid()) : NULL;

    // some dummy spells have trigger spell in spell data already (from 3.0.3)
    uint32 triggered_spell_id = dummySpellEffect->EffectApplyAuraName == SPELL_AURA_DUMMY ? dummySpellEffect->EffectTriggerSpell : 0;
    Unit* target = pVictim;
    int32  basepoints[MAX_EFFECT_INDEX] = {0, 0, 0};

    switch(dummySpell->GetSpellFamilyName())
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (dummySpell->Id)
            {
                // Eye for an Eye, Rank 1
                case 9799:
                {
                    // return damage % to attacker but < 50% own total health
                    basepoints[0] = triggerAmount * int32(damage) / 100;
                    if (basepoints[0] > int32(GetMaxHealth() / 2))
                        basepoints[0] = int32(GetMaxHealth() / 2);

                    triggered_spell_id = 25997;
                    break;
                }
                // Sweeping Strikes (NPC spells may be)
                case 18765:
                case 35429:
                {
                    // prevent chain of triggered spell from same triggered spell
                    if (procSpell && procSpell->Id == 26654)
                        return SPELL_AURA_PROC_FAILED;

                    target = SelectRandomUnfriendlyTarget(pVictim);
                    if(!target)
                        return SPELL_AURA_PROC_FAILED;

                    triggered_spell_id = 26654;
                    break;
                }
                // Twisted Reflection (boss spell)
                case 21063:
                    triggered_spell_id = 21064;
                    break;
                // Unstable Power
                case 24658:
                {
                    if (!procSpell || procSpell->Id == 24659)
                        return SPELL_AURA_PROC_FAILED;
                    // Need remove one 24659 aura
                    RemoveAuraHolderFromStack(24659);
                    return SPELL_AURA_PROC_OK;
                }
                // Deadly Precision
                case 71563:
                {
                    if (!procSpell || procSpell->Id == 71564)
                        return SPELL_AURA_PROC_FAILED;
                    // Need remove one 24659 aura
                    RemoveAuraHolderFromStack(71564);
                    return SPELL_AURA_PROC_OK;
                }

                // Restless Strength
                case 24661:
                {
                    // Need remove one 24662 aura
                    RemoveAuraHolderFromStack(24662);
                    return SPELL_AURA_PROC_OK;
                }
                // Adaptive Warding (Frostfire Regalia set)
                case 28764:
                {
                    if(!procSpell)
                        return SPELL_AURA_PROC_FAILED;

                    // find Mage Armor
                    bool found = false;
                    AuraList const& mRegenInterrupt = GetAurasByType(SPELL_AURA_MOD_MANA_REGEN_INTERRUPT);
                    for (AuraList::const_iterator iter = mRegenInterrupt.begin(); iter != mRegenInterrupt.end(); ++iter)
                    {
                        if(SpellEntry const* iterSpellProto = (*iter)->GetSpellProto())
                        {
                            SpellClassOptionsEntry const* iterClassOptions = iterSpellProto->GetSpellClassOptions();
                            if(iterClassOptions && iterClassOptions->SpellFamilyName == SPELLFAMILY_MAGE && (iterClassOptions->SpellFamilyFlags & UI64LIT(0x10000000)))
                            {
                                found=true;
                                break;
                            }
                        }
                    }
                    if(!found)
                        return SPELL_AURA_PROC_FAILED;

                    switch(GetFirstSchoolInMask(GetSpellSchoolMask(procSpell)))
                    {
                        case SPELL_SCHOOL_NORMAL:
                        case SPELL_SCHOOL_HOLY:
                            return SPELL_AURA_PROC_FAILED;                   // ignored
                        case SPELL_SCHOOL_FIRE:   triggered_spell_id = 28765; break;
                        case SPELL_SCHOOL_NATURE: triggered_spell_id = 28768; break;
                        case SPELL_SCHOOL_FROST:  triggered_spell_id = 28766; break;
                        case SPELL_SCHOOL_SHADOW: triggered_spell_id = 28769; break;
                        case SPELL_SCHOOL_ARCANE: triggered_spell_id = 28770; break;
                        default:
                            return SPELL_AURA_PROC_FAILED;
                    }

                    target = this;
                    break;
                }
                // Obsidian Armor (Justice Bearer`s Pauldrons shoulder)
                case 27539:
                {
                    if(!procSpell)
                        return SPELL_AURA_PROC_FAILED;

                    switch(GetFirstSchoolInMask(GetSpellSchoolMask(procSpell)))
                    {
                        case SPELL_SCHOOL_NORMAL:
                            return SPELL_AURA_PROC_FAILED;                   // ignore
                        case SPELL_SCHOOL_HOLY:   triggered_spell_id = 27536; break;
                        case SPELL_SCHOOL_FIRE:   triggered_spell_id = 27533; break;
                        case SPELL_SCHOOL_NATURE: triggered_spell_id = 27538; break;
                        case SPELL_SCHOOL_FROST:  triggered_spell_id = 27534; break;
                        case SPELL_SCHOOL_SHADOW: triggered_spell_id = 27535; break;
                        case SPELL_SCHOOL_ARCANE: triggered_spell_id = 27540; break;
                        default:
                            return SPELL_AURA_PROC_FAILED;
                    }

                    target = this;
                    break;
                }
                // Mana Leech (Passive) (Priest Pet Aura)
                case 28305:
                {
                    // Cast on owner
                    target = GetOwner();
                    if (!target)
                        return SPELL_AURA_PROC_FAILED;

                    // Item - Priest T13 Shadow 4P Bonus (Shadowfiend and Shadowy Apparition)
                    if (target->HasAura(105844))
                        for (int i = 0; i < 3; ++i)
                            target->CastSpell(target, 77487, true);

                    triggered_spell_id = 34650;
                    break;
                }
                // Mark of Malice
                case 33493:
                {
                    // Cast finish spell at last charge
                    if (triggeredByAura->GetHolder()->GetAuraCharges() > 1)
                        return SPELL_AURA_PROC_FAILED;

                    target = this;
                    triggered_spell_id = 33494;
                    break;
                }
                // Vampiric Aura (boss spell)
                case 38196:
                {
                    basepoints[0] = 3 * damage;               // 300%
                    if (basepoints[0] < 0)
                        return SPELL_AURA_PROC_FAILED;

                    triggered_spell_id = 31285;
                    target = this;
                    break;
                }
                // Aura of Madness (Darkmoon Card: Madness trinket)
                //=====================================================
                // 39511 Sociopath: +35 strength (Paladin, Rogue, Druid, Warrior)
                // 40997 Delusional: +70 attack power (Rogue, Hunter, Paladin, Warrior, Druid)
                // 40998 Kleptomania: +35 agility (Warrior, Rogue, Paladin, Hunter, Druid)
                // 40999 Megalomania: +41 damage/healing (Druid, Shaman, Priest, Warlock, Mage, Paladin)
                // 41002 Paranoia: +35 spell/melee/ranged crit strike rating (All classes)
                // 41005 Manic: +35 haste (spell, melee and ranged) (All classes)
                // 41009 Narcissism: +35 intellect (Druid, Shaman, Priest, Warlock, Mage, Paladin, Hunter)
                // 41011 Martyr Complex: +35 stamina (All classes)
                // 41406 Dementia: Every 5 seconds either gives you +5% damage/healing. (Druid, Shaman, Priest, Warlock, Mage, Paladin)
                // 41409 Dementia: Every 5 seconds either gives you -5% damage/healing. (Druid, Shaman, Priest, Warlock, Mage, Paladin)
                case 39446:
                {
                    if(GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    // Select class defined buff
                    switch (getClass())
                    {
                        case CLASS_PALADIN:                 // 39511,40997,40998,40999,41002,41005,41009,41011,41409
                        case CLASS_DRUID:                   // 39511,40997,40998,40999,41002,41005,41009,41011,41409
                        {
                            uint32 RandomSpell[]={39511,40997,40998,40999,41002,41005,41009,41011,41409};
                            triggered_spell_id = RandomSpell[urand(0, countof(RandomSpell)-1)];
                            break;
                        }
                        case CLASS_ROGUE:                   // 39511,40997,40998,41002,41005,41011
                        case CLASS_WARRIOR:                 // 39511,40997,40998,41002,41005,41011
                        {
                            uint32 RandomSpell[]={39511,40997,40998,41002,41005,41011};
                            triggered_spell_id = RandomSpell[urand(0, countof(RandomSpell)-1)];
                            break;
                        }
                        case CLASS_PRIEST:                  // 40999,41002,41005,41009,41011,41406,41409
                        case CLASS_SHAMAN:                  // 40999,41002,41005,41009,41011,41406,41409
                        case CLASS_MAGE:                    // 40999,41002,41005,41009,41011,41406,41409
                        case CLASS_WARLOCK:                 // 40999,41002,41005,41009,41011,41406,41409
                        {
                            uint32 RandomSpell[]={40999,41002,41005,41009,41011,41406,41409};
                            triggered_spell_id = RandomSpell[urand(0, countof(RandomSpell)-1)];
                            break;
                        }
                        case CLASS_HUNTER:                  // 40997,40999,41002,41005,41009,41011,41406,41409
                        {
                            uint32 RandomSpell[]={40997,40999,41002,41005,41009,41011,41406,41409};
                            triggered_spell_id = RandomSpell[urand(0, countof(RandomSpell)-1)];
                            break;
                        }
                        default:
                            return SPELL_AURA_PROC_FAILED;
                    }

                    target = this;
                    if (roll_chance_i(10))
                        ((Player*)this)->Say("This is Madness!", LANG_UNIVERSAL);
                    break;
                }
                //Deathbringer's Will (Heroic)
                case 71562:
                //Deathbringer's Will (Normal)
                case 71519:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;
                    uint8 heroic = dummySpell->Id == 71562 ? 1 : 0;

                    //http://www.wowwiki.com/Deathbringer%27s_Will
                    /*
                    
                    Druid: Str, Agi, Haste

                    Hunter: Agi, Crit, AP
                    
                    Paladin: Str, Haste, Crit
                    Warrior: Str, Crit, Haste
                    DK: Str, Haste, Crit

                    Rogue: Agi, Haste, AP
                    Shaman: Agi, AP, Haste
                    */
                    uint32 Str[] = {71484, 71561};
                    uint32 Ap[] = {71486, 71558};
                    uint32 Crit[] = {71491, 71559};
                    uint32 Agi[] = {71485, 71556};
                    uint32 Haste[] = {71492, 71560};
                    uint32 RandomSpell[] = {0, 0, 0};
                    switch (getClass())
                    {
                        case CLASS_DRUID:
                        {
                            RandomSpell[0] = Str[heroic];
                            RandomSpell[1] = Agi[heroic];
                            RandomSpell[2] = Haste[heroic];
                            break;
                        }
                        case CLASS_HUNTER:
                        {
                            RandomSpell[0] = Agi[heroic];
                            RandomSpell[1] = Crit[heroic];
                            RandomSpell[2] = Ap[heroic];
                            break;
                        }
                        case CLASS_PALADIN:
                        case CLASS_WARRIOR:
                        case CLASS_DEATH_KNIGHT:
                        {
                            RandomSpell[0] = Str[heroic];
                            RandomSpell[1] = Haste[heroic];
                            RandomSpell[2] = Crit[heroic];
                            break;
                        }
                        case CLASS_ROGUE:
                        case CLASS_SHAMAN:
                        {
                            RandomSpell[0] = Agi[heroic];
                            RandomSpell[1] = Haste[heroic];
                            RandomSpell[2] = Ap[heroic];
                            break;
                        }
                        default:
                            return SPELL_AURA_PROC_FAILED;
                    }
                    triggered_spell_id = RandomSpell[irand(0, 2)];
                    if (triggered_spell_id == 0)
                        return SPELL_AURA_PROC_FAILED;

                    if(cooldown && ((Player*)this)->HasSpellCooldown(triggered_spell_id))
                        return SPELL_AURA_PROC_FAILED;

                    CastSpell(this, triggered_spell_id, true, castItem, triggeredByAura);

                    if (cooldown)
                        for (uint8 i = 0; i < 3; i++)
                            ((Player*)this)->AddSpellCooldown(RandomSpell[i], 0, time(NULL) + cooldown);

                    return SPELL_AURA_PROC_OK;
                }

                // Sunwell Exalted Caster Neck (Shattered Sun Pendant of Acumen neck)
                // cast 45479 Light's Wrath if Exalted by Aldor
                // cast 45429 Arcane Bolt if Exalted by Scryers
                case 45481:
                {
                    if(GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    // Get Aldor reputation rank
                    if (((Player *)this)->GetReputationRank(932) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45479;
                        break;
                    }
                    // Get Scryers reputation rank
                    if (((Player *)this)->GetReputationRank(934) == REP_EXALTED)
                    {
                        // triggered at positive/self casts also, current attack target used then
                        if(IsFriendlyTo(target))
                        {
                            target = getVictim();
                            if(!target)
                            {
                                target = ObjectAccessor::GetUnit(*this,((Player *)this)->GetSelectionGuid());
                                if(!target)
                                    return SPELL_AURA_PROC_FAILED;
                            }
                            if(IsFriendlyTo(target))
                                return SPELL_AURA_PROC_FAILED;
                        }

                        triggered_spell_id = 45429;
                        break;
                    }
                    return SPELL_AURA_PROC_FAILED;
                }
                // Sunwell Exalted Melee Neck (Shattered Sun Pendant of Might neck)
                // cast 45480 Light's Strength if Exalted by Aldor
                // cast 45428 Arcane Strike if Exalted by Scryers
                case 45482:
                {
                    if(GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    // Get Aldor reputation rank
                    if (((Player *)this)->GetReputationRank(932) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45480;
                        break;
                    }
                    // Get Scryers reputation rank
                    if (((Player *)this)->GetReputationRank(934) == REP_EXALTED)
                    {
                        triggered_spell_id = 45428;
                        break;
                    }
                    return SPELL_AURA_PROC_FAILED;
                }
                // Sunwell Exalted Tank Neck (Shattered Sun Pendant of Resolve neck)
                // cast 45431 Arcane Insight if Exalted by Aldor
                // cast 45432 Light's Ward if Exalted by Scryers
                case 45483:
                {
                    if(GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    // Get Aldor reputation rank
                    if (((Player *)this)->GetReputationRank(932) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45432;
                        break;
                    }
                    // Get Scryers reputation rank
                    if (((Player *)this)->GetReputationRank(934) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45431;
                        break;
                    }
                    return SPELL_AURA_PROC_FAILED;
                }
                // Sunwell Exalted Healer Neck (Shattered Sun Pendant of Restoration neck)
                // cast 45478 Light's Salvation if Exalted by Aldor
                // cast 45430 Arcane Surge if Exalted by Scryers
                case 45484:
                {
                    if(GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    // Get Aldor reputation rank
                    if (((Player *)this)->GetReputationRank(932) == REP_EXALTED)
                    {
                        target = this;
                        triggered_spell_id = 45478;
                        break;
                    }
                    // Get Scryers reputation rank
                    if (((Player *)this)->GetReputationRank(934) == REP_EXALTED)
                    {
                        triggered_spell_id = 45430;
                        break;
                    }
                    return SPELL_AURA_PROC_FAILED;
                }
                /*
                // Sunwell Exalted Caster Neck (??? neck)
                // cast ??? Light's Wrath if Exalted by Aldor
                // cast ??? Arcane Bolt if Exalted by Scryers*/
                case 46569:
                    return SPELL_AURA_PROC_FAILED;                           // old unused version
                // Living Seed
                case 48504:
                {
                    triggered_spell_id = 48503;
                    basepoints[0] = triggerAmount;
                    target = this;
                    break;
                }
                // Bloodworms Health Leech
                case 50453:
                {
                    if (Unit* owner = GetOwner())
                    {
                        basepoints[0] = int32(damage * 1.50f);
                        target = owner;
                        triggered_spell_id = 50454;
                        break;
                    }
                    return SPELL_AURA_PROC_FAILED;
                }
                // Vampiric Touch (generic, used by some boss)
                case 52723:
                case 60501:
                {
                    triggered_spell_id = 52724;
                    basepoints[0] = damage / 2;
                    target = this;
                    break;
                }
                // Shadowfiend Death (Gain mana if pet dies with Glyph of Shadowfiend)
                case 57989:
                {
                    Unit *owner = GetOwner();
                    if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    // Glyph of Shadowfiend (need cast as self cast for owner, no hidden cooldown)
                    owner->CastSpell(owner,58227,true,castItem,triggeredByAura);
                    return SPELL_AURA_PROC_OK;
                }
                // Swift Hand of Justice
                case 59906:
                {
                    triggered_spell_id = 59913;
                    basepoints[0] = GetMaxHealth()/50;
                    break;
                }
                // Discerning Eye of the Beast
                case 59915:
                {
                    if (getPowerType() != POWER_MANA)
                        return SPELL_AURA_PROC_FAILED;

                    triggered_spell_id = 59914;
                    break;
                }
                // Petrified Bark
                case 62337:
                {
                    triggered_spell_id = 62379;
                    basepoints[0] = damage;
                    // this == pVictim, why? :/ temp. workaround
                    target = SelectRandomUnfriendlyTarget(getVictim());
                    break;
                }
                // Meteor Fists
                case 66725:
                case 68161:
                    triggered_spell_id = 66765;
                    break;
                // Meteor Fists
                case 66808:
                case 68160:
                    triggered_spell_id = 66809;
                    break;
                // Shiny Shard of the Scale - Equip Effect
                case 69739:
                    // Cauterizing Heal or Searing Flame
                    triggered_spell_id = (procFlag & PROC_FLAG_SUCCESSFUL_POSITIVE_SPELL) ? 69734 : 69730;
                    break;
                // Purified Shard of the Scale - Equip Effect
                case 69755:
                    // Cauterizing Heal or Searing Flame
                    triggered_spell_id = (procFlag & PROC_FLAG_SUCCESSFUL_POSITIVE_SPELL) ? 69733 : 69729;
                    break;
                // Vampiric Might (Cult Fanatic, Icecrown Citadel, Lady Deathwhisper encounter)
                case 70674:
                {
                    if (damage <= 0)
                        return SPELL_AURA_PROC_FAILED;

                    basepoints[0] = 3 * damage;
                    triggered_spell_id = 70677;
                    target = this;
                    break;
                }
                // Essence of the Blood Queen
                case 70871:
                    if (!pVictim || pVictim->GetTypeId() != TYPEID_UNIT)
                    {
                        RemoveAurasDueToSpell(dummySpell->Id);
                        return SPELL_AURA_PROC_FAILED;
                    }
                    break;
                case 71169:
                {
                    // Shadow's Fate
                    if (GetTypeId() != TYPEID_UNIT)
                        return SPELL_AURA_PROC_FAILED;

                    // Amaru: done in scripts
                    /*switch (((Creature*)this)->GetCreatureInfo()->Entry)
                    {
                        case 38431:  // Puthricide 25
                        case 38586:
                            CastSpell(this, 71518, true);
                            break;
                        case 38434:  // Lanathel 25
                        case 38436:
                            CastSpell(this, 72934, true);
                            break;
                        case 38265:  // Sindragosa 25
                        case 38267:
                            CastSpell(this, 72289, true);
                            break;
                        default:
                            break;
                    }*/
                    CastSpell(triggeredByAura->GetCaster(), 71203, true);
                    return SPELL_AURA_PROC_OK;
                }
                // Item - Shadowmourne Legendary
                case 71903:
                {
                    if (HasAura(73422))                     // Chaos Bane Str buff
                        return SPELL_AURA_PROC_FAILED;

                    triggered_spell_id = 71905;             // Soul Fragment

                    SpellAuraHolder *aurHolder = GetSpellAuraHolder(triggered_spell_id);

                    // will added first to stack
                    if (!aurHolder)
                        CastSpell(this, 72521, true);       // Shadowmourne Visual Low
                    // half stack
                    else if (aurHolder->GetStackAmount() + 1 == 6)
                        CastSpell(this, 72523, true);       // Shadowmourne Visual High
                    // full stack
                    else if (aurHolder->GetStackAmount() + 1 >= aurHolder->GetSpellProto()->GetStackAmount())
                    {
                        RemoveAurasDueToSpell(triggered_spell_id);
                        RemoveAurasDueToSpell(72521);
                        RemoveAurasDueToSpell(72523);
                        CastSpell(this, 71904, true);       // Chaos Bane
                        return SPELL_AURA_PROC_OK;
                    }
                    break;
                }
                // Black Bruise's Necrotic Touch
                case 71875:
                case 71877:
                {
                    if (procSpell)                          // only from melee attacks
                        return SPELL_AURA_PROC_FAILED;

                    triggered_spell_id = 71879;             // Necrotic Touch
                    basepoints[0] = triggeredByAura->GetModifier()->m_amount * damage / 100;
                    break;
                }
                case 85466:                                 // Bane of Havoc
                case 85468:
                {
                    triggered_spell_id = 85455;
                    // search bane debuff
                    target = GetSingleCastSpellTarget(80240);
                    if (SpellEntry const* havoc = sSpellStore.LookupEntry(80240))
                        basepoints[0] = int32(damage * havoc->CalculateSimpleValue(EFFECT_INDEX_0) / 100.0f);

                    if (!target || target == pVictim)
                        return SPELL_AURA_PROC_FAILED;
                    break;
                }
                case 96976:                                 // Matrix Restabilizer
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    Player* player = (Player*)this;

                    uint32 spells[] = { 96977, 96978, 96979 };
                    for (int i = 0; i < 3; ++i)
                        if (player->HasSpellCooldown(spells[i]))
                            return SPELL_AURA_PROC_FAILED;

                    for (int i = 0; i < 3; ++i)
                        RemoveAurasDueToSpell(spells[i]);

                    uint32 triggered_spell_id = 96977;
                    CombatRating cr = CR_HASTE_MELEE;
                    if (player->GetRatingBonusValue(CR_CRIT_MELEE) > player->GetRatingBonusValue(cr))
                    {
                        cr = CR_CRIT_MELEE;
                        triggered_spell_id = 96978;
                    }
                    if (player->GetRatingBonusValue(CR_MASTERY) > player->GetRatingBonusValue(cr))
                        triggered_spell_id = 96979;
                    break;
                }
                case 96887:                                 // Electrical Charge
                case 97119:                                 // Electrical Charge
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    triggered_spell_id = 96890;

                    int32 bp;
                    Player* player = (Player*)this;
                    if (Aura* aura = GetAura(triggered_spell_id, EFFECT_INDEX_0))
                        if (aura->GetHolder()->GetStackAmount() >= aura->GetSpellProto()->GetStackAmount())
                        {
                            bp = aura->GetModifier()->m_amount;
                            CastCustomSpell(pVictim, 96891, &bp, NULL, NULL, true, NULL, triggeredByAura);
                            RemoveSpellAuraHolder(aura->GetHolder());
                            return SPELL_AURA_PROC_OK;
                        }

                    if (player->HasSpellCooldown(triggered_spell_id))
                        return SPELL_AURA_PROC_FAILED;

                    bp = CalculateSpellDamage(pVictim, triggeredByAura->GetSpellProto(), EFFECT_INDEX_0);
                    CastCustomSpell(this, triggered_spell_id, &bp, NULL, NULL, true, NULL, triggeredByAura);
                    player->AddSpellCooldown(triggered_spell_id, 0, time(NULL) + 3);
                    return SPELL_AURA_PROC_OK;
                }
                case 97138:                                 // Matrix Restabilizer
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    Player* player = (Player*)this;

                    uint32 spells[] = { 97139, 97140, 97141 };
                    for (int i = 0; i < 3; ++i)
                        if (player->HasSpellCooldown(spells[i]))
                            return SPELL_AURA_PROC_FAILED;

                    for (int i = 0; i < 3; ++i)
                        RemoveAurasDueToSpell(spells[i]);

                    uint32 triggered_spell_id = 97139;
                    CombatRating cr = CR_HASTE_MELEE;
                    if (player->GetRatingBonusValue(CR_CRIT_MELEE) > player->GetRatingBonusValue(cr))
                    {
                        cr = CR_CRIT_MELEE;
                        triggered_spell_id = 97140;
                    }
                    if (player->GetRatingBonusValue(CR_MASTERY) > player->GetRatingBonusValue(cr))
                        triggered_spell_id = 97141;
                    break;
                }
                case 108007:                                // Indomitable
                case 109785:                                // Indomitable
                case 109786:                                // Indomitable
                {
                    if (triggeredByAura->GetEffIndex() != EFFECT_INDEX_1)
                        return SPELL_AURA_PROC_FAILED;

                    if (GetHealthPercent() > triggeredByAura->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_0))
                        return SPELL_AURA_PROC_FAILED;

                    basepoints[0] = triggerAmount * damage / 100;
                    triggered_spell_id = 108008;
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_MAGE:
        {
            // Permafrost
            if (dummySpell->SpellIconID == 143)
            {
                basepoints[0] = int32(triggerAmount * (damage + absorb) / 100.0f);
                triggered_spell_id = 91394;
                break;
            }
            // Magic Absorption
            else if (dummySpell->SpellIconID == 459)            // only this spell have SpellIconID == 459 and dummy aura
            {
                if (getPowerType() != POWER_MANA)
                    return SPELL_AURA_PROC_FAILED;

                // mana reward
                basepoints[0] = (triggerAmount * GetMaxPower(POWER_MANA) / 100);
                target = this;
                triggered_spell_id = 29442;
                break;
            }
            // Improved Mana Gem
            else if (dummySpell->SpellIconID == 1036)
            {
                triggered_spell_id = 83098;
                basepoints[0] = int32(triggerAmount * GetMaxPower(POWER_MANA) / 100.0f);
                basepoints[1] = basepoints[0];
                break;
            }
            // Master of Elements
            else if (dummySpell->SpellIconID == 1920)
            {
                if(!procSpell)
                    return SPELL_AURA_PROC_FAILED;

                // _base_ mana cost save
                int32 cost = procSpell->GetManaCost() + procSpell->GetManaCostPercentage() * GetCreateMana() / 100;

                basepoints[0] = cost * triggerAmount / 100;
                if (basepoints[0] <= 0)
                    return SPELL_AURA_PROC_FAILED;

                target = this;
                triggered_spell_id = 29077;
                break;
            }
            // Arcane Potency
            else if (dummySpell->SpellIconID == 2120)
            {
                if(!procSpell)
                    return SPELL_AURA_PROC_FAILED;

                //megai2: only on pom and clearcast
                if (procSpell->Id != 12536 && procSpell->Id != 12043)
                    return SPELL_AURA_PROC_FAILED;

                target = this;
                switch (dummySpell->Id)
                {
                    case 31571: triggered_spell_id = 57529; break;
                    case 31572: triggered_spell_id = 57531; break;
                    default:
                        ERROR_LOG("Unit::HandleDummyAuraProc: non handled spell id: %u",dummySpell->Id);
                        return SPELL_AURA_PROC_FAILED;
                }
                break;
            }
            // Nether Vortex
            else if (dummySpell->SpellIconID == 2294)
            {
                // ...if no target is currently affected by Slow
                if (GetSingleCastSpellTarget(31589))
                    return SPELL_AURA_PROC_FAILED;

                triggered_spell_id = 31589;
                break;
            }
            // Piercing Chill
            else if (dummySpell->SpellIconID == 4625)
            {
                triggered_spell_id = 83154;
                break;
            }
            // Hot Streak
            else if (dummySpell->Id == 44445)
            {
                if (effIndex != EFFECT_INDEX_0 || !roll_chance_i(triggerAmount))
                    return SPELL_AURA_PROC_FAILED;

                triggered_spell_id = 48108;
                break;
            }
            // Improved Hot Streak
            else if (dummySpell->Id == 44446 || dummySpell->Id == 44448)
            {
                if (effIndex != EFFECT_INDEX_1)
                    return SPELL_AURA_PROC_OK;

                Aura *counter = GetAura(triggeredByAura->GetId(), EFFECT_INDEX_1);
                if (!counter)
                    return SPELL_AURA_PROC_OK;

                // Count spell criticals in a row in second aura
                Modifier* mod = counter->GetModifier();
                if (procEx & PROC_EX_CRITICAL_HIT)
                {
                    mod->m_amount *= 2;
                    if (mod->m_amount < 100) // not enough
                        return SPELL_AURA_PROC_OK;
                    // Critical counted -> roll chance
                    if (roll_chance_i(dummySpell->CalculateSimpleValue(EFFECT_INDEX_0)))
                        CastSpell(this, 48108, true, castItem, triggeredByAura);
                }
                mod->m_amount = 25;
                return SPELL_AURA_PROC_OK;
            }
            // Burnout
            if (dummySpell->SpellIconID == 2998)
            {
                if(!procSpell)
                    return SPELL_AURA_PROC_FAILED;

                int32 cost = procSpell->GetManaCost() + procSpell->GetManaCostPercentage() * GetCreateMana() / 100;
                basepoints[0] = cost * triggerAmount/100;
                if (basepoints[0] <=0)
                    return SPELL_AURA_PROC_FAILED;
                triggered_spell_id = 44450;
                target = this;
                break;
            }
            switch(dummySpell->Id)
            {
                // Ignite
                case 11119:
                case 11120:
                case 12846:
                {
                    triggered_spell_id = 12654;

                    if (!procSpell || procSpell->Id == triggered_spell_id)
                        return SPELL_AURA_PROC_FAILED;

                    basepoints[0] = int32(triggerAmount * (damage + absorb) /
                        GetSpellAuraMaxTicks(triggered_spell_id) / 100.0f);

                    basepoints[0] += target->GetRemainingDotDamage(triggered_spell_id, GetObjectGuid());
                    break;
                }
                // Empowered Fire (mana regen)
                case 12654:
                {
                    Unit* caster = triggeredByAura->GetCaster();
                    // it should not be triggered from other ignites
                    if (caster && pVictim && caster->GetObjectGuid() == pVictim->GetObjectGuid())
                    {
                        Unit::AuraList const& auras = caster->GetAurasByType(SPELL_AURA_ADD_FLAT_MODIFIER);
                        for (Unit::AuraList::const_iterator i = auras.begin(); i != auras.end(); i++)
                        {
                            switch((*i)->GetId())
                            {
                                case 31656:
                                case 31657:
                                case 31658:
                                {
                                    if (roll_chance_i(int32((*i)->GetSpellProto()->GetProcChance())))
                                    {
                                        caster->CastSpell(caster, 67545, true);
                                        return SPELL_AURA_PROC_OK;
                                    }
                                    else
                                        return SPELL_AURA_PROC_FAILED;
                                }
                            }
                        }
                    }
                    return SPELL_AURA_PROC_FAILED;
                }
                // Glyph of Ice Block
                case 56372:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    // not 100% safe with client version switches but for 3.1.3 no spells with cooldown that can have mage player except Frost Nova.
                    ((Player*)this)->RemoveSpellCategoryCooldown(35, true);
                    return SPELL_AURA_PROC_OK;
                }
                // Glyph of Icy Veins
                case 56374:
                {
                    Unit::AuraList const& hasteAuras = GetAurasByType(SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK);
                    for(Unit::AuraList::const_iterator i = hasteAuras.begin(); i != hasteAuras.end();)
                    {
                        if (!IsPositiveSpell((*i)->GetId()))
                        {
                            RemoveAurasDueToSpell((*i)->GetId());
                            i = hasteAuras.begin();
                        }
                        else
                            ++i;
                    }

                    RemoveSpellsCausingAura(SPELL_AURA_MOD_DECREASE_SPEED);
                    RemoveSpellsCausingAura(SPELL_AURA_HASTE_SPELLS);
                    return SPELL_AURA_PROC_OK;
                }
                // Glyph of Polymorph
                case 56375:
                {
                    if (!pVictim || !pVictim->isAlive())
                        return SPELL_AURA_PROC_FAILED;

                    // except shadow word: death periodic
                    pVictim->RemoveSpellsCausingAura(SPELL_AURA_PERIODIC_DAMAGE, pVictim->GetSpellAuraHolder(32409));
                    pVictim->RemoveSpellsCausingAura(SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
                    pVictim->RemoveSpellsCausingAura(SPELL_AURA_PERIODIC_LEECH);

                    return SPELL_AURA_PROC_OK;
                }
                // Blessing of Ancient Kings
                case 64411:
                {
                    // for DOT procs
                    if (!IsPositiveSpell(procSpell->Id))
                        return SPELL_AURA_PROC_FAILED;

                    basepoints[0] = 0;
                    triggered_spell_id = 64413;
                    if (pVictim)
                    {
                        if (Aura* absorbaura = pVictim->GetAura(triggered_spell_id, EFFECT_INDEX_0))
                            basepoints[0] = absorbaura->GetModifier()->m_amount;
                    }

                    basepoints[0] += damage * 15 / 100;
                    if (basepoints[0] > 20000)
                        basepoints[0] = 20000;
                    break;
                }
                // Item - Mage T8 4P Bonus
                case 64869:
                {
                    if (!roll_chance_i(triggeredByAura->GetModifier()->m_amount))
                        return SPELL_AURA_PROC_FAILED;

                    SpellAuraHolder* holder;
                    // Hot Streak
                    if (SpellAuraHolder* _holder = GetSpellAuraHolder(48108, GetObjectGuid()))
                        holder = _holder;
                    // Brain Freeze
                    else if (SpellAuraHolder* _holder = GetSpellAuraHolder(57761, GetObjectGuid()))
                        holder = _holder;

                    if (!holder || holder->GetAuraCharges() > 1)
                        return SPELL_AURA_PROC_FAILED;

                    holder->SetAuraCharges(holder->GetAuraCharges() + 1, false);
                    return SPELL_AURA_PROC_OK;
                }
            }
            break;
        }
        case SPELLFAMILY_WARRIOR:
        {
            // Retaliation
            if (dummySpell->IsFitToFamilyMask(UI64LIT(0x0000000800000000)))
            {
                // check attack comes not from behind
                if (!HasInArc(M_PI_F, pVictim))
                    return SPELL_AURA_PROC_FAILED;

                triggered_spell_id = 22858;
                break;
            }
            // Second Wind
            else if (dummySpell->SpellIconID == 1697)
            {
                // only for spells and hit/crit (trigger start always) and not start from self casted spells (5530 Mace Stun Effect for example)
                if (procSpell == 0 || !(procEx & (PROC_EX_NORMAL_HIT|PROC_EX_CRITICAL_HIT)) || this == pVictim)
                    return SPELL_AURA_PROC_FAILED;
                // Need stun or root mechanic
                if (!(GetAllSpellMechanicMask(procSpell) & IMMUNE_TO_ROOT_AND_STUN_MASK))
                    return SPELL_AURA_PROC_FAILED;

                switch (dummySpell->Id)
                {
                    case 29838: triggered_spell_id=29842; break;
                    case 29834: triggered_spell_id=29841; break;
                    case 42770: triggered_spell_id=42771; break;
                    default:
                        ERROR_LOG("Unit::HandleDummyAuraProc: non handled spell id: %u (SW)",dummySpell->Id);
                    return SPELL_AURA_PROC_FAILED;
                }

                target = this;
                break;
            }
            // Sweeping Strikes
            else if (dummySpell->Id == 12328)
            {
                // prevent chain of triggered spell from same triggered spell
                if (procSpell && procSpell->Id == 26654)
                    return SPELL_AURA_PROC_FAILED;

                target = SelectRandomUnfriendlyTarget(pVictim);
                if (!target)
                    return SPELL_AURA_PROC_FAILED;

                triggered_spell_id = 26654;
                break;
            }
            // Vengeance (warrior)
            else if (dummySpell->Id == 93098)
                return HandleVengeanceProc(pVictim, damage, triggerAmount);
            // Vengeance (death knight)
            else if (dummySpell->Id == 93099)
                return HandleVengeanceProc(pVictim, damage, triggerAmount);
            // Item - Warrior T12 Protection 2P Bonus
            else if (dummySpell->Id == 99239)
            {
                triggered_spell_id = 99240;
                basepoints[0] = triggerAmount * (damage+absorb) / 100 / GetSpellAuraMaxTicks(triggered_spell_id);
                break;
            }
            // Item - Warrior T13 Protection 2P Bonus (Revenge)
            else if (dummySpell->Id == 105908)
            {
                triggered_spell_id = 105909;
                basepoints[0] = triggerAmount * damage / 100;
                break;
            }
            break;
        }
        case SPELLFAMILY_WARLOCK:
        {
            // Seed of Corruption
            if (dummyClassOptions && dummyClassOptions->SpellFamilyFlags & UI64LIT(0x0000001000000000))
            {
                Modifier* mod = triggeredByAura->GetModifier();
                // if damage is more than need or target die from damage deal finish spell
                if( mod->m_amount <= (int32)(damage + absorb) || GetHealth() <= damage + absorb)
                {
                    // remember guid before aura delete
                    ObjectGuid casterGuid = triggeredByAura->GetCasterGuid();

                    // Remove aura (before cast for prevent infinite loop handlers)
                    RemoveAurasDueToSpell(triggeredByAura->GetId());

                    // Cast finish spell (triggeredByAura already not exist!)
                    CastSpell(this, 27285, true, castItem, NULL, casterGuid);

                    if (Unit* caster = GetMap()->GetUnit(casterGuid))
                    {
                        if (caster->soulburnMarker)                     // marked as affected by soulburn
                            caster->CastSpell(caster, 87388, true);     // Soul Shard energize
                        caster->soulburnMarker = false;
                    }

                    return SPELL_AURA_PROC_OK;                            // no hidden cooldown
                }

                // Damage counting
                mod->m_amount-=damage+absorb;
                return SPELL_AURA_PROC_OK;
            }
            // Seed of Corruption (Mobs cast) - no die req
            if (dummyClassOptions && dummyClassOptions->SpellFamilyFlags == UI64LIT(0x0) && dummySpell->SpellIconID == 1932)
            {
                Modifier* mod = triggeredByAura->GetModifier();
                // if damage is more than need deal finish spell
                if( mod->m_amount <= (int32)(damage + absorb) )
                {
                    // remember guid before aura delete
                    ObjectGuid casterGuid = triggeredByAura->GetCasterGuid();

                    // Remove aura (before cast for prevent infinite loop handlers)
                    RemoveAurasDueToSpell(triggeredByAura->GetId());

                    // Cast finish spell (triggeredByAura already not exist!)
                    CastSpell(this, 32865, true, castItem, NULL, casterGuid);
                    return SPELL_AURA_PROC_OK;                            // no hidden cooldown
                }
                // Damage counting
                mod->m_amount-=damage + absorb;
                return SPELL_AURA_PROC_OK;
            }
            // Nether Protection
            if (dummySpell->SpellIconID == 1985)
            {
                if (!procSpell)
                    return SPELL_AURA_PROC_FAILED;

                basepoints[0] = -triggerAmount;

                uint32 spellToSchool[MAX_SPELL_SCHOOL] = { 0, 54370, 54371, 54375, 54372, 54374, 54373 };
                uint32 schoolMask = GetSpellSchoolMask(procSpell);
                for (uint8 i = 0; i < MAX_SPELL_SCHOOL; ++i)
                {
                    if (schoolMask & (1 << i))
                        if (spellToSchool[i])
                            CastCustomSpell(this, spellToSchool[i], &basepoints[0], NULL, NULL, true);
                }

                // TODO: must proc only from absorb by Shadow Ward, Nether Ward, voidwalker Sacrifice
                return SPELL_AURA_PROC_OK;
            }
            // Fel Synergy
            else if (dummySpell->SpellIconID == 3222)
            {
                target = GetPet();
                if (!target)
                    return SPELL_AURA_PROC_FAILED;
                basepoints[0] = (damage + absorb) * triggerAmount / 100;
                triggered_spell_id = 54181;
                break;
            }
            // Jinx
            else if (dummySpell->SpellIconID == 5002)
            {
                if (!target || effIndex != EFFECT_INDEX_1)
                    return SPELL_AURA_PROC_FAILED;

                basepoints[0] = triggerAmount;

                switch (target->getClass())
                {
                    case CLASS_WARRIOR:
                        triggered_spell_id = 85539;
                        break;
                    case CLASS_ROGUE:
                        triggered_spell_id = 85540;
                        break;
                    case CLASS_DEATH_KNIGHT:
                        triggered_spell_id = 85541;
                        break;
                    case CLASS_HUNTER:
                        triggered_spell_id = 85542;
                        break;
                    case CLASS_DRUID:
                    {
                        if (target->GetShapeshiftForm() == FORM_BEAR)
                            triggered_spell_id = 85539;
                        else if (target->GetShapeshiftForm() == FORM_CAT)
                            triggered_spell_id = 85540;
                        break;
                    }
                }

                if (!triggered_spell_id)
                    return SPELL_AURA_PROC_FAILED;

                break;
            }
            // Burning Embers
            else if (dummySpell->SpellIconID == 5116)
            {
                if (effIndex != EFFECT_INDEX_0)
                    return SPELL_AURA_PROC_FAILED;

                triggered_spell_id = 85421;
                basepoints[0] = int32(triggerAmount * damage / 100.0f / 7);
                break;
            }

            switch (dummySpell->Id)
            {
                // Nightfall & Glyph of Corruption
                case 18094:
                case 18095:
                case 56218:
                {
                    target = this;
                    triggered_spell_id = 17941;
                    break;
                }
                // Improved Soul Fire
                case 18119:
                case 18120:
                {
                    basepoints[0] = triggerAmount;
                    triggered_spell_id = 85383;
                    break;
                }
                // Fel Armor
                case 28176:
                {
                    if (!procSpell || IsAreaOfEffectSpell(procSpell))
                        return SPELL_AURA_PROC_FAILED;

                    triggered_spell_id = 96379;
                    basepoints[0] = int32(damage * triggerAmount / 100);
                    break;
                }
                // Shadowflame (Voidheart Raiment set bonus)
                case 37377:
                {
                    triggered_spell_id = 37379;
                    break;
                }
                // Pet Healing (Corruptor Raiment or Rift Stalker Armor)
                case 37381:
                {
                    target = GetPet();
                    if (!target)
                        return SPELL_AURA_PROC_FAILED;

                    // heal amount
                    basepoints[0] = (damage+absorb) * triggerAmount/100;
                    triggered_spell_id = 37382;
                    break;
                }
                // Shadowflame Hellfire (Voidheart Raiment set bonus)
                case 39437:
                {
                    triggered_spell_id = 37378;
                    break;
                }
                // Glyph of Succubus
                // done in other way
                /*case 56250:
                {
                    if (!pVictim || !pVictim->isAlive())
                        return SPELL_AURA_PROC_FAILED;

                    pVictim->RemoveSpellsCausingAura(SPELL_AURA_PERIODIC_DAMAGE, pVictim->GetSpellAuraHolder(32409));
                    pVictim->RemoveSpellsCausingAura(SPELL_AURA_PERIODIC_DAMAGE_PERCENT);

                    return SPELL_AURA_PROC_OK;
                }*/
            }
            break;
        }
        case SPELLFAMILY_PRIEST:
        {
            // Vampiric Touch
            if (dummyClassOptions && dummyClassOptions->SpellFamilyFlags & UI64LIT(0x0000040000000000))
            {
                if (!pVictim || !pVictim->isAlive())
                    return SPELL_AURA_PROC_FAILED;

                // pVictim is caster of aura
                if (triggeredByAura->GetCasterGuid() != pVictim->GetObjectGuid())
                    return SPELL_AURA_PROC_FAILED;

                // Energize 1% of max. mana per 10 secs
                pVictim->CastSpell(pVictim, 57669, true, castItem, triggeredByAura);
                return SPELL_AURA_PROC_OK;                                // no hidden cooldown
            }

            switch (dummySpell->SpellIconID)
            {
                // Sin and Punishment
                case 1869:
                {
                    if (GetTypeId() != TYPEID_PLAYER || triggeredByAura->GetEffIndex() != EFFECT_INDEX_1)
                        return SPELL_AURA_PROC_FAILED;

                    // modify Shadowfiend cooldown
                    ((Player*)this)->SendModifyCooldown(34433, -triggerAmount * IN_MILLISECONDS);
                    return SPELL_AURA_PROC_OK;
                }
                // Evangelism
                case 2138:
                {
                    // Mind Flay
                    bool dark = procSpell && procSpell->Id == 15407;

                    // Don't allow proc from Holy Fire periodic and Mind Flay apply
                    if (dark && (procFlag & PROC_FLAG_ON_DO_PERIODIC) == 0)
                        return SPELL_AURA_PROC_FAILED;
                    else if (!dark && (procFlag & PROC_FLAG_ON_DO_PERIODIC) != 0)
                        return SPELL_AURA_PROC_FAILED;

                    // Rank 1
                    if (dummySpell->Id == 81659)
                        triggered_spell_id = dark ? 87117 : 81660;
                    // Rank 2
                    else
                        triggered_spell_id = dark ? 87118 : 81661;
                    break;
                }
                // Body and Soul
                case 2218:
                {
                    // Body and Soul speed aura
                    if (effIndex == EFFECT_INDEX_0)
                        triggered_spell_id = dummySpell->Id == 64127 ? 64128 : 65081;
                    // Body and Soul dispel poison part
                    else if (pVictim == this && roll_chance_i(triggerAmount))
                        triggered_spell_id = 64136;
                    else
                        return SPELL_AURA_PROC_FAILED;
                    break;
                }
                // Divine Aegis
                case 2820:
                {
                    if (!pVictim || !pVictim->isAlive())
                        return SPELL_AURA_PROC_FAILED;

                    // Heal should be crit if it's not Prayer of Healing
                    if ((!procSpell || procSpell->Id != 596) && (procEx & PROC_EX_CRITICAL_HIT) == 0)
                        return SPELL_AURA_PROC_FAILED;

                    // find Divine Aegis on the target and get absorb amount
                    Aura* divineAegis = pVictim->GetAura(47753, EFFECT_INDEX_0);
                    if (divineAegis)
                        basepoints[0] = divineAegis->GetModifier()->m_amount;
                    basepoints[0] += damage * triggerAmount / 100;

                    // limit absorb amount
                    int32 levelbonus = pVictim->getLevel() * 125;
                    if (basepoints[0] > levelbonus)
                        basepoints[0] = levelbonus;
                    triggered_spell_id = 47753;
                    break;
                }
                // Phantasm
                case 2901:
                {
                    if (!roll_chance_i(triggerAmount))
                        return SPELL_AURA_PROC_FAILED;

                    RemoveSpellsCausingAura(SPELL_AURA_MOD_ROOT);
                    RemoveSpellsCausingAura(SPELL_AURA_MOD_DECREASE_SPEED);
                    break;
                }
                // Train of Thought
                case 2942:
                {
                    if (!procSpell || GetTypeId() != TYPEID_PLAYER || dummySpell->Id == 92295 && !roll_chance_i(50))
                        return SPELL_AURA_PROC_FAILED;

                    // Greater Heal
                    if (procSpell->Id == 2060)
                        // Inner Focus
                        ((Player*)this)->SendModifyCooldown(89485, -triggerAmount * IN_MILLISECONDS);
                    else
                        // Penance
                        ((Player*)this)->SendModifyCooldown(47540, -triggerAmount * IN_MILLISECONDS / 10);

                    return SPELL_AURA_PROC_OK;
                }
                // Divine Touch
                case 3021:
                {
                    if (!procSpell)
                        return SPELL_AURA_PROC_FAILED;

                    // Renew
                    Aura* healingAura = pVictim->GetAura(SPELL_AURA_PERIODIC_HEAL, SPELLFAMILY_PRIEST, UI64LIT(0x40), 0, GetObjectGuid());
                    if (!healingAura)
                        return SPELL_AURA_PROC_FAILED;

                    int32 healingfromticks = healingAura->GetModifier()->m_amount * GetSpellAuraMaxTicks(procSpell);

                    basepoints[0] = healingfromticks * triggerAmount / 100;
                    triggered_spell_id = 63544;
                    break;
                }
                // Improved Devouring Plague
                case 3790:
                {
                    if (!procSpell)
                        return SPELL_AURA_PROC_FAILED;

                    Aura* leachAura = pVictim->GetAura(SPELL_AURA_PERIODIC_LEECH, SPELLFAMILY_PRIEST, UI64LIT(0x02000000), 0, GetObjectGuid());
                    if (!leachAura)
                        return SPELL_AURA_PROC_FAILED;

                    int32 damagefromticks = leachAura->GetModifier()->m_amount * GetSpellAuraMaxTicks(procSpell);
                    basepoints[0] = damagefromticks * triggerAmount / 100;
                    triggered_spell_id = 63675;
                    break;
                }
                // Shadowy Apparition
                case 4879:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    if (!pVictim || !pVictim->IsInMap(this) || pVictim->GetDistance(this) > 50.0f)
                        return SPELL_AURA_PROC_FAILED;

                    if (!roll_chance_i(triggerAmount * (((Player*)this)->isMoving() ? 5 : 1)))
                        return SPELL_AURA_PROC_FAILED;

                    if (((Player*)this)->GetSummonedUnitCount(46954) >= 4)
                        return SPELL_AURA_PROC_FAILED;

                    if (Creature* apparition = SummonCreature(46954, GetPositionX(), GetPositionY(), GetPositionZ(), GetOrientation(), TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT_OR_DEAD_DESPAWN, 1000))
                        apparition->Attack(pVictim, true);

                    return SPELL_AURA_PROC_OK;
                }
                // Atonement
                case 4938:
                {
                    basepoints[0] = damage * triggerAmount / 100;
                    triggered_spell_id = 94472;
                    target = pVictim;
                    break;
                }
            }

            switch(dummySpell->Id)
            {
                // Vampiric Embrace
                case 15286:
                {
                    // Return if self damage
                    if (this == pVictim)
                        return SPELL_AURA_PROC_FAILED;

                    // Heal amount - Self/Team
                    int32 team = triggerAmount * damage / 200;
                    int32 self = triggerAmount * damage / 100;
                    CastCustomSpell(this, 15290, &team, &self, NULL, true, castItem, triggeredByAura);
                    return SPELL_AURA_PROC_OK;                                // no hidden cooldown
                }
                // Priest Tier 6 Trinket (Ashtongue Talisman of Acumen)
                case 40438:
                {
                    // Shadow Word: Pain
                    if (procClassOptions && procClassOptions->SpellFamilyFlags & UI64LIT(0x0000000000008000))
                        triggered_spell_id = 40441;
                    // Renew
                    else if (procClassOptions && procClassOptions->SpellFamilyFlags & UI64LIT(0x0000000000000010))
                        triggered_spell_id = 40440;
                    else
                        return SPELL_AURA_PROC_FAILED;

                    target = this;
                    break;
                }
                // Oracle Healing Bonus ("Garments of the Oracle" set)
                case 26169:
                {
                    // heal amount
                    basepoints[0] = int32(damage * 10/100);
                    target = this;
                    triggered_spell_id = 26170;
                    break;
                }
                // Frozen Shadoweave (Shadow's Embrace set) warning! its not only priest set
                case 39372:
                {
                    if(!procSpell || (GetSpellSchoolMask(procSpell) & (SPELL_SCHOOL_MASK_FROST | SPELL_SCHOOL_MASK_SHADOW))==0 )
                        return SPELL_AURA_PROC_FAILED;

                    // heal amount
                    basepoints[0] = (damage+absorb) * triggerAmount/100;
                    target = this;
                    triggered_spell_id = 39373;
                    break;
                }
                // Greater Heal (Vestments of Faith (Priest Tier 3) - 4 pieces bonus)
                case 28809:
                {
                    triggered_spell_id = 28810;
                    break;
                }
                // Glyph of Prayer of Healing
                case 55680:
                {
                    basepoints[0] = int32(damage * triggerAmount  / 200);   // 10% each tick
                    triggered_spell_id = 56161;             // Glyph of Prayer of Healing
                    break;
                }
                // Glyph of Spirit Tap
                case 63237:
                {
                    triggered_spell_id = 81301;
                    break;
                }
                // Priest T10 Healer 2P Bonus
                case 70770:
                {
                    triggered_spell_id = 70772;             // Blessed Healing
                    basepoints[0] = int32(triggerAmount * damage / 100) / GetSpellAuraMaxTicks(triggered_spell_id);
                    break;
                }
                // Item - Priest T10 Healer 4P Bonus
                case 70799:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    // Circle of Healing
                    ((Player*)this)->RemoveSpellCategoryCooldown(1204, true);

                    // Penance
                    ((Player*)this)->RemoveSpellCategoryCooldown(1230, true);

                    return SPELL_AURA_PROC_OK;
                }
                // Echo of Light
                case 77485:
                {
                    if (effIndex != EFFECT_INDEX_0)
                        return SPELL_AURA_PROC_FAILED;

                    triggered_spell_id = 77489;
                    basepoints[0] = int32(damage * triggerAmount / 100 / GetSpellAuraMaxTicks(triggered_spell_id));
                    break;
                }
                // Item - Priest T11 Healer 4P Bonus
                case 89911:
                {
                    // Indulgence of the Penitent
                    triggered_spell_id = 89913;
                    break;
                }
                // Shadowflame
                case 99155:
                {
                    // Indulgence of the Penitent
                    basepoints[0] = int32(triggerAmount * damage / 100.0f);
                    triggered_spell_id = 99156;
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            switch(dummySpell->Id)
            {
                // Leader of the Pack
                case 24932:
                {
                    // dummy m_amount store health percent
                    int32 heal_percent = triggeredByAura->GetModifier()->m_amount;

                    // check explicitly only to prevent mana cast when halth cast cooldown
                    if (cooldown && ((Player*)this)->HasSpellCooldown(34299))
                        return SPELL_AURA_PROC_FAILED;

                    if (triggeredByAura->GetCasterGuid() != GetObjectGuid())
                        return SPELL_AURA_PROC_FAILED;

                    // health
                    triggered_spell_id = 34299;
                    basepoints[0] = int32(GetMaxHealth() * heal_percent / 100);
                    target = this;

                    if (SpellEntry const* manaSpell = sSpellStore.LookupEntry(17007))
                    {
                        // mana
                        int32 mana = int32(GetMaxPower(POWER_MANA) * manaSpell->CalculateSimpleValue(EFFECT_INDEX_0) / 100);
                        CastCustomSpell(this, 68285, &mana, NULL, NULL, true, castItem, triggeredByAura);
                    }
                    break;
                }
                // Healing Touch (Dreamwalker Raiment set)
                case 28719:
                {
                    // mana back
                    basepoints[0] = int32(procSpell->GetManaCost() * 30 / 100);
                    target = this;
                    triggered_spell_id = 28742;
                    break;
                }
                // Healing Touch Refund (Idol of Longevity trinket)
                case 28847:
                {
                    target = this;
                    triggered_spell_id = 28848;
                    break;
                }
                // Mana Restore (Malorne Raiment set / Malorne Regalia set)
                case 37288:
                case 37295:
                {
                    target = this;
                    triggered_spell_id = 37238;
                    break;
                }
                // Druid Tier 6 Trinket
                case 40442:
                {
                    float  chance;

                    // Starfire
                    if (procClassOptions && procClassOptions->SpellFamilyFlags & UI64LIT(0x0000000000000004))
                    {
                        triggered_spell_id = 40445;
                        chance = 25.0f;
                    }
                    // Rejuvenation
                    else if (procClassOptions && procClassOptions->SpellFamilyFlags & UI64LIT(0x0000000000000010))
                    {
                        triggered_spell_id = 40446;
                        chance = 25.0f;
                    }
                    // Mangle (Bear) and Mangle (Cat)
                    else if (procClassOptions && procClassOptions->SpellFamilyFlags & UI64LIT(0x0000044000000000))
                    {
                        triggered_spell_id = 40452;
                        chance = 40.0f;
                    }
                    else
                        return SPELL_AURA_PROC_FAILED;

                    if (!roll_chance_f(chance))
                        return SPELL_AURA_PROC_FAILED;

                    target = this;
                    break;
                }
                // Maim Interrupt
                case 44835:
                {
                    // Deadly Interrupt Effect
                    triggered_spell_id = 32747;
                    break;
                }
                // Glyph of Regrowth
                case 54743:
                {
                    if (!pVictim || damage + pVictim->GetHealth() < triggerAmount * GetMaxHealth() / 100)
                        return SPELL_AURA_PROC_FAILED;

                    // Regrowth Refresh
                    triggered_spell_id = 93036;
                    break;
                }
                // Glyph of Healing Touch
                case 54825:
                {
                    if (!pVictim || pVictim->GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    ((Player*)this)->SendModifyCooldown(17116, -triggerAmount * IN_MILLISECONDS);
                    return SPELL_AURA_PROC_OK;
                }
                // Glyph of Starfire
                case 54845:
                {
                    if (Aura* moonfire = target->GetAura(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_DRUID, UI64LIT(0x00000002), 0, GetGUID()))
                    {
                        // Moonfire's max duration, note: spells which modifies Moonfire's duration also counted like Glyph of Starfire
                        int32 CountMin = moonfire->GetAuraMaxDuration();

                        // just Moonfire's max duration without other spells
                        int32 CountMax = GetSpellMaxDuration(moonfire->GetSpellProto());

                        // add possible auras' and Glyph of Starfire's max duration
                        CountMax += 3 * triggerAmount * 1000;       // Glyph of Starfire            -> +9 seconds
                        CountMax += HasAura(38414) ? 3 * 1000 : 0;  // Moonfire duration            -> +3 seconds

                        // if min < max -> that means caster didn't cast 3 starfires yet
                        // so set Moonfire's duration and max duration
                        if (CountMin < CountMax)
                        {
                            moonfire->GetHolder()->SetAuraDuration(moonfire->GetAuraDuration() + triggerAmount * 1000);
                            moonfire->GetHolder()->SetAuraMaxDuration(CountMin + triggerAmount * 1000);
                            moonfire->GetHolder()->SendAuraUpdate(false);
                            return SPELL_AURA_PROC_OK;
                        }
                    }
                    return SPELL_AURA_PROC_FAILED;
                }
                // Glyph of Bloodletting
                case 54815:
                {
                    if (Aura* rip = target->GetAura(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_DRUID, UI64LIT(0x00800000), 0, GetGUID()))
                    {
                        // Rip's max duration, note: spells which modifies Rip's duration also counted like Glyph of Rip
                        int32 CountMin = rip->GetAuraMaxDuration();

                        // just Rip's max duration without other spells
                        int32 CountMax = GetSpellMaxDuration(rip->GetSpellProto());

                        // add possible auras' and Glyph of Shred's max duration
                        CountMax += 3 * triggerAmount * 1000;       // Glyph of Shred               -> +6 seconds
                        CountMax += HasAura(60141) ? 4 * 1000 : 0;  // Rip Duration/Lacerate Damage -> +4 seconds

                        // if min < max -> that means caster didn't cast 3 shred yet
                        // so set Rip's duration and max duration
                        if (CountMin < CountMax)
                        {
                            rip->GetHolder()->SetAuraDuration(rip->GetAuraDuration() + triggerAmount * 1000);
                            rip->GetHolder()->SetAuraMaxDuration(CountMin + triggerAmount * 1000);
                            rip->GetHolder()->SendAuraUpdate(false);
                            return SPELL_AURA_PROC_OK;
                        }
                    }
                    return SPELL_AURA_PROC_FAILED;
                }
                // Glyph of Starsurge
                case 62971:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    // modify Starfall cooldown
                    ((Player*)this)->SendModifyCooldown(48505, -triggerAmount * IN_MILLISECONDS);
                    return SPELL_AURA_PROC_OK;
                }
                // Item - Druid T10 Restoration 4P Bonus (Rejuvenation)
                case 70664:
                {
                    if (!procSpell || GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    float radius;
                    SpellEffectEntry const* spellEffect = procSpell->GetSpellEffect(EFFECT_INDEX_0);
                    if (spellEffect && spellEffect->GetRadiusIndex())
                        radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(spellEffect->GetRadiusIndex()));
                    else
                        radius = GetSpellMaxRange(sSpellRangeStore.LookupEntry(procSpell->rangeIndex));

                    ((Player*)this)->ApplySpellMod(procSpell->Id, SPELLMOD_RADIUS, radius,NULL);

                    Unit *second = pVictim->SelectRandomFriendlyTarget(pVictim, radius);

                    if (!second)
                        return SPELL_AURA_PROC_FAILED;

                    pVictim->CastSpell(second, procSpell, true, NULL, triggeredByAura, GetObjectGuid());
                    return SPELL_AURA_PROC_OK;
                }
                // Item - Druid T10 Balance 4P Bonus
                case 70723:
                {
                    triggered_spell_id = 71023;             // Languish
                    basepoints[0] = int32(triggerAmount * (damage+absorb) / 100) / GetSpellAuraMaxTicks(triggered_spell_id);
                    break;
                }
                // Vengeance
                case 84840:
                    return HandleVengeanceProc(pVictim, damage, triggerAmount);
                // Item - Druid T12 Feral 2P Bonus
                case 99001:
                {
                    triggered_spell_id = 99002;
                    basepoints[0] = int32(triggerAmount * (damage+absorb) / 100) / GetSpellAuraMaxTicks(triggered_spell_id);
                    break;
                }
                // Item - Druid T12 Restoration 4P Bonus
                case 99015:
                {
                    basepoints[0] = damage;
                    triggered_spell_id = 99017;
                    break;
                }
                // Item - Rogue T12 2P Bonus
                case 99174:
                {
                    triggered_spell_id = 99173;
                    basepoints[0] = int32(triggerAmount * (damage+absorb) / 100) / GetSpellAuraMaxTicks(triggered_spell_id);
                }
                // Item - Druid T13 Feral 4P Bonus (Frenzied Regeneration and Stampede)
                case 105735:
                {
                    if (triggeredByAura->GetEffIndex() != EFFECT_INDEX_1)
                        return SPELL_AURA_PROC_FAILED;

                    triggered_spell_id = 105737;
                    break;
                }
            }
            // Nature's Ward
            if (dummySpell->SpellIconID == 2250)
            {
                if (GetTypeId() != TYPEID_PLAYER)
                    return SPELL_AURA_PROC_FAILED;

                if (GetHealthPercent() > 50.0f)
                    return SPELL_AURA_PROC_FAILED;

                if (!roll_chance_i(triggerAmount))
                    return SPELL_AURA_PROC_FAILED;

                triggered_spell_id = 774;
                target = this;
                if (GetSpellAuraHolder(triggered_spell_id, GetObjectGuid()))
                    return SPELL_AURA_PROC_FAILED;

                // mana cost spellmod spell
                CastSpell(this, 45281, true);
                break;
            }
            // Empowered Touch
            else if (dummySpell->SpellIconID == 2251)
            {
                if (!roll_chance_i(triggerAmount))
                    return SPELL_AURA_PROC_FAILED;

                triggered_spell_id = 88433;
            }
            // King of the Jungle
            else if (dummySpell->SpellIconID == 2850)
            {
                if (!procSpell)
                    return SPELL_AURA_PROC_FAILED;

                switch (effIndex)
                {
                    case EFFECT_INDEX_0:    // Enrage (bear)
                    {
                        // note : aura removal is done in SpellAuraHolder::HandleSpellSpecificBoosts
                        basepoints[0] = triggerAmount;
                        triggered_spell_id = 51185;
                        break;
                    }
                    case EFFECT_INDEX_1:    // Tiger's Fury (cat)
                    {
                        basepoints[0] = triggerAmount;
                        triggered_spell_id = 51178;
                        break;
                    }
                    default:
                        return SPELL_AURA_PROC_FAILED;
                }
            }
            // Living Seed
            else if (dummySpell->SpellIconID == 2860)
            {
                triggered_spell_id = 48504;
                basepoints[0] = int32(triggerAmount * damage / 100);
                break;
            }
            // Gift of the Earthmother
            else if (dummySpell->SpellIconID == 3186)
            {
                if (!procSpell || effIndex != EFFECT_INDEX_1)
                    return SPELL_AURA_PROC_FAILED;

                triggered_spell_id = 64801;
                basepoints[0] = int32(triggerAmount * CalculateSpellDamage(pVictim, procSpell, EFFECT_INDEX_0) / 100);
                basepoints[0] = SpellHealingBonusDone(pVictim, procSpell, basepoints[0], DOT);
                basepoints[0] = pVictim->SpellHealingBonusTaken(this, procSpell, basepoints[0], DOT);
                basepoints[0] *= GetSpellAuraMaxTicks(procSpell);
                break;
            }
            // Stampede
            else if (dummySpell->SpellIconID == 3930)
            {
                switch (effIndex)
                {
                    case EFFECT_INDEX_0:    // Feral Charge (Bear)
                    {
                        triggered_spell_id = dummySpell->Id == 78892 ? 81016 : 81017;
                        break;
                    }
                    case EFFECT_INDEX_1:    // Feral Charge (Cat)
                    {
                        // Item - Druid T13 Feral 4P Bonus (Frenzied Regeneration and Stampede)
                        if (!procSpell || procSpell->Id == 5217 && !HasAura(105735))
                            return SPELL_AURA_PROC_FAILED;

                        triggered_spell_id = dummySpell->Id == 78892 ? 81021 : 81022;
                        break;
                    }
                    default:
                        return SPELL_AURA_PROC_FAILED;
                }
                break;
            }
            // Blood in the Water
            else if (dummySpell->SpellIconID == 4399)
            {
                if (!pVictim || pVictim->GetHealthPercent() > triggerAmount)
                    return SPELL_AURA_PROC_FAILED;

                // Rip and Ferocius Bite have intersecting class masks
                if (!procSpell || procSpell->Id != 22568)
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            break;
        }
        case SPELLFAMILY_ROGUE:
        {
            switch(dummySpell->Id)
            {
                // Clean Escape
                case 23582:
                    // triggered spell have same masks and etc with main Vanish spell
                    if (!procSpell || procSpell->GetSpellEffectIdByIndex(EFFECT_INDEX_0) == SPELL_EFFECT_NONE)
                        return SPELL_AURA_PROC_FAILED;
                    triggered_spell_id = 23583;
                    break;
                // Deadly Throw Interrupt
                case 32748:
                {
                    // Prevent cast Deadly Throw Interrupt on self from last effect (apply dummy) of Deadly Throw
                    if (this == pVictim)
                        return SPELL_AURA_PROC_FAILED;

                    triggered_spell_id = 32747;
                    break;
                }
                // Glyph of Hemorrhage
                case 56807:
                {
                    basepoints[0] = damage * triggerAmount / 100;
                    break;
                }
                // Tricks of the trade
                case 57934:
                {
                    triggered_spell_id = 57933;             // Tricks of the Trade, increased damage buff
                    target = getHostileRefManager().GetThreatRedirectionTarget();
                    if (!target)
                        return SPELL_AURA_PROC_FAILED;

                    CastSpell(this, 59628, true);           // Tricks of the Trade (caster timer)
                    break;
                }
                case 76806:                                 // Main Gauche
                {
                    if (effIndex != EFFECT_INDEX_0 || !roll_chance_i(triggerAmount))
                        return SPELL_AURA_PROC_FAILED;

                    triggered_spell_id = 86392;
                    break;
                }
                case 84617:                                 // Revealing Strike
                {
                    // Check procSpell for Eviscerate, Envenom, Expose Armor, Kidney Shot and Rupture
                    if (!procSpell || !NeedsComboPoints(procSpell))
                        return SPELL_AURA_PROC_FAILED;
                    break;
                }
                case 91299:                                 // Glyph of Blind
                {
                    if (!pVictim || !pVictim->isAlive())
                        return SPELL_AURA_PROC_FAILED;

                    // except shadow word: death periodic
                    pVictim->RemoveSpellsCausingAura(SPELL_AURA_PERIODIC_DAMAGE, pVictim->GetSpellAuraHolder(32409));
                    pVictim->RemoveSpellsCausingAura(SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
                    pVictim->RemoveSpellsCausingAura(SPELL_AURA_PERIODIC_LEECH);
                    break;
                }
                case 99175:                                 // Item - Rogue T12 4P Bonus
                {
                    uint32 spells[] = { 99186, 99187, 99188 };
                    for (int i = 0; i < 3; ++i)
                        RemoveAurasDueToSpell(spells[i]);

                    triggered_spell_id = spells[urand(0, 2)];
                    break;
                }
                case 105849:                                // Item - Rogue T13 2P Bonus (Tricks of the Trade)
                {
                    triggered_spell_id = 105964;
                    break;
                }
            }
            // Murderous Intent
            if (dummySpell->SpellIconID == 134)
            {
                if (triggeredByAura->GetEffIndex() != EFFECT_INDEX_0)
                    return SPELL_AURA_PROC_FAILED;

                if (!pVictim || pVictim->GetHealthPercent() > dummySpell->CalculateSimpleValue(EFFECT_INDEX_1))
                    return SPELL_AURA_PROC_FAILED;

                triggered_spell_id = 79132;
                basepoints[0] = triggerAmount;
                break;
            }
            // Improved Expose Armor
            else if (dummySpell->SpellIconID == 563)
                return SPELL_AURA_PROC_FAILED;              // done in other way
            // Honor Among Thieves
            else if (dummySpell->SpellIconID == 2903)
            {
                if (GetTypeId() != TYPEID_PLAYER)
                    return SPELL_AURA_PROC_FAILED;

                Unit* caster = triggeredByAura->GetCaster();
                if (!caster || caster->GetTypeId() != TYPEID_PLAYER || caster == this)
                    return SPELL_AURA_PROC_FAILED;

                triggered_spell_id = 51699;
                if (((Player*)caster)->HasSpellCooldown(triggered_spell_id))
                    return SPELL_AURA_PROC_FAILED;

                int32 cd = 4000;
                ((Player*)caster)->ApplySpellMod(triggered_spell_id, SPELLMOD_COOLDOWN, cd);

                if (Unit* target = caster->GetMap()->GetUnit(((Player*)caster)->GetSelectionGuid()))
                    caster->CastSpell(target, 51699, true);

                ((Player*)caster)->AddSpellCooldown(triggered_spell_id, 0, time(NULL) + cd / IN_MILLISECONDS);
                return SPELL_AURA_PROC_OK;
            }
            // Cut to the Chase
            else if (dummySpell->SpellIconID == 2909)
            {
                // "refresh your Slice and Dice duration to its 5 combo point maximum"
                // lookup Slice and Dice
                if (SpellAuraHolder* holder = GetSpellAuraHolder(5171))
                {
                    int32 duration = GetSpellMaxDuration(holder->GetSpellProto());
                    if (GetTypeId() == TYPEID_PLAYER)
                        ((Player*)this)->ApplySpellMod(holder->GetId(), SPELLMOD_DURATION, duration);

                    holder->SetAuraMaxDuration(duration);
                    holder->RefreshHolder();
                    return SPELL_AURA_PROC_OK;
                }

                return SPELL_AURA_PROC_FAILED;
            }
            // Deadly Brew
            else if (dummySpell->SpellIconID == 2963)
            {
                triggered_spell_id = 44289;
                break;
            }
            // Bandit's Guile
            else if (dummySpell->SpellIconID == 2983)
            {
                basepoints[0] = 0;
                triggered_spell_id = 84748;

                // Insight buffs
                uint32 buffs[4] = { 84745, 84746, 84747, 0 };

                if (pVictim->HasAura(triggered_spell_id))
                {
                    int i = 0;
                    bool found = false;
                    for (; i < 3; ++i)
                    {
                        if (SpellAuraHolder* holder = GetSpellAuraHolder(buffs[i]))
                        {
                            found = true;
                            RemoveSpellAuraHolder(holder);
                            break;
                        }
                    }

                    if (found)
                        ++i;
                    else
                        i = 0;

                    if (SpellEntry const * spell = sSpellStore.LookupEntry(buffs[i]))
                    {
                        CastSpell(this, spell, true);
                        basepoints[0] = spell->CalculateSimpleValue(EFFECT_INDEX_0);
                    }
                }
                else
                {
                    for (int i = 0; i < 3; ++i)
                        RemoveAurasDueToSpell(buffs[i]);
                }
                break;
            }
            // Quick Recovery
            else if (dummySpell->SpellIconID == 2116)
            {
                if(!procSpell)
                    return SPELL_AURA_PROC_FAILED;

                //do not proc from spells that do not need combo points
                if(!NeedsComboPoints(procSpell))
                    return SPELL_AURA_PROC_FAILED;

                // energy cost save
                basepoints[0] = procSpell->GetManaCost() * triggerAmount/100;
                if (basepoints[0] <= 0)
                    return SPELL_AURA_PROC_FAILED;

                target = this;
                triggered_spell_id = 31663;
                break;
            }
            // Venomous Wounds
            else if (dummySpell->SpellIconID == 4888)
            {
                if (!pVictim || triggeredByAura->GetEffIndex() != EFFECT_INDEX_1)
                    return SPELL_AURA_PROC_FAILED;

                // search poison
                bool found = false;
                if (pVictim->HasAuraState(AURA_STATE_DEADLY_POISON))
                    found = true;
                else
                {
                    Unit::SpellAuraHolderMap const& auras = pVictim->GetSpellAuraHolderMap();
                    for (Unit::SpellAuraHolderMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
                    {
                        if (itr->second->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_ROGUE &&
                            itr->second->GetSpellProto()->GetDispel() == DISPEL_POISON &&
                            itr->second->GetCasterGuid() == GetObjectGuid())
                        {
                            found = true;
                            break;
                        }
                    }
                }
                if (!found)
                    return SPELL_AURA_PROC_FAILED;

                // Venomous Wound
                CastSpell(pVictim, 79136, true);
                // Venomous Vim
                triggered_spell_id = 51637;
                basepoints[0] = triggerAmount;
                break;
            }
            // Restless Blades
            else if (dummySpell->SpellIconID == 4897)
            {
                if (!procSpell || GetTypeId() != TYPEID_PLAYER || !NeedsComboPoints(procSpell))
                    return SPELL_AURA_PROC_FAILED;

                int32 amt = GetComboPoints() * triggerAmount;
                ((Player*)this)->SendModifyCooldown(2983, -amt);    // Sprint
                ((Player*)this)->SendModifyCooldown(13750, -amt);   // Adrenaline Rush
                ((Player*)this)->SendModifyCooldown(51690, -amt);   // Shadowstep
                ((Player*)this)->SendModifyCooldown(73981, -amt);   // Redirect

                return SPELL_AURA_PROC_OK;
            }
            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            // Improved Serpent Sting
            if (dummySpell->SpellIconID == 536)
            {
                if (!procSpell)
                    return SPELL_AURA_PROC_FAILED;

                triggered_spell_id = 83077;
                basepoints[0] = int32(triggerAmount * CalculateSpellDamage(pVictim, procSpell, EFFECT_INDEX_0) / 100);
                basepoints[0] = SpellDamageBonusDone(pVictim, procSpell, basepoints[0], DOT);
                basepoints[0] = pVictim->SpellDamageBonusTaken(this, procSpell, basepoints[0], DOT);
                basepoints[0] *= GetSpellAuraMaxTicks(procSpell);
                break;
            }
            // Thrill of the Hunt
            else if (dummySpell->SpellIconID == 2236)
            {
                if (!procSpell)
                    return SPELL_AURA_PROC_FAILED;

                // mana cost save
                float focus = procSpell->GetManaCost() + procSpell->GetManaCostPercentage() * GetCreatePowers(POWER_FOCUS) / 100;

                // Black Arrow and Explosive Shot returns only part of $triggerAmount% per critical
                if (procSpell->Id == 3674 || procSpell->Id == 53301)
                {
                    if ((procFlag & PROC_FLAG_ON_DO_PERIODIC) == 0)
                        return SPELL_AURA_PROC_FAILED;

                    if (int32 ticks = GetSpellAuraMaxTicks(procSpell->Id))
                        focus /= ticks;
                }

                basepoints[0] = int32(focus * triggerAmount / 100);
                if (basepoints[0] <= 0)
                    return SPELL_AURA_PROC_FAILED;

                triggered_spell_id = 34720;
                break;
            }
            // Wild Quiver
            else if (dummySpell->SpellIconID == 2935)
            {
                if (triggeredByAura->GetEffIndex() != EFFECT_INDEX_0)
                    return SPELL_AURA_PROC_FAILED;

                if (!roll_chance_i(triggerAmount))
                    return SPELL_AURA_PROC_FAILED;

                triggered_spell_id = 76663;
                break;
            }
            // Improved Steady Shot
            else if (dummySpell->SpellIconID == 3409)
            {
                basepoints[0] = triggeredByAura->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_0);

                Modifier* mod = triggeredByAura->GetModifier();
                if (procSpell->Id == 56641)
                {
                    mod->m_amount *= 2;
                    if (mod->m_amount <= basepoints[0] * 2)
                        return SPELL_AURA_PROC_OK;

                    CastCustomSpell(this, 53220, &basepoints[0], NULL, NULL, true, NULL, triggeredByAura);
                }

                mod->m_amount = basepoints[0];
                return SPELL_AURA_PROC_OK;
            }
            // Marked for Death
            else if (dummySpell->SpellIconID == 3524)
            {
                if (!roll_chance_i(triggerAmount))
                    return SPELL_AURA_PROC_FAILED;

                triggered_spell_id = 88691;
                break;
            }
            // Rapid Recuperation
            else if (dummySpell->SpellIconID == 3560)
            {
                target = this;
                basepoints[0] = triggerAmount;
                triggered_spell_id = 58883;
                break;
            }
            // Crouching Tiger, Hidden Chimera
            else if (dummySpell->SpellIconID == 4752)
            {
                if (GetTypeId() != TYPEID_PLAYER)
                    return SPELL_AURA_PROC_FAILED;

                if (triggeredByAura->GetEffIndex() == EFFECT_INDEX_0 &&
                    (procFlag & (PROC_FLAG_TAKEN_MELEE_HIT | PROC_FLAG_TAKEN_MELEE_SPELL_HIT)) == 0)
                    return SPELL_AURA_PROC_FAILED;
                else if (triggeredByAura->GetEffIndex() == EFFECT_INDEX_1 &&
                    (procFlag & (PROC_FLAG_TAKEN_MELEE_HIT | PROC_FLAG_TAKEN_MELEE_SPELL_HIT)) != 0)
                    return SPELL_AURA_PROC_FAILED;

                if (((Player*)this)->HasSpellCooldown(dummySpell->Id))
                    return SPELL_AURA_PROC_FAILED;

                ((Player*)this)->AddSpellCooldown(dummySpell->Id, 0, time(NULL) + 2);

                // modify cooldown of Disengage or Deterrence
                ((Player*)this)->SendModifyCooldown(triggeredByAura->GetEffIndex() == EFFECT_INDEX_0 ?
                    781 : 19263, -triggerAmount);
                return SPELL_AURA_PROC_OK;
            }
            // Killing Streak
            else if (dummySpell->SpellIconID == 4980)
            {
                if (triggeredByAura->GetEffIndex() != EFFECT_INDEX_0)
                    return SPELL_AURA_PROC_OK;

                // Count spell criticals in a row
                Modifier* mod = triggeredByAura->GetModifier();
                if (procEx & PROC_EX_CRITICAL_HIT)
                {
                    mod->m_amount *= 2;
                    if (mod->m_amount < 60) // not enough
                        return SPELL_AURA_PROC_OK;

                    uint32 spellId = dummySpell->Id == 82748 ? 94006 : 94007;
                    CastSpell(this, spellId, true, NULL, triggeredByAura);
                }

                mod->m_amount = 15;
                return SPELL_AURA_PROC_OK;
            }
            // Sic 'Em!
            else if (dummySpell->SpellIconID == 5092)
            {
                triggered_spell_id = 89388;
                break;
            }
            // Posthaste
            else if (dummySpell->SpellIconID == 5094)
            {
                basepoints[0] = triggerAmount;
                triggered_spell_id = 83559;
            }

            switch(dummySpell->Id)
            {
                // Improved Mend Pet
                case 19572:
                case 19573:
                {
                    if(!roll_chance_i(triggerAmount))
                        return SPELL_AURA_PROC_FAILED;

                    triggered_spell_id = 24406;
                    break;
                }
                // Misdirection
                case 34477:
                {
                    triggered_spell_id = 35079;                 // 4 sec buff on self
                    target = this;
                    break;
                }
                // Improved Kill Command - Item set bonus
                case 37483:
                {
                    triggered_spell_id = 37482;                 // Exploited Weakness
                    break;
                }
                case 82661:                                     // Aspect of the Fox
                {
                    basepoints[0] = triggerAmount;

                    // search One with Nature
                    Unit::AuraList const& mDummyAuras = GetAurasByType(SPELL_AURA_DUMMY);
                    for (Unit::AuraList::const_iterator itr = mDummyAuras.begin(); itr != mDummyAuras.end(); ++itr)
                    {
                        if ((*itr)->GetSpellProto()->SpellIconID == 5080 && (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_HUNTER)
                        {
                            basepoints[0] += (*itr)->GetModifier()->m_amount;
                            break;
                        }
                    }

                    triggered_spell_id = 99289;
                    break;
                }
                case 82897:                                     // Resistance is Futile!
                {
                    triggered_spell_id = 86316;
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            // Seal of Righteousness - melee proc dummy (addition $MWS*(0.011*$AP+0.022*$SPH) damage)
            if (dummySpell->Id == 20154 && effIndex == EFFECT_INDEX_0)
            {
                triggered_spell_id = 25742;
                float ap = GetTotalAttackPowerValue(BASE_ATTACK);
                int32 holy = SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_HOLY);
                if (holy < 0)
                    holy = 0;

                basepoints[0] = int32(GetAttackTime(BASE_ATTACK) * (ap * 0.011f + 0.022f * holy) / 1000);
                break;
            }
            // Seal of Justice - melee proc dummy (addition $MWS*(0.005*$AP+0.01*$SPH) damage)
            else if (dummySpell->Id == 20164)
            {
                triggered_spell_id = 20170;
                float ap = GetTotalAttackPowerValue(BASE_ATTACK);
                int32 holy = SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_HOLY);
                if (holy < 0)
                    holy = 0;

                basepoints[1] = int32(GetAttackTime(BASE_ATTACK) * (ap * 0.005f + 0.01f * holy) / 1000);
                break;
            }
            // Vengeance (paladin)
            else if (dummySpell->Id == 84839)
                return HandleVengeanceProc(pVictim, damage, triggerAmount);
            // Item - Paladin T12 Holy 4P Bonus
            else if(dummySpell->Id == 99070)
            {
                if (!procSpell)
                    return SPELL_AURA_PROC_FAILED;
                triggered_spell_id = procSpell->Id;
                basepoints[0] = triggerAmount * damage / 100;
                target = SelectRandomFriendlyTarget(pVictim, 15.0f);
                if (!target)
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            // Item - Paladin T12 Protection 2P Bonus
            else if (dummySpell->Id == 99074)
            {
                triggered_spell_id = 99075;
                basepoints[0] = triggerAmount * damage / 100;
            }
            // Item - Paladin T12 Retribution 2P Bonus
            else if (dummySpell->Id == 99093)
            {
                triggered_spell_id = 99092;
                basepoints[0] = triggerAmount * damage / 100;
            }

            // Divine Purpose
            if (dummySpell->SpellIconID == 2170)
            {
                if (!roll_chance_i(triggerAmount))
                    return SPELL_AURA_PROC_FAILED;

                triggered_spell_id = 90174;
                break;
            }
            // Tower of Radiance, Rank 3
            else if (dummySpell->SpellIconID == 3402)
            {
                // Must be target of Beacon of Light
                if (!pVictim || !pVictim->GetSpellAuraHolder(53563, GetObjectGuid()))
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            // Selfless Healer
            else if (dummySpell->SpellIconID == 3924)
            {
                target = this;
                basepoints[0] = triggerAmount;
                if (Spell* spell = GetCurrentSpell(CURRENT_GENERIC_SPELL))
                    // Word of Glory
                    if (spell->m_spellInfo->Id == 85673)
                        basepoints[0] *= spell->GetUsedHolyPower();
                break;
            }

            switch (dummySpell->Id)
            {
                // Judgement of Light
                case 20185:
                {
                    // only at real damage
                    if (!damage || pVictim == this)
                        return SPELL_AURA_PROC_FAILED;

                    basepoints[0] = int32( pVictim->GetMaxHealth() * triggeredByAura->GetModifier()->m_amount / 100 );
                    pVictim->CastCustomSpell(pVictim, 20267, &basepoints[0], NULL, NULL, true, NULL, triggeredByAura);
                    return SPELL_AURA_PROC_OK;
                }
                // Judgement of Wisdom
                case 20186:
                {
                    // only at real damage
                    if (!damage)
                        return SPELL_AURA_PROC_FAILED;

                    if (pVictim->getPowerType() == POWER_MANA)
                    {
                        // 2% of maximum base mana
                        basepoints[0] = int32(pVictim->GetCreateMana() * 2 / 100);
                        pVictim->CastCustomSpell(pVictim, 20268, &basepoints[0], NULL, NULL, true, NULL, triggeredByAura);
                    }
                    return SPELL_AURA_PROC_OK;
                }
                // Heart of the Crusader (Rank 1)
                case 20335:
                    triggered_spell_id = 21183;
                    break;
                // Heart of the Crusader (Rank 2)
                case 20336:
                    triggered_spell_id = 54498;
                    break;
                // Heart of the Crusader (Rank 3)
                case 20337:
                    triggered_spell_id = 54499;
                    break;
                // Holy Power (Redemption Armor set)
                case 28789:
                {
                    if(!pVictim)
                        return SPELL_AURA_PROC_FAILED;

                    // Set class defined buff
                    switch (pVictim->getClass())
                    {
                        case CLASS_PALADIN:
                        case CLASS_PRIEST:
                        case CLASS_SHAMAN:
                        case CLASS_DRUID:
                            triggered_spell_id = 28795;     // Increases the friendly target's mana regeneration by $s1 per 5 sec. for $d.
                            break;
                        case CLASS_MAGE:
                        case CLASS_WARLOCK:
                            triggered_spell_id = 28793;     // Increases the friendly target's spell damage and healing by up to $s1 for $d.
                            break;
                        case CLASS_HUNTER:
                        case CLASS_ROGUE:
                            triggered_spell_id = 28791;     // Increases the friendly target's attack power by $s1 for $d.
                            break;
                        case CLASS_WARRIOR:
                            triggered_spell_id = 28790;     // Increases the friendly target's armor
                            break;
                        default:
                            return SPELL_AURA_PROC_FAILED;
                    }
                    break;
                }
                // Spiritual Attunement
                case 31785:
                case 33776:
                {
                    // if healed by another unit (pVictim)
                    if (this == pVictim)
                        return SPELL_AURA_PROC_FAILED;

                    // dont count overhealing
                    uint32 diff = GetMaxHealth()-GetHealth();
                    if (!diff)
                        return SPELL_AURA_PROC_FAILED;
                    basepoints[0] = triggerAmount * (damage > diff ? diff : damage) / 100;
                    target = this;
                    triggered_spell_id = 31786;
                    break;
                }
                // Seal of Truth (damage calc on apply aura)
                case 31801:
                {
                    if (effIndex != EFFECT_INDEX_0)         // effect 1,2 used by seal unleashing code
                        return SPELL_AURA_PROC_FAILED;

                    // At melee attack or Hammer of the Righteous spell damage considered as melee attack
                    if ((procFlag & PROC_FLAG_SUCCESSFUL_MELEE_HIT) || procSpell && procSpell->Id == 53595)
                        triggered_spell_id = 31803;         // Censure

                    // Add 5-stack effect from Censure
                    if (SpellAuraHolder* holder = target->GetSpellAuraHolder(triggered_spell_id, GetObjectGuid()))
                        if (holder->GetStackAmount() >= holder->GetSpellProto()->GetStackAmount())
                            CastSpell(target, 42463, true, NULL, triggeredByAura);
                    break;
                }
                // Judgements of the Wise
                case 31878:
                    // triggered only at casted Judgement spells, not at additional Judgement effects
                    if (!procSpell || procSpell->GetCategory() != 1210)
                        return SPELL_AURA_PROC_FAILED;

                    target = this;
                    triggered_spell_id = 31930;
                    break;
                // Paladin Tier 6 Trinket (Ashtongue Talisman of Zeal)
                case 40470:
                {
                    if (!procSpell)
                        return SPELL_AURA_PROC_FAILED;

                    float  chance;

                    // Flash of light/Holy light
                    if (procClassOptions && procClassOptions->SpellFamilyFlags & UI64LIT(0x00000000C0000000))
                    {
                        triggered_spell_id = 40471;
                        chance = 15.0f;
                    }
                    // Judgement (any)
                    else if (GetSpellSpecific(procSpell->Id)==SPELL_JUDGEMENT)
                    {
                        triggered_spell_id = 40472;
                        chance = 50.0f;
                    }
                    else
                        return SPELL_AURA_PROC_FAILED;

                    if (!roll_chance_f(chance))
                        return SPELL_AURA_PROC_FAILED;

                    break;
                }
                // Light's Beacon (heal target area aura)
                case 53651:
                {
                    // not do bonus heal for explicit beacon focus healing
                    if (GetObjectGuid() == triggeredByAura->GetCasterGuid())
                        return SPELL_AURA_PROC_FAILED;

                    // beacon
                    Unit* beacon = triggeredByAura->GetCaster();
                    if (!beacon || !procSpell || procSpell->Id == 20267)
                        return SPELL_AURA_PROC_FAILED;

                    // find caster main aura at beacon
                    Aura* dummy = NULL;
                    Unit::AuraList const& baa = beacon->GetAurasByType(SPELL_AURA_PERIODIC_TRIGGER_SPELL);
                    for(Unit::AuraList::const_iterator i = baa.begin(); i != baa.end(); ++i)
                    {
                        if ((*i)->GetId() == 53563 && (*i)->GetCasterGuid() == pVictim->GetObjectGuid())
                        {
                            dummy = (*i);
                            break;
                        }
                    }

                    // original heal must be form beacon caster
                    if (!dummy)
                        return SPELL_AURA_PROC_FAILED;

                    triggered_spell_id = 53652;             // Beacon of Light

                    uint32 radius = GetSpellMaxRange(sSpellRangeStore.LookupEntry(sSpellStore.LookupEntry(triggered_spell_id)->rangeIndex));

                    if (!beacon->IsWithinDistInMap(this, radius) ||
                        !beacon->IsWithinLOSInMap(this) ||
                        !beacon->IsWithinDistInMap(pVictim, radius) ||
                        !beacon->IsWithinLOSInMap(pVictim))
                        return SPELL_AURA_PROC_FAILED;

                    basepoints[0] = triggeredByAura->GetModifier()->m_amount * damage / 100;
                    // Holy Light heals for 100%
                    if (procSpell && procSpell->Id == 635)
                        basepoints[0] *= 2;

                    // cast with original caster set but beacon to beacon for apply caster mods and avoid LoS check
                    beacon->CastCustomSpell(beacon, triggered_spell_id, &basepoints[0], NULL, NULL, true, castItem, triggeredByAura, pVictim->GetObjectGuid());
                    return SPELL_AURA_PROC_OK;
                }
                // Item - Paladin T8 Holy 2P Bonus
                case 64890:
                {
                    triggered_spell_id = 64891;             // Holy Mending
                    basepoints[0] = int32(triggerAmount * damage / 100) / GetSpellAuraMaxTicks(triggered_spell_id);
                    break;
                }
                // Anger Capacitor
                case 71406:                                 // normal
                case 71545:                                 // heroic
                {
                    if (!pVictim)
                        return SPELL_AURA_PROC_FAILED;

                    SpellEntry const* mote = sSpellStore.LookupEntry(71432);
                    if (!mote)
                        return SPELL_AURA_PROC_FAILED;
                    uint32 maxStack = mote->GetStackAmount() - (dummySpell->Id == 71545 ? 1 : 0);

                    SpellAuraHolder *aurHolder = GetSpellAuraHolder(71432);
                    if (aurHolder && uint32(aurHolder->GetStackAmount() +1) >= maxStack)
                    {
                        RemoveAurasDueToSpell(71432);       // Mote of Anger

                        // Manifest Anger (main hand/off hand)
                        CastSpell(pVictim, !haveOffhandWeapon() || roll_chance_i(50) ? 71433 : 71434, true);
                        return SPELL_AURA_PROC_OK;
                    }
                    else
                        triggered_spell_id = 71432;

                    break;
                }
                // Heartpierce, Item - Icecrown 25 Normal Dagger Proc
                case 71880:
                {
                    if(GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    switch (this->getPowerType())
                    {
                        case POWER_ENERGY:      triggered_spell_id = 71882; break;
                        case POWER_RAGE:        triggered_spell_id = 71883; break;
                        case POWER_MANA:        triggered_spell_id = 71881; break;
                        case POWER_RUNIC_POWER: triggered_spell_id = 71884; break;
                        default:
                            return SPELL_AURA_PROC_FAILED;
                    }
                    break;
                }
                // Heartpierce, Item - Icecrown 25 Heroic Dagger Proc
                case 71892:
                {
                    if(GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    switch (this->getPowerType())
                    {
                        case POWER_ENERGY:      triggered_spell_id = 71887; break;
                        case POWER_RAGE:        triggered_spell_id = 71886; break;
                        case POWER_MANA:        triggered_spell_id = 71888; break;
                        case POWER_RUNIC_POWER: triggered_spell_id = 71885; break;
                        default:
                            return SPELL_AURA_PROC_FAILED;
                    }
                    break;
                }
                // Illuminated Healing
                case 76669:
                {
                    if (effIndex != EFFECT_INDEX_0)
                        return SPELL_AURA_PROC_FAILED;

                    triggered_spell_id = 86273;
                    int32 maxAmt = GetHealth() / 3;
                    if (SpellAuraHolder* oldHolder = pVictim->GetSpellAuraHolder(triggered_spell_id, GetObjectGuid()))
                        if (Aura* oldAura = oldHolder->GetAuraByEffectIndex(EFFECT_INDEX_0))
                            basepoints[0] = oldAura->GetModifier()->m_amount;

                    basepoints[0] += int32(triggerAmount * damage / 100);
                    // Must not exceed 1/3 of paladin's health
                    if (basepoints[0] > maxAmt)
                        basepoints[0] = maxAmt;
                    break;
                }
                // Ancient Healer
                case 86674:
                {
                    Unit* guard = FindGuardianWithEntry(46499);
                    if (!guard)
                        return SPELL_AURA_PROC_FAILED;

                    basepoints[0] = int32(damage * 10 / 100);
                    guard->CastCustomSpell(pVictim, 86678, &basepoints[0], &basepoints[0], NULL, true, NULL, triggeredByAura, GetObjectGuid());

                    Modifier* mod = triggeredByAura->GetModifier();
                    mod -= 10;
                    if (mod <= 0)
                        RemoveAurasDueToSpell(86674);

                    return SPELL_AURA_PROC_OK;
                }
                // Ancient Crusader
                case 86701:
                {
                    triggered_spell_id = 86700;     // Ancient Power
                    break;
                }
                // Ancient Crusader
                case 86703:
                {
                    Unit* creator = GetCreator();
                    if (!creator)
                        return SPELL_AURA_PROC_FAILED;

                    // Ancient Power
                    creator->CastSpell(creator, 86700, true);
                    return SPELL_AURA_PROC_OK;
                }
                // Judgements of the Bold
                case 89901:
                    // triggered only at casted Judgement spells, not at additional Judgement effects
                    if (!procSpell || procSpell->GetCategory() != 1210)
                        return SPELL_AURA_PROC_FAILED;

                    target = this;
                    triggered_spell_id = 89906;
                    break;
                // Item - Collecting Mana
                case 92272:
                {
                    if (!procSpell || triggeredByAura->GetEffIndex() != EFFECT_INDEX_1)
                        return SPELL_AURA_PROC_FAILED;

                    uint32 triggeredSpellId = 92596;

                    // _base_ mana cost save
                    int32 mana = procSpell->GetManaCost() + procSpell->GetManaCostPercentage() * GetCreateMana() / 100;
                    mana = mana * triggeredByAura->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_0) / 100;
                    if (Aura* aura = GetAura(triggeredSpellId, EFFECT_INDEX_0))
                    {
                        mana += aura->GetModifier()->m_amount;
                        if (mana > triggerAmount)
                            aura->ChangeAmount(triggerAmount);
                        else
                            aura->ChangeAmount(mana);
                        return SPELL_AURA_PROC_OK;
                    }

                    if (mana > triggerAmount)
                        mana = triggerAmount;

                    CastCustomSpell(this, triggeredSpellId, &mana, NULL, NULL, true, NULL, triggeredByAura);
                    return SPELL_AURA_PROC_OK;
                }
            }
            break;
        }
        case SPELLFAMILY_SHAMAN:
        {
            switch(dummySpell->Id)
            {
                // Totemic Power (The Earthshatterer set)
                case 28823:
                {
                    if( !pVictim )
                        return SPELL_AURA_PROC_FAILED;

                    // Set class defined buff
                    switch (pVictim->getClass())
                    {
                        case CLASS_PALADIN:
                        case CLASS_PRIEST:
                        case CLASS_SHAMAN:
                        case CLASS_DRUID:
                            triggered_spell_id = 28824;     // Increases the friendly target's mana regeneration by $s1 per 5 sec. for $d.
                            break;
                        case CLASS_MAGE:
                        case CLASS_WARLOCK:
                            triggered_spell_id = 28825;     // Increases the friendly target's spell damage and healing by up to $s1 for $d.
                            break;
                        case CLASS_HUNTER:
                        case CLASS_ROGUE:
                            triggered_spell_id = 28826;     // Increases the friendly target's attack power by $s1 for $d.
                            break;
                        case CLASS_WARRIOR:
                            triggered_spell_id = 28827;     // Increases the friendly target's armor
                            break;
                        default:
                            return SPELL_AURA_PROC_FAILED;
                    }
                    break;
                }
                // Lesser Healing Wave (Totem of Flowing Water Relic)
                case 28849:
                {
                    target = this;
                    triggered_spell_id = 28850;
                    break;
                }
                // Windfury Weapon (Passive) 1-5 Ranks
                case 33757:
                {
                    if(GetTypeId()!=TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    if(!castItem || !castItem->IsEquipped())
                        return SPELL_AURA_PROC_FAILED;

                    // custom cooldown processing case
                    if( cooldown && ((Player*)this)->HasSpellCooldown(dummySpell->Id))
                        return SPELL_AURA_PROC_FAILED;

                    if (castItem->GetSlot() != EQUIPMENT_SLOT_MAINHAND && castItem->GetSlot() != EQUIPMENT_SLOT_OFFHAND ||
                        !isAttackReady(castItem->GetSlot() == EQUIPMENT_SLOT_MAINHAND ? BASE_ATTACK : OFF_ATTACK))
                        return SPELL_AURA_PROC_FAILED;

                    // Now amount of extra power stored in 1 effect of Enchant spell
                    // Get it by item enchant id
                    uint32 spellId;
                    switch (castItem->GetEnchantmentId(EnchantmentSlot(TEMP_ENCHANTMENT_SLOT)))
                    {
                        case 283: spellId =  8232; break;   // 1 Rank
                        case 284: spellId =  8235; break;   // 2 Rank
                        case 525: spellId = 10486; break;   // 3 Rank
                        case 1669:spellId = 16362; break;   // 4 Rank
                        case 2636:spellId = 25505; break;   // 5 Rank
                        case 3785:spellId = 58801; break;   // 6 Rank
                        case 3786:spellId = 58803; break;   // 7 Rank
                        case 3787:spellId = 58804; break;   // 8 Rank
                        default:
                        {
                            ERROR_LOG("Unit::HandleDummyAuraProc: non handled item enchantment (rank?) %u for spell id: %u (Windfury)",
                                castItem->GetEnchantmentId(EnchantmentSlot(TEMP_ENCHANTMENT_SLOT)),dummySpell->Id);
                            return SPELL_AURA_PROC_FAILED;
                        }
                    }

                    SpellEntry const* windfurySpellEntry = sSpellStore.LookupEntry(spellId);
                    if(!windfurySpellEntry)
                    {
                        ERROR_LOG("Unit::HandleDummyAuraProc: nonexistent spell id: %u (Windfury)",spellId);
                        return SPELL_AURA_PROC_FAILED;
                    }

                    int32 extra_attack_power = CalculateSpellDamage(pVictim, windfurySpellEntry, EFFECT_INDEX_1);

                    // Totem of Splintering
                    if (Aura* aura = GetAura(60764, EFFECT_INDEX_0))
                        extra_attack_power += aura->GetModifier()->m_amount;

                    // Main-Hand case
                    if (castItem->GetSlot() == EQUIPMENT_SLOT_MAINHAND)
                    {
                        // Value gained from additional AP
                        basepoints[0] = int32(extra_attack_power/14.0f * GetAttackTime(BASE_ATTACK)/1000);
                        triggered_spell_id = 25504;
                    }
                    // Off-Hand case
                    else
                    {
                        // Value gained from additional AP
                        basepoints[0] = int32(extra_attack_power/14.0f * GetAttackTime(OFF_ATTACK)/1000/2);
                        triggered_spell_id = 33750;
                    }

                    // apply cooldown before cast to prevent processing itself
                    if( cooldown )
                        ((Player*)this)->AddSpellCooldown(dummySpell->Id,0,time(NULL) + cooldown);

                    // Attack Twice
                    for ( uint32 i = 0; i<2; ++i )
                        CastCustomSpell(pVictim,triggered_spell_id,&basepoints[0],NULL,NULL,true,castItem,triggeredByAura);

                    return SPELL_AURA_PROC_OK;
                }
                // Shaman Tier 6 Trinket
                case 40463:
                {
                    if( !procSpell )
                        return SPELL_AURA_PROC_FAILED;

                    float chance;
                    if (procClassOptions && procClassOptions->SpellFamilyFlags & UI64LIT(0x0000000000000001))
                    {
                        triggered_spell_id = 40465;         // Lightning Bolt
                        chance = 15.0f;
                    }
                    else if (procClassOptions && procClassOptions->SpellFamilyFlags & UI64LIT(0x0000000000000080))
                    {
                        triggered_spell_id = 40465;         // Lesser Healing Wave
                        chance = 10.0f;
                    }
                    else if (procClassOptions && procClassOptions->SpellFamilyFlags & UI64LIT(0x0000001000000000))
                    {
                        triggered_spell_id = 40466;         // Stormstrike
                        chance = 50.0f;
                    }
                    else
                        return SPELL_AURA_PROC_FAILED;

                    if (!roll_chance_f(chance))
                        return SPELL_AURA_PROC_FAILED;

                    target = this;
                    break;
                }
                // Glyph of Healing Wave
                case 55440:
                {
                    // Not proc from self heals
                    if (this==pVictim)
                        return SPELL_AURA_PROC_FAILED;
                    basepoints[0] = triggerAmount * damage / 100;
                    target = this;
                    triggered_spell_id = 55533;
                    break;
                }
                // Spirit Hunt
                case 58877:
                {
                    // Cast on owner
                    if(GetOwner() && pVictim != GetOwner())
                        HandleDummyAuraProc(GetOwner(), damage, absorb, triggeredByAura, procSpell, procFlag, procEx, cooldown);
                    
                    if(target != GetOwner()) 
                        target = this;

                    // is it correct?
                    basepoints[0] = triggerAmount * (damage+absorb) / 100;
                    triggered_spell_id = 58879;
                    break;
                }
                // Item - Shaman T8 Elemental 4P Bonus
                case 64928:
                {
                    triggered_spell_id = 64930;            // Electrified
                    basepoints[0] = int32(triggerAmount * (damage+absorb) / 100) / GetSpellAuraMaxTicks(triggered_spell_id);
                    break;
                }
                // Item - Shaman T9 Elemental 4P Bonus (Lava Burst)
                case 67228:
                {
                    triggered_spell_id = 71824;             // Lava Burst
                    basepoints[0] = int32(triggerAmount * (damage+absorb) / 100) / GetSpellAuraMaxTicks(triggered_spell_id);
                    break;
                }
                // Item - Shaman T10 Restoration 4P Bonus
                case 70808:
                {
                    triggered_spell_id = 70809;             // Chained Heal
                    basepoints[0] = int32(triggerAmount * damage / 100) / GetSpellAuraMaxTicks(triggered_spell_id);
                    break;
                }
                // Item - Shaman T10 Elemental 2P Bonus
                case 70811:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    if (procSpell->IsFitToFamily(SPELLFAMILY_SHAMAN, UI64LIT(0x0000000000000003)))
                    {
                        ((Player*)this)->SendModifyCooldown(16166,-triggerAmount);
                        return SPELL_AURA_PROC_OK;
                    }
                    return SPELL_AURA_PROC_FAILED;
                }
                // Item - Shaman T10 Elemental 4P Bonus
                case 70817:
                {
                    if (Aura *aur = pVictim->GetAura(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_SHAMAN, UI64LIT(0x0000000010000000), 0, GetObjectGuid()))
                    {
                        int32 maxduration = aur->GetAuraMaxDuration();
                        int32 amount = aur->GetAuraDuration() + aur->GetModifier()->periodictime * 2;
                        if (amount > maxduration)
                            amount = maxduration;

                        aur->GetHolder()->SetAuraDuration(amount);
                        aur->GetHolder()->SendAuraUpdate(false);

                        return SPELL_AURA_PROC_OK;
                    }
                    return SPELL_AURA_PROC_FAILED;
                }
                // Fulmination marker
                case 95774:
                {
                    // Earth Shock
                    if (!procSpell || procSpell->Id != 8042)
                        return SPELL_AURA_PROC_FAILED;

                    // Fulmination dmg spell
                    SpellEntry const * triggeredInfo = sSpellStore.LookupEntry(88767);
                    if (!triggeredInfo)
                        return SPELL_AURA_PROC_OK;

                    int32 minCharges = triggeredInfo->CalculateSimpleValue(EFFECT_INDEX_0);

                    // Lightning Shield
                    SpellAuraHolder* ls = GetSpellAuraHolder(324);
                    int32 charges = ls ? int32(ls->GetAuraCharges()) : 0;
                    if (!ls || charges <= minCharges)
                        return SPELL_AURA_PROC_OK;

                    SpellEffectEntry const * shieldDmgEff = ls->GetSpellProto()->GetSpellEffect(EFFECT_INDEX_0);
                    if (!shieldDmgEff)
                        return SPELL_AURA_PROC_OK;

                    SpellEntry const * shieldDmgEntry = sSpellStore.LookupEntry(shieldDmgEff->EffectTriggerSpell);
                    if (!shieldDmgEntry)
                        return SPELL_AURA_PROC_OK;

                    int32 bp = CalculateSpellDamage(pVictim, shieldDmgEntry, EFFECT_INDEX_0) * (charges - minCharges);
                    CastCustomSpell(pVictim, triggeredInfo, &bp, NULL, NULL, true);
                    ls->SetAuraCharges(minCharges);
                    return SPELL_AURA_PROC_OK;
                }
                // Item - Shaman T13 Enhancement 4P Effect (Feral Spirits)
                case 105873:
                {
                    if (Unit* owner = GetOwner())
                        owner->CastSpell(owner, 53817, true);       // Maelstrom Weapon
                    return SPELL_AURA_PROC_OK;
                }
            }
            // Earth's Grasp
            if (dummySpell->SpellIconID == 20)
            {
                // Earthbind Totem summon only
                if (!procSpell || procSpell->Id != 2484)
                    return SPELL_AURA_PROC_FAILED;

                if (!roll_chance_i(triggerAmount))
                    return SPELL_AURA_PROC_FAILED;

                triggered_spell_id = 64695;
                break;
            }
            // Ancestral Healing
            if (dummySpell->SpellIconID == 200)
            {
                if (!pVictim)
                    return SPELL_AURA_PROC_FAILED;

                int32 maxHp = pVictim->GetMaxHealth() * 10 / 100;
                int32 bp = damage * triggerAmount / 100;

                // Cast Ancestral Vigor
                if (Aura* aura = pVictim->GetAura(105284, EFFECT_INDEX_0))
                {
                    maxHp = std::max(maxHp - aura->GetModifier()->m_amount, 0);
                    bp += aura->GetModifier()->m_amount;
                    if (bp > maxHp)
                        bp = aura->GetModifier()->m_amount;
                    aura->ApplyModifier(false, true);
                    aura->ChangeAmount(bp, false);
                    aura->ApplyModifier(true, true);
                    aura->GetHolder()->RefreshHolder();
                }
                else
                {
                    if (bp > maxHp)
                        bp = maxHp;
                    // Cast Ancestral Vigor (max hp part of talent)
                    CastCustomSpell(pVictim, 105284, &bp, NULL, NULL, true);
                }
                return SPELL_AURA_PROC_OK;
            }
            // Focused Insight
            if (dummySpell->SpellIconID == 4674)
            {
                if (effIndex != EFFECT_INDEX_0)
                    return SPELL_AURA_PROC_FAILED;

                int32 powerCost = 0;
                if (Spell* spell = GetCurrentSpell(CURRENT_GENERIC_SPELL))
                    powerCost = spell->GetPowerCost();

                basepoints[0] = -int32(dummySpell->CalculateSimpleValue(EFFECT_INDEX_0) * powerCost / 100.0f);
                basepoints[1] = dummySpell->CalculateSimpleValue(EFFECT_INDEX_1);
                CastCustomSpell(this, 77800, &basepoints[0], &basepoints[1], &basepoints[1], true);
                CastCustomSpell(this, 96300, &basepoints[1], NULL, NULL, true);
                return SPELL_AURA_PROC_OK;
            }
            // Ancestral Awakening
            if (dummySpell->SpellIconID == 3065)
            {
                triggered_spell_id = 52759;
                basepoints[0] = triggerAmount * damage / 100;
                target = this;
                break;
            }
            // Lava Surge
            if (dummySpell->SpellIconID == 4777)
            {
                triggered_spell_id = 77762;
                break;
            }
            // Telluric Currents
            if (dummySpell->SpellIconID == 320)
            {
                triggered_spell_id = 82987;
                basepoints[0] = int32(damage * triggerAmount / 100);
                target = this;
                break;
            }
            // Tidal Waves
            if (dummySpell->SpellIconID == 3057)
            {
                triggered_spell_id = 53390;
                basepoints[0] = triggerAmount;
                basepoints[1] = triggerAmount;
                target = this;
                break;
            }
            // Resurgence
            if (dummySpell->SpellIconID == 2287)
            {
                if (!procSpell || !HasAura(52127))  // do not proc if no Water Shield aura present
                    return SPELL_AURA_PROC_FAILED;

                triggered_spell_id = 101033;
                // store spell procced by in non-existent effect
                basepoints[1] = int32(procSpell->Id);
                break;
            }
            // Flametongue Weapon (Passive), Ranks
            if (dummyClassOptions && dummyClassOptions->SpellFamilyFlags & UI64LIT(0x0000000000200000))
            {
                if (GetTypeId()!=TYPEID_PLAYER || !castItem)
                    return SPELL_AURA_PROC_FAILED;

                // Only proc for enchanted weapon
                Item *usedWeapon = ((Player *)this)->GetWeaponForAttack(procFlag & PROC_FLAG_SUCCESSFUL_OFFHAND_HIT ? OFF_ATTACK : BASE_ATTACK, true, true);
                if (usedWeapon != castItem)
                    return SPELL_AURA_PROC_FAILED;

                switch (dummySpell->Id)
                {
                    case 10400: triggered_spell_id =  8026; break; // Rank 1
                    case 15567: triggered_spell_id =  8028; break; // Rank 2
                    case 15568: triggered_spell_id =  8029; break; // Rank 3
                    case 15569: triggered_spell_id = 10445; break; // Rank 4
                    case 16311: triggered_spell_id = 16343; break; // Rank 5
                    case 16312: triggered_spell_id = 16344; break; // Rank 6
                    case 16313: triggered_spell_id = 25488; break; // Rank 7
                    case 58784: triggered_spell_id = 58786; break; // Rank 8
                    case 58791: triggered_spell_id = 58787; break; // Rank 9
                    case 58792: triggered_spell_id = 58788; break; // Rank 10
                    default:
                        return SPELL_AURA_PROC_FAILED;
                }
                break;
            }
            // Earth Shield
            if (dummyClassOptions && dummyClassOptions->SpellFamilyFlags & UI64LIT(0x0000040000000000))
            {
                target = this;
                int32 bp0 = triggerAmount;

                Unit* caster = triggeredByAura->GetCaster();

                if (target && caster)
                {
                    bp0 = caster->SpellHealingBonusDone(target, triggeredByAura->GetSpellProto(), bp0, SPELL_DIRECT_DAMAGE);
                    bp0 = target->SpellHealingBonusTaken(caster, triggeredByAura->GetSpellProto(), bp0, SPELL_DIRECT_DAMAGE);
                }

                if (caster)
                {
                    // Glyph of Earth Shield
                    if (Aura* aur = caster->GetDummyAura(63279))
                    {
                        int32 aur_mod = aur->GetModifier()->m_amount;
                        bp0 = int32(bp0 * (aur_mod + 100.0f) / 100.0f);
                    }

                    // Improved Shields & Improved Earth Shield
                    uint8 counter = 0;
                    Unit::AuraList const& mAuras = caster->GetAurasByType(SPELL_AURA_ADD_PCT_MODIFIER);
                    for(Unit::AuraList::const_iterator i = mAuras.begin(); i != mAuras.end() && counter < 2; ++i)
                    {
                        if ((*i)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_SHAMAN && ((*i)->GetSpellProto()->SpellIconID == 19 || (*i)->GetSpellProto()->SpellIconID == 2015) && (*i)->GetEffIndex() == EFFECT_INDEX_1)
                        {
                            int32 aur_mod = (*i)->GetModifier()->m_amount;
                            bp0 = int32(bp0 * (aur_mod + 100.0f) / 100.0f);
                            ++counter;
                        }
                    }
                }

                basepoints[0] = bp0;

                triggered_spell_id = 379;
                break;
            }
            // Flametongue Weapon (Passive)
            if (dummySpell->IsFitToFamilyMask(UI64LIT(0x200000)))
            {
                if(GetTypeId()!=TYPEID_PLAYER)
                    return SPELL_AURA_PROC_FAILED;

                if(!castItem || !castItem->IsEquipped())
                    return SPELL_AURA_PROC_FAILED;

                //  firehit =  dummySpell->EffectBasePoints[0] / ((4*19.25) * 1.3);
                SpellEffectEntry const * effect = dummySpell->GetSpellEffect(EFFECT_INDEX_0);
                float fire_onhit = effect ? effect->EffectBasePoints / 100.0 : 0.0f;

                float add_spellpower = SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_FIRE)
                                     + pVictim->SpellBaseDamageBonusTaken(SPELL_SCHOOL_MASK_FIRE);

                // 1.3speed = 5%, 2.6speed = 10%, 4.0 speed = 15%, so, 1.0speed = 3.84%
                add_spellpower= add_spellpower / 100.0 * 3.84;

                // Enchant on Off-Hand and ready?
                if ( castItem->GetSlot() == EQUIPMENT_SLOT_OFFHAND && isAttackReady(OFF_ATTACK))
                {
                    float BaseWeaponSpeed = GetAttackTime(OFF_ATTACK)/1000.0;

                    // Value1: add the tooltip damage by swingspeed + Value2: add spelldmg by swingspeed
                    basepoints[EFFECT_INDEX_0] = int32( (fire_onhit * BaseWeaponSpeed) + (add_spellpower * BaseWeaponSpeed) );
                    triggered_spell_id = 10444;
                }

                // Enchant on Main-Hand and ready?
                else if ( castItem->GetSlot() == EQUIPMENT_SLOT_MAINHAND && isAttackReady(BASE_ATTACK))
                {
                    float BaseWeaponSpeed = GetAttackTime(BASE_ATTACK)/1000.0;

                    // Value1: add the tooltip damage by swingspeed +  Value2: add spelldmg by swingspeed
                    basepoints[EFFECT_INDEX_0] = int32( (fire_onhit * BaseWeaponSpeed) + (add_spellpower * BaseWeaponSpeed) );
                    triggered_spell_id = 10444;
                }

                // If not ready, we should  return, shouldn't we?!
                else
                    return SPELL_AURA_PROC_FAILED;

                CastCustomSpell(pVictim,triggered_spell_id,&basepoints[EFFECT_INDEX_0],NULL,NULL,true,castItem,triggeredByAura);
                return SPELL_AURA_PROC_OK;
            }
            // Elemental Overload
            if (dummySpell->SpellIconID == 2018 && effIndex == EFFECT_INDEX_0)  // only this spell have SpellFamily Shaman SpellIconID == 2018 and dummy aura
            {
                DEBUG_LOG("Elemental overload: amount %i", triggerAmount);
                if(!procSpell || GetTypeId() != TYPEID_PLAYER || !pVictim || !roll_chance_i(triggerAmount))
                    return SPELL_AURA_PROC_FAILED;

                uint32 spellId = 0;
                int32 bp = (damage+absorb) / 2;
                // Every Lightning Bolt and Chain Lightning spell have duplicate vs half damage and zero cost
                switch (procSpell->Id)
                {
                    // Lightning Bolt
                    case   403: spellId = 45284; break;
                    // Chain Lightning
                    case   421: spellId = 45297; break;
                    // Lava Burst
                    case 51505: spellId = 77451; break;
                    default:
                        return SPELL_AURA_PROC_FAILED;
                }

                // Remove cooldown (Chain Lightning - have Category Recovery time)
                if (procClassOptions && procClassOptions->SpellFamilyFlags & UI64LIT(0x0000000000000002))
                    ((Player*)this)->RemoveSpellCooldown(spellId);

                CastCustomSpell(pVictim, spellId, &bp, NULL, NULL, true, castItem, triggeredByAura, GetObjectGuid());

                if (cooldown && GetTypeId() == TYPEID_PLAYER)
                    ((Player*)this)->AddSpellCooldown(dummySpell->Id, 0, time(NULL) + cooldown);

                // Item - Shaman T13 Elemental 4P Bonus (Elemental Overload)
                if (HasAura(105816))
                    CastSpell(this, 105821, true);  // Time Rupture

                return SPELL_AURA_PROC_OK;
            }
            // Static Shock
            if (dummySpell->SpellIconID == 3059)
            {
                // lookup Lightning Shield
                if (SpellAuraHolder* shield = GetSpellAuraHolder(324, GetObjectGuid()))
                {
                    CastSpell(target, 26364, true, castItem, triggeredByAura);
                    return SPELL_AURA_PROC_OK;
                }
                return SPELL_AURA_PROC_FAILED;
            }
            // Frozen Power
            if (dummySpell->SpellIconID == 3780)
            {
                Unit *caster = triggeredByAura->GetCaster();

                if (!procSpell || !caster)
                    return SPELL_AURA_PROC_FAILED;

                float distance = caster->GetDistance(pVictim);
                int32 chance = triggerAmount;

                if (distance < 15.0f || !roll_chance_i(chance))
                    return SPELL_AURA_PROC_FAILED;

                // make triggered cast apply after current damage spell processing for prevent remove by it
                if(Spell* spell = GetCurrentSpell(CURRENT_GENERIC_SPELL))
                    spell->AddTriggeredSpell(63685);
                return SPELL_AURA_PROC_OK;
            }
            // Feedback
            if (dummySpell->SpellIconID == 4628)
            {
                if (GetTypeId() != TYPEID_PLAYER)
                    return SPELL_AURA_PROC_FAILED;

                ((Player*)this)->SendModifyCooldown(16166, triggerAmount);
                return SPELL_AURA_PROC_OK;
            }
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            // Ebon Plaguebringer
            if (dummySpell->SpellIconID == 1766)
            {
                basepoints[0] = triggerAmount;
                triggered_spell_id = 65142;
            }
            // Butchery
            else if (dummySpell->SpellIconID == 2664)
            {
                basepoints[0] = triggerAmount;
                triggered_spell_id = 50163;
                target = this;
                break;
            }
            // Dancing Rune Weapon
            if (dummySpell->Id == 49028)
            {
                // 1 dummy aura for dismiss rune blade
                if (effIndex != EFFECT_INDEX_1)
                    return SPELL_AURA_PROC_FAILED;
                Pet* runeBlade = FindGuardianWithEntry(27893);
                if (runeBlade && pVictim && damage && procSpell)
                {
                    runeBlade->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                    runeBlade->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                    int32 procDmg = int32((damage+absorb) * 0.5f);
                    runeBlade->CastCustomSpell(pVictim, procSpell->Id, &procDmg, NULL, NULL, true, NULL, NULL, runeBlade->GetObjectGuid());
                    //SendSpellNonMeleeDamageLog(pVictim, procSpell->Id, procDmg, SPELL_SCHOOL_MASK_NORMAL, 0, 0, false, 0, false);
                    break;
                }
                else
                    return SPELL_AURA_PROC_FAILED;
            }
            // Mark of Blood
            if (dummySpell->Id == 49005)
            {
                if (!pVictim || pVictim->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_AURA_PROC_FAILED;

                // TODO: need more info (cooldowns/PPM)
                triggered_spell_id = 61607;
                break;
            }
            // Unholy Blight
            if (dummySpell->Id == 49194)
            {
                basepoints[0] = int32((damage + absorb) * triggerAmount / 100);

                triggered_spell_id = 50536;
                // Split between ticks
                basepoints[0] /= GetSpellAuraMaxTicks(triggered_spell_id);
                break;
            }
            // Vendetta
            if (dummyClassOptions && dummyClassOptions->SpellFamilyFlags & UI64LIT(0x0000000000010000))
            {
                basepoints[0] = triggerAmount * GetMaxHealth() / 100;
                triggered_spell_id = 50181;
                target = this;
                break;
            }
            // Threat of Thassarian
            if (dummySpell->SpellIconID == 2023)
            {
                // Must Dual Wield
                if (!procSpell || !haveOffhandWeapon())
                    return SPELL_AURA_PROC_FAILED;

                // Chance as basepoints for dummy aura
                if (!roll_chance_i(triggerAmount))
                    return SPELL_AURA_PROC_FAILED;

                switch (procSpell->Id)
                {
                    // Obliterate
                    case 49020: triggered_spell_id = 66198; break;
                    // Frost Strike
                    case 49143: triggered_spell_id = 66196; break;
                    // Plague Strike
                    case 45462: triggered_spell_id = 66216; break;
                    // Death Strike
                    case 49998: triggered_spell_id = 66188; break;
                    // Rune Strike
                    case 56815: triggered_spell_id = 66217; break;
                    // Blood Strike
                    case 45902: triggered_spell_id = 66215; break;
                    default:
                        return SPELL_AURA_PROC_FAILED;
                }
                break;
            }
            // Runic Power Back on Snare/Root
            if (dummySpell->Id == 61257)
            {
                // only for spells and hit/crit (trigger start always) and not start from self casted spells
                if (procSpell == 0 || !(procEx & (PROC_EX_NORMAL_HIT|PROC_EX_CRITICAL_HIT)) || this == pVictim)
                    return SPELL_AURA_PROC_FAILED;
                // Need snare or root mechanic
                if (!(GetAllSpellMechanicMask(procSpell) & IMMUNE_TO_ROOT_AND_SNARE_MASK))
                    return SPELL_AURA_PROC_FAILED;
                triggered_spell_id = 61258;
                target = this;
                break;
            }
            // Dark Simulacrum
            else if (dummySpell->Id == 77606)
            {
                if (Unit* caster = triggeredByAura->GetCaster())
                {
                    Spell* spell = GetCurrentSpell(CURRENT_GENERIC_SPELL);
                    if (!spell)
                        spell = GetCurrentSpell(CURRENT_CHANNELED_SPELL);
                    if (spell)
                    {
                        if (spell->IsTriggeredSpell())
                            return SPELL_AURA_PROC_FAILED;

                        SpellEntry const* spellInfo = spell->m_spellInfo;
                        if (spellInfo->powerType != POWER_MANA ||
                            !spellInfo->GetManaCost() && !spellInfo->GetManaCostPercentage())
                            return SPELL_AURA_PROC_FAILED;

                        if (IsSpellHaveEffect(spellInfo, SPELL_EFFECT_SUMMON) ||
                            IsSpellHaveEffect(spellInfo, SPELL_EFFECT_SUMMON_PET) ||
                            IsSpellHaveEffect(spellInfo, SPELL_EFFECT_SUMMON_ALL_TOTEMS) ||
                            IsSpellHaveEffect(spellInfo, SPELL_EFFECT_SUMMON_DEAD_PET) ||
                            IsSpellHaveEffect(spellInfo, SPELL_EFFECT_SUMMON_OBJECT_SLOT) ||
                            IsSpellHaveEffect(spellInfo, SPELL_EFFECT_SURVEY) ||
                            IsSpellHaveEffect(spellInfo, SPELL_EFFECT_SUMMON_RAID_MARKER) ||
                            IsSpellHaveEffect(spellInfo, SPELL_EFFECT_LOOT_CORPSE) ||
                            IsSpellHaveEffect(spellInfo, SPELL_EFFECT_SUMMON_CHANGE_ITEM) ||
                            IsSpellHaveEffect(spellInfo, SPELL_EFFECT_SUMMON_DEAD_PET))
                            return SPELL_AURA_PROC_FAILED;

                        basepoints[0] = spellInfo->Id;
                        caster->CastCustomSpell(caster, 77616, &basepoints[0], NULL, NULL, true);
                    }
                }
                return SPELL_AURA_PROC_OK;
            }
            // Runic Empowerment
            else if (dummySpell->Id == 81229)
            {
                if (GetTypeId() != TYPEID_PLAYER)
                    return SPELL_AURA_PROC_FAILED;

                Player* player = (Player*)this;

                // Item - Death Knight T13 DPS 4P Bonus
                Aura* bonus = GetAura(105646, EFFECT_INDEX_0);

                int32 runicCorruptionBp = 0;
                // Search Runic Corruption
                Unit::AuraList const& dummyAuras = GetAurasByType(SPELL_AURA_DUMMY);
                for (Unit::AuraList::const_iterator itr = dummyAuras.begin(); itr != dummyAuras.end(); ++itr)
                {
                    if ((*itr)->GetSpellProto()->SpellIconID == 4068 && (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_DEATHKNIGHT)
                    {
                        runicCorruptionBp = (*itr)->GetModifier()->m_amount;
                        break;
                    }
                }

                // Runic Corruption
                if (runicCorruptionBp)
                {
                    CastCustomSpell(this, 51460, &runicCorruptionBp, &runicCorruptionBp, NULL, true);
                    // Runic Mastery
                    if (bonus && roll_chance_i(bonus->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_1)))
                        CastSpell(this, 105647, true);
                }
                // Runic Empowerment
                else
                {
                    std::vector<uint8> cdRunes;
                    for (uint8 i = 0; i < MAX_RUNES; i += 2)
                    {
                        uint16 cd1 = player->GetRuneCooldown(i);
                        uint16 cd2 = player->GetRuneCooldown(i + 1);
                        // Runic Empowerment can only activate a rune if both runes of that type are currently on cooldown.
                        if (cd1 && cd2)
                        {
                            // find fully depleted runes
                            // do not activate runes that were used by proc spell
                            if (cd1 == player->GetBaseRuneCooldown(i))
                                cdRunes.push_back(i);
                            else if (cd2 == player->GetBaseRuneCooldown(i + 1))
                                cdRunes.push_back(i + 1);
                        }
                    }
                    if (!cdRunes.empty())
                    {
                        uint8 i = urand(0, cdRunes.size() - 1);
                        uint32 spellId = 0;
                        if (runeSlotTypes[i] == RUNE_BLOOD)
                            spellId = 81166;
                        else if (runeSlotTypes[i] == RUNE_UNHOLY)
                            spellId = 81169;
                        else// if (runeSlotTypes[i] == RUNE_FROST)
                            spellId = 81168;

                        CastSpell(this, spellId, true);
                        // Runic Mastery
                        if (bonus && roll_chance_i(bonus->GetModifier()->m_amount))
                            CastSpell(this, 105647, true);
                    }
                }
                return SPELL_AURA_PROC_OK;
            }
            // Item - Death Knight T12 DPS 4P Bonus
            else if (dummySpell->Id == 98996)
            {
                triggered_spell_id = 99000;
                basepoints[0] = triggerAmount * (damage+absorb) / 100 / GetSpellAuraMaxTicks(triggered_spell_id);
                break;
            }
            // Wandering Plague
            if (dummySpell->SpellIconID == 1614)
            {
                if (!procSpell)
                    return SPELL_AURA_PROC_FAILED;
                // prevent proc from other types than disease
                if (procSpell->GetDispel() != DISPEL_DISEASE)
                    return SPELL_AURA_PROC_FAILED; 
                if (!roll_chance_f(GetUnitCriticalChance(BASE_ATTACK, pVictim)))
                    return SPELL_AURA_PROC_FAILED;
                basepoints[0] = triggerAmount * (damage+absorb) / 100;
                triggered_spell_id = 50526;
                break;
            }
            // Blood-Caked Blade
            if (dummySpell->SpellIconID == 138)
            {
                // only main hand melee auto attack affected and Rune Strike
                if ((procFlag & PROC_FLAG_SUCCESSFUL_OFFHAND_HIT) || (procSpell && procSpell->Id != 56815))
                    return SPELL_AURA_PROC_FAILED;

                // triggered_spell_id in spell data
                break;
            }
            break;
        }
        case SPELLFAMILY_PET:
        {
            switch (dummySpell->SpellIconID)
            {
                // Guard Dog
                case 201:
                {
                    triggered_spell_id = 54445;
                    target = this;
                    if (pVictim)
                        if (SpellEffectEntry const * effect = procSpell->GetSpellEffect(EFFECT_INDEX_0))
                            pVictim->AddThreat(this, effect->EffectBasePoints * triggerAmount / 100.0f);
                    break;
                }
                // Silverback
                case 1582:
                    triggered_spell_id = dummySpell->Id == 62765 ? 62801 : 62800;
                    target = this;
                    break;
            }
            break;
        }
        default:
            break;
    }

    // processed charge only counting case
    if(!triggered_spell_id)
        return SPELL_AURA_PROC_OK;

    SpellEntry const* triggerEntry = sSpellStore.LookupEntry(triggered_spell_id);

    if(!triggerEntry)
    {
        ERROR_LOG("Unit::HandleDummyAuraProc: Spell %u have nonexistent triggered spell %u",dummySpell->Id,triggered_spell_id);
        return SPELL_AURA_PROC_FAILED;
    }

    // default case
    if(!target || target!=this && !target->isAlive())
        return SPELL_AURA_PROC_FAILED;

    if( cooldown && GetTypeId()==TYPEID_PLAYER && ((Player*)this)->HasSpellCooldown(triggered_spell_id))
        return SPELL_AURA_PROC_FAILED;

    if (basepoints[EFFECT_INDEX_0] || basepoints[EFFECT_INDEX_1] || basepoints[EFFECT_INDEX_2])
        CastCustomSpell(target, triggered_spell_id,
            basepoints[EFFECT_INDEX_0] ? &basepoints[EFFECT_INDEX_0] : NULL,
            basepoints[EFFECT_INDEX_1] ? &basepoints[EFFECT_INDEX_1] : NULL,
            basepoints[EFFECT_INDEX_2] ? &basepoints[EFFECT_INDEX_2] : NULL,
            true, castItem, triggeredByAura);
    else
        CastSpell(target, triggered_spell_id, true, castItem, triggeredByAura);

    if (cooldown && GetTypeId()==TYPEID_PLAYER)
        ((Player*)this)->AddSpellCooldown(triggered_spell_id,0,time(NULL) + cooldown);

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleProcTriggerSpellAuraProc(Unit *pVictim, uint32 damage, uint32 absorb, Aura* triggeredByAura, SpellEntry const *procSpell, uint32 procFlags, uint32 procEx, uint32 cooldown)
{
    // Get triggered aura spell info
    SpellEntry const* auraSpellInfo = triggeredByAura->GetSpellProto();
    SpellClassOptionsEntry const* auraClassOptions = auraSpellInfo->GetSpellClassOptions();
    SpellClassOptionsEntry const* procClassOptions = procSpell ? procSpell->GetSpellClassOptions() : NULL;

    // Basepoints of trigger aura
    int32 triggerAmount = triggeredByAura->GetModifier()->m_amount;

    // Set trigger spell id, target, custom basepoints
    SpellEffectEntry const* spellEffect = auraSpellInfo->GetSpellEffect(triggeredByAura->GetEffIndex());
    uint32 trigger_spell_id = spellEffect ? spellEffect->EffectTriggerSpell : 0;
    Unit*  target = NULL;
    int32  basepoints[MAX_EFFECT_INDEX] = {0, 0, 0};

    if(triggeredByAura->GetModifier()->m_auraname == SPELL_AURA_PROC_TRIGGER_SPELL_WITH_VALUE)
        basepoints[0] = triggerAmount;

    Item* castItem = triggeredByAura->GetCastItemGuid() && GetTypeId()==TYPEID_PLAYER
        ? ((Player*)this)->GetItemByGuid(triggeredByAura->GetCastItemGuid()) : NULL;

    // Try handle unknown trigger spells
    // Custom requirements (not listed in procEx) Warning! damage dealing after this
    // Custom triggered spells
    switch (auraSpellInfo->GetSpellFamilyName())
    {
        case SPELLFAMILY_GENERIC:
            switch(auraSpellInfo->Id)
            {
                //case 191:                               // Elemental Response
                //    switch (procSpell->School)
                //    {
                //        case SPELL_SCHOOL_FIRE:  trigger_spell_id = 34192; break;
                //        case SPELL_SCHOOL_FROST: trigger_spell_id = 34193; break;
                //        case SPELL_SCHOOL_ARCANE:trigger_spell_id = 34194; break;
                //        case SPELL_SCHOOL_NATURE:trigger_spell_id = 34195; break;
                //        case SPELL_SCHOOL_SHADOW:trigger_spell_id = 34196; break;
                //        case SPELL_SCHOOL_HOLY:  trigger_spell_id = 34197; break;
                //        case SPELL_SCHOOL_NORMAL:trigger_spell_id = 34198; break;
                //    }
                //    break;
                //case 5301:  break;                        // Defensive State (DND)
                //case 7137:  break:                        // Shadow Charge (Rank 1)
                //case 7377:  break:                        // Take Immune Periodic Damage <Not Working>
                case 12298:                                 // Shield Specialization (Rank 1)
                case 12724:                                 // Shield Specialization (Rank 2)
                case 12725:                                 // Shield Specialization (Rank 3)
                {
                    if (procEx & PROC_EX_REFLECT)
                        if (SpellEntry const * spellInfo = sSpellStore.LookupEntry(trigger_spell_id))
                            basepoints[0] = spellInfo->CalculateSimpleValue(EFFECT_INDEX_0) * 4;
                    break;
                }
                //case 13358: break;                        // Defensive State (DND)
                //case 16092: break;                        // Defensive State (DND)
                //case 18943: break;                        // Double Attack
                //case 19194: break;                        // Double Attack
                //case 19817: break;                        // Double Attack
                //case 19818: break;                        // Double Attack
                //case 22835: break;                        // Drunken Rage
                //    trigger_spell_id = 14822; break;
                case 23780:                                 // Aegis of Preservation (Aegis of Preservation trinket)
                    trigger_spell_id = 23781;
                    break;
                case 25988:                                 // Eye for an Eye, Rank 2
                {
                    // return damage % to attacker but < 50% own total health
                    basepoints[0] = triggerAmount * int32(damage) / 100;
                    if (basepoints[0] > int32(GetMaxHealth() / 2))
                        basepoints[0] = int32(GetMaxHealth() / 2);

                    trigger_spell_id = 25997;
                    break;
                }
                //case 24949: break;                        // Defensive State 2 (DND)
                case 27522:                                 // Mana Drain Trigger
                case 40336:                                 // Mana Drain Trigger
                case 46939:                                 // Black Bow of the Betrayer
                    // On successful melee or ranged attack gain $29471s1 mana and if possible drain $27526s1 mana from the target.
                    if (isAlive())
                        CastSpell(this, 29471, true, castItem, triggeredByAura);
                    if (pVictim && pVictim->isAlive())
                        CastSpell(pVictim, 27526, true, castItem, triggeredByAura);
                    return SPELL_AURA_PROC_OK;
                case 31255:                                 // Deadly Swiftness (Rank 1)
                {
                    // whenever you deal damage to a target who is below 20% health.
                    if (pVictim->GetHealth() > pVictim->GetMaxHealth() / 5)
                        return SPELL_AURA_PROC_FAILED;

                    target = this;
                    trigger_spell_id = 22588;
                    break;
                }
                //case 33207: break;                        // Gossip NPC Periodic - Fidget
                case 33896:                                 // Desperate Defense (Stonescythe Whelp, Stonescythe Alpha, Stonescythe Ambusher)
                    trigger_spell_id = 33898;
                    break;
                //case 34082: break;                        // Advantaged State (DND)
                //case 34783: break:                        // Spell Reflection
                //case 35205: break:                        // Vanish
                //case 35321: break;                        // Gushing Wound
                //case 36096: break:                        // Spell Reflection
                //case 36207: break:                        // Steal Weapon
                //case 36576: break:                        // Shaleskin (Shaleskin Flayer, Shaleskin Ripper) 30023 trigger
                //case 37030: break;                        // Chaotic Temperament
                case 38164:                                 // Unyielding Knights
                    if (pVictim && pVictim->GetEntry() != 19457) // Grillok "Darkeye"
                        return SPELL_AURA_PROC_FAILED;
                    break;
                //case 38363: break;                        // Gushing Wound
                //case 39215: break;                        // Gushing Wound
                //case 40250: break;                        // Improved Duration
                //case 40329: break;                        // Demo Shout Sensor
                //case 40364: break;                        // Entangling Roots Sensor
                //case 41054: break;                        // Copy Weapon
                //    trigger_spell_id = 41055; break;
                //case 41248: break;                        // Consuming Strikes
                //    trigger_spell_id = 41249; break;
                //case 42730: break:                        // Woe Strike
                //case 43453: break:                        // Rune Ward
                //case 43504: break;                        // Alterac Valley OnKill Proc Aura
                //case 44326: break:                        // Pure Energy Passive
                //case 44526: break;                        // Hate Monster (Spar) (30 sec)
                //case 44527: break;                        // Hate Monster (Spar Buddy) (30 sec)
                //case 44819: break;                        // Hate Monster (Spar Buddy) (>30% Health)
                //case 44820: break;                        // Hate Monster (Spar) (<30%)
                case 45057:                                 // Evasive Maneuvers (Commendation of Kael`thas trinket)
                case 75475:                                 // Item - Chamber of Aspects 25 Tank Trinket
                case 75481:                                 // Item - Chamber of Aspects 25 Heroic Tank Trinket
                    // reduce you below $s1% health (in fact in this specific case can proc from any attack while health in result less $s1%)
                    if (int32(GetHealth()) - int32(damage) >= int32(GetMaxHealth() * triggerAmount / 100))
                        return SPELL_AURA_PROC_FAILED;
                    break;
                //case 45903: break:                        // Offensive State
                //case 46146: break:                        // [PH] Ahune  Spanky Hands
                //case 46939: break;                        // Black Bow of the Betrayer
                //    trigger_spell_id = 29471; - gain mana
                //                       27526; - drain mana if possible
                case 43820:                                 // Charm of the Witch Doctor (Amani Charm of the Witch Doctor trinket)
                    // Pct value stored in dummy
                    basepoints[0] = pVictim->GetCreateHealth() * auraSpellInfo->CalculateSimpleValue(EFFECT_INDEX_1) / 100;
                    target = pVictim;
                    break;
                //case 45205: break;                        // Copy Offhand Weapon
                case 45234:                                 // Focused Will
                case 45243:
                {
                    if (damage * 100 >= GetMaxHealth() * auraSpellInfo->CalculateSimpleValue(EFFECT_INDEX_1) ||
                        (procEx & PROC_EX_CRITICAL_HIT) != 0 && (procFlags & PROC_FLAG_ON_TAKE_PERIODIC) == 0)
                        break;

                    return SPELL_AURA_PROC_FAILED;
                }
                //case 45343: break;                        // Dark Flame Aura
                //case 47300: break;                        // Dark Flame Aura
                //case 48876: break;                        // Beast's Mark
                //    trigger_spell_id = 48877; break;
                //case 49059: break;                        // Horde, Hate Monster (Spar Buddy) (>30% Health)
                //case 50051: break;                        // Ethereal Pet Aura
                //case 50689: break;                        // Blood Presence (Rank 1)
                case 50720:                                 // Vigilance
                {
                    // proc Vengeance for caster
                    if (Unit* caster = triggeredByAura->GetCaster())
                        caster->HandleVengeanceProc(pVictim, damage * auraSpellInfo->CalculateSimpleValue(EFFECT_INDEX_1) / 100, 5);
                    break;
                }
                //case 50844: break;                        // Blood Mirror
                //case 52856: break;                        // Charge
                //case 54072: break;                        // Knockback Ball Passive
                //case 54476: break;                        // Blood Presence
                //case 54775: break;                        // Abandon Vehicle on Poly
                case 56702:                                 // Shadow Sickle
                {
                    trigger_spell_id = 56701;
                    break;
                }
                case 57345:                                 // Darkmoon Card: Greatness
                {
                    float stat = 0.0f;
                    // strength
                    if (GetStat(STAT_STRENGTH) > stat) { trigger_spell_id = 60229;stat = GetStat(STAT_STRENGTH); }
                    // agility
                    if (GetStat(STAT_AGILITY)  > stat) { trigger_spell_id = 60233;stat = GetStat(STAT_AGILITY);  }
                    // intellect
                    if (GetStat(STAT_INTELLECT)> stat) { trigger_spell_id = 60234;stat = GetStat(STAT_INTELLECT);}
                    // spirit
                    if (GetStat(STAT_SPIRIT)   > stat) { trigger_spell_id = 60235;                               }
                    break;
                }
                //case 55580: break:                        // Mana Link
                //case 57587: break:                        // Steal Ranged ()
                //case 57594: break;                        // Copy Ranged Weapon
                //case 59237: break;                        // Beast's Mark
                //    trigger_spell_id = 59233; break;
                //case 59288: break;                        // Infra-Green Shield
                //case 59532: break;                        // Abandon Passengers on Poly
                //case 59735: break:                        // Woe Strike
                case 64415:                                 // // Val'anyr Hammer of Ancient Kings - Equip Effect
                {
                    // for DOT procs
                    if (!IsPositiveSpell(procSpell->Id))
                        return SPELL_AURA_PROC_FAILED;
                    break;
                }
                case 64440:                                 // Blade Warding
                {
                    trigger_spell_id = 64442;

                    // need scale damage base at stack size
                    if (SpellEntry const* trigEntry = sSpellStore.LookupEntry(trigger_spell_id))
                        basepoints[EFFECT_INDEX_0] = trigEntry->CalculateSimpleValue(EFFECT_INDEX_0) * triggeredByAura->GetStackAmount();

                    break;
                }
                case 67702:                                 // Death's Choice, Item - Coliseum 25 Normal Melee Trinket
                {
                    trigger_spell_id = GetStat(STAT_STRENGTH) > GetStat(STAT_AGILITY) ? 
                        67708 : 67703;
                    break;
                }
                case 67771:                                 // Death's Choice (heroic), Item - Coliseum 25 Heroic Melee Trinket
                {
                    trigger_spell_id = GetStat(STAT_STRENGTH) > GetStat(STAT_AGILITY) ? 
                        67773 : 67772;
                    break;
                }
                case 64568: // Blood Reserve
                {
                    // When your health drops below 35% ....
                    int32 health35 = int32(GetMaxHealth() * 35 / 100);
                    if (int32(GetHealth()) - int32(damage) >= health35 || int32(GetHealth()) < health35)
                        return SPELL_AURA_PROC_FAILED;

                    basepoints[0] = auraSpellInfo->CalculateSimpleValue(EFFECT_INDEX_0) * triggeredByAura->GetStackAmount();
                    trigger_spell_id = 64569;
                    target = this;
                    break;
                }
                case 69023:                                 // Mirrored Soul
                {
                    int32 basepoints = (int32) (damage * 0.45f);
                    if (Unit* caster = triggeredByAura->GetCaster())
                        // Actually this spell should be sent with SMSG_SPELL_START
                        CastCustomSpell(caster, 69034, &basepoints, NULL, NULL, true, NULL, triggeredByAura, GetObjectGuid());

                    return SPELL_AURA_PROC_OK;
                }
                case 71634: //Corpse Tongue Coin
                case 71640: //Heroic
                {
                    // reduce you below $s1% health
                    if (GetHealth() - damage > GetMaxHealth() * triggerAmount / 100)
                        return SPELL_AURA_PROC_FAILED;
                    break;
                }
                case 72178:                                 // Blood link Saurfang aura
                {
                    target = this;
                    trigger_spell_id = 72195;
                    break;
                }
                case 86303:                                 // Reactive Barrier
                case 86304:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    if (GetHealthPercent() > 50.0f)
                        return SPELL_AURA_PROC_FAILED;

                    trigger_spell_id = 11426;
                    if (HasAura(trigger_spell_id) || ((Player*)this)->HasSpellCooldown(trigger_spell_id))
                        return SPELL_AURA_PROC_FAILED;

                    // mana cost spellmod spell
                    CastSpell(this, 86347, true);
                    break;
                }
                case 91321:                                 // Item - Proc Stacking Spirit
                {
                    // Blind Spot
                    if (HasAura(91322))
                        return SPELL_AURA_PROC_FAILED;
                    break;
                }
                case 92236:                                 // Symbiotic Worms
                case 92356:                                 // Symbiotic Worms
                case 96947:                                 // Loom of Fate
                case 97130:                                 // Loom of Fate
                {
                    if (GetHealthPercent() > triggerAmount * GetMaxHealth() / 100)
                        return SPELL_AURA_PROC_FAILED;
                    break;
                }
                case 92330:                                 // Item - Proc Stacking Spirit
                {
                    // Blind Spot
                    if (HasAura(92331))
                        return SPELL_AURA_PROC_FAILED;
                    break;
                }
            }
            break;
        case SPELLFAMILY_MAGE:
            if (auraSpellInfo->SpellIconID == 2127)         // Blazing Speed
            {
                switch (auraSpellInfo->Id)
                {
                    case 31641:  // Rank 1
                    case 31642:  // Rank 2
                        trigger_spell_id = 31643;
                        break;
                    default:
                        ERROR_LOG("Unit::HandleProcTriggerSpellAuraProc: Spell %u miss possibly Blazing Speed",auraSpellInfo->Id);
                        return SPELL_AURA_PROC_FAILED;
                }
            }
            else if (auraSpellInfo->SpellIconID == 2947)    // Fingers of Frost
            {
                if (!roll_chance_i(triggerAmount))
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            else if (auraSpellInfo->SpellIconID == 4623)    // Invocation
            {
                // done in other way
                return SPELL_AURA_PROC_FAILED;
            }
            else if(auraSpellInfo->Id == 26467)             // Persistent Shield (Scarab Brooch trinket)
            {
                // This spell originally trigger 13567 - Dummy Trigger (vs dummy effect)
                basepoints[0] = damage * 15 / 100;
                target = pVictim;
                trigger_spell_id = 26470;
            }
            else if(auraSpellInfo->Id == 71761)             // Deep Freeze Immunity State
            {
                SpellEntry const * spellProto = sSpellStore.LookupEntry(trigger_spell_id);
                if (!spellProto)
                    return SPELL_AURA_PROC_FAILED;

                DiminishingGroup diminishGroup = GetDiminishingReturnsGroupForSpell(spellProto, triggeredByAura);
                DiminishingLevels diminishLevel = pVictim->GetDiminishing(diminishGroup);

                // spell applied only to permanent immunes to stun targets (bosses) and diminished targets
                if (diminishLevel != DIMINISHING_LEVEL_IMMUNE && (pVictim->GetTypeId() != TYPEID_UNIT ||
                    (((Creature*)pVictim)->GetCreatureInfo()->MechanicImmuneMask & (1 << (MECHANIC_STUN - 1))) == 0))
                    return SPELL_AURA_PROC_FAILED;
            }
            else if (auraSpellInfo->Id == 79684)            // Offensive State (DND)
            {
                // do not proc without Arcane Missiles learned
                if (!HasSpell(5143))
                    return SPELL_AURA_PROC_FAILED;

                // do not proc with Hot Streak talent
                if (HasSpell(44445))
                    return SPELL_AURA_PROC_FAILED;

                // do not proc with Brain Freeze talent
                if (HasSpell(44546) || HasSpell(44548) || HasSpell(44549))
                    return SPELL_AURA_PROC_FAILED;

                // do not proc from Arane Missiles themselves
                if (!procSpell || procSpell->IsFitToFamily(SPELLFAMILY_MAGE, UI64LIT(0x200800)))
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            else if (auraSpellInfo->Id == 105788)           // Item - Mage T13 2P Bonus (Haste Rating)
            {
                if (!procSpell)
                    return SPELL_AURA_PROC_FAILED;

                // only Arcane Blast has 100% proc chance
                if (procSpell->Id == 30451 && !roll_chance_i(triggerAmount))
                    return SPELL_AURA_PROC_FAILED;

                // Item - Mage T13 4P Bonus (Arcane Power, Combustion, and Icy Veins)
                if (HasAura(105790))
                    CastSpell(this, 105791, true);
                break;
            }
            break;
        case SPELLFAMILY_WARRIOR:
            // Deep Wounds (replace triggered spells to directly apply DoT), dot spell have familyflags
            if (auraClassOptions && auraClassOptions->SpellFamilyFlags == UI64LIT(0x0) && auraSpellInfo->SpellIconID == 243)
            {
                bool bOffHand = procFlags & PROC_FLAG_SUCCESSFUL_OFFHAND_HIT;
                if (bOffHand && !haveOffhandWeapon())
                {
                    sLog.outError("Unit::HandleProcTriggerSpellAuraProc: offhand %u proc without offhand weapon!",auraSpellInfo->Id);
                    return SPELL_AURA_PROC_FAILED;
                }

                float weaponSpeed = GetAttackTime(bOffHand ? OFF_ATTACK : BASE_ATTACK)/1000.0f;
                float weaponDPS   = ((GetFloatValue(bOffHand ? UNIT_FIELD_MINOFFHANDDAMAGE : UNIT_FIELD_MINDAMAGE) +
                                    GetFloatValue(bOffHand ? UNIT_FIELD_MAXOFFHANDDAMAGE : UNIT_FIELD_MAXDAMAGE))/2.0f) / weaponSpeed;
                float attackPower = GetTotalAttackPowerValue(bOffHand ? OFF_ATTACK : BASE_ATTACK);
                float f_damage    = 0.0f;

                switch (auraSpellInfo->Id)
                {
                    case 12834: f_damage = ((weaponDPS + attackPower / 14.0f) * weaponSpeed)* 0.16f; break;
                    case 12849: f_damage = ((weaponDPS + attackPower / 14.0f) * weaponSpeed)* 0.32f; break;
                    case 12867: f_damage = ((weaponDPS + attackPower / 14.0f) * weaponSpeed)* 0.48f; break;
                    // Impossible case
                    default:
                        sLog.outError("Unit::HandleProcTriggerSpellAuraProc: DW unknown spell rank %u",auraSpellInfo->Id);
                        return SPELL_AURA_PROC_FAILED;
                }

                trigger_spell_id = 12721;

                SpellEntry const* triggerspellInfo = sSpellStore.LookupEntry(trigger_spell_id);

                if (!triggerspellInfo || !f_damage)
                    return SPELL_AURA_PROC_FAILED;

                SpellEffectEntry const * effect = triggerspellInfo->GetSpellEffect(EFFECT_INDEX_0);
                if (!effect)
                    return SPELL_AURA_PROC_FAILED;

                uint32 tickcount = GetSpellDuration(triggerspellInfo) / effect->EffectAmplitude;

                basepoints[0] = floor(f_damage / tickcount);

                break;
            }
            else if (auraSpellInfo->SpellIconID == 23)      // Improved Hamstring
            {
                // done in other way
                return SPELL_AURA_PROC_FAILED;
            }
            else if (auraSpellInfo->SpellIconID == 1938)    // Rude Interruption
            {
                // done in other way
                return SPELL_AURA_PROC_FAILED;
            }
            else if (auraSpellInfo->SpellIconID == 2053)    // Impending Victory
            {
                if (!pVictim ||!pVictim->HasAuraState(AURA_STATE_HEALTHLESS_20_PERCENT))
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            else if (auraSpellInfo->SpellIconID == 2841)    // Incite
            {
                if (HasAura(trigger_spell_id))
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            else if (auraSpellInfo->SpellIconID == 2961)    // Taste for Blood
            {
                // only at real damage
                if (!damage)
                    return SPELL_AURA_PROC_FAILED;
            }
            else if (auraSpellInfo->SpellIconID == 4660)    // Die by the Sword
            {
                if (!HasAuraState(AURA_STATE_HEALTHLESS_20_PERCENT))
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            else if (auraSpellInfo->SpellIconID == 4978)    // Lambs to the Slaughter
            {
                if (pVictim)
                {
                    // Refresh Rend on target
                    if (SpellAuraHolder* rend = pVictim->GetSpellAuraHolder(94009, GetObjectGuid()))
                        rend->RefreshHolder();
                }
            }
            else if (auraSpellInfo->Id == 50421)            // Scent of Blood
            {
                if (!(procFlags & PROC_FLAG_SUCCESSFUL_MELEE_HIT))
                    return SPELL_AURA_PROC_FAILED;

                CastSpell(this, 50422, true);

                Unit* target = triggeredByAura->GetTarget();
                // Remove only single aura from stack
                if (triggeredByAura->GetHolder()->ModStackAmount(-1))
                    target->RemoveSpellAuraHolder(triggeredByAura->GetHolder());

                return SPELL_AURA_PROC_CANT_TRIGGER;
            }
            else if (auraSpellInfo->Id == 90295)            // Item - Warrior T11 DPS 4P Bonus
            {
                if (!procSpell || procSpell->Id == 85288)   // except Raging Blow main spell
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            else if (auraSpellInfo->Id == 99238)            // Item - Warrior T12 DPS 4P Bonus
            {
                if (!procSpell || procSpell->Id == 85288)   // except Raging Blow main spell
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            else if (auraSpellInfo->Id == 105907)           // Item - Warrior T13 Arms and Fury 4P Bonus (Colossus Smash)
            {
                if (!procSpell)
                    return SPELL_AURA_PROC_FAILED;

                // Bloodthirst has 2 times lower chance to proc
                if (procSpell->Id == 23881)
                    if (!roll_chance_i(50))
                        return SPELL_AURA_PROC_FAILED;
                break;
            }
            break;
        case SPELLFAMILY_WARLOCK:
        {
            // Consume Shadows
            if (auraSpellInfo->IsFitToFamilyMask(UI64LIT(0x0000000002000000)))
            {
                Aura* heal = triggeredByAura->GetHolder()->GetAuraByEffectIndex(EFFECT_INDEX_0);
                if (!heal || heal->GetAuraTicks() > 1)
                    return SPELL_AURA_PROC_FAILED;
            }
            // Soul Leech
            else if (auraSpellInfo->SpellIconID == 2027)
            {
                basepoints[0] = triggerAmount;
                basepoints[1] = triggerAmount;

                // Replenishment proc
                CastSpell(this, 57669, true, NULL, triggeredByAura);
                break;
            }
            // Cheat Death
            else if (auraSpellInfo->Id == 28845)
            {
                // When your health drops below 20% ....
                int32 health20 = int32(GetMaxHealth()) / 5;
                if (int32(GetHealth()) - int32(damage) >= health20 || int32(GetHealth()) < health20)
                    return SPELL_AURA_PROC_FAILED;
            }
            // Decimation
            else if (auraSpellInfo->Id == 63156 || auraSpellInfo->Id == 63158)
            {
                // If target's health is not below equal certain value (25%) not proc
                if (!pVictim || pVictim->GetHealthPercent() > auraSpellInfo->CalculateSimpleValue(EFFECT_INDEX_1))
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            // Aftermath
            else if (auraSpellInfo->SpellIconID == 11)
            {
                if (!procSpell || pVictim == this)
                    return SPELL_AURA_PROC_FAILED;

                int32 chance;
                // Conflagrate - take chance and trigger spell from dbc, Rain of Fire - chance from basepoints
                if (procSpell->Id == 5740)
                {
                    chance = triggerAmount;
                    trigger_spell_id = 85387;
                }
                else
                    chance = auraSpellInfo->GetProcChance();

                if (!roll_chance_i(chance))
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            break;
        }
        case SPELLFAMILY_PRIEST:
        {
            // Greater Heal Refund (Avatar Raiment set)
            if (auraSpellInfo->Id==37594)
            {
                // Not give if target already have full health
                if (pVictim->GetHealth() == pVictim->GetMaxHealth())
                    return SPELL_AURA_PROC_FAILED;
                // If your Greater Heal brings the target to full health, you gain $37595s1 mana.
                if (pVictim->GetHealth() + damage < pVictim->GetMaxHealth())
                    return SPELL_AURA_PROC_FAILED;
                trigger_spell_id = 37595;
            }
            // Strength of Soul
            else if (auraSpellInfo->SpellIconID == 177)
            {
                basepoints[0] = triggerAmount;
                break;
            }
            // Blessed Recovery
            else if (auraSpellInfo->SpellIconID == 1875)
            {
                switch (auraSpellInfo->Id)
                {
                    case 27811: trigger_spell_id = 27813; break;
                    case 27815: trigger_spell_id = 27817; break;
                    case 27816: trigger_spell_id = 27818; break;
                    default:
                        ERROR_LOG("Unit::HandleProcTriggerSpellAuraProc: Spell %u not handled in BR", auraSpellInfo->Id);
                    return SPELL_AURA_PROC_FAILED;
                }
                // is it correct?
                basepoints[0] = (damage+absorb) * triggerAmount / 100 / 3;
                target = this;
            }
            // Blessed Resilience
            else if (auraSpellInfo->SpellIconID == 2177)
            {
                if (damage * 100 >= GetMaxHealth() * auraSpellInfo->CalculateSimpleValue(EFFECT_INDEX_1) ||
                    (procEx & PROC_EX_CRITICAL_HIT) != 0 && (procFlags & PROC_FLAG_ON_TAKE_PERIODIC) == 0)
                    break;

                return SPELL_AURA_PROC_FAILED;
            }
            // Masochism
            else if (auraSpellInfo->SpellIconID == 2211)
            {
                // If damage inflicted is less that pct health and not from SWD
                if (damage * 100  < GetMaxHealth() * auraSpellInfo->CalculateSimpleValue(EFFECT_INDEX_1) &&
                    (!procSpell || procSpell->Id != 32409))
                    return SPELL_AURA_PROC_FAILED;
            }
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            // Primal Madness
            if (auraSpellInfo->SpellIconID == 1181)
            {
                // proc only from Berserk and Enrage
                if (!procSpell || procSpell->SpellIconID != 961 && procSpell->SpellIconID != 2852)
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            // Efflorescense
            else if (auraSpellInfo->SpellIconID == 2886)
            {
                basepoints[1] = int32(triggerAmount * damage / 100.0f);
                break;
            }
            // Blood in the Water
            else if (auraSpellInfo->SpellIconID == 4399)
            {
                if (!pVictim || pVictim->GetHealthPercent() > triggerAmount)
                    return SPELL_AURA_PROC_FAILED;

                // Item - Druid T13 Feral 2P Bonus (Savage Defense and Blood In The Water)
                if (Aura* aura = GetAura(105725, EFFECT_INDEX_0))
                {
                    if (pVictim->GetHealthPercent() > aura->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_1))
                        return SPELL_AURA_PROC_FAILED;
                }
                // common case
                else if (pVictim->GetHealthPercent() > triggerAmount)
                    return SPELL_AURA_PROC_FAILED;

                // Rip and Ferocius Bite have intersecting class masks
                if (!procSpell || procSpell->Id != 22568)
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            // Druid Forms Trinket
            else if (auraSpellInfo->Id==37336)
            {
                switch(GetShapeshiftForm())
                {
                    case FORM_NONE:     trigger_spell_id = 37344; break;
                    case FORM_CAT:      trigger_spell_id = 37341; break;
                    case FORM_BEAR:     trigger_spell_id = 37340; break;
                    case FORM_TREE:     trigger_spell_id = 37342; break;
                    case FORM_MOONKIN:  trigger_spell_id = 37343; break;
                    default:
                        return SPELL_AURA_PROC_FAILED;
                }
            }
            // Druid T9 Feral Relic (Lacerate, Swipe, Mangle, and Shred)
            else if (auraSpellInfo->Id==67353)
            {
                switch(GetShapeshiftForm())
                {
                    case FORM_CAT:      trigger_spell_id = 67355; break;
                    case FORM_BEAR:     trigger_spell_id = 67354; break;
                    default:
                        return SPELL_AURA_PROC_FAILED;
                }
            }
            // Nature's Grasp
            else if (auraSpellInfo->IsFitToFamilyMask(UI64LIT(0x0), 0x1000))
            {
                if (pVictim && pVictim->HasAura(trigger_spell_id))
                    return SPELL_AURA_PROC_FAILED;
            }
            // Hunter T10 2P Bonus (bad spellfamily)
            else if (auraSpellInfo->Id == 70727)
            {
                if (Pet* pet = GetPet())
                {
                    if (pet->isAlive())
                    {
                        pet->CastSpell(pet,trigger_spell_id,true);
                        return SPELL_AURA_PROC_OK;
                    }
                }
                return SPELL_AURA_PROC_FAILED;
            }
            // Item - Druid T11 Restoration 4P Bonus
            else if (auraSpellInfo->Id == 90158)
            {
                // done in other way
                return SPELL_AURA_PROC_FAILED;
            }
            else
            switch (auraSpellInfo->Id)
            {
                // Primal Fury haxx
                case 16958:
                case 16952:
                case 16961:
                case 16954:
                {
                    if (GetTypeId() != TYPEID_PLAYER)
                        return SPELL_AURA_PROC_FAILED;

                    uint32 spell_id = 0;
                    if (auraSpellInfo->Id == 16958 || auraSpellInfo->Id == 16952)
                        spell_id = 37116;
                    else
                        spell_id = 37117;
                    
                    Player* pUnit = (Player*)this;
                    TalentSpellPos const* talentPos = GetTalentSpellPos(spell_id);

                    if (talentPos)
                    {
                        PlayerTalent const* talent = pUnit->GetKnownTalentById(talentPos->talent_id);
                        if (!talent)
                            return SPELL_AURA_PROC_FAILED;
                    }
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_ROGUE:
        {
            // Combat Potency
            if (auraSpellInfo->SpellIconID == 2260)
            {
                // proc from offhand hits and Main Gauche
                if (!(procFlags & PROC_FLAG_SUCCESSFUL_OFFHAND_HIT) && (!procSpell || procSpell->Id != 86392))
                    return SPELL_AURA_PROC_FAILED;
            }
            // Seal Fate
            else if (auraSpellInfo->SpellIconID == 2984)
            {
                if (!procSpell || !IsSpellHaveEffect(procSpell, SPELL_EFFECT_ADD_COMBO_POINTS))
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            // Master Poisoner
            else if (auraSpellInfo->Id == 58410)
            {
                if (!procSpell || procSpell->GetDispel() != DISPEL_POISON)
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            // Item - Rogue T10 4P Bonus
            else if (auraSpellInfo->Id == 70803)
            {
                if (!procSpell)
                    return SPELL_AURA_PROC_FAILED;
                // only allow melee finishing move to proc
                if (!(procSpell->AttributesEx & SPELL_ATTR_EX_REQ_TARGET_COMBO_POINTS) || procSpell->Id == 26679)
                    return SPELL_AURA_PROC_FAILED;
                trigger_spell_id = 70802;
                target = this;
            }
            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            // Master Marksman
            if (auraSpellInfo->SpellIconID == 2230)
            {
                basepoints[0] = triggerAmount;
                break;
            }
            // Piercing Shots
            if (auraSpellInfo->SpellIconID == 3247 && auraSpellInfo->SpellVisual[0] == 0)
            {
                basepoints[0] = (damage+absorb) * triggerAmount / 100 / 8;
                trigger_spell_id = 63468;
                target = pVictim;
                basepoints[0] += pVictim->GetRemainingDotDamage(trigger_spell_id, GetObjectGuid());
            }
            // Rapid Recuperation
            else if (auraSpellInfo->Id == 53228 || auraSpellInfo->Id == 53232)
            {
                // This effect only from Rapid Fire (ability cast)
                if (!(procClassOptions && procClassOptions->SpellFamilyFlags & UI64LIT(0x0000000000000020)))
                    return SPELL_AURA_PROC_FAILED;
            }
            // Glyph of Silencing Shot
            else if (auraSpellInfo->Id == 56836)
            {
                return SPELL_AURA_PROC_FAILED;      // done in other way
            }
            // Entrapment correction
            else if ((auraSpellInfo->Id == 19184 || auraSpellInfo->Id == 19387) &&
                !procSpell->IsFitToFamilyMask(UI64LIT(0x200000000000)) &&   // Snake Trap
                !procSpell->IsFitToFamilyMask(UI64LIT(0x0), 0x40000))       // Frost Trap
                    return SPELL_AURA_PROC_FAILED;
            // Lock and Load
            else if (auraSpellInfo->SpellIconID == 3579)
            {
                // Check for Lock and Load Marker
                if (!procSpell || HasAura(67544))
                    return SPELL_AURA_PROC_FAILED;

                int32 chance = 0;
                if (procFlags & PROC_FLAG_ON_DO_PERIODIC)
                {
                    // only Black Arrow, Immolation Trap and Explosive Traps
                    if (!procSpell->IsFitToFamily(SPELLFAMILY_HUNTER, UI64LIT(0x800000000000000), 0x24000))
                        return SPELL_AURA_PROC_FAILED;

                    // search T.N.T.
                    Unit::AuraList const& mDummyAuras = GetAurasByType(SPELL_AURA_DUMMY);
                    for (Unit::AuraList::const_iterator itr = mDummyAuras.begin(); itr != mDummyAuras.end(); ++itr)
                    {
                        if ((*itr)->GetSpellProto()->SpellIconID == 355 && (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_HUNTER)
                        {
                            chance = (*itr)->GetModifier()->m_amount;
                            break;
                        }
                    }
                }
                else
                {
                    // only Ice and Freezing Trap Effects
                    if (!procSpell->IsFitToFamily(SPELLFAMILY_HUNTER, UI64LIT(0x18)))
                        return SPELL_AURA_PROC_FAILED;

                    chance = triggerAmount;
                }

                if (!roll_chance_i(chance))
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            // Tamed Pet Passive 07 (DND)
            else if (auraSpellInfo->Id == 20784)
            {
                basepoints[0] = triggerAmount;
                break;
            }
            // Item - Hunter T9 4P Bonus
            else if (auraSpellInfo->Id == 67151)
            {
                trigger_spell_id = 68130;
                break;
            }
            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            // Tower of Radiance, Ranks 1-2
            if (auraSpellInfo->SpellIconID == 3402)
            {
                // Must be target of Beacon of Light
                if (!pVictim || !pVictim->GetSpellAuraHolder(53563, GetObjectGuid()))
                    return SPELL_AURA_PROC_FAILED;
            }
            // Protector of the Innocent
            else if (auraSpellInfo->SpellIconID == 5014)
            {
                // Target must not be caster and not from Holy Radiance periodic heal
                if (pVictim == this || !procSpell || procSpell->Id == 86452)
                    return SPELL_AURA_PROC_FAILED;
            }

            // Healing Discount
            if (auraSpellInfo->Id==37705)
            {
                trigger_spell_id = 37706;
                target = this;
            }
            // Soul Preserver
            else if (auraSpellInfo->Id==60510)
            {
                trigger_spell_id = 60515;
                target = this;
            }
            // Lightning Capacitor
            else if (auraSpellInfo->Id==37657)
            {
                if(!pVictim || !pVictim->isAlive())
                    return SPELL_AURA_PROC_FAILED;
                // stacking
                CastSpell(this, 37658, true, NULL, triggeredByAura);

                Aura * dummy = GetDummyAura(37658);
                // release at 3 aura in stack (cont contain in basepoint of trigger aura)
                if(!dummy || dummy->GetStackAmount() < uint32(triggerAmount))
                    return SPELL_AURA_PROC_FAILED;

                RemoveAurasDueToSpell(37658);
                trigger_spell_id = 37661;
                target = pVictim;
            }
            // Bonus Healing (Crystal Spire of Karabor mace)
            else if (auraSpellInfo->Id == 40971)
            {
                // If your target is below $s1% health
                if (pVictim->GetHealth() > pVictim->GetMaxHealth() * triggerAmount / 100)
                    return SPELL_AURA_PROC_FAILED;
            }
            // Thunder Capacitor
            else if (auraSpellInfo->Id == 54841)
            {
                if(!pVictim || !pVictim->isAlive())
                    return SPELL_AURA_PROC_FAILED;
                // stacking
                CastSpell(this, 54842, true, NULL, triggeredByAura);

                // counting
                Aura * dummy = GetDummyAura(54842);
                // release at 3 aura in stack (cont contain in basepoint of trigger aura)
                if(!dummy || dummy->GetStackAmount() < uint32(triggerAmount))
                    return SPELL_AURA_PROC_FAILED;

                RemoveAurasDueToSpell(54842);
                trigger_spell_id = 54843;
                target = pVictim;
            }
            // Item - Icecrown 25 Normal/Heroic Healer Weapon Proc
            else if (auraSpellInfo->Id == 71865 || auraSpellInfo->Id == 71868)
            {
                // don't proc on self
                if (procSpell->Id == 71864 || procSpell->Id == 71866)
                    return SPELL_AURA_PROC_FAILED;

                target = pVictim;
            }
            // Item - Coliseum 25 Normal Caster Trinket
            // Item - Coliseum 25 Heroic Caster Trinket
            else if (auraSpellInfo->Id == 67712 || auraSpellInfo->Id == 67758)
            {
                SpellAuraHolder* holder = GetSpellAuraHolder(trigger_spell_id);
                if (!holder || holder->GetStackAmount() + 1 < 3)
                    break;
                else
                {
                    RemoveSpellAuraHolder(holder);
                    CastSpell(pVictim, auraSpellInfo->Id == 67712 ? 67714 : 67760, true);
                    return SPELL_AURA_PROC_OK;
                }
            }
            // Hand of Light
            else if (auraSpellInfo->Id == 76672)
            {
                if (!procSpell)
                    return SPELL_AURA_PROC_FAILED;

                basepoints[0] = int32(triggerAmount * damage / 100.0f);
                trigger_spell_id = 96172;
                break;
            }
            // Seals of Command
            else if (auraSpellInfo->Id == 85126)
            {
                // Do not proc other spells than seal triggers
                // Seal of Justice, Seal of Righteousness, Censure
                if (!procSpell || procSpell->Id != 20170 && procSpell->Id != 25742 && procSpell->Id != 31803)
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            // Sacred Shield
            else if (auraSpellInfo->Id == 85285)
            {
                if (GetHealthPercent() > 30.0f)
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            // Song of Sorrow
            else if (auraSpellInfo->Id == 90998 || auraSpellInfo->Id == 91003)
            {
                if (!pVictim || pVictim->GetHealthPercent() > triggerAmount * pVictim->GetMaxHealth() / 100)
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            // Item - Proc Stacking Activator (5)
            else if (auraSpellInfo->Id == 91833)
            {
                if (HasAura(91836))
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            // Item - Proc Armor
            else if (auraSpellInfo->Id == 92180 || auraSpellInfo->Id == 92185)
            {
                if (GetHealthPercent() > triggerAmount * GetMaxHealth() / 100)
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            // Item - Proc Dodge Below 35%
            else if (auraSpellInfo->Id == 92234)
            {
                if (GetHealthPercent() > GetMaxHealth() * triggerAmount / 100)
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            // Item - Paladin T13 Protection 2P Bonus (Judgement)
            else if (auraSpellInfo->Id == 105800)
            {
                if (!damage)
                    return SPELL_AURA_PROC_FAILED;

                trigger_spell_id = 105801;
                basepoints[0] = damage * triggerAmount / 100;
                break;
            }
            break;
        }
        case SPELLFAMILY_SHAMAN:
        {
            // Lightning Shield (The Ten Storms set)
            if (auraSpellInfo->Id == 23551)
            {
                trigger_spell_id = 23552;
                target = pVictim;
            }
            // Damage from Lightning Shield (The Ten Storms set)
            else if (auraSpellInfo->Id == 23552)
                trigger_spell_id = 27635;
            // Mana Surge (The Earthfury set)
            else if (auraSpellInfo->Id == 23572)
            {
                if(!procSpell)
                    return SPELL_AURA_PROC_FAILED;
                basepoints[0] = procSpell->GetManaCost() * 35 / 100;
                trigger_spell_id = 23571;
                target = this;
            }
            // Nature's Guardian
            else if (auraSpellInfo->SpellIconID == 2013)
            {
                // Check health condition - should drop to less 30% (trigger at any attack with result health less 30%, independent original health state)
                int32 health30 = int32(GetMaxHealth()) * 3 / 10;
                if (int32(GetHealth()) - int32(damage) >= health30)
                    return SPELL_AURA_PROC_FAILED;

                if(pVictim && pVictim->isAlive())
                    pVictim->getThreatManager().modifyThreatPercent(this,-auraSpellInfo->CalculateSimpleValue(EFFECT_INDEX_1));

                basepoints[0] = triggerAmount * GetMaxHealth() / 100;
                trigger_spell_id = 31616;
                target = this;
            }
            // Rolling Thunder
            else if (auraSpellInfo->Id == 88756 || auraSpellInfo->Id == 88764)
            {
                // Lightning Shield
                if (!HasAura(324))
                    return SPELL_AURA_PROC_FAILED;
            }
            // Ancestral Healing
            else if (auraSpellInfo->SpellIconID == 200)
            {
                if ((procEx & PROC_EX_CRITICAL_HIT) == 0)
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            // Shadow Infusion
            if (auraSpellInfo->SpellIconID == 152)
            {
                Unit* pet = GetPet();
                if (!pet || !pet->IsInWorld() || pet->isDead())
                    return SPELL_AURA_PROC_FAILED;

                // Dark Transformation
                if (pet->HasAura(63560))
                    return SPELL_AURA_PROC_FAILED;

                break;
            }
            // Will of the Necropolis
            else if (auraSpellInfo->SpellIconID == 1762)
            {
                if (GetHealthPercent() > 30.0f)
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            // Acclimation
            else if (auraSpellInfo->SpellIconID == 1930)
            {
                if (!procSpell)
                    return SPELL_AURA_PROC_FAILED;
                switch(GetFirstSchoolInMask(GetSpellSchoolMask(procSpell)))
                {
                    case SPELL_SCHOOL_NORMAL:
                        return SPELL_AURA_PROC_FAILED;                   // ignore
                    case SPELL_SCHOOL_HOLY:   trigger_spell_id = 50490; break;
                    case SPELL_SCHOOL_FIRE:   trigger_spell_id = 50362; break;
                    case SPELL_SCHOOL_NATURE: trigger_spell_id = 50488; break;
                    case SPELL_SCHOOL_FROST:  trigger_spell_id = 50485; break;
                    case SPELL_SCHOOL_SHADOW: trigger_spell_id = 50489; break;
                    case SPELL_SCHOOL_ARCANE: trigger_spell_id = 50486; break;
                    default:
                        return SPELL_AURA_PROC_FAILED;
                }
            }
            // Desecration
            else if (auraSpellInfo->SpellIconID == 2296)
            {
                if (!pVictim || pVictim->IsImmuneToSpellEffect(sSpellStore.LookupEntry(trigger_spell_id), EFFECT_INDEX_0, false))
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            // Crimson Scourge
            else if (auraSpellInfo->SpellIconID ==  2725)
            {
                if (!pVictim || pVictim == this)
                    return SPELL_AURA_PROC_FAILED;

                // check Blood Plague present
                if (!pVictim->HasAura(55078))
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            // Death's Advance
            else if (auraSpellInfo->SpellIconID == 3315)
            {
                if (!procSpell || GetTypeId() != TYPEID_PLAYER)
                    return SPELL_AURA_PROC_FAILED;

                Player* player = (Player*)this;

                if ((player->GetLastUsedRuneMask() & (1 << RUNE_UNHOLY)) == 0)
                    return SPELL_AURA_PROC_FAILED;

                if (player->IsBaseRuneSlotsOnCooldown(RUNE_UNHOLY))
                    break;

                return SPELL_AURA_PROC_FAILED;
            }
            // Might of the Frozen Wastes
            else if (auraSpellInfo->SpellIconID == 4444)
            {
                if (haveOffhandWeapon())
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            // Glyph of Death's Embrace - proc only on "heal" Coil
            else if (auraSpellInfo->Id == 58677)
            {
                if (!procSpell || procSpell->Id != 47633)
                    return SPELL_AURA_PROC_FAILED;
                break;
            }
            // Rune Strike enable proc
            else if (auraSpellInfo->Id == 56816)
            {
                // Only send fake aura to client since triggered spell does not exist
                triggeredByAura->GetHolder()->SendFakeAuraUpdate(trigger_spell_id, false);
                return SPELL_AURA_PROC_OK;
            }
            // Item - Death Knight T10 Melee 4P Bonus
            else if (auraSpellInfo->Id == 70656)
            {
                if (GetTypeId() != TYPEID_PLAYER || getClass() != CLASS_DEATH_KNIGHT)
                    return SPELL_AURA_PROC_FAILED;

                for (uint8 i = 0; i < MAX_RUNES; ++i)
                    if (((Player*)this)->GetRuneCooldown(i) == 0)
                        return SPELL_AURA_PROC_FAILED;
            }
            // Item - Death Knight T13 Blood 2P Bonus
            else if (auraSpellInfo->Id == 105552)
            {
                uint32 hp = GetHealth();
                if (hp <= damage || hp - damage > GetMaxHealth() * triggerAmount / 100)
                    return SPELL_AURA_PROC_FAILED;

                break;
            }
            break;
        }
        default:
            break;
    }

    // All ok. Check current trigger spell
    SpellEntry const* triggerEntry = sSpellStore.LookupEntry(trigger_spell_id);
    if (!triggerEntry)
    {
        // Not cast unknown spell
        // ERROR_LOG("Unit::HandleProcTriggerSpellAuraProc: Spell %u have 0 in EffectTriggered[%d], not handled custom case?",auraSpellInfo->Id,triggeredByAura->GetEffIndex());
        return SPELL_AURA_PROC_FAILED;
    }

    // not allow proc extra attack spell at extra attack
    if (m_extraAttacks && IsSpellHaveEffect(triggerEntry, SPELL_EFFECT_ADD_EXTRA_ATTACKS))
        return SPELL_AURA_PROC_FAILED;

    // Custom basepoints/target for exist spell
    // dummy basepoints or other customs
    switch(trigger_spell_id)
    {
        // Cast positive spell on enemy target
        case 7099:  // Curse of Mending
        case 39647: // Curse of Mending
        case 29494: // Temptation
        case 20233: // Improved Lay on Hands (cast on target)
        {
            target = pVictim;
            break;
        }
        // Combo points add triggers (need add combopoint only for main target, and after possible combopoints reset)
        case 15250: // Rogue Setup
        {
            if(!pVictim || pVictim != getVictim())   // applied only for main target
                return SPELL_AURA_PROC_FAILED;
            break;                                   // continue normal case
        }
        // Finishing moves that add combo points
        case 14189: // Seal Fate (talent, Netherblade set)
        case 14157: // Ruthlessness
        case 70802: // Mayhem (Shadowblade sets)
        {
            // Need add combopoint AFTER finishing move (or they get dropped in finish phase)
            if (Spell* spell = GetCurrentSpell(CURRENT_GENERIC_SPELL))
            {
                spell->AddTriggeredSpell(trigger_spell_id);
                return SPELL_AURA_PROC_OK;
            }
            return SPELL_AURA_PROC_FAILED;
        }
        // Bloodthirst ${$23881m2/1000}.1%
        case 23880:
        {
            basepoints[0] = int32(GetMaxHealth() * triggerAmount / 100 / 1000);
            break;
        }
        // Shamanistic Rage triggered spell
        case 30824:
        {
            basepoints[0] = int32(GetTotalAttackPowerValue(BASE_ATTACK) * triggerAmount / 100);
            break;
        }
        // Enlightenment (trigger only from mana cost spells)
        case 35095:
        {
            SpellPowerEntry const* spellPower = procSpell->GetSpellPower();
            if(!spellPower || !procSpell || procSpell->powerType!=POWER_MANA || spellPower->manaCost==0 && spellPower->ManaCostPercentage==0 && spellPower->manaCostPerlevel==0)
                return SPELL_AURA_PROC_FAILED;
            break;
        }
        // Demonic Pact
        case 48090:
        {
            // As the spell is proced from pet's attack - find owner
            Unit* owner = GetOwner();
            if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
                return SPELL_AURA_PROC_FAILED;

            // This spell doesn't stack, but refreshes duration. So we receive current bonuses to minus them later.
            int32 curBonus = 0;
            if (Aura* aur = owner->GetAura(48090, EFFECT_INDEX_0))
                curBonus = aur->GetModifier()->m_amount;
            int32 spellDamage  = owner->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_MAGIC) - curBonus;
            if(spellDamage <= 0)
                return SPELL_AURA_PROC_FAILED;

            // percent stored in owner talent dummy
            AuraList const& dummyAuras = owner->GetAurasByType(SPELL_AURA_DUMMY);
            for (AuraList::const_iterator i = dummyAuras.begin(); i != dummyAuras.end(); ++i)
            {
                if ((*i)->GetSpellProto()->SpellIconID == 3220)
                {
                    basepoints[0] = basepoints[1] = int32(spellDamage * (*i)->GetModifier()->m_amount / 100);
                    break;
                }
            }
            break;
        }
        // Sword and Board
        case 50227:
        {
            // Remove cooldown on Shield Slam
            if (GetTypeId() == TYPEID_PLAYER)
                ((Player*)this)->RemoveSpellCategoryCooldown(971, true);
            break;
        }
        // Maelstrom Weapon
        case 53817:
        {
            // Shaman T10 Enhancement 4P Bonus
            if (Aura* mw = GetAura(53817, EFFECT_INDEX_0))
                if (Aura* dummy = GetDummyAura(70832))
                    if (mw->GetStackAmount() == mw->GetSpellProto()->GetStackAmount() && roll_chance_i(dummy->GetBasePoints()))
                        CastSpell(this, 70831, true,castItem, triggeredByAura);

            // have rank dependent proc chance, ignore too often cases
            // PPM = 2.5 * (rank of talent),
            uint32 rank = sSpellMgr.GetSpellRank(auraSpellInfo->Id);
            // 5 rank -> 100% 4 rank -> 80% and etc from full rate
            if(!roll_chance_i(20*rank))
                return SPELL_AURA_PROC_FAILED;
            break;
        }
        // Brain Freeze
        case 57761:
        {
            if(!procSpell)
                return SPELL_AURA_PROC_FAILED;
            // For trigger from Blizzard need exist Improved Blizzard
            if (procClassOptions && procClassOptions->SpellFamilyName==SPELLFAMILY_MAGE && (procClassOptions->SpellFamilyFlags & UI64LIT(0x0000000000000080)))
            {
                bool found = false;
                AuraList const& mOverrideClassScript = GetAurasByType(SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
                for(AuraList::const_iterator i = mOverrideClassScript.begin(); i != mOverrideClassScript.end(); ++i)
                {
                    int32 script = (*i)->GetModifier()->m_miscvalue;
                    if(script==836 || script==988 || script==989)
                    {
                        found=true;
                        break;
                    }
                }
                if(!found)
                    return SPELL_AURA_PROC_FAILED;
            }
            break;
        }
        // Astral Shift
        case 52179:
        {
            if (procSpell == 0 || !(procEx & (PROC_EX_NORMAL_HIT|PROC_EX_CRITICAL_HIT)) || this == pVictim)
                return SPELL_AURA_PROC_FAILED;

            // Need stun, fear or silence mechanic
            if (!(GetAllSpellMechanicMask(procSpell) & IMMUNE_TO_SILENCE_AND_STUN_AND_FEAR_MASK))
                return SPELL_AURA_PROC_FAILED;
            break;
        }
        // Burning Determination
        case 54748:
        {
            if(!procSpell)
                return SPELL_AURA_PROC_FAILED;
            // Need Interrupt or Silenced mechanic
            if (!(GetAllSpellMechanicMask(procSpell) & IMMUNE_TO_INTERRUPT_AND_SILENCE_MASK))
                return SPELL_AURA_PROC_FAILED;
            break;
        }
        // Druid - Savage Defense
        case 62606:
        {
            int32 chance = triggeredByAura->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_1);
            // Item - Druid T13 Feral 2P Bonus (Savage Defense and Blood In The Water)
            if (Aura* aura = GetAura(105725, EFFECT_INDEX_0))
                // if procced by Mangle (Bear) with Pulverize active
                if (procSpell && procSpell->Id == 33878 && HasAura(80951))
                    chance = aura->GetModifier()->m_amount;

            if (!roll_chance_i(chance))
                return SPELL_AURA_PROC_FAILED;

            basepoints[0] = int32(GetTotalAttackPowerValue(BASE_ATTACK) * triggerAmount / 100);
            // Savage Defender
            if (Aura* mastery = GetAura(77494, EFFECT_INDEX_0))
                basepoints[0] += int32(basepoints[0] * mastery->GetModifier()->m_amount / 100);
            break;
        }
    }

    if( cooldown && GetTypeId()==TYPEID_PLAYER && ((Player*)this)->HasSpellCooldown(trigger_spell_id))
        return SPELL_AURA_PROC_FAILED;

    // try detect target manually if not set
    if (target == NULL)
        target = !(procFlags & PROC_FLAG_SUCCESSFUL_POSITIVE_SPELL) && IsPositiveSpell(trigger_spell_id) ? this : pVictim;

    // default case
    if (!target || target!=this && !target->isAlive())
        return SPELL_AURA_PROC_FAILED;

    if (basepoints[EFFECT_INDEX_0] || basepoints[EFFECT_INDEX_1] || basepoints[EFFECT_INDEX_2])
        CastCustomSpell(target,trigger_spell_id,
            basepoints[EFFECT_INDEX_0] ? &basepoints[EFFECT_INDEX_0] : NULL,
            basepoints[EFFECT_INDEX_1] ? &basepoints[EFFECT_INDEX_1] : NULL,
            basepoints[EFFECT_INDEX_2] ? &basepoints[EFFECT_INDEX_2] : NULL,
            true, castItem, triggeredByAura);
    else
        CastSpell(target,trigger_spell_id,true,castItem,triggeredByAura);

    if( cooldown && GetTypeId()==TYPEID_PLAYER )
        ((Player*)this)->AddSpellCooldown(trigger_spell_id,0,time(NULL) + cooldown);

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleProcTriggerDamageAuraProc(Unit *pVictim, uint32 /*damage*/, uint32 /*absorb*/, Aura* triggeredByAura, SpellEntry const* /*procSpell*/, uint32 /*procFlags*/, uint32 /*procEx*/, uint32 /*cooldown*/)
{
    SpellEntry const *spellInfo = triggeredByAura->GetSpellProto();
    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "ProcDamageAndSpell: doing %u damage from spell id %u (triggered by auratype %u of spell %u)",
        triggeredByAura->GetModifier()->m_amount, spellInfo->Id, triggeredByAura->GetModifier()->m_auraname, triggeredByAura->GetId());
    SpellNonMeleeDamage damageInfo(this, pVictim, spellInfo->Id, SpellSchoolMask(spellInfo->SchoolMask));
    CalculateSpellDamage(&damageInfo, triggeredByAura->GetModifier()->m_amount, spellInfo);
    damageInfo.target->CalculateAbsorbResistBlock(this, &damageInfo, spellInfo);
    DealDamageMods(damageInfo.target,damageInfo.damage,&damageInfo.absorb);
    SendSpellNonMeleeDamageLog(&damageInfo);
    DealSpellDamage(&damageInfo, true);
    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleOverrideClassScriptAuraProc(Unit *pVictim, uint32 /*damage*/, uint32 /*absorb*/, Aura *triggeredByAura, SpellEntry const *procSpell, uint32 /*procFlag*/, uint32 /*procEx*/ ,uint32 cooldown)
{
    int32 scriptId = triggeredByAura->GetModifier()->m_miscvalue;

    if(!pVictim || !pVictim->isAlive())
        return SPELL_AURA_PROC_FAILED;

    Item* castItem = triggeredByAura->GetCastItemGuid() && GetTypeId()==TYPEID_PLAYER
        ? ((Player*)this)->GetItemByGuid(triggeredByAura->GetCastItemGuid()) : NULL;

    // Basepoints of trigger aura
    int32 triggerAmount = triggeredByAura->GetModifier()->m_amount;

    uint32 triggered_spell_id = 0;

    switch(scriptId)
    {
        case 836:                                           // Improved Blizzard (Rank 1)
        {
            if (!procSpell || procSpell->SpellVisual[0] != 9487)
                return SPELL_AURA_PROC_FAILED;
            triggered_spell_id = 12484;
            break;
        }
        case 988:                                           // Improved Blizzard (Rank 2)
        {
            if (!procSpell || procSpell->SpellVisual[0] != 9487)
                return SPELL_AURA_PROC_FAILED;
            triggered_spell_id = 12485;
            break;
        }
        case 4533:                                          // Dreamwalker Raiment 2 pieces bonus
        {
            // Chance 50%
            if (!roll_chance_i(50))
                return SPELL_AURA_PROC_FAILED;

            switch (pVictim->getPowerType())
            {
                case POWER_MANA:   triggered_spell_id = 28722; break;
                case POWER_RAGE:   triggered_spell_id = 28723; break;
                case POWER_ENERGY: triggered_spell_id = 28724; break;
                default:
                    return SPELL_AURA_PROC_FAILED;
            }
            break;
        }
        case 4537:                                          // Dreamwalker Raiment 6 pieces bonus
            triggered_spell_id = 28750;                     // Blessing of the Claw
            break;
        case 5497:                                          // Improved Mana Gems (Serpent-Coil Braid)
            CastSpell(pVictim, 37445, true);                // Mana Surge (direct because triggeredByAura has no duration)
            return SPELL_AURA_PROC_OK;
    }

    // drop charges
    if(!triggered_spell_id)
        return SPELL_AURA_PROC_OK;

    // standard non-dummy case
    SpellEntry const* triggerEntry = sSpellStore.LookupEntry(triggered_spell_id);

    if(!triggerEntry)
    {
        ERROR_LOG("Unit::HandleOverrideClassScriptAuraProc: Spell %u triggering for class script id %u",triggered_spell_id,scriptId);
        return SPELL_AURA_PROC_FAILED;
    }

    if( cooldown && GetTypeId()==TYPEID_PLAYER && ((Player*)this)->HasSpellCooldown(triggered_spell_id))
        return SPELL_AURA_PROC_FAILED;

    DEBUG_LOG("Unit::HandleOverrideClassScriptAuraProc: casting spell %u from aura %u script %u",
        triggered_spell_id, triggeredByAura->GetId(), scriptId);

    CastSpell(pVictim, triggered_spell_id, true, castItem, triggeredByAura);

    if( cooldown && GetTypeId()==TYPEID_PLAYER )
        ((Player*)this)->AddSpellCooldown(triggered_spell_id,0,time(NULL) + cooldown);

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleMendingAuraProc( Unit* /*pVictim*/, uint32 /*damage*/, uint32 /*absorb*/, Aura* triggeredByAura, SpellEntry const* /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/ )
{
    // aura can be deleted at casts
    SpellEntry const* spellProto = triggeredByAura->GetSpellProto();
    SpellEffectIndex effIdx = triggeredByAura->GetEffIndex();
    int32 heal = triggeredByAura->GetModifier()->m_amount;
    ObjectGuid caster_guid = triggeredByAura->GetCasterGuid();

    // jumps
    int32 jumps = triggeredByAura->GetHolder()->GetAuraCharges()-1;

    // current aura expire
    triggeredByAura->GetHolder()->SetAuraCharges(1);             // will removed at next charges decrease

    // next target selection
    if (jumps > 0 && GetTypeId()==TYPEID_PLAYER && caster_guid.IsPlayer())
    {
        SpellEffectEntry const* spellEffect = spellProto->GetSpellEffect(effIdx);
        float radius;
        if (spellEffect && spellEffect->GetRadiusIndex())
            radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(spellEffect->GetRadiusIndex()));
        else
            radius = GetSpellMaxRange(sSpellRangeStore.LookupEntry(spellProto->rangeIndex));

        if(Player* caster = ((Player*)triggeredByAura->GetCaster()))
        {
            caster->ApplySpellMod(spellProto->Id, SPELLMOD_RADIUS, radius, NULL);

            if(Player* target = ((Player*)this)->GetNextRandomRaidMember(radius))
            {
                SpellAuraHolder *holder = GetSpellAuraHolder(spellProto->Id, caster->GetObjectGuid());
                SpellAuraHolder *new_holder = CreateSpellAuraHolder(spellProto, target, caster);

                for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
                {
                    Aura *aur = holder->GetAuraByEffectIndex(SpellEffectIndex(i));
                    if (!aur)
                        continue;

                    int32 basePoints = aur->GetBasePoints();
                    Aura * new_aur = CreateAura(spellProto, aur->GetEffIndex(), &basePoints, new_holder, target, caster);
                    new_holder->AddAura(new_aur, new_aur->GetEffIndex());
                }
                new_holder->SetAuraCharges(jumps, false);

                // lock aura holder (currently SPELL_AURA_PRAYER_OF_MENDING is single target spell, so will attempt removing from old target
                // when applied to new one)
                triggeredByAura->SetInUse(true);
                target->AddSpellAuraHolder(new_holder);
                triggeredByAura->SetInUse(false);
            }

            heal += int32(caster->SpellBaseHealingBonusDone(GetSpellSchoolMask(spellProto)) * 0.318f);

            // Glyph of Prayer of Mending
            if (jumps + 1 == spellProto->GetProcCharges())
                if (Aura* glyph = caster->GetAura(55685, EFFECT_INDEX_0))
                    heal += int32(heal * glyph->GetModifier()->m_amount / 100.0f);

        }
    }

    // heal
    CastCustomSpell(this,33110,&heal,NULL,NULL,true,NULL,NULL,caster_guid, spellProto);
    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleModCastingSpeedNotStackAuraProc(Unit* /*pVictim*/, uint32 /*damage*/, uint32 /*absorb*/, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/)
{
    SpellEntry const* spellProto = triggeredByAura->GetSpellProto();

    // Speed of Light
    if (spellProto->GetSpellFamilyName() == SPELLFAMILY_PALADIN && spellProto->SpellIconID == 5062)
    {
        int32 bp = triggeredByAura->GetModifier()->m_amount;
        CastCustomSpell(this, 85497, &bp, NULL, NULL, true);
    }

    // Skip melee hits or instant cast spells
    return !(procSpell == NULL || GetSpellCastTime(procSpell) == 0) ? SPELL_AURA_PROC_OK : SPELL_AURA_PROC_FAILED;
}

SpellAuraProcResult Unit::HandleReflectSpellsSchoolAuraProc(Unit* /*pVictim*/, uint32 /*damage*/, uint32 /*absorb*/, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/)
{
    // Skip Melee hits and spells ws wrong school
    return !(procSpell == NULL || (triggeredByAura->GetModifier()->m_miscvalue & procSpell->SchoolMask) == 0) ? SPELL_AURA_PROC_OK : SPELL_AURA_PROC_FAILED;
}

SpellAuraProcResult Unit::HandleModPowerCostSchoolAuraProc(Unit* /*pVictim*/, uint32 /*damage*/, uint32 /*absorb*/, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/)
{
    // Skip melee hits and spells ws wrong school or zero cost
    return !(procSpell == NULL ||
            (procSpell->GetManaCost() == 0 && procSpell->GetManaCostPercentage() == 0) || // Cost check
            (triggeredByAura->GetModifier()->m_miscvalue & procSpell->SchoolMask) == 0) ? SPELL_AURA_PROC_OK : SPELL_AURA_PROC_FAILED;  // School check
}

SpellAuraProcResult Unit::HandleMechanicImmuneResistanceAuraProc(Unit* /*pVictim*/, uint32 /*damage*/, uint32 /*absorb*/, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/)
{
    // Compare mechanic
   return !(procSpell==NULL || int32(procSpell->GetMechanic()) != triggeredByAura->GetModifier()->m_miscvalue)
       ? SPELL_AURA_PROC_OK : SPELL_AURA_PROC_FAILED;
}

SpellAuraProcResult Unit::HandleIgnoreUnitStateAuraProc(Unit* pVictim, uint32 damage, uint32 absorb, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown)
{
    SpellEntry const* spellProto = triggeredByAura->GetSpellProto();

    switch (spellProto->GetSpellFamilyName())
    {
        case SPELLFAMILY_WARRIOR:
        {
            // Juggernaut
            if (spellProto->Id == 64976)
            {
                if (!procSpell)
                    return SPELL_AURA_PROC_FAILED;

                // add cooldowns
                // procced from Charge
                if (procSpell->Id == 100)
                {
                    CastSpell(this, 96216, false);  // Intercept cooldown
                    // cast proc
                    CastSpell(this, 65156, true);
                }
                else
                    CastSpell(this, 96215, false);  // Charge cooldown
            }
            break;
        }
        case SPELLFAMILY_MAGE:
        {
            // Fingers of Frost
            if (spellProto->Id == 44544)
            {
                Unit* target = triggeredByAura->GetTarget();
                // Remove only single aura from stack
                if (triggeredByAura->GetHolder()->ModStackAmount(-1))
                    target->RemoveSpellAuraHolder(triggeredByAura->GetHolder());
            }
            break;
        }
    }

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleModDamageFromCasterAuraProc(Unit* pVictim, uint32 /*damage*/, uint32 /*absorb*/, Aura* triggeredByAura, SpellEntry const* /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/)
{
    // Compare casters
    return triggeredByAura->GetCasterGuid() == pVictim->GetObjectGuid() ? SPELL_AURA_PROC_OK : SPELL_AURA_PROC_FAILED;
}

SpellAuraProcResult Unit::HandleAddFlatModifierAuraProc(Unit* pVictim, uint32 /*damage*/, uint32 /*absorb*/, Aura* triggeredByAura, SpellEntry const * procSpell, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/)
{
    SpellEntry const *spellInfo = triggeredByAura->GetSpellProto();

    switch (spellInfo->GetSpellFamilyName())
    {
        case SPELLFAMILY_MAGE:
        {
            switch (spellInfo->Id)
            {
                case 57529:                         // Arcane Potency
                case 57531:
                {
                    // exclude Arcane Blast debuff
                    if (!procSpell || procSpell->Id == 36032)
                        return SPELL_AURA_PROC_FAILED;
                    break;
                }
                case 83049:                         // Early Frost
                case 83050:
                {
                    if (triggeredByAura->GetEffIndex() != EFFECT_INDEX_0)
                        return SPELL_AURA_PROC_FAILED;

                    uint32 triggered_spell_id = 0;
                    // Rank 1
                    if (spellInfo->Id == 83049)
                        triggered_spell_id = 83162;
                    // Rank 2
                    else if (spellInfo->Id == 83050)
                        triggered_spell_id = 83239;

                    if (!HasAura(triggered_spell_id))
                        CastSpell(this, triggered_spell_id, true);
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_WARRIOR:
        {
            switch (spellInfo->Id)
            {
                case 80976:     // Blitz Rank 1
                case 80977:     // Blitz Rank 2
                {
                    if (!pVictim || triggeredByAura->GetEffIndex() != EFFECT_INDEX_0)
                        return SPELL_AURA_PROC_FAILED;

                    CastSpell(pVictim, 96273, true);
                    return SPELL_AURA_PROC_OK;
                }
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_WARLOCK:
        {
            // Pandemic
            if (spellInfo->SpellIconID == 4554)
            {
                if (!roll_chance_i(spellInfo->CalculateSimpleValue(EFFECT_INDEX_0)) ||
                    pVictim->GetHealthPercent() > 25.0f)
                    return SPELL_AURA_PROC_FAILED;

                CastSpell(pVictim, 92931, true);
                return SPELL_AURA_PROC_OK;
            }
            break;
        }
        case SPELLFAMILY_PRIEST:
        {
            // Chakra
            if (spellInfo->Id == 14751)
                // Chakra: Serenity
                CastSpell(this, 81208, true);
            break;
        }
        case SPELLFAMILY_ROGUE:
        {
            // Deadly Momentum
            if (spellInfo->Id == 84590)
            {
                if (GetTypeId() != TYPEID_PLAYER || ((Player*)this)->HasSpellCooldown(spellInfo->Id))
                    return SPELL_AURA_PROC_FAILED;
                return SPELL_AURA_PROC_OK;
            }
            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            if (spellInfo->Id == 53695 || spellInfo->Id == 53696)  // Judgements of the Just
            {
                if (!pVictim || triggeredByAura->GetEffIndex() != EFFECT_INDEX_0)
                    return SPELL_AURA_PROC_FAILED;

                if (!procSpell || procSpell->Id != 54158)
                    return SPELL_AURA_PROC_FAILED;

                int32 bp0 = triggeredByAura->GetModifier()->m_amount;
                CastCustomSpell(pVictim, 68055, &bp0, NULL, NULL, true, NULL, triggeredByAura);
                return SPELL_AURA_PROC_OK;
            }
            break;
        }
        case SPELLFAMILY_SHAMAN:
        {
            if (spellInfo->Id == 55166)                                 // Tidal Force
            {
                Unit* target = triggeredByAura->GetTarget();
                // Remove only single aura from stack
                if (triggeredByAura->GetHolder()->ModStackAmount(-1))
                    target->RemoveSpellAuraHolder(triggeredByAura->GetHolder());

                return SPELL_AURA_PROC_CANT_TRIGGER;
            }
            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            if (spellInfo->Id == 53257)                                 // Cobra Strikes
            {
                Unit* target = triggeredByAura->GetTarget();
                // Remove only single aura from stack
                if (triggeredByAura->GetHolder()->ModStackAmount(-1))
                    target->RemoveSpellAuraHolder(triggeredByAura->GetHolder());
                else
                {
                    if (Unit* owner = target->GetOwner())
                    {
                        if (SpellAuraHolder* holder = owner->GetSpellAuraHolder(53257))
                            if (holder->ModStackAmount(-1))
                                owner->RemoveSpellAuraHolder(holder);
                    }
                }

                return SPELL_AURA_PROC_CANT_TRIGGER;
            }
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            // Unholy Command
            if (spellInfo->SpellIconID == 2723)
            {
                if (GetTypeId() == TYPEID_PLAYER)
                    ((Player*)this)->RemoveSpellCooldown(49576, true);
            }
            break;
        }
    }

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleAddPctModifierAuraProc(Unit* pVictim, uint32 /*damage*/, uint32 /*absorb*/, Aura* triggeredByAura, SpellEntry const *procSpell, uint32 /*procFlag*/, uint32 procEx, uint32 /*cooldown*/)
{
    SpellEntry const *spellInfo = triggeredByAura->GetSpellProto();
    Item* castItem = triggeredByAura->GetCastItemGuid() && GetTypeId()==TYPEID_PLAYER
        ? ((Player*)this)->GetItemByGuid(triggeredByAura->GetCastItemGuid()) : NULL;

    switch(spellInfo->GetSpellFamilyName())
    {
        case SPELLFAMILY_MAGE:
            break;
        case SPELLFAMILY_WARLOCK:
        {
            // Soulburn
            if (spellInfo->Id == 74434)
            {
                if (!procSpell)
                    return SPELL_AURA_PROC_FAILED;

                switch (procSpell->Id)
                {
                    case 5676:      // Searing Pain
                        CastSpell(this, 79440, true);
                        break;
                    case 6262:      // Healthstone
                        CastSpell(this, 79437, true);
                        break;
                    case 27243:     // Seed of Corruption
                    {
                        soulburnMarker = false;
                        // Soulburn: Seed of Corruption, rank 1
                        if (!HasSpell(86664))
                            return SPELL_AURA_PROC_FAILED;

                        soulburnMarker = true;
                        break;
                    }
                    case 48020:     // Demonic Circle: Teleport
                        CastSpell(this, 79438, true);
                        break;
                    case 6353:      // Soul Fire
                        // Item - Warlock T13 4P Bonus (Soulburn)
                        if (HasAura(105787))
                            CastSpell(this, 87388, true);   // Soul Shard
                        break;
                    case 688:       // Summon Imp
                    case 691:       // Summon Felhunter
                    case 697:       // Summon Voidwalker
                    case 11519:     // Summon Succubus
                    case 30146:     // Summon Felguard
                    case 89420:     // Drain Life
                        break;
                    default:
                        return SPELL_AURA_PROC_FAILED;
                }
            }
            break;
        }
        case SPELLFAMILY_PRIEST:
        {
            // Chakra
            if (spellInfo->Id == 14751)
                if (triggeredByAura->GetEffIndex() == EFFECT_INDEX_1)
                    // Chakra: Sanctuary
                    CastSpell(this, 81206, true);
                else
                    // Chakra: Chastice
                    CastSpell(this, 81209, true);
            break;
        }
        case SPELLFAMILY_PALADIN:
            break;
        case SPELLFAMILY_WARRIOR:
        {
            if (spellInfo->Id == 46916)           // Slam!
            {
                if (!(procSpell && procSpell->Id==50782))
                    return SPELL_AURA_PROC_CANT_TRIGGER;
            }
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            // Harmony
            if (spellInfo->Id == 77495)
            {
                int32 bp = triggeredByAura->GetModifier()->m_amount;
                // Harmony
                CastCustomSpell(this, 100977, &bp, &bp, NULL, true);
                return SPELL_AURA_PROC_OK;
            }
            // Item - Druid T11 Feral 4P Bonus
            else if (spellInfo->Id == 90165)
            {
                // Strength of the Panther
                CastSpell(this, 90166, true);
                return SPELL_AURA_PROC_OK;
            }
            break;
        }
        case SPELLFAMILY_HUNTER:
        {
            // Lock and load triggered
            if (spellInfo->Id == 56453)
            {
                // Proc only on first effect
                if (triggeredByAura->GetEffIndex() != EFFECT_INDEX_0)
                    return SPELL_AURA_PROC_CANT_TRIGGER;

                // Remove only single aura from stack
                if (triggeredByAura->GetStackAmount() > 1 && !triggeredByAura->GetHolder()->ModStackAmount(-1))
                    return SPELL_AURA_PROC_CANT_TRIGGER;
            }
            break;
        }
        case SPELLFAMILY_SHAMAN:
        {
            // Maelstrom Weapon proc remove
            if (spellInfo->Id == 53817)
            {
                if (!procSpell || !procSpell->IsFitToFamilyMask(UI64LIT(0x8000000001C3), 0x10000))
                    return SPELL_AURA_PROC_FAILED;
            }
            break;
        }
        default:
            break;
    }
    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleModDamagePercentDoneAuraProc(Unit* /*pVictim*/, uint32 /*damage*/, uint32 /*absorb*/, Aura* triggeredByAura, SpellEntry const *procSpell, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 cooldown)
{
    SpellEntry const *spellInfo = triggeredByAura->GetSpellProto();
    Item* castItem = triggeredByAura->GetCastItemGuid() && GetTypeId()==TYPEID_PLAYER
        ? ((Player*)this)->GetItemByGuid(triggeredByAura->GetCastItemGuid()) : NULL;

    // Aspect of the Viper
    SpellClassOptionsEntry const* classOptions = spellInfo->GetSpellClassOptions();
    if (classOptions && classOptions->SpellFamilyName == SPELLFAMILY_HUNTER && classOptions->SpellFamilyFlags & UI64LIT(0x4000000000000))
    {
        uint32 maxmana = GetMaxPower(POWER_MANA);
        int32 bp = int32(maxmana* GetAttackTime(RANGED_ATTACK)/1000.0f/100.0f);

        if(cooldown && GetTypeId()==TYPEID_PLAYER && ((Player*)this)->HasSpellCooldown(34075))
            return SPELL_AURA_PROC_FAILED;

        CastCustomSpell(this, 34075, &bp, NULL, NULL, true, castItem, triggeredByAura);
    }
    // Bone Shield
    else if (spellInfo->Id == 49222)
    {
        if (cooldown)
        {
            time_t now = time(NULL);
            if (now >= m_boneShieldCooldown)
            {
                m_boneShieldCooldown = time_t(now + cooldown);
                return SPELL_AURA_PROC_OK;
            }

            return SPELL_AURA_PROC_FAILED;
        }
    }

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleModRating(Unit* /*pVictim*/, uint32 /*damage*/, uint32 /*absorb*/, Aura* triggeredByAura, SpellEntry const * /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/)
{
    SpellEntry const *spellInfo = triggeredByAura->GetSpellProto();

    if (spellInfo->Id == 71564)                             // Deadly Precision
    {
        // Remove only single aura from stack
        if (triggeredByAura->GetStackAmount() > 1 && !triggeredByAura->GetHolder()->ModStackAmount(-1))
            return SPELL_AURA_PROC_CANT_TRIGGER;
    }

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleSpellMagnetAuraProc(Unit* /*pVictim*/, uint32 damage, uint32 absorb, Aura* triggeredByAura, SpellEntry const* procSpell, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/)
{
    if (triggeredByAura->GetId() == 8178)                   // Grounding Totem Effect
    {
        // Frozen Power checks
        if (Unit* owner = GetOwner())
        {
            if (procSpell && owner->GetTypeId() == TYPEID_PLAYER && owner->getClass() == CLASS_SHAMAN)
            {
                if (uint32 spellSchoolMask = GetSpellSchoolMask(procSpell))
                {
                    Player* plrOwner = (Player*)owner;
                    if (SpellEntry const * spellProto = plrOwner->GetKnownTalentRankById(11220))
                    {
                        uint32 triggeredSpell = 0;
                        if (spellSchoolMask & SPELL_SCHOOL_MASK_FIRE)
                            triggeredSpell = 97618;
                        else if (spellSchoolMask & SPELL_SCHOOL_MASK_FROST)
                            triggeredSpell = 97619;
                        else if (spellSchoolMask & SPELL_SCHOOL_MASK_NATURE)
                            triggeredSpell = 97620;
                        else if (spellSchoolMask & SPELL_SCHOOL_MASK_ARCANE)
                            triggeredSpell = 97621;
                        else if (spellSchoolMask & SPELL_SCHOOL_MASK_SHADOW)
                            triggeredSpell = 97622;
                        if (triggeredSpell)
                        {
                            int32 bp = GetResistancesAtLevel(plrOwner->getLevel()) * (spellProto->Id == 16086 ? 0.5f : 1.0f);
                            plrOwner->CastCustomSpell(plrOwner, triggeredSpell, &bp, NULL, NULL, true);
                        }
                    }
                }
            }
        }

        // for spells that doesn't do damage but need to destroy totem anyway
        if ((!damage || damage < GetHealth()) && GetTypeId() == TYPEID_UNIT && ((Creature*)this)->IsTotem())
        {
            DealDamage(this, GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            return SPELL_AURA_PROC_OK;
        }
    }
    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleManaShieldAuraProc(Unit *pVictim, uint32 /*damage*/, uint32 /*absorb*/, Aura* triggeredByAura, SpellEntry const* /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 cooldown)
{
    SpellEntry const *dummySpell = triggeredByAura->GetSpellProto ();
    SpellClassOptionsEntry const* dummyClassOptions = dummySpell->GetSpellClassOptions();

    Item* castItem = triggeredByAura->GetCastItemGuid() && GetTypeId()==TYPEID_PLAYER
        ? ((Player*)this)->GetItemByGuid(triggeredByAura->GetCastItemGuid()) : NULL;

    uint32 triggered_spell_id = 0;
    Unit* target = pVictim;

    switch(dummyClassOptions->SpellFamilyName)
    {
        case SPELLFAMILY_MAGE:
        {
            // Incanter's Regalia set (add trigger chance to Mana Shield)
            if (dummyClassOptions->IsFitToFamilyMask(UI64LIT(0x0000000000008000)))
            {
                if (GetTypeId() != TYPEID_PLAYER)
                    return SPELL_AURA_PROC_FAILED;

                target = this;
                triggered_spell_id = 37436;
                break;
            }
            break;
        }
        default:
            break;
    }

    // processed charge only counting case
    if (!triggered_spell_id)
        return SPELL_AURA_PROC_OK;

    SpellEntry const* triggerEntry = sSpellStore.LookupEntry(triggered_spell_id);

    if (!triggerEntry)
    {
        sLog.outError("Unit::HandleManaShieldAuraProc: Spell %u have nonexistent triggered spell %u",dummySpell->Id,triggered_spell_id);
        return SPELL_AURA_PROC_FAILED;
    }

    // default case
    if (!target || (target != this && !target->isAlive()))
        return SPELL_AURA_PROC_FAILED;

    if (cooldown && GetTypeId()==TYPEID_PLAYER && ((Player*)this)->HasSpellCooldown(triggered_spell_id))
        return SPELL_AURA_PROC_FAILED;

    CastSpell(target, triggered_spell_id, true, castItem, triggeredByAura);

    if (cooldown && GetTypeId()==TYPEID_PLAYER)
        ((Player*)this)->AddSpellCooldown(triggered_spell_id,0,time(NULL) + cooldown);

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleModResistanceAuraProc(Unit* /*pVictim*/, uint32 damage, uint32 absorb, Aura* triggeredByAura, SpellEntry const* /*procSpell*/, uint32 /*procFlag*/, uint32 /*procEx*/, uint32 /*cooldown*/)
{
    SpellEntry const *spellInfo = triggeredByAura->GetSpellProto();

    // Inner Fire
    if (spellInfo->IsFitToFamily(SPELLFAMILY_PRIEST, UI64LIT(0x0000000000002)))
    {
        // only at real damage
        if (!damage)
            return SPELL_AURA_PROC_FAILED;
    }

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleRemoveByDamageProc(Unit* pVictim, uint32 damage, uint32 absorb, Aura* triggeredByAura, SpellEntry const *procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown)
{
    // do not proc from positives
    if (procFlag & (PROC_FLAG_TAKEN_POSITIVE_AOE | PROC_FLAG_TAKEN_POSITIVE_SPELL) || procEx & PROC_EX_PERIODIC_POSITIVE)
        return SPELL_AURA_PROC_OK;

    uint32 fullDamage = damage + absorb;
    if (!fullDamage)
        return SPELL_AURA_PROC_FAILED;

    if (!triggeredByAura->GetSpellProto()->GetProcFlags())
        return SPELL_AURA_PROC_FAILED;

    // root type spells do not dispel the root effect
    if (procSpell && triggeredByAura->GetModifier()->m_auraname == SPELL_AURA_MOD_ROOT && (GetAllSpellMechanicMask(procSpell) & (1 << (MECHANIC_ROOT-1))))
        return SPELL_AURA_PROC_FAILED;

    // Polymorph
    if (GetSpellSpecific(triggeredByAura->GetId()) == SPELL_MAGE_POLYMORPH)
    {
        // Improved Polymorph Marker
        if (!HasAura(87515))
        {
            if (Unit* caster = triggeredByAura->GetCaster())
            {
                if (caster->GetTypeId() == TYPEID_PLAYER)
                {
                    // find Improved Polymorph
                    if (SpellEntry const * ip = ((Player*)caster)->GetKnownTalentRankById(9142))
                    {
                        // Improved Polymorph (Rank 1)
                        if (ip->Id == 11210)
                            CastSpell(this, 83046, true, NULL, triggeredByAura, caster->GetObjectGuid());
                        // Improved Polymorph (Rank 2)
                        else if (ip->Id == 12592)
                            CastSpell(this, 83047, true, NULL, triggeredByAura, caster->GetObjectGuid());

                        // Improved Polymorph Marker
                        CastSpell(this, 87515, true);
                    }
                }
            }
        }
    }
    // Hungering Cold
    else if (triggeredByAura->GetId() == 49203)
    {
        // Damage from diseases does not break the freeze effect
        if (procSpell && (GetAllSpellMechanicMask(procSpell) & (1 << (MECHANIC_INFECTED-1))))
            return SPELL_AURA_PROC_FAILED;
    }
    // Gouge
    else if (triggeredByAura->GetId() == 1776)
    {
        if (procSpell && procSpell->Id == triggeredByAura->GetId())
            return SPELL_AURA_PROC_FAILED;
        else if (pVictim && pVictim->GetTypeId() == TYPEID_PLAYER && procSpell && (GetAllSpellMechanicMask(procSpell) & (1 << (MECHANIC_BLEED - 1))))
        {
            // search Sanguinary Vein
            if (SpellEntry const * spellInfo = ((Player*)pVictim)->GetKnownTalentRankById(6520))
                if (roll_chance_i(spellInfo->CalculateSimpleValue(EFFECT_INDEX_1)))
                    return SPELL_AURA_PROC_FAILED;
        }
    }
    // Repentance vs. Censure
    else if (triggeredByAura->GetId() == 20066 && procSpell && procSpell->Id == 31803)
        return SPELL_AURA_PROC_FAILED;

    if (triggeredByAura->GetModifier()->m_amount < (int32)fullDamage)
    {
        triggeredByAura->SetInUse(true);
        RemoveAurasByCasterSpell(triggeredByAura->GetId(), triggeredByAura->GetCasterGuid());
        triggeredByAura->SetInUse(false);
        return SPELL_AURA_PROC_OK;
    }

    triggeredByAura->GetModifier()->m_amount -= (int32)fullDamage;

    return SPELL_AURA_PROC_FAILED;
}

SpellAuraProcResult Unit::HandlePeriodicDummyAuraProc(Unit* pVictim, uint32 damage, uint32 absorb, Aura* triggeredByAura, SpellEntry const *procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown)
{
    SpellEntry const* dummySpell = triggeredByAura->GetSpellProto();

    switch (dummySpell->GetSpellFamilyName())
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (dummySpell->Id)
            {
                case 66334:                                 // Mistress' Kiss (Trial of the Crusader, ->
                case 67905:                                 // -> Lord Jaraxxus encounter, all difficulties)
                case 67906:                                 // ----- // -----
                case 67907:                                 // ----- // -----
                {
                    CastSpell(this, 66359, true, NULL, triggeredByAura);
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            // Reaping
            // Blood Rites
            if (dummySpell->SpellIconID == 22 || dummySpell->SpellIconID == 2724)
            {
                if (GetTypeId() == TYPEID_PLAYER)
                {
                    Player* player = (Player*)this;

                    if (player->getClass() != CLASS_DEATH_KNIGHT)
                        return SPELL_AURA_PROC_FAILED;

                    uint32 runeMask = player->GetLastUsedRuneMask();
                    // cant proc only from death runes
                    if ((runeMask & ~(1 << RUNE_DEATH)) == 0)
                        return SPELL_AURA_PROC_FAILED;

                    // Reset amplitude - set death rune remove timer to 30s
                    triggeredByAura->ResetPeriodic(true);

                    uint32 runesLeft;

                    // Blood Strike or Pestilence
                    if (procSpell->Id == 45902 || procSpell->Id == 50842)
                        runesLeft = 1;
                    else
                        runesLeft = 2;

                    for (uint8 i = 0; i < MAX_RUNES && runesLeft; ++i)
                    {
                        RuneType rune = player->GetCurrentRune(i);
                        if (rune == RUNE_DEATH)
                            continue;

                        if (player->GetRuneCooldown(i) != player->GetBaseRuneCooldown(i))
                            continue;

                        if ((runeMask & (1 << rune)) == 0)
                            continue;

                        --runesLeft;
                        runeMask &= ~(1 << rune);

                        // Mark aura as used
                        player->AddRuneByAuraEffect(i, RUNE_DEATH, triggeredByAura);
                    }
                    return SPELL_AURA_PROC_OK;
                }
                return SPELL_AURA_PROC_FAILED;
            }
            break;
        }
        default:
            break;
    }
    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleIncreaseSpeedAuraProc(Unit* pVictim, uint32 damage, uint32 absorb, Aura* triggeredByAura, SpellEntry const *procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown)
{
    SpellEntry const* spellProto = triggeredByAura->GetSpellProto();

    switch (spellProto->Id)
    {
        case 26022:                     // Pursuit of Justice
        case 26023:
        {
            if (!procSpell || GetTypeId() != TYPEID_PLAYER)
                return SPELL_AURA_PROC_FAILED;

            if (!IsSpellAppliesAura(procSpell))
                return SPELL_AURA_PROC_FAILED;

            uint32 triggered_spell_id = 89024;
            uint32 cooldown_spell_id = 89023;

            // Pursuit of Justice shares cooldown with Blessed Life proc
            if (cooldown && ((Player*)this)->HasSpellCooldown(cooldown_spell_id))
                return SPELL_AURA_PROC_FAILED;

            if (GetAllSpellMechanicMask(procSpell) & MECHANIC_IMMOBILIZE)
            {
                // energize
                CastSpell(this, triggered_spell_id, true);
                if (cooldown)
                    ((Player*)this)->AddSpellCooldown(cooldown_spell_id, 0, time(NULL) + cooldown);
            }

            return SPELL_AURA_PROC_OK;
        }
    }

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleHasteAllProc(Unit* pVictim, uint32 damage, uint32 absorb, Aura* triggeredByAura, SpellEntry const *procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown)
{
    SpellEntry const* spellProto = triggeredByAura->GetSpellProto();

    // Dark Intent (target)
    if (spellProto->Id == 85767)
    {
        if (Unit* caster = triggeredByAura->GetCaster())
            caster->CastSpell(caster, 94310, true);
    }
    // Dark Intent (caster)
    else if (spellProto->Id == 85768)
    {
        if (Unit* caster = triggeredByAura->GetTarget())
        {
            if (Unit* singleTarget = caster->GetSingleCastSpellTarget(85767))
            {
                uint32 triggered_spell = 0;

                switch (singleTarget->getClass())
                {
                    case CLASS_WARRIOR: triggered_spell = 94313; break;
                    case CLASS_PALADIN: triggered_spell = 94323; break;
                    case CLASS_HUNTER: triggered_spell = 94320; break;
                    case CLASS_ROGUE: triggered_spell = 94324; break;
                    case CLASS_PRIEST: triggered_spell = 94311; break;
                    case CLASS_DEATH_KNIGHT: triggered_spell = 94312; break;
                    case CLASS_SHAMAN: triggered_spell = 94319; break;
                    case CLASS_MAGE: triggered_spell = 85759; break;
                    case CLASS_WARLOCK: triggered_spell = 94310; break;
                    case CLASS_DRUID: triggered_spell = 94318; break;
                    default:
                        return SPELL_AURA_PROC_FAILED;
                }

                int32 bp = 1;
                singleTarget->CastCustomSpell(singleTarget, triggered_spell, &bp, &bp, NULL, true);
            }
        }
    }

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleVengeanceProc(Unit* pVictim, int32 damage, int32 triggerAmount)
{
    // do not proc from damage
    if (!pVictim || pVictim->GetCharmerOrOwnerPlayerOrPlayerItself())
        return SPELL_AURA_PROC_FAILED;

    if (int32 basebp = damage)
    {
        int32 bp = 0;
        uint32 triggered_spell_id = 76691;
        // stack with old buff
        if (SpellAuraHolder* oldHolder = GetSpellAuraHolder(triggered_spell_id, GetObjectGuid()))
        {
            basebp = int32(basebp * triggerAmount / 100);
            if (Aura* oldAura = oldHolder->GetAuraByEffectIndex(EFFECT_INDEX_0))
                bp += oldAura->GetModifier()->m_amount;
        }
        else
            basebp = int32(basebp * 33 / 100);

        bp += basebp;

        // not more than pct of stamina
        int32 maxVal = int32((GetCreateHealth() + GetTotalStatValue(STAT_STAMINA)) / 10);
        if (bp > maxVal)
            bp = maxVal;

        CastCustomSpell(this, triggered_spell_id, &bp, &bp, &basebp, true);
    }

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleSpellAuraOverrideActionbarSpellsProc(Unit* pVictim, uint32 damage, uint32 absorb, Aura* triggeredByAura, SpellEntry const *procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown)
{
    SpellEntry const* spellProto = triggeredByAura->GetSpellProto();
    int32 triggerAmount = triggeredByAura->GetModifier()->m_amount;

    // Dark Simulacrum
    if (spellProto->Id == 77616)
    {
        if (!procSpell || procSpell->Id != triggerAmount)
            return SPELL_AURA_PROC_FAILED;
    }

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleAuraProcOnPowerAmount(Unit* pVictim, uint32 damage, uint32 absorb, Aura* triggeredByAura, SpellEntry const *procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown)
{
    SpellEntry const* spellProto = triggeredByAura->GetSpellProto();
    int32 triggerAmount = triggeredByAura->GetModifier()->m_amount;

    Powers powerType = Powers(triggeredByAura->GetModifier()->m_miscvalue);
    if (GetPowerIndex(powerType) == INVALID_POWER_INDEX)
        return SPELL_AURA_PROC_FAILED;

    int32 powerAmount = GetPower(powerType);

    switch (spellProto->Id)
    {
        case 79577:         // Eclipse Mastery Driver Passive
        {
            if (!procSpell)
                return SPELL_AURA_PROC_FAILED;

            // forbid proc when not in balance spec
            if (!HasSpell(78674))
                return SPELL_AURA_PROC_FAILED;

            bool hasMarker = false;
            int32 direction = 1;
            // lunar Eclipse Marker
            if (HasAura(67484))
            {
                hasMarker = true;
                direction = -1;
            }
            // solar Eclipse Marker
            else if (HasAura(67483))
                hasMarker = true;

            int32 powerMod = 0;
            // Starfire
            if (procSpell->Id == 2912)
                powerMod = procSpell->CalculateSimpleValue(EFFECT_INDEX_1);
            // Wrath
            else if (procSpell->Id == 5176)
                powerMod = -procSpell->CalculateSimpleValue(EFFECT_INDEX_1);
            // Moonfire or Sunfire
            else if (procSpell->Id == 8921 || procSpell->Id == 93402)
            {
                // search Lunar Shower buff
                Unit::AuraList const& pctModAuras = GetAurasByType(SPELL_AURA_ADD_PCT_MODIFIER);
                for (Unit::AuraList::const_iterator itr = pctModAuras.begin(); itr != pctModAuras.end(); ++itr)
                {
                    if ((*itr)->GetSpellProto()->SpellIconID == 3698 && (*itr)->GetEffIndex() == EFFECT_INDEX_0 &&
                        (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_DRUID)
                    {
                        if (SpellEntry const * spell = sSpellStore.LookupEntry(33603))
                            powerMod = spell->CalculateSimpleValue(EFFECT_INDEX_2) * (procSpell->Id == 8921 ? 1 : -1);
                        break;
                    }
                }
            }
            // Starsurge
            else if (procSpell->Id == 78674)
                powerMod = direction * procSpell->CalculateSimpleValue(EFFECT_INDEX_1);

            // proc failed if wrong spell or spell direction does not match marker direction
            if (!powerMod || hasMarker && direction * powerMod < 0)
                return SPELL_AURA_PROC_FAILED;

            if (powerMod > 0 && triggeredByAura->GetEffIndex() != EFFECT_INDEX_0)
                return SPELL_AURA_PROC_FAILED;
            else if (powerMod < 0 && triggeredByAura->GetEffIndex() != EFFECT_INDEX_1)
                return SPELL_AURA_PROC_FAILED;

            // while not in Eclipse State
            if (!HasAura(48517) && !HasAura(48518))
            {
                // only Starfire and Wrath
                if (procSpell->Id == 2912 || procSpell->Id == 5176)
                {
                    // search Euphoria
                    Unit::AuraList const& dummyAuras = GetAurasByType(SPELL_AURA_DUMMY);
                    for (Unit::AuraList::const_iterator itr = dummyAuras.begin(); itr != dummyAuras.end(); ++itr)
                    {
                        if ((*itr)->GetSpellProto()->SpellIconID == 4431 && (*itr)->GetEffIndex() == EFFECT_INDEX_0 &&
                            (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_DRUID)
                        {
                            // energize eclipse
                            if (roll_chance_i((*itr)->GetModifier()->m_amount))
                                CastCustomSpell(this, 81069, &powerMod, NULL, NULL, true);
                        }
                    }

                    // Item - Druid T12 Balance 4P Bonus
                    if (HasAura(99049))
                        powerMod += direction * (procSpell->Id == 2912 ? 5 : 3);
                }
            }

            ModifyPower(powerType, powerMod);
            int32 newPower = GetPower(powerType);

            if (newPower == powerAmount)
                return SPELL_AURA_PROC_FAILED;

            // Eclipse is cleared when eclipse power reaches 0
            if (powerAmount * newPower <= 0)
            {
                RemoveAurasDueToSpell(48517);
                RemoveAurasDueToSpell(48518);
            }

            uint32 markerSpellAdd, markerSpellRemove;
            if (newPower == triggerAmount)
            {
                // Remove Nature's Grace Cooldown
                if (GetTypeId() == TYPEID_PLAYER)
                    ((Player*)this)->RemoveSpellCooldown(16880, true);

                // search Euphoria
                Unit::AuraList const& dummyAuras = GetAurasByType(SPELL_AURA_DUMMY);
                for (Unit::AuraList::const_iterator itr = dummyAuras.begin(); itr != dummyAuras.end(); ++itr)
                {
                    if ((*itr)->GetSpellProto()->SpellIconID == 4431 && (*itr)->GetEffIndex() == EFFECT_INDEX_2 &&
                        (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_DRUID)
                    {
                        int32 basepoints = (*itr)->GetModifier()->m_amount;
                        // energize mana
                        CastCustomSpell(this, 81070, &basepoints, NULL, NULL, true);
                        break;
                    }
                }

                // cast Eclipse
                CastSpell(this, triggeredByAura->GetSpellEffect()->EffectTriggerSpell, true);
                // Item - Druid T11 Balance 4P Bonus
                if (Aura* aura = GetAura(90163, EFFECT_INDEX_0))
                {
                    // Astral Alignment
                    if (SpellEntry const* bonus = sSpellStore.LookupEntry(90164))
                    {
                        int32 bp = bonus->CalculateSimpleValue(EFFECT_INDEX_0) * 3;
                        CastCustomSpell(this, bonus, &bp, NULL, NULL, true, NULL, aura);
                    }
                }

                // solar marker or lunar marker
                markerSpellAdd = triggeredByAura->GetEffIndex() == EFFECT_INDEX_0 ? 67484 : 67483;
                markerSpellRemove = triggeredByAura->GetEffIndex() == EFFECT_INDEX_0 ? 67483 : 67484;

                RemoveAurasDueToSpell(markerSpellRemove);
                if (!HasAura(markerSpellAdd))
                    CastSpell(this, markerSpellAdd, true);
            }
            // Marker casted only when not by Starsurge
            else if (!hasMarker && procSpell->Id != 78674)
            {
                uint32 markerSpellAdd, markerSpellRemove;
                // solar marker or lunar marker
                markerSpellAdd = powerMod > 0 ? 67483 : 67484;
                markerSpellRemove = powerMod < 0 ? 67483 : 67484;

                RemoveAurasDueToSpell(markerSpellRemove);
                if (!HasAura(markerSpellAdd))
                    CastSpell(this, markerSpellAdd, true);
            }
            break;
        }
        default:
            break;
    }

    return SPELL_AURA_PROC_OK;
}

SpellAuraProcResult Unit::HandleModRangedHasteAuraProc(Unit* pVictim, uint32 damage, uint32 absorb, Aura* triggeredByAura, SpellEntry const *procSpell, uint32 procFlag, uint32 procEx, uint32 cooldown)
{
    // Improved Steady Shot
    if (triggeredByAura->GetId() == 53220)
        return SPELL_AURA_PROC_FAILED;

    return SPELL_AURA_PROC_OK;
}

