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
#include "WorldPacket.h"
#include "WorldSession.h"
#include "ObjectMgr.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "Log.h"
#include "Opcodes.h"
#include "Spell.h"
#include "CreatureAI.h"
#include "Util.h"
#include "Pet.h"
#include "TemporarySummon.h"

void WorldSession::HandlePetAction(WorldPacket& recv_data)
{
    ObjectGuid petGuid;
    uint32 data;
    ObjectGuid targetGuid;
    float x, y, z;
    recv_data >> petGuid;
    recv_data >> data;
    recv_data >> targetGuid;
    recv_data >> x >> y >> z;

    uint32 spellid = UNIT_ACTION_BUTTON_ACTION(data);
    uint8 flag = UNIT_ACTION_BUTTON_TYPE(data);             // delete = 0x07 CastSpell = C1

    DETAIL_LOG("HandlePetAction: %s flag is %u, spellid is %u, target %s.", petGuid.GetString().c_str(), uint32(flag), spellid, targetGuid.GetString().c_str());

    // used also for charmed creature/player
    Unit* pet = _player->GetMap()->GetUnit(petGuid);
    if (!pet)
    {
        ERROR_LOG("HandlePetAction: %s not exist.", petGuid.GetString().c_str());
        return;
    }

    // copyguids system (for treants or spirit wolves)
    ObjectGuid copyguid = ObjectGuid(pet->GetObjectGuid().GetHigh(), pet->GetObjectGuid().GetEntry()-1, pet->GetGUIDLow()-1);
    Unit* pet2= ObjectAccessor::GetUnit(*_player, copyguid);
    if (pet2 && pet2->GetEntry() == pet->GetEntry() && pet2->GetOwnerGuid() == pet->GetOwnerGuid())
    {
        WorldPacket *virtualpacket = new WorldPacket(recv_data.GetOpcode());
        *virtualpacket << copyguid;
        *virtualpacket << data;
        *virtualpacket << targetGuid;
        *virtualpacket << x << y << z;
        HandlePetAction(*virtualpacket);
        delete virtualpacket;
    }

    if (GetPlayer()->GetObjectGuid() != pet->GetCharmerOrOwnerGuid())
    {
        ERROR_LOG("HandlePetAction: %s isn't controlled by %s.", petGuid.GetString().c_str(), GetPlayer()->GetGuidStr().c_str());
        return;
    }

    CharmInfo* charmInfo = pet->GetCharmInfo();
    if (!charmInfo)
    {
        ERROR_LOG("WorldSession::HandlePetAction: object (GUID: %u TypeId: %u) is considered pet-like but doesn't have a charminfo!", pet->GetGUIDLow(), pet->GetTypeId());
        return;
    }

    if (!pet->isAlive())
    {
        // Heart of the Phoenix hack
        if (flag == ACT_ENABLED || flag == ACT_DISABLED)
            if (SpellEntry const *spellInfo = sSpellStore.LookupEntry(spellid))
                if (spellInfo->Id == 55709 && pet->GetTypeId() == TYPEID_UNIT && ((Creature*)pet)->IsPet()
                    && !((Creature*)pet)->HasSpellCooldown(spellid))
                {
                    pet->CastSpell(pet, spellid, true);
                    ((Creature*)pet)->AddCreatureSpellCooldown(spellid);
                }
        return;
    }

    if (pet->GetTypeId() == TYPEID_PLAYER)
    {
        // controller player can only do melee attack
        if (!(flag == ACT_COMMAND && spellid == COMMAND_ATTACK))
            return;
    }
    else if (((Creature*)pet)->IsPet())
    {
        // pet can have action bar disabled
        if (charmInfo->HasState(CHARM_STATE_ACTION, ACTIONS_DISABLE))
            return;
    }

    switch (flag)
    {
        case ACT_COMMAND:                                   // 0x07
            switch (spellid)
            {
                case COMMAND_STAY:                          // flat=1792  //STAY
                    pet->InterruptNonMeleeSpells(false);
                    pet->AttackStop();
                    pet->StopMoving();
                    pet->GetMotionMaster()->Clear(false);
                    pet->GetMotionMaster()->MoveIdle();
                    charmInfo->SetState(CHARM_STATE_COMMAND,COMMAND_STAY);
                    pet->SendCharmState();
                    break;
                case COMMAND_FOLLOW:                        // spellid=1792  //FOLLOW
                    pet->InterruptNonMeleeSpells(false);
                    pet->AttackStop();
                    pet->GetMotionMaster()->Clear(false);
                    pet->GetMotionMaster()->MoveFollow(_player, PET_FOLLOW_DIST,PET_FOLLOW_ANGLE);
                    charmInfo->SetState(CHARM_STATE_COMMAND, COMMAND_FOLLOW);
                    pet->SendCharmState();
                    break;
                case COMMAND_ATTACK:                        // spellid=1792  // ATTACK
                {
                    Unit* TargetUnit = _player->GetMap()->GetUnit(targetGuid);
                    if (!TargetUnit)
                        return;

                    // not let attack friendly units.
                    if (GetPlayer()->IsFriendlyTo(TargetUnit))
                        return;
                    // Not let attack between maps
                    if (!TargetUnit->isInAccessablePlaceFor(pet))
                        return;

                    // This is true if pet has no target or has target but targets differs.
                    if (pet->getVictim() != TargetUnit)
                    {
                        if (pet->getVictim())
                            pet->AttackStop();

                        if (pet->hasUnitState(UNIT_STAT_CONTROLLED))
                        {
                            pet->Attack(TargetUnit, true);
                            pet->SendPetAIReaction();
                        }
                        else
                        {
                            pet->GetMotionMaster()->Clear();

                            if (((Creature*)pet)->AI())
                                ((Creature*)pet)->AI()->AttackStart(TargetUnit);

                            // 10% chance to play special pet attack talk, else growl
                            if (((Creature*)pet)->IsPet() && ((Pet*)pet)->getPetType() == SUMMON_PET && pet != TargetUnit && roll_chance_i(10))
                                pet->SendPetTalk((uint32)PET_TALK_ATTACK);
                            else
                            {
                                // 90% chance for pet and 100% chance for charmed creature
                                pet->SendPetAIReaction();
                            }
                        }
                    }
                    break;
                }
                case COMMAND_ABANDON:                       // abandon (hunter pet) or dismiss (summoned pet)
                    pet->InterruptNonMeleeSpells(false);
                    pet->AttackStop();
                    pet->StopMoving();
                    pet->GetMotionMaster()->Clear(false);
                    pet->GetMotionMaster()->MoveIdle();

                    if (((Creature*)pet)->IsPet())
                    {
                        Pet* p = (Pet*)pet;
                        if (p->getPetType() == HUNTER_PET)
                        {
                            if (p->m_actualSlot < PET_SAVE_FIRST_STABLE_SLOT)
                                SendPetSlotUpdated(p->GetObjectGuid().GetEntry(), p->m_actualSlot, -1, 0);

                            p->Unsummon(PET_SAVE_AS_DELETED, _player);
                        }
                        else
                            // dismissing a summoned pet is like killing them (this prevents returning a soulshard...)
                            p->SetDeathState(CORPSE);
                    }
                    else                                    // charmed
                        _player->Uncharm();
                    break;
                default:
                    ERROR_LOG("WORLD: unknown PET flag Action %i and spellid %i.", uint32(flag), spellid);
            }
            break;
        case ACT_REACTION:                                  // 0x6
            switch (spellid)
            {
                case REACT_PASSIVE:                         // passive
                case REACT_DEFENSIVE:                       // recovery
                case REACT_AGGRESSIVE:                      // activete
                    charmInfo->SetState(CHARM_STATE_REACT, ReactStates(spellid));
                    pet->SendCharmState();
                    break;
            }
            break;
        case ACT_DISABLED:                                  // 0x81    spell (disabled), ignore
        case ACT_CASTABLE:                                  // 0x80    spell (disabled), toggle state
        case ACT_PASSIVE:                                   // 0x01
        case ACT_ENABLED:                                   // 0xC1    spell
        case ACT_ACTIVE:                                    // 0xC0    spell
        {
            Unit* unit_target = NULL;
            if (targetGuid)
                unit_target = _player->GetMap()->GetUnit(targetGuid);

            // do not cast unknown spells
            SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellid);
            if (!spellInfo)
            {
                ERROR_LOG("WORLD: unknown PET spell id %i", spellid);
                return;
            }

            if (pet->GetCharmInfo() && pet->GetCharmInfo()->GetGlobalCooldownMgr().HasGlobalCooldown(spellInfo))
                return;

            for (int i = 0; i < MAX_EFFECT_INDEX; ++i)
            {
                SpellEffectEntry const* spellEffect = spellInfo->GetSpellEffect(SpellEffectIndex(i));
                if (!spellEffect)
                    continue;
                if (spellEffect->EffectImplicitTargetA == TARGET_ALL_ENEMY_IN_AREA || spellEffect->EffectImplicitTargetA == TARGET_ALL_ENEMY_IN_AREA_INSTANT || spellEffect->EffectImplicitTargetA == TARGET_ALL_ENEMY_IN_AREA_CHANNELED)
                    return;
            }

            // do not cast not learned spells
            if (!pet->HasSpell(spellid) || IsPassiveSpell(spellInfo))
                return;

            pet->clearUnitState(UNIT_STAT_MOVING);

            Spell* spell = new Spell(pet, spellInfo, false);

            SpellCastResult result = spell->CheckPetCast(unit_target);

            // auto turn to target unless possessed
            if (result == SPELL_FAILED_UNIT_NOT_INFRONT && !pet->HasAuraType(SPELL_AURA_MOD_POSSESS))
            {
                if (unit_target)
                {
                    pet->SetInFront(unit_target);
                    if (unit_target->GetTypeId() == TYPEID_PLAYER)
                        pet->SendCreateUpdateToPlayer( (Player*)unit_target );
                }
                else if (Unit* unit_target2 = spell->m_targets.getUnitTarget())
                {
                    pet->SetInFront(unit_target2);
                    if (unit_target2->GetTypeId() == TYPEID_PLAYER)
                        pet->SendCreateUpdateToPlayer((Player*)unit_target2);
                }
                if (Unit* powner = pet->GetCharmerOrOwner())
                    if (powner->GetTypeId() == TYPEID_PLAYER)
                        pet->SendCreateUpdateToPlayer((Player*)powner);
                result = SPELL_CAST_OK;
            }

            if (result == SPELL_CAST_OK)
            {
                ((Creature*)pet)->AddCreatureSpellCooldown(spellid);

                unit_target = spell->m_targets.getUnitTarget();

                // 10% chance to play special pet attack talk, else growl
                // actually this only seems to happen on special spells, fire shield for imp, torment for voidwalker, but it's stupid to check every spell
                if (((Creature*)pet)->IsPet() && (((Pet*)pet)->getPetType() == SUMMON_PET) && (pet != unit_target) && (urand(0, 100) < 10))
                    pet->SendPetTalk((uint32)PET_TALK_SPECIAL_SPELL);
                else
                {
                    pet->SendPetAIReaction();
                }

                if (unit_target && !GetPlayer()->IsFriendlyTo(unit_target) && !pet->HasAuraType(SPELL_AURA_MOD_POSSESS))
                {
                    // This is true if pet has no target or has target but targets differs.
                    if (pet->getVictim() != unit_target)
                    {
                        if (pet->getVictim())
                            pet->AttackStop();
                        pet->GetMotionMaster()->Clear();
                        if (((Creature*)pet)->AI())
                            ((Creature*)pet)->AI()->AttackStart(unit_target);
                    }
                }

                spell->prepare(&(spell->m_targets));
            }
            else
            {
                if (pet->HasAuraType(SPELL_AURA_MOD_POSSESS))
                    Spell::SendCastResult(GetPlayer(), spellInfo, 0, result);
                else
                {
                    Unit* owner = pet->GetCharmerOrOwner();
                    if (owner && owner->GetTypeId() == TYPEID_PLAYER)
                        Spell::SendCastResult((Player*)owner, spellInfo, 0, result, true);
                }

                if (!((Creature*)pet)->HasSpellCooldown(spellid))
                    GetPlayer()->SendClearCooldown(spellid, pet);

                spell->finish(false);
                delete spell;
            }
            break;
        }
        default:
            ERROR_LOG("WORLD: unknown PET flag Action %i and spellid %i.", uint32(flag), spellid);
            break;
    }
}

