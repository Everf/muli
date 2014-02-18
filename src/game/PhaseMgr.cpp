#include "PhaseMgr.h"
#include "Chat.h"
#include "Language.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "SpellAuras.h"

//////////////////////////////////////////////////////////////////
// Updating

PhaseMgr::PhaseMgr(Player* _player) : player(_player), _UpdateFlags(0)
{
    phaseData = new PhaseData(_player);
    _PhaseDefinitionStore = sObjectMgr.GetPhaseDefinitionStore();
    _SpellPhaseStore = sObjectMgr.GetSpellPhaseStore();
}

void PhaseMgr::Update()
{
    if (IsUpdateInProgress())
        return;

    if (_UpdateFlags & PHASE_UPDATE_FLAG_CLIENTSIDE_CHANGED)
        phaseData->SendPhaseshiftToPlayer();

    if (_UpdateFlags & PHASE_UPDATE_FLAG_SERVERSIDE_CHANGED)
        phaseData->SendPhaseMaskToPlayer();

    _UpdateFlags = 0;
}

void PhaseMgr::RemoveUpdateFlag(PhaseUpdateFlag updateFlag)
{
    _UpdateFlags &= ~updateFlag;

    if (updateFlag == PHASE_UPDATE_FLAG_ZONE_UPDATE)
    {
        // Update zone changes
        if (phaseData->HasActiveDefinitions())
        {
            phaseData->ResetDefinitions();
            _UpdateFlags |= (PHASE_UPDATE_FLAG_CLIENTSIDE_CHANGED | PHASE_UPDATE_FLAG_SERVERSIDE_CHANGED);
        }

        if (_PhaseDefinitionStore->find(player->GetZoneId()) != _PhaseDefinitionStore->end())
            Recalculate();
    }

    Update();
}

/////////////////////////////////////////////////////////////////
// Notifier

void PhaseMgr::NotifyConditionChanged(PhaseUpdateData const& updateData)
{
    if (NeedsPhaseUpdateWithData(updateData))
    {
        Recalculate();
        Update();
    }
}

//////////////////////////////////////////////////////////////////
// Phasing Definitions

void PhaseMgr::Recalculate()
{
    if (phaseData->HasActiveDefinitions())
    {
        phaseData->ResetDefinitions();
        _UpdateFlags |= (PHASE_UPDATE_FLAG_CLIENTSIDE_CHANGED | PHASE_UPDATE_FLAG_SERVERSIDE_CHANGED);
    }

    PhaseDefinitionStore::const_iterator itr = _PhaseDefinitionStore->find(player->GetZoneId());
    if (itr != _PhaseDefinitionStore->end())
        for (PhaseDefinitionContainer::const_iterator phase = itr->second.begin(); phase != itr->second.end(); ++phase)
            if (CheckDefinition(*phase))
            {
                phaseData->AddPhaseDefinition(*phase);

                if ((*phase)->phasemask)
                    _UpdateFlags |= PHASE_UPDATE_FLAG_SERVERSIDE_CHANGED;

                if ((*phase)->phaseId || (*phase)->terrainswapmap)
                    _UpdateFlags |= PHASE_UPDATE_FLAG_CLIENTSIDE_CHANGED;

                if ((*phase)->IsLastDefinition())
                    break;
            }
}

inline bool PhaseMgr::CheckDefinition(PhaseDefinition const* phaseDefinition)
{
    return !phaseDefinition->conditionId || sObjectMgr.IsPlayerMeetToCondition(phaseDefinition->conditionId, player, player->GetMap(), player, CONDITION_FROM_PHASEMGR);
}

bool PhaseMgr::NeedsPhaseUpdateWithData(PhaseUpdateData const updateData)
{
    PhaseDefinitionStore::const_iterator itr = _PhaseDefinitionStore->find(player->GetZoneId());
    if (itr != _PhaseDefinitionStore->end())
    {
        for (PhaseDefinitionContainer::const_iterator phase = itr->second.begin(); phase != itr->second.end(); ++phase)
        {
            std::vector<PlayerCondition const*> conditions;
            sObjectMgr.GetConditions((*phase)->conditionId, conditions);

            for (std::vector<PlayerCondition const*>::const_iterator itr = conditions.begin(); itr != conditions.end(); ++itr)
                if (updateData.IsConditionRelated(*itr))
                    return true;
        }
    }
    return false;
}

//////////////////////////////////////////////////////////////////
// Auras

