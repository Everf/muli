/* Copyright (C) 2006 - 2013 ScriptDev2 <http://www.scriptdev2.com/>
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

/* ScriptData
SDName: boss_lady_nazjar
SD%Complete: 0
SDComment: Placeholder
SDCategory: Throne of the Tides
EndScriptData */

#include "precompiled.h"
#include "throne_of_the_tides.h"
#include "SpellMgr.h"
#include "Spell.h"

enum ScriptTexts
{
    SAY_AGGRO   = 0,
    SAY_KILL    = 1,
    SAY_DEATH   = 2,
    SAY_66      = 3,
    SAY_33      = 4,
};

enum Spells
{
    SPELL_FUNGAL_SPORES         = 76001,
    SPELL_FUNGAL_SPORES_DMG     = 80564,
    SPELL_FUNGAL_SPORES_DMG_H   = 91470,
    SPELL_SHOCK_BLAST           = 76008,
    SPELL_SHOCK_BLAST_H         = 91477,
    SPELL_SUMMON_GEYSER         = 75722,
    SPELL_GEYSER_VISUAL         = 75699,
    SPELL_GEYSER_ERRUPT         = 75700,
    SPELL_GEYSER_ERRUPT_H       = 91469,
    SPELL_GEYSER_ERRUPT_KNOCK   = 94046,
    SPELL_GEYSER_ERRUPT_KNOCK_H = 94047,
    SPELL_WATERSPOUT            = 75683,
    SPELL_WATERSPOUT_KNOCK      = 75690,
    SPELL_WATERSPOUT_SUMMON     = 90495,
    SPELL_WATERSPOUT_VISUAL     = 90440,
    SPELL_WATERSPOUT_DMG        = 90479,
    SPELL_VISUAL_INFIGHT_AURA   = 91349,

    // honnor guard
    SPELL_ARC_SLASH             = 75907,
    SPELL_ENRAGE                = 75998,

    // tempest witch
    SPELL_CHAIN_LIGHTNING       = 75813,
    SPELL_CHAIN_LIGHTNING_H     = 91450,
    SPELL_LIGHTNING_SURGE       = 75992,
    SPELL_LIGHTNING_SURGE_DMG   = 75993,
    SPELL_LIGHTNING_SURGE_DMG_H = 91451,
};

enum Events
{
    EVENT_GEYSER            = 1,
    EVENT_GEYSER_ERRUPT     = 2,
    EVENT_FUNGAL_SPORES     = 3,
    EVENT_SHOCK_BLAST       = 4,
    EVENT_WATERSPOUT_END    = 5,
    EVENT_START_ATTACK      = 6,
    EVENT_ARC_SLASH         = 7,
    EVENT_LIGHTNING_SURGE   = 8,
    EVENT_CHAIN_LIGHTNING   = 9,
};

enum Points
{
    POINT_CENTER_1      = 1,
    POINT_CENTER_2      = 2,
    POINT_WATERSPOUT    = 3,
};

enum Adds
{
    NPC_TEMPEST_WITCH       = 44404,
    NPC_HONNOR_GUARD        = 40633,
    NPC_WATERSPOUT          = 48571,
    NPC_WATERSPOUT_H        = 49108,
    NPC_GEYSER              = 40597,
};

const Position2 summonPos[3] =
{
    {174.41f, 802.323f, 808.368f, 0.014f},
    {200.517f, 787.687f, 808.368f, 2.056f},
    {200.558f, 817.046f, 808.368f, 4.141f}
};

const Position2 centerPos = {192.05f, 802.52f, 807.64f, 3.14f};

struct MANGOS_DLL_DECL boss_lady_nazjarAI : public ScriptedAI
{
    boss_lady_nazjarAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;
    uint8 uiSpawnCount;
    uint8 uiPhase;
    EventMap events;
    std::list<ObjectGuid> summons;

    void DepspawnAllSummons()
    {
        for (std::list<ObjectGuid>::iterator itr = summons.begin(); itr != summons.end(); ++itr)
            if (Creature* unit = me->GetMap()->GetAnyTypeCreature(*itr))
                unit->ForcedDespawn();
    }

    void Reset() override
    {
        me->ApplySpellImmune(0, IMMUNITY_EFFECT, SPELL_EFFECT_KNOCK_BACK, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_GRIP, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_STUN, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FEAR, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_ROOT, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_FREEZE, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_POLYMORPH, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_HORROR, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_SAPPED, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_CHARM, true);
        me->ApplySpellImmune(0, IMMUNITY_MECHANIC, MECHANIC_DISORIENTED, true);
        me->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_CONFUSE, true);