void WorldSession::HandlePetStopAttack(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: Received opcode CMSG_PET_STOP_ATTACK");

    ObjectGuid petGuid;
    recv_data >> petGuid;

    Unit* pet = GetPlayer()->GetMap()->GetUnit(petGuid);    // pet or controlled creature/player
    if (!pet)
    {
        sLog.outError("%s doesn't exist.", petGuid.GetString().c_str());
        return;
    }

    if (GetPlayer()->GetObjectGuid() != pet->GetCharmerOrOwnerGuid())
    {
        sLog.outError("HandlePetStopAttack: %s isn't charm/pet of %s.", petGuid.GetString().c_str(), GetPlayer()->GetGuidStr().c_str());
        return;
    }

    if (!pet->isAlive())
        return;

    pet->AttackStop();
}

void WorldSession::HandlePetNameQueryOpcode(WorldPacket& recv_data)
{
    DETAIL_LOG("HandlePetNameQuery. CMSG_PET_NAME_QUERY");

    uint32 petnumber;
    ObjectGuid petguid;

    recv_data >> petnumber;
    recv_data >> petguid;

    SendPetNameQuery(petguid, petnumber);
}

void WorldSession::SendPetNameQuery(ObjectGuid petguid, uint32 petnumber)
{
    Creature* pet = _player->GetMap()->GetAnyTypeCreature(petguid);
    if (!pet || !pet->GetCharmInfo() || pet->GetCharmInfo()->GetPetNumber() != petnumber)
    {
        WorldPacket data(SMSG_PET_NAME_QUERY_RESPONSE, (4+4+7+1));
        data << uint32(petnumber);
        data << "Unknown";
        data << uint32(0);
        data << uint8(0);
        _player->GetSession()->SendPacket(&data);
        return;
    }

    char const* name = pet->GetName();

    // creature pets have localization like other creatures
    if (!pet->GetOwnerGuid().IsPlayer())
    {
        int loc_idx = GetSessionDbLocaleIndex();
        sObjectMgr.GetCreatureLocaleStrings(pet->GetEntry(), loc_idx, &name);
    }

    WorldPacket data(SMSG_PET_NAME_QUERY_RESPONSE, (4+4+strlen(name)+1));
    data << uint32(petnumber);
    data << name;
    data << uint32(pet->GetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP));

    if (pet->IsPet() && ((Pet*)pet)->GetDeclinedNames())
    {
        data << uint8(1);
        for (int i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
            data << ((Pet*)pet)->GetDeclinedNames()->name[i];
    }
    else
        data << uint8(0);

    _player->GetSession()->SendPacket(&data);
}