void PhaseMgr::RegisterPhasingAuraEffect(Aura const* auraEffect)
{
    PhaseInfo* phaseInfo = new PhaseInfo();

    if (auraEffect->GetMiscValue())
    {
        _UpdateFlags |= PHASE_UPDATE_FLAG_SERVERSIDE_CHANGED;
        phaseInfo->phasemask = auraEffect->GetMiscValue();
    }
    else
    {
        SpellPhaseStore::const_iterator itr = _SpellPhaseStore->find(auraEffect->GetHolder()->GetId());
        if (itr != _SpellPhaseStore->end())
        {
            if (itr->second->phasemask)
            {
                _UpdateFlags |= PHASE_UPDATE_FLAG_SERVERSIDE_CHANGED;
                phaseInfo->phasemask = itr->second->phasemask;
            }

            if (itr->second->terrainswapmap)
                phaseInfo->terrainswapmap = itr->second->terrainswapmap;
        }
    }

    // this is not Phase.dbc id...
    //phaseInfo->phaseId = auraEffect->GetMiscBValue();

    if (phaseInfo->NeedsClientSideUpdate())
        _UpdateFlags |= PHASE_UPDATE_FLAG_CLIENTSIDE_CHANGED;

    phaseData->AddAuraInfo(auraEffect->GetHolder()->GetId(), phaseInfo);

    Update();
}

void PhaseMgr::UnRegisterPhasingAuraEffect(Aura const* auraEffect)
{
    _UpdateFlags |= phaseData->RemoveAuraInfo(auraEffect->GetHolder()->GetId());

    Update();
}

//////////////////////////////////////////////////////////////////
// Commands

void PhaseMgr::SendDebugReportToPlayer(Player* const debugger)
{
    ChatHandler(debugger).PSendSysMessage(LANG_PHASING_REPORT_STATUS, player->GetName(), player->GetZoneId(), player->getLevel(), player->GetTeam(), _UpdateFlags);

    PhaseDefinitionStore::const_iterator itr = _PhaseDefinitionStore->find(player->GetZoneId());
    if (itr == _PhaseDefinitionStore->end())
        ChatHandler(debugger).PSendSysMessage(LANG_PHASING_NO_DEFINITIONS, player->GetZoneId());
    else
    {
        for (PhaseDefinitionContainer::const_iterator phase = itr->second.begin(); phase != itr->second.end(); ++phase)
        {
            if (CheckDefinition(*phase))
                ChatHandler(debugger->GetSession()).PSendSysMessage(LANG_PHASING_SUCCESS, (*phase)->entry, (*phase)->IsNegatingPhasemask() ? "negated Phase" : "Phase", (*phase)->phasemask);
            else
                ChatHandler(debugger).PSendSysMessage(LANG_PHASING_FAILED, (*phase)->phasemask, (*phase)->entry, (*phase)->zoneId);

            if ((*phase)->IsLastDefinition())
            {
                ChatHandler(debugger).PSendSysMessage(LANG_PHASING_LAST_PHASE, (*phase)->phasemask, (*phase)->entry, (*phase)->zoneId);
                break;
            }
        }
    }

    ChatHandler(debugger).PSendSysMessage(LANG_PHASING_LIST, phaseData->_PhasemaskThroughDefinitions, phaseData->_PhasemaskThroughAuras, phaseData->_CustomPhasemask);

    ChatHandler(debugger).PSendSysMessage(LANG_PHASING_PHASEMASK, phaseData->GetPhaseMaskForSpawn(), player->GetPhaseMask());
}

void PhaseMgr::SetCustomPhase(uint32 const phaseMask)
{
    phaseData->_CustomPhasemask = phaseMask;

    // phase auras normally not expected at BG but anyway better check
    if (BattleGround *bg = player->GetBattleGround())
        bg->EventPlayerDroppedFlag(player);

    _UpdateFlags |= PHASE_UPDATE_FLAG_SERVERSIDE_CHANGED;

    Update();
}

//////////////////////////////////////////////////////////////////
// Phase Data

uint32 PhaseData::GetCurrentPhasemask() const
{
    if (player->isGameMaster())
        return uint32(PHASEMASK_ANYWHERE);

    if (_CustomPhasemask)
        return _CustomPhasemask;

    return GetPhaseMaskForSpawn();
}

inline uint32 PhaseData::GetPhaseMaskForSpawn() const
{
    uint32 const phase = (_PhasemaskThroughDefinitions | _PhasemaskThroughAuras);
    return (phase ? phase : PHASEMASK_NORMAL);
}