        uiPhase = 0;
        uiSpawnCount = 3;
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        events.Reset();

        DepspawnAllSummons();
    }

    void Aggro(Unit* pWho) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LADY_NAZJAR, IN_PROGRESS);

        events.ScheduleEvent(EVENT_GEYSER, 11000);
        events.ScheduleEvent(EVENT_FUNGAL_SPORES, urand(3000,10000));
        events.ScheduleEvent(EVENT_SHOCK_BLAST, urand(6000,12000));
    }

    void JustDied(Unit* pKiller) override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LADY_NAZJAR, DONE);

        //Talk(SAY_DEATH);
        DepspawnAllSummons();
    }

    void SummonedCreatureJustDied(Creature* pCreature) override
    {
        switch(pCreature->GetEntry())
        {
            case NPC_TEMPEST_WITCH:
            case NPC_HONNOR_GUARD:
                uiSpawnCount--;
                break;
        }
    }

    void JustSummoned(Creature* summon) override
    {
        summons.push_back(summon->GetObjectGuid());

        switch(summon->GetEntry())
        {
            case NPC_TEMPEST_WITCH:
            case NPC_HONNOR_GUARD:
                if (me->isInCombat())
                    summon->SetInCombatWithZone();
                break;
            case NPC_WATERSPOUT:
            case NPC_WATERSPOUT_H:
                if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                {
                    float x, y;
                    me->GetNearPoint2D(x, y, 30.0f, me->GetAngle(pTarget->GetPositionX(), pTarget->GetPositionY()));
                    summon->GetMotionMaster()->MovePoint(POINT_WATERSPOUT, x, y, 808.0f);
                }
                break;
        }
    }

    void KilledUnit(Unit* pVictim) override
    {
        //Talk(SAY_KILL);
    }

    void SpellHit(Unit* caster, SpellEntry const* spellProto) override
    {
        if (Spell const* spell = me->GetCurrentSpell(CURRENT_GENERIC_SPELL))
            if (spell->m_spellInfo->Id == SPELL_SHOCK_BLAST
                || me->GetCurrentSpell(CURRENT_GENERIC_SPELL)->m_spellInfo->Id == SPELL_SHOCK_BLAST_H)
                for (uint8 i = 0; i < 3; ++i)
                    if (IsSpellHaveEffect(spellProto, SPELL_EFFECT_INTERRUPT_CAST))
                        me->InterruptSpell(CURRENT_GENERIC_SPELL);
    }

    void JustReachedHome() override
    {
        if (m_pInstance)
            m_pInstance->SetData(TYPE_LADY_NAZJAR, FAIL);

        DepspawnAllSummons();
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == POINT_MOTION_TYPE)
        {
            if (id == POINT_CENTER_1)
            {
                //Talk(SAY_66);
                SetCombatMovement(false);
                if (!m_bIsRegularMode)
                    DoCast(me, SPELL_WATERSPOUT_SUMMON, true);
                DoCast(me, SPELL_WATERSPOUT);
                me->SummonCreature(NPC_HONNOR_GUARD, summonPos[0].x, summonPos[0].y, summonPos[0].z, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                me->SummonCreature(NPC_TEMPEST_WITCH, summonPos[1].x, summonPos[1].y, summonPos[1].z, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                me->SummonCreature(NPC_TEMPEST_WITCH, summonPos[2].x, summonPos[2].y, summonPos[2].z, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                events.ScheduleEvent(EVENT_WATERSPOUT_END, 60000);
            }
            else if (id == POINT_CENTER_2)
            {
                //Talk(SAY_33);
                SetCombatMovement(false);
                if (!m_bIsRegularMode)
                    DoCast(me, SPELL_WATERSPOUT_SUMMON, true);
                DoCast(me, SPELL_WATERSPOUT);
                me->SummonCreature(NPC_HONNOR_GUARD, summonPos[0].x, summonPos[0].y, summonPos[0].z, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                me->SummonCreature(NPC_TEMPEST_WITCH, summonPos[1].x, summonPos[1].y, summonPos[1].z, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                me->SummonCreature(NPC_TEMPEST_WITCH, summonPos[2].x, summonPos[2].y, summonPos[2].z, 0.0f, TEMPSUMMON_MANUAL_DESPAWN, 0);
                events.ScheduleEvent(EVENT_WATERSPOUT_END, 60000);
            }
        }
    }

    void WaterspoutEnd()
    {

        uiPhase++;
        events.CancelEvent(EVENT_WATERSPOUT_END);
        me->RemoveAurasDueToSpell(SPELL_WATERSPOUT_SUMMON);
        me->InterruptNonMeleeSpells(false);
        me->CastStop();
        SetCombatMovement(true);
        me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        me->GetMotionMaster()->MoveChase(me->getVictim());
        events.RescheduleEvent(EVENT_GEYSER, 11000);
        events.RescheduleEvent(EVENT_FUNGAL_SPORES, urand(3000,10000));
        events.RescheduleEvent(EVENT_SHOCK_BLAST, urand(6000,12000));
    }

    void UpdateAI(const uint32 uiDiff) override
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if ((uiPhase == 1 || uiPhase == 3) && uiSpawnCount == 0)
        {
            WaterspoutEnd();
            return;
        }

        if (me->GetHealthPercent() < 66.0f && uiPhase == 0)
        {              
            uiPhase = 1;
            uiSpawnCount = 3;
            me->InterruptNonMeleeSpells(false);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
            me->GetMotionMaster()->MovePoint(POINT_CENTER_1, centerPos.x, centerPos.y, centerPos.z);
            return;
        }

        if (me->GetHealthPercent() < 33.0f && uiPhase == 2)
        {              
            uiPhase = 3;
            uiSpawnCount = 3;
            me->InterruptNonMeleeSpells(false);
            me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
            me->GetMotionMaster()->MovePoint(POINT_CENTER_2, centerPos.x, centerPos.y, centerPos.z);
            return;
        } 

        switch (uiPhase)
        {
            case 1:
            case 3:
                events.Update(uiDiff);

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                        case EVENT_WATERSPOUT_END:
                            WaterspoutEnd();
                            break;
                    }
                }
                break;
            case 0:
            case 2:
            case 4:
                events.Update(uiDiff);

                if (me->IsNonMeleeSpellCasted(false))
                    return;

                while (uint32 eventId = events.ExecuteEvent())
                {
                    switch (eventId)
                    {
                    case EVENT_GEYSER:
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                            DoCast(pTarget, SPELL_SUMMON_GEYSER);
                        events.ScheduleEvent(EVENT_GEYSER, urand(14000,17000));
                        break;
                    case EVENT_FUNGAL_SPORES:
                        if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                            DoCast(pTarget,SPELL_FUNGAL_SPORES);
                        events.ScheduleEvent(EVENT_FUNGAL_SPORES, urand(15000,18000));
                        break;
                    case EVENT_SHOCK_BLAST:
                        DoCast(me->getVictim(), SPELL_SHOCK_BLAST);
                        events.ScheduleEvent(EVENT_SHOCK_BLAST, urand(12000,14000));
                        break;
                    }
                }
                DoMeleeAttackIfReady();
                break;
        }
    }
};