void WorldSession::HandlePetSetAction(WorldPacket& recv_data)
{
    DETAIL_LOG("HandlePetSetAction. CMSG_PET_SET_ACTION");

    ObjectGuid petGuid;
    uint8  count;

    recv_data >> petGuid;

    Creature* pet = _player->GetMap()->GetAnyTypeCreature(petGuid);

    if (!pet || (pet != _player->GetPet() && pet != _player->GetCharm()))
    {
        ERROR_LOG("HandlePetSetAction: Unknown pet or pet owner.");
        return;
    }

    CharmInfo* charmInfo = pet->GetCharmInfo();
    if (!charmInfo)
    {
        ERROR_LOG("WorldSession::HandlePetSetAction: object (GUID: %u TypeId: %u) is considered pet-like but doesn't have a charminfo!", pet->GetGUIDLow(), pet->GetTypeId());
        return;
    }

    // pet can have action bar disabled
    if (pet->IsPet() && charmInfo->HasState(CHARM_STATE_ACTION, ACTIONS_DISABLE))
        return;

    count = (recv_data.size() == 24) ? 2 : 1;

    uint32 position[2];
    uint32 data[2];
    bool move_command = false;

    for (uint8 i = 0; i < count; ++i)
    {
        recv_data >> position[i];
        recv_data >> data[i];

        uint8 act_state = UNIT_ACTION_BUTTON_TYPE(data[i]);

        // ignore invalid position
        if (position[i] >= MAX_UNIT_ACTION_BAR_INDEX)
            return;

        // in the normal case, command and reaction buttons can only be moved, not removed
        // at moving count ==2, at removing count == 1
        // ignore attempt to remove command|reaction buttons (not possible at normal case)
        if (act_state == ACT_COMMAND || act_state == ACT_REACTION)
        {
            if (count == 1)
                return;

            move_command = true;
        }
    }

    // check swap (at command->spell swap client remove spell first in another packet, so check only command move correctness)
    if (move_command)
    {
        uint8 act_state_0 = UNIT_ACTION_BUTTON_TYPE(data[0]);
        if (act_state_0 == ACT_COMMAND || act_state_0 == ACT_REACTION)
        {
            uint32 spell_id_0 = UNIT_ACTION_BUTTON_ACTION(data[0]);
            UnitActionBarEntry const* actionEntry_1 = charmInfo->GetActionBarEntry(position[1]);
            if (!actionEntry_1 || spell_id_0 != actionEntry_1->GetAction() ||
                act_state_0 != actionEntry_1->GetType())
                return;
        }

        uint8 act_state_1 = UNIT_ACTION_BUTTON_TYPE(data[1]);
        if (act_state_1 == ACT_COMMAND || act_state_1 == ACT_REACTION)
        {
            uint32 spell_id_1 = UNIT_ACTION_BUTTON_ACTION(data[1]);
            UnitActionBarEntry const* actionEntry_0 = charmInfo->GetActionBarEntry(position[0]);
            if (!actionEntry_0 || spell_id_1 != actionEntry_0->GetAction() ||
                act_state_1 != actionEntry_0->GetType())
                return;
        }
    }

    for (uint8 i = 0; i < count; ++i)
    {
        uint32 spell_id = UNIT_ACTION_BUTTON_ACTION(data[i]);
        uint8 act_state = UNIT_ACTION_BUTTON_TYPE(data[i]);

        DETAIL_LOG( "Player %s has changed pet spell action. Position: %u, Spell: %u, State: 0x%X", _player->GetName(), position[i], spell_id, uint32(act_state));

        // if it's act for spell (en/disable/cast) and there is a spell given (0 = remove spell) which pet doesn't know, don't add
        if (!((act_state == ACT_ENABLED || act_state == ACT_DISABLED || act_state == ACT_PASSIVE) && spell_id && !pet->HasSpell(spell_id)))
        {
            // sign for autocast
            if (act_state == ACT_ENABLED && spell_id)
            {
                if (pet->isCharmed())
                    charmInfo->ToggleCreatureAutocast(spell_id, true);
                else
                    ((Pet*)pet)->ToggleAutocast(spell_id, true);
            }
            // sign for no/turn off autocast
            else if (act_state == ACT_DISABLED && spell_id)
            {
                if (pet->isCharmed())
                    charmInfo->ToggleCreatureAutocast(spell_id, false);
                else
                    ((Pet*)pet)->ToggleAutocast(spell_id, false);
            }

            charmInfo->SetActionBar(position[i], spell_id, ActiveStates(act_state));
        }
    }
}