void PhaseData::SendPhaseMaskToPlayer()
{
    // Server side update
    uint32 const phasemask = GetCurrentPhasemask();
    if (player->GetPhaseMask() == phasemask)
        return;

    player->SetPhaseMask(phasemask, false);

    if (player->IsInWorld())
        player->UpdateVisibilityAndView();
}

void PhaseData::SendPhaseshiftToPlayer()
{
    // Client side update
    std::set<uint32> phaseIds;
    std::set<uint32> terrainswaps;

    for (PhaseInfoContainer::const_iterator itr = spellPhaseInfo.begin(); itr != spellPhaseInfo.end(); ++itr)
    {
        if (itr->second->terrainswapmap)
            terrainswaps.insert(itr->second->terrainswapmap);

        if (itr->second->phaseId)
            phaseIds.insert(itr->second->phaseId);
    }

    // Phase Definitions
    for (std::list<PhaseDefinition const*>::const_iterator itr = activePhaseDefinitions.begin(); itr != activePhaseDefinitions.end(); ++itr)
    {
        if ((*itr)->phaseId)
            phaseIds.insert((*itr)->phaseId);

        if ((*itr)->terrainswapmap)
            terrainswaps.insert((*itr)->terrainswapmap);
    }

    player->GetSession()->SendSetPhaseShift(phaseIds, terrainswaps);
}

void PhaseData::AddPhaseDefinition(PhaseDefinition const* phaseDefinition)
{
    if (phaseDefinition->IsOverwritingExistingPhases())
    {
        activePhaseDefinitions.clear();
        _PhasemaskThroughDefinitions = phaseDefinition->phasemask;
    }
    else
    {
        if (phaseDefinition->IsNegatingPhasemask())
            _PhasemaskThroughDefinitions &= ~phaseDefinition->phasemask;
        else
            _PhasemaskThroughDefinitions |= phaseDefinition->phasemask;
    }

    activePhaseDefinitions.push_back(phaseDefinition);
}

void PhaseData::AddAuraInfo(uint32 const spellId, PhaseInfo* phaseInfo)
{
    if (phaseInfo->phasemask)
        _PhasemaskThroughAuras |= phaseInfo->phasemask;

    spellPhaseInfo[spellId] = phaseInfo;
}

uint32 PhaseData::RemoveAuraInfo(uint32 const spellId)
{
    PhaseInfoContainer::iterator rAura = spellPhaseInfo.find(spellId);
    if (rAura != spellPhaseInfo.end())
    {
        uint32 updateflag = 0;

        if (rAura->second->NeedsClientSideUpdate())
            updateflag |= PHASE_UPDATE_FLAG_CLIENTSIDE_CHANGED;

        if (rAura->second->NeedsServerSideUpdate())
        {
            _PhasemaskThroughAuras = 0;

            updateflag |= PHASE_UPDATE_FLAG_SERVERSIDE_CHANGED;

            spellPhaseInfo.erase(rAura);

            for (PhaseInfoContainer::const_iterator itr = spellPhaseInfo.begin(); itr != spellPhaseInfo.end(); ++itr)
                _PhasemaskThroughAuras |= itr->second->phasemask;
        }

        return updateflag;
    }
    else
        return 0;
}

//////////////////////////////////////////////////////////////////
// Phase Update Data

void PhaseUpdateData::AddQuestUpdate(uint32 const questId)
{
    AddConditionType(CONDITION_QUESTREWARDED);
    AddConditionType(CONDITION_QUESTTAKEN);
    //AddConditionType(CONDITION_QUESTCOMPLETED);
    AddConditionType(CONDITION_QUEST_NONE);

    _questId = questId;
}

bool PhaseUpdateData::IsConditionRelated(PlayerCondition const* condition) const
{
    switch (condition->m_condition)
    {
        case CONDITION_QUESTREWARDED:
        case CONDITION_QUESTTAKEN:
        //case CONDITION_QUESTCOMPLETED:
        case CONDITION_QUEST_NONE:
            return condition->m_value1 == _questId && ((1 << condition->m_condition) & _conditionTypeFlags);
        default:
            return (1 << condition->m_condition) & _conditionTypeFlags;
    }
}

bool PhaseMgr::IsConditionTypeSupported(ConditionType const conditionType)
{
    switch (conditionType)
    {
        case CONDITION_QUESTREWARDED:
        case CONDITION_QUESTTAKEN:
        //case CONDITION_QUESTCOMPLETED:
        case CONDITION_QUEST_NONE:
        case CONDITION_TEAM:
        case CONDITION_RACE_CLASS:
        //case CONDITION_INSTANCE_INFO:
        case CONDITION_LEVEL:
            return true;
        default:
            return false;
    }
}