CreatureAI* GetAI_boss_lady_nazjar(Creature* pCreature)
{
    return new boss_lady_nazjarAI(pCreature);
}

struct MANGOS_DLL_DECL npc_lady_nazjar_honnor_guardAI : public ScriptedAI
{
    npc_lady_nazjar_honnor_guardAI(Creature* creature) : ScriptedAI(creature)
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        Reset();
    }

    EventMap events;
    bool bEnrage;

    void Reset() override
    {
        bEnrage = false;
        events.ScheduleEvent(EVENT_START_ATTACK, 2000);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->IsNonMeleeSpellCasted(false))
            return;

        if (me->GetHealthPercent() < 35.0f && !bEnrage)
        {
            DoCast(me, SPELL_ENRAGE);
            bEnrage = true;
            return;
        }

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_START_ATTACK:
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                    events.ScheduleEvent(EVENT_ARC_SLASH, 5000);
                    break;
                case EVENT_ARC_SLASH:
                    DoCast(me, SPELL_ARC_SLASH);
                    events.ScheduleEvent(EVENT_ARC_SLASH, urand(7000, 10000));
                    break;
            }
        }

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_lady_nazjar_honnor_guard(Creature* pCreature)
{
    return new npc_lady_nazjar_honnor_guardAI(pCreature);
}

