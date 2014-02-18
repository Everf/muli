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
#include "Database/DatabaseEnv.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Opcodes.h"
#include "Log.h"
#include "UpdateMask.h"
#include "World.h"
#include "ObjectMgr.h"
#include "SpellMgr.h"
#include "Player.h"
#include "Unit.h"
#include "Spell.h"
#include "DynamicObject.h"
#include "Group.h"
#include "UpdateData.h"
#include "ObjectAccessor.h"
#include "Policies/Singleton.h"
#include "Totem.h"
#include "Creature.h"
#include "Formulas.h"
#include "BattleGround/BattleGround.h"
#include "OutdoorPvP/OutdoorPvP.h"
#include "CreatureAI.h"
#include "ScriptMgr.h"
#include "Util.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Vehicle.h"
#include "CellImpl.h"
#include "InstanceData.h"
#include "Language.h"
#include "MapManager.h"
#include "SpellAuras.h"
#include "Weather.h"
#include "PhaseMgr.h"

#define NULL_AURA_SLOT 0xFF

pAuraHandler AuraHandler[TOTAL_AURAS]=
{
    &Aura::HandleNULL,                                      //  0 SPELL_AURA_NONE
    &Aura::HandleBindSight,                                 //  1 SPELL_AURA_BIND_SIGHT
    &Aura::HandleModPossess,                                //  2 SPELL_AURA_MOD_POSSESS
    &Aura::HandlePeriodicDamage,                            //  3 SPELL_AURA_PERIODIC_DAMAGE
    &Aura::HandleAuraDummy,                                 //  4 SPELL_AURA_DUMMY
    &Aura::HandleModConfuse,                                //  5 SPELL_AURA_MOD_CONFUSE
    &Aura::HandleModCharm,                                  //  6 SPELL_AURA_MOD_CHARM
    &Aura::HandleModFear,                                   //  7 SPELL_AURA_MOD_FEAR
    &Aura::HandlePeriodicHeal,                              //  8 SPELL_AURA_PERIODIC_HEAL
    &Aura::HandleModAttackSpeed,                            //  9 SPELL_AURA_MOD_ATTACKSPEED
    &Aura::HandleModThreat,                                 // 10 SPELL_AURA_MOD_THREAT
    &Aura::HandleModTaunt,                                  // 11 SPELL_AURA_MOD_TAUNT
    &Aura::HandleAuraModStun,                               // 12 SPELL_AURA_MOD_STUN
    &Aura::HandleModDamageDone,                             // 13 SPELL_AURA_MOD_DAMAGE_DONE
    &Aura::HandleModDamageTaken,                            // 14 SPELL_AURA_MOD_DAMAGE_TAKEN   implemented in Unit::MeleeDamageBonusTaken and Unit::SpellBaseDamageBonusTaken
    &Aura::HandleNoImmediateEffect,                         // 15 SPELL_AURA_DAMAGE_SHIELD      implemented in Unit::DealMeleeDamage
    &Aura::HandleModStealth,                                // 16 SPELL_AURA_MOD_STEALTH
    &Aura::HandleNoImmediateEffect,                         // 17 SPELL_AURA_MOD_STEALTH_DETECT implemented in Unit::isVisibleForOrDetect
    &Aura::HandleInvisibility,                              // 18 SPELL_AURA_MOD_INVISIBILITY
    &Aura::HandleInvisibilityDetect,                        // 19 SPELL_AURA_MOD_INVISIBILITY_DETECTION
    &Aura::HandleAuraModTotalHealthPercentRegen,            // 20 SPELL_AURA_OBS_MOD_HEALTH
    &Aura::HandleAuraModTotalEnergyPercentRegen,            // 21 SPELL_AURA_OBS_MOD_ENERGY
    &Aura::HandleAuraModResistance,                         // 22 SPELL_AURA_MOD_RESISTANCE
    &Aura::HandlePeriodicTriggerSpell,                      // 23 SPELL_AURA_PERIODIC_TRIGGER_SPELL
    &Aura::HandlePeriodicEnergize,                          // 24 SPELL_AURA_PERIODIC_ENERGIZE
    &Aura::HandleAuraModPacify,                             // 25 SPELL_AURA_MOD_PACIFY
    &Aura::HandleAuraModRoot,                               // 26 SPELL_AURA_MOD_ROOT
    &Aura::HandleAuraModSilence,                            // 27 SPELL_AURA_MOD_SILENCE
    &Aura::HandleAuraModReflectSpells,                      // 28 SPELL_AURA_REFLECT_SPELLS        implement in Unit::SpellHitResult
    &Aura::HandleAuraModStat,                               // 29 SPELL_AURA_MOD_STAT
    &Aura::HandleAuraModSkill,                              // 30 SPELL_AURA_MOD_SKILL
    &Aura::HandleAuraModIncreaseSpeed,                      // 31 SPELL_AURA_MOD_INCREASE_SPEED
    &Aura::HandleAuraModIncreaseMountedSpeed,               // 32 SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED
    &Aura::HandleAuraModDecreaseSpeed,                      // 33 SPELL_AURA_MOD_DECREASE_SPEED
    &Aura::HandleAuraModIncreaseHealth,                     // 34 SPELL_AURA_MOD_INCREASE_HEALTH
    &Aura::HandleAuraModIncreaseEnergy,                     // 35 SPELL_AURA_MOD_INCREASE_ENERGY
    &Aura::HandleAuraModShapeshift,                         // 36 SPELL_AURA_MOD_SHAPESHIFT
    &Aura::HandleAuraModEffectImmunity,                     // 37 SPELL_AURA_EFFECT_IMMUNITY
    &Aura::HandleAuraModStateImmunity,                      // 38 SPELL_AURA_STATE_IMMUNITY
    &Aura::HandleAuraModSchoolImmunity,                     // 39 SPELL_AURA_SCHOOL_IMMUNITY
    &Aura::HandleAuraModDmgImmunity,                        // 40 SPELL_AURA_DAMAGE_IMMUNITY
    &Aura::HandleAuraModDispelImmunity,                     // 41 SPELL_AURA_DISPEL_IMMUNITY
    &Aura::HandleAuraProcTriggerSpell,                      // 42 SPELL_AURA_PROC_TRIGGER_SPELL  implemented in Unit::ProcDamageAndSpellFor and Unit::HandleProcTriggerSpell
    &Aura::HandleNoImmediateEffect,                         // 43 SPELL_AURA_PROC_TRIGGER_DAMAGE implemented in Unit::ProcDamageAndSpellFor
    &Aura::HandleAuraTrackCreatures,                        // 44 SPELL_AURA_TRACK_CREATURES
    &Aura::HandleAuraTrackResources,                        // 45 SPELL_AURA_TRACK_RESOURCES
    &Aura::HandleUnused,                                    // 46 SPELL_AURA_46 4 spells in 4.3.4
    &Aura::HandleAuraModParryPercent,                       // 47 SPELL_AURA_MOD_PARRY_PERCENT
    &Aura::HandleNoImmediateEffect,                         // 48 SPELL_AURA_PERIODIC_TRIGGER_BY_CLIENT (Client periodic trigger spell by self (3 spells in 3.3.5a)). implemented in pet/player cast chains.
    &Aura::HandleAuraModDodgePercent,                       // 49 SPELL_AURA_MOD_DODGE_PERCENT
    &Aura::HandleNoImmediateEffect,                         // 50 SPELL_AURA_MOD_CRITICAL_HEALING_AMOUNT implemented in Unit::SpellCriticalHealingBonus
    &Aura::HandleAuraModBlockChancePercent,                 // 51 SPELL_AURA_MOD_BLOCK_CHANCE_PERCENT
    &Aura::HandleAuraModCritPercent,                        // 52 SPELL_AURA_MOD_CRIT_PERCENT
    &Aura::HandlePeriodicLeech,                             // 53 SPELL_AURA_PERIODIC_LEECH
    &Aura::HandleModHitChance,                              // 54 SPELL_AURA_MOD_HIT_CHANCE
    &Aura::HandleModSpellHitChance,                         // 55 SPELL_AURA_MOD_SPELL_HIT_CHANCE
    &Aura::HandleAuraTransform,                             // 56 SPELL_AURA_TRANSFORM
    &Aura::HandleModSpellCritChance,                        // 57 SPELL_AURA_MOD_SPELL_CRIT_CHANCE
    &Aura::HandleAuraModIncreaseSwimSpeed,                  // 58 SPELL_AURA_MOD_INCREASE_SWIM_SPEED
    &Aura::HandleNoImmediateEffect,                         // 59 SPELL_AURA_MOD_DAMAGE_DONE_CREATURE implemented in Unit::MeleeDamageBonusDone and Unit::SpellDamageBonusDone
    &Aura::HandleAuraModPacifyAndSilence,                   // 60 SPELL_AURA_MOD_PACIFY_SILENCE
    &Aura::HandleAuraModScale,                              // 61 SPELL_AURA_MOD_SCALE
    &Aura::HandlePeriodicHealthFunnel,                      // 62 SPELL_AURA_PERIODIC_HEALTH_FUNNEL
    &Aura::HandleUnused,                                    // 63 unused (3.0.8a-4.3.4) old SPELL_AURA_PERIODIC_MANA_FUNNEL
    &Aura::HandlePeriodicManaLeech,                         // 64 SPELL_AURA_PERIODIC_MANA_LEECH
    &Aura::HandleModCastingSpeed,                           // 65 SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK
    &Aura::HandleFeignDeath,                                // 66 SPELL_AURA_FEIGN_DEATH
    &Aura::HandleAuraModDisarm,                             // 67 SPELL_AURA_MOD_DISARM
    &Aura::HandleAuraModStalked,                            // 68 SPELL_AURA_MOD_STALKED
    &Aura::HandleSchoolAbsorb,                              // 69 SPELL_AURA_SCHOOL_ABSORB implemented in Unit::CalculateAbsorbAndResist
    &Aura::HandleUnused,                                    // 70 SPELL_AURA_EXTRA_ATTACKS      Useless, used by only one spell 41560 that has only visual effect (3.2.2a)
    &Aura::HandleModSpellCritChanceShool,                   // 71 SPELL_AURA_MOD_SPELL_CRIT_CHANCE_SCHOOL
    &Aura::HandleModPowerCostPCT,                           // 72 SPELL_AURA_MOD_POWER_COST_SCHOOL_PCT
    &Aura::HandleModPowerCost,                              // 73 SPELL_AURA_MOD_POWER_COST_SCHOOL
    &Aura::HandleNoImmediateEffect,                         // 74 SPELL_AURA_REFLECT_SPELLS_SCHOOL  implemented in Unit::SpellHitResult
    &Aura::HandleNoImmediateEffect,                         // 75 SPELL_AURA_MOD_LANGUAGE           implemented in WorldSession::HandleMessagechatOpcode
    &Aura::HandleFarSight,                                  // 76 SPELL_AURA_FAR_SIGHT
    &Aura::HandleModMechanicImmunity,                       // 77 SPELL_AURA_MECHANIC_IMMUNITY
    &Aura::HandleAuraMounted,                               // 78 SPELL_AURA_MOUNTED
    &Aura::HandleModDamagePercentDone,                      // 79 SPELL_AURA_MOD_DAMAGE_PERCENT_DONE
    &Aura::HandleModPercentStat,                            // 80 SPELL_AURA_MOD_PERCENT_STAT
    &Aura::HandleNoImmediateEffect,                         // 81 SPELL_AURA_SPLIT_DAMAGE_PCT       implemented in Unit::CalculateAbsorbAndResist
    &Aura::HandleWaterBreathing,                            // 82 SPELL_AURA_WATER_BREATHING
    &Aura::HandleModBaseResistance,                         // 83 SPELL_AURA_MOD_BASE_RESISTANCE
    &Aura::HandleModRegen,                                  // 84 SPELL_AURA_MOD_REGEN
    &Aura::HandleModPowerRegen,                             // 85 SPELL_AURA_MOD_POWER_REGEN
    &Aura::HandleChannelDeathItem,                          // 86 SPELL_AURA_CHANNEL_DEATH_ITEM
    &Aura::HandleDamagePercentTaken,                        // 87 SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN implemented in Unit::MeleeDamageBonusTaken and Unit::SpellDamageBonusTaken
    &Aura::HandleNoImmediateEffect,                         // 88 SPELL_AURA_MOD_HEALTH_REGEN_PERCENT implemented in Player::RegenerateHealth
    &Aura::HandlePeriodicDamagePCT,                         // 89 SPELL_AURA_PERIODIC_DAMAGE_PERCENT
    &Aura::HandleUnused,                                    // 90 unused (3.0.8a-4.3.4) old SPELL_AURA_MOD_RESIST_CHANCE
    &Aura::HandleNoImmediateEffect,                         // 91 SPELL_AURA_MOD_DETECT_RANGE implemented in Creature::GetAttackDistance
    &Aura::HandlePreventFleeing,                            // 92 SPELL_AURA_PREVENTS_FLEEING
    &Aura::HandleModUnattackable,                           // 93 SPELL_AURA_MOD_UNATTACKABLE
    &Aura::HandleNoImmediateEffect,                         // 94 SPELL_AURA_INTERRUPT_REGEN implemented in Player::RegenerateAll
    &Aura::HandleAuraGhost,                                 // 95 SPELL_AURA_GHOST
    &Aura::HandleNoImmediateEffect,                         // 96 SPELL_AURA_SPELL_MAGNET implemented in Unit::SelectMagnetTarget
    &Aura::HandleManaShield,                                // 97 SPELL_AURA_MANA_SHIELD implemented in Unit::CalculateAbsorbAndResist
    &Aura::HandleAuraModSkill,                              // 98 SPELL_AURA_MOD_SKILL_TALENT
    &Aura::HandleAuraModAttackPower,                        // 99 SPELL_AURA_MOD_ATTACK_POWER
    &Aura::HandleUnused,                                    //100 SPELL_AURA_AURAS_VISIBLE obsolete 3.x? all player can see all auras now, but still have 2 spells including GM-spell (1852,2855)
    &Aura::HandleModResistancePercent,                      //101 SPELL_AURA_MOD_RESISTANCE_PCT
    &Aura::HandleNoImmediateEffect,                         //102 SPELL_AURA_MOD_MELEE_ATTACK_POWER_VERSUS implemented in Unit::MeleeDamageBonusDone
    &Aura::HandleAuraModTotalThreat,                        //103 SPELL_AURA_MOD_TOTAL_THREAT
    &Aura::HandleAuraWaterWalk,                             //104 SPELL_AURA_WATER_WALK
    &Aura::HandleAuraFeatherFall,                           //105 SPELL_AURA_FEATHER_FALL
    &Aura::HandleAuraHover,                                 //106 SPELL_AURA_HOVER
    &Aura::HandleAddModifier,                               //107 SPELL_AURA_ADD_FLAT_MODIFIER
    &Aura::HandleAddModifier,                               //108 SPELL_AURA_ADD_PCT_MODIFIER
    &Aura::HandleNoImmediateEffect,                         //109 SPELL_AURA_ADD_TARGET_TRIGGER
    &Aura::HandleModPowerRegenPCT,                          //110 SPELL_AURA_MOD_POWER_REGEN_PERCENT
    &Aura::HandleNoImmediateEffect,                         //111 SPELL_AURA_ADD_CASTER_HIT_TRIGGER implemented in Unit::SelectMagnetTarget
    &Aura::HandleNoImmediateEffect,                         //112 SPELL_AURA_OVERRIDE_CLASS_SCRIPTS implemented in diff functions.
    &Aura::HandleNoImmediateEffect,                         //113 SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN implemented in Unit::MeleeDamageBonusTaken
    &Aura::HandleNoImmediateEffect,                         //114 SPELL_AURA_MOD_RANGED_DAMAGE_TAKEN_PCT implemented in Unit::MeleeDamageBonusTaken
    &Aura::HandleNoImmediateEffect,                         //115 SPELL_AURA_MOD_HEALING                 implemented in Unit::SpellBaseHealingBonusTaken
    &Aura::HandleNoImmediateEffect,                         //116 SPELL_AURA_MOD_REGEN_DURING_COMBAT     imppemented in Player::RegenerateAll and Player::RegenerateHealth
    &Aura::HandleNoImmediateEffect,                         //117 SPELL_AURA_MOD_MECHANIC_RESISTANCE     implemented in Unit::MagicSpellHitResult
    &Aura::HandleNoImmediateEffect,                         //118 SPELL_AURA_MOD_HEALING_PCT             implemented in Unit::SpellHealingBonusTaken
    &Aura::HandleUnused,                                    //119 unused (3.0.8a-4.3.4) old SPELL_AURA_SHARE_PET_TRACKING
    &Aura::HandleAuraUntrackable,                           //120 SPELL_AURA_UNTRACKABLE
    &Aura::HandleAuraEmpathy,                               //121 SPELL_AURA_EMPATHY
    &Aura::HandleModOffhandDamagePercent,                   //122 SPELL_AURA_MOD_OFFHAND_DAMAGE_PCT
    &Aura::HandleModTargetResistance,                       //123 SPELL_AURA_MOD_TARGET_RESISTANCE
    &Aura::HandleAuraModRangedAttackPower,                  //124 SPELL_AURA_MOD_RANGED_ATTACK_POWER
    &Aura::HandleNoImmediateEffect,                         //125 SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN implemented in Unit::MeleeDamageBonusTaken
    &Aura::HandleNoImmediateEffect,                         //126 SPELL_AURA_MOD_MELEE_DAMAGE_TAKEN_PCT implemented in Unit::MeleeDamageBonusTaken
    &Aura::HandleRangedAPBonus,                             //127 SPELL_AURA_RANGED_ATTACK_POWER_ATTACKER_BONUS implemented in Unit::MeleeDamageBonusDone
    &Aura::HandleModPossessPet,                             //128 SPELL_AURA_MOD_POSSESS_PET
    &Aura::HandleAuraModIncreaseSpeed,                      //129 SPELL_AURA_MOD_SPEED_ALWAYS
    &Aura::HandleAuraModIncreaseMountedSpeed,               //130 SPELL_AURA_MOD_MOUNTED_SPEED_ALWAYS
    &Aura::HandleNoImmediateEffect,                         //131 SPELL_AURA_MOD_RANGED_ATTACK_POWER_VERSUS implemented in Unit::MeleeDamageBonusDone
    &Aura::HandleAuraModIncreaseEnergyPercent,              //132 SPELL_AURA_MOD_INCREASE_ENERGY_PERCENT
    &Aura::HandleAuraModIncreaseHealthPercent,              //133 SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT
    &Aura::HandleAuraModRegenInterrupt,                     //134 SPELL_AURA_MOD_MANA_REGEN_INTERRUPT
    &Aura::HandleModHealingDone,                            //135 SPELL_AURA_MOD_HEALING_DONE
    &Aura::HandleNoImmediateEffect,                         //136 SPELL_AURA_MOD_HEALING_DONE_PERCENT   implemented in Unit::SpellHealingBonusDone
    &Aura::HandleModTotalPercentStat,                       //137 SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE
    &Aura::HandleModMeleeSpeedPct,                          //138 SPELL_AURA_MOD_MELEE_HASTE
    &Aura::HandleForceReaction,                             //139 SPELL_AURA_FORCE_REACTION
    &Aura::HandleAuraModRangedHaste,                        //140 SPELL_AURA_MOD_RANGED_HASTE
    &Aura::HandleUnused,                                    //141 SPELL_AURA_MOD_RANGED_AMMO_HASTE
    &Aura::HandleAuraModBaseResistancePCT,                  //142 SPELL_AURA_MOD_BASE_RESISTANCE_PCT
    &Aura::HandleAuraModResistanceExclusive,                //143 SPELL_AURA_MOD_RESISTANCE_EXCLUSIVE
    &Aura::HandleAuraSafeFall,                              //144 SPELL_AURA_SAFE_FALL                  implemented in WorldSession::HandleMovementOpcodes
    &Aura::HandleAuraModPetTalentsPoints,                   //145 SPELL_AURA_MOD_PET_TALENT_POINTS
    &Aura::HandleNoImmediateEffect,                         //146 SPELL_AURA_ALLOW_TAME_PET_TYPE        implemented in Player::CanTameExoticPets
    &Aura::HandleModMechanicImmunityMask,                   //147 SPELL_AURA_MECHANIC_IMMUNITY_MASK     implemented in Unit::IsImmuneToSpell and Unit::IsImmuneToSpellEffect (check part)
    &Aura::HandleAuraRetainComboPoints,                     //148 SPELL_AURA_RETAIN_COMBO_POINTS
    &Aura::HandleNoImmediateEffect,                         //149 SPELL_AURA_REDUCE_PUSHBACK            implemented in Spell::Delayed and Spell::DelayedChannel
    &Aura::HandleModShieldBlockDamage,                      //150 SPELL_AURA_MOD_SHIELD_BLOCKDAMAGE
    &Aura::HandleAuraTrackStealthed,                        //151 SPELL_AURA_TRACK_STEALTHED
    &Aura::HandleNoImmediateEffect,                         //152 SPELL_AURA_MOD_DETECTED_RANGE         implemented in Creature::GetAttackDistance
    &Aura::HandleUnused,                                    //153 old SPELL_AURA_SPLIT_DAMAGE_FLAT
    &Aura::HandleNoImmediateEffect,                         //154 SPELL_AURA_MOD_STEALTH_LEVEL          implemented in Unit::isVisibleForOrDetect
    &Aura::HandleNoImmediateEffect,                         //155 SPELL_AURA_MOD_WATER_BREATHING        implemented in Player::getMaxTimer
    &Aura::HandleNoImmediateEffect,                         //156 SPELL_AURA_MOD_REPUTATION_GAIN        implemented in Player::CalculateReputationGain
    &Aura::HandleUnused,                                    //157 SPELL_AURA_PET_DAMAGE_MULTI (single test like spell 20782, also single for 214 aura)
    &Aura::HandleNULL,                                      //158 SPELL_AURA_MOD_SHIELD_BLOCKVALUE
    &Aura::HandleNoImmediateEffect,                         //159 SPELL_AURA_NO_PVP_CREDIT              implemented in Player::RewardHonor
    &Aura::HandleUnused,                                    //160 old SPELL_AURA_MOD_AOE_AVOIDANCE
    &Aura::HandleNoImmediateEffect,                         //161 SPELL_AURA_MOD_HEALTH_REGEN_IN_COMBAT implemented in Player::RegenerateAll and Player::RegenerateHealth
    &Aura::HandleAuraPowerBurn,                             //162 SPELL_AURA_POWER_BURN_ENERGY
    &Aura::HandleNoImmediateEffect,                         //163 SPELL_AURA_MOD_CRIT_DAMAGE_BONUS      implemented in Unit::CalculateMeleeDamage and Unit::SpellCriticalDamageBonus
    &Aura::HandleUnused,                                    //164 2 spells in 4.3.4
    &Aura::HandleNoImmediateEffect,                         //165 SPELL_AURA_MELEE_ATTACK_POWER_ATTACKER_BONUS implemented in Unit::MeleeDamageBonusDone
    &Aura::HandleAuraModAttackPowerPercent,                 //166 SPELL_AURA_MOD_ATTACK_POWER_PCT
    &Aura::HandleAuraModRangedAttackPowerPercent,           //167 SPELL_AURA_MOD_RANGED_ATTACK_POWER_PCT
    &Aura::HandleNoImmediateEffect,                         //168 SPELL_AURA_MOD_DAMAGE_DONE_VERSUS            implemented in Unit::SpellDamageBonusDone, Unit::MeleeDamageBonusDone
    &Aura::HandleUnused,                                    //169 old SPELL_AURA_MOD_CRIT_PERCENT_VERSUS
    &Aura::HandleDetectAmore,                               //170 SPELL_AURA_DETECT_AMORE       different spells that ignore transformation effects
    &Aura::HandleAuraModIncreaseSpeed,                      //171 SPELL_AURA_MOD_SPEED_NOT_STACK
    &Aura::HandleAuraModIncreaseMountedSpeed,               //172 SPELL_AURA_MOD_MOUNTED_SPEED_NOT_STACK
    &Aura::HandleUnused,                                    //173 unused (3.0.8a-4.3.4) no spells, old SPELL_AURA_ALLOW_CHAMPION_SPELLS  only for Proclaim Champion spell
    &Aura::HandleModSpellDamagePercentFromStat,             //174 SPELL_AURA_MOD_SPELL_DAMAGE_OF_STAT_PERCENT  implemented in Unit::SpellBaseDamageBonusDone
    &Aura::HandleModSpellHealingPercentFromStat,            //175 SPELL_AURA_MOD_SPELL_HEALING_OF_STAT_PERCENT implemented in Unit::SpellBaseHealingBonusDone
    &Aura::HandleSpiritOfRedemption,                        //176 SPELL_AURA_SPIRIT_OF_REDEMPTION   only for Spirit of Redemption spell, die at aura end
    &Aura::HandleNULL,                                      //177 SPELL_AURA_AOE_CHARM (22 spells)
    &Aura::HandleUnused,                                    //178 old SPELL_AURA_MOD_DEBUFF_RESISTANCE
    &Aura::HandleNoImmediateEffect,                         //179 SPELL_AURA_MOD_ATTACKER_SPELL_CRIT_CHANCE implemented in Unit::SpellCriticalBonus
    &Aura::HandleNoImmediateEffect,                         //180 SPELL_AURA_MOD_FLAT_SPELL_DAMAGE_VERSUS   implemented in Unit::SpellDamageBonusDone
    &Aura::HandleUnused,                                    //181 unused (3.0.8a-4.3.4) old SPELL_AURA_MOD_FLAT_SPELL_CRIT_DAMAGE_VERSUS
    &Aura::HandleAuraModResistenceOfStatPercent,            //182 SPELL_AURA_MOD_RESISTANCE_OF_STAT_PERCENT
    &Aura::HandleNoImmediateEffect,                         //183 SPELL_AURA_MOD_CRITICAL_THREAT only used in 28746, implemented in ThreatCalcHelper::CalcThreat
    &Aura::HandleNoImmediateEffect,                         //184 SPELL_AURA_MOD_ATTACKER_MELEE_HIT_CHANCE  implemented in Unit::RollMeleeOutcomeAgainst
    &Aura::HandleNoImmediateEffect,                         //185 SPELL_AURA_MOD_ATTACKER_RANGED_HIT_CHANCE implemented in Unit::RollMeleeOutcomeAgainst
    &Aura::HandleNoImmediateEffect,                         //186 SPELL_AURA_MOD_ATTACKER_SPELL_HIT_CHANCE  implemented in Unit::MagicSpellHitResult
    &Aura::HandleNoImmediateEffect,                         //187 SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_CHANCE  implemented in Unit::GetUnitCriticalChance
    &Aura::HandleNoImmediateEffect,                         //188 SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_CHANCE implemented in Unit::GetUnitCriticalChance
    &Aura::HandleModRating,                                 //189 SPELL_AURA_MOD_RATING
    &Aura::HandleNoImmediateEffect,                         //190 SPELL_AURA_MOD_FACTION_REPUTATION_GAIN     implemented in Player::CalculateReputationGain
    &Aura::HandleAuraModUseNormalSpeed,                     //191 SPELL_AURA_USE_NORMAL_MOVEMENT_SPEED
    &Aura::HandleModMeleeRangedSpeedPct,                    //192 SPELL_AURA_MOD_MELEE_RANGED_HASTE
    &Aura::HandleModCombatSpeedPct,                         //193 SPELL_AURA_HASTE_ALL (in fact combat (any type attack) speed pct)
    &Aura::HandleNoImmediateEffect,                         //194 SPELL_AURA_MOD_IGNORE_ABSORB_SCHOOL       implement in Unit::CalcNotIgnoreAbsorbDamage
    &Aura::HandleNoImmediateEffect,                         //195 SPELL_AURA_MOD_IGNORE_ABSORB_FOR_SPELL    implement in Unit::CalcNotIgnoreAbsorbDamage
    &Aura::HandleNULL,                                      //196 SPELL_AURA_MOD_COOLDOWN (single spell 24818 in 3.2.2a)
    &Aura::HandleNoImmediateEffect,                         //197 SPELL_AURA_MOD_ATTACKER_SPELL_AND_WEAPON_CRIT_CHANCE implemented in Unit::SpellCriticalBonus Unit::GetUnitCriticalChance
    &Aura::HandleUnused,                                    //198 unused (3.0.8a-4.3.4) old SPELL_AURA_MOD_ALL_WEAPON_SKILLS
    &Aura::HandleUnused,                                    //199 old SPELL_AURA_MOD_INCREASES_SPELL_PCT_TO_HIT
    &Aura::HandleNoImmediateEffect,                         //200 SPELL_AURA_MOD_KILL_XP_PCT                 implemented in Player::GiveXP
    &Aura::HandleAuraAllowFlight,                           //201 SPELL_AURA_FLY                             this aura enable flight mode...
    &Aura::HandleNoImmediateEffect,                         //202 SPELL_AURA_IGNORE_COMBAT_RESULT            implemented in Unit::MeleeSpellHitResult
    &Aura::HandleNoImmediateEffect,                         //203 SPELL_AURA_MOD_ATTACKER_MELEE_CRIT_DAMAGE  implemented in Unit::CalculateMeleeDamage and Unit::SpellCriticalDamageBonus
    &Aura::HandleNoImmediateEffect,                         //204 SPELL_AURA_MOD_ATTACKER_RANGED_CRIT_DAMAGE implemented in Unit::CalculateMeleeDamage and Unit::SpellCriticalDamageBonus
    &Aura::HandleNoImmediateEffect,                         //205 SPELL_AURA_MOD_ATTACKER_SPELL_CRIT_DAMAGE  implemented in Unit::SpellCriticalDamageBonus
    &Aura::HandleAuraModIncreaseFlightSpeed,                //206 SPELL_AURA_MOD_FLIGHT_SPEED
    &Aura::HandleAuraModIncreaseFlightSpeed,                //207 SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED
    &Aura::HandleAuraModIncreaseFlightSpeed,                //208 SPELL_AURA_MOD_FLIGHT_SPEED_STACKING
    &Aura::HandleAuraModIncreaseFlightSpeed,                //209 SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED_STACKING
    &Aura::HandleAuraModIncreaseFlightSpeed,                //210 SPELL_AURA_MOD_FLIGHT_SPEED_NOT_STACKING
    &Aura::HandleAuraModIncreaseFlightSpeed,                //211 SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED_NOT_STACKING
    &Aura::HandleUnused,                                    //212 old SPELL_AURA_MOD_RANGED_ATTACK_POWER_OF_STAT_PERCENT
    &Aura::HandleNoImmediateEffect,                         //213 SPELL_AURA_MOD_RAGE_FROM_DAMAGE_DEALT implemented in Player::RewardRage
    &Aura::HandleUnused,                                    //214 Tamed Pet Passive (single test like spell 20782, also single for 157 aura)
    &Aura::HandleArenaPreparation,                          //215 SPELL_AURA_ARENA_PREPARATION
    &Aura::HandleModCastingSpeed,                           //216 SPELL_AURA_HASTE_SPELLS
    &Aura::HandleModMeleeSpeedPct,                          //217 SPELL_AURA_MOD_MELEE_HASTE_2
    &Aura::HandleAuraModRangedHaste,                        //218 SPELL_AURA_HASTE_RANGED
    &Aura::HandleModManaRegen,                              //219 SPELL_AURA_MOD_MANA_REGEN_FROM_STAT
    &Aura::HandleModRatingFromStat,                         //220 SPELL_AURA_MOD_RATING_FROM_STAT
    &Aura::HandleNULL,                                      //221 SPELL_AURA_IGNORED 5 spells in 4.3.4
    &Aura::HandleUnused,                                    //222 2 spells in 4.3.4 Prayer of Mending
    &Aura::HandleNULL,                                      //223 dummy code (cast damage spell to attacker) and another dummy (jump to another nearby raid member)
    &Aura::HandleUnused,                                    //224 unused (3.0.8a-4.3.4)
    &Aura::HandleNoImmediateEffect,                         //225 SPELL_AURA_PRAYER_OF_MENDING
    &Aura::HandleAuraPeriodicDummy,                         //226 SPELL_AURA_PERIODIC_DUMMY
    &Aura::HandlePeriodicTriggerSpellWithValue,             //227 SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE
    &Aura::HandleNoImmediateEffect,                         //228 SPELL_AURA_DETECT_STEALTH
    &Aura::HandleNoImmediateEffect,                         //229 SPELL_AURA_MOD_AOE_DAMAGE_AVOIDANCE        implemented in Unit::SpellDamageBonusTaken
    &Aura::HandleAuraModIncreaseMaxHealth,                  //230 Commanding Shout
    &Aura::HandleNoImmediateEffect,                         //231 SPELL_AURA_PROC_TRIGGER_SPELL_WITH_VALUE
    &Aura::HandleNoImmediateEffect,                         //232 SPELL_AURA_MECHANIC_DURATION_MOD           implement in Unit::CalculateAuraDuration
    &Aura::HandleNULL,                                      //233 set model id to the one of the creature with id m_modifier.m_miscvalue
    &Aura::HandleNoImmediateEffect,                         //234 SPELL_AURA_MECHANIC_DURATION_MOD_NOT_STACK implement in Unit::CalculateAuraDuration
    &Aura::HandleAuraModDispelResist,                       //235 SPELL_AURA_MOD_DISPEL_RESIST               implement in Unit::MagicSpellHitResult
    &Aura::HandleAuraControlVehicle,                        //236 SPELL_AURA_CONTROL_VEHICLE
    &Aura::HandleModSpellDamagePercentFromAttackPower,      //237 SPELL_AURA_MOD_SPELL_DAMAGE_OF_ATTACK_POWER  implemented in Unit::SpellBaseDamageBonusDone
    &Aura::HandleModSpellHealingPercentFromAttackPower,     //238 SPELL_AURA_MOD_SPELL_HEALING_OF_ATTACK_POWER implemented in Unit::SpellBaseHealingBonusDone
    &Aura::HandleAuraModScale,                              //239 SPELL_AURA_MOD_SCALE_2 only in Noggenfogger Elixir (16595) before 2.3.0 aura 61
    &Aura::HandleAuraModExpertise,                          //240 SPELL_AURA_MOD_EXPERTISE
    &Aura::HandleForceMoveForward,                          //241 Forces the caster to move forward
    &Aura::HandleUnused,                                    //242 SPELL_AURA_MOD_SPELL_DAMAGE_FROM_HEALING (only 2 test spels in 3.2.2a)
    &Aura::HandleNULL,                                      //243 faction reaction override spells
    &Aura::HandleComprehendLanguage,                        //244 SPELL_AURA_COMPREHEND_LANGUAGE
    &Aura::HandleNoImmediateEffect,                         //245 SPELL_AURA_MOD_DURATION_OF_MAGIC_EFFECTS     implemented in Unit::CalculateAuraDuration
    &Aura::HandleNoImmediateEffect,                         //246 SPELL_AURA_MOD_DURATION_OF_EFFECTS_BY_DISPEL implemented in Unit::CalculateAuraDuration
    &Aura::HandleAuraMirrorImage,                           //247 SPELL_AURA_MIRROR_IMAGE                      target to become a clone of the caster
    &Aura::HandleNoImmediateEffect,                         //248 SPELL_AURA_MOD_COMBAT_RESULT_CHANCE         implemented in Unit::RollMeleeOutcomeAgainst
    &Aura::HandleAuraConvertRune,                           //249 SPELL_AURA_CONVERT_RUNE
    &Aura::HandleAuraModIncreaseHealth,                     //250 SPELL_AURA_MOD_INCREASE_HEALTH_2
    &Aura::HandleNULL,                                      //251 SPELL_AURA_MOD_ENEMY_DODGE
    &Aura::HandleModCombatSpeedPct,                         //252 SPELL_AURA_SLOW_ALL
    &Aura::HandleAuraModBlockCritChance,                    //253 SPELL_AURA_MOD_BLOCK_CRIT_CHANCE              obsolete in 4.x, but spells exist
    &Aura::HandleAuraModDisarm,                             //254 SPELL_AURA_MOD_DISARM_OFFHAND     also disarm shield
    &Aura::HandleNoImmediateEffect,                         //255 SPELL_AURA_MOD_MECHANIC_DAMAGE_TAKEN_PERCENT    implemented in Unit::SpellDamageBonusTaken
    &Aura::HandleNoReagentUseAura,                          //256 SPELL_AURA_NO_REAGENT_USE Use SpellClassMask for spell select
    &Aura::HandleNULL,                                      //257 SPELL_AURA_MOD_TARGET_RESIST_BY_SPELL_CLASS Use SpellClassMask for spell select
    &Aura::HandleNULL,                                      //258 SPELL_AURA_MOD_SPELL_VISUAL
    &Aura::HandleUnused,                                    //259 old SPELL_AURA_MOD_PERIODIC_HEAL
    &Aura::HandleNoImmediateEffect,                         //260 SPELL_AURA_SCREEN_EFFECT (miscvalue = id in ScreenEffect.dbc) not required any code
    &Aura::HandlePhase,                                     //261 SPELL_AURA_PHASE undetectable invisibility?     implemented in Unit::isVisibleForOrDetect
    &Aura::HandleNoImmediateEffect,                         //262 SPELL_AURA_IGNORE_UNIT_STATE Alows some abilities whitch are aviable only in some cases.... implemented in Spell::CheckCast
    &Aura::HandleAllowOnlyAbility,                          //263 SPELL_AURA_ALLOW_ONLY_ABILITY
    &Aura::HandleUnused,                                    //264 1 spell in 4.3.4 Deterrence
    &Aura::HandleUnused,                                    //265 unused (3.0.8a-4.3.4)
    &Aura::HandleUnused,                                    //266 unused (3.0.8a-4.3.4)
    &Aura::HandleNoImmediateEffect,                         //267 SPELL_AURA_MOD_IMMUNE_AURA_APPLY_SCHOOL         implemented in Unit::IsImmuneToSpellEffect
    &Aura::HandleUnused,                                    //268 old SPELL_AURA_MOD_ATTACK_POWER_OF_STAT_PERCENT
    &Aura::HandleNoImmediateEffect,                         //269 SPELL_AURA_MOD_IGNORE_DAMAGE_REDUCTION_SCHOOL   implemented in Unit::CalcNotIgnoreDamageReduction
    &Aura::HandleUnused,                                    //270 old SPELL_AURA_MOD_IGNORE_TARGET_RESIST
    &Aura::HandleNoImmediateEffect,                         //271 SPELL_AURA_MOD_DAMAGE_FROM_CASTER    implemented in Unit::SpellDamageBonusTaken
    &Aura::HandleNoImmediateEffect,                         //272 SPELL_AURA_IGNORE_MELEE_RESET (unclear use for aura, it used in (3.2.2a...3.3.0) in single spell 53817 that spellmode stacked and charged spell expected to be drop as stack
    &Aura::HandleNoImmediateEffect,                         //273 SPELL_AURA_X_RAY (client side implementation)
    &Aura::HandleNULL,                                      //274 proc free shot?
    &Aura::HandleNoImmediateEffect,                         //275 SPELL_AURA_MOD_IGNORE_SHAPESHIFT Use SpellClassMask for spell select
    &Aura::HandleNoImmediateEffect,                         //276 SPELL_AURA_MOD_DAMAGE_DONE_BY_MECHANIC
    &Aura::HandleUnused,                                    //277 old SPELL_AURA_MOD_MAX_AFFECTED_TARGETS
    &Aura::HandleAuraModDisarm,                             //278 SPELL_AURA_MOD_DISARM_RANGED disarm ranged weapon
    &Aura::HandleAuraInitializeImages,                      //279 SPELL_AURA_INITIALIZE_IMAGES
    &Aura::HandleUnused,                                    //280 old SPELL_AURA_MOD_TARGET_ARMOR_PCT
    &Aura::HandleNoImmediateEffect,                         //281 SPELL_AURA_MOD_GUILD_REPUTATION_GAIN
    &Aura::HandleAuraIncreaseBaseHealthPercent,             //282 SPELL_AURA_INCREASE_BASE_HEALTH_PERCENT
    &Aura::HandleNoImmediateEffect,                         //283 SPELL_AURA_MOD_HEALING_RECEIVED       implemented in Unit::SpellHealingBonusTaken
    &Aura::HandleAuraLinked,                                //284 SPELL_AURA_LINKED 51 spells
    &Aura::HandleAuraModAttackPowerOfArmor,                 //285 SPELL_AURA_MOD_ATTACK_POWER_OF_ARMOR  implemented in Player::UpdateAttackPowerAndDamage
    &Aura::HandleNoImmediateEffect,                         //286 SPELL_AURA_ABILITY_PERIODIC_CRIT      implemented in Aura::IsCritFromAbilityAura called from Aura::PeriodicTick
    &Aura::HandleNoImmediateEffect,                         //287 SPELL_AURA_DEFLECT_SPELLS             implemented in Unit::MagicSpellHitResult and Unit::MeleeSpellHitResult
    &Aura::HandleNoImmediateEffect,                         //288 SPELL_AURA_MOD_PARRY_FROM_BEHIND_PERCENT percent from normal parry/deflect applied to from behind attack case (single spell used 67801, also look 4.1.0 spell 97574)
    &Aura::HandleUnused,                                    //289 1 spell in 4.3.4 Mortality
    &Aura::HandleAuraModAllCritChance,                      //290 SPELL_AURA_MOD_ALL_CRIT_CHANCE
    &Aura::HandleNoImmediateEffect,                         //291 SPELL_AURA_MOD_QUEST_XP_PCT           implemented in Player::GiveXP
    &Aura::HandleAuraOpenStable,                            //292 call stabled pet
    &Aura::HandleAuraAddMechanicAbilities,                  //293 SPELL_AURA_ADD_MECHANIC_ABILITIES  replaces target's action bars with a predefined spellset
    &Aura::HandleAuraStopNaturalManaRegen,                  //294 SPELL_AURA_STOP_NATURAL_MANA_REGEN implemented in Player:Regenerate
    &Aura::HandleUnused,                                    //295 unused (4.3.4)
    &Aura::HandleAuraSetVehicle,                            //296 SPELL_AURA_SET_VEHICLE_ID 62 spells in 4.3.4
    &Aura::HandleNULL,                                      //297 14 spells in 4.3.4
    &Aura::HandleUnused,                                    //298 6 spells in 4.3.4
    &Aura::HandleUnused,                                    //299 unused (3.2.2a-4.3.4)
    &Aura::HandleNULL,                                      //300 21 spells (share damage?)
    &Aura::HandleNULL,                                      //301 SPELL_AURA_HEAL_ABSORB 31 spells
    &Aura::HandleUnused,                                    //302 unused (3.2.2a-4.3.4)
    &Aura::HandleNoImmediateEffect,                         //303 AURA_STATE_BLEEDING 35 spells increases damage done vs. aurastate
    &Aura::HandleAuraFakeInebriation,                       //304 SPELL_AURA_FAKE_INEBRIATE
    &Aura::HandleAuraModIncreaseSpeed,                      //305 SPELL_AURA_MOD_MINIMUM_SPEED
    &Aura::HandleUnused,                                    //306 0 spells in 4.3.4
    &Aura::HandleUnused,                                    //307 0 spells in 4.3.4
    &Aura::HandleNULL,                                      //308 SPELL_AURA_MOD_CRIT_FROM_CASTER
    &Aura::HandleNULL,                                      //309 0 spells in 4.3.4
    &Aura::HandleNoImmediateEffect,                         //310 SPELL_AURA_MOD_PET_AOE_DAMAGE_AVOIDANCE implemented in Unit::SpellDamageBonusTaken
    &Aura::HandleNULL,                                      //311 2 spells in 4.3.4 some kind of stun effect
    &Aura::HandleNULL,                                      //312 37 spells in 4.3.4 some cosmetic auras
    &Aura::HandleNULL,                                      //313 0 spells in 4.3.4
    &Aura::HandleAuraPreventResurrection,                   //314 SPELL_AURA_PREVENT_RESURRECTION 2 spells int 4.3.4 prevents ressurection ?
    &Aura::HandleNULL,                                      //315 SPELL_AURA_UNDERWATER_WALKING 4 spells in 4.3.4 underwater walking
    &Aura::HandleUnused,                                    //316 old SPELL_AURA_MOD_PERIODIC_HASTE 0 spells in 4.3.4
    &Aura::HandleModIncreaseSpellPowerPct,                  //317 SPELL_AURA_MOD_INCREASE_SPELL_POWER_PCT 13 spells in 4.3.4, implemented in Unit::SpellBaseDamageBonusDone and Unit::SpellBaseHealingBonusDone
    &Aura::HandleAuraMastery,                               //318 SPELL_AURA_MASTERY 12 spells in 4.3
    &Aura::HandleModMeleeSpeedPct,                          //319 SPELL_AURA_MOD_MELEE_HASTE_3 47 spells in 4.3.4
    &Aura::HandleAuraModRangedHaste,                        //320 SPELL_AURA_MOD_RANGED_HASTE_2 5 spells in 4.3.4
    &Aura::HandleNULL,                                      //321 1 spells in 4.3 Hex
    &Aura::HandleAuraInterfereTargeting,                    //322 SPELL_AURA_INTERFERE_TARGETING 6 spells in 4.3
    &Aura::HandleUnused,                                    //323 0 spells in 4.3.4
    &Aura::HandleNULL,                                      //324 2 spells in 4.3.4 test spells
    &Aura::HandleUnused,                                    //325 0 spells in 4.3.4
    &Aura::HandlePhase,                                     //326 SPELL_AURA_PHASE_2 24 spells in 4.3.4 new phase auras
    &Aura::HandleUnused,                                    //327 0 spells in 4.3.4
    &Aura::HandleNoImmediateEffect,                         //328 SPELL_AURA_PROC_ON_TARGET_AMOUNT 2 spells in 4.3.4 Eclipse Mastery Driver Passive
    &Aura::HandleNULL,                                      //329 SPELL_AURA_MOD_RUNIC_POWER_GAIN 3 spells in 4.3.4
    &Aura::HandleNoImmediateEffect,                         //330 SPELL_AURA_ALLOW_CAST_WHILE_MOVING 16 spells in 4.3.4
    &Aura::HandleAuraForceWeather,                          //331 SPELL_AURA_MOD_WEATHER 10 spells in 4.3.4
    &Aura::HandleAuraOverrideActionbarSpells,               //332 SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS 16 spells in 4.3.4, implemented in WorldSession::HandleCastSpellOpcode
    &Aura::HandleAuraOverrideActionbarSpells,               //333 SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS_2 10 spells in 4.3.4, implemented in WorldSession::HandleCastSpellOpcode
    &Aura::HandleNULL,                                      //334 SPELL_AURA_BLIND_SIGHT 2 spells in 4.3.4
    &Aura::HandleAuraSeeWhileInvisible,                     //335 SPELL_AURA_SEE_WHILE_INVISIBLE implemented in Unit::isVisibleForOrDetect 5 spells in 4.3.4
    &Aura::HandleNULL,                                      //336 SPELL_AURA_FLIGHT_RESTRICTIONS 8 spells in 4.3.4
    &Aura::HandleNoImmediateEffect,                         //337 SPELL_AURA_MOD_VENDOR_PRICE 1 spells in 4.3.4
    &Aura::HandleNoImmediateEffect,                         //338 SPELL_AURA_MOD_DURABILITY_LOSS 3 spells in 4.3.4,  implemented in Player::DurabilityLossAll
    &Aura::HandleNoImmediateEffect,                         //339 SPELL_AURA_MOD_SKILLCHANCE 1 spells in 4.3.4, implemented in Player::UpdateSkillPro
    &Aura::HandleNoImmediateEffect,                         //340 SPELL_AURA_MOD_RESURRECTED_HEALTH_BY_GUILD_MEMBER 1 spells in 4.3.4
    &Aura::HandleAuraModCategoryCooldown,                   //341 SPELL_AURA_MOD_SPELL_CATEGORY_COOLDOWN spells in 4.3.4 implemented in Player::AddSpellAndCategoryCooldowns
    &Aura::HandleModMeleeRangedSpeedPct,                    //342 SPELL_AURA_MOD_ATTACKSPEED_2 17 spells in 4.3.4
    &Aura::HandleNULL,                                      //343 SPELL_AURA_MOD_ALL_DAMAGE_FROM_CASTER 2 spells in 4.3.4
    &Aura::HandleNoImmediateEffect,                         //344 SPELL_AURA_MOD_AUTOATTACK_DAMAGE 6 spells in 4.3.4, implemented in Unit::MeleeDamageBonusDone
    &Aura::HandleNoImmediateEffect,                         //345 SPELL_AURA_MOD_IGNORE_ARMOR_PCT 3 spells in 4.3.4, implemented in Unit::CalcArmorReducedDamage
    &Aura::HandleAltPowerIndicator,                         //346 SPELL_AURA_ALT_POWER_INDICATOR 32 spells in 4.3.4 shows progressbar-like ui?
    &Aura::HandleNoImmediateEffect,                         //347 SPELL_AURA_MOD_CD_FROM_HASTE 2 spells in 4.3.4, implemented in Player::AddSpellAndCategoryCooldowns
    &Aura::HandleNoImmediateEffect,                         //348 SPELL_AURA_MOD_MONEY_TO_GUILD_BANK 2 spells in 4.3.4, implemented in WorldSession::HandleLootMoneyOpcode
    &Aura::HandleNoImmediateEffect,                         //349 SPELL_AURA_MOD_CURRENCY_GAIN 16 spells in 4.3.4, implemented in Player::ModifyCurrencyCount
    &Aura::HandleNoImmediateEffect,                         //350 SPELL_AURA_MOD_ITEM_LOOT 1 spells in 4.3.4
    &Aura::HandleNoImmediateEffect,                         //351 SPELL_AURA_MOD_CURRENCY_LOOT 8 spells in 4.3.4
    &Aura::HandleNoImmediateEffect,                         //352 SPELL_AURA_ALLOW_WORGEN_TRANSFORM 1 spells in 4.3.4 enables worgen<>human form switches
    &Aura::HandleNoImmediateEffect,                         //353 SPELL_AURA_CAMOUFLAGE 3 spells in 4.3.4
    &Aura::HandleNoImmediateEffect,                         //354 SPELL_AURA_MOD_HEALING_DONE_FROM_PCT_HEALTH 2 spells in 4.3.4, implemented in Unit::SpellHealingBonusDone
    &Aura::HandleUnused,                                    //355 0 spells in 4.3.4
    &Aura::HandleNoImmediateEffect,                         //356 SPELL_AURA_MOD_DAMAGE_DONE_FROM_PCT_POWER 2 spells in 4.3.4, imlemented in Unit::SpellDamageBonusDone
    &Aura::HandleNULL,                                      //357 1 spells in 4.3.4 Enable Tower Assault Unit Frame
    &Aura::HandleNoImmediateEffect,                         //358 SPELL_AURA_WORGEN_TRANSFORM 5 spells in 4.3.4 forces client-side transformation to worgen form
    &Aura::HandleNULL,                                      //359 1 spells in 4.3.4 healing done vs aurastate?
    &Aura::HandleNULL,                                      //360 SPELL_AURA_PROC_DUPLICATE_SPELLS 2 spells in 4.3.4
    &Aura::HandleNULL,                                      //361 SPELL_AURA_PROC_TRIGGER_SPELL_2 4 spells
    &Aura::HandleUnused,                                    //362 0 spells in 4.3.4
    &Aura::HandleNULL,                                      //363 1 spells in 4.3.4 Throw Totem
    &Aura::HandleUnused,                                    //364 0 spells in 4.3.4
    &Aura::HandleNULL,                                      //365 1 spells in 4.3.4 Max Far Clip Plane
    &Aura::HandleOverrideSpellPowerByAp,                    //366 SPELL_AURA_OVERRIDE_SPELL_POWER_BY_AP_PCT 1 spells in 4.3.4
    &Aura::HandleNULL,                                      //367 2 spells in 4.3.4 test spells
    &Aura::HandleUnused,                                    //368 0 spells in 4.3.4
    &Aura::HandleNULL,                                      //369 5 spells in 4.3.4 darkmoon faire related
    &Aura::HandleNULL                                       //370 1 spells in 4.3.4 Fair Far Clip
};

static AuraType const frozenAuraTypes[] = { SPELL_AURA_MOD_ROOT, SPELL_AURA_MOD_STUN, SPELL_AURA_NONE };

Aura::Aura(SpellEntry const* spellproto, SpellEffectIndex eff, int32 *currentBasePoints, SpellAuraHolder *holder, Unit *target, Unit *caster, Item* castItem) :
m_periodicTimer(0), m_periodicTick(0), m_removeMode(AURA_REMOVE_BY_DEFAULT),
m_effIndex(eff), m_positive(false), m_isPeriodic(false), m_isAreaAura(false),
m_isPersistent(false), m_in_use(0), m_spellAuraHolder(holder)
{
    MANGOS_ASSERT(target);
    MANGOS_ASSERT(spellproto && spellproto == sSpellStore.LookupEntry( spellproto->Id ) && "`info` must be pointer to sSpellStore element");

    m_spellEffect = spellproto->GetSpellEffect(m_effIndex);

    MANGOS_ASSERT(m_spellEffect);                           // need testing...

    m_currentBasePoints = currentBasePoints ? *currentBasePoints : m_spellEffect->CalculateSimpleValue();

    m_positive = IsPositiveEffect(spellproto, m_effIndex);
    m_applyTime = time(NULL);

    int32 damage;
    if(!caster)
        damage = m_currentBasePoints;
    else
    {
        damage = caster->CalculateSpellDamage(target, spellproto, m_effIndex, &m_currentBasePoints);

        if (!damage && castItem && castItem->GetItemSuffixFactor())
        {
            ItemRandomSuffixEntry const *item_rand_suffix = sItemRandomSuffixStore.LookupEntry(abs(castItem->GetItemRandomPropertyId()));
            if(item_rand_suffix)
            {
                for (int k = 0; k < 3; ++k)
                {
                    SpellItemEnchantmentEntry const *pEnchant = sSpellItemEnchantmentStore.LookupEntry(item_rand_suffix->enchant_id[k]);
                    if(pEnchant)
                    {
                        for (int t = 0; t < 3; ++t)
                            if(pEnchant->spellid[t] == spellproto->Id)
                        {
                            damage = uint32((item_rand_suffix->prefix[k]*castItem->GetItemSuffixFactor()) / 10000 );
                            break;
                        }
                    }

                    if(damage)
                        break;
                }
            }
        }
    }

    damage *= holder->GetStackAmount();
    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "Aura: construct Spellid : %u, Aura : %u Target : %d Damage : %d Positive: %u", spellproto->Id, m_spellEffect->EffectApplyAuraName, m_spellEffect->EffectImplicitTargetA, damage, m_positive);

    SetModifier(AuraType(m_spellEffect->EffectApplyAuraName), damage, m_spellEffect->EffectAmplitude, m_spellEffect->EffectMiscValue);

    if (m_modifier.periodictime)
    {
        if (Player* modOwner = caster ? caster->GetSpellModOwner() : NULL)
        {
            int32 newperiodictime = m_modifier.periodictime;
            modOwner->ApplySpellMod(spellproto->Id, SPELLMOD_ACTIVATION_TIME, newperiodictime);
            m_modifier.periodictime = uint32(ApplyHasteToDuration(spellproto, modOwner, newperiodictime));
        }

        if (spellproto->HasAttribute(SPELL_ATTR_EX8_HASTE_ADD_TICKS))
        {
            int32 diff = holder->GetAuraMaxDurationWithoutHaste() - holder->GetAuraMaxDuration();
            int32 addTicks = diff > 0 ? int32(float(diff) / m_modifier.periodictime + 0.5f) : 0;
            if (addTicks)
            {
                int32 newDuration = holder->GetAuraMaxDuration() + addTicks * m_modifier.periodictime;
                holder->SetAuraMaxDuration(newDuration);
                holder->SetAuraDuration(newDuration);
            }
        }
    }

    if (caster && caster->GetTypeId() == TYPEID_PLAYER && spellproto->GetSpellFamilyName() == SPELLFAMILY_POTION)
    {
        SpellSpecific spellspec = GetSpellSpecific(spellproto->Id);
        switch (spellspec)
        {
            case SPELL_BATTLE_ELIXIR:
            case SPELL_GUARDIAN_ELIXIR:
            case SPELL_FLASK_ELIXIR:
            {
                // Recipe Id is stored here
                if (SpellEffectEntry const * effect = spellproto->GetSpellEffect(EFFECT_INDEX_0))
                {
                    uint32 trigger = effect->EffectTriggerSpell;
                    // Player knows that recipe and has Mixology
                    if (trigger && caster->HasSpell(trigger) && caster->HasAura(53042))
                    {
                        // Need recalculate duration - may exist more than one aura
                        // with SPELLFAMILY_POTION in SpellAuraHolder
                        uint32 duration = CalculateSpellDuration(spellproto, caster);
                        GetHolder()->SetAuraMaxDuration(duration * 2);
                        GetHolder()->SetAuraDuration(duration * 2);
                        m_modifier.m_amount *= 1.3f;
                    }
                }
                break;
            }
        }
    }

    // Start periodic on next tick or at aura apply
    if (!spellproto->HasAttribute(SPELL_ATTR_EX5_START_PERIODIC_AT_APPLY))
        m_periodicTimer = m_modifier.periodictime;

    // Earthbind Totem Passive
    if (spellproto->Id == 6474)
        m_periodicTimer = 1000;
}

Aura::~Aura()
{
}

AreaAura::AreaAura(SpellEntry const* spellproto, SpellEffectIndex eff, int32 *currentBasePoints, SpellAuraHolder *holder, Unit *target,
Unit *caster, Item* castItem) : Aura(spellproto, eff, currentBasePoints, holder, target, caster, castItem)
{
    m_isAreaAura = true;

    // caster==NULL in constructor args if target==caster in fact
    Unit* caster_ptr = caster ? caster : target;

    m_radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(m_spellEffect->GetRadiusIndex()));
    if(Player* modOwner = caster_ptr->GetSpellModOwner())
        modOwner->ApplySpellMod(spellproto->Id, SPELLMOD_RADIUS, m_radius);

    switch(m_spellEffect->Effect)
    {
        case SPELL_EFFECT_APPLY_AREA_AURA_PARTY:
            m_areaAuraType = AREA_AURA_PARTY;
            break;
        case SPELL_EFFECT_APPLY_AREA_AURA_RAID:
            m_areaAuraType = AREA_AURA_RAID;
            // Light's Beacon not applied to caster itself (TODO: more generic check for another similar spell if any?)
            if (target == caster_ptr && spellproto->Id == 53651)
                m_modifier.m_auraname = SPELL_AURA_NONE;
            break;
        case SPELL_EFFECT_APPLY_AREA_AURA_FRIEND:
            m_areaAuraType = AREA_AURA_FRIEND;
            break;
        case SPELL_EFFECT_APPLY_AREA_AURA_ENEMY:
            m_areaAuraType = AREA_AURA_ENEMY;
            if (target == caster_ptr)
                m_modifier.m_auraname = SPELL_AURA_NONE;    // Do not do any effect on self
            break;
        case SPELL_EFFECT_APPLY_AREA_AURA_PET:
            m_areaAuraType = AREA_AURA_PET;
            break;
        case SPELL_EFFECT_APPLY_AREA_AURA_OWNER:
            m_areaAuraType = AREA_AURA_OWNER;
            if (target == caster_ptr)
                m_modifier.m_auraname = SPELL_AURA_NONE;
            break;
        default:
            ERROR_LOG("Wrong spell effect in AreaAura constructor");
            MANGOS_ASSERT(false);
            break;
    }

    // totems are immune to any kind of area auras
    if (target->GetTypeId() == TYPEID_UNIT && ((Creature*)target)->IsTotem())
        m_modifier.m_auraname = SPELL_AURA_NONE;
}

AreaAura::~AreaAura()
{
}

PersistentAreaAura::PersistentAreaAura(SpellEntry const* spellproto, SpellEffectIndex eff, int32 *currentBasePoints, SpellAuraHolder *holder, Unit *target,
Unit *caster, Item* castItem) : Aura(spellproto, eff, currentBasePoints, holder, target, caster, castItem)
{
    m_isPersistent = true;
}

PersistentAreaAura::~PersistentAreaAura()
{
}

SingleEnemyTargetAura::SingleEnemyTargetAura(SpellEntry const* spellproto, SpellEffectIndex eff, int32 *currentBasePoints, SpellAuraHolder *holder, Unit *target,
Unit *caster, Item* castItem) : Aura(spellproto, eff, currentBasePoints, holder, target, caster, castItem)
{
    if(caster && caster->GetTypeId()==TYPEID_PLAYER && ((Player*)caster)->m_lastSpellTargetGuid && spellproto->GetEffectApplyAuraNameByIndex(eff) != 227)
        m_castersTargetGuid = ((Player*)caster)->m_lastSpellTargetGuid;
    else if (caster)
        m_castersTargetGuid = caster->GetTypeId()==TYPEID_PLAYER ? ((Player*)caster)->GetSelectionGuid() : caster->GetTargetGuid();
}

SingleEnemyTargetAura::~SingleEnemyTargetAura()
{
}

Unit* SingleEnemyTargetAura::GetTriggerTarget() const
{
    return ObjectAccessor::GetUnit(*(m_spellAuraHolder->GetTarget()), m_castersTargetGuid);
}

Aura* CreateAura(SpellEntry const* spellproto, SpellEffectIndex eff, int32 *currentBasePoints, SpellAuraHolder *holder, Unit *target, Unit *caster, Item* castItem)
{
    SpellEffectEntry const* effectEntry = spellproto->GetSpellEffect(eff);

    if (effectEntry && IsAreaAuraEffect(effectEntry->Effect))
        return new AreaAura(spellproto, eff, currentBasePoints, holder, target, caster, castItem);

    uint32 triggeredSpellId = effectEntry ? effectEntry->EffectTriggerSpell : 0;

    if(SpellEntry const* triggeredSpellInfo = sSpellStore.LookupEntry(triggeredSpellId))
    {
        for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
        {
            SpellEffectEntry const* triggeredeffectEntry = triggeredSpellInfo->GetSpellEffect(SpellEffectIndex(i));
            if (triggeredeffectEntry && triggeredeffectEntry->EffectImplicitTargetA == TARGET_SINGLE_ENEMY)
                return new SingleEnemyTargetAura(spellproto, eff, currentBasePoints, holder, target, caster, castItem);
        }
    }
    return new Aura(spellproto, eff, currentBasePoints, holder, target, caster, castItem);
}

SpellAuraHolder* CreateSpellAuraHolder(SpellEntry const* spellproto, Unit *target, WorldObject *caster, Item *castItem)
{
    return new SpellAuraHolder(spellproto, target, caster, castItem);
}

void Aura::SetModifier(AuraType t, int32 a, uint32 pt, int32 miscValue)
{
    m_modifier.m_auraname = t;
    m_modifier.m_amount = a;
    m_modifier.m_miscvalue = miscValue;
    m_modifier.periodictime = pt;
}

void Aura::UpdateModifierAmount(int32 amount)
{
    // use this method, to modify modifier.amount when aura is already applied
    AuraType aura = m_modifier.m_auraname;

    SetInUse(true);
    if(aura < TOTAL_AURAS)
    {
        // maybe we can find a better way here?
        (*this.*AuraHandler [aura])(false, true);
        m_modifier.m_amount = amount;
        (*this.*AuraHandler [aura])(true, true);
    }
    SetInUse(false);
}

void Aura::Update(uint32 diff)
{
    if (m_isPeriodic)
    {
        m_periodicTimer -= diff;
        if(m_periodicTimer <= 0) // tick also at m_periodicTimer==0 to prevent lost last tick in case max m_duration == (max m_periodicTimer)*N
        {
            // update before applying (aura can be removed in TriggerSpell or PeriodicTick calls)
            m_periodicTimer += m_modifier.periodictime;
            ++m_periodicTick;                               // for some infinity auras in some cases can overflow and reset
            PeriodicTick();
        }
    }
}

void AreaAura::Update(uint32 diff)
{
    // update for the caster of the aura
    if(GetCasterGuid() == GetTarget()->GetObjectGuid())
    {
        Unit* caster = GetTarget();

        if( !caster->hasUnitState(UNIT_STAT_ISOLATED) )
        {
            Unit* owner = caster->GetCharmerOrOwner();
            if (!owner)
                owner = caster;
            Spell::UnitList targets;

            switch(m_areaAuraType)
            {
                case AREA_AURA_PARTY:
                {
                    Group *pGroup = NULL;

                    if (owner->GetTypeId() == TYPEID_PLAYER)
                        pGroup = ((Player*)owner)->GetGroup();

                    if( pGroup)
                    {
                        uint8 subgroup = ((Player*)owner)->GetSubGroup();
                        for(GroupReference *itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
                        {
                            Player* Target = itr->getSource();
                            if(Target && Target->isAlive() && Target->GetSubGroup()==subgroup && caster->IsFriendlyTo(Target))
                            {
                                if(caster->IsWithinDistInMap(Target, m_radius))
                                    targets.push_back(Target);
                                Pet *pet = Target->GetPet();
                                if(pet && pet->isAlive() && caster->IsWithinDistInMap(pet, m_radius))
                                    targets.push_back(pet);
                            }
                        }
                    }
                    else
                    {
                        // add owner
                        if( owner != caster && caster->IsWithinDistInMap(owner, m_radius) )
                            targets.push_back(owner);
                        else if (owner == caster)
                            targets.push_back(owner);
                        // add caster's pet
                        Unit* pet = caster->GetPet();
                        if( pet && caster->IsWithinDistInMap(pet, m_radius))
                            targets.push_back(pet);
                    }
                    break;
                }
                case AREA_AURA_RAID:
                {
                    Group *pGroup = NULL;

                    if (owner->GetTypeId() == TYPEID_PLAYER)
                        pGroup = ((Player*)owner)->GetGroup();

                    if( pGroup)
                    {
                        for(GroupReference *itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
                        {
                            Player* Target = itr->getSource();
                            if(Target && Target->isAlive() && caster->IsFriendlyTo(Target))
                            {
                                if(caster->IsWithinDistInMap(Target, m_radius))
                                    targets.push_back(Target);
                                Pet *pet = Target->GetPet();
                                if(pet && pet->isAlive() && caster->IsWithinDistInMap(pet, m_radius))
                                    targets.push_back(pet);
                            }
                        }
                    }
                    else
                    {
                        // add owner
                        if( owner != caster && caster->IsWithinDistInMap(owner, m_radius) )
                            targets.push_back(owner);
                        else if (owner == caster)
                            targets.push_back(owner);
                        // add caster's pet
                        Unit* pet = caster->GetPet();
                        if( pet && caster->IsWithinDistInMap(pet, m_radius))
                            targets.push_back(pet);
                    }
                    break;
                }
                case AREA_AURA_FRIEND:
                {
                    MaNGOS::AnyFriendlyUnitInObjectRangeCheck u_check(caster, m_radius);
                    MaNGOS::UnitListSearcher<MaNGOS::AnyFriendlyUnitInObjectRangeCheck> searcher(targets, u_check);
                    Cell::VisitAllObjects(caster, searcher, m_radius);
                    break;
                }
                case AREA_AURA_ENEMY:
                {
                    MaNGOS::AnyAoETargetUnitInObjectRangeCheck u_check(caster, m_radius); // No GetCharmer in searcher
                    MaNGOS::UnitListSearcher<MaNGOS::AnyAoETargetUnitInObjectRangeCheck> searcher(targets, u_check);
                    Cell::VisitAllObjects(caster, searcher, m_radius);
                    break;
                }
                case AREA_AURA_OWNER:
                case AREA_AURA_PET:
                {
                    if(owner != caster && caster->IsWithinDistInMap(owner, m_radius))
                        targets.push_back(owner);
                    else if (owner == caster)
                        targets.push_back(owner);
                    break;
                }
            }

            for (Spell::UnitList::iterator tIter = targets.begin(); tIter != targets.end(); ++tIter)
            {
                // flag for selection is need apply aura to current iteration target
                bool apply = true;

                switch (GetId())
                {
                    case 45822: // Iceblood Warmaster
                    case 45823: // Tower Point Warmaster
                    case 45824: // West Frostwolf Warmaster
                    case 45826: // East Frostwolf Warmaster
                    case 45828: // Dun Baldar North Marshal
                    case 45829: // Dun Baldar South Marshal
                    case 45830: // Stonehearth Marshal
                    case 45831: // Icewing Marshal
                        if ((*tIter)->GetObjectGuid().IsPlayerOrPet() ||
                            (*tIter)->GetEntry() != 11946 && (*tIter)->GetEntry() != 11948 ||
                            (*tIter)->GetCharmerOrOwnerPlayerOrPlayerItself())
                            continue;
                        break;
                }

                // we need ignore present caster self applied are auras sometime
                // in cases if this only auras applied for spell effect
                Unit::SpellAuraHolderBounds spair = (*tIter)->GetSpellAuraHolderBounds(GetId());
                for(Unit::SpellAuraHolderMap::const_iterator i = spair.first; i != spair.second; ++i)
                {
                    if (i->second->IsDeleted())
                        continue;

                    Aura *aur = i->second->GetAuraByEffectIndex(m_effIndex);

                    if (!aur)
                        continue;

                    switch(m_areaAuraType)
                    {
                        case AREA_AURA_ENEMY:
                            // non caster self-casted auras (non stacked)
                            if(aur->GetModifier()->m_auraname != SPELL_AURA_NONE)
                                apply = false;
                            break;
                        case AREA_AURA_RAID:
                            // non caster self-casted auras (stacked from diff. casters)
                            if(aur->GetModifier()->m_auraname != SPELL_AURA_NONE  || i->second->GetCasterGuid() == GetCasterGuid())
                                apply = false;
                            break;
                        default:
                            // in generic case not allow stacking area auras
                            apply = false;
                            break;
                    }

                    if(!apply)
                        break;
                }

                if(!apply)
                    continue;

                // Skip some targets (TODO: Might require better checks, also unclear how the actual caster must/can be handled)
                if (GetSpellProto()->HasAttribute(SPELL_ATTR_EX3_TARGET_ONLY_PLAYER) && (*tIter)->GetTypeId() != TYPEID_PLAYER)
                    continue;

                if (SpellEntry const* actualSpellInfo = sSpellMgr.SelectAuraRankForLevel(GetSpellProto(), (*tIter)->getLevel()))
                {
                    int32 actualBasePoints = m_currentBasePoints;
                    // recalculate basepoints for lower rank (all AreaAura spell not use custom basepoints?)
                    if(actualSpellInfo != GetSpellProto())
                        actualBasePoints = actualSpellInfo->CalculateSimpleValue(m_effIndex);

                    SpellAuraHolder *holder = (*tIter)->GetSpellAuraHolder(actualSpellInfo->Id, GetCasterGuid());

                    bool addedToExisting = true;
                    if (!holder)
                    {
                        holder = CreateSpellAuraHolder(actualSpellInfo, (*tIter), caster);
                        addedToExisting = false;
                    }

                    holder->SetAuraDuration(GetAuraDuration());

                    AreaAura *aur = new AreaAura(actualSpellInfo, m_effIndex, &actualBasePoints, holder, (*tIter), caster, NULL);
                    holder->AddAura(aur, m_effIndex);

                    if (addedToExisting)
                    {
                        (*tIter)->AddAuraToModList(aur);
                        holder->SetInUse(true);
                        aur->ApplyModifier(true,true);
                        holder->SetInUse(false);
                    }
                    else
                        (*tIter)->AddSpellAuraHolder(holder);
                }
            }
        }
        Aura::Update(diff);
    }
    else                                                    // aura at non-caster
    {
        Unit* caster = GetCaster();
        Unit* target = GetTarget();

        Aura::Update(diff);

        // remove aura if out-of-range from caster (after teleport for example)
        // or caster is isolated or caster no longer has the aura
        // or caster is (no longer) friendly
        bool needFriendly = (m_areaAuraType == AREA_AURA_ENEMY ? false : true);
        if( !caster || caster->hasUnitState(UNIT_STAT_ISOLATED) ||
            !caster->IsWithinDistInMap(target, m_radius)        ||
            !caster->HasAura(GetId(), GetEffIndex())            ||
            caster->IsFriendlyTo(target) != needFriendly
           )
        {
            target->RemoveSingleAuraFromSpellAuraHolder(GetId(), GetEffIndex(), GetCasterGuid());
        }
        else if( m_areaAuraType == AREA_AURA_PARTY)         // check if in same sub group
        {
            // not check group if target == owner or target == pet
            if (caster->GetCharmerOrOwnerGuid() != target->GetObjectGuid() && caster->GetObjectGuid() != target->GetCharmerOrOwnerGuid())
            {
                Player* check = caster->GetCharmerOrOwnerPlayerOrPlayerItself();

                Group *pGroup = check ? check->GetGroup() : NULL;
                if( pGroup )
                {
                    Player* checkTarget = target->GetCharmerOrOwnerPlayerOrPlayerItself();
                    if(!checkTarget || !pGroup->SameSubGroup(check, checkTarget))
                        target->RemoveSingleAuraFromSpellAuraHolder(GetId(), GetEffIndex(), GetCasterGuid());
                }
                else
                    target->RemoveSingleAuraFromSpellAuraHolder(GetId(), GetEffIndex(), GetCasterGuid());
            }
        }
        else if( m_areaAuraType == AREA_AURA_RAID)          // TODO: fix me!
        {
            // not check group if target == owner or target == pet
            if (caster->GetCharmerOrOwnerGuid() != target->GetObjectGuid() && caster->GetObjectGuid() != target->GetCharmerOrOwnerGuid())
            {
                Player* check = caster->GetCharmerOrOwnerPlayerOrPlayerItself();

                Group *pGroup = check ? check->GetGroup() : NULL;
                if( pGroup )
                {
                    Player* checkTarget = target->GetCharmerOrOwnerPlayerOrPlayerItself();
                    if(!checkTarget)
                        target->RemoveSingleAuraFromSpellAuraHolder(GetId(), GetEffIndex(), GetCasterGuid());
                }
                else
                    target->RemoveSingleAuraFromSpellAuraHolder(GetId(), GetEffIndex(), GetCasterGuid());
            }
        }
        else if (m_areaAuraType == AREA_AURA_PET || m_areaAuraType == AREA_AURA_OWNER)
        {
            if (target->GetObjectGuid() != caster->GetCharmerOrOwnerGuid())
                target->RemoveSingleAuraFromSpellAuraHolder(GetId(), GetEffIndex(), GetCasterGuid());
        }
    }
}

void PersistentAreaAura::Update(uint32 diff)
{
    bool remove = false;

    // remove the aura if its caster or the dynamic object causing it was removed
    // or if the target moves too far from the dynamic object
    if(Unit *caster = GetCaster())
    {
        DynamicObject *dynObj = caster->GetDynObject(GetId(), GetEffIndex());
        if (dynObj)
        {
            if (!GetTarget()->IsWithinDistInMap(dynObj, dynObj->GetRadius()))
            {
                remove = true;
                dynObj->RemoveAffected(GetTarget());        // let later reapply if target return to range
            }
        }
        else
            remove = true;
    }
    else
        remove = true;

    Aura::Update(diff);

    if(remove)
        GetTarget()->RemoveAura(GetId(), GetEffIndex());
}

void Aura::ApplyModifier(bool apply, bool Real)
{
    AuraType aura = m_modifier.m_auraname;

    GetHolder()->SetInUse(true);
    SetInUse(true);
    if(aura < TOTAL_AURAS)
        (*this.*AuraHandler [aura])(apply, Real);
    SetInUse(false);
    GetHolder()->SetInUse(false);
}

bool Aura::isAffectedOnSpell(SpellEntry const *spell) const
{
    return spell->IsFitToFamily(GetSpellProto()->GetSpellFamilyName(), GetAuraSpellClassMask());
}

bool Aura::CanProcFrom(SpellEntry const *spell, uint32 /*procFlag*/, uint32 EventProcEx, uint32 procEx, bool active, bool useClassMask) const
{
    // Check EffectClassMask
    ClassFamilyMask const& mask  = GetAuraSpellClassMask();

    // allow proc for modifier auras with charges
    if (IsCastEndProcModifierAura(GetSpellProto(), GetEffIndex(), spell))
    {
        if (GetHolder()->GetAuraCharges() > 0)
        {
            if (procEx != PROC_EX_CAST_END && EventProcEx == PROC_EX_NONE)
                return false;
        }
    }
    else if (EventProcEx == PROC_EX_NONE && procEx == PROC_EX_CAST_END)
        return false;

    // if no class mask defined, or spell_proc_event has SpellFamilyName=0 - allow proc
    if (!useClassMask || !mask)
    {
        if (!(EventProcEx & PROC_EX_EX_TRIGGER_ALWAYS))
        {
            // Check for extra req (if none) and hit/crit
            if (EventProcEx == PROC_EX_NONE)
            {
                // No extra req, so can trigger only for active (damage/healing present) and hit/crit
                return (procEx & (PROC_EX_NORMAL_HIT | PROC_EX_CRITICAL_HIT)) && active || procEx == PROC_EX_CAST_END;
            }
            else // Passive spells hits here only if resist/reflect/immune/evade
            {
                // Passive spells can`t trigger if need hit (exclude cases when procExtra include non-active flags)
                if ((EventProcEx & (PROC_EX_NORMAL_HIT|PROC_EX_CRITICAL_HIT) & procEx) && !active)
                    return false;
            }
        }

        return true;
    }
    else
    {
        // SpellFamilyName check is performed in SpellMgr::IsSpellProcEventCanTriggeredBy and it is done once for whole holder
        // note: SpellFamilyName is not checked if no spell_proc_event is defined
        SpellClassOptionsEntry const* classOpt = spell->GetSpellClassOptions();
        if (!classOpt)
            return false;

        return mask.IsFitToFamilyMask(classOpt->SpellFamilyFlags);
    }
}

void Aura::ReapplyAffectedPassiveAuras( Unit* target, bool owner_mode )
{
    // we need store cast item guids for self casted spells
    // expected that not exist permanent auras from stackable auras from different items
    std::map<uint32, ObjectGuid> affectedSelf;
    std::set<uint32> affectedAuraCaster;

    for(Unit::SpellAuraHolderMap::const_iterator itr = target->GetSpellAuraHolderMap().begin(); itr != target->GetSpellAuraHolderMap().end(); ++itr)
    {
        // permanent passive or permanent area aura
        // passive spells can be affected only by own or owner spell mods)
        if ((itr->second->IsPermanent() && ((owner_mode && itr->second->IsPassive()) || itr->second->IsAreaAura())) &&
            // non deleted and not same aura (any with same spell id)
            !itr->second->IsDeleted() && itr->second->GetId() != GetId() &&
            // and affected by aura
            isAffectedOnSpell(itr->second->GetSpellProto()))
        {
            // only applied by self or aura caster
            if (itr->second->GetCasterGuid() == target->GetObjectGuid())
                affectedSelf[itr->second->GetId()] = itr->second->GetCastItemGuid();
            else if (itr->second->GetCasterGuid() == GetCasterGuid())
                affectedAuraCaster.insert(itr->second->GetId());
        }
    }

    if (!affectedSelf.empty())
    {
        Player* pTarget = target->GetTypeId() == TYPEID_PLAYER ? (Player*)target : NULL;

        for(std::map<uint32, ObjectGuid>::const_iterator map_itr = affectedSelf.begin(); map_itr != affectedSelf.end(); ++map_itr)
        {
            Item* item = pTarget && map_itr->second ? pTarget->GetItemByGuid(map_itr->second) : NULL;
            target->RemoveAurasDueToSpell(map_itr->first);
            target->CastSpell(target, map_itr->first, true, item);
        }
    }

    if (!affectedAuraCaster.empty())
    {
        Unit* caster = GetCaster();
        for(std::set<uint32>::const_iterator set_itr = affectedAuraCaster.begin(); set_itr != affectedAuraCaster.end(); ++set_itr)
        {
            target->RemoveAurasDueToSpell(*set_itr);
            if (caster)
                caster->CastSpell(GetTarget(), *set_itr, true);
        }
    }
}

struct ReapplyAffectedPassiveAurasHelper
{
    explicit ReapplyAffectedPassiveAurasHelper(Aura* _aura) : aura(_aura) {}
    void operator()(Unit* unit) const { aura->ReapplyAffectedPassiveAuras(unit, true); }
    Aura* aura;
};

void Aura::ReapplyAffectedPassiveAuras()
{
    // not reapply spell mods with charges (use original value because processed and at remove)
    if (GetSpellProto()->GetProcCharges())
        return;

    // not reapply some spell mods ops (mostly speedup case)
    switch (m_modifier.m_miscvalue)
    {
        case SPELLMOD_DURATION:
        case SPELLMOD_CHARGES:
        case SPELLMOD_NOT_LOSE_CASTING_TIME:
        case SPELLMOD_CASTING_TIME:
        case SPELLMOD_COOLDOWN:
        case SPELLMOD_COST:
        case SPELLMOD_ACTIVATION_TIME:
        case SPELLMOD_GLOBAL_COOLDOWN:
            return;
    }

    // reapply talents to own passive persistent auras
    ReapplyAffectedPassiveAuras(GetTarget(), true);

    // re-apply talents/passives/area auras applied to pet/totems (it affected by player spellmods)
    GetTarget()->CallForAllControlledUnits(ReapplyAffectedPassiveAurasHelper(this), CONTROLLED_PET|CONTROLLED_TOTEMS);

    // re-apply talents/passives/area auras applied to group members (it affected by player spellmods)
    if (Group* group = ((Player*)GetTarget())->GetGroup())
        for(GroupReference *itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
            if (Player* member = itr->getSource())
                if (member != GetTarget() && member->IsInMap(GetTarget()))
                    ReapplyAffectedPassiveAuras(member, false);
}

/*********************************************************/
/***               BASIC AURA FUNCTION                 ***/
/*********************************************************/

void Aura::HandleAuraInitializeImages( bool Apply, bool Real) //same. All copyright goes to TC2 team
{
    if (!Real || !Apply)
        return;

    Unit* caster = GetCaster();
    if (!caster)
        return;

    Unit* target = GetTarget();

    // Set item visual
    if (caster->GetTypeId()== TYPEID_PLAYER && target->GetOwner() == caster)
    {
        if (Item const * item = ((Player*)caster)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
            target->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, item->GetVisibleEntry());
        if (Item const * item = ((Player*)caster)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
            target->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, item->GetVisibleEntry());
    }
    else
    {
        target->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID, caster->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID));
        target->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, caster->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1));
        target->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2, caster->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2));
    }
}

void Aura::HandleAddModifier(bool apply, bool Real)
{
    Unit* target = GetTarget();
    if (target->GetTypeId() != TYPEID_PLAYER || !Real)
        return;

    if (m_modifier.m_miscvalue >= MAX_SPELLMOD)
        return;

    if (apply)
    {
        SpellEntry const* spellProto = GetSpellProto();

        // Add custom charges for some mod aura
        switch (spellProto->Id)
        {
            case 17941:                                     // Shadow Trance
            case 22008:                                     // Netherwind Focus
            case 34936:                                     // Backlash
            case 47283:                                     // Empowered Imp
            case 51124:                                     // Killing Machine
            case 54741:                                     // Firestarter
            case 57761:                                     // Fireball!
            case 64823:                                     // Elune's Wrath (Balance Druid T8 Set)
            case 63165:                                     // Decimation (Rank 1)
            case 63167:                                     // Decimation (Rank 2)
            case 74434:                                     // Soulburn
            case 81093:                                     // Fury of Stormrage
            case 83359:                                     // Sic 'Em!
            case 89388:                                     // Sic 'Em!
            case 88819:                                     // Daybreak
            case 89485:                                     // Inner Focus
            case 90174:                                     // Divine Purpose
            case 93400:                                     // Shooting Stars
                GetHolder()->SetAuraCharges(1);
                break;
        }

        /*
        // Improved Flametongue Weapon, overwrite wrong data, maybe time re-add table
        else if (spellProto->Id == 37212)
        {
            // Flametongue Weapon (Passive)
            // TODO: drop when override will be possible
            SpellEntry *entry = const_cast<SpellEntry*>(spellProto);
            entry->EffectSpellClassMask[GetEffIndex()].Flags = UI64LIT(0x0000000000200000);
        }*/

        // Chakra
        if (spellProto->Id == 14751)
        {
            const_cast<SpellEffectEntry*>(m_spellEffect)->EffectSpellClassMask.Flags = 0;
            const_cast<SpellEffectEntry*>(m_spellEffect)->EffectSpellClassMask.Flags2 = 0;
        }
    }

    switch (GetId())
    {
        case 59052:     // Freezing Fog
        {
            if (apply)
            {
                // Item - Death Knight T13 DPS 2P Bonus
                if (Aura* aura = target->GetAura(105609, EFFECT_INDEX_0))
                    if (roll_chance_i(aura->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_1)))
                        GetHolder()->SetAuraCharges(2);
            }
            break;
        }
        case 81340:     // Sudden Doom
        {
            if (apply)
            {
                // Item - Death Knight T13 DPS 2P Bonus
                if (Aura* aura = target->GetAura(105609, EFFECT_INDEX_0))
                    if (roll_chance_i(aura->GetModifier()->m_amount))
                        GetHolder()->SetAuraCharges(2);
            }
            break;
        }
        case 81661:     // Evangelism
        case 87118:     // Dark Evangelism
        {
            if (GetEffIndex() == EFFECT_INDEX_0 && !apply)
                target->RemoveAurasByCasterSpell(87154, GetCasterGuid());
            break;
        }
        case 84590:     // Deadly Momentum
        {
            if (apply)
            {
                // Slice and Dice
                if (SpellAuraHolder* holder = target->GetSpellAuraHolder(5171))
                    holder->RefreshHolder();

                // Recuperate
                if (SpellAuraHolder* holder = target->GetSpellAuraHolder(73651))
                    holder->RefreshHolder();
            }
            break;
        }
        case 93400:     // Shooting Stars
        {
            if (apply && target->GetTypeId() == TYPEID_PLAYER)
                ((Player*)target)->RemoveSpellCooldown(78674, true);
            break;
        }
        case 93622:     // Berserk
        {
            if (apply && target->GetTypeId() == TYPEID_PLAYER)
                // remove Mangle (Bear) cooldown
                ((Player*)target)->RemoveSpellCooldown(33878, true);
            break;
        }
        case 96206:     // Nature's Bounty
        {
            if (apply && target->GetTypeId() == TYPEID_PLAYER)
                if (((Player*)target)->m_naturesBountyCounter < 3)
                    target->RemoveAurasDueToSpell(GetId());
            break;
        }
        default:
            break;
    }

    ((Player*)target)->AddSpellMod(this, apply);

    ReapplyAffectedPassiveAuras();
}

void Aura::TriggerSpell()
{
    ObjectGuid casterGUID = GetCasterGuid();
    Unit* triggerTarget = GetTriggerTarget();

    if (!casterGUID || !triggerTarget)
        return;

    // generic casting code with custom spells and target/caster customs
    uint32 trigger_spell_id = m_spellEffect->EffectTriggerSpell;

    SpellEntry const* triggeredSpellInfo = sSpellStore.LookupEntry(trigger_spell_id);
    SpellEntry const* auraSpellInfo = GetSpellProto();
    uint32 auraId = auraSpellInfo->Id;
    Unit* target = GetTarget();
    Unit* triggerCaster = triggerTarget;
    WorldObject* triggerTargetObject = NULL;

    // specific code for cases with no trigger spell provided in field
    if (triggeredSpellInfo == NULL)
    {
        switch(auraSpellInfo->GetSpellFamilyName())
        {
            case SPELLFAMILY_GENERIC:
            {
                switch(auraId)
                {
                    case 812:                               // Periodic Mana Burn
                    {
                        trigger_spell_id = 25779;           // Mana Burn

                        if (GetTarget()->GetTypeId() != TYPEID_UNIT)
                            return;

                        triggerTarget = ((Creature*)GetTarget())->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0, trigger_spell_id, SELECT_FLAG_POWER_MANA);
                        if (!triggerTarget)
                            return;

                        break;
                    }
//                    // Polymorphic Ray
//                    case 6965: break;
                    case 9712:                              // Thaumaturgy Channel
                        trigger_spell_id = 21029;
                        break;
//                    // Egan's Blaster
//                    case 17368: break;
//                    // Haunted
//                    case 18347: break;
//                    // Ranshalla Waiting
//                    case 18953: break;
//                    // Inferno
//                    case 19695: break;
//                    // Frostwolf Muzzle DND
//                    case 21794: break;
//                    // Alterac Ram Collar DND
//                    case 21866: break;
//                    // Celebras Waiting
//                    case 21916: break;
                    case 23170:                             // Brood Affliction: Bronze
                    {
                        target->CastSpell(target, 23171, true, NULL, this);
                        return;
                    }
                    case 23184:                             // Mark of Frost
                    case 25041:                             // Mark of Nature
                    case 37125:                             // Mark of Death
                    {
                        std::list<Player*> targets;

                        // spells existed in 1.x.x; 23183 - mark of frost; 25042 - mark of nature; both had radius of 100.0 yards in 1.x.x DBC
                        // spells are used by Azuregos and the Emerald dragons in order to put a stun debuff on the players which resurrect during the encounter
                        // in order to implement the missing spells we need to make a grid search for hostile players and check their auras; if they are marked apply debuff
                        // spell 37127 used for the Mark of Death, is used server side, so it needs to be implemented here

                        uint32 markSpellId = 0;
                        uint32 debuffSpellId = 0;

                        switch (auraId)
                        {
                            case 23184:
                                markSpellId = 23182;
                                debuffSpellId = 23186;
                                break;
                            case 25041:
                                markSpellId = 25040;
                                debuffSpellId = 25043;
                                break;
                            case 37125:
                                markSpellId = 37128;
                                debuffSpellId = 37131;
                                break;
                        }

                        MaNGOS::AnyPlayerInObjectRangeWithAuraCheck u_check(GetTarget(), 100.0f, markSpellId);
                        MaNGOS::PlayerListSearcher<MaNGOS::AnyPlayerInObjectRangeWithAuraCheck > checker(targets, u_check);
                        Cell::VisitWorldObjects(GetTarget(), checker, 100.0f);

                        for (std::list<Player*>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
                            (*itr)->CastSpell((*itr), debuffSpellId, true, NULL, NULL, casterGUID);

                        return;
                    }
                    case 23493:                             // Restoration
                    {
                        uint32 heal = triggerTarget->GetMaxHealth() / 10;
                        uint32 absorb = 0;
                        triggerTarget->CalculateHealAbsorb(heal, &absorb);
                        triggerTarget->DealHeal(triggerTarget, heal - absorb, auraSpellInfo, false, absorb);

                        if (int32 mana = triggerTarget->GetMaxPower(POWER_MANA))
                        {
                            mana /= 10;
                            triggerTarget->EnergizeBySpell(triggerTarget, 23493, mana, POWER_MANA);
                        }
                        return;
                    }
//                    // Stoneclaw Totem Passive TEST
//                    case 23792: break;
//                    // Axe Flurry
//                    case 24018: break;
                    case 24210:                             // Mark of Arlokk
                    {
                        // Replacement for (classic) spell 24211 (doesn't exist anymore)
                        std::list<Creature*> lList;

                        // Search for all Zulian Prowler in range
                        MaNGOS::AllCreaturesOfEntryInRangeCheck check(triggerTarget, 15101, 15.0f);
                        MaNGOS::CreatureListSearcher<MaNGOS::AllCreaturesOfEntryInRangeCheck> searcher(lList, check);
                        Cell::VisitGridObjects(triggerTarget, searcher, 15.0f);

                        for (std::list<Creature*>::const_iterator itr = lList.begin(); itr != lList.end(); ++itr)
                            if ((*itr)->isAlive())
                                (*itr)->AddThreat(triggerTarget, float(5000));

                        return;
                    }
//                    // Restoration
//                    case 24379: break;
//                    // Happy Pet
//                    case 24716: break;
                    case 24780:                             // Dream Fog
                    {
                        // Note: In 1.12 triggered spell 24781 still exists, need to script dummy effect for this spell then
                        // Select an unfriendly enemy in 100y range and attack it
                        if (target->GetTypeId() != TYPEID_UNIT)
                            return;

                        ThreatList const& tList = target->getThreatManager().getThreatList();
                        for (ThreatList::const_iterator itr = tList.begin();itr != tList.end(); ++itr)
                        {
                            Unit* pUnit = target->GetMap()->GetUnit((*itr)->getUnitGuid());

                            if (pUnit && target->getThreatManager().getThreat(pUnit))
                                target->getThreatManager().modifyThreatPercent(pUnit, -100);
                        }

                        if (Unit* pEnemy = target->SelectRandomUnfriendlyTarget(target->getVictim(), 100.0f))
                            ((Creature*)target)->AI()->AttackStart(pEnemy);

                        return;
                    }
//                    // Cannon Prep
//                    case 24832: break;
                    case 24834:                             // Shadow Bolt Whirl
                    {
                        uint32 spellForTick[8] = { 24820, 24821, 24822, 24823, 24835, 24836, 24837, 24838 };
                        uint32 tick = GetAuraTicks();
                        if(tick < 8)
                        {
                            trigger_spell_id = spellForTick[tick];

                            // casted in left/right (but triggered spell have wide forward cone)
                            float forward = target->GetOrientation();
                            float angle = target->GetOrientation() + ( tick % 2 == 0 ? M_PI_F / 2 : - M_PI_F / 2);
                            target->SetOrientation(angle);
                            triggerTarget->CastSpell(triggerTarget, trigger_spell_id, true, NULL, this, casterGUID);
                            target->SetOrientation(forward);
                        }
                        return;
                    }
//                    // Stink Trap
//                    case 24918: break;
//                    // Agro Drones
//                    case 25152: break;
                    case 25371:                             // Consume
                    {
                        int32 bpDamage = triggerTarget->GetMaxHealth()*10/100;
                        triggerTarget->CastCustomSpell(triggerTarget, 25373, &bpDamage, NULL, NULL, true, NULL, this, casterGUID);
                        return;
                    }
//                    // Pain Spike
//                    case 25572: break;
                    case 26009:                             // Rotate 360
                    case 26136:                             // Rotate -360
                    {
                        float newAngle = target->GetOrientation();

                        if (auraId == 26009)
                            newAngle += M_PI_F / 40;
                        else
                            newAngle -= M_PI_F / 40;

                        newAngle = NormalizeOrientation(newAngle);

                        target->SetFacingTo(newAngle);

                        target->CastSpell(target, 26029, true);
                        return;
                    }
//                    // Consume
//                    case 26196: break;
//                    // Berserk
//                    case 26615: break;
//                    // Defile
//                    case 27177: break;
//                    // Teleport: IF/UC
//                    case 27601: break;
//                    // Five Fat Finger Exploding Heart Technique
//                    case 27673: break;
//                    // Nitrous Boost
//                    case 27746: break;
//                    // Steam Tank Passive
//                    case 27747: break;
                    case 27808:                             // Frost Blast
                    {
                        int32 bpDamage = triggerTarget->GetMaxHealth()*26/100;
                        triggerTarget->CastCustomSpell(triggerTarget, 29879, &bpDamage, NULL, NULL, true, NULL, this, casterGUID);
                        return;
                    }
                    // Detonate Mana
                    case 27819:
                    {
                        // 33% Mana Burn on normal mode, 50% on heroic mode
                        int32 bpDamage = (int32)triggerTarget->GetPower(POWER_MANA) / (triggerTarget->GetMap()->GetDifficulty() ? 2 : 3);
                        triggerTarget->ModifyPower(POWER_MANA, -bpDamage);
                        triggerTarget->CastCustomSpell(triggerTarget, 27820, &bpDamage, NULL, NULL, true, NULL, this, triggerTarget->GetObjectGuid());
                        return;
                    }
//                    // Controller Timer
//                    case 28095: break;
                    // Stalagg Chain and Feugen Chain
                    case 28096:
                    case 28111:
                    {
                        // X-Chain is casted by Tesla to X, so: caster == Tesla, target = X
                        Unit* pCaster = GetCaster();
                        if (pCaster && pCaster->GetTypeId() == TYPEID_UNIT && !pCaster->IsWithinDistInMap(target, 60.0f))
                        {
                            pCaster->InterruptNonMeleeSpells(true);
                            ((Creature*)pCaster)->SetInCombatWithZone();
                            // Stalagg Tesla Passive or Feugen Tesla Passive
                            pCaster->CastSpell(pCaster, auraId == 28096 ? 28097 : 28109, true, NULL, NULL, target->GetObjectGuid());
                        }
                        return;
                    }
                    // Stalagg Tesla Passive and Feugen Tesla Passive
                    case 28097:
                    case 28109:
                    {
                        // X-Tesla-Passive is casted by Tesla on Tesla with original caster X, so: caster = X, target = Tesla
                        Unit* pCaster = GetCaster();
                        if (pCaster && pCaster->GetTypeId() == TYPEID_UNIT)
                        {
                            if (pCaster->getVictim() && !pCaster->IsWithinDistInMap(target, 60.0f))
                            {
                                if (Unit* pTarget = ((Creature*)pCaster)->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                                    target->CastSpell(pTarget, 28099, false);// Shock
                            }
                            else
                            {
                                // "Evade"
                                target->RemoveAurasDueToSpell(auraId);
                                target->DeleteThreatList();
                                target->CombatStop(true);
                                // Recast chain (Stalagg Chain or Feugen Chain
                                target->CastSpell(pCaster, auraId == 28097 ? 28096 : 28111, false);
                            }
                        }
                        return;
                    }
//                    // Mark of Didier
//                    case 28114: break;
//                    // Communique Timer, camp
//                    case 28346: break;
                    case 28522:                             // Icebolt (Sapphiron - Naxxramas)
                        // dunno if triggered spell id is correct
                        if (!target->HasAura(45776))
                            trigger_spell_id = 45776;
                        break;
//                    // Silithyst
//                    case 29519: break;
                    case 29528:                             // Inoculate Nestlewood Owlkin
                        // prevent error reports in case ignored player target
                        if (triggerTarget->GetTypeId() != TYPEID_UNIT)
                            return;
                        break;
//                    // Overload
//                    case 29768: break;
//                    // Return Fire
//                    case 29788: break;
//                    // Return Fire
//                    case 29793: break;
//                    // Return Fire
//                    case 29794: break;
//                    // Guardian of Icecrown Passive
//                    case 29897: break;
                    case 29917:                             // Feed Captured Animal
                        trigger_spell_id = 29916;
                        break;
//                    // Flame Wreath
//                    case 29946: break;
//                    // Flame Wreath
//                    case 29947: break;
//                    // Mind Exhaustion Passive
//                    case 30025: break;
//                    // Nether Beam - Serenity
//                    case 30401: break;
                    case 30427:                             // Extract Gas
                    {
                        Unit* caster = GetCaster();
                        if (!caster)
                            return;
                        // move loot to player inventory and despawn target
                        if (caster->GetTypeId() ==TYPEID_PLAYER &&
                           triggerTarget->GetTypeId() == TYPEID_UNIT &&
                           ((Creature*)triggerTarget)->GetCreatureInfo()->type == CREATURE_TYPE_GAS_CLOUD)
                        {
                            Player* player = (Player*)caster;
                            Creature* creature = (Creature*)triggerTarget;
                            // missing lootid has been reported on startup - just return
                            if (!creature->GetCreatureInfo()->SkinLootId)
                                return;

                            player->AutoStoreLoot(creature, creature->GetCreatureInfo()->SkinLootId, LootTemplates_Skinning, true);

                            creature->ForcedDespawn();
                        }
                        return;
                    }
                    case 30576:                             // Quake
                        trigger_spell_id = 30571;
                        break;
//                    // Burning Maul
//                    case 30598: break;
//                    // Regeneration
//                    case 30799:
//                    case 30800:
//                    case 30801:
//                        break;
//                    // Despawn Self - Smoke cloud
//                    case 31269: break;
//                    // Time Rift Periodic
//                    case 31320: break;
//                    // Corrupt Medivh
//                    case 31326: break;
                    case 31347:                             // Doom
                    {
                        target->CastSpell(target,31350,true);
                        target->DealDamage(target, target->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                        return;
                    }
                    case 31373:                             // Spellcloth
                    {
                        // Summon Elemental after create item
                        triggerTarget->SummonCreature(17870, 0.0f, 0.0f, 0.0f, triggerTarget->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 0);
                        return;
                    }
//                    // Bloodmyst Tesla
//                    case 31611: break;
                    case 31944:                             // Doomfire
                    {
                        int32 damage = m_modifier.m_amount * ((GetAuraDuration() + m_modifier.periodictime) / GetAuraMaxDuration());
                        triggerTarget->CastCustomSpell(triggerTarget, 31969, &damage, NULL, NULL, true, NULL, this, casterGUID);
                        return;
                    }
//                    // Teleport Test
//                    case 32236: break;
//                    // Earthquake
//                    case 32686: break;
//                    // Possess
//                    case 33401: break;
//                    // Draw Shadows
//                    case 33563: break;
//                    // Murmur's Touch
//                    case 33711: break;
                    case 34229:                             // Flame Quills
                    {
                        // cast 24 spells 34269-34289, 34314-34316
                        for(uint32 spell_id = 34269; spell_id != 34290; ++spell_id)
                            triggerTarget->CastSpell(triggerTarget, spell_id, true, NULL, this, casterGUID);
                        for(uint32 spell_id = 34314; spell_id != 34317; ++spell_id)
                            triggerTarget->CastSpell(triggerTarget, spell_id, true, NULL, this, casterGUID);
                        return;
                    }
//                    // Gravity Lapse
//                    case 34480: break;
//                    // Tornado
//                    case 34683: break;
//                    // Frostbite Rotate
//                    case 34748: break;
//                    // Arcane Flurry
//                    case 34821: break;
//                    // Interrupt Shutdown
//                    case 35016: break;
//                    // Interrupt Shutdown
//                    case 35176: break;
//                    // Inferno
//                    case 35268: break;
//                    // Salaadin's Tesla
//                    case 35515: break;
//                    // Ethereal Channel (Red)
//                    case 35518: break;
//                    // Nether Vapor
//                    case 35879: break;
//                    // Dark Portal Storm
//                    case 36018: break;
//                    // Burning Maul
//                    case 36056: break;
//                    // Living Grove Defender Lifespan
//                    case 36061: break;
//                    // Professor Dabiri Talks
//                    case 36064: break;
//                    // Kael Gaining Power
//                    case 36091: break;
//                    // They Must Burn Bomb Aura
//                    case 36344: break;
//                    // They Must Burn Bomb Aura (self)
//                    case 36350: break;
//                    // Stolen Ravenous Ravager Egg
//                    case 36401: break;
//                    // Activated Cannon
//                    case 36410: break;
//                    // Stolen Ravenous Ravager Egg
//                    case 36418: break;
//                    // Enchanted Weapons
//                    case 36510: break;
//                    // Cursed Scarab Periodic
//                    case 36556: break;
//                    // Cursed Scarab Despawn Periodic
//                    case 36561: break;
                    case 36573:                             // Vision Guide
                    {
                        if (GetAuraTicks() == 10 && target->GetTypeId() == TYPEID_PLAYER)
                        {
                            ((Player*)target)->AreaExploredOrEventHappens(10525);
                            target->RemoveAurasDueToSpell(36573);
                        }

                        return;
                    }
//                    // Cannon Charging (platform)
//                    case 36785: break;
//                    // Cannon Charging (self)
//                    case 36860: break;
                    case 37027:                             // Remote Toy
                        trigger_spell_id = 37029;
                        break;
//                    // Mark of Death
//                    case 37125: break;
//                    // Arcane Flurry
//                    case 37268: break;
                    case 37429:                             // Spout (left)
                    case 37430:                             // Spout (right)
                    {
                        float newAngle = target->GetOrientation();

                        if (auraId == 37429)
                            newAngle += 2 * M_PI_F / 100;
                        else
                            newAngle -= 2 * M_PI_F / 100;

                        newAngle = NormalizeOrientation(newAngle);

                        target->SetFacingTo(newAngle);

                        target->CastSpell(target, 37433, true);
                        return;
                    }
//                    // Karazhan - Chess NPC AI, Snapshot timer
//                    case 37440: break;
//                    // Karazhan - Chess NPC AI, action timer
//                    case 37504: break;
//                    // Karazhan - Chess: Is Square OCCUPIED aura (DND)
//                    case 39400: break;
//                    // Banish
//                    case 37546: break;
//                    // Shriveling Gaze
//                    case 37589: break;
//                    // Fake Aggro Radius (2 yd)
//                    case 37815: break;
//                    // Corrupt Medivh
//                    case 37853: break;
                    case 38495:                             // Eye of Grillok
                    {
                        target->CastSpell(target, 38530, true);
                        return;
                    }
                    case 38554:                             // Absorb Eye of Grillok (Zezzak's Shard)
                    {
                        if (target->GetTypeId() != TYPEID_UNIT)
                            return;

                        if (Unit* caster = GetCaster())
                            caster->CastSpell(caster, 38495, true, NULL, this);
                        else
                            return;

                        Creature* creatureTarget = (Creature*)target;

                        creatureTarget->ForcedDespawn();
                        return;
                    }
//                    // Magic Sucker Device timer
//                    case 38672: break;
//                    // Tomb Guarding Charging
//                    case 38751: break;
//                    // Murmur's Touch
//                    case 38794: break;
                    case 39105:                             // Activate Nether-wraith Beacon (31742 Nether-wraith Beacon item)
                    {
                        float fX, fY, fZ;
                        triggerTarget->GetClosePoint(fX, fY, fZ, triggerTarget->GetObjectBoundingRadius(), 20.0f);
                        triggerTarget->SummonCreature(22408, fX, fY, fZ, triggerTarget->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 0);
                        return;
                    }
//                    // Drain World Tree Visual
//                    case 39140: break;
//                    // Quest - Dustin's Undead Dragon Visual aura
//                    case 39259: break;
//                    // Hellfire - The Exorcism, Jules releases darkness, aura
//                    case 39306: break;
//                    // Inferno
//                    case 39346: break;
//                    // Enchanted Weapons
//                    case 39489: break;
//                    // Shadow Bolt Whirl
//                    case 39630: break;
//                    // Shadow Bolt Whirl
//                    case 39634: break;
//                    // Shadow Inferno
//                    case 39645: break;
                    case 39857:                             // Tear of Azzinoth Summon Channel - it's not really supposed to do anything,and this only prevents the console spam
                        trigger_spell_id = 39856;
                        break;
//                    // Soulgrinder Ritual Visual (Smashed)
//                    case 39974: break;
//                    // Simon Game Pre-game timer
//                    case 40041: break;
//                    // Knockdown Fel Cannon: The Aggro Check Aura
//                    case 40113: break;
//                    // Spirit Lance
//                    case 40157: break;
                    case 40398:                             // Demon Transform 2
                        switch (GetAuraTicks())
                        {
                            case 1:
                                if (target->HasAura(40506))
                                    target->RemoveAurasDueToSpell(40506);
                                else
                                    trigger_spell_id = 40506;
                                break;
                            case 2:
                                trigger_spell_id = 40510;
                                break;
                        }
                        break;
                    case 40511:                             // Demon Transform 1
                        trigger_spell_id = 40398;
                        break;
//                    // Ancient Flames
//                    case 40657: break;
//                    // Ethereal Ring Cannon: Cannon Aura
//                    case 40734: break;
//                    // Cage Trap
//                    case 40760: break;
//                    // Random Periodic
//                    case 40867: break;
//                    // Prismatic Shield
//                    case 40879: break;
//                    // Aura of Desire
//                    case 41350: break;
//                    // Dementia
//                    case 41404: break;
//                    // Chaos Form
//                    case 41629: break;
//                    // Alert Drums
//                    case 42177: break;
//                    // Spout
//                    case 42581: break;
//                    // Spout
//                    case 42582: break;
//                    // Return to the Spirit Realm
//                    case 44035: break;
//                    // Curse of Boundless Agony
//                    case 45050: break;
//                    // Earthquake
//                    case 46240: break;
                    case 46736:                             // Personalized Weather
                        trigger_spell_id = 46737;
                        break;
                    case 46924:                             // Bladestorm
                    {
                        if (target->GetTypeId() != TYPEID_PLAYER)
                            break;

                        if (!((Player*)target)->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
                        {
                            target->RemoveAurasDueToSpell(46924);
                            return;
                        }
                        break;
                    }
//                    // Stay Submerged
//                    case 46981: break;
//                    // Dragonblight Ram
//                    case 47015: break;
//                    // Party G.R.E.N.A.D.E.
//                    case 51510: break;
//                    // Horseman Abilities
//                    case 52347: break;
//                    // GPS (Greater drake Positioning System)
//                    case 53389: break;
//                    // WotLK Prologue Frozen Shade Summon Aura
//                    case 53459: break;
//                    // WotLK Prologue Frozen Shade Speech
//                    case 53460: break;
//                    // WotLK Prologue Dual-plagued Brain Summon Aura
//                    case 54295: break;
//                    // WotLK Prologue Dual-plagued Brain Speech
//                    case 54299: break;
//                    // Rotate 360 (Fast)
//                    case 55861: break;
//                    // Shadow Sickle
//                    case 56702: break;
//                    // Portal Periodic
//                    case 58008: break;
//                    // Destroy Door Seal
//                    case 58040: break;
//                    // Draw Magic
//                    case 58185: break;
                    case 58886:                             // Food
                    {
                        if (GetAuraTicks() != 1)
                            return;

                        uint32 randomBuff[5] = {57288, 57139, 57111, 57286, 57291};

                        trigger_spell_id = urand(0, 1) ? 58891 : randomBuff[urand(0, 4)];

                        break;
                    }
//                    // Shadow Sickle
//                    case 59103: break;
//                    // Time Bomb
//                    case 59376: break;
//                    // Whirlwind Visual
//                    case 59551: break;
//                    // Hearstrike
//                    case 59783: break;
//                    // Z Check
//                    case 61678: break;
//                    // isDead Check
                    case 61719:                             // Easter Lay Noblegarden Egg Aura
                    {
                        if (triggerTarget->GetTypeId() != TYPEID_PLAYER)
                            return;

                        // reset tick timer
                        if (((Player*)triggerTarget)->isMoving())
                        {
                            m_periodicTimer = m_modifier.periodictime;
                            return;
                        }
                        break;
                    }
//                    case 61976: break;
//                    // Start the Engine
//                    case 62432: break;
//                    // Enchanted Broom
//                    case 62571: break;
//                    // Mulgore Hatchling
//                    case 62586: break;
                    case 62679:                             // Durotar Scorpion
                        trigger_spell_id = auraSpellInfo->CalculateSimpleValue(m_effIndex);
                        break;
//                    // Fighting Fish
//                    case 62833: break;
//                    // Shield Level 1
//                    case 63130: break;
//                    // Shield Level 2
//                    case 63131: break;
//                    // Shield Level 3
//                    case 63132: break;
//                    // Food
//                    case 64345: break;
//                    // Remove Player from Phase
//                    case 64445: break;
//                    // Food
//                    case 65418: break;
//                    // Food
//                    case 65419: break;
//                    // Food
//                    case 65420: break;
//                    // Food
//                    case 65421: break;
//                    // Food
//                    case 65422: break;
//                    // Rolling Throw
//                    case 67546: break;
                    case 70017:                             // Gunship Cannon Fire
                        trigger_spell_id = 70021;
                        break;
//                    // Ice Tomb
//                    case 70157: break;
//                    // Mana Barrier
//                    case 70842: break;
//                    // Summon Timer: Suppresser
//                    case 70912: break;
//                    // Aura of Darkness
//                    case 71110: break;
//                    // Aura of Darkness
//                    case 71111: break;
//                    // Ball of Flames Visual
//                    case 71706: break;
//                    // Summon Broken Frostmourne
//                    case 74081: break;
                    case 83676:                             // Resistance is Futile
                    {
                        Unit* caster = GetCaster();
                        if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
                            return;

                        if (!target->isMoving())
                            return;

                        if (SpellEntry const* talent = ((Player*)caster)->GetKnownTalentRankById(9420))
                            if (roll_chance_i(talent->CalculateSimpleValue(EFFECT_INDEX_0)))
                                caster->CastSpell(caster, 82897, true);
                        return;
                    }
                    case 87604:                             // Food
                    {
                        uint32 spells[11] = { 87545, 87546, 87547, 87548, 87549, 87550, 87551, 87552, 87554, 87555, 87635 };
                        target->CastSpell(target, spells[urand(0, 10)], true);
                        return;
                    }
                    default:
                        break;
                }
                break;
            }
            case SPELLFAMILY_MAGE:
            {
                switch(auraId)
                {
                    case 66:                                // Invisibility
                        // Here need periodic trigger reducing threat spell (or do it manually)
                        return;
                    /// Adonai, bug #34, spell 1
                    case 82676:                             // Ring of Frost
                    {
                        if (Unit* caster = GetCaster())
                        {
                            if(caster->GetTypeId() == TYPEID_PLAYER)
                            {
                                if (Unit* ring = ((Player *)GetCaster())->GetSummonUnit(auraId))
                                {
                                    trigger_spell_id = 82691;
                                    triggerTarget = ring;
                                }
                            }
                        }
                        break;
                    }
                    default:
                        break;
                }
                break;
            }

//            case SPELLFAMILY_WARRIOR:
//            {
//                switch(auraId)
//                {
//                    // Wild Magic
//                    case 23410: break;
//                    // Corrupted Totems
//                    case 23425: break;
//                    default:
//                        break;
//                }
//                break;
//            }
//            case SPELLFAMILY_PRIEST:
//            {
//                switch (auraId)
//                {
//                    // Blue Beam
//                    case 32930: break;
//                    // Fury of the Dreghood Elders
//                    case 35460: break;
//                    default:
//                        break;
//                }
//                break;
//            }
            case SPELLFAMILY_HUNTER:
            {
                switch (auraId)
                {
                    case 53302:                             // Sniper training
                    case 53303:
                    case 53304:
                        if (triggerTarget->GetTypeId() != TYPEID_PLAYER)
                            return;

                        // Reset reapply counter at move
                        if (((Player*)triggerTarget)->isMoving())
                        {
                            m_modifier.m_amount = 6;
                            return;
                        }

                        // We are standing at the moment
                        if (m_modifier.m_amount > 0)
                        {
                            --m_modifier.m_amount;
                            return;
                        }

                        // select rank of buff
                        switch(auraId)
                        {
                            case 53302: trigger_spell_id = 64418; break;
                            case 53303: trigger_spell_id = 64419; break;
                            case 53304: trigger_spell_id = 64420; break;
                        }

                        // If aura is active - no need to continue
                        if (triggerTarget->HasAura(trigger_spell_id))
                            return;

                        break;
                    default:
                        break;
                }
                break;
            }
            case SPELLFAMILY_DRUID:
            {
                switch(auraId)
                {
                    case 768:                               // Cat Form
                        // trigger_spell_id not set and unknown effect triggered in this case, ignoring for while
                        return;
                    default:
                        break;
                }
                break;
            }

//            case SPELLFAMILY_HUNTER:
//            {
//                switch(auraId)
//                {
//                    //Frost Trap Aura
//                    case 13810:
//                        return;
//                    //Rizzle's Frost Trap
//                    case 39900:
//                        return;
//                    // Tame spells
//                    case 19597:         // Tame Ice Claw Bear
//                    case 19676:         // Tame Snow Leopard
//                    case 19677:         // Tame Large Crag Boar
//                    case 19678:         // Tame Adult Plainstrider
//                    case 19679:         // Tame Prairie Stalker
//                    case 19680:         // Tame Swoop
//                    case 19681:         // Tame Dire Mottled Boar
//                    case 19682:         // Tame Surf Crawler
//                    case 19683:         // Tame Armored Scorpid
//                    case 19684:         // Tame Webwood Lurker
//                    case 19685:         // Tame Nightsaber Stalker
//                    case 19686:         // Tame Strigid Screecher
//                    case 30100:         // Tame Crazed Dragonhawk
//                    case 30103:         // Tame Elder Springpaw
//                    case 30104:         // Tame Mistbat
//                    case 30647:         // Tame Barbed Crawler
//                    case 30648:         // Tame Greater Timberstrider
//                    case 30652:         // Tame Nightstalker
//                        return;
//                    default:
//                        break;
//                }
//                break;
//            }
            case SPELLFAMILY_SHAMAN:
            {
                switch(auraId)
                {
                    case 28820:                             // Lightning Shield (The Earthshatterer set trigger after cast Lighting Shield)
                    {
                        // Need remove self if Lightning Shield not active
                        Unit::SpellAuraHolderMap const& auras = triggerTarget->GetSpellAuraHolderMap();
                        for(Unit::SpellAuraHolderMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
                        {
                            SpellEntry const* spell = itr->second->GetSpellProto();
                            if (spell->IsFitToFamily(SPELLFAMILY_SHAMAN, UI64LIT(0x0000000000000400)))
                                return;
                        }
                        triggerTarget->RemoveAurasDueToSpell(28820);
                        return;
                    }
                    case 38443:                             // Totemic Mastery (Skyshatter Regalia (Shaman Tier 6) - bonus)
                    {
                        if (triggerTarget->IsAllTotemSlotsUsed())
                            triggerTarget->CastSpell(triggerTarget, 38437, true, NULL, this);
                        else
                            triggerTarget->RemoveAurasDueToSpell(38437);
                        return;
                    }
                    default:
                        break;
                }
                break;
            }
            default:
                break;
        }

        // Reget trigger spell proto
        triggeredSpellInfo = sSpellStore.LookupEntry(trigger_spell_id);
    }
    else                                                    // initial triggeredSpellInfo != NULL
    {
        SpellEffectEntry const* spellEffect = GetSpellProto()->GetSpellEffect(GetEffIndex());

        // for channeled spell cast applied from aura owner to channel target (persistent aura affects already applied to true target)
        // come periodic casts applied to targets, so need seelct proper caster (ex. 15790)
        if (IsChanneledSpell(GetSpellProto()) && (spellEffect && spellEffect->Effect != SPELL_EFFECT_PERSISTENT_AREA_AURA))
        {
            // interesting 2 cases: periodic aura at caster of channeled spell
            if (target->GetObjectGuid() == casterGUID)
            {
                triggerCaster = target;

                if (WorldObject* channelTarget = target->GetMap()->GetWorldObject(target->GetChannelObjectGuid()))
                {
                    if (channelTarget->isType(TYPEMASK_UNIT))
                        triggerTarget = (Unit*)channelTarget;
                    else
                        triggerTargetObject = channelTarget;
                }
            }
            // or periodic aura at caster channel target
            else if (Unit* caster = GetCaster())
            {
                if (target->GetObjectGuid() == caster->GetChannelObjectGuid())
                {
                    triggerCaster = caster;
                    triggerTarget = target;
                }
            }
        }

        // Spell exist but require custom code
        switch(auraId)
        {
            case 9347:                                      // Mortal Strike
            {
                if (target->GetTypeId() != TYPEID_UNIT)
                    return;
                // expected selection current fight target
                triggerTarget = ((Creature*)target)->SelectAttackingTarget(ATTACKING_TARGET_TOPAGGRO, 0, triggeredSpellInfo);
                if (!triggerTarget)
                    return;

                break;
            }
            case 1010:                                      // Curse of Idiocy
            {
                // TODO: spell casted by result in correct way mostly
                // BUT:
                // 1) target show casting at each triggered cast: target don't must show casting animation for any triggered spell
                //      but must show affect apply like item casting
                // 2) maybe aura must be replace by new with accumulative stat mods instead stacking

                // prevent cast by triggered auras
                if (casterGUID == triggerTarget->GetObjectGuid())
                    return;

                // stop triggering after each affected stats lost > 90
                int32 intelectLoss = 0;
                int32 spiritLoss = 0;

                Unit::AuraList const& mModStat = triggerTarget->GetAurasByType(SPELL_AURA_MOD_STAT);
                for(Unit::AuraList::const_iterator i = mModStat.begin(); i != mModStat.end(); ++i)
                {
                    if ((*i)->GetId() == 1010)
                    {
                        switch((*i)->GetModifier()->m_miscvalue)
                        {
                            case STAT_INTELLECT: intelectLoss += (*i)->GetModifier()->m_amount; break;
                            case STAT_SPIRIT:    spiritLoss   += (*i)->GetModifier()->m_amount; break;
                            default: break;
                        }
                    }
                }

                if (intelectLoss <= -90 && spiritLoss <= -90)
                    return;

                break;
            }
            // Earthen Power (from Earthbind Totem Passive)
            case 6474:
            {
                Unit *owner = target->GetOwner();

                if (!owner)
                    break;

                Unit::AuraList const& dummyAuras = owner->GetAurasByType(SPELL_AURA_DUMMY);
                for(Unit::AuraList::const_iterator itr = dummyAuras.begin(); itr != dummyAuras.end(); ++itr)
                {
                    if ((*itr)->GetSpellProto()->SpellIconID == 2289 && (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_SHAMAN)
                    {
                        if (!roll_chance_i((*itr)->GetModifier()->m_amount))
                            break;

                        target->CastSpell(target, 59566, true, NULL, this);
                        break;
                    }
                }
                break;
            }
            case 28084:                                     // Negative Charge
            {
                if (triggerTarget->HasAura(29660))
                    triggerTarget->RemoveAurasDueToSpell(29660);
                break;
            }
            case 28059:                                     // Positive Charge
            {
                if (triggerTarget->HasAura(29659))
                    triggerTarget->RemoveAurasDueToSpell(29659);
                break;
            }
            case 33525:                                     // Ground Slam
                triggerTarget->CastSpell(triggerTarget, trigger_spell_id, true, NULL, this, casterGUID);
                return;
            case 38280:                                     // Static Charge (Lady Vashj in Serpentshrine Cavern)
            case 53563:                                     // Beacon of Light
            case 52658:                                     // Static Overload (normal&heroic) (Ionar in Halls of Lightning)
            case 59795:
            case 63018:                                     // Searing Light (normal&heroic) (XT-002 in Ulduar)
            case 65121:
            case 63024:                                     // Gravity Bomb (normal&heroic) (XT-002 in Ulduar)
            case 64234:
                // original caster must be target
                target->CastSpell(target, trigger_spell_id, true, NULL, this, target->GetObjectGuid());
                return;
            case 38736:                                     // Rod of Purification - for quest 10839 (Veil Skith: Darkstone of Terokk)
            {
                if (Unit* caster = GetCaster())
                    caster->CastSpell(triggerTarget, trigger_spell_id, true, NULL, this);
                return;
            }
            case 44883:                                     // Encapsulate
            {
                // Self cast spell, hence overwrite caster (only channeled spell where the triggered spell deals dmg to SELF)
                triggerCaster = triggerTarget;
                break;
            }
            case 48094:                                     // Intense Cold
                triggerTarget->CastSpell(triggerTarget, trigger_spell_id, true, NULL, this);
                return;
            case 56654:                                     // Rapid Recuperation (triggered energize have baspioints == 0)
            case 58882:
            {
                int32 mana = target->GetMaxPower(POWER_MANA) * m_modifier.m_amount / 100;
                triggerTarget->CastCustomSpell(triggerTarget, trigger_spell_id, &mana, NULL, NULL, true, NULL, this);
                return;
            }
            case 58678:                                     // Rock Shards (Vault of Archavon, Archavon)
            {
                if (GetAuraTicks() != 1 && GetAuraTicks()%7)
                    return;
                break;
            }
            case 80326:                                     // Camouflage
            {
                if (target->isMoving())
                    return;
                break;
            }
            // Power Word: Barrier
            case 81781:
            {
                if (GetEffIndex() ==  EFFECT_INDEX_1)
                {
                    if (Unit* caster = GetCaster())
                        if (Unit* owner = caster->GetOwner())
                            // Glyph of Power Word: Barrier
                            if (!owner->HasAura(55689))
                                return;
                }
                break;
            }
            // Siege Cannon
            case 85167:
            {
                VehicleKit* vehicle = target->GetVehicle();
                if (!vehicle)
                    return;

                // Deploy Siege Engine
                if (!vehicle->GetBase()->HasAura(84974))
                    return;

                break;
            }
            // Molting
            case 99464:
            case 99504:
            case 100698:
            case 100699:
            {
                target->SummonCreature(53089,target->GetPositionX()-urand(5,30),target->GetPositionY()-urand(-5,-30),56.500f, target->GetOrientation(), TEMPSUMMON_MANUAL_DESPAWN, 0);
                return;
            }
            case 105784:                            // Blade Dance
            {
                if (target->HasAura(trigger_spell_id))
                    return;
                break;
            }
            case 107837:                            // Throw Totem
            {
                // Throw Totem
                if (target->HasAura(101601))
                    return;
                break;
            }
            default:
                break;
        }
    }

    // All ok cast by default case
    if (triggeredSpellInfo)
    {
        if (triggerTargetObject)
            triggerCaster->CastSpell(triggerTargetObject->GetPositionX(), triggerTargetObject->GetPositionY(), triggerTargetObject->GetPositionZ(),
                triggeredSpellInfo, true, NULL, this, casterGUID);
        else
            triggerCaster->CastSpell(triggerTarget, triggeredSpellInfo, true, NULL, this, casterGUID);
    }
    else
    {
        if (Unit* caster = GetCaster())
        {
            if (triggerTarget->GetTypeId() != TYPEID_UNIT || !sScriptMgr.OnEffectDummy(caster, GetId(), GetEffIndex(), (Creature*)triggerTarget))
                DEBUG_LOG("Aura::TriggerSpell: Spell %u have 0 in EffectTriggered[%d], not handled custom case?",GetId(),GetEffIndex());
        }
    }
}

void Aura::TriggerSpellWithValue()
{
    ObjectGuid casterGuid = GetCasterGuid();
    Unit* target = GetTriggerTarget();

    if (!casterGuid || !target)
        return;

    // Hand of Salvation
    if (GetId() == 1038)
        if (Unit* caster = GetCaster())
            // Glyph of Salvation
            if (caster->HasAura(63225))
                return;

    // generic casting code with custom spells and target/caster customs
    uint32 trigger_spell_id = m_spellEffect->EffectTriggerSpell;
    int32  basepoints0 = GetModifier()->m_amount;

    target->CastCustomSpell(target, trigger_spell_id, &basepoints0, NULL, NULL, true, NULL, this, casterGuid);
}

/*********************************************************/
/***                  AURA EFFECTS                     ***/
/*********************************************************/

void Aura::HandleAuraDummy(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if (!Real)
        return;

    Unit *target = GetTarget();

    SpellClassOptionsEntry const* classOptions = GetSpellProto()->GetSpellClassOptions();

    // AT APPLY
    if (apply)
    {
        switch(GetSpellProto()->GetSpellFamilyName())
        {
            case SPELLFAMILY_GENERIC:
            {
                switch(GetId())
                {
                    case 1515:                              // Tame beast
                        // FIX_ME: this is 2.0.12 threat effect replaced in 2.1.x by dummy aura, must be checked for correctness
                        if (target->CanHaveThreatList())
                            if (Unit* caster = GetCaster())
                                target->AddThreat(caster, 10.0f, false, GetSpellSchoolMask(GetSpellProto()), GetSpellProto());
                        return;
                    case 7057:                              // Haunting Spirits
                        // expected to tick with 30 sec period (tick part see in Aura::PeriodicTick)
                        m_isPeriodic = true;
                        m_modifier.periodictime = 30*IN_MILLISECONDS;
                        m_periodicTimer = m_modifier.periodictime;
                        return;
                    case 10255:                             // Stoned
                    {
                        if (Unit* caster = GetCaster())
                        {
                            if (caster->GetTypeId() != TYPEID_UNIT)
                                return;

                            caster->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                            caster->addUnitState(UNIT_STAT_ROOT);
                        }
                        return;
                    }
                    case 13139:                             // net-o-matic
                        // root to self part of (root_target->charge->root_self sequence
                        if (Unit* caster = GetCaster())
                            caster->CastSpell(caster, 13138, true, NULL, this);
                        return;
                    case 28832:                             // Mark of Korth'azz
                    case 28833:                             // Mark of Blaumeux
                    case 28834:                             // Mark of Rivendare
                    case 28835:                             // Mark of Zeliek
                    {
                        int32 damage = 0;

                        switch (GetStackAmount())
                        {
                            case 1:
                                return;
                            case 2: damage =   500; break;
                            case 3: damage =  1500; break;
                            case 4: damage =  4000; break;
                            case 5: damage = 12500; break;
                            default:
                                damage = 14000 + 1000 * GetStackAmount();
                                break;
                        }

                        if (Unit* caster = GetCaster())
                            caster->CastCustomSpell(target, 28836, &damage, NULL, NULL, true, NULL, this);
                        return;
                    }
                    case 31606:                             // Stormcrow Amulet
                    {
                        CreatureInfo const * cInfo = ObjectMgr::GetCreatureTemplate(17970);

                        // we must assume db or script set display id to native at ending flight (if not, target is stuck with this model)
                        if (cInfo)
                            target->SetDisplayId(Creature::ChooseDisplayId(cInfo));

                        return;
                    }
                    case 32045:                             // Soul Charge
                    case 32051:
                    case 32052:
                    {
                        // max duration is 2 minutes, but expected to be random duration
                        // real time randomness is unclear, using max 30 seconds here
                        // see further down for expire of this aura
                        GetHolder()->SetAuraDuration(rand()%30*IN_MILLISECONDS);
                        return;
                    }
                    case 33326:                             // Stolen Soul Dispel
                    {
                        target->RemoveAurasDueToSpell(32346);
                        return;
                    }
                    case 36587:                             // Vision Guide
                    {
                        target->CastSpell(target, 36573, true, NULL, this);
                        return;
                    }
                    // Gender spells
                    case 38224:                             // Illidari Agent Illusion
                    case 37096:                             // Blood Elf Illusion
                    case 46354:                             // Blood Elf Illusion
                    {
                        uint8 gender = target->getGender();
                        uint32 spellId;
                        switch (GetId())
                        {
                            case 38224: spellId = (gender == GENDER_MALE ? 38225 : 38227); break;
                            case 37096: spellId = (gender == GENDER_MALE ? 37092 : 37094); break;
                            case 46354: spellId = (gender == GENDER_MALE ? 46355 : 46356); break;
                            default: return;
                        }
                        target->CastSpell(target, spellId, true, NULL, this);
                        return;
                    }
                    case 39238:                             // Fumping
                    {
                        if (!target)
                            return;

                        if (Unit* caster = GetCaster())
                            caster->SummonCreature(urand(0,1) ? 22482 : 22483, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 60000 );
                        return;
                    }
                    case 39850:                             // Rocket Blast
                    {
                        if (roll_chance_i(20))              // backfire stun
                            target->CastSpell(target, 51581, true, NULL, this);
                        return;
                    }
                    case 43873:                             // Headless Horseman Laugh
                        target->PlayDistanceSound(11965);
                        return;
                    case 43885:                             // Headless Horseman - Horseman Laugh, Maniacal
                        target->PlayDirectSound(11965);     // triggered by spells delaying laugh
                        return;
                    case 45963:                             // Call Alliance Deserter
                    {
                        // Escorting Alliance Deserter
                        if (target->GetMiniPet())
                            target->CastSpell(target, 45957, true);

                        return;
                    }
                    case 46361:                             // Reinforced Net
                    {
                        if (!target)
                            return;

                        float x = target->GetPositionX();
                        float y = target->GetPositionY();
                        float z = target->GetPositionZ();
                        float o = target->GetOrientation();
                        float ground = target->GetMap()->GetHeight(target->GetPhaseMask(), x, y, MAX_HEIGHT);

                        if (ground <= INVALID_HEIGHT)
                            return;

                        target->MonsterMoveWithSpeed(x, y, ground, 32);
                        return;
                    }
                    case 46699:                             // Requires No Ammo
                        return;
                    case 47190:                             // Toalu'u's Spiritual Incense
                        target->CastSpell(target, 47189, true, NULL, this);
                        // allow script to process further (text)
                        break;
                    case 47795:                             // Cold Cleanse
                    {
                        if (Unit* Caster = GetCaster())
                        {
                            Caster->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                            Caster->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                            Caster->DeleteThreatList();
                            Caster->CombatStop(true);
                        }
                        return;
                    }
                    case 47977:                             // Magic Broom
                        Spell::SelectMountByAreaAndSkill(target, GetSpellProto(), 42680, 42683, 42667, 42668, 0);
                        return;
                    case 48025:                             // Headless Horseman's Mount
                        Spell::SelectMountByAreaAndSkill(target, GetSpellProto(), 51621, 48024, 51617, 48023, 0);
                        return;
                    case 48143:                             // Forgotten Aura
                        // See Death's Door
                        target->CastSpell(target, 48814, true, NULL, this);
                        return;
                    case 48276:                             // Svala - Ritual Of Sword
                        target->CastSpell(target, 54148, true);   //Cast Choose Target
                        target->CastSpell(target, 48331, true);   //Cast Swirl Sword
                        target->CastSpell(target, 54159, true);   //Cast Remove Equipment
                        return;
                    case 50756:                             // Drakos Shpere Passive
                        target->CastSpell(target, 50758, true);
                        return;
                    case 51405:                             // Digging for Treasure
                        target->HandleEmote(EMOTE_STATE_WORK);
                        // Pet will be following owner, this makes him stop
                        target->addUnitState(UNIT_STAT_STUNNED);
                        return;
                    case 52921:                             // Arc Lightning (Halls of Lighning: Loken)
                        target->CastSpell(target, 52924, false);
                        return;
                    case 54236:                             // Death Touch - Lich King kill Overlord Drakuru
                        target->CastSpell(target, 54248, false);    // Cast Drakuru Death
                        return;
                    case 54729:                             // Winged Steed of the Ebon Blade
                        Spell::SelectMountByAreaAndSkill(target, GetSpellProto(), 0, 0, 54726, 54727, 0);
                        return;
                    case 58600:                             // Restricted Flight Area
                    case 91604:                             // Restricted Flight Area
                        target->MonsterWhisper(LANG_NO_FLY_ZONE, target, true);
                        return;
                    case 58983:                             // Big Blizzard Bear
                        Spell::SelectMountByAreaAndSkill(target, GetSpellProto(), 58997, 58999, 58999, 58999, 0);
                        return;
                    /*case 61187:                             // Twilight Shift
                        target->CastSpell(target, 61885, true);
                        if (target->HasAura(57620))
                            target->RemoveAurasDueToSpell(57620);
                        if (target->HasAura(57874))
                            target->RemoveAurasDueToSpell(57874);
                        break;*/
                    case 62061:                             // Festive Holiday Mount
                        if (target->HasAuraType(SPELL_AURA_MOUNTED))
                            // Reindeer Transformation
                            target->CastSpell(target, 25860, true, NULL, this);
                        return;
                    case 62109:                             // Tails Up: Aura
                        target->setFaction(1990);           // Ambient (hostile)
                        target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                        return;
                    case 63322:                             // Saronite Vapors
                        if (Unit* caster = GetCaster())
                        {
                            int32 damage = 50 << GetStackAmount();
                            target->CastCustomSpell(target, 63338, &damage, 0, 0, true, 0, 0, caster->GetObjectGuid()); // damage spell
                            damage = damage >> 1;
                            target->CastCustomSpell(target, 63337, &damage, 0, 0, true); // manareg spell
                        }
                        return;
                    case 63624:                             // Learn a Second Talent Specialization
                        // Teach Learn Talent Specialization Switches, required for client triggered casts, allow after 30 sec delay
                        if (target->GetTypeId() == TYPEID_PLAYER)
                            ((Player*)target)->learnSpell(63680, false);
                        return;
                    case 68645:
                        // Rocket Pack
                        if (target->GetTypeId() == TYPEID_PLAYER)
                        {
                            // Rocket Burst - visual effect
                            target->CastSpell(target, 69192, true, NULL, this);
                            // Rocket Pack - causing damage
                            target->CastSpell(target, 69193, true, NULL, this);
                            return;
                        }
                        return;
                    case 63651:                             // Revert to One Talent Specialization
                        // Teach Learn Talent Specialization Switches, remove
                        if (target->GetTypeId() == TYPEID_PLAYER)
                            ((Player*)target)->removeSpell(63680);
                        return;
                    case 68912:                             // Wailing Souls
                        if (Unit* caster = GetCaster())
                        {
                            caster->SetTargetGuid(target->GetObjectGuid());

                            // TODO - this is confusing, it seems the boss should channel this aura, and start casting the next spell
                            caster->CastSpell(caster, 68899, false);
                        }
                        return;
                    case 71342:                             // Big Love Rocket
                        Spell::SelectMountByAreaAndSkill(target, GetSpellProto(), 71344, 71345, 71346, 71347, 0);
                        return;
                    case 71563:                             // Deadly Precision
                        target->CastSpell(target, 71564, true, NULL, this);
                        return;
                    case 72087:                             // Kinetic Bomb Knockback
                        float x, y, z;
                        target->GetPosition(x, y, z);
                        target->GetMotionMaster()->Clear();
                        target->GetMotionMaster()->MovePoint(0, x, y, z + 6.0f * GetStackAmount());
                        return;
                    case 72286:                             // Invincible
                        Spell::SelectMountByAreaAndSkill(target, GetSpellProto(), 72281, 72282, 72283, 72284, 0);
                        return;
                    case 74856:                             // Blazing Hippogryph
                        Spell::SelectMountByAreaAndSkill(target, GetSpellProto(), 0, 0, 74854, 74855, 0);
                        return;
                    case 75614:                             // Celestial Steed
                        Spell::SelectMountByAreaAndSkill(target, GetSpellProto(), 75619, 75620, 75617, 75618, 76153);
                        return;
                    case 75973:                             // X-53 Touring Rocket
                        Spell::SelectMountByAreaAndSkill(target, GetSpellProto(), 0, 0, 75957, 75972, 76154);
                        return;
                    case 87649:                             // Satisfied
                    {
                        if (GetStackAmount() == 91 && target->GetTypeId() == TYPEID_PLAYER)
                            ((Player*)target)->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, 99041, 1);
                        return;
                    }
                }
                break;
            }
            case SPELLFAMILY_WARRIOR:
            {
                switch(GetId())
                {
                    case 41099:                             // Battle Stance
                    {
                        if (target->GetTypeId() != TYPEID_UNIT)
                            return;

                        // Stance Cooldown
                        target->CastSpell(target, 41102, true, NULL, this);

                        // Battle Aura
                        target->CastSpell(target, 41106, true, NULL, this);

                        // equipment
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, 32614);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_1, 0);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_2, 0);
                        return;
                    }
                    case 41100:                             // Berserker Stance
                    {
                        if (target->GetTypeId() != TYPEID_UNIT)
                            return;

                        // Stance Cooldown
                        target->CastSpell(target, 41102, true, NULL, this);

                        // Berserker Aura
                        target->CastSpell(target, 41107, true, NULL, this);

                        // equipment
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, 32614);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_1, 0);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_2, 0);
                        return;
                    }
                    case 41101:                             // Defensive Stance
                    {
                        if (target->GetTypeId() != TYPEID_UNIT)
                            return;

                        // Stance Cooldown
                        target->CastSpell(target, 41102, true, NULL, this);

                        // Defensive Aura
                        target->CastSpell(target, 41105, true, NULL, this);

                        // equipment
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, 32604);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_1, 31467);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_2, 0);
                        return;
                    }
                    case 52437:                             // Sudden Death
                    {
                        if (target->GetTypeId() != TYPEID_PLAYER)
                            return;

                        ((Player*)target)->RemoveSpellCooldown(86346, true);
                        return;
                    }
                    case 53790:                             // Defensive Stance
                    {
                        if (target->GetTypeId() != TYPEID_UNIT)
                            return;

                        // Stance Cooldown
                        target->CastSpell(target, 59526, true, NULL, this);

                        // Defensive Aura
                        target->CastSpell(target, 41105, true, NULL, this);

                        // equipment
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, 43625);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_1, 39384);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_2, 0);
                        return;
                    }
                    case 53791:                             // Berserker Stance
                    {
                        if (target->GetTypeId() != TYPEID_UNIT)
                            return;

                        // Stance Cooldown
                        target->CastSpell(target, 59526, true, NULL, this);

                        // Berserker Aura
                        target->CastSpell(target, 41107, true, NULL, this);

                        // equipment
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, 43625);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_1, 43625);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_2, 0);
                        return;
                    }
                    case 53792:                             // Battle Stance
                    {
                        if (target->GetTypeId() != TYPEID_UNIT)
                            return;

                        // Stance Cooldown
                        target->CastSpell(target, 59526, true, NULL, this);

                        // Battle Aura
                        target->CastSpell(target, 41106, true, NULL, this);

                        // equipment
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_0, 43623);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_1, 0);
                        ((Creature*)target)->SetVirtualItem(VIRTUAL_ITEM_SLOT_2, 0);
                        return;
                    }
                }

                // Overpower
                if (classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x0000000000000004))
                {
                    // Must be casting target
                    if (!target->GetObjectGuid().IsPlayerOrPet() || !target->IsNonMeleeSpellCasted(false, false, true, false))
                        return;

                    Unit* caster = GetCaster();
                    if (!caster)
                        return;

                    Unit::AuraList const& modifierAuras = caster->GetAurasByType(SPELL_AURA_ADD_FLAT_MODIFIER);
                    for(Unit::AuraList::const_iterator itr = modifierAuras.begin(); itr != modifierAuras.end(); ++itr)
                    {
                        // Unrelenting Assault
                        if ((*itr)->GetSpellProto()->GetSpellFamilyName()==SPELLFAMILY_WARRIOR && (*itr)->GetSpellProto()->SpellIconID == 2775)
                        {
                            switch ((*itr)->GetSpellProto()->Id)
                            {
                                case 46859:                 // Unrelenting Assault, rank 1
                                    target->CastSpell(target,64849,true,NULL,(*itr));
                                    break;
                                case 46860:                 // Unrelenting Assault, rank 2
                                    target->CastSpell(target,64850,true,NULL,(*itr));
                                    break;
                                default:
                                    break;
                            }
                            break;
                        }
                    }
                    return;
                }
                break;
            }
            case SPELLFAMILY_HUNTER:
            {
                // Ready, Set, Aim...
                if (GetId() == 82925)
                {
                    if (GetStackAmount() >= GetSpellProto()->GetStackAmount())
                    {
                        target->CastSpell(target, 82926, true);
                        target->RemoveAurasDueToSpell(GetId());
                        return;
                    }
                }
                break;
            }
            case SPELLFAMILY_MAGE:
            {
                // Fingers of Frost stacks set to max at apply
                if (GetId() == 74396)
                    GetHolder()->SetAuraCharges(GetSpellProto()->GetStackAmount());
                break;
            }
            case SPELLFAMILY_SHAMAN:
            {
                switch(GetId())
                {
                    case 55198:                             // Tidal Force
                        target->CastSpell(target, 55166, true, NULL, this);
                        return;
                    case 95774:                             // Fulmination marker
                        GetHolder()->SetAuraCharges(1);
                        return;
                }
                break;
            }
        }
    }
    // AT REMOVE
    else
    {
        if (IsQuestTameSpell(GetId()) && target->isAlive())
        {
            Unit* caster = GetCaster();
            if (!caster || !caster->isAlive())
                return;

            uint32 finalSpellId = 0;
            switch(GetId())
            {
                case 19548: finalSpellId = 19597; break;
                case 19674: finalSpellId = 19677; break;
                case 19687: finalSpellId = 19676; break;
                case 19688: finalSpellId = 19678; break;
                case 19689: finalSpellId = 19679; break;
                case 19692: finalSpellId = 19680; break;
                case 19693: finalSpellId = 19684; break;
                case 19694: finalSpellId = 19681; break;
                case 19696: finalSpellId = 19682; break;
                case 19697: finalSpellId = 19683; break;
                case 19699: finalSpellId = 19685; break;
                case 19700: finalSpellId = 19686; break;
                case 30646: finalSpellId = 30647; break;
                case 30653: finalSpellId = 30648; break;
                case 30654: finalSpellId = 30652; break;
                case 30099: finalSpellId = 30100; break;
                case 30102: finalSpellId = 30103; break;
                case 30105: finalSpellId = 30104; break;
            }

            if (finalSpellId)
                caster->CastSpell(target, finalSpellId, true, NULL, this);

            return;
        }

        switch(GetId())
        {
            case 10255:                                     // Stoned
            {
                if (Unit* caster = GetCaster())
                {
                    if (caster->GetTypeId() != TYPEID_UNIT)
                        return;

                    // see dummy effect of spell 10254 for removal of flags etc
                    caster->CastSpell(caster, 10254, true);
                }
                return;
            }
            case 12479:                                     // Hex of Jammal'an
                target->CastSpell(target, 12480, true, NULL, this);
                return;
            case 12774:                                     // (DND) Belnistrasz Idol Shutdown Visual
            {
                if (m_removeMode == AURA_REMOVE_BY_DEATH)
                    return;

                // Idom Rool Camera Shake <- wtf, don't drink while making spellnames?
                if (Unit* caster = GetCaster())
                    caster->CastSpell(caster, 12816, true);

                return;
            }
            case 28059:                                     // Positive Charge (Thaddius)
            {
                if (target->HasAura(29659))
                    target->RemoveAurasDueToSpell(29659);
                return;
            }
            case 28084:                                     // Negative Charge (Thaddius)
            {
                if (target->HasAura(29660))
                    target->RemoveAurasDueToSpell(29660);
                return;
            }
            case 28169:                                     // Mutating Injection
            {
                // Mutagen Explosion
                target->CastSpell(target, 28206, true, NULL, this);
                // Poison Cloud
                target->CastSpell(target, 28240, true, NULL, this);
                return;
            }
            case 32045:                                     // Soul Charge
            {
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                    target->CastSpell(target, 32054, true, NULL, this);

                return;
            }
            case 32051:                                     // Soul Charge
            {
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                    target->CastSpell(target, 32057, true, NULL, this);

                return;
            }
            case 32052:                                     // Soul Charge
            {
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                    target->CastSpell(target, 32053, true, NULL, this);

                return;
            }
            case 32286:                                     // Focus Target Visual
            {
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                    target->CastSpell(target, 32301, true, NULL, this);

                return;
            }
            case 35079:                                     // Misdirection, triggered buff
            case 59628:                                     // Tricks of the Trade, triggered buff
            case 59665:                                     // Vigilance, redirection spell
            {
                if (Unit* pCaster = GetCaster())
                    pCaster->getHostileRefManager().ResetThreatRedirection();
                return;
            }
            case 36730:                                     // Flame Strike
            {
                target->CastSpell(target, 36731, true, NULL, this);
                return;
            }
            case 41099:                                     // Battle Stance
            {
                // Battle Aura
                target->RemoveAurasDueToSpell(41106);
                return;
            }
            case 41100:                                     // Berserker Stance
            {
                // Berserker Aura
                target->RemoveAurasDueToSpell(41107);
                return;
            }
            case 41101:                                     // Defensive Stance
            {
                // Defensive Aura
                target->RemoveAurasDueToSpell(41105);
                return;
            }
            case 42454:                                     // Captured Totem
            {
                if (m_removeMode == AURA_REMOVE_BY_DEFAULT)
                {
                    if (target->getDeathState() != CORPSE)
                        return;

                    Unit* pCaster = GetCaster();

                    if (!pCaster)
                        return;

                    // Captured Totem Test Credit
                    if (Player* pPlayer = pCaster->GetCharmerOrOwnerPlayerOrPlayerItself())
                        pPlayer->CastSpell(pPlayer, 42455, true);
                }

                return;
            }
            case 42517:                                     // Beam to Zelfrax
            {
                // expecting target to be a dummy creature
                Creature* pSummon = target->SummonCreature(23864, 0.0f, 0.0f, 0.0f, target->GetOrientation(), TEMPSUMMON_DEAD_DESPAWN, 0);

                Unit* pCaster = GetCaster();

                if (pSummon && pCaster)
                    pSummon->GetMotionMaster()->MovePoint(0, pCaster->GetPositionX(), pCaster->GetPositionY(), pCaster->GetPositionZ());

                return;
            }
            case 43681:                                     // Inactive
            {
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE && target->GetTypeId() == TYPEID_PLAYER)
                {
                    Player* plr = (Player*)target;
                    plr->ToggleAFK();
                    if (!plr->isAFK())
                        plr->ToggleAFK();
                }
                return;
            }
            case 43969:                                     // Feathered Charm
            {
                // Steelfeather Quest Credit, Are there any requirements for this, like area?
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                    target->CastSpell(target, 43984, true);

                return;
            }
            case 44191:                                     // Flame Strike
            {
                if (target->GetMap()->IsDungeon())
                {
                    uint32 spellId = target->GetMap()->IsRegularDifficulty() ? 44190 : 46163;

                    target->CastSpell(target, spellId, true, NULL, this);
                }
                return;
            }
            case 45934:                                     // Dark Fiend
            {
                // Kill target if dispelled
                if (m_removeMode==AURA_REMOVE_BY_DISPEL)
                    target->DealDamage(target, target->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                return;
            }
            case 45963:                                     // Call Alliance Deserter
            {
                // Escorting Alliance Deserter
                target->RemoveAurasDueToSpell(45957);
                return;
            }
            case 46308:                                     // Burning Winds
            {
                // casted only at creatures at spawn
                target->CastSpell(target, 47287, true, NULL, this);
                return;
            }
            case 47744:                                     // Rage of Jin'arrak
            {
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                {
                    if (Unit* caster = GetCaster())
                    {
                        caster->CastSpell(caster, 61611, true);
                        ((Player*)caster)->KilledMonsterCredit(26902);
                    }
                    return;
                }
            }
            case 47795:                                     // Cold Cleanse
            {
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                {
                    Unit* Caster = GetCaster();

                    if (Caster->isAlive())
                    {
                        Caster->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
                        Caster->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                        Creature* pCreature = NULL;

                        MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck creature_check(*Caster,  26591, true, false, 15.0f);
                        MaNGOS::CreatureLastSearcher<MaNGOS::NearestCreatureEntryWithLiveStateInObjectRangeCheck> searcher(pCreature, creature_check);

                        Cell::VisitGridObjects(Caster, searcher, 15.0f);

                        if (pCreature)
                        {
                            float fX, fY, fZ;

                            fX = pCreature->GetPositionX();
                            fY = pCreature->GetPositionY();
                            fZ = pCreature->GetPositionZ();

                            Caster->SetSpeedRate(MOVE_RUN, 0.7f);
                            Caster->GetMotionMaster()->MovePoint(1, fX, fY, fZ);

                            switch(urand(0,1))
                            {
                                case 0: Caster->MonsterSay("I could sleep forever, mon...", LANG_UNIVERSAL, 0); break;
                                case 1: Caster->MonsterSay("Finally, I can be restin\'...", LANG_UNIVERSAL, 0); break;
                            }

                            //This should happen when Caster arive to dest place
                            pCreature->CastSpell(pCreature, 47798, true);
                            pCreature->CastSpell(pCreature, 48150, true);
                            pCreature->ForcedDespawn(15000);
                            ((Creature*)Caster)->ForcedDespawn(3000);
                            return;
                        }
                    }
                }
            }
            case 48385:                                     // Create Spirit Fount Beam
            {
                target->CastSpell(target, target->GetMap()->IsRegularDifficulty() ? 48380 : 59320, true);
                return;
            }
            case 49356:                                     // Flesh Decay - Tharonja
            {
                if (Unit* caster = GetCaster())
                {
                    caster->SetDisplayId(27073);            // Set Skeleton Model
                    caster->CastSpell(caster, 52509, true); // Cast Gift Of Tharonja
                    caster->CastSpell(caster, 52582, true); // Cast Transform Visual
                }
                return;
            }
            case 49440:                                     // Racer Slam, Slamming
            {
                Unit* chargeBunny = NULL;
                Unit* racer = NULL;
                std::list<Unit*> targets;

                MaNGOS::AnyUnitInObjectRangeCheck u_check(target, 30.0f);
                MaNGOS::UnitListSearcher<MaNGOS::AnyUnitInObjectRangeCheck> searcher(targets, u_check);
                Cell::VisitAllObjects(target, searcher, 30.0f);

                for (std::list<Unit*>::const_iterator itr = targets.begin(); itr != targets.end(); ++itr)
                {
                    Unit* unit = *itr;
                    if (unit->GetTypeId() == TYPEID_UNIT)
                    {
                        if (unit->GetEntry() == 27674 && unit->GetCreatorGuid() == target->GetObjectGuid())
                            chargeBunny = unit;
                        else if (unit != target && (unit->GetEntry() == 27664 || unit->GetEntry() == 40281))
                        {
                            if (target->isInFront(unit, 30.0f, 15.0f))
                                racer = unit;
                        }
                    }
                }

                if (racer)
                {
                    racer->StopMoving();

                    // Racer Slam Hit Destination
                    target->CastSpell(racer, 49302, true);

                    // Racer Slam, death: root and pacify
                    racer->CastSpell(racer, 49439, true);

                    // cast Racer Slam, death scene: debris
                    for (uint8 i = 0; i < 3; ++i)
                        racer->CastSpell(racer, 49327, true);

                    // Racer Slam, death scene: car flip Parent
                    racer->CastSpell(racer, 49337, true);

                    // Racer Kill Counter
                    target->CastSpell(target, 49444, true, NULL, NULL, target->GetCreatorGuid());
                }
                else if (chargeBunny)
                {
                    // Racer Slam Hit Destination
                    target->CastSpell(chargeBunny, 49302, true);
                }
                return;
            }
            case 50141:                                     // Blood Oath
            {
                // Blood Oath
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                    target->CastSpell(target, 50001, true, NULL, this);

                return;
            }
            case 51405:                                     // Digging for Treasure
            {
                const uint32 spell_list[7] =
                {
                    51441,                                  // hare
                    51397,                                  // crystal
                    51398,                                  // armor
                    51400,                                  // gem
                    51401,                                  // platter
                    51402,                                  // treasure
                    51443                                   // bug
                };

                target->CastSpell(target, spell_list[urand(0,6)], true);

                target->HandleEmote(EMOTE_STATE_NONE);
                target->clearUnitState(UNIT_STAT_STUNNED);
                return;
            }
            case 51870:                                     // Collect Hair Sample
            {
                if (Unit* pCaster = GetCaster())
                {
                    if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                        pCaster->CastSpell(target, 51872, true, NULL, this);
                }

                return;
            }
            case 52098:                                     // Charge Up
            {
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                    target->CastSpell(target, 52092, true, NULL, this);

                return;
            }
            case 53039:                                     // Deploy Parachute
            {
                // Crusader Parachute
                target->RemoveAurasDueToSpell(53031);
                return;
            }
            case 53463:                                     // Flesh Return - Tharonja
            {
                if (Unit* caster = GetCaster())
                {
                    caster->SetDisplayId(27072);            // Set Basic Model
                    caster->CastSpell(caster, 52582, true); // Cast Transform Visual
                }
                return;
            }
            case 53790:                                     // Defensive Stance
            {
                // Defensive Aura
                target->RemoveAurasDueToSpell(41105);
                return;
            }
            case 53791:                                     // Berserker Stance
            {
                // Berserker Aura
                target->RemoveAurasDueToSpell(41107);
                return;
            }
            case 53792:                                     // Battle Stance
            {
                // Battle Aura
                target->RemoveAurasDueToSpell(41106);
                return;
            }
            case 56150:                                     // Jedoga Sacriface Beam
            {
                if (Unit* caster = GetCaster())
                {
                    int32 health = target->GetHealthPercent();
                    int32 mana = target->GetPower(POWER_MANA)*100/target->GetMaxPower(POWER_MANA);
                    int32 instantkill = 1;
                    caster->CastCustomSpell(target, 58919, &health, &instantkill, &mana, true);
                }
                return;
            }
            case 56511:                                     // Towers of Certain Doom: Tower Bunny Smoke Flare Effect
            {
                // Towers of Certain Doom: Skorn Cannonfire
                if (m_removeMode == AURA_REMOVE_BY_DEFAULT)
                    target->CastSpell(target, 43069, true);

                return;
            }
            case 58600:                                     // Restricted Flight Area
            {
                //AreaTableEntry const* area = GetAreaEntryByAreaID(target->GetAreaId());

                // Dalaran restricted flight zone (recheck before apply unmount)
                //if (area && target->GetTypeId() == TYPEID_PLAYER && (area->flags & AREA_FLAG_CANNOT_FLY) &&
                //    ((Player*)target)->IsFreeFlying() && !((Player*)target)->isGameMaster())
                //{
                //    target->CastSpell(target, 58601, true); // Remove Flight Auras (also triggered Parachute (45472))
                //}
                return;
            }
            case 91604:                                     //  Restricted Flight Area
            {
                if (target->GetTypeId() != TYPEID_PLAYER)
                    return;

                if (((Player*)target)->IsFreeFlying() && !((Player*)target)->isGameMaster())
                    target->CastSpell(target, 58601, true);
                return;
            }
            // megai2: Lightwell die on charges end
            case 59907:
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;
                // megai2: simple...
                caster->CastSpell(caster, 5, true);
                return;
            }
            case 61900:                                     // Electrical Charge
            {
                if (m_removeMode == AURA_REMOVE_BY_DEATH)
                    target->CastSpell(target, GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_0), true);

                return;
            }
            case 64398:                                     // Summon Scrap Bot (Ulduar, Mimiron) - for Scrap Bots
            case 64426:                                     // Summon Scrap Bot (Ulduar, Mimiron) - for Assault Bots
            case 64621:                                     // Summon Fire Bot (Ulduar, Mimiron)
            {
                uint32 triggerSpell = 0;
                switch (GetId())
                {
                    case 64398: triggerSpell = 63819; break;
                    case 64426: triggerSpell = 64427; break;
                    case 64621: triggerSpell = 64622; break;
                }
                target->CastSpell(target, triggerSpell, false);
                return;
            }
            case 68839:                                     // Corrupt Soul
            {
                // Knockdown Stun
                target->CastSpell(target, 68848, true, NULL, this);
                // Draw Corrupted Soul
                target->CastSpell(target, 68846, true, NULL, this);
                return;
            }
            case 72087:                                     // Kinetic Bomb Knockback
            {
                float x, y, z;
                target->GetPosition(x, y, z);
                z = target->GetMap()->GetHeight(target->GetPhaseMask(), x, y, z);
                target->GetMotionMaster()->Clear();
                target->GetMotionMaster()->MovePoint(0, x, y, z);
                return;
            }
        }

        // Living Bomb
        if (classOptions && classOptions->SpellFamilyName == SPELLFAMILY_MAGE && (classOptions->SpellFamilyFlags & UI64LIT(0x2000000000000)))
        {
            if (m_removeMode == AURA_REMOVE_BY_EXPIRE || m_removeMode == AURA_REMOVE_BY_DISPEL)
                if (GetCaster())
                    GetCaster()->CastSpell(target,m_modifier.m_amount,true,NULL,this);
                else
                    target->CastSpell(target,m_modifier.m_amount,true,NULL,this);

            return;
        }
    }

    // AT APPLY & REMOVE
    switch(GetSpellProto()->GetSpellFamilyName())
    {
        case SPELLFAMILY_GENERIC:
        {
            switch(GetId())
            {
                case 6606:                                  // Self Visual - Sleep Until Cancelled (DND)
                {
                    if (apply)
                    {
                        target->SetStandState(UNIT_STAND_STATE_SLEEP);
                        target->addUnitState(UNIT_STAT_ROOT);
                    }
                    else
                    {
                        target->clearUnitState(UNIT_STAT_ROOT);
                        target->SetStandState(UNIT_STAND_STATE_STAND);
                    }

                    return;
                }
                case 11196:                                 // Recently Bandaged
                    target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, GetMiscValue(), apply);
                    return;
                case 24658:                                 // Unstable Power
                {
                    if (apply)
                    {
                        Unit* caster = GetCaster();
                        if (!caster)
                            return;

                        caster->CastSpell(target, 24659, true, NULL, NULL, GetCasterGuid());
                    }
                    else
                        target->RemoveAurasDueToSpell(24659);
                    return;
                }
                case 25673:                                 // Riding Har'koa's Kitten
                {
                    if (target->GetTypeId() != TYPEID_PLAYER)
                        return;

                    ((Player*)target)->SetClientControl(target, !apply);
                    return;
                }
                case 71563:                                 // Deadly Precision
                {
                    uint32 spellId = 71564;
                    if (apply)
                    {
                        SpellEntry const *spell = sSpellStore.LookupEntry(spellId);
                        Unit* caster = GetCaster();
                        if (!spell || !caster)
                            return;

                        for (uint32 i = 0; i < spell->GetStackAmount(); ++i)
                            caster->CastSpell(GetTarget(), spellId, true, NULL, NULL, GetCasterGuid());

                        return;
                    }
                    GetTarget()->RemoveAurasDueToSpell(spellId);
                    return;
                }
                case 24661:                                 // Restless Strength
                {
                    if (apply)
                    {
                        Unit* caster = GetCaster();
                        if (!caster)
                            return;

                        caster->CastSpell(target, 24662, true, NULL, NULL, GetCasterGuid());
                    }
                    else
                        target->RemoveAurasDueToSpell(24662);
                    return;
                }
                case 29266:                                 // Permanent Feign Death
                case 31261:                                 // Permanent Feign Death (Root)
                case 37493:                                 // Feign Death
                case 52593:                                 // Bloated Abomination Feign Death
                case 55795:                                 // Falling Dragon Feign Death
                case 57626:                                 // Feign Death
                case 57685:                                 // Permanent Feign Death
                case 58768:                                 // Permanent Feign Death (Freeze Jumpend)
                case 58806:                                 // Permanent Feign Death (Drowned Anim)
                case 58951:                                 // Permanent Feign Death
                case 64461:                                 // Permanent Feign Death (No Anim) (Root)
                case 65985:                                 // Permanent Feign Death (Root Silence Pacify)
                case 70592:                                 // Permanent Feign Death
                case 70628:                                 // Permanent Feign Death
                case 70630:                                 // Frozen Aftermath - Feign Death
                case 71598:                                 // Feign Death
                {
                    // Unclear what the difference really is between them.
                    // Some has effect1 that makes the difference, however not all.
                    // Some appear to be used depending on creature location, in water, at solid ground, in air/suspended, etc
                    // For now, just handle all the same way
                    if (target->GetTypeId() == TYPEID_UNIT)
                        target->SetFeignDeath(apply);

                    return;
                }
                case 35356:                                 // Spawn Feign Death
                case 35357:                                 // Spawn Feign Death
                case 42557:                                 // Feign Death
                case 51329:                                 // Feign Death
                {
                    if (target->GetTypeId() == TYPEID_UNIT)
                    {
                        // Flags not set like it's done in SetFeignDeath()
                        // UNIT_DYNFLAG_DEAD does not appear with these spells.
                        // All of the spells appear to be present at spawn and not used to feign in combat or similar.
                        if (apply)
                        {
                            target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_29);
                            target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);

                            target->addUnitState(UNIT_STAT_DIED);
                        }
                        else
                        {
                            target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_29);
                            target->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH);

                            target->clearUnitState(UNIT_STAT_DIED);
                        }
                    }
                    return;
                }
                case 40133:                                 //Summon Fire Elemental
                {
                    Unit* caster = GetCaster();
                    if (!caster)
                        return;

                    Unit *owner = caster->GetOwner();
                    if (owner && owner->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (apply)
                            owner->CastSpell(owner, 8985, true);
                        else
                            ((Player*)owner)->RemovePet(PET_SAVE_REAGENTS);
                    }
                    return;
                }
                case 40132:                                 //Summon Earth Elemental
                {
                    Unit* caster = GetCaster();
                    if (!caster)
                        return;

                    Unit *owner = caster->GetOwner();
                    if (owner && owner->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (apply)
                            owner->CastSpell(owner, 19704, true);
                        else
                            ((Player*)owner)->RemovePet(PET_SAVE_REAGENTS);
                    }
                    return;
                }
                case 40214:                                 //Dragonmaw Illusion
                {
                    if (apply)
                    {
                        target->CastSpell(target, 40216, true);
                        target->CastSpell(target, 42016, true);
                    }
                    else
                    {
                        target->RemoveAurasDueToSpell(40216);
                        target->RemoveAurasDueToSpell(42016);
                    }
                    return;
                }
                case 42515:                                 // Jarl Beam
                {
                    // aura animate dead (fainted) state for the duration, but we need to animate the death itself (correct way below?)
                    if (Unit* pCaster = GetCaster())
                        pCaster->ApplyModFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH, apply);

                    // Beam to Zelfrax at remove
                    if (!apply)
                        target->CastSpell(target, 42517, true);
                    return;
                }
                case 43874:                                 // Scourge Mur'gul Camp: Force Shield Arcane Purple x3
                    target->ApplyModFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE, apply);
                    if (apply)
                        target->addUnitState(UNIT_STAT_ROOT);
                    return;
                case 47178:                                 // Plague Effect Self
                    target->SetFeared(apply, GetCasterGuid(), GetId());
                    return;
                case 53813:                                 // Urom Shield
                case 50053:                                 // Varos Shield
                    target->ApplyModFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE, apply);
                    target->ApplyModFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE, apply);
                    return;
                case 54852:                                 // Drakkari Colossus Stun (Hmmm... I'm lookup all stun effect spell, but not find needed!)
                    if (apply)
                    {
                        target->addUnitState(UNIT_STAT_STUNNED);
                        target->SetTargetGuid(target->GetObjectGuid());
                        target->CastSpell(target, 16245, true);
                    }
                    else
                    {
                        if (target->getVictim() && target->isAlive())
                            target->SetTargetGuid(target->getVictim()->GetObjectGuid());
                        target->clearUnitState(UNIT_STAT_STUNNED);
                        target->RemoveAurasDueToSpell(16245);
                    }
                    target->ApplyModFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE, apply);
                    target->ApplyModFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED, apply);
                    return;
                case 56422:                                 // Nerubian Submerge
                    // not known if there are other things todo, only flag are confirmed valid
                    target->ApplyModFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE, apply);
                    return;
                case 58204:                                 // LK Intro VO (1)
                    if (target->GetTypeId() == TYPEID_PLAYER)
                    {
                        // Play part 1
                        if (apply)
                            target->PlayDirectSound(14970, (Player *)target);
                        // continue in 58205
                        else
                            target->CastSpell(target, 58205, true);
                    }
                    return;
                case 58205:                                 // LK Intro VO (2)
                    if (target->GetTypeId() == TYPEID_PLAYER)
                    {
                        // Play part 2
                        if (apply)
                            target->PlayDirectSound(14971, (Player *)target);
                        // Play part 3
                        else
                            target->PlayDirectSound(14972, (Player *)target);
                    }
                    return;
                case 27978:
                case 40131:
                    if (apply)
                        target->m_AuraFlags |= UNIT_AURAFLAG_ALIVE_INVISIBLE;
                    else
                        target->m_AuraFlags |= ~UNIT_AURAFLAG_ALIVE_INVISIBLE;
                    return;
                case 77487:                             // Shadow Orb
                {
                    if (apply)
                    {
                        if (GetStackAmount() == GetSpellProto()->GetStackAmount())
                            target->CastSpell(target, 93683, true);     // Shadow Orb Stack Marker
                    }
                    else
                        target->RemoveAurasDueToSpell(93683);
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_MAGE:
            break;
        case SPELLFAMILY_WARLOCK:
        {
            // Haunt
            if (classOptions && GetSpellProto()->SpellIconID == 3172 && (classOptions->SpellFamilyFlags & UI64LIT(0x0004000000000000)))
            {
                // NOTE: for avoid use additional field damage stored in dummy value (replace unused 100%
                if (apply)
                    m_modifier.m_amount = 0;                // use value as damage counter instead redundant 100% percent
                else
                {
                    int32 bp0 = m_modifier.m_amount;

                    // megai2: just deal heal, without any threat calculations
                    if (Unit* caster = GetCaster())
                    {
                        target->DealHeal(caster, bp0, sSpellStore.LookupEntry(48210));
                        // megai2: and cast visual ^^
                        target->CastSpell(caster, 50091, true);
                    }
                }
            }
            // Shadowburn
            else if (GetId() == 29341)
            {
                if (m_removeMode == AURA_REMOVE_BY_DEATH)
                {
                    if (Unit* caster = GetCaster())
                        if (caster->GetTypeId() == TYPEID_PLAYER && ((Player*)caster)->isHonorOrXPTarget(target))
                            // Soul Shard Energize
                            caster->CastSpell(caster, 95810, true);
                }
            }
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            switch(GetId())
            {
                case 52610:                                 // Savage Roar
                {
                    if (apply)
                    {
                        if (target->GetShapeshiftForm() != FORM_CAT)
                            return;

                        target->CastSpell(target, 62071, true);
                    }
                    else
                        target->RemoveAurasDueToSpell(62071);
                    return;
                }
                case 61336:                                 // Survival Instincts
                {
                    if(apply)
                    {
                        if (!target->IsInFeralForm())
                            return;

                        target->CastSpell(target, 50322, true);
                    }
                    else
                        target->RemoveAurasDueToSpell(50322);
                    return;
                }
            }

            // Lifebloom
            if (classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x1000000000))
            {
                Unit* caster = GetCaster();
                int32 damage = 0;
                if (caster)
                {
                    damage = caster->CalculateSpellDamage(target, GetSpellProto(), GetEffIndex());
                    // prevent double apply bonuses
                    if (target->GetTypeId() != TYPEID_PLAYER || !((Player*)target)->GetSession()->PlayerLoading())
                    {
                        damage = caster->SpellHealingBonusDone(target, GetSpellProto(), damage, SPELL_DIRECT_DAMAGE);
                        damage = target->SpellHealingBonusTaken(caster, GetSpellProto(), damage, SPELL_DIRECT_DAMAGE);
                    }
                }
                damage *= (GetStackAmount() ? GetStackAmount() : 1);

                if (apply)
                {
                    m_modifier.m_amount = damage;
                }
                else
                {
                    // Final heal on duration end
                    if (m_removeMode != AURA_REMOVE_BY_EXPIRE && !(m_removeMode == AURA_REMOVE_BY_CANCEL && caster && caster != target))
                        return;

                    // have a look if there is still some other Lifebloom dummy aura
                    Unit::AuraList const& auras = target->GetAurasByType(SPELL_AURA_DUMMY);
                    for(Unit::AuraList::const_iterator itr = auras.begin(); itr!=auras.end(); ++itr)
                        if ((*itr)->GetSpellProto()->IsFitToFamily(SPELLFAMILY_DRUID, UI64LIT(0x1000000000)))
                            return;

                    // final heal on expire
                    if (caster && target->IsInWorld() && GetStackAmount() > 0)
                    {
                        // Heal only on expire
                        if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                        {
                            int32 amount = damage;
                            target->CastCustomSpell(target, 33778, &amount, NULL, NULL, true, NULL, NULL, caster->GetObjectGuid());
                        }
                    }
                }
                return;
            }
            break;
        }
        case SPELLFAMILY_ROGUE:
            switch(GetId())
            {
                case 57934:                                 // Tricks of the Trade, main spell
                {
                    if (apply)
                        GetHolder()->SetAuraCharges(1);     // not have proper charges set in spell data
                    else
                    {
                        // used for direct in code aura removes and spell proc event charges expire
                        if (m_removeMode != AURA_REMOVE_BY_DEFAULT)
                            target->getHostileRefManager().ResetThreatRedirection();
                    }
                    return;
                }
            }
            break;
        case SPELLFAMILY_HUNTER:
        {
            switch(GetId())
            {
                case 34477:                                 // Misdirection, main spell
                {
                    if (apply)
                        GetHolder()->SetAuraCharges(1);     // not have proper charges set in spell data
                    else
                    {
                        // used for direct in code aura removes and spell proc event charges expire
                        if (m_removeMode != AURA_REMOVE_BY_DEFAULT)
                            target->getHostileRefManager().ResetThreatRedirection();
                    }
                    return;
                }
            }
            break;
        }
        case SPELLFAMILY_PALADIN:
            break;
        case SPELLFAMILY_SHAMAN:
        {
            switch(GetId())
            {
                case 6495:                                  // Sentry Totem
                {
                    if (target->GetTypeId() != TYPEID_PLAYER)
                        return;

                    Totem* totem = target->GetTotem(TOTEM_SLOT_AIR);

                    if (totem && apply)
                        ((Player*)target)->GetCamera().SetView(totem);
                    else
                        ((Player*)target)->GetCamera().ResetView();

                    return;
                }
            }
            break;
        }
    }

    // pet auras
    if (PetAura const* petSpell = sSpellMgr.GetPetAura(GetId(), m_effIndex))
    {
        if (apply)
            target->AddPetAura(petSpell);
        else
            target->RemovePetAura(petSpell);

        return;
    }

    if (GetEffIndex() == EFFECT_INDEX_0 && target->GetTypeId() == TYPEID_PLAYER)
    {
        SpellAreaForAreaMapBounds saBounds = sSpellMgr.GetSpellAreaForAuraMapBounds(GetId());
        if (saBounds.first != saBounds.second)
        {
            uint32 zone, area;
            target->GetZoneAndAreaId(zone, area);

            for(SpellAreaForAreaMap::const_iterator itr = saBounds.first; itr != saBounds.second; ++itr)
            {
                // some auras remove at aura remove
                if (!itr->second->IsFitToRequirements((Player*)target, zone, area))
                    target->RemoveAurasDueToSpell(itr->second->spellId);
                // some auras applied at aura apply
                else if (itr->second->autocast)
                {
                    if (!target->HasAura(itr->second->spellId, EFFECT_INDEX_0))
                        target->CastSpell(target, itr->second->spellId, true);
                }
            }
        }
    }

    // script has to "handle with care", only use where data are not ok to use in the above code.
    if (target->GetTypeId() == TYPEID_UNIT)
        sScriptMgr.OnAuraDummy(this, apply);
}

void Aura::HandleAuraMounted(bool apply, bool Real)
{
    // only at real add/remove aura
    if(!Real)
        return;

    Unit *target = GetTarget();

    if(apply)
    {
        // Running Wild
        if (GetId() == 87840)
            target->Mount(target->getGender() == GENDER_MALE ? 29422 : 29423, GetId(), 0, GetMiscValue());
        else
        {
            CreatureInfo const* ci = ObjectMgr::GetCreatureTemplate(m_modifier.m_miscvalue);
            if(!ci)
            {
                sLog.outErrorDb("AuraMounted: `creature_template`='%u' not found in database (only need it modelid)", m_modifier.m_miscvalue);
                return;
            }

            uint32 display_id = Creature::ChooseDisplayId(ci);
            CreatureModelInfo const *minfo = sObjectMgr.GetCreatureModelRandomGender(display_id);
            if (minfo)
                display_id = minfo->modelid;

            target->Mount(display_id, GetId(), ci->vehicleId, GetMiscValue());
        }

        // cast speed aura
        if (MountCapabilityEntry const* mountCapability = target->GetMountCapability(uint32(GetSpellEffect()->EffectMiscValueB)))
            target->CastSpell(target, mountCapability->SpeedModSpell, true);
    }
    else
    {
        target->Unmount(true);

        // remove speed aura
        if (MountCapabilityEntry const* mountCapability = sMountCapabilityStore.LookupEntry(m_modifier.m_amount))
            target->RemoveAurasByCasterSpell(mountCapability->SpeedModSpell, target->GetObjectGuid());
    }
}

void Aura::HandleAuraWaterWalk(bool apply, bool Real)
{
    // only at real add/remove aura
    if(!Real)
        return;

    // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
    if (!apply && GetTarget()->HasAuraType(SPELL_AURA_WATER_WALK))
        return;

    WorldPacket data;
    GetTarget()->BuildMoveWaterWalkPacket(&data, apply, 0);
    GetTarget()->SendMessageToSet(&data, true);
}

void Aura::HandleAuraFeatherFall(bool apply, bool Real)
{
    // only at real add/remove aura
    if(!Real)
        return;
    Unit *target = GetTarget();

    // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
    if (!apply && target->HasAuraType(SPELL_AURA_FEATHER_FALL))
        return;

    WorldPacket data;
    target->BuildMoveFeatherFallPacket(&data, apply, 0);
    target->SendMessageToSet(&data, true);

    // start fall from current height
    if(!apply && target->GetTypeId() == TYPEID_PLAYER)
        ((Player*)target)->SetFallInformation(0, target->GetPositionZ());

    // additional custom cases
    if(!apply)
    {
        switch(GetId())
        {
            // Soaring - Test Flight chain
            case 36812:
            case 37910:
            case 37940:
            case 37962:
            case 37968:
            {
                if (Unit* pCaster = GetCaster())
                    pCaster->CastSpell(pCaster, 37108, true);
                return;
            }
        }
    }
}

void Aura::HandleAuraHover(bool apply, bool Real)
{
    // only at real add/remove aura
    if(!Real)
        return;

    // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
    if (!apply && GetTarget()->HasAuraType(SPELL_AURA_HOVER))
        return;

    WorldPacket data;
    if (apply)
    {
        GetTarget()->m_movementInfo.AddMovementFlag(MOVEFLAG_HOVER);
        if (GetTarget()->GetTypeId() == TYPEID_PLAYER)
        {
            data.Initialize(SMSG_MOVE_SET_HOVER, 8 + 4 + 1);
            data.WriteGuidMask<1, 4, 2, 3, 0, 5, 6, 7>(GetTarget()->GetObjectGuid());
            data.WriteGuidBytes<5, 4, 1, 2, 3, 6, 0, 7>(GetTarget()->GetObjectGuid());
            data << uint32(0);
        }
        else
        {
            data.Initialize(SMSG_SPLINE_MOVE_SET_HOVER, 8 + 4 + 1);
            data.WriteGuidMask<3, 7, 0, 1, 4, 6, 2, 5>(GetTarget()->GetObjectGuid());
            data.WriteGuidBytes<2, 4, 3, 1, 7, 0, 5, 6>(GetTarget()->GetObjectGuid());
            GetTarget()->SetByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_HOVER);
        }
    }
    else
    {
        GetTarget()->m_movementInfo.RemoveMovementFlag(MOVEFLAG_HOVER);
        data.Initialize(GetTarget()->GetTypeId() == TYPEID_PLAYER ? SMSG_MOVE_UNSET_HOVER : SMSG_SPLINE_MOVE_UNSET_HOVER, 8+4);
        if (GetTarget()->GetTypeId() == TYPEID_PLAYER)
        {
            data.Initialize(SMSG_MOVE_UNSET_HOVER, 8 + 4 + 1);
            data.WriteGuidMask<4, 6, 3, 1, 2, 7, 5, 0>(GetTarget()->GetObjectGuid());
            data.WriteGuidBytes<4, 5, 3, 6, 7, 1, 2, 0>(GetTarget()->GetObjectGuid());
            data << uint32(0);
        }
        else
        {
            data.Initialize(SMSG_SPLINE_MOVE_UNSET_HOVER, 8 + 4 + 1);
            data.WriteGuidMask<6, 7, 4, 0, 3, 1, 5, 2>(GetTarget()->GetObjectGuid());
            data.WriteGuidBytes<4, 5, 3, 0, 2, 7, 6, 1>(GetTarget()->GetObjectGuid());
            GetTarget()->RemoveByteFlag(UNIT_FIELD_BYTES_1, 3, UNIT_BYTE1_FLAG_HOVER);
        }
    }
    GetTarget()->SendMessageToSet(&data, true);
}

void Aura::HandleWaterBreathing(bool /*apply*/, bool /*Real*/)
{
    // update timers in client
    if(GetTarget()->GetTypeId()==TYPEID_PLAYER)
        ((Player*)GetTarget())->UpdateMirrorTimers();
}

void Aura::HandleAuraModShapeshift(bool apply, bool Real)
{
    if (!Real)
        return;

    ShapeshiftForm form = ShapeshiftForm(m_modifier.m_miscvalue);

    SpellShapeshiftFormEntry const* ssEntry = sSpellShapeshiftFormStore.LookupEntry(form);
    if (!ssEntry)
    {
        ERROR_LOG("Unknown shapeshift form %u in spell %u", form, GetId());
        return;
    }

    uint32 modelid = 0;
    Powers PowerType = POWER_MANA;
    Unit* target = GetTarget();

    modelid = GetTarget()->GetModelForForm(form);

    if (!modelid && ssEntry->modelID_A)
    {
        // i will asume that creatures will always take the defined model from the dbc
        // since no field in creature_templates describes wether an alliance or
        // horde modelid should be used at shapeshifting
        if (target->GetTypeId() != TYPEID_PLAYER)
            modelid = ssEntry->modelID_A;
        else
        {
            // players are a bit different since the dbc has seldomly an horde modelid
            if (Player::TeamForRace(target->getRace()) == HORDE)
            {
                if (ssEntry->modelID_H)
                    modelid = ssEntry->modelID_H;           // 3.2.3 only the moonkin form has this information
                else                                        // get model for race
                    modelid = sObjectMgr.GetModelForRace(ssEntry->modelID_A, target->getRaceMask());
            }

            // nothing found in above, so use default
            if (!modelid)
                modelid = ssEntry->modelID_A;
        }
    }

    // remove polymorph before changing display id to keep new display id
    switch (form)
    {
        case FORM_CAT:
        case FORM_TREE:
        case FORM_TRAVEL:
        case FORM_AQUA:
        case FORM_BEAR:
        case FORM_FLIGHT_EPIC:
        case FORM_FLIGHT:
        case FORM_MOONKIN:
        {
            target->RemoveRootsAndSnares(form, GetHolder());
            break;
        }
        default:
           break;
    }

    if (apply)
    {
        // remove other shapeshift before applying a new one
        target->RemoveSpellsCausingAura(SPELL_AURA_MOD_SHAPESHIFT, GetHolder());

        // need send to client not form active state, or at re-apply form client go crazy
        // target->SendForcedObjectUpdate();

        if (modelid > 0)
            target->SetDisplayId(modelid);

        // now only powertype must be set
        switch (form)
        {
            case FORM_CAT:
            case FORM_SHADOW_DANCE:
                PowerType = POWER_ENERGY;
                break;
            case FORM_BEAR:
            case FORM_BATTLESTANCE:
            case FORM_BERSERKERSTANCE:
            case FORM_DEFENSIVESTANCE:
                PowerType = POWER_RAGE;
                break;
            default:
                break;
        }

        if (PowerType != POWER_MANA)
        {
            // reset power to default values only at power change
            if (target->getPowerType() != PowerType)
                target->setPowerType(PowerType);

            switch (form)
            {
                case FORM_CAT:
                case FORM_BEAR:
                {
                    // get furor proc chance
                    int32 furorChance = 0;
                    Unit::AuraList const& mDummy = target->GetAurasByType(SPELL_AURA_DUMMY);
                    for (Unit::AuraList::const_iterator i = mDummy.begin(); i != mDummy.end(); ++i)
                    {
                        if ((*i)->GetSpellProto()->SpellIconID == 238 && (*i)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_DRUID)
                        {
                            furorChance = (*i)->GetModifier()->m_amount;
                            break;
                        }
                    }

                    if (m_modifier.m_miscvalue == FORM_CAT)
                    {
                        // Furor chance is now amount allowed to save energy for cat form
                        // without talent it reset to 0
                        if ((int32)target->GetPower(POWER_ENERGY) > furorChance)
                        {
                            target->SetPower(POWER_ENERGY, 0);
                            target->CastCustomSpell(target, 17099, &furorChance, NULL, NULL, true, NULL, this);
                        }
                    }
                    else if (furorChance)                   // only if talent known
                    {
                        target->SetPower(POWER_RAGE, 0);
                        if (irand(1, 100) <= furorChance)
                            target->CastSpell(target, 17057, true, NULL, this);
                    }
                    break;
                }
                case FORM_BATTLESTANCE:
                case FORM_DEFENSIVESTANCE:
                case FORM_BERSERKERSTANCE:
                {
                    uint32 Rage_val = 0;
                    // Stance mastery + Tactical mastery (both passive, and last have aura only in defense stance, but need apply at any stance switch)
                    if (target->GetTypeId() == TYPEID_PLAYER)
                    {
                        PlayerSpellMap const& sp_list = ((Player *)target)->GetSpellMap();
                        for (PlayerSpellMap::const_iterator itr = sp_list.begin(); itr != sp_list.end(); ++itr)
                        {
                            if(itr->second.state == PLAYERSPELL_REMOVED) continue;
                            SpellEntry const *spellInfo = sSpellStore.LookupEntry(itr->first);
                            if (spellInfo && spellInfo->GetSpellFamilyName() == SPELLFAMILY_WARRIOR && spellInfo->SpellIconID == 139)
                                Rage_val += target->CalculateSpellDamage(target, spellInfo, EFFECT_INDEX_0) * 10;
                        }
                    }

                    if (target->GetPower(POWER_RAGE) > Rage_val)
                        target->SetPower(POWER_RAGE, Rage_val);
                    break;
                }
                // Shadow Dance - apply stealth mode stand flag
                case FORM_SHADOW_DANCE:
                    GetTarget()->SetStandFlags(UNIT_STAND_FLAGS_CREEP);
                    break;
                default:
                    break;
            }
        }

        target->SetShapeshiftForm(form == FORM_SHADOW_DANCE ? FORM_STEALTH : form);

        // a form can give the player a new castbar with some spells.. this is a clientside process..
        // serverside just needs to register the new spells so that player isn't kicked as cheater
        if (target->GetTypeId() == TYPEID_PLAYER)
            for (uint32 i = 0; i < 8; ++i)
                if (ssEntry->spellId[i])
                    ((Player*)target)->addSpell(ssEntry->spellId[i], true, false, false, false);
    }
    else
    {
        Unit::AuraList const& transforms = target->GetAurasByType(SPELL_AURA_TRANSFORM);
        if (!transforms.empty())
        {
            // look for other transform auras
            Aura* handledAura = *transforms.begin();
            for(Unit::AuraList::const_iterator i = transforms.begin(); i != transforms.end(); ++i)
            {
                // negative auras are preferred
                if (!IsPositiveSpell((*i)->GetSpellProto()->Id))
                {
                    handledAura = *i;
                    break;
                }
            }
            handledAura->ApplyModifier(true);
        }
        else if (modelid > 0)
            target->SetDisplayId(target->GetNativeDisplayId());

        if (target->getClass() == CLASS_DRUID)
            target->setPowerType(POWER_MANA);

        target->SetShapeshiftForm(FORM_NONE);

        switch (form)
        {
            // Nordrassil Harness - bonus
            case FORM_BEAR:
            case FORM_CAT:
                if (Aura* dummy = target->GetDummyAura(37315))
                    target->CastSpell(target, 37316, true, NULL, dummy);
                break;
            // Nordrassil Regalia - bonus
            case FORM_MOONKIN:
                if (Aura* dummy = target->GetDummyAura(37324))
                    target->CastSpell(target, 37325, true, NULL, dummy);
                break;
            // Shadow Dance - remove stealth mode stand flag
            case FORM_SHADOW_DANCE:
                GetTarget()->RemoveStandFlags(UNIT_STAND_FLAGS_CREEP);
                break;
            default:
                break;
        }

        // look at the comment in apply-part
        if (target->GetTypeId() == TYPEID_PLAYER)
            for (uint32 i = 0; i < 8; ++i)
                if (ssEntry->spellId[i])
                    ((Player*)target)->removeSpell(ssEntry->spellId[i], false, false, false);
    }

    // adding/removing linked auras
    // add/remove the shapeshift aura's boosts
    HandleShapeshiftBoosts(apply);

    // Amaru: for druids with Dash buff, force recalculate speed bonus of shapeshift
    if (target->getClass() == CLASS_DRUID)
        target->UpdateSpeed(MOVE_RUN, true);

    if (target->GetTypeId() == TYPEID_PLAYER)
        ((Player*)target)->InitDataForForm();

    // update form-dependent armor specializations
    if (target->GetTypeId() == TYPEID_PLAYER && ((Player*)target)->getClass() == CLASS_DRUID)
        ((Player*)target)->UpdateArmorSpecializations();
}

void Aura::HandleAuraTransform(bool apply, bool Real)
{
    Unit *target = GetTarget();

    if (apply)
    {
        // special case (spell specific functionality)
        if (m_modifier.m_miscvalue == 0)
        {
            switch (GetId())
            {
                case 16739:                                 // Orb of Deception
                {
                    uint32 orb_model = target->GetNativeDisplayId();
                    switch(orb_model)
                    {
                        // Troll Female
                        case 1479: target->SetDisplayId(10134); break;
                        // Troll Male
                        case 1478: target->SetDisplayId(10135); break;
                        // Tauren Male
                        case 59:   target->SetDisplayId(10136); break;
                        // Human Male
                        case 49:   target->SetDisplayId(10137); break;
                        // Human Female
                        case 50:   target->SetDisplayId(10138); break;
                        // Orc Male
                        case 51:   target->SetDisplayId(10139); break;
                        // Orc Female
                        case 52:   target->SetDisplayId(10140); break;
                        // Dwarf Male
                        case 53:   target->SetDisplayId(10141); break;
                        // Dwarf Female
                        case 54:   target->SetDisplayId(10142); break;
                        // NightElf Male
                        case 55:   target->SetDisplayId(10143); break;
                        // NightElf Female
                        case 56:   target->SetDisplayId(10144); break;
                        // Undead Female
                        case 58:   target->SetDisplayId(10145); break;
                        // Undead Male
                        case 57:   target->SetDisplayId(10146); break;
                        // Tauren Female
                        case 60:   target->SetDisplayId(10147); break;
                        // Gnome Male
                        case 1563: target->SetDisplayId(10148); break;
                        // Gnome Female
                        case 1564: target->SetDisplayId(10149); break;
                        // BloodElf Female
                        case 15475: target->SetDisplayId(17830); break;
                        // BloodElf Male
                        case 15476: target->SetDisplayId(17829); break;
                        // Dranei Female
                        case 16126: target->SetDisplayId(17828); break;
                        // Dranei Male
                        case 16125: target->SetDisplayId(17827); break;
                        default: break;
                    }
                    break;
                }
                case 42365:                                 // Murloc costume
                    target->SetDisplayId(21723);
                    break;
                //case 44186:                               // Gossip NPC Appearance - All, Brewfest
                    //break;
                //case 48305:                               // Gossip NPC Appearance - All, Spirit of Competition
                    //break;
                case 50517:                                 // Dread Corsair
                case 51926:                                 // Corsair Costume
                {
                    // expected for players
                    uint32 race = target->getRace();

                    switch(race)
                    {
                        case RACE_HUMAN:
                            target->SetDisplayId(target->getGender() == GENDER_MALE ? 25037 : 25048);
                            break;
                        case RACE_ORC:
                            target->SetDisplayId(target->getGender() == GENDER_MALE ? 25039 : 25050);
                            break;
                        case RACE_DWARF:
                            target->SetDisplayId(target->getGender() == GENDER_MALE ? 25034 : 25045);
                            break;
                        case RACE_NIGHTELF:
                            target->SetDisplayId(target->getGender() == GENDER_MALE ? 25038 : 25049);
                            break;
                        case RACE_UNDEAD:
                            target->SetDisplayId(target->getGender() == GENDER_MALE ? 25042 : 25053);
                            break;
                        case RACE_TAUREN:
                            target->SetDisplayId(target->getGender() == GENDER_MALE ? 25040 : 25051);
                            break;
                        case RACE_GNOME:
                            target->SetDisplayId(target->getGender() == GENDER_MALE ? 25035 : 25046);
                            break;
                        case RACE_TROLL:
                            target->SetDisplayId(target->getGender() == GENDER_MALE ? 25041 : 25052);
                            break;
                        case RACE_GOBLIN:                   // not really player race (3.x), but model exist
                            target->SetDisplayId(target->getGender() == GENDER_MALE ? 25036 : 25047);
                            break;
                        case RACE_BLOODELF:
                            target->SetDisplayId(target->getGender() == GENDER_MALE ? 25032 : 25043);
                            break;
                        case RACE_DRAENEI:
                            target->SetDisplayId(target->getGender() == GENDER_MALE ? 25033 : 25044);
                            break;
                    }

                    break;
                }
                //case 50531:                               // Gossip NPC Appearance - All, Pirate Day
                    //break;
                //case 51010:                               // Dire Brew
                    //break;
                case 53806:                                 // Pygmy Oil
                {
                    uint32 model_id;
        
                    CreatureInfo const * ci = ObjectMgr::GetCreatureTemplate(14883);
                    if (!ci)
                    {
                        model_id = 14973;
                        ERROR_LOG("Auras: unknown creature id = 14883 (only need its modelid) Form Spell Aura Transform in Spell ID = %d", GetId());
                    }
                    else
                        model_id = Creature::ChooseDisplayId(ci);   // Will use the default model here
        
                    target->SetDisplayId(model_id);
                    break;
                }
                case 62847:                                 // NPC Appearance - Valiant 02
                    target->SetDisplayId(target->getGender() == GENDER_MALE ? 26185 : 26186);
                    break;
                //case 62852:                               // NPC Appearance - Champion 01
                    //break;
                //case 63965:                               // NPC Appearance - Champion 02
                    //break;
                //case 63966:                               // NPC Appearance - Valiant 03
                    //break;
                case 65386:                                 // Honor the Dead
                case 65495:
                {
                    switch(target->getGender())
                    {
                        case GENDER_MALE:
                            target->SetDisplayId(29203);    // Chapman
                            break;
                        case GENDER_FEMALE:
                        case GENDER_NONE:
                            target->SetDisplayId(29204);    // Catrina
                            break;
                    }
                    break;
                }
                //case 65511:                               // Gossip NPC Appearance - Brewfest
                    //break;
                //case 65522:                               // Gossip NPC Appearance - Winter Veil
                    //break;
                //case 65523:                               // Gossip NPC Appearance - Default
                    //break;
                //case 65524:                               // Gossip NPC Appearance - Lunar Festival
                    //break;
                //case 65525:                               // Gossip NPC Appearance - Hallow's End
                    //break;
                //case 65526:                               // Gossip NPC Appearance - Midsummer
                    //break;
                //case 65527:                               // Gossip NPC Appearance - Spirit of Competition
                    //break;
                case 65528:                                 // Gossip NPC Appearance - Pirates' Day
                {
                    // expecting npc's using this spell to have models with race info.
                    // random gender, regardless of current gender
                    switch (target->getRace())
                    {
                        case RACE_HUMAN:
                            target->SetDisplayId(roll_chance_i(50) ? 25037 : 25048);
                            break;
                        case RACE_ORC:
                            target->SetDisplayId(roll_chance_i(50) ? 25039 : 25050);
                            break;
                        case RACE_DWARF:
                            target->SetDisplayId(roll_chance_i(50) ? 25034 : 25045);
                            break;
                        case RACE_NIGHTELF:
                            target->SetDisplayId(roll_chance_i(50) ? 25038 : 25049);
                            break;
                        case RACE_UNDEAD:
                            target->SetDisplayId(roll_chance_i(50) ? 25042 : 25053);
                            break;
                        case RACE_TAUREN:
                            target->SetDisplayId(roll_chance_i(50) ? 25040 : 25051);
                            break;
                        case RACE_GNOME:
                            target->SetDisplayId(roll_chance_i(50) ? 25035 : 25046);
                            break;
                        case RACE_TROLL:
                            target->SetDisplayId(roll_chance_i(50) ? 25041 : 25052);
                            break;
                        case RACE_GOBLIN:
                            target->SetDisplayId(roll_chance_i(50) ? 25036 : 25047);
                            break;
                        case RACE_BLOODELF:
                            target->SetDisplayId(roll_chance_i(50) ? 25032 : 25043);
                            break;
                        case RACE_DRAENEI:
                            target->SetDisplayId(roll_chance_i(50) ? 25033 : 25044);
                            break;
                    }

                    break;
                }
                case 65529:                                 // Gossip NPC Appearance - Day of the Dead (DotD)
                    // random, regardless of current gender
                    target->SetDisplayId(roll_chance_i(50) ? 29203 : 29204);
                    break;
                //case 66236:                               // Incinerate Flesh
                    //break;
                //case 69999:                               // [DND] Swap IDs
                    //break;
                //case 70764:                               // Citizen Costume (note: many spells w/same name)
                    //break;
                //case 71309:                               // [DND] Spawn Portal
                    //break;
                case 71450:                                 // Crown Parcel Service Uniform
                    target->SetDisplayId(target->getGender() == GENDER_MALE ? 31002 : 31003);
                    break;
                case 75531:                                 // Gnomeregan Pride
                {
                    if (target->getGender() == GENDER_MALE)
                        target->SetDisplayId(31654);
                    else
                        target->SetDisplayId(31655);
                    break;
                }
                case 75532:                                 // Darkspear Pride
                {
                    if (target->getGender() == GENDER_MALE)
                        target->SetDisplayId(31737);
                    else
                        target->SetDisplayId(31738);
                    break;
                }
                default:
                    sLog.outError("Aura::HandleAuraTransform, spell %u does not have creature entry defined, need custom defined model.", GetId());
                    break;
            }
        }
        else                                                // m_modifier.m_miscvalue != 0
        {
            uint32 model_id;

            CreatureInfo const * ci = ObjectMgr::GetCreatureTemplate(m_modifier.m_miscvalue);
            if (!ci)
            {
                model_id = 16358;                           // pig pink ^_^
                ERROR_LOG("Auras: unknown creature id = %d (only need its modelid) Form Spell Aura Transform in Spell ID = %d", m_modifier.m_miscvalue, GetId());
            }
            else
                model_id = Creature::ChooseDisplayId(ci);   // Will use the default model here

            // Polymorph (sheep/penguin case)
            if (GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_MAGE && GetSpellProto()->SpellIconID == 82)
                if (Unit* caster = GetCaster())
                {
                    if (caster->HasAura(52648))             // Glyph of the Penguin
                        model_id = 26452;
                    else if (caster->HasAura(57927))        // Glyph of the Monkey
                        model_id = 21362;
                }

            target->SetDisplayId(model_id);

            // creature case, need to update equipment if additional provided
            if (ci && target->GetTypeId() == TYPEID_UNIT)
                ((Creature*)target)->LoadEquipment(ci->equipmentId, false);

            // Dragonmaw Illusion (set mount model also)
            if(GetId()==42016 && target->GetMountID() && !target->GetAurasByType(SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED).empty())
                target->SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID,16314);
        }

        // update active transform spell only not set or not overwriting negative by positive case
        if (!target->getTransForm() || !IsPositiveSpell(GetId()) || IsPositiveSpell(target->getTransForm()))
            target->setTransForm(GetId());

        // polymorph case
        if (Real && target->GetTypeId() == TYPEID_PLAYER && target->IsPolymorphed())
        {
            // for players, start regeneration after 1s (in polymorph fast regeneration case)
            // only if caster is Player (after patch 2.4.2)
            if (GetCasterGuid().IsPlayer())
                ((Player*)target)->setRegenTimer(1 * IN_MILLISECONDS);

            //dismount polymorphed target (after patch 2.4.2)
            if (target->IsMounted())
                target->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED, GetHolder());
        }
    }
    else                                                    // !apply
    {
        // ApplyModifier(true) will reapply it if need
        target->setTransForm(0);
        target->SetDisplayId(target->GetNativeDisplayId());

        // apply default equipment for creature case
        if (target->GetTypeId() == TYPEID_UNIT)
            ((Creature*)target)->LoadEquipment(((Creature*)target)->GetCreatureInfo()->equipmentId, true);

        // re-apply some from still active with preference negative cases
        Unit::AuraList const& otherTransforms = target->GetAurasByType(SPELL_AURA_TRANSFORM);
        if (!otherTransforms.empty())
        {
            // look for other transform auras
            Aura* handledAura = *otherTransforms.begin();
            for(Unit::AuraList::const_iterator i = otherTransforms.begin();i != otherTransforms.end(); ++i)
            {
                // negative auras are preferred
                if (!IsPositiveSpell((*i)->GetSpellProto()->Id))
                {
                    handledAura = *i;
                    break;
                }
            }
            handledAura->ApplyModifier(true);
        }
        else
        // Amaru: Look for shapeshifts
        {
            Unit::AuraList const& shapeshifts = target->GetAurasByType(SPELL_AURA_MOD_SHAPESHIFT);
            if (!shapeshifts.empty()) // Amaru: only one ss aura is possible?
                if (uint32 ssmodel = target->GetModelForForm(ShapeshiftForm(shapeshifts.front()->GetModifier()->m_miscvalue)))
                    target->SetDisplayId(ssmodel);
        }

        // Dragonmaw Illusion (restore mount model)
        if (GetId() == 42016 && target->GetMountID() == 16314)
        {
            if (!target->GetAurasByType(SPELL_AURA_MOUNTED).empty())
            {
                uint32 cr_id = target->GetAurasByType(SPELL_AURA_MOUNTED).front()->GetModifier()->m_miscvalue;
                if (CreatureInfo const* ci = ObjectMgr::GetCreatureTemplate(cr_id))
                {
                    uint32 display_id = Creature::ChooseDisplayId(ci);
                    CreatureModelInfo const *minfo = sObjectMgr.GetCreatureModelRandomGender(display_id);
                    if (minfo)
                        display_id = minfo->modelid;

                    target->SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID, display_id);
                }
            }
        }
    }
}

void Aura::HandleForceReaction(bool apply, bool Real)
{
    if(GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    if(!Real)
        return;

    Player* player = (Player*)GetTarget();

    uint32 faction_id = m_modifier.m_miscvalue;
    ReputationRank faction_rank = ReputationRank(m_modifier.m_amount);

    player->GetReputationMgr().ApplyForceReaction(faction_id, faction_rank, apply);
    player->GetReputationMgr().SendForceReactions();

    // stop fighting if at apply forced rank friendly or at remove real rank friendly
    if (apply && faction_rank >= REP_FRIENDLY || !apply && player->GetReputationRank(faction_id) >= REP_FRIENDLY)
        player->StopAttackFaction(faction_id);

    switch(GetId())
    {
        case 1953:                          // Blink
        case 48020:                         // Demonic Circle
        case 54861:                         // Nitro Boosts
            if (apply &&player->InBattleGround() && (player->HasAura(23335) || player->HasAura(23333) || player->HasAura(34976)))
                if (BattleGround *bg = player->GetBattleGround())
                    bg->EventPlayerDroppedFlag(player);
            break;
        case 43450:                         // Brewfest - apple trap - friendly DND
        {
            if (apply)
                player->RemoveAurasDueToSpell(43052);   // Exhausted Ram
            break;
        }
        default:
            break;
    }
}

void Aura::HandleAuraModSkill(bool apply, bool /*Real*/)
{
    if(GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    uint32 prot = m_spellEffect->EffectMiscValue;
    int32 points = GetModifier()->m_amount;

    // defense skill is removed in 4.x.x, spell tooltips updated,
    // but auras still exist
    if (prot == SKILL_DEFENSE)
        return;

    ((Player*)GetTarget())->ModifySkillBonus(prot, (apply ? points : -points), m_modifier.m_auraname == SPELL_AURA_MOD_SKILL_TALENT);
}

void Aura::HandleChannelDeathItem(bool apply, bool Real)
{
    if(Real && !apply)
    {
        if(m_removeMode != AURA_REMOVE_BY_DEATH)
            return;
        // Item amount
        if (m_modifier.m_amount <= 0)
            return;

        if(m_spellEffect->EffectItemType == 0)
            return;

        Unit* victim = GetTarget();
        Unit* caster = GetCaster();
        if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
            return;

        // Soul Shard (target req.)
        if (m_spellEffect->EffectItemType == 6265)
        {
            // Only from non-grey units
            if (!((Player*)caster)->isHonorOrXPTarget(victim) ||
                victim->GetTypeId() == TYPEID_UNIT && !((Player*)caster)->isAllowedToLoot((Creature*)victim))
                return;
        }

        //Adding items
        uint32 noSpaceForCount = 0;
        uint32 count = m_modifier.m_amount;

        ItemPosCountVec dest;
        InventoryResult msg = ((Player*)caster)->CanStoreNewItem( NULL_BAG, NULL_SLOT, dest, m_spellEffect->EffectItemType, count, &noSpaceForCount);
        if( msg != EQUIP_ERR_OK )
        {
            count-=noSpaceForCount;
            ((Player*)caster)->SendEquipError( msg, NULL, NULL, m_spellEffect->EffectItemType );
            if (count==0)
                return;
        }

        Item* newitem = ((Player*)caster)->StoreNewItem(dest, m_spellEffect->EffectItemType, true);
        ((Player*)caster)->SendNewItem(newitem, count, true, true);
    }
}

void Aura::HandleBindSight(bool apply, bool /*Real*/)
{
    Unit* caster = GetCaster();
    if(!caster || caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Camera& camera = ((Player*)caster)->GetCamera();
    if (apply)
        camera.SetView(GetTarget());
    else
        camera.ResetView();
}

void Aura::HandleFarSight(bool apply, bool /*Real*/)
{
    Unit* caster = GetCaster();
    if(!caster || caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Camera& camera = ((Player*)caster)->GetCamera();
    if (apply)
        camera.SetView(GetTarget());
    else
        camera.ResetView();
}

void Aura::HandleAuraTrackCreatures(bool apply, bool /*Real*/)
{
    if (GetTarget()->GetTypeId()!=TYPEID_PLAYER)
        return;

    if (apply)
        GetTarget()->RemoveNoStackAurasDueToAuraHolder(GetHolder());

    if (apply)
        GetTarget()->SetFlag(PLAYER_TRACK_CREATURES, uint32(1) << (m_modifier.m_miscvalue-1));
    else
        GetTarget()->RemoveFlag(PLAYER_TRACK_CREATURES, uint32(1) << (m_modifier.m_miscvalue-1));
}

void Aura::HandleAuraTrackResources(bool apply, bool /*Real*/)
{
    if (GetTarget()->GetTypeId()!=TYPEID_PLAYER)
        return;

    if (apply)
        GetTarget()->RemoveNoStackAurasDueToAuraHolder(GetHolder());

    if (apply)
        GetTarget()->SetFlag(PLAYER_TRACK_RESOURCES, uint32(1) << (m_modifier.m_miscvalue-1));
    else
        GetTarget()->RemoveFlag(PLAYER_TRACK_RESOURCES, uint32(1) << (m_modifier.m_miscvalue-1));
}

void Aura::HandleAuraTrackStealthed(bool apply, bool /*Real*/)
{
    if(GetTarget()->GetTypeId()!=TYPEID_PLAYER)
        return;

    if(apply)
        GetTarget()->RemoveNoStackAurasDueToAuraHolder(GetHolder());

    GetTarget()->ApplyModByteFlag(PLAYER_FIELD_BYTES, 0, PLAYER_FIELD_BYTE_TRACK_STEALTHED, apply);
}

void Aura::HandleAuraModScale(bool apply, bool /*Real*/)
{
    GetTarget()->ApplyPercentModFloatValue(OBJECT_FIELD_SCALE_X, float(m_modifier.m_amount), apply);
    GetTarget()->UpdateModelData();
}

void Aura::HandleModPossess(bool apply, bool Real)
{
    if(!Real)
        return;

    Unit *target = GetTarget();

    Unit* caster = GetCaster();
    if(!caster || caster->GetTypeId() != TYPEID_PLAYER)
        return;

    if (GetSpellProto()->Id == 126)
    {
        target = caster->FindGuardianWithEntry(4277);
        if (!target)
            return;
    }

    // not possess yourself
    if (GetCasterGuid() == target->GetObjectGuid())
        return;

    Player* p_caster = (Player*)caster;
    Camera& camera = p_caster->GetCamera();

    if( apply )
    {
        target->addUnitState(UNIT_STAT_CONTROLLED);

        target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);
        target->SetCharmerGuid(p_caster->GetObjectGuid());
        target->setFaction(p_caster->getFaction());

        // target should became visible at SetView call(if not visible before):
        // otherwise client\p_caster will ignore packets from the target(SetClientControl for example)
        camera.SetView(target);

        p_caster->SetCharm(target);
        p_caster->SetClientControl(target, 1);
        p_caster->SetMover(target);

        target->CombatStop(true);
        target->DeleteThreatList();
        target->getHostileRefManager().deleteReferences();

        if (CharmInfo* charmInfo = target->InitCharmInfo(target))
        {
            charmInfo->SetState(CHARM_STATE_REACT, REACT_PASSIVE);
            charmInfo->SetState(CHARM_STATE_COMMAND, COMMAND_STAY);
            charmInfo->InitPossessCreateSpells();
        }

        p_caster->PossessSpellInitialize();

        if(target->GetTypeId() == TYPEID_UNIT)
        {
            ((Creature*)target)->AIM_Initialize();
        }
        else if(target->GetTypeId() == TYPEID_PLAYER && !target->GetVehicle())
        {
            ((Player*)target)->SetClientControl(target, 0);
        }
    }
    else
    {
        p_caster->SetCharm(NULL);

        p_caster->SetClientControl(target, 0);
        p_caster->SetMover(NULL);

        // there is a possibility that target became invisible for client\p_caster at ResetView call:
        // it must be called after movement control unapplying, not before! the reason is same as at aura applying
        camera.ResetView();

        p_caster->RemovePetActionBar();

        // on delete only do caster related effects
        if(m_removeMode == AURA_REMOVE_BY_DELETE)
            return;

        target->clearUnitState(UNIT_STAT_CONTROLLED);

        target->CombatStop(true);
        target->DeleteThreatList();
        target->getHostileRefManager().deleteReferences();

        target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);

        target->SetCharmerGuid(ObjectGuid());

        if(target->GetTypeId() == TYPEID_PLAYER && !target->GetVehicle())
        {
            ((Player*)target)->setFactionForRace(target->getRace());
            ((Player*)target)->SetClientControl(target, 1);
        }
        else if(target->GetTypeId() == TYPEID_UNIT)
        {
            CreatureInfo const *cinfo = ((Creature*)target)->GetCreatureInfo();
            target->setFaction(cinfo->faction_A);
        }

        if(target->GetTypeId() == TYPEID_UNIT)
        {
            ((Creature*)target)->AIM_Initialize();
            if (target->IsHostileTo(caster))
                target->AttackedBy(caster);
        }
    }

    // Mind Control
    if (!apply && GetSpellProto()->Id == 605)
        caster->FinishSpell(CURRENT_CHANNELED_SPELL);
}

void Aura::HandleRangedAPBonus(bool apply, bool Real)
{
    if(!Real)
        return;

    Unit* caster = GetCaster();
    if (!caster)
        return;

    SpellClassOptionsEntry const * opt = GetSpellProto()->GetSpellClassOptions();
    if (apply)
    {
        switch(GetSpellProto()->GetSpellFamilyName())
        {
            case SPELLFAMILY_HUNTER:
                break;
        }
    }
}

void Aura::HandleModPossessPet(bool apply, bool Real)
{
    if(!Real)
        return;

    Unit* caster = GetCaster();
    if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
        return;

    Unit* target = GetTarget();
    if (target->GetTypeId() != TYPEID_UNIT || !((Creature*)target)->IsPet())
        return;

    Pet* pet = (Pet*)target;

    Player* p_caster = (Player*)caster;
    Camera& camera = p_caster->GetCamera();

    if (apply)
    {
        pet->addUnitState(UNIT_STAT_CONTROLLED);

        // target should became visible at SetView call(if not visible before):
        // otherwise client\p_caster will ignore packets from the target(SetClientControl for example)
        camera.SetView(pet);

        p_caster->SetCharm(pet);
        p_caster->SetClientControl(pet, 1);
        ((Player*)caster)->SetMover(pet);

        pet->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);

        pet->StopMoving();
        pet->GetMotionMaster()->Clear(false);
        pet->GetMotionMaster()->MoveIdle();
    }
    else
    {
        p_caster->SetCharm(NULL);
        p_caster->SetClientControl(pet, 0);
        p_caster->SetMover(p_caster);

        // there is a possibility that target became invisible for client\p_caster at ResetView call:
        // it must be called after movement control unapplying, not before! the reason is same as at aura applying
        camera.ResetView();

        // on delete only do caster related effects
        if(m_removeMode == AURA_REMOVE_BY_DELETE)
            return;

        pet->clearUnitState(UNIT_STAT_CONTROLLED);

        pet->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PLAYER_CONTROLLED);

        pet->AttackStop();

        // out of range pet dismissed
        if (!pet->IsWithinDistInMap(p_caster, pet->GetMap()->GetVisibilityDistance()))
        {
            p_caster->RemovePet(PET_SAVE_REAGENTS);
        }
        else
        {
            pet->GetMotionMaster()->MoveFollow(caster, PET_FOLLOW_DIST, PET_FOLLOW_ANGLE);
        }
    }
}

void Aura::HandleAuraModPetTalentsPoints(bool /*Apply*/, bool Real)
{
    if(!Real)
        return;

    // Recalculate pet talent points
    if (Pet *pet=GetTarget()->GetPet())
        pet->InitTalentForLevel();
}

void Aura::HandleModCharm(bool apply, bool Real)
{
    if(!Real)
        return;

    Unit *target = GetTarget();

    // not charm yourself
    if (GetCasterGuid() == target->GetObjectGuid())
        return;

    Unit* caster = GetCaster();
    if(!caster)
        return;

    if( apply )
    {
        // is it really need after spell check checks?
        target->RemoveSpellsCausingAura(SPELL_AURA_MOD_CHARM, GetHolder());
        target->RemoveSpellsCausingAura(SPELL_AURA_MOD_POSSESS, GetHolder());

        target->SetCharmerGuid(GetCasterGuid());
        target->setFaction(caster->getFaction());
        target->CastStop(target == caster ? GetId() : 0);
        caster->SetCharm(target);

        target->CombatStop(true);
        target->DeleteThreatList();
        target->getHostileRefManager().deleteReferences();

        if(target->GetTypeId() == TYPEID_UNIT)
        {
            ((Creature*)target)->AIM_Initialize();
            CharmInfo *charmInfo = target->InitCharmInfo(target);
            charmInfo->SetState(CHARM_STATE_REACT, REACT_DEFENSIVE);
            charmInfo->InitCharmCreateSpells();

            if(caster->GetTypeId() == TYPEID_PLAYER && caster->getClass() == CLASS_WARLOCK)
            {
                CreatureInfo const *cinfo = ((Creature*)target)->GetCreatureInfo();
                if(cinfo && cinfo->type == CREATURE_TYPE_DEMON)
                {
                    // creature with pet number expected have class set
                    if(target->GetByteValue(UNIT_FIELD_BYTES_0, 1)==0)
                    {
                        if(cinfo->unit_class==0)
                            sLog.outErrorDb("Creature (Entry: %u) have unit_class = 0 but used in charmed spell, that will be result client crash.",cinfo->Entry);
                        else
                            ERROR_LOG("Creature (Entry: %u) have unit_class = %u but at charming have class 0!!! that will be result client crash.",cinfo->Entry,cinfo->unit_class);

                        target->SetByteValue(UNIT_FIELD_BYTES_0, 1, CLASS_MAGE);
                    }

                    //just to enable stat window
                    charmInfo->SetPetNumber(sObjectMgr.GeneratePetNumber(), true);
                    //if charmed two demons the same session, the 2nd gets the 1st one's name
                    target->SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, uint32(time(NULL)));
                }
            }
        }

        if(caster->GetTypeId() == TYPEID_PLAYER)
            ((Player*)caster)->CharmSpellInitialize();
    }
    else
    {
        target->SetCharmerGuid(ObjectGuid());

        if(target->GetTypeId() == TYPEID_PLAYER)
            ((Player*)target)->setFactionForRace(target->getRace());
        else
        {
            CreatureInfo const *cinfo = ((Creature*)target)->GetCreatureInfo();

            // restore faction
            if(((Creature*)target)->IsPet())
            {
                if(Unit* owner = target->GetOwner())
                    target->setFaction(owner->getFaction());
                else if(cinfo)
                    target->setFaction(cinfo->faction_A);
            }
            else if(cinfo)                              // normal creature
                target->setFaction(cinfo->faction_A);

            // restore UNIT_FIELD_BYTES_0
            if(cinfo && caster->GetTypeId() == TYPEID_PLAYER && caster->getClass() == CLASS_WARLOCK && cinfo->type == CREATURE_TYPE_DEMON)
            {
                // DB must have proper class set in field at loading, not req. restore, including workaround case at apply
                // GetTarget()->SetByteValue(UNIT_FIELD_BYTES_0, 1, cinfo->unit_class);

                if(target->GetCharmInfo())
                    target->GetCharmInfo()->SetPetNumber(0, true);
                else
                    ERROR_LOG("Aura::HandleModCharm: target (GUID: %u TypeId: %u) has a charm aura but no charm info!", target->GetGUIDLow(), target->GetTypeId());
            }
        }

        caster->SetCharm(NULL);

        if(caster->GetTypeId() == TYPEID_PLAYER)
            ((Player*)caster)->RemovePetActionBar();

        target->CombatStop(true);
        target->DeleteThreatList();
        target->getHostileRefManager().deleteReferences();

        if(target->GetTypeId() == TYPEID_UNIT)
        {
            ((Creature*)target)->AIM_Initialize();
            target->AttackedBy(caster);
        }
    }
}

void Aura::HandleModConfuse(bool apply, bool Real)
{
    if(!Real)
        return;

    GetTarget()->SetConfused(apply, GetCasterGuid(), GetId());
}

void Aura::HandleModFear(bool apply, bool Real)
{
    if (!Real)
        return;

    GetTarget()->SetFeared(apply, GetCasterGuid(), GetId());
}

void Aura::HandleFeignDeath(bool apply, bool Real)
{
    if(!Real)
        return;

    Unit* target = GetTarget();

    if (apply)
    {
        Spell::UnitList targets;
        MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck u_check(target, target, target->GetMap()->GetVisibilityDistance());
        MaNGOS::UnitListSearcher<MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck> searcher(targets, u_check);
        Cell::VisitAllObjects(target, searcher, target->GetMap()->GetVisibilityDistance());

        for (Spell::UnitList::iterator tIter = targets.begin(); tIter != targets.end(); ++tIter)
        {
            for (uint32 i = CURRENT_MELEE_SPELL; i < CURRENT_MAX_SPELL; ++i)
            {
                if ((*tIter)->GetCurrentSpell(CurrentSpellTypes(i))
                    && (*tIter)->GetCurrentSpell(CurrentSpellTypes(i))->m_targets.getUnitTargetGuid() == target->GetObjectGuid())
                {
                    (*tIter)->InterruptSpell(CurrentSpellTypes(i), false);
                    (*tIter)->AttackStop();

                    if ((*tIter)->GetTypeId() == TYPEID_PLAYER && (*tIter)->GetTargetGuid() == target->GetObjectGuid())
                    {
                        WorldPacket data(SMSG_CLEAR_TARGET, 8);
                        data << (*tIter)->GetObjectGuid();
                        (*tIter)->SendMessageToSet(&data, false);
                    }
                }
            }
        }
    }

    target->SetFeignDeath(apply, GetCasterGuid(), GetId());
}

void Aura::HandleAuraModDisarm(bool apply, bool Real)
{
    if(!Real)
        return;

    Unit *target = GetTarget();
    AuraType type = m_modifier.m_auraname;
    bool loading = target->GetTypeId() == TYPEID_PLAYER && ((Player*)target)->GetSession()->PlayerLoading();

    if (apply ? target->GetAurasByType(type).size() > 1 : target->HasAuraType(type))
        return;

    uint32 field, flags, slot;
    WeaponAttackType attType;
 
    switch (type)
    {
        case SPELL_AURA_MOD_DISARM:
            field = UNIT_FIELD_FLAGS;
            flags = UNIT_FLAG_DISARMED;
            slot = EQUIPMENT_SLOT_MAINHAND;
            attType = BASE_ATTACK;
            break;
        case SPELL_AURA_MOD_DISARM_OFFHAND:
            field = UNIT_FIELD_FLAGS_2;
            flags = UNIT_FLAG2_DISARM_OFFHAND;
            slot = EQUIPMENT_SLOT_OFFHAND;
            attType = OFF_ATTACK;
            break;
        case SPELL_AURA_MOD_DISARM_RANGED:
            field = UNIT_FIELD_FLAGS_2;
            flags = UNIT_FLAG2_DISARM_RANGED;
            slot = EQUIPMENT_SLOT_RANGED;
            attType = RANGED_ATTACK;
            break;
        default:
            return;
    }
 
    target->ApplyModFlag(field, flags, apply);

    //if (apply)
    //    target->SetFlag(field, flag);
 
    //if (target->GetTypeId() == TYPEID_PLAYER && !loading)
    //{
    //    //Amaru: yoa oooea aie?ia auou oi?ii ia?ao Set e Remove
    //    if (Item *pItem = ((Player*)target)->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
    //        ((Player*)target)->_ApplyItemMods(pItem, slot, !apply);
    //}

    //if (!apply)
    //    target->RemoveFlag(field, flag);

    if (target->GetTypeId() != TYPEID_PLAYER)
        return;
 
    // main-hand attack speed was set to special value for feral form already and must not be changed and reset at remove.
    if (!target->IsInFeralForm())
    {
        if (apply)
            target->SetAttackTime(attType, BASE_ATTACK_TIME);
        else
            ((Player*)target)->SetRegularAttackTime();
    }

    // Bladestorm vs. Disarm hack: cancel Bladestorm aura on apply
    // perhaps should be done likewise to all auras dependent on weapons?
    if (apply && ((Player*)target)->getClass() == CLASS_WARRIOR && target->HasAura(46924))
        target->RemoveAurasDueToSpell(46924);

    ((Player*)target)->UpdateDamagePhysical(attType);
}

void Aura::HandleAuraModStun(bool apply, bool Real)
{
    if(!Real)
        return;

    Unit *target = GetTarget();

    if (apply)
    {
        // Frost stun aura -> freeze/unfreeze target
        if (GetSpellSchoolMask(GetSpellProto()) & SPELL_SCHOOL_MASK_FROST)
            target->ModifyAuraState(AURA_STATE_FROZEN, apply);

        target->addUnitState(UNIT_STAT_STUNNED);
        target->SetTargetGuid(ObjectGuid());

        target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);
        target->CastStop(target->GetObjectGuid() == GetCasterGuid() ? GetId() : 0);

        // Creature specific
        if(target->GetTypeId() != TYPEID_PLAYER)
            target->StopMoving();
        else
        {
            ((Player*)target)->m_movementInfo.SetMovementFlags(MOVEFLAG_NONE);
            target->SetStandState(UNIT_STAND_STATE_STAND);// in 1.5 client
        }

        WorldPacket data;
        target->BuildForceMoveRootPacket(&data, true, 0);
        target->SendMessageToSet(&data, true);

        // Summon the Naj'entus Spine GameObject on target if spell is Impaling Spine
        if (GetId() == 39837)
        {
            GameObject* pObj = new GameObject;
            if(pObj->Create(target->GetMap()->GenerateLocalLowGuid(HIGHGUID_GAMEOBJECT), 185584, target->GetMap(), target->GetPhaseMask(),
                target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation()))
            {
                pObj->SetRespawnTime(GetAuraDuration()/IN_MILLISECONDS);
                pObj->SetSpellId(GetId());
                target->AddGameObject(pObj);
                target->GetMap()->Add(pObj);
            }
            else
                delete pObj;
        }
    }
    else
    {
        // Frost stun aura -> freeze/unfreeze target
        if (GetSpellSchoolMask(GetSpellProto()) & SPELL_SCHOOL_MASK_FROST)
        {
            bool found_another = false;
            for(AuraType const* itr = &frozenAuraTypes[0]; *itr != SPELL_AURA_NONE; ++itr)
            {
                Unit::AuraList const& auras = target->GetAurasByType(*itr);
                for(Unit::AuraList::const_iterator i = auras.begin(); i != auras.end(); ++i)
                {
                    if( GetSpellSchoolMask((*i)->GetSpellProto()) & SPELL_SCHOOL_MASK_FROST)
                    {
                        found_another = true;
                        break;
                    }
                }
                if(found_another)
                    break;
            }

            if(!found_another)
                target->ModifyAuraState(AURA_STATE_FROZEN, apply);
        }

        // Real remove called after current aura remove from lists, check if other similar auras active
        if(target->HasAuraType(SPELL_AURA_MOD_STUN))
            return;

        target->clearUnitState(UNIT_STAT_STUNNED);
        target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED);

        if(!target->hasUnitState(UNIT_STAT_ROOT | UNIT_STAT_ON_VEHICLE))       // prevent allow move if have also root effect
        {
            if(target->getVictim() && target->isAlive())
                target->SetTargetGuid(target->getVictim()->GetObjectGuid());

            WorldPacket data;
            target->BuildForceMoveRootPacket(&data, false, 0);
            target->SendMessageToSet(&data, true);
        }

        // Wyvern Sting
        if (GetId() == 19386)
        {
            Unit* caster = GetCaster();
            if (!caster)
                return;

            caster->CastSpell(target, 24131, true, NULL, this);
            return;
        }
    }

    // Seduction (Succubus spell)
    if (GetId() == 6358)
    {
        Unit* pCaster = GetCaster();
        if (!pCaster)
            return;

        if (apply)
        {
            if(pCaster->GetOwner() && target->isAlive() && pCaster->GetOwner()->HasAura(56250)) // Glyph of Succubus
            {
                // except shadow word: death periodic
                target->RemoveSpellsCausingAura(SPELL_AURA_PERIODIC_DAMAGE, target->GetSpellAuraHolder(32409));
                target->RemoveSpellsCausingAura(SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
                target->RemoveSpellsCausingAura(SPELL_AURA_PERIODIC_LEECH);
            }
        }
    }
    // Ritual Preparation
    else if (GetId() == 48267)
    {
        if (!apply)
        {
            target->CastSpell(target, 48271, true);    // Target Summon Banshee
            target->CastSpell(target, 48274, true);    // Target Summon Banshee
            target->CastSpell(target, 48275, true);    // Target Summon Banshee
        }
    }
    // Ring of Frost
    else if (GetId() == 82691)
    {
        if (!apply)
            if (Unit* caster = GetCaster())
                caster->CastSpell(target, 91264, true); // 3sec immune spell
    }
}

void Aura::HandleModStealth(bool apply, bool Real)
{
    Unit *target = GetTarget();

    SpellClassOptionsEntry const* classOptions = GetSpellProto()->GetSpellClassOptions();

    if (apply)
    {
        // drop flag at stealth in bg
        target->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_IMMUNE_OR_LOST_SELECTION);

        // only at real aura add
        if (Real)
        {
            target->SetStandFlags(UNIT_STAND_FLAGS_CREEP);

            if (target->GetTypeId()==TYPEID_PLAYER)
                target->SetByteFlag(PLAYER_FIELD_BYTES2, 3, PLAYER_FIELD_BYTE2_STEALTH);

            // apply only if not in GM invisibility (and overwrite invisibility state)
            if (target->GetVisibility()!=VISIBILITY_OFF)
            {
                target->SetVisibility(VISIBILITY_GROUP_NO_DETECT);
                target->SetVisibility(VISIBILITY_GROUP_STEALTH);
            }

            // apply full stealth period bonuses only at first stealth aura in stack
            if(target->GetAurasByType(SPELL_AURA_MOD_STEALTH).size()<=1)
            {
                Unit::AuraList const& mDummyAuras = target->GetAurasByType(SPELL_AURA_DUMMY);
                for(Unit::AuraList::const_iterator i = mDummyAuras.begin();i != mDummyAuras.end(); ++i)
                {
                    // Master of Subtlety
                    if ((*i)->GetSpellProto()->SpellIconID == 2114)
                    {
                        target->RemoveAurasDueToSpell(31666);
                        int32 bp = (*i)->GetModifier()->m_amount;
                        target->CastCustomSpell(target,31665,&bp,NULL,NULL,true);
                    }
                    // Overkill
                    else if ((*i)->GetId() == 58426 && classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x0000000000400000))
                    {
                        target->CastSpell(target, 58427, true);
                    }
                }
            }
        }
    }
    else
    {
        // Vanish
        if (Real && GetId() == 11327 && m_removeMode == AURA_REMOVE_BY_EXPIRE)
        {
            // cast Stealth at remove
            if (target->GetTypeId() == TYPEID_PLAYER)
                if (((Player*)target)->HasSpellCooldown(1784))
                    ((Player*)target)->RemoveSpellCooldown(1784);

            target->_AddAura(1784, 0);
        }

        // only at real aura remove of _last_ SPELL_AURA_MOD_STEALTH
        if (Real && !target->HasAuraType(SPELL_AURA_MOD_STEALTH))
        {
            // if no GM invisibility
            if (target->GetVisibility()!=VISIBILITY_OFF)
            {
                target->RemoveStandFlags(UNIT_STAND_FLAGS_CREEP);

                if (target->GetTypeId()==TYPEID_PLAYER)
                    target->RemoveByteFlag(PLAYER_FIELD_BYTES2, 3, PLAYER_FIELD_BYTE2_STEALTH);

                // restore invisibility if any
                if (target->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                {
                    target->SetVisibility(VISIBILITY_GROUP_NO_DETECT);
                    target->SetVisibility(VISIBILITY_GROUP_INVISIBILITY);
                }
                else
                    target->SetVisibility(VISIBILITY_ON);
            }

            // apply delayed talent bonus remover at last stealth aura remove
            Unit::AuraList const& mDummyAuras = target->GetAurasByType(SPELL_AURA_DUMMY);
            for(Unit::AuraList::const_iterator i = mDummyAuras.begin();i != mDummyAuras.end(); ++i)
            {
                // Master of Subtlety
                if ((*i)->GetSpellProto()->SpellIconID == 2114)
                    target->CastSpell(target, 31666, true);
                // Overkill
                else if ((*i)->GetId() == 58426 && classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x0000000000400000))
                {
                    if (SpellAuraHolder* holder = target->GetSpellAuraHolder(58427))
                    {
                        holder->SetAuraMaxDuration(20*IN_MILLISECONDS);
                        holder->RefreshHolder();
                    }
                }
            }
        }
    }
}

void Aura::HandleInvisibility(bool apply, bool Real)
{
    Unit *target = GetTarget();

    if (apply)
    {
        target->m_invisibilityMask |= (1 << m_modifier.m_miscvalue);

        target->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_IMMUNE_OR_LOST_SELECTION);

        //Amaru: this will cancel food aura on entering invisibility
        target->SetStandState(UNIT_STAND_STATE_STAND);

        if(Real && target->GetTypeId()==TYPEID_PLAYER && GetId() != SPELL_ARENA_PREPARATION)
        {
            // apply glow vision
            target->SetByteFlag(PLAYER_FIELD_BYTES2, 3, PLAYER_FIELD_BYTE2_INVISIBILITY_GLOW);
        }

        // apply only if not in GM invisibility and not stealth
        if (target->GetVisibility() == VISIBILITY_ON)
        {
            // Aura not added yet but visibility code expect temporary add aura
            target->SetVisibility(VISIBILITY_GROUP_NO_DETECT);
            target->SetVisibility(VISIBILITY_GROUP_INVISIBILITY);
        }
    }
    else
    {
        // recalculate value at modifier remove (current aura already removed)
        target->m_invisibilityMask = 0;
        Unit::AuraList const& auras = target->GetAurasByType(SPELL_AURA_MOD_INVISIBILITY);
        for(Unit::AuraList::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
            target->m_invisibilityMask |= (1 << (*itr)->GetModifier()->m_miscvalue);

        // only at real aura remove and if not have different invisibility auras.
        if (Real && target->m_invisibilityMask == 0)
        {
            // remove glow vision
            if (target->GetTypeId() == TYPEID_PLAYER)
                target->RemoveByteFlag(PLAYER_FIELD_BYTES2, 3, PLAYER_FIELD_BYTE2_INVISIBILITY_GLOW);

            // apply only if not in GM invisibility & not stealthed while invisible
            if (target->GetVisibility() != VISIBILITY_OFF)
            {
                // if have stealth aura then already have stealth visibility
                if (!target->HasAuraType(SPELL_AURA_MOD_STEALTH))
                    target->SetVisibility(VISIBILITY_ON);
            }
        }

        // Binding Life
        if (GetId() == 48809)
            target->CastSpell(target, GetSpellProto()->CalculateSimpleValue(m_effIndex), true);

        // Racer Slam, death scene: car flip Parent
        if (GetId() == 49337 && target->GetTypeId() == TYPEID_UNIT) 
            ((Creature*)target)->ForcedDespawn();
    }
}

void Aura::HandleInvisibilityDetect(bool apply, bool Real)
{
    Unit *target = GetTarget();

    if(apply)
    {
        target->m_detectInvisibilityMask |= (1 << m_modifier.m_miscvalue);
    }
    else
    {
        // recalculate value at modifier remove (current aura already removed)
        target->m_detectInvisibilityMask = 0;
        Unit::AuraList const& auras = target->GetAurasByType(SPELL_AURA_MOD_INVISIBILITY_DETECTION);
        for(Unit::AuraList::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
            target->m_detectInvisibilityMask |= (1 << (*itr)->GetModifier()->m_miscvalue);
    }
    if(Real && target->GetTypeId()==TYPEID_PLAYER)
        ((Player*)target)->GetCamera().UpdateVisibilityForOwner();
}

void Aura::HandleDetectAmore(bool apply, bool /*real*/)
{
    GetTarget()->ApplyModByteFlag(PLAYER_FIELD_BYTES2, 3, (PLAYER_FIELD_BYTE2_DETECT_AMORE_0 << m_modifier.m_amount), apply);
}

void Aura::HandleAuraModRoot(bool apply, bool Real)
{
    // only at real add/remove aura
    if(!Real)
        return;

    Unit *target = GetTarget();

    if (apply)
    {
        // Frost root aura -> freeze/unfreeze target
        if (GetSpellSchoolMask(GetSpellProto()) & SPELL_SCHOOL_MASK_FROST)
            target->ModifyAuraState(AURA_STATE_FROZEN, apply);

        target->addUnitState(UNIT_STAT_ROOT);
        target->SetTargetGuid(ObjectGuid());

        //Save last orientation
        if( target->getVictim() )
            target->SetOrientation(target->GetAngle(target->getVictim()));

        if(target->GetTypeId() == TYPEID_PLAYER)
        {
            WorldPacket data;
            target->BuildForceMoveRootPacket(&data, true, 2);
            target->SendMessageToSet(&data, true);

            //Clear unit movement flags
            ((Player*)target)->m_movementInfo.SetMovementFlags(MOVEFLAG_NONE);
        }
        else
            target->StopMoving();
    }
    else
    {
        // Frost root aura -> freeze/unfreeze target
        if (GetSpellSchoolMask(GetSpellProto()) & SPELL_SCHOOL_MASK_FROST)
        {
            bool found_another = false;
            for(AuraType const* itr = &frozenAuraTypes[0]; *itr != SPELL_AURA_NONE; ++itr)
            {
                Unit::AuraList const& auras = target->GetAurasByType(*itr);
                for(Unit::AuraList::const_iterator i = auras.begin(); i != auras.end(); ++i)
                {
                    if( GetSpellSchoolMask((*i)->GetSpellProto()) & SPELL_SCHOOL_MASK_FROST)
                    {
                        found_another = true;
                        break;
                    }
                }
                if(found_another)
                    break;
            }

            if(!found_another)
                target->ModifyAuraState(AURA_STATE_FROZEN, apply);
        }

        // Real remove called after current aura remove from lists, check if other similar auras active
        if(target->HasAuraType(SPELL_AURA_MOD_ROOT))
            return;

        target->clearUnitState(UNIT_STAT_ROOT);

        if(!target->hasUnitState(UNIT_STAT_STUNNED | UNIT_STAT_ON_VEHICLE))      // prevent allow move if have also stun effect
        {
            if(target->getVictim() && target->isAlive())
                target->SetTargetGuid(target->getVictim()->GetObjectGuid());

            if(target->GetTypeId() == TYPEID_PLAYER)
            {
                WorldPacket data;
                target->BuildForceMoveRootPacket(&data, false, 2);
                target->SendMessageToSet(&data, true);
            }
        }

        if (GetSpellProto()->Id == 70980)                   // Web Wrap (Icecrown Citadel, trash mob Nerub'ar Broodkeeper)
            target->CastSpell(target, 71010, true);
    }
}

void Aura::HandleAuraModSilence(bool apply, bool Real)
{
    // only at real add/remove aura
    if(!Real)
        return;

    Unit *target = GetTarget();

    if(apply)
    {
        target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
        // Stop cast only spells vs PreventionType == SPELL_PREVENTION_TYPE_SILENCE
        for (uint32 i = CURRENT_MELEE_SPELL; i < CURRENT_MAX_SPELL; ++i)
            if (Spell* spell = target->GetCurrentSpell(CurrentSpellTypes(i)))
                if(spell->m_spellInfo->GetPreventionType() == SPELL_PREVENTION_TYPE_SILENCE)
                    // Stop spells on prepare or casting state
                    target->InterruptSpell(CurrentSpellTypes(i), false);
    }
    else
    {
        // Real remove called after current aura remove from lists, check if other similar auras active
        if(target->HasAuraType(SPELL_AURA_MOD_SILENCE))
            return;

        target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_SILENCED);
    }
}

void Aura::HandleModThreat(bool apply, bool Real)
{
    // only at real add/remove aura
    if (!Real)
        return;

    Unit *target = GetTarget();

    if (!target->isAlive())
        return;

    int level_diff = 0;
    int multiplier = 0;
    switch (GetId())
    {
        // Arcane Shroud
        case 26400:
            level_diff = target->getLevel() - 60;
            multiplier = 2;
            break;
        // The Eye of Diminution
        case 28862:
            level_diff = target->getLevel() - 60;
            multiplier = 1;
            break;
    }

    if (level_diff > 0)
        m_modifier.m_amount += multiplier * level_diff;

    if (target->GetTypeId() == TYPEID_PLAYER)
        for (int8 x = 0; x < MAX_SPELL_SCHOOL; ++x)
            if (m_modifier.m_miscvalue & int32(1<<x))
                ApplyPercentModFloatVar(target->m_threatModifier[x], float(m_modifier.m_amount), apply);
}

void Aura::HandleAuraModTotalThreat(bool apply, bool Real)
{
    // only at real add/remove aura
    if (!Real)
        return;

    Unit *target = GetTarget();

    if (!target->isAlive() || target->GetTypeId() != TYPEID_PLAYER)
        return;

    Unit* caster = GetCaster();

    if (!caster || !caster->isAlive())
        return;

    // Glyph of Salvation
    if (apply && GetId() == 1038 && caster->HasAura(63225))
        m_modifier.m_amount = -100;

    float threatMod = apply ? float(m_modifier.m_amount) : float(-m_modifier.m_amount);

    target->getHostileRefManager().threatAssist(caster, threatMod, GetSpellProto());
}

void Aura::HandleModTaunt(bool apply, bool Real)
{
    // only at real add/remove aura
    if (!Real)
        return;

    Unit *target = GetTarget();

    if (!target->isAlive() || !target->CanHaveThreatList())
        return;

    Unit* caster = GetCaster();

    if (!caster || !caster->isAlive())
        return;

    if (apply)
        target->TauntApply(caster);
    else
    {
        // When taunt aura fades out, mob will switch to previous target if current has less than 1.1 * secondthreat
        target->TauntFadeOut(caster);
    }
}

/*********************************************************/
/***                  MODIFY SPEED                     ***/
/*********************************************************/
void Aura::HandleAuraModIncreaseSpeed(bool apply, bool Real)
{
    // all applied/removed only at real aura add/remove
    if (!Real)
        return;

    Unit* target = GetTarget();

    if (apply)
    {
        // Ranger: Spirit Walk - Removes all movement impairing effects
        if (GetId() == 58875)
            target->CastSpell(target, 58876, true);
        // Dash or Stampeding Roar (Bear Form)
        else if (GetId() == 1850 || GetId() == 77761)
        {
            if (Unit* caster = GetCaster())
            {
                if (caster->GetTypeId() == TYPEID_PLAYER)
                {
                    // search Feral Swiftness
                    if (SpellEntry const* talent = ((Player*)caster)->GetKnownTalentRankById(8295))
                        if (roll_chance_i(talent->CalculateSimpleValue(EFFECT_INDEX_1)))
                            // cast Feral Swiftness Clear
                            caster->CastSpell(target, 97985, true);
                }
            }
        }
    }

    target->UpdateSpeed(MOVE_RUN, true);
}

void Aura::HandleAuraModIncreaseMountedSpeed(bool apply, bool Real)
{
    // all applied/removed only at real aura add/remove
    if(!Real)
        return;

    Unit *target = GetTarget();

    target->UpdateSpeed(MOVE_RUN, true);

    // Festive Holiday Mount
    if (apply && GetSpellProto()->SpellIconID != 1794 && target->HasAura(62061))
        // Reindeer Transformation
        target->CastSpell(target, 25860, true, NULL, this);
}

void Aura::HandleAuraModIncreaseFlightSpeed(bool apply, bool Real)
{
    // all applied/removed only at real aura add/remove
    if(!Real)
        return;

    Unit *target = GetTarget();

    // Enable Fly mode for flying mounts
    if (m_modifier.m_auraname == SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED)
    {
        WorldPacket data;
        target->BuildMoveSetCanFlyPacket(&data, apply, 0);
        target->SendMessageToSet(&data, true);

        //Players on flying mounts must be immune to polymorph
        if (target->GetTypeId()==TYPEID_PLAYER)
            target->ApplySpellImmune(GetId(),IMMUNITY_MECHANIC,MECHANIC_POLYMORPH,apply);

        // Dragonmaw Illusion (overwrite mount model, mounted aura already applied)
        if (apply && target->HasAura(42016, EFFECT_INDEX_0) && target->GetMountID())
            target->SetUInt32Value(UNIT_FIELD_MOUNTDISPLAYID,16314);

        // Festive Holiday Mount
        if (apply && GetSpellProto()->SpellIconID != 1794 && target->HasAura(62061))
            // Reindeer Transformation
            target->CastSpell(target, 25860, true, NULL, this);
    }

    // Swift Flight Form check for higher speed flying mounts
    if (apply && target->GetTypeId() == TYPEID_PLAYER && GetSpellProto()->Id == 40121)
    {
        for (PlayerSpellMap::const_iterator iter = ((Player*)target)->GetSpellMap().begin(); iter != ((Player*)target)->GetSpellMap().end(); ++iter)
        {
            if (iter->second.state != PLAYERSPELL_REMOVED)
            {
                bool changedSpeed = false;
                SpellEntry const *spellInfo = sSpellStore.LookupEntry(iter->first);
                for(int i = 0; i < MAX_EFFECT_INDEX; ++i)
                {
                    SpellEffectEntry const* effectEntry = spellInfo->GetSpellEffect(SpellEffectIndex(i));
                    if(effectEntry && effectEntry->EffectApplyAuraName == SPELL_AURA_MOD_FLIGHT_SPEED_MOUNTED)
                    {
                        int32 mountSpeed = spellInfo->CalculateSimpleValue(SpellEffectIndex(i));
                        if (mountSpeed > m_modifier.m_amount)
                        {
                            m_modifier.m_amount = mountSpeed;
                            changedSpeed = true;
                            break;
                        }
                    }
                }
                if (changedSpeed)
                    break;
            }
        }
    }

    target->UpdateSpeed(MOVE_FLIGHT, true);
}

void Aura::HandleAuraModIncreaseSwimSpeed(bool /*apply*/, bool Real)
{
    // all applied/removed only at real aura add/remove
    if(!Real)
        return;

    GetTarget()->UpdateSpeed(MOVE_SWIM, true);
}

void Aura::HandleAuraModDecreaseSpeed(bool apply, bool Real)
{
    // all applied/removed only at real aura add/remove
    if (!Real)
        return;

    Unit* target = GetTarget();

    if (apply)
    {
        // Gronn Lord's Grasp, becomes stoned
        if (GetId() == 33572)
        {
            if (GetStackAmount() >= 5 && !target->HasAura(33652))
                target->CastSpell(target, 33652, true);
        }
        // Chains of Ice
        else if (GetId() == 45524)
        {
            if (Unit* caster = GetCaster())
            {
                // Chilblains (Rank 1)
                if (caster->HasAura(50040))
                    caster->CastSpell(target, 96293, true);
                // Chilblains (Rank 2)
                else if (caster->HasAura(50041))
                    caster->CastSpell(target, 96294, true);
            }
        }
    }

    target->UpdateSpeed(MOVE_RUN, true);
    target->UpdateSpeed(MOVE_SWIM, true);
    target->UpdateSpeed(MOVE_FLIGHT, true);
}

void Aura::HandleAuraModUseNormalSpeed(bool /*apply*/, bool Real)
{
    // all applied/removed only at real aura add/remove
    if(!Real)
        return;

    Unit *target = GetTarget();

    target->UpdateSpeed(MOVE_RUN, true);
    target->UpdateSpeed(MOVE_SWIM, true);
    target->UpdateSpeed(MOVE_FLIGHT, true);
}

/*********************************************************/
/***                     IMMUNITY                      ***/
/*********************************************************/

void Aura::HandleModMechanicImmunity(bool apply, bool /*Real*/)
{
    uint32 misc  = m_modifier.m_miscvalue;

    Unit* target = GetTarget();
    uint32 mechanicMask = 1 << (misc-1);

    if (GetSpellProto()->HasAttribute(SPELL_ATTR_EX_DISPEL_AURAS_ON_IMMUNITY))
    {
        // immune movement impairment and loss of control (spell data have special structure for mark this case)
        if (IsSpellRemoveAllMovementAndControlLossEffects(GetSpellProto()))
            mechanicMask = IMMUNE_TO_MOVEMENT_IMPAIRMENT_AND_LOSS_CONTROL_MASK;

        // Demonic Empowerment (Felguard)
        if (GetId() == 54508)
            mechanicMask = (1<<(MECHANIC_ROOT-1))|(1<<(MECHANIC_SNARE-1))|(1<<(MECHANIC_STUN-1))|
            (1<<(MECHANIC_BANISH-1))|(1<<(MECHANIC_HORROR-1))|(1<<(MECHANIC_FEAR-1));
        // Bestial Wrath
        else if (GetId() == 19574)
            mechanicMask = IMMUNE_TO_MOVEMENT_IMPAIRMENT_AND_LOSS_CONTROL_MASK;

        if (apply)
            target->RemoveAurasAtMechanicImmunity(mechanicMask, GetId());
    }

    // The Beast Within cast on owner if talent present
    if (GetId() == 19574)
    {
        if (Unit* owner = target->GetOwner())
        {
            // Search talent The Beast Within
            if (apply && owner->HasAura(34692))
                owner->CastSpell(owner, 34471, true, NULL, this);
            else
                owner->RemoveAurasDueToSpell(34471);
        }

        mechanicMask = IMMUNE_TO_MOVEMENT_IMPAIRMENT_AND_LOSS_CONTROL_MASK | (1 << (MECHANIC_DISARM - 1));

        target->ApplySpellImmune(GetId(), IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, apply);
    }
    // Demonic Circle: Teleport
    else if (GetId() == 48020)
    {
        if (target->GetTypeId() != TYPEID_PLAYER)
            return;

        if (apply)
        {
            if (GameObject* obj = target->GetGameObject(48018))
               ((Player*)target)->TeleportTo(obj->GetMapId(), obj->GetPositionX(), obj->GetPositionY(), obj->GetPositionZ(), obj->GetOrientation(), TELE_TO_NOT_LEAVE_COMBAT);
        }
    }
    // Heroic Fury (Intercept cooldown remove)
    else if (GetId() == 60970)
    {
        if (apply && target->GetTypeId() == TYPEID_PLAYER)
            ((Player*)target)->RemoveSpellCooldown(20252, true);
    }

    for (uint8 i = FIRST_MECHANIC - 1; i < MAX_MECHANIC - 1; ++i)
        if (uint8 mech = (mechanicMask >> i) & 1)
            target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, i+1, apply);
}

void Aura::HandleModMechanicImmunityMask(bool apply, bool /*Real*/)
{
    uint32 mechanicMask = m_modifier.m_miscvalue;
    Unit *target = GetTarget();

    if (apply && GetSpellProto()->HasAttribute(SPELL_ATTR_EX_DISPEL_AURAS_ON_IMMUNITY))
        GetTarget()->RemoveAurasAtMechanicImmunity(mechanicMask, GetId());

    // Bladestorm
    if (GetId() == 46924)
    {
        mechanicMask = IMMUNE_TO_MOVEMENT_IMPAIRMENT_AND_LOSS_CONTROL_MASK;
        for (uint8 i = FIRST_MECHANIC - 1; i < MAX_MECHANIC - 1; ++i)
            if (uint8 mech = (mechanicMask >> i) & 1)
                target->ApplySpellImmune(GetId(), IMMUNITY_MECHANIC, i+1, apply);

        target->ApplySpellImmune(GetId(), IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, apply);
    }
    // Pillar of Frost
    else if (GetId() == 51271)
    {
        m_modifier.m_miscvalue = (1 << (MECHANIC_GRIP - 1));

        target->ApplySpellImmune(GetId(), IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, apply);
    }
    // Pillar of Frost (from glyph)
    else if (GetId() == 90259)
    {
        m_modifier.m_miscvalue = IMMUNE_TO_MOVEMENT_IMPAIRMENT_AND_LOSS_CONTROL_MASK;
    }
    // check implemented in Unit::IsImmuneToSpell and Unit::IsImmuneToSpellEffect
}

//this method is called whenever we add / remove aura which gives m_target some imunity to some spell effect
void Aura::HandleAuraModEffectImmunity(bool apply, bool /*Real*/)
{
    Unit *target = GetTarget();

    // when removing flag aura, handle flag drop
    if( !apply && target->GetTypeId() == TYPEID_PLAYER
        && (GetSpellProto()->GetAuraInterruptFlags() & AURA_INTERRUPT_FLAG_IMMUNE_OR_LOST_SELECTION) )
    {
        Player* player = (Player*)target;
        if (BattleGround* bg = player->GetBattleGround())
            bg->EventPlayerDroppedFlag(player);
        else
        {
            if (InstanceData* mapInstance = player->GetInstanceData())
                mapInstance->OnPlayerDroppedFlag(player, GetSpellProto()->Id);

            if (OutdoorPvP* outdoorPvP = sOutdoorPvPMgr.GetScript(player->GetCachedZoneId()))
                outdoorPvP->HandleDropFlag(player, GetSpellProto()->Id);
        }
    }

    target->ApplySpellImmune(GetId(), IMMUNITY_EFFECT, m_modifier.m_miscvalue, apply);
}

void Aura::HandleAuraModStateImmunity(bool apply, bool Real)
{
    if (apply && Real && GetSpellProto()->HasAttribute(SPELL_ATTR_EX_DISPEL_AURAS_ON_IMMUNITY))
    {
        Unit::AuraList const& auraList = GetTarget()->GetAurasByType(AuraType(m_modifier.m_miscvalue));
        for(Unit::AuraList::const_iterator itr = auraList.begin(); itr != auraList.end();)
        {
            if (auraList.front() != this)                   // skip itself aura (it already added)
            {
                GetTarget()->RemoveAurasDueToSpell(auraList.front()->GetId());
                itr = auraList.begin();
            }
            else
                ++itr;
        }
    }

    GetTarget()->ApplySpellImmune(GetId(), IMMUNITY_STATE, m_modifier.m_miscvalue, apply);
}

void Aura::HandleAuraModSchoolImmunity(bool apply, bool Real)
{
    Unit* target = GetTarget();
    target->ApplySpellImmune(GetId(), IMMUNITY_SCHOOL, m_modifier.m_miscvalue, apply);

    // remove all flag auras (they are positive, but they must be removed when you are immune)
    if (GetSpellProto()->HasAttribute(SPELL_ATTR_EX_DISPEL_AURAS_ON_IMMUNITY) && GetSpellProto()->HasAttribute(SPELL_ATTR_EX2_DAMAGE_REDUCED_SHIELD))
        target->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_IMMUNE_OR_LOST_SELECTION);

    // TODO: optimalize this cycle - use RemoveAurasWithInterruptFlags call or something else
    if (Real && apply
        && GetSpellProto()->HasAttribute(SPELL_ATTR_EX_DISPEL_AURAS_ON_IMMUNITY)
        && IsPositiveSpell(GetId()))                        // Only positive immunity removes auras
    {
        std::set<uint32> toRemoveSpellList;
        uint32 school_mask = m_modifier.m_miscvalue;
        Unit::SpellAuraHolderMap& Auras = target->GetSpellAuraHolderMap();
        for(Unit::SpellAuraHolderMap::iterator iter = Auras.begin(); iter != Auras.end(); ++iter)
        {
            if (!iter->second || iter->second->IsDeleted())
                continue;

            SpellEntry const* spell = iter->second->GetSpellProto();

            if ((GetSpellSchoolMask(spell) & school_mask)   //Check for school mask
                && !spell->HasAttribute(SPELL_ATTR_UNAFFECTED_BY_INVULNERABILITY)   //Spells unaffected by invulnerability
                && !iter->second->IsPositive()              //Don't remove positive spells
                && !iter->second->IsPassive()               //Don't remove own talents!
                && spell->Id != GetId() )                   // Don't remove self
            {
                toRemoveSpellList.insert(iter->second->GetId());
            }
        }
        for (std::set<uint32>::iterator i = toRemoveSpellList.begin(); i != toRemoveSpellList.end(); ++i)
            target->RemoveAurasDueToSpell(*i);
    }

    if( Real && GetSpellProto()->GetMechanic() == MECHANIC_BANISH )
    {
        if ( apply )
            target->addUnitState(UNIT_STAT_ISOLATED);
        else
        {
            bool banishFound = false;
            Unit::AuraList const& banishAuras = target->GetAurasByType(m_modifier.m_auraname);
            for(Unit::AuraList::const_iterator itr = banishAuras.begin(); itr != banishAuras.end(); ++itr)
            {
                if ((*itr)->GetSpellProto()->GetMechanic() == MECHANIC_BANISH)
                {
                    banishFound = true;
                    break;
                }
            }

            if (!banishFound)
                target->clearUnitState(UNIT_STAT_ISOLATED);
        }
    }

    if (apply && target->GetTypeId() == TYPEID_PLAYER && target->getClass() == CLASS_WARRIOR)
        target->RemoveAurasDueToSpell(46924);
}

void Aura::HandleAuraModDmgImmunity(bool apply, bool /*Real*/)
{
    GetTarget()->ApplySpellImmune(GetId(), IMMUNITY_DAMAGE, m_modifier.m_miscvalue, apply);
}

void Aura::HandleAuraModDispelImmunity(bool apply, bool Real)
{
    // all applied/removed only at real aura add/remove
    if(!Real)
        return;

    GetTarget()->ApplySpellDispelImmunity(GetSpellProto(), DispelType(m_modifier.m_miscvalue), apply);
}

void Aura::HandleAuraProcTriggerSpell(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();

    switch (GetId())
    {
        // some spell have charges by functionality not have its in spell data
        case 28200:                                         // Ascendance (Talisman of Ascendance trinket)
            if(apply)
                GetHolder()->SetAuraCharges(6);
            break;
        case 64343:                                         // Impact
        {
            if (target->GetTypeId() == TYPEID_PLAYER)
                ((Player*)target)->RemoveSpellCooldown(2136, true);
            break;
        }
        case 75806:                                         // Grand Crusader (Rank 1)
        case 85043:                                         // Grand Crusader (Rank 2)
        {
            if (apply)
            {
                if (target->GetTypeId() != TYPEID_PLAYER)
                    break;

                // remove cooldown on Avenger's Shield
                ((Player*)target)->RemoveSpellCooldown(31935);
            }
            break;
        }
        default:
            break;
    }
}

void Aura::HandleAuraModStalked(bool apply, bool /*Real*/)
{
    // used by spells: Hunter's Mark, Mind Vision, Syndicate Tracker (MURP) DND
    if(apply)
        GetTarget()->SetFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_TRACK_UNIT);
    else
        GetTarget()->RemoveFlag(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_TRACK_UNIT);
}

/*********************************************************/
/***                   PERIODIC                        ***/
/*********************************************************/

void Aura::HandlePeriodicTriggerSpell(bool apply, bool /*Real*/)
{
    m_isPeriodic = apply;

    Unit *target = GetTarget();

    if (!apply)
    {
        switch(GetId())
        {
            case 66:                                        // Invisibility
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                {
                    //Amaru: if not channeling and doesn't have aura
                    if (!target->GetCurrentSpell(CURRENT_CHANNELED_SPELL) && !target->hasPositiveAuraWithInterruptFlag(AURA_INTERRUPT_FLAG_NOT_SEATED))
                        target->CastSpell(target, 32612, true, NULL, this);
                }
                return;
            case 28522:                                     // Icebolt (Naxxramas: Sapphiron)
                if (target->HasAura(45776))                 // Should trigger/remove some kind of iceblock
                    // not sure about ice block spell id
                    target->RemoveAurasDueToSpell(45776);

                return;
            case 42783:                                     // Wrath of the Astrom...
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE && GetEffIndex() + 1 < MAX_EFFECT_INDEX)
                    target->CastSpell(target, GetSpellProto()->CalculateSimpleValue(SpellEffectIndex(GetEffIndex()+1)), true);

                return;
            case 46221:                                     // Animal Blood
                if (target->GetTypeId() == TYPEID_PLAYER && m_removeMode == AURA_REMOVE_BY_DEFAULT && target->IsInWater())
                {
                    float position_z = target->GetTerrain()->GetWaterLevel(target->GetPositionX(), target->GetPositionY(), target->GetPositionZ());
                    // Spawn Blood Pool
                    target->CastSpell(target->GetPositionX(), target->GetPositionY(), position_z, 63471, true);
                }

                return;
            case 51121:                                     // Urom Clocking Bomb
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                    target->CastSpell(target, 51132, true);
                return;
            case 51912:                                     // Ultra-Advanced Proto-Typical Shortening Blaster
            case 53102:                                     // Scepter of Domination
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                {
                    if (Unit* pCaster = GetCaster())
                        pCaster->CastSpell(target, m_spellEffect->EffectTriggerSpell, true, NULL, this);
                }
                return;
            case 52658:                                     // Ionar Static Overload Explode (N)
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                    target->CastSpell(target, 53337, true);

                return;
            case 59795:                                     // Ionar Static Overload Explode (H)
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                    target->CastSpell(target, 59798, true);

                return;
            case 63018:                                     // Searing Light (Ulduar: XT-002)
            case 65121:                                     // Searing Light (h) (Ulduar: XT-002)
                if (Unit *pCaster = GetCaster())
                {
                    if (pCaster->HasAura(GetModifier()->m_amount))
                        pCaster->CastSpell(target, 64210, true);
                }

                return;
            case 63024:                                     // Gravity Bomb (Ulduar: XT-002)
            case 64234:                                     // Gravity Bomb (h) (Ulduar: XT-002)
                if (Unit *pCaster = GetCaster())
                {
                    uint32 spellId = GetId() == 63024 ? 64203 : 64235;
                    if (pCaster->HasAura(GetModifier()->m_amount))
                        pCaster->CastSpell(target, spellId, true);
                }

                return;
            case 66083:                                     // Lightning Arrows (Trial of the Champion encounter)
                if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                {
                    if (Unit* pCaster = GetCaster())
                        pCaster->CastSpell(pCaster, 66085, true, NULL, this);
                }

                return;
            default:
                break;
        }
    }
}

void Aura::HandlePeriodicTriggerSpellWithValue(bool apply, bool /*Real*/)
{
    m_isPeriodic = apply;
}

void Aura::HandlePeriodicEnergize(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();

    // For prevent double apply bonuses
    bool loading = (target->GetTypeId() == TYPEID_PLAYER && ((Player*)target)->GetSession()->PlayerLoading());

    if (apply && !loading)
    {
        switch (GetId())
        {
            case 29166:                                     // Innervate (value% of caster max mana)
            {
                if (Unit* caster = GetCaster())
                {
                    int32 percent = 0;
                    if (caster == target)
                    {
                        percent = 15;
                        // search Dreamstate
                        Unit::AuraList const& mDummyAuras = caster->GetAurasByType(SPELL_AURA_DUMMY);
                        for (Unit::AuraList::const_iterator itr = mDummyAuras.begin(); itr != mDummyAuras.end(); ++itr)
                        {
                            if ((*itr)->GetSpellProto()->SpellIconID == 2255 && (*itr)->GetEffIndex() == EFFECT_INDEX_0 &&
                                (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_DRUID)
                            {
                                percent += (*itr)->GetModifier()->m_amount;
                                break;
                            }
                        }
                    }
                    else if (caster->HasAura(54832))
                        percent = 10;

                    if (percent)
                        caster->CastCustomSpell(caster, 54833, &percent, 0, 0, true, NULL, this);
                }
                ChangeAmount(int32(target->GetMaxPower(POWER_MANA) * GetBasePoints() / 4 / (100 * GetAuraMaxTicks())));
                break;
            }
            case 54833:                                     // Innervate and Glyph of Innervate (value% of caster max mana)
            {
                if (Unit* caster = GetCaster())
                    ChangeAmount(int32(caster->GetMaxPower(POWER_MANA) * GetBasePoints() / (100 * GetAuraMaxTicks())));
                break;
            }
            case 31930:                                     // Judgements of the Wise
            case 89906:                                     // Judgements of the Bold
            {
                ChangeAmount(target->GetCreateMana() * GetBasePoints() / 100 / GetAuraMaxTicks());
                break;
            }
            case 57669:                                     // Replenishment (1% from max)
                ChangeAmount(target->GetMaxPower(POWER_MANA) / 100 / GetAuraMaxTicks());
                break;
            case 61782:                                     // Infinite Replenishment (0.25% from max)
                ChangeAmount(target->GetMaxPower(POWER_MANA) * 25 / 10000);
                break;
            default:
                break;
        }
    }

    m_isPeriodic = apply;
}

void Aura::HandleAuraPowerBurn(bool apply, bool /*Real*/)
{
    m_isPeriodic = apply;
}

void Aura::HandleAuraPeriodicDummy(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if(!Real)
        return;

    Unit *target = GetTarget();

    // For prevent double apply bonuses
    bool loading = (target->GetTypeId() == TYPEID_PLAYER && ((Player*)target)->GetSession()->PlayerLoading());

    SpellEntry const* spell = GetSpellProto();
    switch(GetSpellProto()->GetSpellFamilyName())
    {
        case SPELLFAMILY_GENERIC:
        {
            switch(spell->Id)
            {
                case 49555:                             // Corpse Explode (Trollgore - Drak'Tharon Keep Normal)
                    if (!apply)
                    {
                        if (target)
                        {
                            if (Unit *caster = GetCaster())
                            {
                                target->CastSpell(target, 49618, true, 0, 0, caster->GetObjectGuid());
                                target->CastSpell(target, 51270, true);
                                target->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.0f);
                                target->SetDeathState(JUST_DIED);
                            }
                        }
                    }
                    break;
                case 59807:                             // Corpse Explode (Trollgore - Drak'Tharon Keep Hero)
                    if (!apply)
                    {
                        if (target)
                        {
                            if (Unit *caster = GetCaster())
                            {
                                target->CastSpell(target, 59809, true, 0, 0, caster->GetObjectGuid());
                                target->CastSpell(target, 51270, true);
                                target->SetFloatValue(OBJECT_FIELD_SCALE_X, 1.0f);
                                target->SetDeathState(JUST_DIED);
                            }
                        }
                    }
                    break;
                case 55093:                                   // Grip of Slad'ran
                case 61474:                                   // Grip of Slad'ran (h)
                {
                    if (apply)
                    {
                        if (target->HasAura(55126) || target->HasAura(61476))
                        {
                            target->RemoveSpellAuraHolder(GetHolder());
                            return;
                        }

                        if (GetHolder()->GetStackAmount() >= 5)
                        {
                            target->RemoveAura(this);
                            target->CastSpell(target, (spell->Id == 55093) ? 55126 : 61476, true);
                        }
                    }
                    break;
                }
                case 62717:                                   // Slag Pot (Ulduar: Ignis)
                case 63477:
                {
                    Unit *caster = GetCaster();

                    if (!caster || !target)
                        return;

                    // Haste buff (Slag Imbued)
                    if (!apply)
                        target->CastSpell(caster, (spell->Id == 62717) ? 62836 : 63536, true);

                    break;
                }
                case 63050:                                   // Sanity (Yogg Saron - Ulduar)
                {
                                                              // here is the special handling of Sanity
                    Unit *caster = GetCaster();
                    if (!caster)
                    {
                        target->RemoveAurasDueToSpell(63050);
                        return;
                    }

                    if (!caster->isAlive())
                    {
                        target->RemoveAurasDueToSpell(63050);
                        return;
                    }

                    uint32 stacks = GetHolder()->GetStackAmount();

                    if ((stacks < 30) && !(target->HasAura(63752)))
                        target->CastSpell(target, 63752, true);

                    if ((stacks > 30) && (target->HasAura(63752)))
                        target->RemoveAurasDueToSpell(63752);

                    if (target->HasAura(64169))               // sanity well Aura
                        GetHolder()->ModStackAmount(20);
                    return;
                }
                case 64217:                                 // Overcharged (spell from Emalon adds)
                {
                    if (GetHolder()->GetStackAmount() > 11)
                    {
                        target->CastSpell(target, 64219, true);
                        target->DealDamage(target, target->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    }
                    return;
                }
            }
        }
        case SPELLFAMILY_ROGUE:
        {
            switch(GetSpellProto()->Id)
            {
                // Master of Subtlety
                case 31666:
                {
                    if (apply)
                    {
                        // for make duration visible
                        if (SpellAuraHolder* holder = target->GetSpellAuraHolder(31665))
                        {
                            holder->SetAuraMaxDuration(GetHolder()->GetAuraDuration());
                            holder->RefreshHolder();
                        }
                    }
                    else
                        target->RemoveAurasDueToSpell(31665);
                    break;
                }
            }
            break;
        }
        case SPELLFAMILY_WARLOCK:
        {
            switch (spell->Id)
            {
                // Demonic Circle: Summon
                case 48018:
                {
                    if (apply)
                        GetHolder()->SendFakeAuraUpdate(62388, false);
                    else
                    {
                        //GetTarget()->RemoveGameObject(spell->Id,true);//megai2: maybe it deletes somewhere in prev code?
                        GetHolder()->SendFakeAuraUpdate(62388, true);
                    }
                    break;
                }
            }
        }
        case SPELLFAMILY_HUNTER:
            break;
        case SPELLFAMILY_DEATHKNIGHT:
        {
            // Reaping and Blood Rites
            if (GetSpellProto()->SpellIconID == 22 || GetSpellProto()->SpellIconID == 2724)
            {
                if (apply)
                    break;

                if (m_spellEffect->EffectApplyAuraName != SPELL_AURA_PERIODIC_DUMMY)
                    break;
                if (target->GetTypeId() != TYPEID_PLAYER)
                    break;
                if (((Player*)target)->getClass() != CLASS_DEATH_KNIGHT)
                    break;

                // aura removed - remove death runes
                ((Player*)target)->RemoveRunesByAuraEffect(this);
            }
            break;
        }
    }

    m_isPeriodic = apply;
}

void Aura::HandlePeriodicHeal(bool apply, bool /*Real*/)
{
    m_isPeriodic = apply;

    Unit *target = GetTarget();

    // For prevent double apply bonuses
    bool loading = (target->GetTypeId() == TYPEID_PLAYER && ((Player*)target)->GetSession()->PlayerLoading());

    // Custom damage calculation after
    if (apply)
    {
        if(loading)
            return;

        Unit *caster = GetCaster();
        if (!caster)
            return;

        // Gift of the Naaru (have multiple spellfamilies)
        if (GetSpellProto()->IsFitToFamilyMask(UI64LIT(0x0), 0x80000000))
        {
            float add = 0.0f;
            switch (GetSpellProto()->GetSpellFamilyName())
            {
                case SPELLFAMILY_MAGE:
                case SPELLFAMILY_WARLOCK:
                case SPELLFAMILY_PRIEST:
                    add = 1.885f * (float)caster->SpellBaseDamageBonusDone(GetSpellSchoolMask(GetSpellProto()));
                    break;

                case SPELLFAMILY_PALADIN:
                case SPELLFAMILY_SHAMAN:
                    add = std::max(1.885f * (float)caster->SpellBaseDamageBonusDone(GetSpellSchoolMask(GetSpellProto())), 1.1f * (float)caster->GetTotalAttackPowerValue(BASE_ATTACK));
                    break;

                case SPELLFAMILY_WARRIOR:
                case SPELLFAMILY_HUNTER:
                case SPELLFAMILY_DEATHKNIGHT:
                    add = 1.1f * (float)std::max(caster->GetTotalAttackPowerValue(BASE_ATTACK), caster->GetTotalAttackPowerValue(RANGED_ATTACK));
                    break;

                case SPELLFAMILY_GENERIC:
                default:
                    sLog.outError("Aura::HandlePeriodicHeal unknown type of aura %u",GetId());
                    break;
            }

            int32 add_per_tick = floor(add / GetAuraMaxTicks());
            m_modifier.m_amount += (add_per_tick > 0 ? add_per_tick : 0);
        }
        // Lightwell
        else if (GetSpellProto()->IsFitToFamily(SPELLFAMILY_PRIEST, UI64LIT(0x0), 0x4000))
        {
            Aura* cAur = caster->GetAura(59907, EFFECT_INDEX_0);
            if (cAur && cAur->GetHolder() && cAur->GetHolder()->DropAuraCharge())
                caster->RemoveAura(59907, EFFECT_INDEX_0);

            if (caster->GetOwner())
                m_modifier.m_amount = caster->GetOwner()->SpellHealingBonusDone(target, GetSpellProto(), m_modifier.m_amount, DOT, GetStackAmount());
            return;
        }
        // Lifeblood
        else if (GetSpellProto()->SpellIconID == 3088 && GetSpellProto()->SpellVisual[0] == 8145)
        {
            int32 healthBonus = int32 (0.0032f * caster->GetMaxHealth());
            m_modifier.m_amount += healthBonus;
        }
        // Spirit Mend (Exotic Ability)
        else if (GetId() == 90361)
        {
            if (Unit* owner = caster->GetOwner())
            {
                float rap = owner->GetTotalAttackPowerValue(RANGED_ATTACK);
                int32 healthBonus = int32((rap * 0.35f) * 0.335f);
                m_modifier.m_amount += healthBonus;
            }
        }

        uint32 stackAmount = GetStackAmount() > 0 ? GetStackAmount() : 1;
        // Amaru: lifebloom special case
        if (GetSpellProto()->IsFitToFamily(SPELLFAMILY_DRUID, UI64LIT(0x1000000000)))
            m_modifier.m_amount = caster->SpellHealingBonusDone(target, GetSpellProto(), m_modifier.m_amount / stackAmount, DOT) * stackAmount;
        else
            m_modifier.m_amount = caster->SpellHealingBonusDone(target, GetSpellProto(), m_modifier.m_amount, DOT, stackAmount);

        // Rejuvenation
        if (GetSpellProto()->IsFitToFamily(SPELLFAMILY_DRUID, UI64LIT(0x0000000000000010)))
            if (caster->HasAura(64760))                     // Item - Druid T8 Restoration 4P Bonus
                caster->CastCustomSpell(target, 64801, &m_modifier.m_amount, NULL, NULL, true, NULL);
    }
}

void Aura::HandleDamagePercentTaken(bool apply, bool Real)
{
    m_isPeriodic = apply;

    Unit* target = GetTarget();

    if (!Real)
        return;

    // For prevent double apply bonuses
    bool loading = (target->GetTypeId() == TYPEID_PLAYER && ((Player*)target)->GetSession()->PlayerLoading());

    if (apply)
    {
        if (loading)
            return;

        // Shadow Sight
        if (GetId() == 34709)
        {
            target->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
            target->RemoveSpellsCausingAura(SPELL_AURA_MOD_INVISIBILITY);
            target->RemoveSpellsCausingAura(SPELL_AURA_CAMOUFLAGE);
        }
        // Will of the Necropolis
        else if (GetId() == 81162)
        {
            if (target->GetTypeId() == TYPEID_PLAYER)
                ((Player*)target)->RemoveSpellCooldown(48982, true);
            target->CastSpell(target, 96171, true);
        }
    }

    // Deterrence
    if (GetId() == 114406)
        if (apply)
            target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
        else
            target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
}

void Aura::HandlePeriodicDamage(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if(!Real)
        return;

    m_isPeriodic = apply;

    Unit *target = GetTarget();
    SpellEntry const* spellProto = GetSpellProto();
    SpellClassOptionsEntry const* classOptions = spellProto->GetSpellClassOptions();

    // For prevent double apply bonuses
    bool loading = (target->GetTypeId() == TYPEID_PLAYER && ((Player*)target)->GetSession()->PlayerLoading());

    // Custom damage calculation after
    if (apply)
    {
        if(loading)
            return;

        Unit *caster = GetCaster();
        if (!caster)
            return;

        if(!classOptions)
            return;

        switch (classOptions->SpellFamilyName)
        {
            case SPELLFAMILY_GENERIC:
            {
                // Dream Funnel
                if (GetId() == 50344)
                    m_modifier.m_amount = caster->GetMaxHealth() * 5 / 100.0f;
                break;
            }
            case SPELLFAMILY_WARRIOR:
            {
                // Rend
                if (classOptions->SpellFamilyFlags & UI64LIT(0x0000000000000020))
                {
                    // ${0.25*6*(($MWB+$mwb)/2+$AP/14*$MWS)}
                    float ap = caster->GetTotalAttackPowerValue(BASE_ATTACK);
                    int32 mws = caster->GetAttackTime(BASE_ATTACK);
                    float mwb_min = caster->GetWeaponDamageRange(BASE_ATTACK, MINDAMAGE);
                    float mwb_max = caster->GetWeaponDamageRange(BASE_ATTACK, MAXDAMAGE);
                    m_modifier.m_amount += int32(((mwb_min + mwb_max) / 2 + ap * mws / 14000) * 0.25f);
                    ChangeAmount(m_modifier.m_amount);
                }
                break;
            }
            case SPELLFAMILY_DRUID:
            {
                // Rip
                if (classOptions->SpellFamilyFlags & UI64LIT(0x000000000000800000))
                {
                    if (caster->GetTypeId() != TYPEID_PLAYER)
                        break;

                    // 0.01*$AP*cp
                    uint8 cp = caster->GetComboPoints();

                    // Idol of Feral Shadows. Cant be handled as SpellMod in SpellAura:Dummy due its dependency from CPs
                    Unit::AuraList const& dummyAuras = caster->GetAurasByType(SPELL_AURA_DUMMY);
                    for(Unit::AuraList::const_iterator itr = dummyAuras.begin(); itr != dummyAuras.end(); ++itr)
                    {
                        if((*itr)->GetId()==34241)
                        {
                            m_modifier.m_amount += cp * (*itr)->GetModifier()->m_amount;
                            break;
                        }
                    }
                    m_modifier.m_amount += int32(caster->GetTotalAttackPowerValue(BASE_ATTACK) * cp / 100);
                }
                // Insect Swarm
                else if (spellProto->IsFitToFamilyMask(UI64LIT(0x200000)))
                {
                    // Idol of the Crying Wind
                    Unit::AuraList const& dummyAuras = caster->GetAurasByType(SPELL_AURA_DUMMY);
                    for(Unit::AuraList::const_iterator itr = dummyAuras.begin(); itr != dummyAuras.end(); ++itr)
                    {
                        if((*itr)->GetId()==64950)
                        {
                            m_modifier.m_amount += (*itr)->GetModifier()->m_amount;
                            break;
                        }
                    }
                }
                break;
            }
            case SPELLFAMILY_ROGUE:
                break;
            case SPELLFAMILY_PALADIN:
            {
                // Holy Vengeance / Blood Corruption
                if (classOptions->SpellFamilyFlags & UI64LIT(0x0000080000000000) && spellProto->SpellVisual[0] == 7902)
                {
                    // AP * 0.027 + SPH * 0.01 bonus per tick
                    float ap = caster->GetTotalAttackPowerValue(BASE_ATTACK);
                    int32 holy = caster->SpellBaseDamageBonusDone(GetSpellSchoolMask(spellProto));
                    if (holy < 0)
                        holy = 0;
                    m_modifier.m_amount += int32(GetStackAmount()) * (int32(ap * 0.027f) + int32(0.01f * holy));
                }
                break;
            }
            default:
                break;
        }

        if(m_modifier.m_auraname == SPELL_AURA_PERIODIC_DAMAGE)
        {
            // SpellDamageBonusDone for magic spells
            uint32 dmgClass = spellProto->GetDmgClass();
            if(dmgClass == SPELL_DAMAGE_CLASS_NONE || dmgClass == SPELL_DAMAGE_CLASS_MAGIC)
                m_modifier.m_amount = caster->SpellDamageBonusDone(target, GetSpellProto(), m_modifier.m_amount, DOT, GetStackAmount());
            // MeleeDamagebonusDone for weapon based spells
            else
            {
                WeaponAttackType attackType = GetWeaponAttackType(GetSpellProto());
                m_modifier.m_amount = caster->MeleeDamageBonusDone(target, m_modifier.m_amount, attackType, GetSpellProto(), DOT, GetStackAmount());
            }
        }
    }
    // remove time effects
    else
    {
        // Drain Soul
        if (spellProto->Id == 1120)
        {
            if (m_removeMode == AURA_REMOVE_BY_DEATH)
            {
                if (Unit* caster = GetCaster())
                {
                    // energize
                    caster->CastSpell(caster, 79264, true);
                    // Glyph of Drain Soul
                    if (caster->GetTypeId() == TYPEID_PLAYER && ((Player*)caster)->isHonorOrXPTarget(target))
                        caster->CastSpell(caster, 58068, true);
                }
            }
        }
        // Rupture
        else if (spellProto->Id == 1943)
        {
            if (m_removeMode == AURA_REMOVE_BY_DEATH)
            {
                if (Unit* caster = GetCaster())
                {
                    Unit::AuraList const& mDummyAuras = caster->GetAurasByType(SPELL_AURA_DUMMY);
                    for (Unit::AuraList::const_iterator itr = mDummyAuras.begin(); itr != mDummyAuras.end(); ++itr)
                    {
                        // Venomous Wounds
                        if ((*itr)->GetSpellProto()->SpellIconID == 4888 && (*itr)->GetEffIndex() == EFFECT_INDEX_1 && (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_ROGUE)
                        {
                            int32 bp = 0;
                            uint32 ticks = GetAuraMaxTicks() - GetAuraTicks();
                            for (int i = 0; i < ticks; ++i)
                            {
                                if (roll_chance_i((*itr)->GetSpellProto()->GetProcChance()))
                                    bp += (*itr)->GetModifier()->m_amount;
                            }
                            if (bp)
                                // Venomous Vim
                                caster->CastCustomSpell(caster, 51637, &bp, NULL, NULL, true);
                            break;
                        }
                    }
                }
            }
        }
        // Parasitic Shadowfiend - handle summoning of two Shadowfiends on DoT expire
        else if (spellProto->Id == 41917)
            target->CastSpell(target, 41915, true);
        else if (spellProto->Id == 74562) // SPELL_FIERY_COMBUSTION - Ruby sanctum boss Halion
            target->CastSpell(target, 74607, true, NULL, NULL, GetCasterGuid());
        else if (spellProto->Id == 74792) // SPELL_SOUL_CONSUMPTION - Ruby sanctum boss Halion
            target->CastSpell(target, 74799, true, NULL, NULL, GetCasterGuid());
        // Void Shifted
        else if (spellProto->Id == 54361 || spellProto->Id == 59743)
            target->CastSpell(target, 54343, true, NULL, NULL, GetCasterGuid());
    }
}

void Aura::HandlePeriodicDamagePCT(bool apply, bool /*Real*/)
{
    m_isPeriodic = apply;
}

void Aura::HandlePeriodicLeech(bool apply, bool /*Real*/)
{
    m_isPeriodic = apply;

    // For prevent double apply bonuses
    bool loading = (GetTarget()->GetTypeId() == TYPEID_PLAYER && ((Player*)GetTarget())->GetSession()->PlayerLoading());

    // Custom damage calculation after
    if (apply)
    {
        if(loading)
            return;

        Unit *caster = GetCaster();
        if (!caster)
            return;

        m_modifier.m_amount = caster->SpellDamageBonusDone(GetTarget(), GetSpellProto(), m_modifier.m_amount, DOT, GetStackAmount());
    }
}

void Aura::HandlePeriodicManaLeech(bool apply, bool /*Real*/)
{
    m_isPeriodic = apply;
}

void Aura::HandlePeriodicHealthFunnel(bool apply, bool /*Real*/)
{
    m_isPeriodic = apply;

    // For prevent double apply bonuses
    bool loading = (GetTarget()->GetTypeId() == TYPEID_PLAYER && ((Player*)GetTarget())->GetSession()->PlayerLoading());

    // Custom damage calculation after
    if (apply)
    {
        if(loading)
            return;

        Unit *caster = GetCaster();
        if (!caster)
            return;

        m_modifier.m_amount = caster->SpellDamageBonusDone(GetTarget(), GetSpellProto(), m_modifier.m_amount, DOT, GetStackAmount());
    }
}

/*********************************************************/
/***                  MODIFY STATS                     ***/
/*********************************************************/

/********************************/
/***        RESISTANCE        ***/
/********************************/

void Aura::HandleAuraModResistanceExclusive(bool apply, bool /*Real*/)
{
    for (int8 x = SPELL_SCHOOL_NORMAL; x < MAX_SPELL_SCHOOL; ++x)
    {
        int32 oldMaxValue = 0;
        if(m_modifier.m_miscvalue & int32(1<<x))
        {
            Unit::AuraList const& REAuras = GetTarget()->GetAurasByType(SPELL_AURA_MOD_RESISTANCE_EXCLUSIVE);
            for (Unit::AuraList::const_iterator i = REAuras.begin(); i != REAuras.end(); ++i)
                if ( ((*i)->GetMiscValue() & int32(1<<x)) && (*i)->GetSpellProto()->Id != GetSpellProto()->Id)
                    if (oldMaxValue < (*i)->GetModifier()->m_amount)
                        oldMaxValue = (*i)->GetModifier()->m_amount;

            float value = (m_modifier.m_amount > oldMaxValue) ? m_modifier.m_amount - oldMaxValue : 0.0f;
            GetTarget()->HandleStatModifier(UnitMods(UNIT_MOD_RESISTANCE_START + x), BASE_VALUE, value, apply);
            if (GetTarget()->GetTypeId() == TYPEID_PLAYER)
                GetTarget()->ApplyResistanceBuffModsMod(SpellSchools(x), m_positive, value, apply);
        }
    }
}

void Aura::HandleAuraModResistance(bool apply, bool /*Real*/)
{
    for (int8 x = SPELL_SCHOOL_NORMAL; x < MAX_SPELL_SCHOOL; ++x)
    {
        if(m_modifier.m_miscvalue & int32(1<<x))
        {
            GetTarget()->HandleStatModifier(UnitMods(UNIT_MOD_RESISTANCE_START + x), TOTAL_VALUE, float(m_modifier.m_amount), apply);
            if(GetTarget()->GetTypeId() == TYPEID_PLAYER || ((Creature*)GetTarget())->IsPet())
                GetTarget()->ApplyResistanceBuffModsMod(SpellSchools(x), m_positive, float(m_modifier.m_amount), apply);
        }
    }
}

void Aura::HandleAuraModBaseResistancePCT(bool apply, bool /*Real*/)
{
    // only players have base stats
    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
    {
        //pets only have base armor
        if (((Creature*)GetTarget())->IsPet() && (m_modifier.m_miscvalue & SPELL_SCHOOL_MASK_NORMAL))
            GetTarget()->HandleStatModifier(UNIT_MOD_ARMOR, BASE_PCT, float(m_modifier.m_amount), apply);
    }
    else
    {
        for (int8 x = SPELL_SCHOOL_NORMAL; x < MAX_SPELL_SCHOOL; ++x)
        {
            if(m_modifier.m_miscvalue & int32(1<<x))
                GetTarget()->HandleStatModifier(UnitMods(UNIT_MOD_RESISTANCE_START + x), BASE_PCT, float(m_modifier.m_amount), apply);
        }
    }
}

void Aura::HandleModResistancePercent(bool apply, bool /*Real*/)
{
    Unit* target = GetTarget();

    for (int8 i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; ++i)
    {
        if (m_modifier.m_miscvalue & int32(1<<i))
        {
            target->HandleStatModifier(UnitMods(UNIT_MOD_RESISTANCE_START + i), TOTAL_PCT, float(m_modifier.m_amount), apply);
            if(target->GetTypeId() == TYPEID_PLAYER || ((Creature*)target)->IsPet())
            {
                target->ApplyResistanceBuffModsPercentMod(SpellSchools(i), true, float(m_modifier.m_amount), apply);
                target->ApplyResistanceBuffModsPercentMod(SpellSchools(i), false, float(m_modifier.m_amount), apply);
            }
        }
    }

    // Faerie Fire
    if (GetId() == 91565)
    {
        if (Unit* caster = GetCaster())
        {
            // search Feral Aggression
            Unit::AuraList const& mDummyAuras = caster->GetAurasByType(SPELL_AURA_DUMMY);
            for (Unit::AuraList::const_iterator itr = mDummyAuras.begin(); itr != mDummyAuras.end(); ++itr)
            {
                SpellEntry const* spell = (*itr)->GetSpellProto();
                if (spell->SpellIconID == 960 && spell->GetSpellFamilyName() == SPELLFAMILY_DRUID &&
                    (*itr)->GetEffIndex() == EFFECT_INDEX_0)
                {
                    GetHolder()->SetStackAmount((*itr)->GetModifier()->m_amount);
                    break;
                }
            }
        }
    }
}

void Aura::HandleModBaseResistance(bool apply, bool /*Real*/)
{
    // only players have base stats
    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
    {
        //only pets have base stats
        if (((Creature*)GetTarget())->IsPet() && (m_modifier.m_miscvalue & SPELL_SCHOOL_MASK_NORMAL))
            GetTarget()->HandleStatModifier(UNIT_MOD_ARMOR, TOTAL_VALUE, float(m_modifier.m_amount), apply);
    }
    else
    {
        for (int i = SPELL_SCHOOL_NORMAL; i < MAX_SPELL_SCHOOL; ++i)
            if (m_modifier.m_miscvalue & (1<<i))
                GetTarget()->HandleStatModifier(UnitMods(UNIT_MOD_RESISTANCE_START + i), TOTAL_VALUE, float(m_modifier.m_amount), apply);
    }
}

/********************************/
/***           STAT           ***/
/********************************/

void Aura::HandleAuraModStat(bool apply, bool /*Real*/)
{
    if (m_modifier.m_miscvalue < -2 || m_modifier.m_miscvalue > 4)
    {
        ERROR_LOG("WARNING: Spell %u effect %u have unsupported misc value (%i) for SPELL_AURA_MOD_STAT ",GetId(),GetEffIndex(),m_modifier.m_miscvalue);
        return;
    }

    Unit* target = GetTarget();

    if (apply)
    {
        switch (GetSpellProto()->Id)
        {
            // Mana Tide Totem
            case 16191:
            {
                if (Unit* caster = GetCaster())
                {
                    if (Unit* owner = caster->GetOwner())
                        ChangeAmount(int32(owner->GetTotalStatValue(STAT_SPIRIT) * m_modifier.m_amount / 100.0f));
                }
                break;
            }
            // Forged Fury
            case 91836:
            {
                // Raw Fury
                target->RemoveAurasDueToSpell(91832);
                break;
            }
            default:
                break;
        }
    }

    for (int32 i = STAT_STRENGTH; i < MAX_STATS; ++i)
    {
        // -1 or -2 is all stats ( misc < -2 checked in function beginning )
        if (m_modifier.m_miscvalue < 0 || m_modifier.m_miscvalue == i)
        {
            // pet scaling aura workaround
            float energyPct = 0.0f;
            if (GetSpellProto()->AttributesEx4 & SPELL_ATTR_EX4_PET_SCALING_AURA)
            {
                if (m_modifier.m_miscvalue == STAT_STAMINA)
                    energyPct = float(target->GetHealth()) / target->GetMaxHealth();
                else if (m_modifier.m_miscvalue == STAT_INTELLECT && target->GetMaxPower(POWER_MANA) > 0)
                    energyPct = float(target->GetPower(POWER_MANA)) / target->GetMaxPower(POWER_MANA);
            }

            //m_target->ApplyStatMod(Stats(i), m_modifier.m_amount,apply);
            target->HandleStatModifier(UnitMods(UNIT_MOD_STAT_START + i), TOTAL_VALUE, float(m_modifier.m_amount), apply);
            if (target->GetTypeId() == TYPEID_PLAYER || ((Creature*)target)->IsPet())
                target->ApplyStatBuffMod(Stats(i), float(m_modifier.m_amount), apply);

            if (energyPct)
            {
                if (m_modifier.m_miscvalue == STAT_STAMINA)
                    target->SetHealth(target->GetMaxHealth() * energyPct);
                else if (m_modifier.m_miscvalue == STAT_INTELLECT)
                    target->SetPower(POWER_MANA, target->GetMaxPower(POWER_MANA) * energyPct);
            }
        }
    }
}

void Aura::HandleModPercentStat(bool apply, bool /*Real*/)
{
    if (m_modifier.m_miscvalue < -1 || m_modifier.m_miscvalue > 4)
    {
        ERROR_LOG("WARNING: Misc Value for SPELL_AURA_MOD_PERCENT_STAT not valid");
        return;
    }

    // only players have base stats
    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    for (int32 i = STAT_STRENGTH; i < MAX_STATS; ++i)
    {
        if (m_modifier.m_miscvalue == i || m_modifier.m_miscvalue == -1)
            GetTarget()->HandleStatModifier(UnitMods(UNIT_MOD_STAT_START + i), BASE_PCT, float(m_modifier.m_amount), apply);
    }
}

void Aura::HandleModSpellDamagePercentFromStat(bool /*apply*/, bool /*Real*/)
{
    if(GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    // Magic damage modifiers implemented in Unit::SpellDamageBonusDone
    // This information for client side use only
    // Recalculate bonus
    ((Player*)GetTarget())->UpdateSpellDamageAndHealingBonus();
}

void Aura::HandleModSpellHealingPercentFromStat(bool /*apply*/, bool /*Real*/)
{
    if(GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    // Recalculate bonus
    ((Player*)GetTarget())->UpdateSpellDamageAndHealingBonus();
}

void Aura::HandleAuraModDispelResist(bool apply, bool Real)
{
    if(!Real || !apply)
        return;

    if(GetId() == 33206)
        GetTarget()->CastSpell(GetTarget(), 44416, true, NULL, this, GetCasterGuid());
}

void Aura::HandleModSpellDamagePercentFromAttackPower(bool /*apply*/, bool /*Real*/)
{
    if(GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    // Magic damage modifiers implemented in Unit::SpellDamageBonusDone
    // This information for client side use only
    // Recalculate bonus
    ((Player*)GetTarget())->UpdateSpellDamageAndHealingBonus();
}

void Aura::HandleModSpellHealingPercentFromAttackPower(bool /*apply*/, bool /*Real*/)
{
    if(GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    // Recalculate bonus
    ((Player*)GetTarget())->UpdateSpellDamageAndHealingBonus();
}

void Aura::HandleModHealingDone(bool /*apply*/, bool /*Real*/)
{
    if(GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;
    // implemented in Unit::SpellHealingBonusDone
    // this information is for client side only
    ((Player*)GetTarget())->UpdateSpellDamageAndHealingBonus();
}

void Aura::HandleModTotalPercentStat(bool apply, bool /*Real*/)
{
    if (!m_modifier.m_amount)
        return;

    Unit* target = GetTarget();
    uint32 miscValueB = GetSpellEffect()->EffectMiscValueB;

    //save current and max HP before applying aura
    uint32 curHPValue = target->GetHealth();
    uint32 maxHPValue = target->GetMaxHealth();


    // Heart of the Wild - stamina
    // Bear Form (Passive)
    if (apply && GetId() == 1178 && target->GetTypeId() == TYPEID_PLAYER && target->getClass() == CLASS_DRUID && target->GetShapeshiftForm() && m_modifier.m_miscvalue == STAT_STAMINA)
    {
        switch (target->GetShapeshiftForm())
        {
            case FORM_BEAR:
            {
                if (SpellEntry const* spellInfo = ((Player*)target)->GetKnownTalentRankById(11715))
                    m_modifier.m_amount += spellInfo->CalculateSimpleValue(EFFECT_INDEX_2);
                break;
            }
        }
    }

    for (int32 i = STAT_STRENGTH; i < MAX_STATS; ++i)
    {
        if ((miscValueB & (1 << i)) || miscValueB == 0)
        {
            target->HandleStatModifier(UnitMods(UNIT_MOD_STAT_START + i), TOTAL_PCT, float(m_modifier.m_amount), apply);
            if (target->GetTypeId() == TYPEID_PLAYER || ((Creature*)target)->IsPet())
            {
                target->ApplyStatPercentBuffMod(Stats(i), float(m_modifier.m_amount), apply );

                // Ranger: update pet stat now!
                if (((Creature*)target)->IsPet())
                     ((Pet*)target)->UpdateStats(Stats(i));
            }
        }
    }

    // recalculate current HP/MP after applying aura modifications (only for spells with 0x10 flag)
    if ((miscValueB & (1 << STAT_STAMINA)) && maxHPValue > 0 && GetSpellProto()->HasAttribute(SPELL_ATTR_UNK4))
    {
        uint32 newHPValue = uint32(float(target->GetMaxHealth()) / maxHPValue * curHPValue);
        target->SetHealth(newHPValue);
    }
}

void Aura::HandleAuraModResistenceOfStatPercent(bool /*apply*/, bool /*Real*/)
{
    if(GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    if(m_modifier.m_miscvalue != SPELL_SCHOOL_MASK_NORMAL)
    {
        // support required adding replace UpdateArmor by loop by UpdateResistence at intellect update
        // and include in UpdateResistence same code as in UpdateArmor for aura mod apply.
        ERROR_LOG("Aura SPELL_AURA_MOD_RESISTANCE_OF_STAT_PERCENT(182) need adding support for non-armor resistances!");
        return;
    }

    // Recalculate Armor
    GetTarget()->UpdateArmor();
}

/********************************/
/***      HEAL & ENERGIZE     ***/
/********************************/
void Aura::HandleAuraModTotalHealthPercentRegen(bool apply, bool /*Real*/)
{
    m_isPeriodic = apply;
}

void Aura::HandleAuraModTotalEnergyPercentRegen(bool apply, bool /*Real*/)
{
    if (m_modifier.periodictime == 0)
        m_modifier.periodictime = 1000;

    if (!GetSpellProto()->HasAttribute(SPELL_ATTR_EX5_START_PERIODIC_AT_APPLY))
        m_periodicTimer = m_modifier.periodictime;
    m_isPeriodic = apply;
}

void Aura::HandleModRegen(bool apply, bool /*Real*/)        // eating
{
    if (m_modifier.periodictime == 0)
        m_modifier.periodictime = 5000;

    if (!GetSpellProto()->HasAttribute(SPELL_ATTR_EX5_START_PERIODIC_AT_APPLY))
        m_periodicTimer = 5000;
    m_isPeriodic = apply;
}

void Aura::HandleModPowerRegen(bool apply, bool Real)       // drinking
{
    if (!Real)
        return;

    Powers pt = GetTarget()->getPowerType();
    if (m_modifier.periodictime == 0)
    {
        // Anger Management (only spell use this aura for rage)
        if (pt == POWER_RAGE)
            m_modifier.periodictime = 3000;
        else if(pt == POWER_RUNIC_POWER)
            m_modifier.periodictime = 5000;
        else
            m_modifier.periodictime = 2000;
    }

    m_periodicTimer = 5000;

    if (GetTarget()->GetTypeId() == TYPEID_PLAYER && m_modifier.m_miscvalue == POWER_MANA)
        ((Player*)GetTarget())->UpdateManaRegen();

    m_isPeriodic = apply;
}

void Aura::HandleModPowerRegenPCT(bool /*apply*/, bool Real)
{
    // spells required only Real aura add/remove
    if(!Real)
        return;

    Unit* target = GetTarget();
    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    // Update manaregen value
    if (m_modifier.m_miscvalue == POWER_MANA)
        ((Player*)target)->UpdateManaRegen();
    else if (m_modifier.m_miscvalue == POWER_RUNE)
        ((Player*)target)->UpdateRuneRegen(RuneType(GetMiscBValue()));
}

void Aura::HandleModManaRegen(bool /*apply*/, bool Real)
{
    // spells required only Real aura add/remove
    if(!Real)
        return;

    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    //Note: an increase in regen does NOT cause threat.
    ((Player*)GetTarget())->UpdateManaRegen();
}

void Aura::HandleComprehendLanguage(bool apply, bool /*Real*/)
{
    if(apply)
        GetTarget()->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_COMPREHEND_LANG);
    else
        GetTarget()->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_COMPREHEND_LANG);
}

void Aura::HandleAuraModIncreaseHealth(bool apply, bool Real)
{
    Unit *target = GetTarget();

    // Special case with temporary increase max/current health
    switch(GetId())
    {
        case 12976:                                         // Warrior Last Stand triggered spell
        case 28726:                                         // Nightmare Seed ( Nightmare Seed )
        case 31616:                                         // Nature's Guardian
        case 34511:                                         // Valor (Bulwark of Kings, Bulwark of the Ancient Kings)
        case 44055: case 55915: case 55917: case 67596:     // Tremendous Fortitude (Battlemaster's Alacrity)
        case 53479:                                         // Hunter pet - Last Stand
        case 54443:                                         // Demonic Empowerment (Voidwalker)
        case 55233:                                         // Vampiric Blood
        case 59465:                                         // Brood Rage (Ahn'Kahet)
        case 79437:                                         // Soulburn: Healthstone
        case 105284:                                        // Ancestral Vigor
        case 105588:                                        // Vampiric Blood
        {
            if(Real)
            {
                if(apply)
                {
                    // Demonic Empowerment (Voidwalker), Soulburn: Healthstone & Vampiric Blood - special cases, store percent in data
                    // recalculate to full amount at apply for proper remove
                    if (GetId() == 54443 || GetId() == 55233 || GetId() == 79437 || GetId() == 105588)
                        m_modifier.m_amount = target->GetMaxHealth() * m_modifier.m_amount / 100;

                    target->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_VALUE, float(m_modifier.m_amount), apply);
                    target->ModifyHealth(m_modifier.m_amount);
                }
                else
                {
                    if (int32(target->GetHealth()) > m_modifier.m_amount)
                        target->ModifyHealth(-m_modifier.m_amount);
                    else
                        target->SetHealth(1);
                    target->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_VALUE, float(m_modifier.m_amount), apply);
                }
            }
            return;
        }
        case 22842:                                         // Frenzied Regeneration (Bear Form)
        case 105737:                                        // Mass Regeneration (Bear Form)
        {
            if (apply)
                if (target->GetHealthPercent() < m_modifier.m_amount)
                    target->SetHealthPercent(m_modifier.m_amount);

            target->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_PCT, float(m_modifier.m_amount), apply);
            return;
        }
    }

    // generic case
    target->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_VALUE, float(m_modifier.m_amount), apply);
}

void  Aura::HandleAuraModIncreaseMaxHealth(bool apply, bool /*Real*/)
{
    Unit *target = GetTarget();
    uint32 oldhealth = target->GetHealth();
    double healthPercentage = (double)oldhealth / (double)target->GetMaxHealth();

    target->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_VALUE, float(m_modifier.m_amount), apply);

    // refresh percentage
    if(oldhealth > 0)
    {
        uint32 newhealth = uint32(ceil((double)target->GetMaxHealth() * healthPercentage));
        if(newhealth==0)
            newhealth = 1;

        target->SetHealth(newhealth);
    }
}

void Aura::HandleAuraModIncreaseEnergy(bool apply, bool Real)
{
    Unit* target = GetTarget();
    Powers powerType = target->getPowerType();
    if (int32(powerType) != m_modifier.m_miscvalue)
    {
        if (target->GetTypeId() == TYPEID_PLAYER && target->getClass() == CLASS_DRUID)
            powerType = Powers(m_modifier.m_miscvalue);
        else
            return;
    }

    UnitMods unitMod = UnitMods(UNIT_MOD_POWER_START + powerType);

    // generic flat case
    target->HandleStatModifier(unitMod, TOTAL_VALUE, float(m_modifier.m_amount), apply);
}

void Aura::HandleAuraModIncreaseEnergyPercent(bool apply, bool /*Real*/)
{
    Unit* target = GetTarget();
    Powers powerType = GetTarget()->getPowerType();
    if (int32(powerType) != m_modifier.m_miscvalue)
    {
        if (target->GetTypeId() == TYPEID_PLAYER && target->getClass() == CLASS_DRUID)
            powerType = Powers(m_modifier.m_miscvalue);
        else
            return;
    }

    UnitMods unitMod = UnitMods(UNIT_MOD_POWER_START + powerType);

    target->HandleStatModifier(unitMod, TOTAL_PCT, float(m_modifier.m_amount), apply);
}

void Aura::HandleAuraModIncreaseHealthPercent(bool apply, bool /*Real*/)
{
    Unit *target = GetTarget();

    target->HandleStatModifier(UNIT_MOD_HEALTH, TOTAL_PCT, float(m_modifier.m_amount), apply);

    // spell special cases when current health set to max value at apply
    switch (GetId())
    {
        case 60430:                                         // Molten Fury
        case 64193:                                         // Heartbreak
        case 65737:                                         // Heartbreak
            target->SetHealth(target->GetMaxHealth());
            break;
        default:
            break;
    }
}

void Aura::HandleAuraIncreaseBaseHealthPercent(bool apply, bool /*Real*/)
{
    GetTarget()->HandleStatModifier(UNIT_MOD_HEALTH, BASE_PCT, float(m_modifier.m_amount), apply);
}

/********************************/
/***          FIGHT           ***/
/********************************/

void Aura::HandleAuraModParryPercent(bool /*apply*/, bool /*Real*/)
{
    if(GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    ((Player*)GetTarget())->UpdateParryPercentage();
}

void Aura::HandleAuraModDodgePercent(bool /*apply*/, bool /*Real*/)
{
    if(GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    ((Player*)GetTarget())->UpdateDodgePercentage();
    //ERROR_LOG("BONUS DODGE CHANCE: + %f", float(m_modifier.m_amount));
}

void Aura::HandleAuraModBlockChancePercent(bool /*apply*/, bool /*Real*/)
{
    if(GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    ((Player*)GetTarget())->UpdateBlockPercentage();
    //ERROR_LOG("BONUS BLOCK CHANCE: + %f", float(m_modifier.m_amount));
}

void Aura::HandleAuraModRegenInterrupt(bool /*apply*/, bool Real)
{
    // spells required only Real aura add/remove
    if(!Real)
        return;

    if(GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    ((Player*)GetTarget())->UpdateManaRegen();
}

void Aura::HandleAuraModCritPercent(bool apply, bool Real)
{
    Unit *target = GetTarget();

    if(target->GetTypeId() != TYPEID_PLAYER)
        return;

    // apply item specific bonuses for already equipped weapon
    if(Real)
    {
        for(int i = 0; i < MAX_ATTACK; ++i)
            if(Item* pItem = ((Player*)target)->GetWeaponForAttack(WeaponAttackType(i),true,false))
                ((Player*)target)->_ApplyWeaponDependentAuraCritMod(pItem, WeaponAttackType(i), this, apply);
    }

    // mods must be applied base at equipped weapon class and subclass comparison
    // with spell->EquippedItemClass and  EquippedItemSubClassMask and EquippedItemInventoryTypeMask
    // m_modifier.m_miscvalue comparison with item generated damage types

    if (GetSpellProto()->GetEquippedItemClass() == -1)
    {
        ((Player*)target)->HandleBaseModValue(CRIT_PERCENTAGE,         FLAT_MOD, float (m_modifier.m_amount), apply);
        ((Player*)target)->HandleBaseModValue(OFFHAND_CRIT_PERCENTAGE, FLAT_MOD, float (m_modifier.m_amount), apply);
        ((Player*)target)->HandleBaseModValue(RANGED_CRIT_PERCENTAGE,  FLAT_MOD, float (m_modifier.m_amount), apply);
    }
    else
    {
        // done in Player::_ApplyWeaponDependentAuraMods
    }
}

void Aura::HandleModHitChance(bool apply, bool /*Real*/)
{
    Unit *target = GetTarget();

    if(target->GetTypeId() == TYPEID_PLAYER)
    {
        ((Player*)target)->UpdateMeleeHitChances();
        ((Player*)target)->UpdateRangedHitChances();
    }
    else
    {
        target->m_modMeleeHitChance += apply ? m_modifier.m_amount : (-m_modifier.m_amount);
        target->m_modRangedHitChance += apply ? m_modifier.m_amount : (-m_modifier.m_amount);
    }
}

void Aura::HandleModSpellHitChance(bool apply, bool /*Real*/)
{
    if(GetTarget()->GetTypeId() == TYPEID_PLAYER)
    {
        ((Player*)GetTarget())->UpdateSpellHitChances();
    }
    else
    {
        GetTarget()->m_modSpellHitChance += apply ? m_modifier.m_amount: (-m_modifier.m_amount);
    }
}

void Aura::HandleModSpellCritChance(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if(!Real)
        return;

    if(GetTarget()->GetTypeId() == TYPEID_PLAYER)
    {
        ((Player*)GetTarget())->UpdateAllSpellCritChances();
    }
    else
    {
        GetTarget()->m_baseSpellCritChance += apply ? m_modifier.m_amount:(-m_modifier.m_amount);
    }
}

void Aura::HandleModSpellCritChanceShool(bool /*apply*/, bool Real)
{
    // spells required only Real aura add/remove
    if(!Real)
        return;

    if(GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    for(int school = SPELL_SCHOOL_NORMAL; school < MAX_SPELL_SCHOOL; ++school)
        if (m_modifier.m_miscvalue & (1<<school))
            ((Player*)GetTarget())->UpdateSpellCritChance(school);
}

/********************************/
/***         ATTACK SPEED     ***/
/********************************/

void Aura::HandleModCastingSpeed(bool apply, bool /*Real*/)
{
    Unit* target = GetTarget();

    target->ApplyCastTimePercentMod(float(m_modifier.m_amount), apply);

    if (apply)
    {
        // Pyromaniac
        if (GetId() == 83582)
        {
            if (target->GetTypeId() == TYPEID_PLAYER)
                if (((Player*)target)->m_pyromaniacCounter < 3)
                    target->RemoveAurasDueToSpell(GetId());
        }
    }
}

void Aura::HandleModMeleeRangedSpeedPct(bool apply, bool /*Real*/)
{
    Unit *target = GetTarget();
    target->ApplyAttackTimePercentMod(BASE_ATTACK, float(m_modifier.m_amount), apply);
    target->ApplyAttackTimePercentMod(OFF_ATTACK, float(m_modifier.m_amount), apply);
    target->ApplyAttackTimePercentMod(RANGED_ATTACK, float(m_modifier.m_amount), apply);
}

void Aura::HandleModCombatSpeedPct(bool apply, bool /*Real*/)
{
    Unit *target = GetTarget();
    target->ApplyCastTimePercentMod(float(m_modifier.m_amount), apply);
    target->ApplyAttackTimePercentMod(BASE_ATTACK, float(m_modifier.m_amount), apply);
    target->ApplyAttackTimePercentMod(OFF_ATTACK, float(m_modifier.m_amount), apply);
    target->ApplyAttackTimePercentMod(RANGED_ATTACK, float(m_modifier.m_amount), apply);
}

void Aura::HandleModAttackSpeed(bool apply, bool /*Real*/)
{
    GetTarget()->ApplyAttackTimePercentMod(BASE_ATTACK,float(m_modifier.m_amount),apply);
}

void Aura::HandleModMeleeSpeedPct(bool apply, bool /*Real*/)
{
    Unit* target = GetTarget();

    // auras below are rune related
    // special check for Unholy Presence
    if ((m_modifier.m_auraname == SPELL_AURA_MOD_MELEE_HASTE || m_modifier.m_auraname == SPELL_AURA_MOD_MELEE_HASTE_3) &&
        (GetId() == 48265 || m_modifier.m_miscvalue == 2 || m_modifier.m_miscvalue == 5))
    {
        if (target->GetTypeId() == TYPEID_PLAYER)
            ((Player*)target)->UpdateRuneRegen();
        return;
    }

    // Frenzy Effect
    if (GetId() == 19615)
    {
        if (apply)
        {
            if (GetStackAmount() >= GetSpellProto()->GetStackAmount())
                if (Unit* owner = target->GetOwner())
                    owner->CastSpell(owner, 88843, true);   // Focus Fire!

        }
        else
        {
            if (GetStackAmount() < GetSpellProto()->GetStackAmount())
                if (Unit* owner = target->GetOwner())
                    owner->RemoveAurasDueToSpell(88843);    // Focus Fire!
        }
    }

    target->ApplyAttackTimePercentMod(BASE_ATTACK, float(m_modifier.m_amount), apply);
    target->ApplyAttackTimePercentMod(OFF_ATTACK, float(m_modifier.m_amount), apply);
}

void Aura::HandleAuraModRangedHaste(bool apply, bool /*Real*/)
{
    Unit* target = GetTarget();

    // Focus Fire
    if (apply && GetId() == 82692)
    {
        if (Pet* pet = target->GetPet())
            if (Aura* aura = pet->GetAura(19615, EFFECT_INDEX_0))
            {
                int32 bp = m_modifier.m_amount * aura->GetStackAmount();

                ChangeAmount(bp);

                // energize pet
                bp = GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_1) * aura->GetStackAmount();
                target->CastCustomSpell(target, 83468, &bp, NULL, NULL, true);

                pet->RemoveSpellAuraHolder(aura->GetHolder());
            }
    }

    target->ApplyAttackTimePercentMod(RANGED_ATTACK, float(m_modifier.m_amount), apply);
}

/********************************/
/***        ATTACK POWER      ***/
/********************************/

void Aura::HandleAuraModAttackPower(bool apply, bool /*Real*/)
{
    GetTarget()->HandleStatModifier(UNIT_MOD_ATTACK_POWER, TOTAL_VALUE, float(m_modifier.m_amount), apply);
}

void Aura::HandleAuraModRangedAttackPower(bool apply, bool /*Real*/)
{
    if((GetTarget()->getClassMask() & CLASSMASK_WAND_USERS)!=0)
        return;

    GetTarget()->HandleStatModifier(UNIT_MOD_ATTACK_POWER_RANGED, TOTAL_VALUE, float(m_modifier.m_amount), apply);
}

void Aura::HandleAuraModAttackPowerPercent(bool apply, bool /*Real*/)
{
    //UNIT_FIELD_ATTACK_POWER_MULTIPLIER = multiplier - 1
    GetTarget()->HandleStatModifier(UNIT_MOD_ATTACK_POWER, TOTAL_PCT, float(m_modifier.m_amount), apply);
}

void Aura::HandleAuraModRangedAttackPowerPercent(bool apply, bool /*Real*/)
{
    if((GetTarget()->getClassMask() & CLASSMASK_WAND_USERS)!=0)
        return;

    //UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER = multiplier - 1
    GetTarget()->HandleStatModifier(UNIT_MOD_ATTACK_POWER_RANGED, TOTAL_PCT, float(m_modifier.m_amount), apply);
}

void Aura::HandleAuraModAttackPowerOfArmor(bool /*apply*/, bool Real)
{
    // spells required only Real aura add/remove
    if(!Real)
        return;

    // Recalculate bonus
    if(GetTarget()->GetTypeId() == TYPEID_PLAYER)
        ((Player*)GetTarget())->UpdateAttackPowerAndDamage(false);
}
/********************************/
/***        DAMAGE BONUS      ***/
/********************************/
void Aura::HandleModDamageDone(bool apply, bool Real)
{
    Unit *target = GetTarget();

    // apply item specific bonuses for already equipped weapon
    if(Real && target->GetTypeId() == TYPEID_PLAYER)
    {
        for(int i = 0; i < MAX_ATTACK; ++i)
            if(Item* pItem = ((Player*)target)->GetWeaponForAttack(WeaponAttackType(i),true,false))
                ((Player*)target)->_ApplyWeaponDependentAuraDamageMod(pItem, WeaponAttackType(i), this, apply);
    }

    // m_modifier.m_miscvalue is bitmask of spell schools
    // 1 ( 0-bit ) - normal school damage (SPELL_SCHOOL_MASK_NORMAL)
    // 126 - full bitmask all magic damages (SPELL_SCHOOL_MASK_MAGIC) including wands
    // 127 - full bitmask any damages
    //
    // mods must be applied base at equipped weapon class and subclass comparison
    // with spell->EquippedItemClass and  EquippedItemSubClassMask and EquippedItemInventoryTypeMask
    // m_modifier.m_miscvalue comparison with item generated damage types

    SpellEquippedItemsEntry const* equippedItems = GetSpellProto()->GetSpellEquippedItems();

    if ((m_modifier.m_miscvalue & SPELL_SCHOOL_MASK_NORMAL) != 0)
    {
        // apply generic physical damage bonuses including wand case
        if (!equippedItems || equippedItems->EquippedItemClass == -1 || target->GetTypeId() != TYPEID_PLAYER)
        {
            target->HandleStatModifier(UNIT_MOD_DAMAGE_MAINHAND, TOTAL_VALUE, float(m_modifier.m_amount), apply);
            target->HandleStatModifier(UNIT_MOD_DAMAGE_OFFHAND, TOTAL_VALUE, float(m_modifier.m_amount), apply);
            target->HandleStatModifier(UNIT_MOD_DAMAGE_RANGED, TOTAL_VALUE, float(m_modifier.m_amount), apply);
        }
        else
        {
            // done in Player::_ApplyWeaponDependentAuraMods
        }

        if (target->GetTypeId() == TYPEID_PLAYER)
        {
            if(m_positive)
                target->ApplyModUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS, m_modifier.m_amount, apply);
            else
                target->ApplyModUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG, m_modifier.m_amount, apply);
        }
    }

    // Skip non magic case for speedup
    if ((m_modifier.m_miscvalue & SPELL_SCHOOL_MASK_MAGIC) == 0)
        return;

    if (equippedItems && (equippedItems->EquippedItemClass != -1 || equippedItems->EquippedItemInventoryTypeMask != 0))
    {
        // wand magic case (skip generic to all item spell bonuses)
        // done in Player::_ApplyWeaponDependentAuraMods

        // Skip item specific requirements for not wand magic damage
        return;
    }

    // Magic damage modifiers implemented in Unit::SpellDamageBonusDone
    // This information for client side use only
    if (target->GetTypeId() == TYPEID_PLAYER)
    {
        if (m_positive)
        {
            for(int i = SPELL_SCHOOL_HOLY; i < MAX_SPELL_SCHOOL; ++i)
            {
                if((m_modifier.m_miscvalue & (1<<i)) != 0)
                    target->ApplyModUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + i, m_modifier.m_amount, apply);
            }
        }
        else
        {
            for(int i = SPELL_SCHOOL_HOLY; i < MAX_SPELL_SCHOOL; ++i)
            {
                if((m_modifier.m_miscvalue & (1<<i)) != 0)
                    target->ApplyModUInt32Value(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + i, m_modifier.m_amount, apply);
            }
        }
        ((Player*)target)->UpdateSpellDamageAndHealingBonus();
        Pet* pet = target->GetPet();
        if(pet)
            pet->UpdateAttackPowerAndDamage();
    }
}

void Aura::HandleModDamagePercentDone(bool apply, bool Real)
{
    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "AURA MOD DAMAGE type:%u negative:%u", m_modifier.m_miscvalue, m_positive ? 0 : 1);
    Unit *target = GetTarget();

    // apply item specific bonuses for already equipped weapon
    if(Real && target->GetTypeId() == TYPEID_PLAYER)
    {
        for(int i = 0; i < MAX_ATTACK; ++i)
            if(Item* pItem = ((Player*)target)->GetWeaponForAttack(WeaponAttackType(i),true,false))
                ((Player*)target)->_ApplyWeaponDependentAuraDamageMod(pItem, WeaponAttackType(i), this, apply);
    }

    // m_modifier.m_miscvalue is bitmask of spell schools
    // 1 ( 0-bit ) - normal school damage (SPELL_SCHOOL_MASK_NORMAL)
    // 126 - full bitmask all magic damages (SPELL_SCHOOL_MASK_MAGIC) including wand
    // 127 - full bitmask any damages
    //
    // mods must be applied base at equipped weapon class and subclass comparison
    // with spell->EquippedItemClass and  EquippedItemSubClassMask and EquippedItemInventoryTypeMask
    // m_modifier.m_miscvalue comparison with item generated damage types

    SpellEquippedItemsEntry const* equippedItems = GetSpellProto()->GetSpellEquippedItems();

    if ((m_modifier.m_miscvalue & SPELL_SCHOOL_MASK_NORMAL) != 0)
    {
        // apply generic physical damage bonuses including wand case
        if (!equippedItems || equippedItems->EquippedItemClass == -1 || target->GetTypeId() != TYPEID_PLAYER)
        {
            target->HandleStatModifier(UNIT_MOD_DAMAGE_MAINHAND, TOTAL_PCT, float(m_modifier.m_amount), apply);
            target->HandleStatModifier(UNIT_MOD_DAMAGE_OFFHAND, TOTAL_PCT, float(m_modifier.m_amount), apply);
            target->HandleStatModifier(UNIT_MOD_DAMAGE_RANGED, TOTAL_PCT, float(m_modifier.m_amount), apply);
        }
        else
        {
            // done in Player::_ApplyWeaponDependentAuraMods
        }
        // For show in client
        if (target->GetTypeId() == TYPEID_PLAYER)
            target->ApplyModSignedFloatValue(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT, m_modifier.m_amount/100.0f, apply);
    }

    // Skip non magic case for speedup
    if((m_modifier.m_miscvalue & SPELL_SCHOOL_MASK_MAGIC) == 0)
        return;

    if( equippedItems && (equippedItems->EquippedItemClass != -1 || equippedItems->EquippedItemInventoryTypeMask != 0) )
    {
        // wand magic case (skip generic to all item spell bonuses)
        // done in Player::_ApplyWeaponDependentAuraMods

        // Skip item specific requirements for not wand magic damage
        return;
    }

    if (apply)
    {
        // Elemental Oath - Damage increase on Clearcasting
        if (GetId() == 16246)
        {
            Unit::AuraList const& auras = GetTarget()->GetAurasByType(SPELL_AURA_MOD_ALL_CRIT_CHANCE);
            for (Unit::AuraList::const_iterator i = auras.begin(); i != auras.end(); ++i)
                if ((*i)->GetId() == 51466 ||   //Elemental Oath rank 1
                    (*i)->GetId() == 51470)     //Elemental Oath rank 2
                {
                    m_modifier.m_amount += (*i)->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_1);
                    break;
                }
        }
        // Dark Transformation
        else if (GetId() == 63560)
            target->RemoveAurasDueToSpell(91342);       // Remove Shadow Infusion
        // Shadow Infusion
        else if (GetId() == 91342)
        {
            if (GetStackAmount() == GetSpellProto()->GetStackAmount())
            {
                if (Unit* owner = target->GetOwner())
                    if (owner->HasSpell(63560))
                        owner->CastSpell(owner, 93426, true);   // cast Dark Transformation marker
            }
        }
    }

    // Magic damage percent modifiers implemented in Unit::SpellDamageBonusDone
    // Send info to client
    if(target->GetTypeId() == TYPEID_PLAYER)
        for(int i = SPELL_SCHOOL_HOLY; i < MAX_SPELL_SCHOOL; ++i)
            target->ApplyModSignedFloatValue(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT + i, m_modifier.m_amount/100.0f, apply);
}

void Aura::HandleModOffhandDamagePercent(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if(!Real)
        return;

    DEBUG_FILTER_LOG(LOG_FILTER_SPELL_CAST, "AURA MOD OFFHAND DAMAGE");

    GetTarget()->HandleStatModifier(UNIT_MOD_DAMAGE_OFFHAND, TOTAL_PCT, float(m_modifier.m_amount), apply);
}

/********************************/
/***        POWER COST        ***/
/********************************/

void Aura::HandleModPowerCostPCT(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if(!Real)
        return;

    float amount = m_modifier.m_amount/100.0f;
    for(int i = 0; i < MAX_SPELL_SCHOOL; ++i)
        if(m_modifier.m_miscvalue & (1<<i))
            GetTarget()->ApplyModSignedFloatValue(UNIT_FIELD_POWER_COST_MULTIPLIER + i, amount, apply);
}

void Aura::HandleModPowerCost(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if(!Real)
        return;

    for(int i = 0; i < MAX_SPELL_SCHOOL; ++i)
        if(m_modifier.m_miscvalue & (1<<i))
            GetTarget()->ApplyModInt32Value(UNIT_FIELD_POWER_COST_MODIFIER + i, m_modifier.m_amount, apply);
}

void Aura::HandleNoReagentUseAura(bool /*Apply*/, bool Real)
{
    // spells required only Real aura add/remove
    if(!Real)
        return;
    Unit *target = GetTarget();
    if(target->GetTypeId() != TYPEID_PLAYER)
        return;

    ClassFamilyMask mask;
    Unit::AuraList const& noReagent = target->GetAurasByType(SPELL_AURA_NO_REAGENT_USE);
    for(Unit::AuraList::const_iterator i = noReagent.begin(); i !=  noReagent.end(); ++i)
        mask |= (*i)->GetAuraSpellClassMask();

    target->SetUInt64Value(PLAYER_NO_REAGENT_COST_1+0, mask.Flags);
    target->SetUInt32Value(PLAYER_NO_REAGENT_COST_1+2, mask.Flags2);
}

/*********************************************************/
/***                    OTHERS                         ***/
/*********************************************************/

void Aura::HandleShapeshiftBoosts(bool apply)
{
    uint32 spellId1 = 0;
    uint32 spellId2 = 0;
    uint32 spellId3 = 0;
    uint32 MasterShaperSpellId = 0;

    ShapeshiftForm form = ShapeshiftForm(GetModifier()->m_miscvalue);

    Unit *target = GetTarget();

    switch(form)
    {
        case FORM_CAT:
            spellId1 = 3025;
            MasterShaperSpellId = 48420;
            break;
        case FORM_TREE:
            spellId1 = 5420;
            spellId2 = 81097;
            spellId3 = 81098;
            break;
        case FORM_TRAVEL:
            spellId1 = 5419;
            break;
        case FORM_AQUA:
            spellId1 = 5421;
            break;
        case FORM_BEAR:
            spellId1 = 1178;
            spellId2 = 21178;
            MasterShaperSpellId = 48418;
            break;
        case FORM_BATTLESTANCE:
            spellId1 = 21156;
            break;
        case FORM_DEFENSIVESTANCE:
            spellId1 = 7376;
            break;
        case FORM_BERSERKERSTANCE:
            spellId1 = 7381;
            break;
        case FORM_MOONKIN:
            spellId1 = 24905;
            spellId2 = 24907;
            MasterShaperSpellId = 48421;
            break;
        case FORM_FLIGHT:
            spellId1 = 33948;
            spellId2 = 34764;
            break;
        case FORM_FLIGHT_EPIC:
            spellId1 = 40122;
            spellId2 = 40121;
            break;
        case FORM_METAMORPHOSIS:
            spellId1 = 54817;
            spellId2 = 54879;
            break;
        case FORM_SPIRITOFREDEMPTION:
            spellId1 = 27792;
            spellId2 = 27795;                               // must be second, this important at aura remove to prevent to early iterator invalidation.
            break;
        case FORM_SHADOW:
            spellId1 = 49868;
            break;
        case FORM_GHOSTWOLF:
            spellId1 = 67116;
            break;
        case FORM_AMBIENT:
        case FORM_GHOUL:
        case FORM_STEALTH:
        case FORM_CREATURECAT:
        case FORM_CREATUREBEAR:
        case FORM_STEVES_GHOUL:
        case FORM_THARONJA_SKELETON:
        case FORM_TEST_OF_STRENGTH:
        case FORM_BLB_PLAYER:
        case FORM_SHADOW_DANCE:
        case FORM_TEST:
        case FORM_ZOMBIE:
        case FORM_UNDEAD:
        case FORM_FRENZY:
        case FORM_NONE:
            break;
    }

    if(apply)
    {
        if (spellId1)
        {
            if (target->GetTypeId() == TYPEID_PLAYER && ((Player*)target)->HasSpellCooldown(spellId1))
                ((Player*)target)->RemoveSpellCooldown(spellId1);

            target->CastSpell(target, spellId1, true, NULL, this );
        }
        if (spellId2)
        {
            if (target->GetTypeId() == TYPEID_PLAYER && ((Player*)target)->HasSpellCooldown(spellId2))
                ((Player*)target)->RemoveSpellCooldown(spellId2);

            target->CastSpell(target, spellId2, true, NULL, this);
        }
        if (spellId3)
        {
            if (target->GetTypeId() == TYPEID_PLAYER && ((Player*)target)->HasSpellCooldown(spellId3))
                ((Player*)target)->RemoveSpellCooldown(spellId3);

            target->CastSpell(target, spellId3, true, NULL, this);
        }

        if (target->GetTypeId() == TYPEID_PLAYER)
        {
            const PlayerSpellMap& sp_list = ((Player *)target)->GetSpellMap();
            for (PlayerSpellMap::const_iterator itr = sp_list.begin(); itr != sp_list.end(); ++itr)
            {
                if (itr->second.state == PLAYERSPELL_REMOVED) continue;
                if (itr->first==spellId1 || itr->first==spellId2 || itr->first==spellId3) continue;
                SpellEntry const *spellInfo = sSpellStore.LookupEntry(itr->first);
                if (!spellInfo || !IsNeedCastSpellAtFormApply(spellInfo, form))
                    continue;
                target->CastSpell(target, itr->first, true, NULL, this);
            }
            // remove auras that do not require shapeshift, but are not active in this specific form (like Improved Barkskin)
            Unit::SpellAuraHolderMap& tAuras = target->GetSpellAuraHolderMap();
            for (Unit::SpellAuraHolderMap::iterator itr = tAuras.begin(); itr != tAuras.end();)
            {
                SpellEntry const *spellInfo = itr->second->GetSpellProto();
                if (itr->second->IsPassive() && (spellInfo->AttributesEx2 & SPELL_ATTR_EX2_NOT_NEED_SHAPESHIFT)
                    && (spellInfo->GetStancesNot() & (1<<(form-1))))
                {
                    target->RemoveAurasDueToSpell(itr->second->GetId());
                    itr = tAuras.begin();
                }
                else
                    ++itr;
            }

            // Master Shapeshifter
            if (MasterShaperSpellId)
            {
                Unit::AuraList const& ShapeShifterAuras = target->GetAurasByType(SPELL_AURA_MOD_HEALING_DONE_PERCENT);
                for(Unit::AuraList::const_iterator i = ShapeShifterAuras.begin(); i != ShapeShifterAuras.end(); ++i)
                {
                    if ((*i)->GetSpellProto()->SpellIconID == 2851)
                    {
                        int32 ShiftMod = (*i)->GetModifier()->m_amount;
                        target->CastCustomSpell(target, MasterShaperSpellId, &ShiftMod, NULL, NULL, true);
                        break;
                    }
                }
            }

            // Leader of the Pack
            if (((Player*)target)->HasSpell(17007))
            {
                SpellEntry const *spellInfo = sSpellStore.LookupEntry(24932);
                if (spellInfo && spellInfo->GetStances() & (1<<(form-1)))
                    target->CastSpell(target, 24932, true, NULL, this);
            }

            // Savage Roar
            if (form == FORM_CAT && ((Player*)target)->HasAura(52610))
                target->CastSpell(target, 62071, true);

            // remove Vengeance Buff on entering cat form
            if (form == FORM_CAT)
                target->RemoveAurasDueToSpell(76691);

            if (form == FORM_BEAR)
            {
                // Item - Druid T13 Feral 4P Bonus (Frenzied Regeneration and Stampede)
                if (target->HasAura(105735))
                    // Mass Regeneration (Bear Form)
                    ((Player*)target)->AddSpellCooldown(105737, 0, time(NULL) + 15);
            }
        }
    }
    else
    {
        if(spellId1)
            target->RemoveAurasDueToSpell(spellId1);
        if(spellId2)
            target->RemoveAurasDueToSpell(spellId2);
        if(spellId3)
            target->RemoveAurasDueToSpell(spellId3);
        if(MasterShaperSpellId)
            target->RemoveAurasDueToSpell(MasterShaperSpellId);

        if (target->GetTypeId() == TYPEID_PLAYER)
        {
            // re-apply passive spells that don't need shapeshift but were inactive in current form:
            const PlayerSpellMap& sp_list = ((Player *)target)->GetSpellMap();
            for (PlayerSpellMap::const_iterator itr = sp_list.begin(); itr != sp_list.end(); ++itr)
            {
                if (itr->second.state == PLAYERSPELL_REMOVED) continue;
                if (itr->first==spellId1 || itr->first==spellId2 || itr->first==spellId3) continue;
                SpellEntry const *spellInfo = sSpellStore.LookupEntry(itr->first);
                if (!spellInfo || !IsPassiveSpell(spellInfo))
                    continue;
                if (spellInfo->HasAttribute(SPELL_ATTR_EX2_NOT_NEED_SHAPESHIFT) && (spellInfo->GetStancesNot() & (1 << (form - 1))))
                    target->CastSpell(target, itr->first, true, NULL, this);
            }
        }

        Unit::SpellAuraHolderMap& tAuras = target->GetSpellAuraHolderMap();
        for (Unit::SpellAuraHolderMap::iterator itr = tAuras.begin(); itr != tAuras.end();)
        {
            if (itr->second->IsRemovedOnShapeLost())
            {
                bool remove = true;
                // Bastion of Defense special check
                if (itr->second->GetId() == 29593 || itr->second->GetId() == 29594)
                {
                    if (Aura* aura = itr->second->GetAuraByEffectIndex(EFFECT_INDEX_0))
                    {
                        target->RemoveAura(aura);
                        remove = false;
                    }
                }

                if (remove)
                {
                    target->RemoveAurasDueToSpell(itr->second->GetId());
                    itr = tAuras.begin();
                }
                else
                    ++itr;
            }
            else
                ++itr;
        }
    }
}

void Aura::HandleAuraEmpathy(bool apply, bool /*Real*/)
{
    if(GetTarget()->GetTypeId() != TYPEID_UNIT)
        return;

    CreatureInfo const * ci = ObjectMgr::GetCreatureTemplate(GetTarget()->GetEntry());
    if(ci && ci->type == CREATURE_TYPE_BEAST)
        GetTarget()->ApplyModUInt32Value(UNIT_DYNAMIC_FLAGS, UNIT_DYNFLAG_SPECIALINFO, apply);
}

void Aura::HandleAuraUntrackable(bool apply, bool /*Real*/)
{
    Unit* target = GetTarget();

    if (apply)
        target->SetByteFlag(UNIT_FIELD_BYTES_1, 2, UNIT_STAND_FLAGS_UNTRACKABLE);
    else
    {
        // do not remove unit flag if there are more than this auraEffect of that kind on unit on unit
        if (target->HasAuraType(SPELL_AURA_UNTRACKABLE))
            return;

        target->RemoveByteFlag(UNIT_FIELD_BYTES_1, 2, UNIT_STAND_FLAGS_UNTRACKABLE);
    }
}

void Aura::HandleAuraModPacify(bool apply, bool /*Real*/)
{
    if (apply)
        GetTarget()->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
    else
        GetTarget()->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PACIFIED);
}

void Aura::HandleAuraModPacifyAndSilence(bool apply, bool Real)
{
    HandleAuraModPacify(apply, Real);
    HandleAuraModSilence(apply, Real);

    if (apply)
    {
        // Wolpertinger Net
        if (GetId() == 41621 && GetTarget()->GetTypeId() == TYPEID_UNIT)
            GetTarget()->SetFlag(UNIT_NPC_FLAGS, UNIT_NPC_FLAG_SPELLCLICK);
    }
}

void Aura::HandleAuraGhost(bool apply, bool /*Real*/)
{
    if(GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    if(apply)
    {
        GetTarget()->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST);
    }
    else
    {
        GetTarget()->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_GHOST);
    }
}

void Aura::HandleAuraAllowFlight(bool apply, bool Real)
{
    // all applied/removed only at real aura add/remove
    if(!Real)
        return;

    // allow fly
    WorldPacket data;
    GetTarget()->BuildMoveSetCanFlyPacket(&data, apply, 0);
    GetTarget()->SendMessageToSet(&data, true);
}

void Aura::HandleModRating(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if(!Real)
        return;

    Unit* target = GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    // Heart's Judgement
    if (GetId() == 91041)
    {
        // Heart's Revelation
        if (Aura* aura = target->GetAura(91027, EFFECT_INDEX_0))
        {
            ChangeAmount(m_modifier.m_amount * aura->GetStackAmount());
            target->RemoveSpellAuraHolder(aura->GetHolder());
        }
    }
    // Heart's Judgement
    else if (GetId() == 92328)
    {
        // Heart's Revelation
        if (Aura* aura = target->GetAura(92325, EFFECT_INDEX_0))
        {
            ChangeAmount(m_modifier.m_amount * aura->GetStackAmount());
            target->RemoveSpellAuraHolder(aura->GetHolder());
        }
    }

    for (uint32 rating = 0; rating < MAX_COMBAT_RATING; ++rating)
        if (m_modifier.m_miscvalue & (1 << rating))
            ((Player*)target)->ApplyRatingMod(CombatRating(rating), m_modifier.m_amount, apply);
}

void Aura::HandleModRatingFromStat(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if(!Real)
        return;

    if(GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;
    // Just recalculate ratings
    for (uint32 rating = 0; rating < MAX_COMBAT_RATING; ++rating)
        if (m_modifier.m_miscvalue & (1 << rating))
            ((Player*)GetTarget())->ApplyRatingMod(CombatRating(rating), 0, apply);
}

void Aura::HandleForceMoveForward(bool apply, bool Real)
{
    if(!Real)
        return;

    if(apply)
        GetTarget()->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FORCE_MOVE);
    else
        GetTarget()->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FORCE_MOVE);
}

void Aura::HandleAuraModExpertise(bool /*apply*/, bool /*Real*/)
{
    if(GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    ((Player*)GetTarget())->UpdateExpertise(BASE_ATTACK);
    ((Player*)GetTarget())->UpdateExpertise(OFF_ATTACK);
}

void Aura::HandleModTargetResistance(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if(!Real)
        return;
    Unit *target = GetTarget();
    // applied to damage as HandleNoImmediateEffect in Unit::CalculateAbsorbAndResist and Unit::CalcArmorReducedDamage
    // show armor penetration
    if (target->GetTypeId() == TYPEID_PLAYER && (m_modifier.m_miscvalue & SPELL_SCHOOL_MASK_NORMAL))
        target->ApplyModInt32Value(PLAYER_FIELD_MOD_TARGET_PHYSICAL_RESISTANCE, m_modifier.m_amount, apply);

    // show as spell penetration only full spell penetration bonuses (all resistances except armor and holy
    if (target->GetTypeId() == TYPEID_PLAYER && (m_modifier.m_miscvalue & SPELL_SCHOOL_MASK_SPELL)==SPELL_SCHOOL_MASK_SPELL)
        target->ApplyModInt32Value(PLAYER_FIELD_MOD_TARGET_RESISTANCE, m_modifier.m_amount, apply);
}

void Aura::HandleModShieldBlockDamage(bool apply, bool /*Real*/)
{
    if (GetTarget()->GetTypeId() == TYPEID_PLAYER)
        ((Player*)GetTarget())->HandleBaseModValue(SHIELD_BLOCK_DAMAGE_VALUE, FLAT_MOD, float(m_modifier.m_amount), apply);
}

void Aura::HandleAuraRetainComboPoints(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if(!Real)
        return;

    Unit *target = GetTarget();

    // combo points was added in SPELL_EFFECT_ADD_COMBO_POINTS handler
    // remove only if aura expire by time (in case combo points amount change aura removed without combo points lost)
    if (!apply && m_removeMode == AURA_REMOVE_BY_EXPIRE && target->GetComboTargetGuid())
        if (Unit* unit = ObjectAccessor::GetUnit(*GetTarget(),target->GetComboTargetGuid()))
            target->AddComboPoints(unit, -m_modifier.m_amount);
}

void Aura::HandleModUnattackable( bool Apply, bool Real )
{
    if(Real && Apply)
     {
        GetTarget()->CombatStop();
        GetTarget()->RemoveAurasWithInterruptFlags(AURA_INTERRUPT_FLAG_IMMUNE_OR_LOST_SELECTION);
     }
    GetTarget()->ApplyModFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE,Apply);
}

void Aura::HandleSpiritOfRedemption( bool apply, bool Real )
{
    // spells required only Real aura add/remove
    if(!Real)
        return;

    Unit *target = GetTarget();

    // prepare spirit state
    if(apply)
    {
        if(target->GetTypeId()==TYPEID_PLAYER)
        {
            // disable breath/etc timers
            ((Player*)target)->StopMirrorTimers();

            // set stand state (expected in this form)
            if(!target->IsStandState())
                target->SetStandState(UNIT_STAND_STATE_STAND);
        }

        target->SetHealth(1);
    }
    // die at aura end
    else
        target->DealDamage(target, target->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, GetSpellProto(), false);
}

void Aura::HandleSchoolAbsorb(bool apply, bool Real)
{
    if(!Real)
        return;

    Unit* caster = GetCaster();
    if(!caster)
        return;

    Unit *target = GetTarget();
    SpellEntry const* spellProto = GetSpellProto();
    SpellClassOptionsEntry const* classOptions = spellProto->GetSpellClassOptions();

    if (apply)
    {
        // prevent double apply bonuses
        if (target->GetTypeId()!=TYPEID_PLAYER || !((Player*)target)->GetSession()->PlayerLoading())
        {
            float DoneActualBenefit = 0.0f;
            float customModifier = 0.0f;
            switch(spellProto->GetSpellFamilyName())
            {
                case SPELLFAMILY_GENERIC:
                    // Stoicism
                    if (spellProto->Id == 70845)
                        DoneActualBenefit = caster->GetMaxHealth() * 0.20f;
                    break;
                case SPELLFAMILY_PRIEST:
                    // Power Word: Shield
                    if (classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x0000000000000001))
                    {
                        float baseAmt = caster->CalculateSpellDamage(target, spellProto, SpellEffectIndex(m_spellEffect->EffectIndex));
                        float spd = caster->SpellBaseHealingBonusDone(GetSpellSchoolMask(spellProto));
                        float spdBonus = 0.0f;
                        if (SpellBonusEntry const* bonus = sSpellMgr.GetSpellBonusData(spellProto->Id))
                            spdBonus = bonus->direct_damage;

                        int32 IMP = 0;
                        int32 SD = 0;

                        Unit::AuraList const& ipwstd = caster->GetAurasByType(SPELL_AURA_DUMMY);
                        for (Unit::AuraList::const_iterator itr = ipwstd.begin(); itr != ipwstd.end(); ++itr)
                        {
                            switch ((*itr)->GetId())
                            {
                                case 14748:     // Improved Power Word: Shield
                                case 14768:
                                    if ((*itr)->GetEffIndex() == EFFECT_INDEX_0)
                                        IMP = (*itr)->GetModifier()->m_amount;
                                    break;
                                case 77484:     // Shield Discipline
                                    if ((*itr)->GetEffIndex() == EFFECT_INDEX_0)
                                        SD = (*itr)->GetModifier()->m_amount;
                                    break;
                            }
                        }

                        customModifier = (baseAmt + spdBonus * spd) * (100.0f + IMP) / 100.0f * (100.0f + SD) / 100.0f;
                        // Item - Priest T13 Healer 4P Bonus (Holy Word and Power Word: Shield)
                        if (Aura* aura = caster->GetAura(105832, EFFECT_INDEX_1))
                            if (roll_chance_i(aura->GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_0)))
                                customModifier *= (100.0f + aura->GetModifier()->m_amount) / 100.0f;
                    }
                    break;
                case SPELLFAMILY_MAGE:
                    if (spellProto->IsFitToFamilyMask(UI64LIT(0x0000000100000000)))
                    {
                        //+80.67% from +spell bonus
                        DoneActualBenefit = caster->SpellBaseDamageBonusDone(GetSpellSchoolMask(spellProto)) * 0.8067f;
                        customModifier = DoneActualBenefit + m_modifier.m_amount;
                        // Glyph of Ice Barrier
                        if (Aura* glyph = caster->GetAura(63095, EFFECT_INDEX_0))
                            customModifier *= (glyph->GetModifier()->m_amount + 100.0f) / 100.0f;
                    }
                    // Mage Ward
                    // +80.7% from +spell bonus
                    else if (spellProto->Id == 543)
                        DoneActualBenefit = caster->SpellBaseDamageBonusDone(GetSpellSchoolMask(spellProto)) * 0.807f;
                    break;
                case SPELLFAMILY_WARLOCK:
                    // Shadow Ward and Nether Ward
                    if (spellProto->IsFitToFamilyMask(UI64LIT(0x0000000000000000), 0xA0000000))
                        //+30% from +spell bonus
                        DoneActualBenefit = caster->SpellBaseDamageBonusDone(GetSpellSchoolMask(spellProto)) * 0.807f;
                    break;
                case SPELLFAMILY_PALADIN:
                    // Sacred Shield
                    if (spellProto->Id == 96263)
                    {
                        // +75% from spell power
                        DoneActualBenefit = caster->GetTotalAttackPowerValue(BASE_ATTACK) * 2.8f;
                    }
                    break;
                default:
                    break;
            }

            if (customModifier == 0.0f)
            {
                DoneActualBenefit *= caster->CalculateLevelPenalty(GetSpellProto());
                ChangeAmount(m_modifier.m_amount + (int32)DoneActualBenefit);
            }
            else
            {
                customModifier *= caster->CalculateLevelPenalty(GetSpellProto());
                ChangeAmount((int32)customModifier);
            }
        }
    }
    else
    {
        if (caster &&
            // Power Word: Shield
            classOptions && classOptions->SpellFamilyName == SPELLFAMILY_PRIEST && spellProto->GetMechanic() == MECHANIC_SHIELD &&
            (classOptions->SpellFamilyFlags & UI64LIT(0x0000000000000001)) &&
            // completely absorbed or dispelled
            (m_removeMode == AURA_REMOVE_BY_SHIELD_BREAK || m_removeMode == AURA_REMOVE_BY_DISPEL))
        {
            Unit::AuraList const& vDummyAuras = caster->GetAurasByType(SPELL_AURA_DUMMY);
            for (Unit::AuraList::const_iterator itr = vDummyAuras.begin(); itr != vDummyAuras.end(); ++itr)
            {
                SpellEntry const* vSpell = (*itr)->GetSpellProto();

                // Rapture
                if (vSpell->GetSpellFamilyName() == SPELLFAMILY_PRIEST && vSpell->SpellIconID == 2894 && (*itr)->GetEffIndex() == EFFECT_INDEX_0)
                {
                    int32 bp = caster->GetMaxPower(POWER_MANA) * (*itr)->GetModifier()->m_amount / 100;
                    // Item - Priest T13 Healer 4P Bonus (Holy Word and Power Word: Shield)
                    if (Aura* aura = caster->GetAura(105832, EFFECT_INDEX_1))
                        bp += bp * aura->GetModifier()->m_amount / 100.0f;
                    caster->CastCustomSpell(caster, 47755, &bp, NULL, NULL, true);
                    break;
                }
            }
        }
        // Mage Ward
        if (spellProto->Id == 543)
        {
            // only from fully depleted shield
            if (m_modifier.m_amount > 0)
                return;

            Unit* caster = GetCaster();
            if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
                return;

            // Molten Shields
            if (caster->HasAura(11094))
                target->CastSpell(target, 31643, true);
            return;
        }
        // Shield of Runes (Runemaster Molgeim: Ulduar)
        else if ((GetId() == 62274 || GetId() == 63489) && m_removeMode == AURA_REMOVE_BY_SHIELD_BREAK)
        {
            uint32 trigger_spell_Id = GetId() == 62274 ? 62277 : 63967;
            target->CastSpell(target, trigger_spell_Id, true);
        }
        // Egg Shell
        else if (GetId() == 91296 || GetId() == 91308)
        {
            if (Unit* caster = GetCaster())
                caster->CastSpell(caster, GetId() == 91296 ? 91305 : 91310, true);
        }
        // Stay of Execution
        else if (GetId() == 96988 || GetId() == 97145)
        {
            int32 absorbed = target->CalculateSpellDamage(target, GetSpellProto(), EFFECT_INDEX_0) - m_modifier.m_amount;
            if (absorbed > 0)
            {
                int32 triggered_spell = 96993;
                int32 bp = absorbed * 40 / 100 / GetSpellAuraMaxTicks(triggered_spell);
                target->CastCustomSpell(target, triggered_spell, &bp, NULL, NULL, true);
            }
        }
    }
}

void Aura::PeriodicTick()
{
    Unit *target = GetTarget();
    SpellEntry const* spellProto = GetSpellProto();
    SpellClassOptionsEntry const* classOptions = spellProto->GetSpellClassOptions();

    switch(m_modifier.m_auraname)
    {
        case SPELL_AURA_PERIODIC_DAMAGE:
        case SPELL_AURA_PERIODIC_DAMAGE_PERCENT:
        {
            // don't damage target if not alive, possible death persistent effects
            if (!target->isAlive())
                return;

            Unit *pCaster = GetCaster();
            if(!pCaster)
                return;

            if (target->hasUnitState(UNIT_STAT_ISOLATED))
                return;

            if( spellProto->GetSpellEffectIdByIndex(GetEffIndex()) == SPELL_EFFECT_PERSISTENT_AREA_AURA &&
                pCaster->SpellHitResult(target, spellProto, false) != SPELL_MISS_NONE)
                return;

            // Check for immune (not use charges)
            if(target->IsImmunedToDamage(spellProto))
                return;

            // some auras remove at specific health level or more
            switch (GetId())
            {
                case 689:       // Drain Life
                case 89420:     // Drain Life (Soulburn)
                {
                    int32 bp = 0;
                    if (pCaster->GetHealthPercent() < 25.0f)
                    {
                        // Drain Life (Health Energize)
                        Unit::AuraList const& mDummyAuras= pCaster->GetAurasByType(SPELL_AURA_DUMMY);
                        for(Unit::AuraList::const_iterator i = mDummyAuras.begin(); i != mDummyAuras.end(); ++i)
                        {
                            if ((*i)->GetSpellProto()->SpellIconID == 3223 && (*i)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_WARLOCK &&
                                (*i)->GetEffIndex() == EFFECT_INDEX_0)
                            {
                                bp = 2 + (*i)->GetModifier()->m_amount;
                                break;
                            }
                        }
                    }

                    // Drain Life (Health Energize)
                    if (bp)
                        pCaster->CastCustomSpell(pCaster, 89653, &bp, NULL, NULL, true);
                    else
                        pCaster->CastSpell(pCaster, 89653, true);
                    break;
                }
                case 31956:
                case 35321:
                case 38363:
                case 38801:
                case 39215:
                case 43093:
                case 48920:
                case 70292:
                case 71316:
                case 71317:
                {
                    if (target->GetHealth() == target->GetMaxHealth() )
                    {
                        target->RemoveAurasDueToSpell(GetId());
                        return;
                    }
                    break;
                }
                case 38772:
                {
                    SpellEffectEntry const * effect = GetSpellEffect();
                    uint32 percent =
                        GetEffIndex() < EFFECT_INDEX_2 && effect->Effect == SPELL_EFFECT_DUMMY ?
                        pCaster->CalculateSpellDamage(target, spellProto, SpellEffectIndex(GetEffIndex() + 1)) :
                        100;
                    if (target->GetHealth() * 100 >= target->GetMaxHealth() * percent )
                    {
                        target->RemoveAurasDueToSpell(GetId());
                        return;
                    }
                    break;
                }
                case 70541: // Infest (Lich King)
                case 73779:
                case 73780:
                case 73781:
                {
                    if (target->GetHealth() >= target->GetMaxHealth() * 0.9f )
                    {
                        target->RemoveAurasDueToSpell(GetId());
                        return;
                    }
                    /*else
                    {
                        // increasing damage (15% more each tick)
                        // don't increase first tick damage
                        if (GetModifier()->m_miscvalue > 0)
                            GetModifier()->m_amount = GetModifier()->m_amount * 1.15f;
                        else
                            GetModifier()->m_miscvalue += 1;
                    }*/
                    break;
                }
                case 70672: // Gaseous Bloat (Putricide)
                case 72455:
                case 72832:
                case 72833:
                {
                    // drop 1 stack
                    if (GetHolder()->ModStackAmount(-1))
                    {
                        target->RemoveAurasDueToSpell(GetId());
                        return;
                    }

                    break;
                }
                case 74562: // SPELL_FIERY_COMBUSTION - Ruby sanctum boss Halion, added mark (74567, dummy) every tick
                {
                    target->CastSpell(target, 74567, true, NULL, NULL, GetCasterGuid());
                    break;
                }
                case 74792: // SPELL_SOUL_CONSUMPTION - Ruby sanctum boss Halion, added mark (74795, dummy) every tick
                {
                    target->CastSpell(target, 74795, true, NULL, NULL, GetCasterGuid());
                    break;
                }
                case 67297:
                case 65950:
                    pCaster->CastSpell(target, 65951, true);
                    break;
                case 66001:
                case 67282:
                    pCaster->CastSpell(target, 66002, true);
                    break;
                case 67281:
                case 67283:
                    pCaster->CastSpell(target, 66000, true);
                    break;
                case 67296:
                case 67298:
                    pCaster->CastSpell(target, 65952, true);
                    break;
                // Unbound Plague (Putricide)
                /*case 70911:
                case 72854:
                case 72855:
                case 72856:
                    m_modifier.m_miscvalue += 1; // store ticks number in miscvalue
                    m_modifier.m_amount = GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_0) * pow(2.7f, m_modifier.m_miscvalue * 0.223f);
                    break;
                // Boiling Blood (Saurfang)
                case 72385:
                case 72441:
                case 72442:
                case 72443:
                    target->CastSpell(target, 72202, true); // Blood Link
                    break;*/
                // Gushing Wound
                case 100024:
                case 100721:
                case 100722:
                case 100723:
                {
                    if (target->GetHealthPercent() < 50.0f)
                    {
                        target->RemoveAurasDueToSpell(GetId());
                        return;
                    }
                    break;
                }
                default:
                    break;
            }

            uint32 absorb = 0;
            uint32 resist = 0;
            CleanDamage cleanDamage =  CleanDamage(0, BASE_ATTACK, MELEE_HIT_NORMAL );

            // ignore non positive values (can be result apply spellmods to aura damage
            uint32 amount = m_modifier.m_amount > 0 ? m_modifier.m_amount : 0;

            //megai2: FUCK initialize variables ALWAYS!
            uint32 pdamage = 0;

            if(m_modifier.m_auraname == SPELL_AURA_PERIODIC_DAMAGE)
                pdamage = amount;
            else
                pdamage = uint32(target->GetMaxHealth()*amount/100);

            // SpellDamageBonus for magic spells
            uint32 dmgClass = spellProto->GetDmgClass();
            if(dmgClass == SPELL_DAMAGE_CLASS_NONE || dmgClass == SPELL_DAMAGE_CLASS_MAGIC)
                pdamage = target->SpellDamageBonusTaken(pCaster, spellProto, pdamage, DOT, GetStackAmount());
            // MeleeDamagebonus for weapon based spells
            else
            {
                WeaponAttackType attackType = GetWeaponAttackType(spellProto);
                pdamage = target->MeleeDamageBonusTaken(pCaster, pdamage, attackType, spellProto, DOT, GetStackAmount());
            }

            // Calculate armor mitigation if it is a physical spell
            // But not for bleed mechanic spells
            if (GetSpellSchoolMask(spellProto) & SPELL_SCHOOL_MASK_NORMAL &&
                GetEffectMechanic(spellProto, m_effIndex) != MECHANIC_BLEED)
            {
                uint32 pdamageReductedArmor = pCaster->CalcArmorReducedDamage(target, pdamage, spellProto);
                cleanDamage.damage += pdamage - pdamageReductedArmor;
                pdamage = pdamageReductedArmor;
            }

            // Curse of Agony damage-per-tick calculation
            if (classOptions && classOptions->SpellFamilyName==SPELLFAMILY_WARLOCK && (classOptions->SpellFamilyFlags & UI64LIT(0x0000000000000400)) && spellProto->SpellIconID==544)
            {
                // 1..4 ticks, 1/2 from normal tick damage
                if (GetAuraTicks() <= 4)
                    pdamage = pdamage/2;
                // 9..12 ticks, 3/2 from normal tick damage
                else if(GetAuraTicks() >= 9)
                    pdamage += (pdamage + 1) / 2;       // +1 prevent 0.5 damage possible lost at 1..4 ticks
                // 5..8 ticks have normal tick damage
            }

            // This method can modify pdamage
            bool isCrit = IsCritFromAbilityAura(pCaster, pdamage);

            // send critical in hit info for threat calculation
            if (isCrit)
            {
                cleanDamage.hitOutCome = MELEE_HIT_CRIT;
                // Resilience - reduce crit damage
                //pdamage -= target->GetCritDamageReduction(pdamage);
            }

            // only from players
            // FIXME: need use SpellDamageBonus instead?
            if (pCaster->GetTypeId() == TYPEID_PLAYER)
                pdamage -= target->GetDamageReduction(pdamage);

            target->CalculateDamageAbsorbAndResist(pCaster, GetSpellSchoolMask(spellProto), DOT, pdamage, &absorb, &resist, spellProto, Spell::CanReflect(spellProto));

            DETAIL_FILTER_LOG(LOG_FILTER_PERIODIC_AFFECTS, "PeriodicTick: %s attacked %s for %u dmg inflicted by %u abs is %u",
                GetCasterGuid().GetString().c_str(), target->GetGuidStr().c_str(), pdamage, GetId(),absorb);

            pCaster->DealDamageMods(target, pdamage, &absorb);

            // Set trigger flag
            uint32 procAttacker = PROC_FLAG_ON_DO_PERIODIC; //  | PROC_FLAG_SUCCESSFUL_HARMFUL_SPELL_HIT;
            uint32 procVictim   = PROC_FLAG_ON_TAKE_PERIODIC;// | PROC_FLAG_TAKEN_HARMFUL_SPELL_HIT;
            uint32 procEx = isCrit ? PROC_EX_CRITICAL_HIT : PROC_EX_NORMAL_HIT;

            pdamage = (pdamage <= absorb + resist) ? 0 : (pdamage - absorb - resist);

            uint32 overkill = pdamage > target->GetHealth() ? pdamage - target->GetHealth() : 0;
            SpellPeriodicAuraLogInfo pInfo(this, pdamage, overkill, absorb, resist, 0.0f, isCrit);
            target->SendPeriodicAuraLog(&pInfo);

            if (pdamage)
            {
                procVictim|=PROC_FLAG_TAKEN_ANY_DAMAGE;

                // Lacerate
                if (GetId() == 33745)
                {
                    if (Unit* caster = GetCaster())
                    {
                        // Berserk passive - removes Mangle (Bear) cooldown on Lacerate tick
                        // original spell missing in dbc
                        if (caster->HasSpell(50334) && roll_chance_i(50))
                            caster->CastSpell(caster, 93622, true);
                    }
                }

            }

            pCaster->ProcDamageAndSpell(target, procAttacker, procVictim, procEx, pdamage, absorb, BASE_ATTACK, spellProto);

            pCaster->DealDamage(target, pdamage, &cleanDamage, DOT, GetSpellSchoolMask(spellProto), spellProto, true);

            break;
        }
        case SPELL_AURA_PERIODIC_LEECH:
        case SPELL_AURA_PERIODIC_HEALTH_FUNNEL:
        {
            // don't damage target if not alive, possible death persistent effects
            if (!target->isAlive())
                return;

            Unit *pCaster = GetCaster();
            if(!pCaster)
                return;

            if(!pCaster->isAlive())
                return;

            if (target->hasUnitState(UNIT_STAT_ISOLATED))
                return;

            if( spellProto->GetSpellEffectIdByIndex(GetEffIndex()) == SPELL_EFFECT_PERSISTENT_AREA_AURA &&
                pCaster->SpellHitResult(target, spellProto, false) != SPELL_MISS_NONE)
                return;

            // Check for immune
            if(target->IsImmunedToDamage(spellProto))
                return;

            uint32 absorb=0;
            uint32 resist=0;
            CleanDamage cleanDamage =  CleanDamage(0, BASE_ATTACK, MELEE_HIT_NORMAL );

            uint32 pdamage = m_modifier.m_amount > 0 ? m_modifier.m_amount : 0;

            //Calculate armor mitigation if it is a physical spell
            if (GetSpellSchoolMask(spellProto) & SPELL_SCHOOL_MASK_NORMAL)
            {
                uint32 pdamageReductedArmor = pCaster->CalcArmorReducedDamage(target, pdamage, spellProto);
                cleanDamage.damage += pdamage - pdamageReductedArmor;
                pdamage = pdamageReductedArmor;
            }

            pdamage = target->SpellDamageBonusTaken(pCaster, spellProto, pdamage, DOT, GetStackAmount());

            bool isCrit = IsCritFromAbilityAura(pCaster, pdamage);

            // send critical in hit info for threat calculation
            if (isCrit)
            {
                cleanDamage.hitOutCome = MELEE_HIT_CRIT;
                // Resilience - reduce crit damage
                //pdamage -= target->GetCritDamageReduction(pdamage);
            }

            // only from players
            // FIXME: need use SpellDamageBonus instead?
            if (GetCasterGuid().IsPlayer())
                pdamage -= target->GetDamageReduction(pdamage);

            target->CalculateDamageAbsorbAndResist(pCaster, GetSpellSchoolMask(spellProto), DOT, pdamage, &absorb, &resist, spellProto, Spell::CanReflect(spellProto));

            DETAIL_FILTER_LOG(LOG_FILTER_PERIODIC_AFFECTS, "PeriodicTick: %s health leech of %s for %u dmg inflicted by %u abs is %u",
                GetCasterGuid().GetString().c_str(), target->GetGuidStr().c_str(), pdamage, GetId(),absorb);

            pCaster->DealDamageMods(target, pdamage, &absorb);

            pCaster->SendSpellNonMeleeDamageLog(target, GetId(), pdamage, GetSpellSchoolMask(spellProto), absorb, resist, false, 0, isCrit);

            float multiplier = m_spellEffect->EffectMultipleValue > 0 ? m_spellEffect->EffectMultipleValue : 1;

            // Set trigger flag
            uint32 procAttacker = PROC_FLAG_ON_DO_PERIODIC; //  | PROC_FLAG_SUCCESSFUL_HARMFUL_SPELL_HIT;
            uint32 procVictim   = PROC_FLAG_ON_TAKE_PERIODIC;// | PROC_FLAG_TAKEN_HARMFUL_SPELL_HIT;
            uint32 procEx = isCrit ? PROC_EX_CRITICAL_HIT : PROC_EX_NORMAL_HIT;

            pdamage = (pdamage <= absorb + resist) ? 0 : (pdamage-absorb-resist);
            if (pdamage)
                procVictim|=PROC_FLAG_TAKEN_ANY_DAMAGE;

            pCaster->ProcDamageAndSpell(target, procAttacker, procVictim, procEx, pdamage, absorb, BASE_ATTACK, spellProto);
            int32 new_damage = pCaster->DealDamage(target, pdamage, &cleanDamage, DOT, GetSpellSchoolMask(spellProto), spellProto, false);

            if (!target->isAlive() && pCaster->IsNonMeleeSpellCasted(false))
                for (uint32 i = CURRENT_FIRST_NON_MELEE_SPELL; i < CURRENT_MAX_SPELL; ++i)
                    if (Spell* spell = pCaster->GetCurrentSpell(CurrentSpellTypes(i)))
                        if (spell->m_spellInfo->Id == GetId())
                            spell->cancel();

            if(Player *modOwner = pCaster->GetSpellModOwner())
                modOwner->ApplySpellMod(GetId(), SPELLMOD_MULTIPLE_VALUE, multiplier);

            int32 heal = pCaster->SpellHealingBonusTaken(pCaster, spellProto, int32(new_damage * multiplier), DOT, GetStackAmount());

            uint32 absorbHeal = 0;
            pCaster->CalculateHealAbsorb(heal, &absorbHeal);

            int32 gain = pCaster->DealHeal(pCaster, heal - absorbHeal, spellProto, false, absorbHeal);
            pCaster->getHostileRefManager().threatAssist(pCaster, gain * 0.5f * sSpellMgr.GetSpellThreatMultiplier(spellProto), spellProto);
            break;
        }
        case SPELL_AURA_PERIODIC_HEAL:
        case SPELL_AURA_OBS_MOD_HEALTH:
        {
            // don't heal target if not alive, mostly death persistent effects from items
            if (!target->isAlive())
                return;

            Unit *pCaster = GetCaster();
            if(!pCaster)
                return;

            // heal for caster damage (must be alive)
            if(target != pCaster && spellProto->SpellVisual[0] == 163 && !pCaster->isAlive())
                return;

            // ignore non positive values (can be result apply spellmods to aura damage
            uint32 amount = m_modifier.m_amount > 0 ? m_modifier.m_amount : 0;

            uint32 pdamage = 0;

            if(m_modifier.m_auraname==SPELL_AURA_OBS_MOD_HEALTH)
                pdamage = uint32(target->GetMaxHealth() * amount / 100);
            else
            {
                pdamage = amount;

                // Wild Growth (1/7 - 6 + 2*ramainTicks) %
                if (classOptions && classOptions->SpellFamilyName == SPELLFAMILY_DRUID && spellProto->SpellIconID == 2864)
                {
                    int32 ticks = GetAuraMaxTicks();
                    int32 remainingTicks = ticks - GetAuraTicks();
                    int32 addition = int32(amount)*ticks*(-6+2*remainingTicks)/100;

                    if (GetAuraTicks() != 1)
                        // Item - Druid T10 Restoration 2P Bonus
                        if (Aura *aura = pCaster->GetAura(70658, EFFECT_INDEX_0))
                            addition += abs(int32((addition * aura->GetModifier()->m_amount) / ((ticks-1)* 100)));

                    pdamage = int32(pdamage) + addition;
                }
                // Second Wind
                else if (spellProto->Id == 29842 || spellProto->Id == 29841)
                    pdamage = int32(amount * target->GetMaxHealth() / 100.0f);
                // Recuperate
                else if (spellProto->Id == 73651)
                {
                    float healthPct = amount;
                    if (target->GetTypeId() == TYPEID_PLAYER)
                    {
                        // Improved Recuperate
                        if (SpellEntry const * spellInfo = ((Player*)target)->GetKnownTalentRankById(6395))
                            healthPct += spellInfo->CalculateSimpleValue(EFFECT_INDEX_0) / 1000.0f;
                    }
                    pdamage = int32(healthPct * target->GetMaxHealth() / 100.0f);
                }
            }

            // Blood Craze
            if (spellProto->Id == 16488 || spellProto->Id == 16490 || spellProto->Id == 16491)
                pdamage /= 10;

            // Amaru: lifebloom special case
            if (GetSpellProto()->IsFitToFamily(SPELLFAMILY_DRUID, UI64LIT(0x1000000000)))
            {
                uint32 stackAmount = (GetStackAmount() > 0 ? GetStackAmount() : 1);
                pdamage = target->SpellHealingBonusTaken(pCaster, spellProto, pdamage / stackAmount, DOT) * stackAmount;
            }
            else
                pdamage = target->SpellHealingBonusTaken(pCaster, spellProto, pdamage, DOT, GetStackAmount());

            // This method can modify pdamage
            bool isCrit = IsCritFromAbilityAura(pCaster, pdamage);

            uint32 absorbHeal = 0;
            pCaster->CalculateHealAbsorb(pdamage, &absorbHeal);
            pdamage -= absorbHeal;

            DETAIL_FILTER_LOG(LOG_FILTER_PERIODIC_AFFECTS, "PeriodicTick: %s heal of %s for %u health  (absorbed %u) inflicted by %u",
                GetCasterGuid().GetString().c_str(), target->GetGuidStr().c_str(), pdamage, absorbHeal, GetId());

            int32 gain = target->ModifyHealth(pdamage);
            SpellPeriodicAuraLogInfo pInfo(this, pdamage, (pdamage - uint32(gain)), absorbHeal, 0, 0.0f, isCrit);
            target->SendPeriodicAuraLog(&pInfo);

            // Set trigger flag
            uint32 procAttacker = PROC_FLAG_ON_DO_PERIODIC;
            uint32 procVictim   = PROC_FLAG_ON_TAKE_PERIODIC;
            uint32 procEx = PROC_EX_PERIODIC_POSITIVE | (isCrit ? PROC_EX_CRITICAL_HIT : PROC_EX_NORMAL_HIT);
            pCaster->ProcDamageAndSpell(target, procAttacker, procVictim, procEx, gain, absorbHeal, BASE_ATTACK, spellProto);

            // add HoTs to amount healed in bgs
            if( pCaster->GetTypeId() == TYPEID_PLAYER )
                if( BattleGround *bg = ((Player*)pCaster)->GetBattleGround() )
                    bg->UpdatePlayerScore(((Player*)pCaster), SCORE_HEALING_DONE, gain);

            target->getHostileRefManager().threatAssist(pCaster, float(gain) * 0.5f * sSpellMgr.GetSpellThreatMultiplier(spellProto), spellProto);

            // Health Funnel
            // heal for caster damage
            if(target != pCaster && spellProto->SpellVisual[0] == 163)
            {
                int32 dmg = spellProto->CalculateSimpleValue(EFFECT_INDEX_0);
                if (dmg > gain)
                    dmg = gain;
                if(pCaster->GetHealth() <= dmg && pCaster->GetTypeId()==TYPEID_PLAYER)
                {
                    pCaster->RemoveAurasDueToSpell(GetId());

                    // finish current generic/channeling spells, don't affect autorepeat
                    pCaster->FinishSpell(CURRENT_GENERIC_SPELL);
                    pCaster->FinishSpell(CURRENT_CHANNELED_SPELL);
                }
                else
                {
                    uint32 damage = dmg;
                    uint32 absorb = 0;
                    pCaster->DealDamageMods(pCaster, damage, &absorb);
                    pCaster->SendSpellNonMeleeDamageLog(pCaster, GetId(), damage, GetSpellSchoolMask(spellProto), absorb, 0, false, 0, isCrit);

                    CleanDamage cleanDamage =  CleanDamage(0, BASE_ATTACK, MELEE_HIT_NORMAL );
                    pCaster->DealDamage(pCaster, damage, &cleanDamage, NODAMAGE, GetSpellSchoolMask(spellProto), spellProto, true);
                }
            }

            // Light's Favor (Lich King)
            // recalculate bonus damage done after each tick
            if (GetId() == 69382)
            {
                if (Aura *aur = GetHolder()->GetAuraByEffectIndex(EFFECT_INDEX_1))
                    aur->GetModifier()->m_amount = int32(target->GetHealthPercent());
            }

//            uint32 procAttacker = PROC_FLAG_ON_DO_PERIODIC;//   | PROC_FLAG_SUCCESSFUL_HEAL;
//            uint32 procVictim   = 0;//ROC_FLAG_ON_TAKE_PERIODIC | PROC_FLAG_TAKEN_HEAL;
            // ignore item heals
//            if(procSpell && !haveCastItem)
//                pCaster->ProcDamageAndSpell(target, procAttacker, procVictim, PROC_EX_NORMAL_HIT, pdamage, BASE_ATTACK, spellProto);
            break;
        }
        case SPELL_AURA_PERIODIC_MANA_LEECH:
        {
            // don't damage target if not alive, possible death persistent effects
            if (!target->isAlive())
                return;

            if (m_modifier.m_miscvalue < 0 || m_modifier.m_miscvalue >= MAX_POWERS)
                return;

            Powers power = Powers(m_modifier.m_miscvalue);

            // power type might have changed between aura applying and tick (druid's shapeshift)
            if (target->getPowerType() != power)
                return;

            Unit* pCaster = GetCaster();
            if (!pCaster || !pCaster->isAlive())
                return;

            if (target->hasUnitState(UNIT_STAT_ISOLATED))
                return;

            if( GetSpellProto()->GetSpellEffectIdByIndex(GetEffIndex()) == SPELL_EFFECT_PERSISTENT_AREA_AURA &&
                pCaster->SpellHitResult(target, spellProto, false) != SPELL_MISS_NONE)
                return;

            // Check for immune (not use charges)
            if (target->IsImmunedToDamage(spellProto))
                return;

            // ignore non positive values (can be result apply spellmods to aura damage
            uint32 pdamage = m_modifier.m_amount > 0 ? m_modifier.m_amount : 0;

            // Special case: draining x% of mana (up to a maximum of 2*x% of the caster's maximum mana)
            // It's mana percent cost spells, m_modifier.m_amount is percent drain from target
            if (spellProto->GetManaCostPercentage())
            {
                // max value
                uint32 maxmana = pCaster->GetMaxPower(power)  * pdamage * 2 / 100;
                pdamage = target->GetMaxPower(power) * pdamage / 100;
                if (pdamage > maxmana)
                    pdamage = maxmana;
            }

            DETAIL_FILTER_LOG(LOG_FILTER_PERIODIC_AFFECTS, "PeriodicTick: %s power leech of %s for %u dmg inflicted by %u",
                GetCasterGuid().GetString().c_str(), target->GetGuidStr().c_str(), pdamage, GetId());

            int32 drain_amount = target->GetPower(power) > pdamage ? pdamage : target->GetPower(power);

            // resilience reduce mana draining effect at spell crit damage reduction (added in 2.4)
            if (power == POWER_MANA)
                drain_amount -= target->GetCritDamageReduction(drain_amount);

            target->ModifyPower(power, -drain_amount);

            float gain_multiplier = 0.0f;

            if (pCaster->GetMaxPower(power) > 0)
            {
                gain_multiplier = m_spellEffect->EffectMultipleValue;

                if (Player *modOwner = pCaster->GetSpellModOwner())
                    modOwner->ApplySpellMod(GetId(), SPELLMOD_MULTIPLE_VALUE, gain_multiplier);
            }

            SpellPeriodicAuraLogInfo pInfo(this, drain_amount, 0, 0, 0, gain_multiplier);
            target->SendPeriodicAuraLog(&pInfo);

            if (int32 gain_amount = int32(drain_amount * gain_multiplier))
            {
                int32 gain = pCaster->ModifyPower(power, gain_amount);

                if (GetId() == 5138)                        // Drain Mana
                    if (Aura* petPart = GetHolder()->GetAuraByEffectIndex(EFFECT_INDEX_1))
                        if (int pet_gain = gain_amount * petPart->GetModifier()->m_amount / 100)
                            pCaster->CastCustomSpell(pCaster, 32554, &pet_gain, NULL, NULL, true);

                target->AddThreat(pCaster, float(gain) * 0.5f, pInfo.critical, GetSpellSchoolMask(spellProto), spellProto);
            }

            // Some special cases
            switch (GetId())
            {
                case 32960:                                 // Mark of Kazzak
                {
                    if (target->GetTypeId() == TYPEID_PLAYER && target->getPowerType() == POWER_MANA)
                    {
                        // Drain 5% of target's mana
                        pdamage = target->GetMaxPower(POWER_MANA) * 5 / 100;
                        drain_amount = target->GetPower(POWER_MANA) > pdamage ? pdamage : target->GetPower(POWER_MANA);
                        target->ModifyPower(POWER_MANA, -drain_amount);

                        SpellPeriodicAuraLogInfo pInfo(this, drain_amount, 0, 0, 0, 0.0f);
                        target->SendPeriodicAuraLog(&pInfo);
                    }
                    // no break here
                }
                case 21056:                                 // Mark of Kazzak
                case 31447:                                 // Mark of Kaz'rogal
                {
                    uint32 triggerSpell = 0;
                    switch (GetId())
                    {
                        case 21056: triggerSpell = 21058; break;
                        case 31447: triggerSpell = 31463; break;
                        case 32960: triggerSpell = 32961; break;
                    }
                    if (target->GetTypeId() == TYPEID_PLAYER && target->GetPower(power) == 0)
                    {
                        target->CastSpell(target, triggerSpell, true, NULL, this);
                        target->RemoveAurasDueToSpell(GetId());
                    }
                    break;
                }
            }
            break;
        }
        case SPELL_AURA_PERIODIC_ENERGIZE:
        {
            // don't energize target if not alive, possible death persistent effects
            if (!target->isAlive() || target->IsImmuneToSpell(GetSpellProto(), target->GetObjectGuid() == GetCasterGuid()))
                return;

            // ignore non positive values (can be result apply spellmods to aura damage
            uint32 pdamage = m_modifier.m_amount > 0 ? m_modifier.m_amount : 0;

            DETAIL_FILTER_LOG(LOG_FILTER_PERIODIC_AFFECTS, "PeriodicTick: %s energize %s for %u dmg inflicted by %u",
                GetCasterGuid().GetString().c_str(), target->GetGuidStr().c_str(), pdamage, GetId());

            if(m_modifier.m_miscvalue < 0 || m_modifier.m_miscvalue >= MAX_POWERS)
                break;

            Powers power = Powers(m_modifier.m_miscvalue);

            if (target->GetMaxPower(power) == 0)
                break;

            SpellPeriodicAuraLogInfo pInfo(this, pdamage, 0, 0, 0, 0.0f);
            target->SendPeriodicAuraLog(&pInfo);

            int32 gain = target->ModifyPower(power,pdamage);

            if(Unit* pCaster = GetCaster())
                target->getHostileRefManager().threatAssist(pCaster, float(gain) * 0.5f * sSpellMgr.GetSpellThreatMultiplier(spellProto), spellProto);
            break;
        }
        case SPELL_AURA_OBS_MOD_ENERGY:
        {
            if(m_modifier.m_miscvalue < 0 || m_modifier.m_miscvalue >= MAX_POWERS)
                break;

            // don't energize target if not alive, possible death persistent effects
            if (!target->isAlive() || target->IsImmuneToSpell(GetSpellProto(), target->GetObjectGuid() == GetCasterGuid()))
                return;

            // ignore non positive values (can be result apply spellmods to aura damage
            uint32 amount = m_modifier.m_amount > 0 ? m_modifier.m_amount : 0;

            uint32 pdamage = uint32(target->GetMaxPower(Powers(m_modifier.m_miscvalue)) * amount / 100);

            DETAIL_FILTER_LOG(LOG_FILTER_PERIODIC_AFFECTS, "PeriodicTick: %s energize %s for %u energy %u inflicted by %u",
                GetCasterGuid().GetString().c_str(), target->GetGuidStr().c_str(), pdamage, m_modifier.m_miscvalue, GetId());

            if (target->GetMaxPower(Powers(m_modifier.m_miscvalue)) == 0)
                break;

            SpellPeriodicAuraLogInfo pInfo(this, pdamage, 0, 0, 0, 0.0f);
            target->SendPeriodicAuraLog(&pInfo);

            int32 gain = target->ModifyPower(Powers(m_modifier.m_miscvalue), pdamage);

            if (Unit* pCaster = GetCaster())
                target->getHostileRefManager().threatAssist(pCaster, float(gain) * 0.5f * sSpellMgr.GetSpellThreatMultiplier(spellProto), spellProto);
            break;
        }
        case SPELL_AURA_POWER_BURN_ENERGY:
        {
            // don't mana burn target if not alive, possible death persistent effects
            if (!target->isAlive())
                return;

            Unit *pCaster = GetCaster();
            if(!pCaster)
                return;

            if (target->hasUnitState(UNIT_STAT_ISOLATED))
                return;

            // Check for immune (not use charges)
            if(target->IsImmunedToDamage(spellProto))
                return;

            int32 pdamage = m_modifier.m_amount > 0 ? m_modifier.m_amount : 0;

            Powers powerType = Powers(m_modifier.m_miscvalue);

            if(!target->isAlive() || target->getPowerType() != powerType)
                return;

            // resilience reduce mana draining effect at spell crit damage reduction (added in 2.4)
            if (powerType == POWER_MANA)
                pdamage -= target->GetCritDamageReduction(pdamage);

            uint32 gain = uint32(-target->ModifyPower(powerType, -pdamage));

            gain = uint32(gain * m_spellEffect->EffectMultipleValue);

            // maybe has to be sent different to client, but not by SMSG_PERIODICAURALOG
            SpellNonMeleeDamage damageInfo(pCaster, target, spellProto->Id, SpellSchoolMask(spellProto->SchoolMask));
            pCaster->CalculateSpellDamage(&damageInfo, gain, spellProto);

            damageInfo.target->CalculateAbsorbResistBlock(pCaster, &damageInfo, spellProto);

            pCaster->DealDamageMods(damageInfo.target, damageInfo.damage, &damageInfo.absorb);

            pCaster->SendSpellNonMeleeDamageLog(&damageInfo);

            // Set trigger flag
            uint32 procAttacker = PROC_FLAG_ON_DO_PERIODIC; //  | PROC_FLAG_SUCCESSFUL_HARMFUL_SPELL_HIT;
            uint32 procVictim   = PROC_FLAG_ON_TAKE_PERIODIC;// | PROC_FLAG_TAKEN_HARMFUL_SPELL_HIT;
            uint32 procEx       = createProcExtendMask(&damageInfo, SPELL_MISS_NONE);
            if (damageInfo.damage)
                procVictim|=PROC_FLAG_TAKEN_ANY_DAMAGE;

            pCaster->ProcDamageAndSpell(damageInfo.target, procAttacker, procVictim, procEx, damageInfo.damage, damageInfo.absorb, BASE_ATTACK, spellProto);

            pCaster->DealSpellDamage(&damageInfo, true);
            break;
        }
        case SPELL_AURA_MOD_REGEN:
        {
            // don't heal target if not alive, possible death persistent effects
            if (!target->isAlive())
                return;

            int32 gain = target->ModifyHealth(m_modifier.m_amount);
            if (Unit *caster = GetCaster())
                target->getHostileRefManager().threatAssist(caster, float(gain) * 0.5f  * sSpellMgr.GetSpellThreatMultiplier(spellProto), spellProto);
            break;
        }
        case SPELL_AURA_MOD_POWER_REGEN:
        {
            // don't energize target if not alive, possible death persistent effects
            if (!target->isAlive())
                return;

            Powers pt = target->getPowerType();
            if(int32(pt) != m_modifier.m_miscvalue)
                return;

            if ( spellProto->GetAuraInterruptFlags() & AURA_INTERRUPT_FLAG_NOT_SEATED )
            {
                // eating anim
                target->HandleEmote(EMOTE_ONESHOT_EAT);
            }
            else if( GetId() == 20577 )
            {
                // cannibalize anim
                target->HandleEmoteCommand(EMOTE_STATE_CANNIBALIZE);
            }

            if (target->isInCombat())
            {
                // Anger Management
                // amount = 1+ 16 = 17 = 3,4*5 = 10,2*5/3
                // so 17 is rounded amount for 5 sec tick grow ~ 1 range grow in 3 sec
                if (pt == POWER_RAGE)
                    target->ModifyPower(pt, m_modifier.m_amount * 3 / 5);
                // Butchery regen
                else if (pt == POWER_RUNIC_POWER)
                    target->ModifyPower(pt, m_modifier.m_amount);
            }

            break;
        }
        // Here tick dummy auras
        case SPELL_AURA_DUMMY:                              // some spells have dummy aura
        case SPELL_AURA_PERIODIC_DUMMY:
        {
            PeriodicDummyTick();
            break;
        }
        case SPELL_AURA_PERIODIC_TRIGGER_SPELL:
        {
            TriggerSpell();
            break;
        }
        case SPELL_AURA_PERIODIC_TRIGGER_SPELL_WITH_VALUE:
        {
            TriggerSpellWithValue();
            break;
        }
        default:
            break;
    }
}

void Aura::PeriodicDummyTick()
{
    SpellEntry const* spell = GetSpellProto();
    SpellClassOptionsEntry const* classOptions = spell->GetSpellClassOptions();

    Unit *target = GetTarget();
    switch (spell->GetSpellFamilyName())
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (spell->Id)
            {
                // Forsaken Skills
                case 7054:
                {
                    // Possibly need cast one of them (but
                    // 7038 Forsaken Skill: Swords
                    // 7039 Forsaken Skill: Axes
                    // 7040 Forsaken Skill: Daggers
                    // 7041 Forsaken Skill: Maces
                    // 7042 Forsaken Skill: Staves
                    // 7043 Forsaken Skill: Bows
                    // 7044 Forsaken Skill: Guns
                    // 7045 Forsaken Skill: 2H Axes
                    // 7046 Forsaken Skill: 2H Maces
                    // 7047 Forsaken Skill: 2H Swords
                    // 7048 Forsaken Skill: Defense
                    // 7049 Forsaken Skill: Fire
                    // 7050 Forsaken Skill: Frost
                    // 7051 Forsaken Skill: Holy
                    // 7053 Forsaken Skill: Shadow
                    return;
                }
                // Steal Flesh (The Culling of Stratholme - Salramm the Fleshcrafter)
                case 52708:
                {
                    if (Unit *caster = GetCaster())
                        caster->CastSpell(caster, 52712, true );

                    target->CastSpell(target, 52711, true);
                    return;
                }
                case 7057:                                  // Haunting Spirits
                    if (roll_chance_i(33))
                        target->CastSpell(target,m_modifier.m_amount,true,NULL,this);
                    return;
//              // Panda
//              case 19230: break;
//              // Gossip NPC Periodic - Talk
//              case 33208: break;
//              // Gossip NPC Periodic - Despawn
//              case 33209: break;
//              // Steal Weapon
//              case 36207: break;
//              // Simon Game START timer, (DND)
//              case 39993: break;
//              // Knockdown Fel Cannon: break; The Aggro Burst
//              case 40119: break;
//              // Old Mount Spell
//              case 40154: break;
//              // Magnetic Pull
//              case 40581: break;
//              // Ethereal Ring: break; The Bolt Burst
//              case 40801: break;
//              // Crystal Prison
//              case 40846: break;
//              // Copy Weapon
//              case 41054: break;
//              // Dementia
//              case 41404: break;
//              // Ethereal Ring Visual, Lightning Aura
//              case 41477: break;
//              // Ethereal Ring Visual, Lightning Aura (Fork)
//              case 41525: break;
//              // Ethereal Ring Visual, Lightning Jumper Aura
//              case 41567: break;
//              // No Man's Land
//              case 41955: break;
//              // Headless Horseman - Fire
//              case 42074: break;
//              // Headless Horseman - Visual - Large Fire
//              case 42075: break;
//              // Headless Horseman - Start Fire, Periodic Aura
//              case 42140: break;
//              // Ram Speed Boost
//              case 42152: break;
//              // Headless Horseman - Fires Out Victory Aura
//              case 42235: break;
//              // Pumpkin Life Cycle
//              case 42280: break;
//              // Brewfest Request Chick Chuck Mug Aura
//              case 42537: break;
//              // Squashling
//              case 42596: break;
//              // Headless Horseman Climax, Head: Periodic
//              case 42603: break;
                case 42621:                                 // Fire Bomb
                {
                    // Cast the summon spells (42622 to 42627) with increasing chance
                    uint32 rand = urand(0, 99);
                    for (uint32 i = 1; i <= 6; ++i)
                    {
                        if (rand < i * (i + 1) / 2 * 5)
                        {
                            target->CastSpell(target, spell->Id + i, true);
                            break;
                        }
                    }
                    break;
                }
//              // Headless Horseman - Conflagrate, Periodic Aura
//              case 42637: break;
//              // Headless Horseman - Create Pumpkin Treats Aura
//              case 42774: break;
//              // Headless Horseman Climax - Summoning Rhyme Aura
//              case 42879: break;
//              // Tricky Treat
//              case 42919: break;
                // Giddyup!
                case 42924:
                {
                    // Exhausted Ram
                    if (target->HasAura(43332))
                    {
                        GetHolder()->SetStackAmount(1);
                        return;
                    }

                    if (!target->HasAura(43880) && !target->HasAura(43883))
                        return;

                    if (GetStackAmount() > 1)
                    {
                        GetHolder()->ModStackAmount(-1);
                        int8 spellIndex = -1;
                        uint32 spellIds[4] = { 43310, 42992, 42993, 42994 };
                        switch(GetStackAmount())
                        {
                            case 1: spellIndex = 0; break; // Ram Level - Neutral
                            case 2: spellIndex = 1; break; // Ram - Trot
                            case 6: spellIndex = 2; break; // Ram - Canter
                        }

                        if (spellIndex != -1)
                        {
                            target->RemoveAurasDueToSpell(spellIds[spellIndex + 1]);
                            target->CastSpell(target, spellIds[spellIndex], true);
                        }
                    }
                    break;
                }
                // Ram - Trot
                case 42992:
                {
                    if (target->GetTypeId() == TYPEID_PLAYER)
                    {
                        GetModifier()->m_amount += 1;
                        if (GetModifier()->m_amount == 5)
                            ((Player*)target)->KilledMonsterCredit(24263);
                    }

                    if (SpellAuraHolder* holder = target->GetSpellAuraHolder(43052))
                        if (holder->ModStackAmount(-2))
                            target->RemoveSpellAuraHolder(holder);
                    break;
                }
                // Ram - Canter
                case 42993:
                {
                    if (target->GetTypeId() == TYPEID_PLAYER)
                    {
                        GetModifier()->m_amount += 1;
                        if (GetModifier()->m_amount == 9)
                            ((Player*)target)->KilledMonsterCredit(24264);
                    }

                    if (SpellAuraHolder* holder = target->GetSpellAuraHolder(43052))
                    {
                        if (holder->GetStackAmount() != holder->GetSpellProto()->GetStackAmount())
                            holder->ModStackAmount(1);
                        else if (!target->HasAura(43332))
                        {
                            target->RemoveAurasDueToSpell(GetId());
                            // Exhausted Ram
                            target->CastSpell(target, 43332, true);
                            // Ram Level - Neutral
                            target->CastSpell(target, 43310, true);
                        }
                    }
                    else
                        target->CastSpell(target, 43052, true);
                    break;
                }
                // Ram - Gallop
                case 42994:
                {
                    if (target->GetTypeId() == TYPEID_PLAYER)
                    {
                        GetModifier()->m_amount += 1;
                        if (GetModifier()->m_amount == 9)
                            ((Player*)target)->KilledMonsterCredit(24265);
                    }

                    if (SpellAuraHolder* holder = target->GetSpellAuraHolder(43052))
                    {
                        if (holder->GetStackAmount() != holder->GetSpellProto()->GetStackAmount())
                            holder->ModStackAmount(5);
                        else if (!target->HasAura(43332))
                        {
                            target->RemoveAurasDueToSpell(GetId());
                            // Exhausted Ram
                            target->CastSpell(target, 43332, true);
                            // Ram Level - Neutral
                            target->CastSpell(target, 43310, true);
                        }
                    }
                    else
                        target->CastSpell(target, 43052, true);
                    break;
                }
                // Ram Level - Neutral
                case 43310:
                {
                    if (SpellAuraHolder* holder = target->GetSpellAuraHolder(43052))
                        if (holder->ModStackAmount(-4))
                            target->RemoveSpellAuraHolder(holder);
                    break;
                }
//              // Headless Horseman - Maniacal Laugh, Maniacal, Delayed 17
//              case 43884: break;
//              // Wretched!
//              case 43963: break;
//              // Headless Horseman - Maniacal Laugh, Maniacal, other, Delayed 17
//              case 44000: break;
//              // Energy Feedback
//              case 44328: break;
//              // Romantic Picnic
//              case 45102: break;
//              // Romantic Picnic
//              case 45123: break;
//              // Looking for Love
//              case 45124: break;
//              // Kite - Lightning Strike Kite Aura
//              case 45197: break;
//              // Rocket Chicken
//              case 45202: break;
//              // Copy Offhand Weapon
//              case 45205: break;
//              // Upper Deck - Kite - Lightning Periodic Aura
//              case 45207: break;
//              // Kite -Sky  Lightning Strike Kite Aura
//              case 45251: break;
//              // Ribbon Pole Dancer Check Aura
//              case 45390: break;
                // Holiday - Midsummer, Ribbon Pole Periodic Visual
                case 45406:
                {
                    if (SpellAuraHolder* buff = target->GetSpellAuraHolder(29175))
                    {
                        int32 duration = buff->GetAuraMaxDuration();
                        duration += 3 * MINUTE * IN_MILLISECONDS;
                        if (duration > 60 * MINUTE * IN_MILLISECONDS)
                        {
                            duration = 60 * MINUTE * IN_MILLISECONDS;
                            if (target->GetTypeId() == TYPEID_PLAYER && target->IsInWorld())
                                ((Player*)target)->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_BE_SPELL_TARGET, 58934);
                        }
                        buff->SetAuraMaxDuration(duration);
                        buff->SetAuraDuration(duration);
                        buff->RefreshHolder();
                    }
                    else
                        target->CastSpell(target, 29175, true);

                    GameObject* pPole = NULL;
                    MaNGOS::NearestGameObjectEntryInPosRangeCheck go_check(*target, 181605, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 15.0f);
                    MaNGOS::GameObjectSearcher<MaNGOS::NearestGameObjectEntryInPosRangeCheck> checker(pPole, go_check);
                    Cell::VisitGridObjects(target, checker, 15.0f);

                    if (!pPole)
                        target->RemoveAurasDueToSpell(45406);

                    break;
                }
//              // Parachute
//              case 45472: break;
//              // Alliance Flag, Extra Damage Debuff
//              case 45898: break;
//              // Horde Flag, Extra Damage Debuff
//              case 45899: break;
//              // Ahune - Summoning Rhyme Aura
//              case 45926: break;
//              // Ahune - Slippery Floor
//              case 45945: break;
//              // Ahune's Shield
//              case 45954: break;
//              // Nether Vapor Lightning
//              case 45960: break;
//              // Darkness
//              case 45996: break;
                case 46041:                                 // Summon Blood Elves Periodic
                    target->CastSpell(target, 46037, true, NULL, this);
                    target->CastSpell(target, roll_chance_i(50) ? 46038 : 46039, true, NULL, this);
                    target->CastSpell(target, 46040, true, NULL, this);
                    return;
//              // Transform Visual Missile Periodic
//              case 46205: break;
//              // Find Opening Beam End
//              case 46333: break;
//              // Ice Spear Control Aura
//              case 46371: break;
//              // Hailstone Chill
//              case 46458: break;
//              // Hailstone Chill, Internal
//              case 46465: break;
//              // Chill, Internal Shifter
//              case 46549: break;
//              // Summon Ice Spear Knockback Delayer
//              case 46878: break;
                case 47214: // Burninate Effect
                {
                    Unit * caster = GetCaster();
                    if (!caster)
                        return;

                    if (target->GetEntry() == 26570)
                    {
                        if (target->HasAura(54683, EFFECT_INDEX_0))
                            return;
                        else
                        {
                            // Credit Scourge
                            caster->CastSpell(caster, 47208, true);
                            // set ablaze
                            target->CastSpell(target, 54683, true);
                            ((Creature*)target)->ForcedDespawn(4000);
                        }
                    }
                    break;
                }
//              // Fizzcrank Practice Parachute
//              case 47228: break;
//              // Send Mug Control Aura
//              case 47369: break;
//              // Direbrew's Disarm (precast)
//              case 47407: break;
//              // Mole Machine Port Schedule
//              case 47489: break;
//              case 47941: break; // Crystal Spike
//              case 48200: break; // Healer Aura
                case 48630:                                 // Summon Gauntlet Mobs Periodic
                case 59275:                                 // Below may need some adjustment, pattern for amount of summon and where is not verified 100% (except for odd/even tick)
                {
                    bool chance = roll_chance_i(50);

                    target->CastSpell(target, chance ? 48631 : 48632, true, NULL, this);

                    if (GetAuraTicks() % 2)                 // which doctor at odd tick
                        target->CastSpell(target, chance ? 48636 : 48635, true, NULL, this);
                    else                                    // or harponeer, at even tick
                        target->CastSpell(target, chance ? 48634 : 48633, true, NULL, this);

                    return;
                }
//              case 49313: break; // Proximity Mine Area Aura
//              // Mole Machine Portal Schedule
//              case 49466: break;
                case 49555:                                 // Corpse Explode (Drak'tharon Keep - Trollgore)
                case 59807:                                 // Corpse Explode (heroic)
                {
                    if (GetAuraTicks() == 3 && target->GetTypeId() == TYPEID_UNIT)
                        ((Creature*)target)->ForcedDespawn();
                    if (GetAuraTicks() != 2)
                        return;

                    if (Unit* pCaster = GetCaster())
                        pCaster->CastSpell(target, spell->Id == 49555 ? 49618 : 59809, true);

                    return;
                }
//              case 49592: break; // Temporal Rift
//              case 49957: break; // Cutting Laser
//              case 50085: break; // Slow Fall
//              // Listening to Music
//              case 50493: break;
//              // Love Rocket Barrage
//              case 50530: break;
                case 50789:                                 // Summon iron dwarf (left or right)
                case 59860:
                    target->CastSpell(target, roll_chance_i(50) ? 50790 : 50791, true, NULL, this);
                    return;
                case 50792:                                 // Summon iron trogg (left or right)
                case 59859:
                    target->CastSpell(target, roll_chance_i(50) ? 50793 : 50794, true, NULL, this);
                    return;
                case 50801:                                 // Summon malformed ooze (left or right)
                case 59858:
                    target->CastSpell(target, roll_chance_i(50) ? 50802 : 50803, true, NULL, this);
                    return;
                case 50824:                                 // Summon earthen dwarf
                    target->CastSpell(target, roll_chance_i(50) ? 50825 : 50826, true, NULL, this);
                    return;
                case 52441:                                 // Cool Down
                    target->CastSpell(target, 52443, true);
                    return;
                case 53035:                                 // Summon Anub'ar Champion Periodic (Azjol Nerub)
                    target->CastSpell(target, 53014, true); // Summon Anub'ar Champion
                    return;
                case 53036:                                 // Summon Anub'ar Necromancer Periodic (Azjol Nerub)
                    target->CastSpell(target, 53015, true); // Summon Anub'ar Necromancer
                    return;
                case 53037:                                 // Summon Anub'ar Crypt Fiend Periodic (Azjol Nerub)
                    target->CastSpell(target, 53016, true); // Summon Anub'ar Crypt Fiend
                    return;
                case 53520:                                 // Carrion Beetles
                    target->CastSpell(target, 53521, true, NULL, this);
                    target->CastSpell(target, 53521, true, NULL, this);
                    return;
                case 55592:                                 // Clean
                    switch(urand(0,2))
                    {
                        case 0: target->CastSpell(target, 55731, true); break;
                        case 1: target->CastSpell(target, 55738, true); break;
                        case 2: target->CastSpell(target, 55739, true); break;
                    }
                    return;
                case 54798:                                 // FLAMING Arrow Triggered Effect
                {
                    Unit * caster = GetCaster();
                    if (!caster)
                        return;

                    Player *rider = caster->GetCharmerOrOwnerPlayerOrPlayerItself();
                    if (!rider)
                        return;

                    if (target->GetEntry() == 29358)
                    {
                        if (target->HasAura(54683, EFFECT_INDEX_0))
                            return;
                        else
                        {
                            // Credit Frostworgs
                            rider->CastSpell(rider, 54896, true);
                            // set ablaze
                            target->CastSpell(target, 54683, true);
                            ((Creature*)target)->ForcedDespawn(6000);
                        }
                    }
                    else if (target->GetEntry() == 29351)
                    {
                        if (target->HasAura(54683, EFFECT_INDEX_0))
                            return;
                        else
                        {
                            // Credit Frost Giants
                            rider->CastSpell(rider, 54893, true);
                            // set ablaze
                            target->CastSpell(target, 54683, true);
                            ((Creature*)target)->ForcedDespawn(6000);
                        }
                    }

                    break;
                }
                case 62019:                                 // Rune of Summoning
                {
                    target->CastSpell(target, 62020, true, NULL, this);
                    return;
                }
                case 62038:                                 // Biting Cold (Ulduar: Hodir)
                {
                    if (target->GetTypeId() != TYPEID_PLAYER)
                        return;

                    Unit * caster = GetCaster();
                    if (!caster)
                        return;

                    if (!target->HasAura(62821))     // Toasty Fire
                    {
                        // dmg dealing every second
                        target->CastSpell(target, 62188, true, 0, 0, caster->GetObjectGuid());
                    }

                    // aura stack increase every 3 (data in m_miscvalue) seconds and decrease every 1s
                    // Reset reapply counter at move and decrease stack amount by 1
                    if (((Player*)target)->isMoving() || target->HasAura(62821))
                    {
                        if (SpellAuraHolder* holder = target->GetSpellAuraHolder(62039))
                        {
                            if (holder->ModStackAmount(-1))
                                target->RemoveSpellAuraHolder(holder);
                        }
                        m_modifier.m_miscvalue = 3;
                        return;
                    }
                    // We are standing at the moment, countdown
                    if (m_modifier.m_miscvalue > 0)
                    {
                        --m_modifier.m_miscvalue;
                        return;
                    }

                    target->CastSpell(target, 62039, true);

                    // recast every ~3 seconds
                    m_modifier.m_miscvalue = 3;
                    return;
                }
                case 62566:                                 // Healthy Spore Summon Periodic
                {
                    target->CastSpell(target, 62582, true);
                    target->CastSpell(target, 62591, true);
                    target->CastSpell(target, 62592, true);
                    target->CastSpell(target, 62593, true);
                    return;
                }
                case 62717:                                 // Slag Pot (periodic dmg)
                case 63477:
                {
                    Unit *caster = GetCaster();

                    if (caster && target)
                        caster->CastSpell(target, (spell->Id == 62717) ? 65722 : 65723, true, 0, this, this->GetCasterGuid(), this->GetSpellProto());
                    return;
                }
                case 63276:                                 // Mark of the Faceless (General Vezax - Ulduar)
                {
                    Unit *caster = GetCaster();
                    SpellEffectEntry const * effect = GetSpellProto()->GetSpellEffect(EFFECT_INDEX_0);

                    if (effect && caster && target)
                    {
                        int32 bp = effect->EffectBasePoints;
                        caster->CastCustomSpell(target, 63278, 0, &bp, 0, false, 0, 0, caster->GetObjectGuid() , spell);
                    }
                    return;
                }
                case 64217:                                 // Overcharged
                {
                    if (GetHolder()->GetStackAmount() >= 10)
                    {
                        target->CastSpell(target, 64219, true);
                        target->DealDamage(target, target->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
                    }
                    return;
                }
                case 67574:                                // Trial Of Crusader (Spike Aggro Aura - Anub'arak)
                {
                    if (!target->GetMap()->Instanceable())
                        return;

                    if (InstanceData* data = target->GetInstanceData())
                    {
                        if (Creature* pSpike = target->GetMap()->GetCreature(data->GetGuid(34660)))
                            pSpike->AddThreat(target, 1000000.0f);
                    }
                    return;
                }
                case 66118:                                 // Leeching Swarm
                case 67630:                                 // Leeching Swarm
                case 68646:                                 // Leeching Swarm
                case 68647:                                 // Leeching Swarm
                {
                    Unit* caster = GetCaster();
                    if (!caster)
                        return;

                    int32 lifeLeeched = int32(target->GetHealth() * m_modifier.m_amount * 0.01f);

                    if (lifeLeeched < 250)
                        lifeLeeched = 250;

                    // Leeching swarm damage
                    caster->CastCustomSpell(target, 66240, &lifeLeeched, NULL, NULL, true, NULL, this);

                    // Leeching swarm heal
                    target->CastCustomSpell(caster, 66125, &lifeLeeched, NULL, NULL, true, NULL, this);

                    return;
                }
                case 68870:                                 // Soulstorm
                case 69008:                                 // Soulstorm (OOC aura)                
                {
                    uint32 triggerSpells[8] = {68898, 68904, 68886, 68905, 68896, 68906, 68897, 68907};
                    target->CastSpell(target, triggerSpells[GetAuraTicks() % 8], true);
                    return;
                }
                case 68875:                                 // Wailing Souls
                case 68876:                                 // Wailing Souls
                {
                    // Sweep around
                    float newAngle = target->GetOrientation();
                    if (spell->Id == 68875)
                        newAngle += 0.09f;
                    else
                        newAngle -= 0.09f;

                    newAngle = NormalizeOrientation(newAngle);

                    target->SetFacingTo(newAngle);

                    // Should actually be SMSG_SPELL_START, too
                    target->CastSpell(target, 68873, true);
                    return;
                }
                case 71340:                                 // Pact of darkfallen remove effect
                {
                    int32 radius = m_modifier.m_amount  / 1000;
                    bool inRadius = true;
                    Map::PlayerList const& pList = target->GetMap()->GetPlayers();
                    for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
                        if (itr->getSource() && itr->getSource()->HasAura(spell->Id) && !itr->getSource()->IsWithinDistInMap(target,radius))
                        {
                            inRadius = false;
                            break;
                        }
                    if (inRadius)
                        target->RemoveAurasDueToSpell(spell->Id);
                    return;
                }
                case 76691:                                 // Vengeance
                {
                    if (target->IsInCombat())
                        return;

                    if (target->GetDamageCounterInPastSecs(2, DAMAGE_TAKEN_COUNTER))
                        return;

                    int32 bp0 = 0, bp1 = 0, bp2 = GetModifier()->m_amount;
                    SpellAuraHolder* holder = GetHolder();
                    if (!holder)
                        return;

                    // Remove damage from last 'stack' from aura amounts
                    Aura* aura0 = holder->GetAuraByEffectIndex(EFFECT_INDEX_0);
                    if (aura0)
                        bp0 = aura0->GetModifier()->m_amount - bp2;

                    Aura* aura1 = holder->GetAuraByEffectIndex(EFFECT_INDEX_1);
                    if (aura1)
                        bp1 = aura1->GetModifier()->m_amount - bp2;

                    // If exist expired aura - remove buff
                    if (bp0 <=0 || bp1 <= 0)
                        target->RemoveAurasDueToSpell(spell->Id);
                    else
                    {
                        if (aura0)
                        {
                            aura0->ApplyModifier(false, true);
                            aura0->ChangeAmount(bp0, false);
                            aura0->ApplyModifier(true, true);
                        }
                        if (aura1)
                        {
                            aura1->ApplyModifier(false, true);
                            aura1->ChangeAmount(bp1, false);
                            aura1->ApplyModifier(true, true);
                        }
                        holder->SendAuraUpdate(false);
                    }
                    break;
                }
                case 91296:                                 // Egg Shell
                case 91308:                                 // Egg Shell
                {
                    if (Unit* caster = GetCaster())
                        caster->CastSpell(caster, GetId() == 91296 ? 91306 : 91311, true);
                    return;
                }
                // Smoldering Rune
                case 98971:
                {
                    target->CastSpell(target, 99055, true);
                    return;
                }
                // Exist more after, need add later
                default:
                    break;
            }

            // Drink (item drink spells)
            if (GetEffIndex() > EFFECT_INDEX_0 && spell->GetEffectApplyAuraNameByIndex(SpellEffectIndex(GetEffIndex()-1)) == SPELL_AURA_MOD_POWER_REGEN)
            {
                if (target->GetTypeId() != TYPEID_PLAYER)
                    return;
                // Search SPELL_AURA_MOD_POWER_REGEN aura for this spell and add bonus
                if (Aura* aura = GetHolder()->GetAuraByEffectIndex(SpellEffectIndex(GetEffIndex() - 1)))
                {
                    aura->GetModifier()->m_amount = m_modifier.m_amount;
                    ((Player*)target)->UpdateManaRegen();
                    // Disable continue
                    m_isPeriodic = false;
                    return;
                }
                return;
            }

            // Prey on the Weak
            if (spell->SpellIconID == 2983)
            {
                Unit *victim = target->getVictim();
                if (victim && (target->GetHealth() * 100 / target->GetMaxHealth() > victim->GetHealth() * 100 / victim->GetMaxHealth()))
                {
                    if(!target->HasAura(58670))
                    {
                        int32 basepoints = GetBasePoints();
                        target->CastCustomSpell(target, 58670, &basepoints, 0, 0, true);
                    }
                }
                else
                    target->RemoveAurasDueToSpell(58670);
            }
        }
            break;
        case SPELLFAMILY_MAGE:
            // Mirror Image
            if (spell->Id == 55342)
            {
                if(GetTarget()->GetTypeId() != TYPEID_PLAYER)
                    break;

                std::list<Player*> nearPlayers;
                MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck u_check(GetTarget(), GetTarget(), 80);
                MaNGOS::PlayerListSearcher<MaNGOS::AnyUnfriendlyUnitInObjectRangeCheck > searcher(nearPlayers, u_check);
                Cell::VisitAllObjects(GetTarget(), searcher, 80);
                for (std::list<Player*>::const_iterator itr = nearPlayers.begin(); itr != nearPlayers.end(); ++itr)
                {
                    if ((*itr)->GetTargetGuid() == GetTarget()->GetObjectGuid())
                    {
                        WorldPacket data(SMSG_CLEAR_TARGET, 8);
                        data << GetTarget()->GetObjectGuid();
                        (*itr)->GetSession()->SendPacket(&data);
                    }
                }

                //GetTarget()->CastSpell((Unit *)NULL, GetSpellProto()->EffectTriggerSpell[m_effIndex], true); //clones are transforming into a mirrors
                m_isPeriodic = false;
            }
            break;
        case SPELLFAMILY_DRUID:
        {
            switch (spell->Id)
            {
                // Frenzied Regeneration
                case 22842:
                {
                    // Converts up to 10 rage per second into health for $d.  Each point of rage is converted into ${$m2/10}.1% of max health.
                    // Should be manauser
                    if (target->getPowerType() != POWER_RAGE)
                        return;

                    // Glyph of Frenzied Regeneration
                    if (target->HasAura(54810))
                        return;

                    int32 rage = std::min(target->GetPower(POWER_RAGE), 100);

                    // Nothing todo
                    if (rage == 0)
                        return;

                    float healthPerRage = target->CalculateSpellDamage(target, spell, EFFECT_INDEX_1) / 100.0f;
                    int32 regen = int32(target->GetMaxHealth() * rage * healthPerRage / 100 / 10);
                    target->CastCustomSpell(target, 22845, &regen, NULL, NULL, true, NULL, this);
                    // Item - Druid T13 Feral 4P Bonus (Frenzied Regeneration and Stampede)
                    if (target->HasAura(105735))
                        // Mass Regeneration
                        target->CastCustomSpell(target, 105739, &regen, NULL, NULL, true, NULL, this);
                    target->ModifyPower(POWER_RAGE, -rage);
                    return;
                }
                // Force of Nature
                case 33831:
                    return;
                // Solar Beam
                case 78675:
                {
                    if (DynamicObject* dynObj = target->GetDynObject(spell->Id))
                        target->CastSpell(dynObj->GetPositionX(), dynObj->GetPositionY(), dynObj->GetPositionZ(), 81261, true, NULL, this, GetCasterGuid());
                    return;
                }
                // Efflorescence
                case 81262:
                {
                    int32 bp = m_modifier.m_amount;
                    if (DynamicObject* dynObj = target->GetDynObject(spell->Id))
                        target->CastCustomSpell(dynObj->GetPositionX(), dynObj->GetPositionY(), dynObj->GetPositionZ(), 81269, &bp, NULL, NULL, true, NULL, this, GetCasterGuid());
                    return;
                }
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_ROGUE:
        {
            switch (spell->Id)
            {
                // Killing Spree
                case 51690:
                {
                    if (target->hasUnitState(UNIT_STAT_STUNNED) || target->isFeared())
                        return;

                    Spell::UnitList targets;
                    {
                        // eff_radius ==0
                        float radius = GetSpellMaxRange(sSpellRangeStore.LookupEntry(spell->rangeIndex));

                        MaNGOS::AnyUnfriendlyVisibleUnitInObjectRangeCheck u_check(target, target, radius);
                        MaNGOS::UnitListSearcher<MaNGOS::AnyUnfriendlyVisibleUnitInObjectRangeCheck> checker(targets, u_check);
                        Cell::VisitAllObjects(target, checker, radius);
                    }

                    if(targets.empty())
                        return;

                    Spell::UnitList::const_iterator itr = targets.begin();
                    std::advance(itr, rand()%targets.size());
                    Unit* victim = *itr;

                    target->CastSpell(victim, 57840, true);
                    target->CastSpell(victim, 57841, true);
                    return;
                }
                // Smoke Bomb
                case 76577:
                {
                    if (DynamicObject* dynObj = target->GetDynObject(spell->Id))
                        target->CastSpell(dynObj->GetPositionX(), dynObj->GetPositionY(), dynObj->GetPositionZ(), 88611, true);
                    return;
                }
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_WARLOCK:
            switch (spell->Id)
            {
                // Curse of the Elements
                case 1490:
                {
                    if (Unit* caster = GetCaster())
                        if (caster->HasAura(18179))         // Jinx (Rank 1)
                            caster->CastSpell(target, 85547, true);
                        else if (caster->HasAura(85479))    // Jinx (Rank 2)
                            caster->CastSpell(target, 86105, true);
                    return;
                }
                // Demonic Circle: Summon
                case 48018:
                {
                    GameObject* obj = GetTarget()->GetGameObject(spell->Id);
                    if (!obj)
                        return;
                    // We must take a range of teleport spell, not summon.
                    SpellEntry const* goToCircleSpell = sSpellStore.LookupEntry(48020);
                    if (target->IsWithinDist(obj, GetSpellMaxRange(sSpellRangeStore.LookupEntry(goToCircleSpell->rangeIndex))))
                        GetHolder()->SendFakeAuraUpdate(62388, false);
                    else
                        GetHolder()->SendFakeAuraUpdate(62388, true);
                    break;
                }
                // Soul Harvest
                case 79268:
                {
                    // Gain soul shard every 3rd tick
                    if (m_periodicTick % 3 == 0)
                        target->CastSpell(target, 101977, true);
                    break;
                }
            }
            break;
        case SPELLFAMILY_HUNTER:
        {
            switch (spell->Id)
            {
                // Harpooner's Mark
                // case 40084:
                //    return;
                // Feeding Frenzy Rank 1 & 2
                case 53511:
                case 53512:
                {
                    Unit* victim = target->getVictim();
                    if( victim && victim->GetHealth() * 100 < victim->GetMaxHealth() * 35 )
                        target->CastSpell(target, spell->Id == 53511 ? 60096 : 60097, true, NULL, this);
                    return;
                }
                default:
                    break;
            }
            break;
        }
        case SPELLFAMILY_SHAMAN:
        {
            // Astral Shift
            if (spell->Id == 52179)
            {
                // Periodic need for remove visual on stun/fear/silence lost
                if (!target->HasFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_STUNNED | UNIT_FLAG_FLEEING | UNIT_FLAG_SILENCED))
                    target->RemoveAurasDueToSpell(52179);
                return;
            }
            // Earthquake
            else if (spell->Id == 61882)
            {
                if (DynamicObject* dynObj = target->GetDynObject(spell->Id))
                    target->CastSpell(dynObj->GetPositionX(), dynObj->GetPositionY(), dynObj->GetPositionZ(), 77478, true, NULL, this, GetCasterGuid());
                return;
            }
            // Healing Rain
            else if (spell->Id == 73920)
            {
                if (DynamicObject* dynObj = target->GetDynObject(spell->Id))
                    target->CastSpell(dynObj->GetPositionX(), dynObj->GetPositionY(), dynObj->GetPositionZ(), 73921, true, NULL, this, GetCasterGuid());
                return;
            }
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            // Death and Decay
            if (GetId() == 43265)
            {
                int32 bp = 0;
                if (SpellAuraHolder* holder = GetHolder())
                    if (Aura* aura = holder->GetAuraByEffectIndex(EFFECT_INDEX_0))
                        bp = aura->GetModifier()->m_amount;

                // Item - Death Knight T10 Tank 4P Bonus
                if (Aura* glyph = target->GetDummyAura(70650))
                    bp *= (glyph->GetModifier()->m_amount + 100.0f)/100.0f;

                if (DynamicObject* dynObj = target->GetDynObject(spell->Id))
                    target->CastCustomSpell(dynObj->GetPositionX(), dynObj->GetPositionY(), dynObj->GetPositionZ(), 52212, &bp, NULL, NULL, true, NULL, this);
                return;
            }
            // Dark Transformation
            else if (GetId() == 93426)
            {
                Unit* pet = target->GetPet();
                if (!pet || !pet->IsInWorld() || pet->isDead())
                {
                    target->RemoveAurasDueToSpell(GetId());
                    return;
                }

                // Dark Transformation
                if (target->HasAura(63560))
                {
                    target->RemoveAurasDueToSpell(GetId());
                    return;
                }

                // Search Shadow Infusion
                SpellAuraHolder* infusion = pet->GetSpellAuraHolder(91342);
                if (!infusion || infusion->GetStackAmount() != infusion->GetSpellProto()->GetStackAmount())
                {
                    target->RemoveAurasDueToSpell(GetId());
                    return;
                }

                return;
            }
            // Death's Advance
            else if (GetId() == 96268)
            {
                if (target->GetTypeId() != TYPEID_PLAYER || target->getClass() != CLASS_DEATH_KNIGHT)
                    return;

                Player* player = (Player*)target;

                if (!player->IsBaseRuneSlotsOnCooldown(RUNE_UNHOLY))
                    target->RemoveAurasDueToSpell(GetId());
                else
                    GetHolder()->RefreshHolder();
                return;
            }
            // Raise Dead
//            if (spell->SpellFamilyFlags & UI64LIT(0x0000000000001000))
//                return;
            // Chains of Ice
            if (classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x0000400000000000))
            {
                // Get 0 effect aura
                Aura *slow = target->GetAura(GetId(), EFFECT_INDEX_0);
                if (slow)
                {
                    slow->ApplyModifier(false, true);
                    Modifier *mod = slow->GetModifier();
                    mod->m_amount+= m_modifier.m_amount;
                    if (mod->m_amount > 0) mod->m_amount = 0;
                    slow->ApplyModifier(true, true);
                }
                return;
            }
            // Summon Gargoyle
//            if (spell->SpellFamilyFlags & UI64LIT(0x0000008000000000))
//                return;
            // Reaping and Blood Rites
            if (spell->SpellIconID == 22 || spell->SpellIconID == 2724)
            {
                if (target->GetTypeId() != TYPEID_PLAYER)
                    return;
                if (((Player*)target)->getClass() != CLASS_DEATH_KNIGHT)
                    return;

                // timer expired - remove death runes
                ((Player*)target)->RemoveRunesByAuraEffect(this);
            }
            break;
        }
        case SPELLFAMILY_PRIEST:
        {
            // Holy Word: Sanctuary
            if (spell->Id == 88685)
            {
                    if (DynamicObject* dynObj = target->GetDynObject(spell->Id))
                        target->CastSpell(dynObj->GetPositionX(), dynObj->GetPositionY(), dynObj->GetPositionZ(), 88686, true, NULL, this, GetCasterGuid());
                return;
            }
            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            // Consecration
            if (spell->Id == 26573)
            {
                if (Unit* caster = GetCaster())
                    if (DynamicObject* dynObj = caster->GetDynObject(spell->Id))
                        target->CastSpell(dynObj->GetPositionX(), dynObj->GetPositionY(), dynObj->GetPositionZ(), 81297, true, NULL, this, GetCasterGuid());
                return;
            }
            // Holy Radiance
            else if (spell->Id == 82327)
            {
                if (Unit* caster = GetCaster())
                    caster->CastSpell(target, 86452, true);
            }
            break;
        }
        default:
            break;
    }
}

void Aura::HandlePreventFleeing(bool apply, bool Real)
{
    if(!Real)
        return;

    Unit::AuraList const& fearAuras = GetTarget()->GetAurasByType(SPELL_AURA_MOD_FEAR);
    if( !fearAuras.empty() )
    {
        if (apply)
            GetTarget()->SetFeared(false, fearAuras.front()->GetCasterGuid());
        else
            GetTarget()->SetFeared(true);
    }
}

void Aura::HandleManaShield(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();

    // prevent double apply bonuses
    if (apply && (target->GetTypeId() != TYPEID_PLAYER || !((Player*)target)->GetSession()->PlayerLoading()))
    {
        if (Unit* caster = GetCaster())
        {
            float DoneActualBenefit = 0.0f;
            switch(GetSpellProto()->GetSpellFamilyName())
            {
                case SPELLFAMILY_MAGE:
                    break;
                default:
                    break;
            }

            DoneActualBenefit *= caster->CalculateLevelPenalty(GetSpellProto());

            m_modifier.m_amount += (int32)DoneActualBenefit;
        }
    }
    else if (!apply)
    {
        // Mana Shield
        if (GetId() == 1463)
        {
            // only from fully depleted shield
            if (m_modifier.m_amount > 0)
                return;

            Unit* caster = GetCaster();
            if (!caster || caster->GetTypeId() != TYPEID_PLAYER)
                return;

            // Incanters Absorbtion
            if (((Player*)caster)->GetKnownTalentById(9188))
                target->CastSpell(target, 86261, true);

            return;
        }
    }
}

void Aura::HandleArenaPreparation(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();

    target->ApplyModFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PREPARATION, apply);

    if (apply)
    {
        // max regen powers at start preparation
        target->SetHealth(target->GetMaxHealth());
        target->SetPower(POWER_MANA, target->GetMaxPower(POWER_MANA));
        target->SetPower(POWER_ENERGY, target->GetMaxPower(POWER_ENERGY));
    }
    else
    {
        // reset originally 0 powers at start/leave
        target->SetPower(POWER_RAGE, 0);
        target->SetPower(POWER_RUNIC_POWER, 0);
    }
}

/**
 * Such auras are applied from a caster(=player) to a vehicle.
 * This has been verified using spell #49256
 */
void Aura::HandleAuraControlVehicle(bool apply, bool Real)
{
    if(!Real)
        return;

    Unit *caster = GetCaster();
    if (!caster)
        return;

    Unit* target = GetTarget();
    if (!target)
        return;

    VehicleKit* pVehicle = target->GetVehicleKit();

    if (target->GetTypeId() != TYPEID_UNIT || !pVehicle)
        return;

    if (apply)
    {
        //if (caster->GetTypeId() == TYPEID_PLAYER)
        //    ((Player*)caster)->RemovePet(PET_SAVE_AS_CURRENT);

        //int8 seat = pVehicle->HasEmptySeat(m_modifier.m_amount) ? m_modifier.m_amount : -1;
        //caster->EnterVehicle(pVehicle, seat);
        caster->EnterVehicle(pVehicle);
    }
    else
    {
        // some SPELL_AURA_CONTROL_VEHICLE auras have a dummy effect on the player - remove them
        caster->RemoveAurasDueToSpell(GetId());

        if (caster->GetVehicle() == pVehicle)
            caster->ExitVehicle();

        //if (caster->GetTypeId() == TYPEID_PLAYER)
        //    ((Player*)caster)->ResummonPetTemporaryUnSummonedIfAny();
    }
}

void Aura::HandleAuraAddMechanicAbilities(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();

    if (!target || target->GetTypeId() != TYPEID_PLAYER)    // only players should be affected by this aura
        return;

    uint16 i_OverrideSetId = GetMiscValue();

    const OverrideSpellDataEntry *spellSet = sOverrideSpellDataStore.LookupEntry(i_OverrideSetId);
    if (!spellSet)
        return;

    if (apply)
    {

        // spell give the player a new castbar with some spells.. this is a clientside process..
        // serverside just needs to register the new spells so that player isn't kicked as cheater
        for (int i = 0; i < MAX_OVERRIDE_SPELLS; ++i)
            if (uint32 spellId = spellSet->Spells[i])
                static_cast<Player*>(target)->addSpell(spellId, true, false, false, false);

        target->SetUInt16Value(PLAYER_FIELD_BYTES2, 0, i_OverrideSetId);
    }
    else
    {
        target->SetUInt16Value(PLAYER_FIELD_BYTES2, 0, 0);
        for (int i = 0; i < MAX_OVERRIDE_SPELLS; ++i)
            if (uint32 spellId = spellSet->Spells[i])
                static_cast<Player*>(target)->removeSpell(spellId, false , false, false);
    }
}

void Aura::HandleAuraOpenStable(bool apply, bool Real)
{
    if(!Real || GetTarget()->GetTypeId() != TYPEID_PLAYER || !GetTarget()->IsInWorld())
        return;

    Player* player = (Player*)GetTarget();

    if (apply)
        player->GetSession()->SendStablePet(player->GetObjectGuid());

    // client auto close stable dialog at !apply aura
}

void Aura::HandleAuraMirrorImage(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();

    if (apply)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        // Set display id
        target->SetDisplayId(caster->GetDisplayId());
        target->SetFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_CLONED);
    }
    else
    {
        target->SetDisplayId(target->GetNativeDisplayId());
        target->RemoveFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_CLONED);
    }
}

void Aura::HandleAuraConvertRune(bool apply, bool Real)
{
    if (!Real)
        return;

    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* plr = (Player*)GetTarget();

    if (plr->getClass() != CLASS_DEATH_KNIGHT)
        return;

    uint32 runes = GetModifier()->m_amount;
    // convert number of runes specified in aura amount of rune type in miscvalue to runetype in miscvalueb
    if (apply)
    {
        for (uint32 i = 0; i < MAX_RUNES && runes; ++i)
        {
            if (GetMiscValue() != plr->GetCurrentRune(i))
                continue;

            if (!plr->GetRuneCooldown(i))
            {
                plr->AddRuneByAuraEffect(i, RuneType(GetMiscBValue()), this);
                --runes;
            }
        }
    }
    else
        plr->RemoveRunesByAuraEffect(this);
}

void Aura::HandlePhase(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();

    if (target->GetTypeId() == TYPEID_PLAYER)
    {
        if (apply)
            ((Player*)target)->GetPhaseMgr()->RegisterPhasingAuraEffect(this);
        else
            ((Player*)target)->GetPhaseMgr()->UnRegisterPhasingAuraEffect(this);
    }
    else
    {
        uint32 phaseMask = 0;
        if (apply)
        {
            phaseMask = target->GetPhaseMask();
            if (target->GetAurasByType(SPELL_AURA_PHASE).size() == 1 && target->GetAurasByType(SPELL_AURA_PHASE_2).size() == 1)
                phaseMask &= ~PHASEMASK_NORMAL;

            phaseMask |= GetMiscValue();
        }
        else
        {
            Unit::AuraList const& phases = target->GetAurasByType(SPELL_AURA_PHASE);
            for (Unit::AuraList::const_iterator itr = phases.begin(); itr != phases.end(); ++itr)
                phaseMask |= (*itr)->GetMiscValue();

            Unit::AuraList const& phases2 = target->GetAurasByType(SPELL_AURA_PHASE_2);
            for (Unit::AuraList::const_iterator itr = phases2.begin(); itr != phases2.end(); ++itr)
                phaseMask |= (*itr)->GetMiscValue();
        }

        if (!phaseMask)
            phaseMask = PHASEMASK_NORMAL;

        target->SetPhaseMask(phaseMask, true);
    }
    // no-phase is also phase state so same code for apply and remove
    if (GetEffIndex() == EFFECT_INDEX_0 && target->GetTypeId() == TYPEID_PLAYER)
    {
        SpellAreaForAreaMapBounds saBounds = sSpellMgr.GetSpellAreaForAuraMapBounds(GetId());
        if(saBounds.first != saBounds.second)
        {
            uint32 zone, area;
            target->GetZoneAndAreaId(zone, area);

            for(SpellAreaForAreaMap::const_iterator itr = saBounds.first; itr != saBounds.second; ++itr)
            {
                // some auras remove at aura remove
                if(!itr->second->IsFitToRequirements((Player*)target, zone, area))
                    target->RemoveAurasDueToSpell(itr->second->spellId);
                // some auras applied at aura apply
                else if(itr->second->autocast)
                {
                    if (!target->HasAura(itr->second->spellId, EFFECT_INDEX_0))
                        target->CastSpell(target, itr->second->spellId, true);
                }
            }
        }
    }
}

void Aura::HandleAuraSafeFall( bool Apply, bool Real )
{
    // implemented in WorldSession::HandleMovementOpcodes

    // only special case
    if(Apply && Real && GetId() == 32474 && GetTarget()->GetTypeId() == TYPEID_PLAYER)
        ((Player*)GetTarget())->ActivateTaxiPathTo(506, GetId());
}

bool Aura::IsCritFromAbilityAura(Unit* caster, uint32& damage)
{
    if (caster->IsSpellCrit(GetTarget(), GetSpellProto(), GetSpellSchoolMask(GetSpellProto())))
    {
        damage = caster->SpellCriticalDamageBonus(GetSpellProto(), damage, GetTarget());
        return true;
    }

    return false;
}

void Aura::HandleAuraModAllCritChance(bool apply, bool Real)
{
    // spells required only Real aura add/remove
    if(!Real)
        return;

    Unit *target = GetTarget();

    if(target->GetTypeId() != TYPEID_PLAYER)
        return;

    ((Player*)target)->HandleBaseModValue(CRIT_PERCENTAGE,         FLAT_MOD, float (m_modifier.m_amount), apply);
    ((Player*)target)->HandleBaseModValue(OFFHAND_CRIT_PERCENTAGE, FLAT_MOD, float (m_modifier.m_amount), apply);
    ((Player*)target)->HandleBaseModValue(RANGED_CRIT_PERCENTAGE,  FLAT_MOD, float (m_modifier.m_amount), apply);

    // included in Player::UpdateSpellCritChance calculation
    ((Player*)target)->UpdateAllSpellCritChances();
}

void Aura::HandleAuraStopNaturalManaRegen(bool apply, bool Real)
{
    if (!Real)
        return;

    GetTarget()->ApplyModFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_REGENERATE_POWER, !apply);
}

void Aura::HandleAuraMastery(bool apply, bool Real)
{
    Unit* target = GetTarget();
    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    ((Player*)target)->UpdateMasteryAuras();
}

void Aura::HandleAuraModBlockCritChance(bool apply, bool Real)
{
    if (GetTarget()->GetTypeId() == TYPEID_PLAYER)
        ((Player*)GetTarget())->ApplyModUInt32Value(PLAYER_SHIELD_BLOCK_CRIT_PERCENTAGE, m_modifier.m_amount, apply);
}

bool Aura::IsLastAuraOnHolder()
{
    for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (i != GetEffIndex() && GetHolder()->m_auras[i])
            return false;
    return true;
}

bool Aura::HasMechanic(uint32 mechanic) const
{
    if (GetSpellProto()->GetMechanic() == mechanic)
        return true;

    return m_spellEffect->EffectMechanic == mechanic;
}

SpellAuraHolder::SpellAuraHolder(SpellEntry const* spellproto, Unit* target, WorldObject* caster, Item* castItem) :
    m_spellProto(spellproto),
    m_target(target), m_castItemGuid(castItem ? castItem->GetObjectGuid() : ObjectGuid()),
    m_auraSlot(MAX_AURAS), m_auraFlags(AFLAG_NONE), m_auraLevel(1),
    m_procCharges(0), m_stackAmount(1),
    m_timeCla(1000), m_removeMode(AURA_REMOVE_BY_DEFAULT), m_AuraDRGroup(DIMINISHING_NONE),
    m_permanent(false), m_isRemovedOnShapeLost(true), m_deleted(false), m_in_use(0)
{
    MANGOS_ASSERT(target);
    MANGOS_ASSERT(spellproto && spellproto == sSpellStore.LookupEntry( spellproto->Id ) && "`info` must be pointer to sSpellStore element");

    if(!caster)
        m_casterGuid = target->GetObjectGuid();
    else
    {
        // remove this assert when not unit casters will be supported
        MANGOS_ASSERT(caster->isType(TYPEMASK_UNIT))
        m_casterGuid = caster->GetObjectGuid();
    }

    m_applyTime      = time(NULL);
    m_isPassive      = IsPassiveSpell(spellproto);
    m_isDeathPersist = IsDeathPersistentSpell(spellproto);
    m_isSingleTarget = IsSingleTargetSpell(spellproto);
    m_procCharges = m_spellProto->GetProcCharges();

    m_isRemovedOnShapeLost = (GetCasterGuid() == m_target->GetObjectGuid() &&
                              m_spellProto->GetStances() &&
                              !m_spellProto->HasAttribute(SPELL_ATTR_EX2_NOT_NEED_SHAPESHIFT) &&
                              !m_spellProto->HasAttribute(SPELL_ATTR_NOT_SHAPESHIFT));

    Unit* unitCaster = caster && caster->isType(TYPEMASK_UNIT) ? (Unit*)caster : NULL;

    m_maxDurationWithoutHaste =  CalculateSpellDuration(spellproto, unitCaster, false);
    m_duration = m_maxDuration = ApplyHasteToDuration(spellproto, unitCaster, m_maxDurationWithoutHaste);

    if (m_maxDuration == -1 || (m_isPassive && spellproto->DurationIndex == 0))
        m_permanent = true;

    if (unitCaster)
    {
        if (Player* modOwner = unitCaster->GetSpellModOwner())
            modOwner->ApplySpellMod(GetId(), SPELLMOD_CHARGES, m_procCharges);
    }

    // some custom stack values at aura holder create
    switch (m_spellProto->Id)
    {
        // some auras applied with max stack
        case 24575:                                         // Brittle Armor
        case 24659:                                         // Unstable Power
        case 24662:                                         // Restless Strength
        case 26464:                                         // Mercurial Shield
        case 53257:                                         // Cobra strike
        case 55166:                                         // Tidal Force
        case 62519:                                         // Attuned to Nature
        case 64455:                                         // Feral Essence
        case 66228:                                         // Nether Power (ToC: Lord Jaraxxus)
        case 67106:                                         // Nether Power (ToC: Lord Jaraxxus)
        case 67107:                                         // Nether Power (ToC: Lord Jaraxxus)
        case 67108:                                         // Nether Power (ToC: Lord Jaraxxus)
        //case 70672:                                         // Gaseous Bloat (Putricide)
        case 71564:                                         // Deadly Precision
        //case 74396:                                         // Fingers of Frost
            m_stackAmount = m_spellProto->GetStackAmount();
            break;
    }

    for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        m_auras[i] = NULL;
}

void SpellAuraHolder::AddAura(Aura *aura, SpellEffectIndex index)
{
    m_auras[index] = aura;
    m_auraFlags |= (1 << index);
}

void SpellAuraHolder::RemoveAura(SpellEffectIndex index)
{
    m_auras[index] = NULL;
    m_auraFlags &= ~(1 << index);
}

class AuraSorter
{
    public:
        bool operator() (Aura* const& left, Aura* const& right) const
        {
            if ((right->GetModifier()->m_auraname == SPELL_AURA_MOD_ROOT || right->GetModifier()->m_auraname == SPELL_AURA_MOD_STUN) &&
                (left->GetModifier()->m_auraname != SPELL_AURA_MOD_ROOT && left->GetModifier()->m_auraname != SPELL_AURA_MOD_STUN))
                return false;

            return true;
        }
};

void SpellAuraHolder::ApplyAuraModifiers(bool apply, bool real)
{
    std::vector<Aura*> auras;
    for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (Aura *aur = GetAuraByEffectIndex(SpellEffectIndex(i)))
            auras.push_back(aur);

    AuraSorter sorter;
    std::sort(auras.begin(), auras.end(), sorter);

    for (uint32 i = 0; i < auras.size() && !IsDeleted(); ++i)
        auras[i]->ApplyModifier(apply, real);
}

void SpellAuraHolder::_AddSpellAuraHolder()
{
    if (!GetId())
        return;
    if(!m_target)
        return;

    // Try find slot for aura
    uint8 slot = NULL_AURA_SLOT;

    // Lookup free slot
    if (m_target->GetVisibleAurasCount() < MAX_AURAS)
    {
        Unit::VisibleAuraMap const& visibleAuras = m_target->GetVisibleAuras();
        for(uint8 i = 0; i < MAX_AURAS; ++i)
        {
            Unit::VisibleAuraMap::const_iterator itr = visibleAuras.find(i);
            if (itr == visibleAuras.end())
            {
                slot = i;
                // update for out of range group members (on 1 slot use)
                m_target->UpdateAuraForGroup(slot);
                break;
            }
        }
    }

    Unit* caster = GetCaster();

    // set infinity cooldown state for spells
    if(caster && caster->GetTypeId() == TYPEID_PLAYER)
    {
        if (m_spellProto->HasAttribute(SPELL_ATTR_DISABLED_WHILE_ACTIVE))
        {
            Item* castItem = m_castItemGuid ? ((Player*)caster)->GetItemByGuid(m_castItemGuid) : NULL;
            ((Player*)caster)->AddSpellAndCategoryCooldowns(m_spellProto,castItem ? castItem->GetEntry() : 0, NULL,true);
        }
    }

    uint8 flags = 0;
    for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        if (m_auras[i] && IsAuraApplyEffect(m_spellProto, SpellEffectIndex(i)))
        {
            flags |= (1 << i);
            if (m_auras[i]->GetModifier()->m_amount)
                flags |= AFLAG_EFFECT_AMOUNT_SEND;

            //switch (m_auras[i]->GetModifier()->m_auraname)
            //{
            //    case SPELL_AURA_SCHOOL_ABSORB:
            //    case SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS:
            //    case SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS_2:
            //        flags |= AFLAG_EFFECT_AMOUNT_SEND;
            //        break;
            //    default:
            //        break;
            //}
        }
    }

    if (GetCasterGuid() == GetTarget()->GetObjectGuid())
        flags |= AFLAG_NOT_CASTER;

    if (!m_spellProto->HasAttribute(SPELL_ATTR_EX5_HIDE_DURATION) && GetSpellMaxDuration(m_spellProto) > 0)
        flags |= AFLAG_DURATION;

    if (IsPositive())
        flags |= AFLAG_POSITIVE;
    else
        flags |= AFLAG_NEGATIVE;

    //if (m_spellProto->HasAttribute(SPELL_ATTR_EX8_AURA_SENDS_AMOUNT) &&
    //    (flags & (AFLAG_EFF_INDEX_0 | AFLAG_EFF_INDEX_1 | AFLAG_EFF_INDEX_2)))
    //    flags |= AFLAG_EFFECT_AMOUNT_SEND;

    SetAuraFlags(flags);

    SetAuraLevel(caster ? caster->getLevel() : sWorld.getConfig(CONFIG_UINT32_MAX_PLAYER_LEVEL));

    if (IsNeedVisibleSlot(caster))
    {
        SetAuraSlot( slot );
        if(slot < MAX_AURAS)                        // slot found send data to client
        {
            SetVisibleAura(false);
            SendAuraUpdate(false);
        }

        //*****************************************************
        // Update target aura state flag on holder apply
        // TODO: Make it easer
        //*****************************************************

        // Sitdown on apply aura req seated
        if (m_spellProto->GetAuraInterruptFlags() & AURA_INTERRUPT_FLAG_NOT_SEATED && !m_target->IsSitState())
            m_target->SetStandState(UNIT_STAND_STATE_SIT);

        // register aura diminishing on apply
        if (getDiminishGroup() != DIMINISHING_NONE )
            m_target->ApplyDiminishingAura(getDiminishGroup(), true);

        // Update Seals information
        if (IsSealSpell(m_spellProto))
            m_target->ModifyAuraState(AURA_STATE_JUDGEMENT, true);

        // Conflagrate aura state on Immolate
        if (m_spellProto->Id == 348)
            m_target->ModifyAuraState(AURA_STATE_CONFLAGRATE, true);

        // Faerie Fire (druid versions)
        if (m_spellProto->IsFitToFamily(SPELLFAMILY_DRUID, UI64LIT(0x0000000000000400)))
            m_target->ModifyAuraState(AURA_STATE_FAERIE_FIRE, true);

        // Sting (hunter versions)
        if(m_spellProto->IsFitToFamily(SPELLFAMILY_HUNTER, UI64LIT(0x1000000000000000)))
            m_target->ModifyAuraState(AURA_STATE_FAERIE_FIRE, true); 

        // Victorious
        if (m_spellProto->IsFitToFamily(SPELLFAMILY_WARRIOR, UI64LIT(0x0004000000000000)))
            m_target->ModifyAuraState(AURA_STATE_WARRIOR_VICTORY_RUSH, true);

        // Swiftmend state on Regrowth & Rejuvenation
        if (m_spellProto->IsFitToFamily(SPELLFAMILY_DRUID, UI64LIT(0x0000000000000050)))
            m_target->ModifyAuraState(AURA_STATE_SWIFTMEND, true);

        // Deadly poison aura state
        if (m_spellProto->IsFitToFamily(SPELLFAMILY_ROGUE, UI64LIT(0x0000000000010000)))
            m_target->ModifyAuraState(AURA_STATE_DEADLY_POISON, true);

        // Enrage aura state (Excluding Vengeance)
        if (m_spellProto->GetDispel() == DISPEL_ENRAGE && m_spellProto->Id != 76691)
            m_target->ModifyAuraState(AURA_STATE_ENRAGE, true);

        // Bleeding aura state
        if (GetAllSpellMechanicMask(m_spellProto) & (1 << (MECHANIC_BLEED-1)))
            m_target->ModifyAuraState(AURA_STATE_BLEEDING, true);

        // Blood Presence and Rune Strike enabler
        if (GetId() == 48263 || GetId() == 56817)
            m_target->ModifyAuraState(AURA_STATE_DEFENSE, true);
    }
}

void SpellAuraHolder::_RemoveSpellAuraHolder()
{
    // Remove all triggered by aura spells vs unlimited duration
    // except same aura replace case
    if(m_removeMode!=AURA_REMOVE_BY_STACK)
        CleanupTriggeredSpells();

    Unit* caster = GetCaster();

    if (caster && IsPersistent())
        if (DynamicObject *dynObj = caster->GetDynObject(GetId()))
            dynObj->RemoveAffected(m_target);

    // remove at-store spell cast items (for all remove modes?)
    if (m_target->GetTypeId() == TYPEID_PLAYER && m_removeMode != AURA_REMOVE_BY_DEFAULT && m_removeMode != AURA_REMOVE_BY_DELETE)
        if (ObjectGuid castItemGuid = GetCastItemGuid())
            if (Item* castItem = ((Player*)m_target)->GetItemByGuid(castItemGuid))
                ((Player*)m_target)->DestroyItemWithOnStoreSpell(castItem, GetId());

    //passive auras do not get put in slots - said who? ;)
    // Note: but totem can be not accessible for aura target in time remove (to far for find in grid)
    //if(m_isPassive && !(caster && caster->GetTypeId() == TYPEID_UNIT && ((Creature*)caster)->IsTotem()))
    //    return;

    uint8 slot = GetAuraSlot();

    if(slot >= MAX_AURAS)                                   // slot not set
        return;

    if (!m_target->GetVisibleAura(slot))
        return;

    // unregister aura diminishing (and store last time)
    if (getDiminishGroup() != DIMINISHING_NONE )
        m_target->ApplyDiminishingAura(getDiminishGroup(), false);

    SetAuraFlags(AFLAG_NONE);
    SetAuraLevel(0);
    SetVisibleAura(true);

    if (m_removeMode != AURA_REMOVE_BY_DELETE)
    {
        SendAuraUpdate(true);

        // update for out of range group members
        m_target->UpdateAuraForGroup(slot);

        //*****************************************************
        // Update target aura state flag (at last aura remove)
        //*****************************************************
        // Enrage aura state (Excluding Vengeance)
        if (m_spellProto->GetDispel() == DISPEL_ENRAGE && m_spellProto->Id != 76691)
            m_target->ModifyAuraState(AURA_STATE_ENRAGE, false);

        // Bleeding aura state
        if (GetAllSpellMechanicMask(m_spellProto) & (1 << (MECHANIC_BLEED-1)))
        {
            bool found = false;

            Unit::SpellAuraHolderMap const& holders = m_target->GetSpellAuraHolderMap();
            for (Unit::SpellAuraHolderMap::const_iterator itr = holders.begin(); itr != holders.end(); ++itr)
            {
                if (GetAllSpellMechanicMask(itr->second->GetSpellProto()) & (1 << (MECHANIC_BLEED-1)))
                {
                    found = true;
                    break;
                }
            }

            if (!found)
                m_target->ModifyAuraState(AURA_STATE_BLEEDING, false);
        }

        uint32 removeState = 0;
        SpellClassOptionsEntry const* classOptions = m_spellProto->GetSpellClassOptions();
        ClassFamilyMask removeFamilyFlag = classOptions ? classOptions->SpellFamilyFlags : ClassFamilyMask();
        switch(m_spellProto->GetSpellFamilyName())
        {
            case SPELLFAMILY_PALADIN:
                if (IsSealSpell(m_spellProto))
                    removeState = AURA_STATE_JUDGEMENT;     // Update Seals information
                break;
            case SPELLFAMILY_WARLOCK:
                // Conflagrate aura state on Immolate
                if (m_spellProto->Id == 348)
                {
                    removeFamilyFlag = ClassFamilyMask(UI64LIT(0x0000000000000004));
                    removeState = AURA_STATE_CONFLAGRATE;
                }
                break;
            case SPELLFAMILY_DRUID:
                if (m_spellProto->IsFitToFamilyMask(UI64LIT(0x0000000000000400)))
                    removeState = AURA_STATE_FAERIE_FIRE;   // Faerie Fire (druid versions)
                else if (m_spellProto->IsFitToFamilyMask(UI64LIT(0x0000000000000050)))
                {
                    removeFamilyFlag = ClassFamilyMask(UI64LIT(0x00000000000050));
                    removeState = AURA_STATE_SWIFTMEND;     // Swiftmend aura state
                }
                break;
            case SPELLFAMILY_WARRIOR:
                if (m_spellProto->IsFitToFamilyMask(UI64LIT(0x0004000000000000)))
                    removeState = AURA_STATE_WARRIOR_VICTORY_RUSH; // Victorious
                break;
            case SPELLFAMILY_ROGUE:
                if (m_spellProto->IsFitToFamilyMask(UI64LIT(0x0000000000010000)))
                    removeState = AURA_STATE_DEADLY_POISON; // Deadly poison aura state
                break;
            case SPELLFAMILY_HUNTER:
                if (m_spellProto->IsFitToFamilyMask(UI64LIT(0x1000000000000000)))
                    removeState = AURA_STATE_FAERIE_FIRE;   // Sting (hunter versions)
                break;
            case SPELLFAMILY_DEATHKNIGHT:
                if (m_spellProto->Id == 48263)              // Blood Presence
                {
                    if (!m_target->GetSpellAuraHolder(56817, m_target->GetObjectGuid()))    // Rune Strike enabler
                        removeState = AURA_STATE_DEFENSE;
                    break;
                }
                else if (m_spellProto->Id == 56817)         // Rune Strike enabler
                {
                    if (!m_target->GetSpellAuraHolder(48263, m_target->GetObjectGuid()))    // Blood Presence
                        removeState = AURA_STATE_DEFENSE;
                    break;
                }
                break;
            default:
                break;
        }

        // Remove state (but need check other auras for it)
        if (removeState)
        {
            bool found = false;
            Unit::SpellAuraHolderMap const& holders = m_target->GetSpellAuraHolderMap();
            for (Unit::SpellAuraHolderMap::const_iterator i = holders.begin(); i != holders.end(); ++i)
            {
                SpellEntry const *auraSpellInfo = (*i).second->GetSpellProto();
                if (auraSpellInfo->IsFitToFamily(SpellFamily(m_spellProto->GetSpellFamilyName()), removeFamilyFlag))
                {
                    found = true;
                    break;
                }
            }

            // this has been last aura
            if (!found)
                m_target->ModifyAuraState(AuraState(removeState), false);
        }

        // reset cooldown state for spells
        if (caster && caster->GetTypeId() == TYPEID_PLAYER)
        {
            if (GetSpellProto()->HasAttribute(SPELL_ATTR_DISABLED_WHILE_ACTIVE))
                // note: item based cooldowns and cooldown spell mods with charges ignored (unknown existing cases)
                ((Player*)caster)->SendCooldownEvent(GetSpellProto());
        }
    }
}

void SpellAuraHolder::CleanupTriggeredSpells()
{
    for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        SpellEffectEntry const* spellEffect = m_spellProto->GetSpellEffect(SpellEffectIndex(i));
        if(!spellEffect)
            continue;

        if (!spellEffect->EffectApplyAuraName)
            continue;

        uint32 tSpellId = spellEffect->EffectTriggerSpell;
        if(!tSpellId)
            continue;

        SpellEntry const* tProto = sSpellStore.LookupEntry(tSpellId);
        if(!tProto)
            continue;

        if(GetSpellDuration(tProto) != -1)
            continue;

        // needed for spell 43680, maybe others
        // TODO: is there a spell flag, which can solve this in a more sophisticated way?
        if (spellEffect->EffectApplyAuraName == SPELL_AURA_PERIODIC_TRIGGER_SPELL &&
            GetSpellDuration(m_spellProto) == int32(spellEffect->EffectAmplitude))
            continue;

        m_target->RemoveAurasDueToSpell(tSpellId);
    }
}

bool SpellAuraHolder::ModStackAmount(int32 num)
{
    uint32 protoStackAmount = m_spellProto->GetStackAmount();

    // Can`t mod
    if (!protoStackAmount)
        return true;

    // Modify stack but limit it
    int32 stackAmount = m_stackAmount + num;
    if (stackAmount > (int32)protoStackAmount)
        stackAmount = protoStackAmount;
    else if (stackAmount <=0) // Last aura from stack removed
    {
        m_stackAmount = 0;
        return true; // need remove aura
    }

    // reset charges when modding stack (there are spells using both)
    m_procCharges = m_spellProto->GetProcCharges();
    Player* modOwner = GetCaster() ? GetCaster()->GetSpellModOwner() : NULL;
    if(modOwner)
        modOwner->ApplySpellMod(GetId(), SPELLMOD_CHARGES, m_procCharges);

    // Update stack amount
    SetStackAmount(stackAmount);
    return false;
}

void SpellAuraHolder::SetStackAmount(uint32 stackAmount)
{
    Unit *target = GetTarget();
    Unit *caster = GetCaster();
    if (!target || !caster)
        return;

    bool refresh = stackAmount >= m_stackAmount;
    if (stackAmount != m_stackAmount)
    {
        m_stackAmount = stackAmount;

        for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        {
            if (Aura *aur = m_auras[i])
            {
                int32 bp = aur->GetBasePoints();
                int32 amount = m_stackAmount * caster->CalculateSpellDamage(target, m_spellProto, SpellEffectIndex(i), &bp);
                // Reapply if amount change
                if (amount != aur->GetModifier()->m_amount)
                {
                    aur->ApplyModifier(false, true);
                    aur->GetModifier()->m_amount = amount;
                    aur->ApplyModifier(true, true);

                    // change duration if aura refreshes
                    if (refresh)
                    {
                        int32 maxduration = GetSpellMaxDuration(aur->GetSpellProto());
                        int32 duration = GetSpellDuration(aur->GetSpellProto());

                        // new duration based on combo points
                        if (duration != maxduration)
                        {
                            if (Unit *caster = aur->GetCaster())
                            {
                                duration += int32((maxduration - duration) * caster->GetComboPoints() / 5);
                                SetAuraMaxDuration(duration);
                                SetAuraDuration(duration);
                                refresh = false;
                            }
                        }
                    }
                }
            }
        }
    }

    if (refresh)
        // Stack increased refresh duration
        RefreshHolder();
    else
        // Stack decreased only send update
        SendAuraUpdate(false);
}

Unit* SpellAuraHolder::GetCaster() const
{
    if(GetCasterGuid() == m_target->GetObjectGuid())
        return m_target;

    return ObjectAccessor::GetUnit(*m_target, m_casterGuid);// player will search at any maps
}

bool SpellAuraHolder::IsWeaponBuffCoexistableWith(SpellAuraHolder const* ref) const
{
    // only item casted spells
    if (!GetCastItemGuid())
        return false;

    // Exclude Debuffs
    if (!IsPositive())
        return false;

    // Exclude Non-generic Buffs [ie: Runeforging] and Executioner-Enchant
    if (GetSpellProto()->GetSpellFamilyName() != SPELLFAMILY_GENERIC || GetId() == 42976)
        return false;

    // Exclude Stackable Buffs [ie: Blood Reserve]
    if (GetSpellProto()->GetStackAmount())
        return false;

    // only self applied player buffs
    if (m_target->GetTypeId() != TYPEID_PLAYER || m_target->GetObjectGuid() != GetCasterGuid())
        return false;

    Item* castItem = ((Player*)m_target)->GetItemByGuid(GetCastItemGuid());
    if (!castItem)
        return false;

    // Limit to Weapon-Slots
    if (!castItem->IsEquipped() ||
        (castItem->GetSlot() != EQUIPMENT_SLOT_MAINHAND && castItem->GetSlot() != EQUIPMENT_SLOT_OFFHAND))
        return false;

    // form different weapons
    return ref->GetCastItemGuid() && ref->GetCastItemGuid() != GetCastItemGuid();
}

bool SpellAuraHolder::IsNeedVisibleSlot(Unit const* caster) const
{
    bool totemAura = caster && caster->GetTypeId() == TYPEID_UNIT && ((Creature*)caster)->IsTotem();

    if (m_spellProto->GetProcFlags())
        return true;
    else if (HasAuraWithTriggerEffect(m_spellProto))
        return true;
    else if (IsSpellHaveAura(m_spellProto, SPELL_AURA_IGNORE_UNIT_STATE))
        return true;
    else if (IsSpellHaveAura(m_spellProto, SPELL_AURA_MOD_IGNORE_SHAPESHIFT))
        return true;
    else if (IsSpellHaveAura(m_spellProto, SPELL_AURA_ALLOW_CAST_WHILE_MOVING))
        return true;
    else if (IsSpellHaveAura(m_spellProto, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS))
        return true;
    else if (IsSpellHaveAura(m_spellProto, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS_2))
        return true;
    else if (IsSpellHaveAura(m_spellProto, SPELL_AURA_MOD_CD_FROM_HASTE))
        return true;
    else if (IsSpellHaveAura(m_spellProto, SPELL_AURA_WORGEN_TRANSFORM))
        return true;

    // passive auras (except totem auras) do not get placed in the slots
    return !m_isPassive || totemAura || HasAreaAuraEffect(m_spellProto);
}

void SpellAuraHolder::BuildUpdatePacket(WorldPacket& data) const
{
    data << uint8(GetAuraSlot());
    data << uint32(GetId());

    uint8 auraFlags = GetAuraFlags();
    // recheck effect amount
    if ((auraFlags & AFLAG_EFFECT_AMOUNT_SEND) == 0)
    {
        for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
            if (Aura const* aura = m_auras[i])
                if ((auraFlags & (1 << i)) && aura->GetModifier()->m_amount)
                {
                    auraFlags |= AFLAG_EFFECT_AMOUNT_SEND;
                    break;
                }
    }

    data << uint16(auraFlags);
    data << uint8(GetAuraLevel());

    //data << uint8(GetStackAmount() > 1 ? GetStackAmount() : (GetAuraCharges()) ? GetAuraCharges() : 1);
    data << uint8(GetSpellProto()->GetStackAmount() ? GetStackAmount() : (GetAuraCharges() ? GetAuraCharges() : 1));

    if(!(auraFlags & AFLAG_NOT_CASTER))
    {
        data << GetCasterGuid().WriteAsPacked();
    }

    if(auraFlags & AFLAG_DURATION)
    {
        data << uint32(GetAuraMaxDuration());
        data << uint32(GetAuraDuration());
    }

    if (auraFlags & AFLAG_EFFECT_AMOUNT_SEND)
    {
        for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
            if (auraFlags & (1 << i))
                if (Aura const* aura = m_auras[i])
                    data << int32(aura->GetModifier()->m_amount);
                else
                    data << int32(0);
    }
}

void SpellAuraHolder::SendAuraUpdate(bool remove) const
{
    WorldPacket data(SMSG_AURA_UPDATE);
    data << m_target->GetPackGUID();

    if(remove)
    {
        data << uint8(GetAuraSlot());
        data << uint32(0);
    }
    else
        BuildUpdatePacket(data);

    m_target->SendMessageToSet(&data, true);
}

void SpellAuraHolder::HandleSpellSpecificBoosts(bool apply)
{
    DEBUG_LOG("SpellAuraHolder::HandleSpellSpecificBoosts for %u apply: %u", GetId(), apply);

    bool cast_at_remove = false;                            // if spell must be casted at last aura from stack remove
    uint32 spellId1 = 0;
    uint32 spellId2 = 0;
    uint32 spellId3 = 0;
    uint32 spellId4 = 0;

    SpellClassOptionsEntry const* classOptions = m_spellProto->GetSpellClassOptions();

    switch(m_spellProto->GetSpellFamilyName())
    {
        case SPELLFAMILY_GENERIC:
        {
            switch(GetId())
            {
                case 3714:                                  // Path of Frost
                {
                    if (!apply)
                        spellId1 = 93061;
                    else if (Unit* caster = GetCaster())
                        // Glyph of Path of Frost
                        if (caster == m_target && caster->HasAura(59307))
                            spellId1 = 93061;
                    break;
                }
                case 7376:                                  // Defensive Stance Passive
                {
                    spellId1 = 57339;
                    break;
                }
                case 29865:                                 // Deathbloom (10 man)
                {
                    if (!apply && m_removeMode == AURA_REMOVE_BY_EXPIRE)
                    {
                        cast_at_remove = true;
                        spellId1 = 55594;
                    }
                    else
                        return;
                    break;
                }
                case 39870:                                 // Speed Burst
                {
                    if (m_target->GetTypeId() != TYPEID_PLAYER || !((Player*)m_target)->IsSpectator())
                        return;

                    if (apply)
                    {
                        m_target->setFaction(35);
                        m_target->SetVisibility(VISIBILITY_OFF);
                        m_target->addUnitState(UNIT_STAT_DIED);
                        m_target->addUnitState(UNIT_STAT_ISOLATED);
                    }
                    else
                    {
                        ((Player*)m_target)->setFactionForRace(m_target->getRace());

                        if (m_target->HasAuraType(SPELL_AURA_MOD_STEALTH))
                            m_target->SetVisibility(VISIBILITY_GROUP_STEALTH);
                        else if (m_target->HasAuraType(SPELL_AURA_MOD_INVISIBILITY))
                            m_target->SetVisibility(VISIBILITY_GROUP_INVISIBILITY);
                        else
                            m_target->SetVisibility(VISIBILITY_ON);

                        m_target->clearUnitState(UNIT_STAT_DIED);
                        m_target->clearUnitState(UNIT_STAT_ISOLATED);
                    }
                    return;
                }
                case 43880:                                 // Ramstein's Swift Work Ram
                case 43883:                                 // Rental Racing Ram
                {
                    if (!apply)
                    {
                        m_target->RemoveAurasDueToSpell(42146); // Brewfest Racing Ram Aura [DND]
                        m_target->RemoveAurasDueToSpell(43492); // Brewfest - apple trap - Unfriendly DND

                        m_target->RemoveAurasDueToSpell(43310); // Ram Level - Neutral
                        m_target->RemoveAurasDueToSpell(42992); // Ram - Trot
                        m_target->RemoveAurasDueToSpell(42993); // Ram - Canter
                        m_target->RemoveAurasDueToSpell(42994); // Ram - Gallop

                        m_target->RemoveAurasDueToSpell(42924); // Giddyup!
                        m_target->RemoveAurasDueToSpell(43052); // Ram Fatigue
                        m_target->RemoveAurasDueToSpell(43332); // Exhausted Ram
                    }
                    break;
                }
                case 55001:                                 // Parachute
                {
                    if (apply)
                    {
                        m_target->RemoveSpellsCausingAura(SPELL_AURA_MOUNTED);
                        m_target->RemoveSpellsCausingAura(SPELL_AURA_FLY);
                        m_target->RemoveSpellsCausingAura(SPELL_AURA_MOD_INCREASE_SPEED);
                    }
                    break;
                }
                case 55053:                                 // Deathbloom (25 man)
                {
                    if (!apply && m_removeMode == AURA_REMOVE_BY_EXPIRE)
                    {
                        cast_at_remove = true;
                        spellId1 = 55601;
                    }
                    else
                        return;
                    break;
                }
                case 57350:                                 // Illusionary Barrier
                {
                    if (!apply && m_target->getPowerType() == POWER_MANA)
                    {
                        cast_at_remove = true;
                        spellId1 = 60242;                   // Darkmoon Card: Illusion
                    }
                    else
                        return;
                    break;
                }
                case 61716:                                 // Rabbit Costume
                case 61734:                                 // Noblegarden Bunny
                {
                    spellId1 = 61719;                       // Easter Lay Noblegarden Egg Aura

                    if (apply)
                        m_target->CastSpell(m_target, spellId1, true);
                    else
                        m_target->RemoveAurasDueToSpell(spellId1);
                    return;
                }
                case 61804:                                 // A Serving of Cranberries
                case 61805:                                 // A Serving of Pie
                case 61806:                                 // A Serving of Stuffing
                case 61807:                                 // A Serving of Turkey
                case 61808:                                 // A Serving of Sweet Potatoes
                {
                    uint32 spellId = GetSpellProto()->CalculateSimpleValue(EFFECT_INDEX_0);
                    if (apply)
                        m_target->CastSpell(m_target, spellId, true);
                    else
                        m_target->RemoveAurasDueToSpell(spellId);
                    return;
                }
                case 62274:                                 // Shield of Runes (normal) (Runemaster Molgeim, Assembly of Iron encounter in Ulduar)
                {
                    if (!apply && m_removeMode == AURA_REMOVE_BY_SHIELD_BREAK)
                    {
                        cast_at_remove = true;
                        spellId1 = 62277;
                    }
                    else
                        return;
                    break;
                }
                case 62619:                                 // Potent Pheromones (Freya encounter)
                case 64321:                                 // Potent Pheromones (Freya encounter) heroic
                {
                    if (apply)
                        if (Unit* target = GetTarget())
                            target->RemoveAurasDueToSpell(62532);
                    return;
                }
                case 62692:                                 // Aura of Despair
                {
                    spellId1 = 64848;
                    break;
                }
                case 63120:                                 // Insane
                {
                    spellId1 = 64464;
                    break;
                }
                case 63277:                                 // Shadow Crash (General Vezax - Ulduar)
                {
                    spellId1 = 65269;
                    break;
                }
                case 63489:                                 // Shield of Runes (heroic) (Runemaster Molgeim, Assembly of Iron encounter in Ulduar)
                {
                    if (!apply && m_removeMode == AURA_REMOVE_BY_SHIELD_BREAK)
                    {
                        cast_at_remove = true;
                        spellId1 = 63967;
                    }
                    else
                        return;
                    break;
                }
                case 63830:                                 // Malady of the Mind
                case 63881:
                {
                    if (!apply)
                    {
                        spellId1 = 63881;
                        cast_at_remove = true;
                    }
                    break;
                }
                case 69290:                                 // Blighted Spores
                case 73034:
                case 73033:
                case 71222:
                {
                    if (!apply)
                    {
                        if (m_removeMode == AURA_REMOVE_BY_EXPIRE)
                        {
                            cast_at_remove = true;
                            spellId1 = 69291;
                            // Cast unknown spell - spore explode (override)
                            if (SpellEffectEntry const * effect = GetSpellProto()->GetSpellEffect(EFFECT_INDEX_0))
                            {
                                float radius = GetSpellRadius(sSpellRadiusStore.LookupEntry(effect->GetRadiusIndex()));
                                Map::PlayerList const& pList = m_target->GetMap()->GetPlayers();
                                for (Map::PlayerList::const_iterator itr = pList.begin(); itr != pList.end(); ++itr)
                                    if (itr->getSource() && itr->getSource()->IsWithinDistInMap(m_target,radius))
                                        itr->getSource()->CastSpell(itr->getSource(), spellId1, true);
                            }
                        }
                    }
                    break;
                }
                case 69674:                                 // SPELL_STICKY_OOZE
                {
                    if (!apply)
                    {
                        if (m_removeMode == AURA_REMOVE_BY_DISPEL)
                        {
                            cast_at_remove = true;
                            spellId1 = 69706;
                        }
                    }
                    break;
                }
                case 70867:                                 // Essence of the Blood Queen
                case 71473:
                case 71532:
                case 71533:
                {
                    spellId1 = 70871;
                    break;
                }
                case 71905:                                 // Soul Fragment
                {
                    if (!apply)
                    {
                        m_target->RemoveAurasDueToSpell(72521);     // Shadowmourne Visual Low
                        m_target->RemoveAurasDueToSpell(72523);     // Shadowmourne Visual High
                        return;
                    }
                    else
                        return;
                    break;
                }
                case 74002:                                 // Combat Readiness
                {
                    if (!apply && m_removeMode == AURA_REMOVE_BY_EXPIRE)
                        m_target->RemoveAurasDueToSpell(74001);
                    return;
                }
                case 85768:                                 // Dark Intent
                {
                    if (!apply)
                    {
                        // remove haster buff from target on remove from caster
                        if (Unit* target = m_target->GetSingleCastSpellTarget(85767))
                            target->RemoveAurasByCasterSpell(85767, m_target->GetObjectGuid());
                    }
                    return;
                }
                case 97128:                                 // Molten Feather
                {
                    if (!apply)
                    {
                        GetTarget()->SetPower(POWER_ALTERNATIVE, 0);

                        GetTarget()->RemoveAurasDueToSpell(98761);
                        GetTarget()->RemoveAurasDueToSpell(98762);
                        GetTarget()->RemoveAurasDueToSpell(98763);
                        GetTarget()->RemoveAurasDueToSpell(98764);
                        GetTarget()->RemoveAurasDueToSpell(98765);
                        GetTarget()->RemoveAurasDueToSpell(98766);
                        GetTarget()->RemoveAurasDueToSpell(98767);
                        GetTarget()->RemoveAurasDueToSpell(98768);
                        GetTarget()->RemoveAurasDueToSpell(98769);
                        GetTarget()->RemoveAurasDueToSpell(98770);
                        GetTarget()->RemoveAurasDueToSpell(98771);
                    }
                    return;
                }
                case 101223:                                // Fieroblast
                case 101294:
                case 101295:
                case 101296:
                {
                    if (apply)
                    {
                        if (Unit* caster =GetCaster())
                            caster->CastSpell(caster, 100093, true);
                    }
                    return;
                }
                case 105785:                                // Stolen Time
                {
                    if (!apply)
                        spellId1 = 105791;
                    break;
                }
                default:
                    return;
            }

            // Improved Blood Presence
            if (GetId() == 50365 || GetId() == 50371)
            {
                // if presence active: Frost Presence or Unholy Presence
                if (apply && (m_target->HasAura(48266) || m_target->HasAura(48265)))
                {
                    Aura* aura = GetAuraByEffectIndex(EFFECT_INDEX_0);
                    if (!aura)
                        return;

                    int32 bp = -aura->GetModifier()->m_amount;
                    m_target->CastCustomSpell(m_target, 61261, &bp, NULL, NULL, true, NULL, NULL, GetCasterGuid());
                }
                else
                    m_target->RemoveAurasDueToSpell(61261);
                return;
            }
            break;
        }
        case SPELLFAMILY_MAGE:
        {
            // Pyromaniac check
            if (Unit* caster = GetCaster())
            {
                if (caster->GetTypeId() == TYPEID_PLAYER)
                {
                    Player* player = (Player*)caster;
                    SpellEntry const* tal = player->GetKnownTalentRankById(10559);
                    if (IsSpellHaveAura(GetSpellProto(), SPELL_AURA_PERIODIC_DAMAGE) ||
                        IsSpellHaveAura(GetSpellProto(), SPELL_AURA_PERIODIC_DAMAGE_PERCENT) ||
                        IsSpellHaveAura(GetSpellProto(), SPELL_AURA_PERIODIC_LEECH))
                    {
                        if (!apply)
                        {
                            if (player->m_pyromaniacCounter > 0)
                                --player->m_pyromaniacCounter;

                            if (player->m_pyromaniacCounter < 3 || !tal)
                                caster->RemoveAurasDueToSpell(83582);
                        }
                        else
                        {
                            ++player->m_pyromaniacCounter;
                            if (player->m_pyromaniacCounter >= 3 && tal && !caster->HasAura(83582))
                            {
                                int32 bp = tal->CalculateSimpleValue(EFFECT_INDEX_0);
                                caster->CastCustomSpell(caster, 83582, &bp, NULL, NULL, true);
                            }
                        }
                    }
                }
            }

            // Cone of Cold
            if (m_spellProto->Id == 120 && apply)
            {
                if (Unit* caster = GetCaster())
                {
                    // Search Improved Cone of Cold
                    Unit::AuraList const& mDummyAuras = caster->GetAurasByType(SPELL_AURA_DUMMY);
                    for (Unit::AuraList::const_iterator itr = mDummyAuras.begin(); itr != mDummyAuras.end(); ++itr)
                    {
                        if ((*itr)->GetSpellProto()->SpellIconID == 35 && (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_MAGE)
                        {
                            // Rank 1
                            if ((*itr)->GetId() == 11190)
                                m_target->CastSpell(m_target, 83301, true, NULL, NULL, caster->GetObjectGuid());
                            // Rank 2
                            else if ((*itr)->GetId() == 12489)
                                m_target->CastSpell(m_target, 83302, true, NULL, NULL, caster->GetObjectGuid());
                            break;
                        }
                    }
                }
            }
            // Arcane Power or Icy Veins
            else if (m_spellProto->Id == 12042 || m_spellProto->Id == 12472)
            {
                if (!apply)
                    spellId1 = 105785;      // Stolen Time
            }
            // Arcane Missiles!
            else if (m_spellProto->Id == 79683)
            {
                // Arcane Missiles Aurastate
                spellId1 = 79808;
                break;
            }
            // Time Warp
            else if (m_spellProto->Id == 80353)
            {
                if (apply)
                {
                    Unit* caster = GetCaster();
                    if (!caster)
                        return;

                    caster->CastSpell(m_target, 80354, true);
                    return;
                }
            }
            // Combustion
            else if (m_spellProto->Id == 83853)
            {
                if (!apply)
                    if (Unit* caster = GetCaster())
                        caster->RemoveAurasDueToSpell(105785);
                return;
            }
            // Ice Barrier (non stacking from one caster)
            else if (m_spellProto->SpellIconID == 32)
            {
                if (!apply && (m_removeMode == AURA_REMOVE_BY_DISPEL || m_removeMode == AURA_REMOVE_BY_SHIELD_BREAK))
                {
                    Unit::AuraList const& dummyAuras = m_target->GetAurasByType(SPELL_AURA_DUMMY);
                    for(Unit::AuraList::const_iterator itr = dummyAuras.begin(); itr != dummyAuras.end(); ++itr)
                    {
                        // Shattered Barrier
                        if ((*itr)->GetSpellProto()->SpellIconID == 3260 && (*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_MAGE)
                        {
                            cast_at_remove = true;
                            // Rank 1
                            if ((*itr)->GetId() == 44745)
                                spellId1 = 55080;
                            // Rank 2
                            else if ((*itr)->GetId() == 54787)
                                spellId1 = 83073;
                            break;
                        }
                    }
                }
                else
                    return;
                break;
            }

            switch(GetId())
            {
                case 32612:                                 // Invisibility
                {
                    // Glyph of Invisibility
                    if (!apply || m_target->HasAura(56366))
                        spellId1 = 87833;

                    Pet* pet = m_target->GetPet();
                    if (!pet)
                        break;

                    if (apply)
                        pet->CastSpell(pet, 32612, true);
                    else
                        pet->RemoveAurasDueToSpell(32612);

                    break;
                }
                case 44614:                                 // Frostfire Bolt (Rank 1)
                case 47610:                                 // Frostfire Bolt (Rank 2)
                {
                    // Permafrost
                    spellId1 = 68391;
                    break;
                }
                case 48108:                                 // Hot Streak (triggered)
                case 57761:                                 // Fireball! (Brain Freeze triggered)
                {
                    // consumed aura
                    if (!apply && m_removeMode == AURA_REMOVE_BY_DEFAULT)
                    {
                        Unit* caster = GetCaster();
                        // Item - Mage T10 2P Bonus
                        if (!caster || !caster->HasAura(70752))
                            return;

                        cast_at_remove = true;
                        spellId1 = 70753;                   // Pushing the Limit
                    }
                    else
                        return;
                    break;
                }
                default:
                    return;
            }
            break;
        }
        case SPELLFAMILY_WARRIOR:
        {
            // Shield Wall
            if (GetId() == 871)
            {
                // Item - Warrior T13 Protection 4P Bonus (Shield Wall)
                // Shield Fortress
                if (apply && m_target->HasAura(105911))
                    spellId1 = 105914;
                break;
            }
            // Inner Rage
            else if (GetId() == 1134)
            {
                // Item - Warrior T13 Arms and Fury 2P Bonus (Inner Rage)
                // Volatile Outrage
                if (!apply)
                    spellId1 = 105860;
                break;
            }
            // Shield Block (Defensive Stance)
            else if (GetId() == 2565)
            {
                // Item - Warrior T12 Protection 4P Bonus
                if (!apply && m_target->HasAura(99242))
                {
                    spellId1 = 99243;
                    cast_at_remove = true;
                }
                break;
            }

            if(!apply)
            {
                // Remove Blood Frenzy only if target no longer has any Deep Wound or Rend (applying is handled by procs)
                if (GetSpellProto()->GetMechanic() != MECHANIC_BLEED)
                    return;

                // If target still has one of Warrior's bleeds, do nothing
                Unit::AuraList const& PeriodicDamage = m_target->GetAurasByType(SPELL_AURA_PERIODIC_DAMAGE);
                for(Unit::AuraList::const_iterator i = PeriodicDamage.begin(); i != PeriodicDamage.end(); ++i)
                    if( (*i)->GetCasterGuid() == GetCasterGuid() &&
                        (*i)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_WARRIOR &&
                        (*i)->GetSpellProto()->GetMechanic() == MECHANIC_BLEED)
                        return;

                spellId1 = 30069;                           // Blood Frenzy (Rank 1)
                spellId2 = 30070;                           // Blood Frenzy (Rank 2)
            }
            else
            {
                // Shield Block
                if (GetId() == 2565)
                {
                    if (m_target->GetTypeId() == TYPEID_PLAYER)
                    {
                        float blockValue = m_target->GetFloatValue(PLAYER_BLOCK_PERCENTAGE);
                        if (blockValue > 100.0f)
                            if (Aura* aura = GetAuraByEffectIndex(EFFECT_INDEX_2))
                                aura->ChangeAmount(int32(blockValue - 100.0f));
                    }
                }
            }
            break;
        }
        case SPELLFAMILY_WARLOCK:
        {
            // Fear (non stacking)
            if (classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x0000040000000000))
            {
                if(!apply)
                {
                    Unit* caster = GetCaster();
                    if(!caster)
                        return;

                    Unit::AuraList const& dummyAuras = caster->GetAurasByType(SPELL_AURA_DUMMY);
                    for(Unit::AuraList::const_iterator itr = dummyAuras.begin(); itr != dummyAuras.end(); ++itr)
                    {
                        SpellEntry const* dummyEntry = (*itr)->GetSpellProto();
                        // Improved Fear
                        if (dummyEntry->GetSpellFamilyName() == SPELLFAMILY_WARLOCK && dummyEntry->SpellIconID == 98)
                        {
                            cast_at_remove = true;
                            switch((*itr)->GetModifier()->m_amount)
                            {
                                // Rank 1
                                case 0: spellId1 = 60946; break;
                                // Rank 2
                                case 1: spellId1 = 60947; break;
                            }
                            break;
                        }
                    }
                }
                else
                    return;
            }
            // Shadowflame (DoT)
            else if (m_spellProto->IsFitToFamilyMask(UI64LIT(0x0000000000000000), 0x00000002))
            {
                // Glyph of Shadowflame
                Unit* caster = GetCaster();
                if (!apply)
                    spellId1 = 63311;
                else if (caster && caster->HasAura(63310))
                    spellId1 = 63311;
                else
                    return;
            }
            // Health Funnel
            else if (m_spellProto->IsFitToFamilyMask(0x1000000))
            {
                if (apply)
                {
                    if (Unit* caster = GetCaster())
                    {
                        if (m_target == caster)
                            return;

                        Unit::AuraList const& pctAuras = caster->GetAurasByType(SPELL_AURA_ADD_PCT_MODIFIER);
                        for (Unit::AuraList::const_iterator itr = pctAuras.begin(); itr != pctAuras.end(); ++itr)
                        {
                            if ((*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_WARLOCK && (*itr)->GetSpellProto()->SpellIconID == 153 && (*itr)->GetEffIndex() == EFFECT_INDEX_0)
                            {
                                switch((*itr)->GetId())
                                {
                                    case 18703: spellId1 = 60955; break;
                                    case 18704: spellId1 = 60956; break;
                                }
                                break;
                            }
                        }
                    }
                    else
                        return;
                }
                else
                {
                    spellId1 = 60955;
                    spellId2 = 60956;
                }
            }
            // Curse of Weakness
            else if (m_spellProto->Id == 702)
            {
                if (!apply)
                {
                    spellId1 = 85539;
                    spellId2 = 85540;
                    spellId3 = 85541;
                    spellId4 = 85542;
                }
                else
                    return;
            }
            // Seed of Corruption
            else if (m_spellProto->Id == 27243)
            {
                if (m_removeMode != AURA_REMOVE_BY_DEFAULT)
                    if (Unit* caster = GetCaster())
                        caster->soulburnMarker = false;
            }
            // Soulburn
            else if (m_spellProto->Id == 74434)
            {
                // Soulburn: Seed of Corruption, rank 1
                if (!apply || m_target->HasSpell(86664))
                    spellId1 = 93313;   // Soulburn Dummy
                break;
            }
            // Bane of Havoc
            else if (m_spellProto->Id == 80240)
            {
                if (Unit* caster = GetCaster())
                {
                    uint32 spellId = 85466;     // pve spell
                    if (m_target->GetCharmerOrOwnerPlayerOrPlayerItself())
                        spellId = 85468;        // pvp spell

                    if (apply)
                        caster->CastSpell(caster, spellId, true);
                    else
                        caster->RemoveAurasByCasterSpell(spellId, caster->GetObjectGuid());
                }
                return;
            }
            // Dark Intent
            else if (m_spellProto->Id == 85767)
            {
                if (!apply)
                {
                    // remove buff from caster on remove from target
                    if (Unit* caster = GetCaster())
                        caster->RemoveAurasByCasterSpell(85768, caster->GetObjectGuid());
                }
                return;
            }
            else
                return;
            break;
        }
        case SPELLFAMILY_PRIEST:
        {
            // Shadow Word: Pain (need visual check fro skip improvement talent) or Vampiric Touch
            if (m_spellProto->SpellIconID == 234 && m_spellProto->SpellVisual[0] || m_spellProto->SpellIconID == 2213)
            {
                if (!apply && m_removeMode == AURA_REMOVE_BY_DISPEL)
                {
                    Unit* caster = GetCaster();
                    if(!caster)
                        return;

                    Unit::AuraList const& dummyAuras = caster->GetAurasByType(SPELL_AURA_DUMMY);
                    for(Unit::AuraList::const_iterator itr = dummyAuras.begin(); itr != dummyAuras.end(); ++itr)
                    {
                        // Shadow Affinity
                        if ((*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_PRIEST
                            && (*itr)->GetSpellProto()->SpellIconID == 178)
                        {
                            // custom cast code
                            int32 basepoints0 = (*itr)->GetModifier()->m_amount * caster->GetCreateMana() / 100;
                            caster->CastCustomSpell(caster, 64103, &basepoints0, NULL, NULL, true, NULL);
                            return;
                        }
                    }
                }
                else
                    return;
            }
            // Power Word: Shield
            else if (apply && classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x0000000000000001) && m_spellProto->GetMechanic() == MECHANIC_SHIELD)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                // Weakened Soul
                caster->CastSpell(m_target, 6788, true);

                // Glyph of Power Word: Shield
                if (Aura* glyph = caster->GetAura(55672, EFFECT_INDEX_0))
                {
                    Aura *shield = GetAuraByEffectIndex(EFFECT_INDEX_0);
                    int32 heal = (glyph->GetModifier()->m_amount * shield->GetModifier()->m_amount) / 100;
                    caster->CastCustomSpell(m_target, 56160, &heal, NULL, NULL, true, 0, shield);
                }

                // Holy Walk
                if (caster == m_target && caster->HasAura(33333))
                    caster->CastSpell(caster, 96219, true);     // Holy Walk
                return;
            }

            // Shadow Word: Pain, Devouring Plague, Vampiric Touch
            if (GetId() == 589 || GetId() == 2944 || GetId() == 34914)
            {
                if (Unit* caster = GetCaster())
                {
                    // Vampiric Touch
                    if (GetId() == 34914)
                        ((Player*)caster)->vampiricTouchTargetGuid = m_target->GetObjectGuid();

                    // Item - Priest T12 Shadow 4P Bonus
                    if (caster->HasAura(99157))
                    {
                        Unit* target;
                        if (m_target->GetObjectGuid() == ((Player*)caster)->vampiricTouchTargetGuid)
                            target = m_target;
                        else
                            target = caster->GetMap()->GetAnyTypeCreature(((Player*)caster)->vampiricTouchTargetGuid);

                        uint32 count = 0;
                        if (target)
                        {
                            Unit::SpellAuraHolderMap const& holders = target->GetSpellAuraHolderMap();
                            for (Unit::SpellAuraHolderMap::const_iterator itr = holders.begin(); itr != holders.end(); ++itr)
                                if ((itr->first == 589 || itr->first == 2944 || itr->first == 34914) && itr->second->GetCasterGuid() == caster->GetObjectGuid())
                                    ++count;
                        }

                        // Dark Flames
                        // Item - Priest T12 Shadow 4P Bonus
                        if (count < 3)
                            caster->RemoveAurasDueToSpell(99158);
                        else
                            caster->CastSpell(caster, 99158, true);
                    }
                    else
                        caster->RemoveAurasDueToSpell(99158);
                }
            }

            switch (GetId())
            {
                case 15473:                                 // Shadowform
                {
                    if (!apply)
                    {
                        spellId1 = 107903;
                        spellId2 = 107904;
                    }
                    else
                        // Glyph of Shadow alters Shadowform visual
                        spellId1 = GetTarget()->HasAura(107906) ? 107904 : 107903;
                    break;
                }
                // Dispersion mana reg and immunity
                case 47585:
                    spellId1 = 60069;                       // Dispersion
                    spellId2 = 63230;                       // Dispersion
                    break;
                // Chakra: Sanctuary (Chakra: Sanctuary)
                case 81206:
                    spellId1 = 81207;
                    break;
                // Item - Priest T12 Shadow 4P Bonus
                case 99157:
                    if (!apply)
                        spellId1 = 99158;                   // Dark Flames
                    break;
                default:
                {
                    // Item - Priest T11 Healer 4P Bonus
                    if (GetSpellSpecific(GetId()) == SPELL_PRIEST_CHAKRA && GetId() != 14751)
                        if (!apply || m_target->HasAura(89911))
                        {
                            spellId1 = 89912;
                            break;
                        }
                    return;
                }
            }
            break;
        }
        case SPELLFAMILY_DRUID:
        {
            // Rejuvenation
            if (GetId() == 774)
            {
                // Nature's Bounty check
                if (Unit* caster = GetCaster())
                {
                    if (caster->GetTypeId() == TYPEID_PLAYER)
                    {
                        Player* player = (Player*)caster;
                        SpellEntry const * tal = player->GetKnownTalentRankById(8255);
                        if (!apply)
                        {
                            if (player->m_naturesBountyCounter > 0)
                                --player->m_naturesBountyCounter;

                            if (player->m_naturesBountyCounter < 3 || !tal)
                                caster->RemoveAurasDueToSpell(96206);
                        }
                        else
                        {
                            ++player->m_naturesBountyCounter;
                            if (player->m_naturesBountyCounter >= 3 && tal && !caster->HasAura(96206))
                            {
                                int32 bp = -tal->CalculateSimpleValue(EFFECT_INDEX_1);
                                caster->CastCustomSpell(caster, 96206, &bp, NULL, NULL, true);
                            }
                        }
                    }
                }
            }
            // Tiger's Fury or Berserk
            else if (GetId() == 5217 || GetId() == 50334)
            {
                if (apply)
                {
                    // search Primal Madness
                    if (m_target->GetTypeId() == TYPEID_PLAYER)
                        if (SpellEntry const* spellInfo = ((Player*)m_target)->GetKnownTalentRankById(8335))
                            spellId1 = spellInfo->Id == 80316 ? 80879 : 80886;
                }
                else
                {
                    spellId1 = 80879;
                    spellId2 = 80886;
                }

                // Berserk
                if (GetId() == 50334)
                    spellId3 = 58923;                       // Berserk (Mangle (Bear) modifier)
                break;
            }
            // Bear Form (Passive2)
            else if (GetId() == 21178)
                spellId1 = 57339;
            // Barkskin
            else if (GetId()==22812)                        // Glyph of Barkskin
            {
                if (!apply || m_target->HasAura(63057))
                    spellId1 = 63058;                       // Glyph of Amberskin Protection
                if (!apply && m_target->HasAura(99009))     // Item - Druid T12 Feral 4P Bonus
                    m_target->CastSpell(m_target, 99011, true);// Smokescreen
            }
            // Item - Druid T10 Feral 4P Bonus
            else if (GetId() == 5229)                       // Enrage (Druid Bear)
            {
                if (m_target->HasAura(70726))
                    spellId1 = 70725;
                if (!apply)
                    spellId2 = 51185;                       // King of the Jungle (Enrage damage aura)
            }
            // Eclipse (Solar)
            else if (GetId() == 48517)
                spellId1 = 94338;
            // Stampede, Tiger's Fury
            else if (GetId() == 81021 || GetId() == 81022 || GetId() == 108566)
                spellId1 = 109881;                          // Stampede Ravage Marker
            // Harmony
            else if (GetId() == 100977)
            {
                // Item - Druid T11 Restoration 4P Bonus
                if (!apply || m_target->HasAura(90158))
                    spellId1 = 90159;
            }
            else
                return;
            break;
        }
        case SPELLFAMILY_ROGUE:
            // remove debuf savage combat
            if (GetSpellProto()->IsFitToFamilyMask(UI64LIT(0x0008000010014000)))
            {
                // search poison
                bool found = false;
                if (m_target->HasAuraState(AURA_STATE_DEADLY_POISON))
                    found = true;
                else
                {
                    Unit::SpellAuraHolderMap const& auras = m_target->GetSpellAuraHolderMap();
                    for (Unit::SpellAuraHolderMap::const_iterator itr = auras.begin(); itr != auras.end(); ++itr)
                    {
                        if (itr->second->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_ROGUE &&
                            itr->second->GetSpellProto()->GetDispel() == DISPEL_POISON)
                        {
                            found = true;
                            break;
                        }
                    }
                }

                if (!found)
                {
                    m_target->RemoveAurasDueToSpell(58684); // Savage Combat rank 1
                    m_target->RemoveAurasDueToSpell(58683); // Savage Combat rank 2
                }
            }
            // Sprint (skip non player casted spells by category)
            else if (classOptions && classOptions->SpellFamilyFlags & UI64LIT(0x0000000000000040) && GetSpellProto()->GetCategory() == 44)
            {
                if(!apply || m_target->HasAura(58039))      // Glyph of Blurred Speed
                    spellId1 = 61922;                       // Sprint (waterwalk)
                else
                   return;
            }
            // Sap
            else if (GetId() == 6770)
            {
                if (!apply)
                {
                    if (Unit* caster = GetCaster())
                    {
                        if (caster->GetTypeId() == TYPEID_PLAYER)
                        {
                            // Search Blackjack talent
                            if (SpellEntry const* blackJack = ((Player*)caster)->GetKnownTalentRankById(6515))
                            {
                                cast_at_remove = true;
                                if (blackJack->Id == 79123)         // Rank 1
                                    m_target->CastSpell(m_target, 79124, true, NULL, NULL, caster->GetObjectGuid());
                                else if (blackJack->Id == 79125)    // Rank 2
                                    m_target->CastSpell(m_target, 79126, true, NULL, NULL, caster->GetObjectGuid());
                            }
                        }
                    }
                }
                return;
            }
            // Bandit's Guile
            else if (GetId() == 84748)
            {
                if (!apply && m_removeMode == AURA_REMOVE_BY_EXPIRE)
                {
                    if (Unit* caster = GetCaster())
                    {
                        caster->RemoveAurasDueToSpell(84745);   // Shallow Insight
                        caster->RemoveAurasDueToSpell(84746);   // Moderate Insight
                        caster->RemoveAurasDueToSpell(84747);   // Deep Insight
                    }
                }
                return;
            }
            else
                return;
            break;
        case SPELLFAMILY_HUNTER:
        {
            switch (GetId())
            {
                case 1130:                                  // Hunter's Mark
                case 88691:                                 // Marked for Death
                {
                    Unit* caster = GetCaster();
                    if (!caster)
                        return;

                    if (caster->GetTypeId() != TYPEID_PLAYER)
                        return;

                    if (apply)
                    {
                        // search Resistance is Futile
                        if (SpellEntry const * talent = ((Player*)caster)->GetKnownTalentRankById(9420))
                            caster->CastSpell(m_target, 83676, true);
                    }
                    else
                        m_target->RemoveAurasByCasterSpell(83676, caster->GetObjectGuid());
                    return;
                }
                case 13161:                                 // Aspect of the Beast
                {
                    if (Pet* pet = m_target->GetPet())
                    {
                        spellId1 = 61669;                   // Aspect of the Beast
                        if (!apply)
                        {
                            pet->RemoveAurasDueToSpell(spellId1);
                            return;
                        }
                    }
                    break;
                }
                case 34074:                                 // Aspect of the Viper
                {
                    if (!apply || m_target->HasAura(60144)) // Viper Attack Speed
                        spellId1 = 61609;                   // Vicious Viper
                    else
                        return;
                    break;
                }
                case 51755:                                 // Camouflage
                {
                    spellId1 = 80326;                       // Camouflage
                    break;
                }
                case 53257:                                 // Cobra strikes
                {
                    if (m_target->GetObjectGuid().IsPet())
                    {
                        if (!apply)
                            if (Unit* owner = ((Pet*)m_target)->GetOwner())
                                if (SpellAuraHolder* holder = owner->GetSpellAuraHolder(53257))
                                    owner->RemoveSpellAuraHolder(holder);
                    }
                    else if (apply)
                    {
                        if (Pet* pet = m_target->GetPet())
                            if (pet->isAlive())
                                pet->CastSpell(pet, 53257, true);
                    }
                    return;
                }
                case 77769:                                 // Trap Launcher
                {
                    spellId1 = 82946;
                    break;
                }
                case 90355:                                 // Ancient Hysteria
                {
                    if (apply)
                    {
                        Unit* caster = GetCaster();
                        if (!caster)
                            return;

                        caster->CastSpell(m_target, 57724, true);
                        return;
                    }
                    return;
                }
                default:
                    // Freezing Trap Effect
                    if (m_spellProto->IsFitToFamilyMask(UI64LIT(0x0000000000000008)))
                    {
                        if(!apply)
                        {
                            Unit *caster = GetCaster();
                            // Glyph of Freezing Trap
                            if (caster && caster->HasAura(56845))
                            {
                                cast_at_remove = true;
                                spellId1 = 61394;
                            }
                            else
                                return;
                        }
                        else
                            return;
                    }
                    // Aspect of the Dragonhawk dodge
                    else if (GetSpellProto()->IsFitToFamilyMask(UI64LIT(0x0000000000000000), 0x00001000))
                    {
                        spellId1 = 61848;

                        // triggered spell have same category as main spell and cooldown
                        if (apply && m_target->GetTypeId()==TYPEID_PLAYER)
                            ((Player*)m_target)->RemoveSpellCooldown(61848);
                    }
                    else
                        return;
                    break;
            }
            break;
        }
        case SPELLFAMILY_PALADIN:
        {
            if (m_spellProto->Id == 498)                    // Divine Protection
            {
                // Item - Paladin T12 Protection 4P Bonus
                if (!apply && m_target->HasAura(99091))
                {
                    cast_at_remove = true;
                    spellId1 = 99090;
                }
            }
            else if (m_spellProto->Id == 19746)             // Aura Mastery (on Concentration Aura remove and apply)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                if (apply && caster->HasAura(31821))
                    caster->CastSpell(caster, 64364, true);
                else
                    if (!apply)
                        caster->RemoveAurasDueToSpell(64364);
            }
            else if (m_spellProto->Id == 25780)             // Righteous Fury
                spellId1 = 57339;
            else if (m_spellProto->Id == 31821)             // Aura Mastery (on Aura Mastery original buff remove and apply)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                if (apply && caster->HasAura(19746))
                    caster->CastSpell(caster, 64364, true);
                else if (!apply)
                    caster->RemoveAurasDueToSpell(64364);
            }
            else if (m_spellProto->Id == 31842)             // Divine Illumination
            {
                if (Unit* caster = GetCaster())
                {
                    if (apply && caster->HasAura(70755))    // Item - Paladin T10 Holy 2P Bonus
                        caster->CastSpell(caster, 71166, true);
                    else if (!apply)
                        caster->RemoveAurasDueToSpell(71166);
                }
            }
            else if (m_spellProto->Id == 31884)             // Avenging Wrath
            {
                spellId1 = 57318;                           // Sanctified Wrath (triggered)
                break;
            }

            // Only process on player casting paladin aura
            // all aura bonuses applied also in aura area effect way to caster
            if (GetCasterGuid() != m_target->GetObjectGuid() || !GetCasterGuid().IsPlayer())
                return;

            if (GetSpellSpecific(m_spellProto->Id) != SPELL_AURA)
                return;

            // Communion, party damage increase
            spellId1 = 63531;                               // placeholder for talent spell mods
            // Improved Concentration Aura (auras bonus)
            spellId2 = 63510;                               // placeholder for talent spell mods
            // Improved Devotion Aura (auras bonus)
            spellId3 = 63514;                               // placeholder for talent spell mods
            break;
        }
        case SPELLFAMILY_SHAMAN:
        {
            // Maelstrom Weapon
            if (GetId() == 53817)
            {
                // Item - Shaman T13 Enhancement 2P Bonus (Maelstrom Weapon)
                if (!apply || m_target->HasAura(105866))
                    spellId1 = 105869;
                break;
            }
            // Spiritwalker's Grace
            else if (GetId() == 79206)
            {
                // Item - Shaman T13 Restoration 4P Bonus (Spiritwalker's Grace)
                if (!apply || m_target->HasAura(105876))
                    spellId1 = 105877;
                break;
            }
            // Temporal Maelstrom
            else if (GetId() == 105869)
            {
                // Maelstrom Weapon
                if (apply && !m_target->HasAura(53817))
                {
                    m_target->RemoveAurasDueToSpell(GetId());
                    return;
                }
                break;
            }

            if (apply)
            {
                Unit* caster = GetCaster();
                if (!caster)
                    return;

                // Bloodlust
                if (GetId() == 2825)
                    caster->CastSpell(m_target, 57724, true);
                // Heroism
                else if (GetId() == 32182)
                    caster->CastSpell(m_target, 57723, true);
            }
            break;
        }
        case SPELLFAMILY_DEATHKNIGHT:
        {
            // second part of spell apply
            switch (GetId())
            {
                case 46619:                                 // Raise ally
                {
                    if (!m_target || m_target->GetTypeId() != TYPEID_PLAYER)
                        return;
                    Player* m_player = (Player*)m_target;
                    if (apply)
                    {
                        // convert player to ghoul
                        m_player->SetDeathState(GHOULED);
                        m_player->SetHealth(1);
                        m_player->SetRoot(true);
                    }
                    else
                    {
                        m_player->SetRoot(false);
                        m_player->SetHealth(0);
                        m_player->SetDeathState(JUST_DIED);
                    }
                    break;
                }
                case 48263:                                 // Frost Presence
                case 48265:                                 // Unholy Presence
                case 48266:                                 // Blood Presence
                {
                    // else part one per 3 pair
                    if (GetId() == 48266 || GetId() == 48265)   // Frost Presence or Unholy Presence
                    {
                        // Improved Blood Presence
                        int32 reduce_pct = 0;
                        if (apply)
                        {
                            Unit::AuraList const& bloodAuras = m_target->GetAurasByType(SPELL_AURA_DUMMY);
                            for(Unit::AuraList::const_iterator itr = bloodAuras.begin(); itr != bloodAuras.end(); ++itr)
                            {
                                // skip same icon
                                if ((*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_GENERIC &&
                                    (*itr)->GetSpellProto()->SpellIconID == 2636 &&
                                    (*itr)->GetEffIndex() == EFFECT_INDEX_0)
                                {
                                    reduce_pct = -(*itr)->GetModifier()->m_amount;
                                    break;
                                }
                            }
                        }

                        if (reduce_pct)
                            m_target->CastCustomSpell(m_target, 61261, &reduce_pct, NULL, NULL, true, NULL, NULL, GetCasterGuid());
                        else
                            m_target->RemoveAurasDueToSpell(61261);
                    }
                    else if (apply)
                        m_target->CastSpell(m_target, 61261, true, NULL, NULL, GetCasterGuid());
                    else 
                        m_target->RemoveAurasDueToSpell(61261);

                    if (GetId() == 48263 || GetId() == 48266)   // Frost Presence or Blood Presence
                    {
                        // Improved Unholy Presence
                        int32 speed_pct = 0;
                        if (apply)
                        {
                            Unit::AuraList const& unholyAuras = m_target->GetAurasByType(SPELL_AURA_DUMMY);
                            for(Unit::AuraList::const_iterator itr = unholyAuras.begin(); itr != unholyAuras.end(); ++itr)
                            {
                                // skip same icon
                                if ((*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_DEATHKNIGHT &&
                                    (*itr)->GetSpellProto()->SpellIconID == 2633)
                                {
                                    speed_pct = (*itr)->GetModifier()->m_amount;
                                    break;
                                }
                            }
                        }

                        if (speed_pct)
                            m_target->CastCustomSpell(m_target, 63622, &speed_pct, NULL, NULL, true, NULL, NULL, GetCasterGuid());        // Unholy Presence, speed part, spell1 used for Improvement presence fit to own presence
                        else
                            m_target->RemoveAurasDueToSpell(63622);
                    }
                    else if(apply)
                        m_target->CastSpell(m_target, 63622, true, NULL, NULL, GetCasterGuid());
                    else
                        m_target->RemoveAurasDueToSpell(63622);

                    if (GetId() == 48265 || GetId() == 48263)   // Unholy Presence or Blood Presence
                    {
                        // Improved Frost Presence
                        int32 regen_pct = 0;
                        if (apply)
                        {
                            Unit::AuraList const& frostAuras = m_target->GetAurasByType(SPELL_AURA_DUMMY);
                            for(Unit::AuraList::const_iterator itr = frostAuras.begin(); itr != frostAuras.end(); ++itr)
                            {
                                // skip same icon
                                if ((*itr)->GetSpellProto()->GetSpellFamilyName() == SPELLFAMILY_DEATHKNIGHT &&
                                    (*itr)->GetSpellProto()->SpellIconID == 2632)
                                {
                                    regen_pct = (*itr)->GetModifier()->m_amount;
                                    break;
                                }
                            }
                        }

                        if (regen_pct)
                            m_target->CastCustomSpell(m_target, 63621, &regen_pct, NULL, NULL, true, NULL, NULL, GetCasterGuid());
                        else
                            m_target->RemoveAurasDueToSpell(63621);
                    }
                    else if (apply)
                        m_target->CastSpell(m_target, 63621, true, NULL, NULL, GetCasterGuid());
                    else
                        m_target->RemoveAurasDueToSpell(63621);
                    return;
                }
                case 49039: spellId1 = 50397; break;        // Lichborne
                case 51124:                                 // Killing Machine
                {
                    if (apply && m_target->HasAura(90459))  // Item - Death Knight T11 DPS 4P Bonus
                        spellId1 = 90507;                   // Death Eater
                    break;
                }
                // Pillar of Frost
                case 51271:
                {
                    if (apply)
                    {
                        // Glyph of Pillar of Frost
                        if (m_target->HasAura(58635))
                            spellId1 = 90259;
                    }
                    else
                        spellId1 = 90259;
                    break;
                }
                case 55095:                                 // Frost Fever
                {
                    if (!apply)
                    {
                        // Remove Brittle Bones debuffs
                        spellId1 = 81325;
                        spellId1 = 81326;
                    }
                    else
                        return;
                    break;
                }
                case 81256:                                 // Dancing Rune Weapon
                {
                    // Item - Death Knight T12 Blood 4P Bonus
                    if (!apply && m_target->HasAura(98966))
                    {
                        cast_at_remove = true;
                        spellId1 = 101162;                  // Flaming Rune Weapon
                    }
                    break;
                }
            }

            // Improved Frost Presence
            if (GetSpellProto()->SpellIconID == 2632 && m_isPassive)
            {
                // if presence active: Unholy Presence or Blood Presence
                if (apply && (m_target->HasAura(48265) || m_target->HasAura(48263)))
                {
                    Aura* aura = GetAuraByEffectIndex(EFFECT_INDEX_0);
                    if (!aura)
                        return;

                    int32 bp = aura->GetModifier()->m_amount;
                    m_target->CastCustomSpell(m_target, 63621, &bp, NULL, NULL, true, NULL, NULL, GetCasterGuid());
                }
                else
                    m_target->RemoveAurasDueToSpell(63621);
                return;
            }

            // Improved Unholy Presence
            if (GetSpellProto()->SpellIconID == 2633 && m_isPassive)
            {
                // if presence active: Frost Presence or Blood Presence
                if (apply && (m_target->HasAura(48263) || m_target->HasAura(48266)))
                {
                    Aura* aura = GetAuraByEffectIndex(EFFECT_INDEX_0);
                    if (!aura)
                        return;

                    int32 bp = aura->GetModifier()->m_amount;
                    m_target->CastCustomSpell(m_target, 63622, &bp, NULL, NULL, true, NULL, NULL, GetCasterGuid());
                }
                else
                    m_target->RemoveAurasDueToSpell(63622);
                return;
            }
            break;
        }
        default:
            return;
    }

    // prevent aura deletion, specially in multi-boost case
    SetInUse(true);

    if (apply || cast_at_remove)
    {
        if (spellId1)
            m_target->CastSpell(m_target, spellId1, true, NULL, NULL, GetCasterGuid());
        if (spellId2 && !IsDeleted())
            m_target->CastSpell(m_target, spellId2, true, NULL, NULL, GetCasterGuid());
        if (spellId3 && !IsDeleted())
            m_target->CastSpell(m_target, spellId3, true, NULL, NULL, GetCasterGuid());
        if (spellId4 && !IsDeleted())
            m_target->CastSpell(m_target, spellId4, true, NULL, NULL, GetCasterGuid());
    }
    else
    {
        if (spellId1)
            m_target->RemoveAurasByCasterSpell(spellId1, GetCasterGuid());
        if (spellId2)
            m_target->RemoveAurasByCasterSpell(spellId2, GetCasterGuid());
        if (spellId3)
            m_target->RemoveAurasByCasterSpell(spellId3, GetCasterGuid());
        if (spellId4)
            m_target->RemoveAurasByCasterSpell(spellId4, GetCasterGuid());
    }

    SetInUse(false);
}

SpellAuraHolder::~SpellAuraHolder()
{
    // note: auras in delete list won't be affected since they clear themselves from holder when adding to deletedAuraslist
    for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (Aura *aur = m_auras[i])
            delete aur;
}

void SpellAuraHolder::Update(uint32 diff)
{
    if (m_duration > 0)
    {
        m_duration -= diff;
        if (m_duration < 0)
            m_duration = 0;

        m_timeCla -= diff;

        if (m_timeCla <= 0)
        {
            if (Unit* caster = GetCaster())
            {
                Powers powertype = Powers(GetSpellProto()->powerType);
                m_timeCla = 1*IN_MILLISECONDS;

                if (SpellPowerEntry const* spellPower = GetSpellProto()->GetSpellPower())
                {
                    if (int32 manaPerSecond = spellPower->manaPerSecond)
                    {
                        if (powertype == POWER_HEALTH)
                            caster->ModifyHealth(-manaPerSecond);
                        else
                            caster->ModifyPower(powertype, -manaPerSecond);
                    }
                }
            }
        }
    }

    for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (Aura *aura = m_auras[i])
            aura->UpdateAura(diff);

    // Channeled aura required check distance from caster
    if(IsChanneledSpell(m_spellProto) && GetCasterGuid() != m_target->GetObjectGuid())
    {
        Unit* caster = GetCaster();
        if(!caster)
        {
            m_target->RemoveAurasByCasterSpell(GetId(), GetCasterGuid());
            return;
        }

        // need check distance for channeled target only
        if (caster->GetChannelObjectGuid() == m_target->GetObjectGuid())
        {
            // Get spell range
            float max_range = GetSpellMaxRange(sSpellRangeStore.LookupEntry(m_spellProto->rangeIndex));

            if(Player* modOwner = caster->GetSpellModOwner())
                modOwner->ApplySpellMod(GetId(), SPELLMOD_RANGE, max_range, NULL);

            if(!caster->IsWithinDistInMap(m_target, max_range))
            {
                caster->InterruptSpell(CURRENT_CHANNELED_SPELL);
                return;
            }
        }
    }
}

void SpellAuraHolder::RefreshHolder()
{
    SetAuraDuration(GetAuraMaxDuration());

    SendAuraUpdate(false);
}

void SpellAuraHolder::SetAuraMaxDuration(int32 duration)
{
    m_maxDuration = duration;

    // possible overwrite persistent state
    if (duration > 0)
    {
        if (!(IsPassive() && GetSpellProto()->DurationIndex == 0))
            SetPermanent(false);

        SetAuraFlags(GetAuraFlags() | AFLAG_DURATION);
    }
    else
        SetAuraFlags(GetAuraFlags() & ~AFLAG_DURATION);
}

bool SpellAuraHolder::HasMechanic(uint32 mechanic) const
{
    if (mechanic == m_spellProto->GetMechanic())
        return true;

    for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        if (Aura* aura = m_auras[i])
        {
            if (aura->GetSpellEffect()->EffectMechanic == mechanic)
                return true;
        }
    }
    return false;
}

bool SpellAuraHolder::HasMechanicMask(uint32 mechanicMask) const
{
    if (mechanicMask & (1 << (m_spellProto->GetMechanic() - 1)))
        return true;

    for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        if (Aura* aura = m_auras[i])
        {
            if (aura->GetSpellEffect()->EffectMechanic && ((1 << (aura->GetSpellEffect()->EffectMechanic - 1)) & mechanicMask))
                return true;
        }
    }
    return false;
}

bool SpellAuraHolder::IsPersistent() const
{
    for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (Aura *aur = m_auras[i])
            if (aur->IsPersistent())
                return true;
    return false;
}

bool SpellAuraHolder::IsPositive() const
{
    for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (Aura *aur = m_auras[i])
            if (!aur->IsPositive())
                return false;
    return true;
}

bool SpellAuraHolder::IsAreaAura() const
{
    for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (Aura *aur = m_auras[i])
            if (aur->IsAreaAura())
                return true;
    return false;
}

bool SpellAuraHolder::IsEmptyHolder() const
{
    for (int32 i = 0; i < MAX_EFFECT_INDEX; ++i)
        if (m_auras[i])
            return false;
    return true;
}

void SpellAuraHolder::UnregisterSingleCastHolder()
{
    if (IsSingleTarget())
    {
        if(Unit* caster = GetCaster())
        {
            caster->GetSingleCastSpellTargets().erase(GetSpellProto());
        }
        else
        {
            ERROR_LOG("Couldn't find the caster of the single target aura (SpellId %u), may crash later!", GetId());
            //ASSERT(false);
        }
        m_isSingleTarget = false;
    }
}

void Aura::HandleAuraSetVehicle(bool apply, bool real)
{
    if (!real)
        return;

    Unit* target = GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER || !target->IsInWorld())
        return;

    uint32 vehicleId = GetMiscValue();

    if (vehicleId == 0)
        return;

    if (apply)
    {
        if (!target->CreateVehicleKit(vehicleId))
            return;
    }
    else
        if (target->GetVehicleKit())
            target->RemoveVehicleKit();

    WorldPacket data(SMSG_SET_VEHICLE_REC_ID, target->GetPackGUID().size()+4);
    data.appendPackGUID(target->GetObjectGuid().GetRawValue());
    data << uint32(apply ? vehicleId : 0);
    target->SendMessageToSet(&data, true);

    if (apply)
    {
        data.Initialize(SMSG_ON_CANCEL_EXPECTED_RIDE_VEHICLE_AURA, 0);
        ((Player*)target)->GetSession()->SendPacket(&data);
    }
}

void SpellAuraHolder::SendFakeAuraUpdate(uint32 auraId, bool remove)
{
    if (!GetTarget())
        return;

    WorldPacket data(SMSG_AURA_UPDATE);
    data << m_target->GetPackGUID();
    data << uint8(MAX_AURAS);
    data << uint32(remove ? 0 : auraId);

    if(remove)
    {
        GetTarget()->SendMessageToSet(&data, true);
        return;
    }

    uint8 auraFlags = GetAuraFlags();
    data << uint16(auraFlags);
    data << uint8(GetAuraLevel());
    data << uint8(GetStackAmount() > 1 ? GetStackAmount() : (GetAuraCharges()) ? GetAuraCharges() : 1);

    if (!(auraFlags & AFLAG_NOT_CASTER))
    {
        data << GetCasterGuid().WriteAsPacked();
    }

    if (auraFlags & AFLAG_DURATION)
    {
        data << uint32(GetAuraMaxDuration());
        data << uint32(GetAuraDuration());
    }

    if (auraFlags & AFLAG_EFFECT_AMOUNT_SEND)
    {
        for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
            if (auraFlags & (1 << i))
                if (Aura const* aura = m_auras[i])
                    data << int32(aura->GetModifier()->m_amount);
                else
                    data << int32(0);
    }

    GetTarget()->SendMessageToSet(&data, true);
}

void Aura::HandleModDamageTaken(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* unitTarget = GetTarget();

    switch (GetSpellProto()->GetSpellFamilyName())
    {
        case SPELLFAMILY_GENERIC:
        {
            switch (GetId())
            {
                case 54306:                                 // Protective Bubble
                {
                    if (GetStackAmount() == GetSpellProto()->GetStackAmount())
                    {
                        unitTarget->RemoveAurasDueToSpell(GetId());
                    }
                    break;
                }
            }
            break;
        }
    }
}

void Aura::HandleAuraModReflectSpells(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();
    Unit* caster = GetCaster();

    if (apply)
    {
        switch (GetId())
        {
            case 23920:                 // Spell Reflection
            {
                if (!caster)
                    return;

                // Search Improved Spell Reflection
                Unit::AuraList const& lDummyAuras = caster->GetAurasByType(SPELL_AURA_DUMMY);
                for(Unit::AuraList::const_iterator i = lDummyAuras.begin(); i != lDummyAuras.end(); ++i)
                {
                    if((*i)->GetSpellProto()->SpellIconID == 1935)
                    {
                        caster->CastSpell(caster, 59725, true);
                        break;
                    }
                }
                break;
            }
        }
    }
}

void Aura::HandleAuraFakeInebriation(bool apply, bool Real)
{
    Unit* target = GetTarget();

    Player* pTarget = target->GetTypeId() == TYPEID_PLAYER ? (Player*)target : NULL;
    if (apply)
    {
        target->m_detectInvisibilityMask |= (1 << INVISIBILITY_DRUNK);

        if (pTarget)
        {
            int32 oldval = pTarget->GetInt32Value(PLAYER_FAKE_INEBRIATION);
            pTarget->SetInt32Value(PLAYER_FAKE_INEBRIATION, oldval + m_modifier.m_amount);
        }
    }
    else
    {
        bool removeDetect = !target->HasAuraType(SPELL_AURA_FAKE_INEBRIATE);

        if (pTarget)
        {
            int32 oldval = pTarget->GetInt32Value(PLAYER_FAKE_INEBRIATION);
            pTarget->SetInt32Value(PLAYER_FAKE_INEBRIATION, oldval - m_modifier.m_amount);

            if (removeDetect)
                removeDetect = !pTarget->GetDrunkValue();
        }

        if (removeDetect)
            target->m_detectInvisibilityMask &= ~(1 << INVISIBILITY_DRUNK);
    }

    target->UpdateObjectVisibility();
}

void Aura::HandleAuraLinked(bool apply, bool Real)
{
    if (!Real)
        return;

    SpellEntry const * spellInfo = GetSpellProto();
    uint32 linkedSpell = GetSpellEffect()->EffectTriggerSpell;
    SpellEntry const * linkedInfo = sSpellStore.LookupEntry(linkedSpell);
    if (!linkedInfo)
    {
        ERROR_LOG("HandleAuraLinked for spell %u effect %u: triggering unknown spell %u", spellInfo->Id, m_effIndex, linkedSpell);
        return;
    }

    Unit* target = GetTarget();
    Unit* caster = target;

    for (uint8 i = 0; i < MAX_EFFECT_INDEX; ++i)
    {
        SpellEffectEntry const * effect = spellInfo->GetSpellEffect(SpellEffectIndex(i));
        if (!effect)
            continue;

        if (effect->EffectImplicitTargetA == TARGET_RANDOM_ENEMY_CHAIN_IN_AREA
            || effect->EffectImplicitTargetB == TARGET_RANDOM_ENEMY_CHAIN_IN_AREA
            || effect->EffectImplicitTargetA == TARGET_CHANNEL
            || effect->EffectImplicitTargetB == TARGET_CHANNEL
            || effect->EffectImplicitTargetA == TARGET_AREAEFFECT_CUSTOM
            || effect->EffectImplicitTargetB == TARGET_AREAEFFECT_CUSTOM)
            caster = GetCaster();
    }

    if (!caster)
        return;

    if (apply)
    {
        if (m_modifier.m_amount)
            caster->CastCustomSpell(target, linkedSpell, &m_modifier.m_amount, NULL, NULL, true, NULL, this);
        else
            caster->CastSpell(target, linkedSpell, true, NULL, this);
    }
    else
        target->RemoveAurasByCasterSpell(linkedSpell, caster->GetObjectGuid()); 
}

void Aura::HandleAllowOnlyAbility(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();
    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    if (apply)
        target->SetFlag(PLAYER_FLAGS, PLAYER_FLAGS_ALLOW_ONLY_ABILITY);
    else
    {
        if (target->HasAuraType(SPELL_AURA_ALLOW_ONLY_ABILITY))
            return;

        target->RemoveFlag(PLAYER_FLAGS, PLAYER_FLAGS_ALLOW_ONLY_ABILITY);
    }
}

void Aura::HandleAuraForceWeather(bool apply, bool Real)
{
    if (!Real)
        return;

    Unit* target = GetTarget();

    if (target->GetTypeId() != TYPEID_PLAYER)
        return;

    Player* plrTarget = (Player*)target;

    if (apply)
    {
        WorldPacket data(SMSG_WEATHER, (4 + 4 + 1));

        data << uint32(GetMiscValue()) << 1.0f << uint8(0);
        plrTarget->GetSession()->SendPacket(&data);
    }
    else
    {
        // send weather for current zone
        if (Weather* weather = sWorld.FindWeather(plrTarget->GetZoneId()))
            weather->SendWeatherUpdateToPlayer(plrTarget);
        else
        {
            if (!sWorld.AddWeather(plrTarget->GetZoneId()))
            {
                // send fine weather packet to remove old weather
                Weather::SendFineWeatherUpdateToPlayer(plrTarget);
            }
        }
    }
}

void Aura::HandleModIncreaseSpellPowerPct(bool apply, bool Real)
{
    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    ((Player*)GetTarget())->UpdateSpellDamageAndHealingBonus();
}

void Aura::HandleOverrideSpellPowerByAp(bool apply, bool Real)
{
    if (GetTarget()->GetTypeId() != TYPEID_PLAYER)
        return;

    ((Player*)GetTarget())->UpdateSpellDamageAndHealingBonus();
}

void Aura::HandleAuraPreventResurrection(bool apply, bool Real)
{
    if (apply)
        GetTarget()->RemoveByteFlag(PLAYER_FIELD_BYTES, 0, PLAYER_FIELD_BYTE_RELEASE_TIMER);
    else if (!GetTarget()->GetMap()->Instanceable())
        GetTarget()->SetByteFlag(PLAYER_FIELD_BYTES, 0, PLAYER_FIELD_BYTE_RELEASE_TIMER);
}

void Aura::HandleAuraOverrideActionbarSpells(bool apply, bool Real)
{
    Unit* target = GetTarget();

    if (apply)
    {
        switch (GetId())
        {
            case 48108:         // Hot Streak!
            case 88688:         // Surge of Light
                GetHolder()->SetAuraCharges(1);
                break;
        }
    }
    else
    {
        // Soul Swap
        if (GetId() == 86211)
        {
            if (target->GetTypeId() == TYPEID_PLAYER)
            {
                ((Player*)target)->m_soulSwapData.spells.clear();
                ((Player*)target)->m_soulSwapData.swapTarget.Clear();
            }
        }
    }
}

void Aura::HandleAuraInterfereTargeting(bool apply, bool Real)
{
    Unit* target = GetTarget();

    if (apply)
    {
        // Smoke Bomb
        if (GetId() == 88611)
        {
            if (Unit* caster = GetCaster())
                if (target->IsHostileTo(caster))
                    target->RemoveAurasWithDispelType(DISPEL_STEALTH);
        }
    }
}

void Aura::HandleAuraSeeWhileInvisible(bool apply, bool Real)
{
    Unit* target = GetTarget();

    if (Real && target->GetTypeId() == TYPEID_PLAYER)
        ((Player*)target)->GetCamera().UpdateVisibilityForOwner();
}

void Aura::HandleAltPowerIndicator(bool apply, bool Real)
{
    Unit* target = GetTarget();

    if (!apply)
        target->SetPower(POWER_ALTERNATIVE, 0);
}

void Aura::HandleAuraModCategoryCooldown(bool apply, bool Real)
{
    Unit* target = GetTarget();
    if (target->GetTypeId() != TYPEID_PLAYER || ((Player*)target)->GetSession()->PlayerLoading())
        return;

    ((Player*)target)->SendCategoryCooldownMods();
}