void WorldSession::HandlePetRename(WorldPacket& recv_data)
{
    DETAIL_LOG("HandlePetRename. CMSG_PET_RENAME");

    ObjectGuid petGuid;
    uint8 isdeclined;

    std::string name;
    DeclinedName declinedname;

    recv_data >> petGuid;
    recv_data >> name;
    recv_data >> isdeclined;

    Pet* pet = _player->GetMap()->GetPet(petGuid);
                                                            // check it!
    if (!pet || pet->getPetType() != HUNTER_PET ||
        !pet->HasByteFlag(UNIT_FIELD_BYTES_2, 2, UNIT_CAN_BE_RENAMED) ||
        pet->GetOwnerGuid() != _player->GetObjectGuid() || !pet->GetCharmInfo())
        return;

    PetNameInvalidReason res = ObjectMgr::CheckPetName(name);
    if (res != PET_NAME_SUCCESS)
    {
        SendPetNameInvalid(res, name, NULL);
        return;
    }

    if (sObjectMgr.IsReservedName(name))
    {
        SendPetNameInvalid(PET_NAME_RESERVED, name, NULL);
        return;
    }

    pet->SetName(name);

    if (_player->GetGroup())
        _player->SetGroupUpdateFlag(GROUP_UPDATE_FLAG_PET_NAME);

    pet->RemoveByteFlag(UNIT_FIELD_BYTES_2, 2, UNIT_CAN_BE_RENAMED);

    if (isdeclined)
    {
        for (int i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
        {
            recv_data >> declinedname.name[i];
        }

        std::wstring wname;
        Utf8toWStr(name, wname);
        if (!ObjectMgr::CheckDeclinedNames(GetMainPartOfName(wname, 0), declinedname))
        {
            SendPetNameInvalid(PET_NAME_DECLENSION_DOESNT_MATCH_BASE_NAME, name, &declinedname);
            return;
        }
    }

    CharacterDatabase.BeginTransaction();
    if (isdeclined)
    {
        for (int i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
            CharacterDatabase.escape_string(declinedname.name[i]);
        CharacterDatabase.PExecute("DELETE FROM character_pet_declinedname WHERE owner = '%u' AND id = '%u'", _player->GetGUIDLow(), pet->GetCharmInfo()->GetPetNumber());
        CharacterDatabase.PExecute("INSERT INTO character_pet_declinedname (id, owner, genitive, dative, accusative, instrumental, prepositional) VALUES ('%u','%u','%s','%s','%s','%s','%s')",
            pet->GetCharmInfo()->GetPetNumber(), _player->GetGUIDLow(), declinedname.name[0].c_str(), declinedname.name[1].c_str(), declinedname.name[2].c_str(), declinedname.name[3].c_str(), declinedname.name[4].c_str());
    }

    CharacterDatabase.escape_string(name);
    CharacterDatabase.PExecute("UPDATE character_pet SET name = '%s', renamed = '1' WHERE owner = '%u' AND id = '%u'", name.c_str(), _player->GetGUIDLow(), pet->GetCharmInfo()->GetPetNumber());
    CharacterDatabase.CommitTransaction();

    pet->SetUInt32Value(UNIT_FIELD_PET_NAME_TIMESTAMP, uint32(time(NULL)));
}

void WorldSession::HandlePetAbandon(WorldPacket& recv_data)
{
    ObjectGuid guid;
    recv_data >> guid;                                      // pet guid

    DETAIL_LOG("HandlePetAbandon. CMSG_PET_ABANDON pet guid is %s", guid.GetString().c_str());

    if (!_player->IsInWorld())
        return;

    // pet/charmed
    if (Creature* pet = _player->GetMap()->GetAnyTypeCreature(guid))
    {
        if (pet->IsPet())
        {
            Pet* p = (Pet*)pet;
            if (p->m_actualSlot < PET_SAVE_FIRST_STABLE_SLOT)
                SendPetSlotUpdated(p->GetObjectGuid().GetEntry(), p->m_actualSlot, -1, 0);

            p->Unsummon(PET_SAVE_AS_DELETED, _player);
        }
        else if (pet->GetObjectGuid() == _player->GetCharmGuid())
        {
            _player->Uncharm();
        }
    }
}

void WorldSession::HandlePetUnlearnOpcode(WorldPacket& recvPacket)
{
    DETAIL_LOG("CMSG_PET_UNLEARN");

    ObjectGuid guid;
    recvPacket >> guid;                 // Pet guid

    Pet* pet = _player->GetPet();

    if (!pet || guid != pet->GetObjectGuid())
    {
        ERROR_LOG("HandlePetUnlearnOpcode. %s isn't pet of %s .", guid.GetString().c_str(), GetPlayer()->GetGuidStr().c_str());
        return;
    }

    if (pet->getPetType() != HUNTER_PET || pet->m_usedTalentCount == 0)
        return;

    CharmInfo* charmInfo = pet->GetCharmInfo();
    if (!charmInfo)
    {
        ERROR_LOG("WorldSession::HandlePetUnlearnOpcode: %s is considered pet-like but doesn't have a charminfo!", pet->GetGuidStr().c_str());
        return;
    }
    pet->resetTalents();
    _player->SendTalentsInfoData(true);
}

void WorldSession::HandlePetSpellAutocastOpcode(WorldPacket& recvPacket)
{
    DETAIL_LOG("CMSG_PET_SPELL_AUTOCAST");

    ObjectGuid guid;
    uint32 spellid;
    uint8  state;                                           // 1 for on, 0 for off
    recvPacket >> guid >> spellid >> state;

    Creature* pet = _player->GetMap()->GetAnyTypeCreature(guid);
    if (!pet || (guid != _player->GetPetGuid() && guid != _player->GetCharmGuid()))
    {
        ERROR_LOG("HandlePetSpellAutocastOpcode. %s isn't pet of %s .", guid.GetString().c_str(), GetPlayer()->GetGuidStr().c_str());
        return;
    }

    // do not add not learned spells/ passive spells
    if (!pet->HasSpell(spellid) || IsPassiveSpell(spellid))
        return;

    CharmInfo* charmInfo = pet->GetCharmInfo();
    if (!charmInfo)
    {
        ERROR_LOG("WorldSession::HandlePetSpellAutocastOpcod: %s is considered pet-like but doesn't have a charminfo!", guid.GetString().c_str());
        return;
    }

    if (pet->isCharmed())
                                                            // state can be used as boolean
        pet->GetCharmInfo()->ToggleCreatureAutocast(spellid, state);
    else
        ((Pet*)pet)->ToggleAutocast(spellid, state);

    charmInfo->SetSpellAutocast(spellid, state);
}

void WorldSession::HandlePetCastSpellOpcode(WorldPacket& recvPacket)
{
    DETAIL_LOG("WORLD: CMSG_PET_CAST_SPELL");

    ObjectGuid guid;
    uint32 spellid;
    uint8  cast_count;
    uint8  cast_flags;                                      // flags (if 0x02 - some additional data are received)

    recvPacket >> guid >> cast_count >> spellid >> cast_flags;

    DEBUG_LOG("WORLD: CMSG_PET_CAST_SPELL, %s, cast_count: %u, spellid %u, cast_flags %u", guid.GetString().c_str(), cast_count, spellid, cast_flags);

    Creature* pet = _player->GetMap()->GetAnyTypeCreature(guid);

    if (!pet || (guid != _player->GetPetGuid() && guid != _player->GetCharmGuid()))
    {
        ERROR_LOG("HandlePetCastSpellOpcode: %s isn't pet of %s .", guid.GetString().c_str(), GetPlayer()->GetGuidStr().c_str());
        recvPacket.rfinish();
        return;
    }

    SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellid);
    if (!spellInfo)
    {
        ERROR_LOG("WORLD: unknown PET spell id %i", spellid);
        recvPacket.rfinish();
        return;
    }

    if (pet->GetCharmInfo() && pet->GetCharmInfo()->GetGlobalCooldownMgr().HasGlobalCooldown(spellInfo))
    {
        recvPacket.rfinish();
        return;
    }

    SpellCastTargets targets;
    recvPacket >> targets.ReadForCaster(pet);
    targets.ReadAdditionalData(recvPacket, cast_flags);

    bool triggered = false;
    SpellEntry const * triggeredBy = NULL;
    Aura* triggeredByAura = pet->GetTriggeredByClientAura(spellid);
    if (triggeredByAura)
    {
        triggered = true;
        triggeredBy = triggeredByAura->GetSpellProto();
        cast_count = 0;
    }

    // do not cast not learned spells
    if (!pet->HasSpell(spellid) && !triggered || IsPassiveSpell(spellInfo))
        return;

    pet->clearUnitState(UNIT_STAT_MOVING);

    Spell* spell = new Spell(pet, spellInfo, triggered, pet->GetObjectGuid(), triggeredBy);
    spell->m_cast_count = cast_count;                       // probably pending spell cast
    spell->m_targets = targets;

    SpellCastResult result = spell->CheckPetCast(NULL);
    if (result == SPELL_CAST_OK)
    {
        pet->AddCreatureSpellCooldown(spellid);
        if (pet->IsPet())
        {
            // 10% chance to play special pet attack talk, else growl
            // actually this only seems to happen on special spells, fire shield for imp, torment for voidwalker, but it's stupid to check every spell
            if (((Pet*)pet)->getPetType() == SUMMON_PET && (urand(0, 100) < 10))
                pet->SendPetTalk((uint32)PET_TALK_SPECIAL_SPELL);
            else
                pet->SendPetAIReaction();
        }

        spell->prepare(&(spell->m_targets), triggeredByAura);
    }
    else
    {
        Unit* owner = pet->GetCharmerOrOwner();
        if (owner && owner->GetTypeId() == TYPEID_PLAYER)
            Spell::SendCastResult((Player*)owner, spellInfo, 0, result, true);

        if (!pet->HasSpellCooldown(spellid))
            GetPlayer()->SendClearCooldown(spellid, pet);

        spell->finish(false);
        delete spell;
    }
}

void WorldSession::SendPetNameInvalid(uint32 error, const std::string& name, DeclinedName* declinedName)
{
    WorldPacket data(SMSG_PET_NAME_INVALID, 4 + name.size() + 1 + 1);
    data << uint32(error);
    data << name;
    if (declinedName)
    {
        data << uint8(1);
        for (uint32 i = 0; i < MAX_DECLINED_NAME_CASES; ++i)
            data << declinedName->name[i];
    }
    else
        data << uint8(0);
    SendPacket(&data);
}

void WorldSession::HandlePetLearnTalent(WorldPacket& recv_data)
{
    DEBUG_LOG("WORLD: CMSG_PET_LEARN_TALENT");

    ObjectGuid guid;
    uint32 talent_id, requested_rank;
    recv_data >> guid >> talent_id >> requested_rank;

    _player->LearnPetTalent(guid, talent_id, requested_rank);
    _player->SendTalentsInfoData(true);
}

void WorldSession::HandleLearnPreviewTalentsPet(WorldPacket& recv_data)
{
    DEBUG_LOG("CMSG_LEARN_PREVIEW_TALENTS_PET");

    ObjectGuid guid;
    recv_data >> guid;

    uint32 talentsCount;
    recv_data >> talentsCount;

    uint32 talentId, talentRank;

    for (uint32 i = 0; i < talentsCount; ++i)
    {
        recv_data >> talentId >> talentRank;

        _player->LearnPetTalent(guid, talentId, talentRank);
    }

    _player->SendTalentsInfoData(true);
}

void WorldSession::SendPetSlotUpdated(uint32 petNumber, int32 srcSlot, int32 dstSlot, int32 unk)
{
    WorldPacket data(SMSG_PET_SLOT_UPDATED, 16);
    data << int32(dstSlot);     // dest slot
    data << int32(srcSlot);     // src slot?
    data << uint32(petNumber);  // pet number
    data << uint32(0);          // unk

    SendPacket(&data);
}

void WorldSession::HandleDismissCritter(WorldPacket& recvData)
{
    ObjectGuid guid;
    recvData >> guid;

    DEBUG_LOG("WORLD: Received CMSG_DISMISS_CRITTER for %s", guid.GetString().c_str());

    Unit* pet = _player->GetMap()->GetAnyTypeCreature(guid);
    if (!pet)
    {
        DEBUG_LOG("Vanitypet %s does not exist - Player %s (guid %u, account: %u) attempted to dismiss it (possibly lagged out)",
            guid.GetString().c_str(), GetPlayer()->GetName(), GetPlayer()->GetGUIDLow(), GetAccountId());
        return;
    }

    if (_player->GetCritterGuid() == guid)
    {
        if (pet->GetTypeId() == TYPEID_UNIT && ((Creature*)pet)->IsTemporarySummon())
            ((TemporarySummon*)pet)->UnSummon();
    }
}