struct MANGOS_DLL_DECL npc_lady_nazjar_tempest_witchAI : public Scripted_NoMovementAI
{
    npc_lady_nazjar_tempest_witchAI(Creature* creature) : Scripted_NoMovementAI(creature)
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        Reset();
    }

    EventMap events;

    void Reset() override
    {
        events.Reset();
        events.ScheduleEvent(EVENT_START_ATTACK, 2000);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        events.Update(diff);

        if (me->IsNonMeleeSpellCasted(false))
            return;

        while (uint32 eventId = events.ExecuteEvent())
        {
            switch (eventId)
            {
                case EVENT_START_ATTACK:
                    me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
                    events.ScheduleEvent(EVENT_LIGHTNING_SURGE, urand(5000, 7000));
                    events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 2000);
                    break;
                case EVENT_LIGHTNING_SURGE:
                    if (Unit* pTarget = m_creature->SelectAttackingTarget(ATTACKING_TARGET_RANDOM, 0))
                        DoCast(pTarget, SPELL_LIGHTNING_SURGE);
                    events.ScheduleEvent(EVENT_LIGHTNING_SURGE, urand(10000, 15000));
                    break;
                case EVENT_CHAIN_LIGHTNING:
                    DoCast(me->getVictim(), SPELL_CHAIN_LIGHTNING);
                    events.ScheduleEvent(EVENT_CHAIN_LIGHTNING, 2000);
                    break;
            }
        }
    }
};

CreatureAI* GetAI_npc_lady_nazjar_tempest_witch(Creature* pCreature)
{
    return new npc_lady_nazjar_tempest_witchAI(pCreature);
}

struct MANGOS_DLL_DECL npc_lady_nazjar_waterspoutAI : public ScriptedAI
{
    npc_lady_nazjar_waterspoutAI(Creature* creature) : ScriptedAI(creature)
    {
        Reset();
    }

    bool bHit;

    void Reset() override
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);
        me->SetSpeedRate(MOVE_RUN, 0.3f);
        DoCast(me, SPELL_WATERSPOUT_VISUAL, true);
        bHit = false;
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if (type == POINT_MOTION_TYPE)
        {
            if (id == POINT_WATERSPOUT)
                me->ForcedDespawn();
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (bHit)
            return;

        if (Unit* pTarget = GetClosestAttackableUnit(me, 2.0f))
        {
            if (pTarget->GetTypeId() != TYPEID_PLAYER)
                return;

            bHit = true;

            pTarget->CastSpell(pTarget, SPELL_WATERSPOUT_DMG, true);
        }
    }
};

CreatureAI* GetAI_npc_lady_nazjar_waterspout(Creature* pCreature)
{
    return new npc_lady_nazjar_waterspoutAI(pCreature);
}

struct MANGOS_DLL_DECL npc_lady_nazjar_geyserAI : public Scripted_NoMovementAI
{
    npc_lady_nazjar_geyserAI(Creature* creature) : Scripted_NoMovementAI(creature)
    {
        Reset();
    }

    uint32 uiErruptTimer;
    bool bErrupt;

    void Reset() override
    {
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
        me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_PASSIVE);

        uiErruptTimer = 5000;
        bErrupt = false;
        DoCast(me, SPELL_GEYSER_VISUAL, true);
    }

    void UpdateAI(const uint32 diff) override
    {
        if (uiErruptTimer <= diff && !bErrupt)
        {
            bErrupt = true;
            me->RemoveAurasDueToSpell(SPELL_GEYSER_VISUAL);
            DoCast(me, SPELL_GEYSER_ERRUPT, true);
        }
        else
            uiErruptTimer -= diff;
    }
};

CreatureAI* GetAI_npc_lady_nazjar_geyser(Creature* pCreature)
{
    return new npc_lady_nazjar_geyserAI(pCreature);
}

void AddSC_boss_lady_nazjar()
{
    Script* pNewScript;

    pNewScript = new Script;
    pNewScript->Name = "boss_lady_nazjar";
    pNewScript->GetAI = &GetAI_boss_lady_nazjar;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_lady_nazjar_honnor_guard";
    pNewScript->GetAI = &GetAI_npc_lady_nazjar_honnor_guard;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_lady_nazjar_tempest_witch";
    pNewScript->GetAI = &GetAI_npc_lady_nazjar_tempest_witch;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_lady_nazjar_waterspout";
    pNewScript->GetAI = &GetAI_npc_lady_nazjar_waterspout;
    pNewScript->RegisterSelf();

    pNewScript = new Script;
    pNewScript->Name = "npc_lady_nazjar_geyser";
    pNewScript->GetAI = &GetAI_npc_lady_nazjar_geyser;
    pNewScript->RegisterSelf();
}
