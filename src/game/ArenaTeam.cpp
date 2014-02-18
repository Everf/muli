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

#include "WorldPacket.h"
#include "ObjectMgr.h"
#include "ObjectGuid.h"
#include "ArenaTeam.h"
#include "World.h"
#include "Player.h"

void ArenaTeamMember::ModifyPersonalRating(Player* plr, int32 mod, uint32 slot)
{
    if (int32(personal_rating) + mod < 0)
        personal_rating = 0;
    else
        personal_rating += mod;
    if(plr)
        plr->SetArenaTeamInfoField(slot, ARENA_TEAM_PERSONAL_RATING, personal_rating);
}

void ArenaTeamMember::ModifyMatchmakerRating(int32 mod, uint32 /*slot*/)
{
    if (MatchMakerRating + mod < 0)
        MatchMakerRating = 0;
    else
        MatchMakerRating += mod;
}

ArenaTeam::ArenaTeam()
{
    m_TeamId              = 0;
    m_Type                = ARENA_TYPE_NONE;
    m_BackgroundColor     = 0;                              // background
    m_EmblemStyle         = 0;                              // icon
    m_EmblemColor         = 0;                              // icon color
    m_BorderStyle         = 0;                              // border
    m_BorderColor         = 0;                              // border color
    m_stats.games_week    = 0;
    m_stats.games_season  = 0;
    m_stats.rank          = 0;

    int32 conf_value = sWorld.getConfig(CONFIG_INT32_ARENA_STARTRATING);
    if (conf_value < 0)                                     // -1 = select by season id
    {
        if (sWorld.getConfig(CONFIG_UINT32_ARENA_SEASON_ID) >= 6)
            m_stats.rating    = 0;
        else
            m_stats.rating    = 1500;
    }
    else
        m_stats.rating = uint32(conf_value);

    m_stats.wins_week     = 0;
    m_stats.wins_season   = 0;
}

ArenaTeam::~ArenaTeam()
{
}

bool ArenaTeam::Create(ObjectGuid captainGuid, ArenaType type, std::string arenaTeamName)
{
    if (!IsArenaTypeValid(type))
        return false;
    if (!sObjectMgr.GetPlayer(captainGuid))                 // player not exist
        return false;
    if (sObjectMgr.GetArenaTeamByName(arenaTeamName))       // arena team with this name already exist
        return false;

    DEBUG_LOG("GUILD: creating arena team %s to leader: %s", arenaTeamName.c_str(), captainGuid.GetString().c_str());

    m_CaptainGuid = captainGuid;
    m_Name = arenaTeamName;
    m_Type = type;

    m_TeamId = sObjectMgr.GenerateArenaTeamId();

    // ArenaTeamName already assigned to ArenaTeam::name, use it to encode string for DB
    CharacterDatabase.escape_string(arenaTeamName);

    CharacterDatabase.BeginTransaction();
    // CharacterDatabase.PExecute("DELETE FROM arena_team WHERE arenateamid='%u'", m_TeamId); - MAX(arenateam)+1 not exist
    CharacterDatabase.PExecute("DELETE FROM arena_team_member WHERE arenateamid='%u'", m_TeamId);
    CharacterDatabase.PExecute("INSERT INTO arena_team (arenateamid,name,captainguid,type,BackgroundColor,EmblemStyle,EmblemColor,BorderStyle,BorderColor) "
        "VALUES('%u','%s','%u','%u','%u','%u','%u','%u','%u')",
        m_TeamId, arenaTeamName.c_str(), m_CaptainGuid.GetCounter(), m_Type, m_BackgroundColor, m_EmblemStyle, m_EmblemColor, m_BorderStyle, m_BorderColor);
    CharacterDatabase.PExecute("INSERT INTO arena_team_stats (arenateamid, rating, games_week, wins_week, games_season, wins_season, rank) VALUES "
        "('%u', '%u', '%u', '%u', '%u', '%u', '%u')", m_TeamId, m_stats.rating, m_stats.games_week, m_stats.wins_week, m_stats.games_season, m_stats.wins_season, m_stats.rank);

    CharacterDatabase.CommitTransaction();

    AddMember(m_CaptainGuid);
    return true;
}

bool ArenaTeam::AddMember(ObjectGuid playerGuid)
{
    std::string plName;
    uint8 plClass;

    // arena team is full (can't have more than type * 2 players!)
    if (GetMembersSize() >= GetMaxMembersSize())
        return false;

    Player *pl = sObjectMgr.GetPlayer(playerGuid);
    if (pl)
    {
        if (pl->GetArenaTeamId(GetSlot()))
        {
            ERROR_LOG("Arena::AddMember() : player already in this sized team");
            return false;
        }

        plClass = pl->getClass();
        plName = pl->GetName();
    }
    else
    {
        //                                                     0     1
        QueryResult *result = CharacterDatabase.PQuery("SELECT name, class FROM characters WHERE guid='%u'", playerGuid.GetCounter());
        if (!result)
            return false;

        plName = (*result)[0].GetCppString();
        plClass = (*result)[1].GetUInt8();
        delete result;

        // check if player already in arenateam of that size
        if (Player::GetArenaTeamIdFromDB(playerGuid, GetType()) != 0)
        {
            ERROR_LOG("Arena::AddMember() : player %s already in this sized team", playerGuid.GetString().c_str());
            return false;
        }
    }

    ArenaTeamMember newmember;
    newmember.name              = plName;
    newmember.guid              = playerGuid;
    newmember.Class             = plClass;
    newmember.games_season      = 0;
    newmember.games_week        = 0;
    newmember.wins_season       = 0;
    newmember.wins_week         = 0;
    newmember.MatchMakerRating  = 1500;

    if (QueryResult* result = CharacterDatabase.PQuery("SELECT `mmr` FROM `character_mmr` WHERE `guid` = '%u' AND `slot` = '%u'", playerGuid.GetCounter(), GetSlot()))
        newmember.MatchMakerRating = (*result)[0].GetUInt32();

    int32 conf_value = sWorld.getConfig(CONFIG_INT32_ARENA_STARTPERSONALRATING);
    if (conf_value < 0)                                     // -1 = select by season id
    {
        if (sWorld.getConfig(CONFIG_UINT32_ARENA_SEASON_ID) >= 6)
        {
            if (m_stats.rating < 1000)
                newmember.personal_rating = 0;
            else
                newmember.personal_rating = 1000;
        }
        else
        {
            newmember.personal_rating = 1500;
        }
    }
    else
        newmember.personal_rating = uint32(conf_value);

    m_members.push_back(newmember);

    CharacterDatabase.PExecute("INSERT INTO arena_team_member (arenateamid, guid, personal_rating) VALUES ('%u', '%u', '%u')", m_TeamId, newmember.guid.GetCounter(), newmember.personal_rating );

    if(pl)
    {
        pl->SetInArenaTeam(m_TeamId, GetSlot(), GetType());
        pl->SetArenaTeamIdInvited(0);
        pl->SetArenaTeamInfoField(GetSlot(), ARENA_TEAM_PERSONAL_RATING, newmember.personal_rating);

        // hide promote/remove buttons
        if (m_CaptainGuid != playerGuid)
            pl->SetArenaTeamInfoField(GetSlot(), ARENA_TEAM_MEMBER, 1);
    }
    return true;
}

bool ArenaTeam::LoadArenaTeamFromDB(QueryResult *arenaTeamDataResult)
{
    if(!arenaTeamDataResult)
        return false;

    Field *fields = arenaTeamDataResult->Fetch();

    m_TeamId             = fields[0].GetUInt32();
    m_Name               = fields[1].GetCppString();
    m_CaptainGuid        = ObjectGuid(HIGHGUID_PLAYER, fields[2].GetUInt32());
    m_Type               = ArenaType(fields[3].GetUInt32());

    if (!IsArenaTypeValid(m_Type))
        return false;

    m_BackgroundColor    = fields[4].GetUInt32();
    m_EmblemStyle        = fields[5].GetUInt32();
    m_EmblemColor        = fields[6].GetUInt32();
    m_BorderStyle        = fields[7].GetUInt32();
    m_BorderColor        = fields[8].GetUInt32();
    //load team stats
    m_stats.rating       = fields[9].GetUInt32();
    m_stats.games_week   = fields[10].GetUInt32();
    m_stats.wins_week    = fields[11].GetUInt32();
    m_stats.games_season = fields[12].GetUInt32();
    m_stats.wins_season  = fields[13].GetUInt32();
    m_stats.rank         = fields[14].GetUInt32();

    return true;
}

bool ArenaTeam::LoadMembersFromDB(QueryResult *arenaTeamMembersResult)
{
    if(!arenaTeamMembersResult)
        return false;

    bool captainPresentInTeam = false;

    do
    {
        Field *fields = arenaTeamMembersResult->Fetch();
        //prevent crash if db records are broken, when all members in result are already processed and current team hasn't got any members
        if (!fields)
            break;
        uint32 arenaTeamId        = fields[0].GetUInt32();
        if (arenaTeamId < m_TeamId)
        {
            //there is in table arena_team_member record which doesn't have arenateamid in arena_team table, report error
            sLog.outErrorDb("ArenaTeam %u does not exist but it has record in arena_team_member table, deleting it!", arenaTeamId);
            CharacterDatabase.PExecute("DELETE FROM arena_team_member WHERE arenateamid = '%u'", arenaTeamId);
            continue;
        }

        if (arenaTeamId > m_TeamId)
            //we loaded all members for this arena_team already, break cycle
            break;

        ArenaTeamMember newmember;
        newmember.guid              = ObjectGuid(HIGHGUID_PLAYER, fields[1].GetUInt32());
        newmember.games_week        = fields[2].GetUInt32();
        newmember.wins_week         = fields[3].GetUInt32();
        newmember.games_season      = fields[4].GetUInt32();
        newmember.wins_season       = fields[5].GetUInt32();
        newmember.personal_rating   = fields[6].GetUInt32();
        newmember.name              = fields[7].GetCppString();
        newmember.Class             = fields[8].GetUInt8();
        newmember.MatchMakerRating  = fields[9].GetUInt32() > 0 ? fields[9].GetUInt32() : 1500;

        //check if member exists in characters table
        if (newmember.name.empty())
        {
            sLog.outErrorDb("ArenaTeam %u has member with empty name - probably player %s doesn't exist, deleting him from memberlist!", arenaTeamId, newmember.guid.GetString().c_str());
            DelMember(newmember.guid);
            continue;
        }

        // arena team can't be > 2 * arenatype (2 for 2x2, 3 for 3x3, 5 for 5x5)
        if (GetMembersSize() >= GetMaxMembersSize())
            return false;

        if (newmember.guid == GetCaptainGuid())
            captainPresentInTeam = true;

        m_members.push_back(newmember);
    } while (arenaTeamMembersResult->NextRow());

    if (Empty() || !captainPresentInTeam)
    {
        // arena team is empty or captain is not in team, delete from db
        sLog.outErrorDb("ArenaTeam %u does not have any members or its captain is not in team, disbanding it...", m_TeamId);
        return false;
    }

    return true;
}

void ArenaTeam::SetCaptain(ObjectGuid guid)
{
    // disable remove/promote buttons
    Player *oldcaptain = sObjectMgr.GetPlayer(GetCaptainGuid());
    if (oldcaptain)
        oldcaptain->SetArenaTeamInfoField(GetSlot(), ARENA_TEAM_MEMBER, 1);

    // set new captain
    m_CaptainGuid = guid;

    // update database
    CharacterDatabase.PExecute("UPDATE arena_team SET captainguid = '%u' WHERE arenateamid = '%u'", guid.GetCounter(), m_TeamId);

    // enable remove/promote buttons
    if (Player *newcaptain = sObjectMgr.GetPlayer(guid))
        newcaptain->SetArenaTeamInfoField(GetSlot(), ARENA_TEAM_MEMBER, 0);
}

void ArenaTeam::DelMember(ObjectGuid guid)
{
    for (MemberList::iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
    {
        if (itr->guid == guid)
        {
            m_members.erase(itr);
            break;
        }
    }

    if (Player* player = sObjectMgr.GetPlayer(guid))
    {
        player->GetSession()->SendArenaTeamCommandResult(ERR_ARENA_TEAM_QUIT_S, GetName(), "", 0);
        // delete all info regarding this team
        for(int i = 0; i < ARENA_TEAM_END; ++i)
            player->SetArenaTeamInfoField(GetSlot(), ArenaTeamInfoType(i), 0);
    }

    CharacterDatabase.PExecute("DELETE FROM arena_team_member WHERE arenateamid = '%u' AND guid = '%u'", GetId(), guid.GetCounter());
}

void ArenaTeam::Disband(WorldSession *session)
{
    // event
    if (session)
    {
        // probably only 1 string required...
        BroadcastEvent(ERR_ARENA_TEAM_DISBANDED_S, session->GetPlayerName(), GetName().c_str());
    }

    while (!m_members.empty())
    {
        // Removing from members is done in DelMember.
        DelMember(m_members.front().guid);
    }

    CharacterDatabase.BeginTransaction();
    CharacterDatabase.PExecute("DELETE FROM arena_team WHERE arenateamid = '%u'", m_TeamId);
    CharacterDatabase.PExecute("DELETE FROM arena_team_member WHERE arenateamid = '%u'", m_TeamId); //< this should be already done by calling DelMember(memberGuids[j]); for each member
    CharacterDatabase.PExecute("DELETE FROM arena_team_stats WHERE arenateamid = '%u'", m_TeamId);
    CharacterDatabase.CommitTransaction();
    sObjectMgr.RemoveArenaTeam(m_TeamId);
}

void ArenaTeam::Roster(WorldSession *session)
{
    Player *pl = NULL;

    uint8 unk308 = 0;

    WorldPacket data(SMSG_ARENA_TEAM_ROSTER, 100);
    data << uint32(GetId());                                // team id
    data << uint8(unk308);                                  // 308 unknown value but affect packet structure
    data << uint32(GetMembersSize());                       // members count
    data << uint32(GetType());                              // arena team type?

    for (MemberList::const_iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
    {
        pl = sObjectMgr.GetPlayer(itr->guid);

        data << itr->guid;                                  // guid
        data << uint8((pl ? 1 : 0));                        // online flag
        data << itr->name;                                  // member name
        data << uint32((itr->guid == GetCaptainGuid() ? 0 : 1));// captain flag 0 captain 1 member
        data << uint8((pl ? pl->getLevel() : 0));           // unknown, level?
        data << uint8(itr->Class);                          // class
        data << uint32(itr->games_week);                    // played this week
        data << uint32(itr->wins_week);                     // wins this week
        data << uint32(itr->games_season);                  // played this season
        data << uint32(itr->wins_season);                   // wins this season
        data << uint32(itr->personal_rating);               // personal rating
        if(unk308)
        {
            data << float(0.0);                             // 308 unk
            data << float(0.0);                             // 308 unk
        }
    }

    session->SendPacket(&data);
    DEBUG_LOG("WORLD: Sent SMSG_ARENA_TEAM_ROSTER");
}

void ArenaTeam::Query(WorldSession *session)
{
    WorldPacket data(SMSG_ARENA_TEAM_QUERY_RESPONSE, 4*7+GetName().size()+1);
    data << uint32(GetId());                                // team id
    data << GetName();                                      // team name
    data << uint32(GetType());                              // arena team type (2=2x2, 3=3x3 or 5=5x5)
    data << uint32(m_BackgroundColor);                      // background color
    data << uint32(m_EmblemStyle);                          // emblem style
    data << uint32(m_EmblemColor);                          // emblem color
    data << uint32(m_BorderStyle);                          // border style
    data << uint32(m_BorderColor);                          // border color
    session->SendPacket(&data);
    DEBUG_LOG("WORLD: Sent SMSG_ARENA_TEAM_QUERY_RESPONSE");
}

void ArenaTeam::Stats(WorldSession *session)
{
    WorldPacket data(SMSG_ARENA_TEAM_STATS, 4*7);
    data << uint32(GetId());                                // team id
    data << uint32(m_stats.rating);                         // rating
    data << uint32(m_stats.games_week);                     // games this week
    data << uint32(m_stats.wins_week);                      // wins this week
    data << uint32(m_stats.games_season);                   // played this season
    data << uint32(m_stats.wins_season);                    // wins this season
    data << uint32(m_stats.rank);                           // rank
    session->SendPacket(&data);
}

void ArenaTeam::NotifyStatsChanged()
{
    // this is called after a rated match ended
    // updates arena team stats for every member of the team (not only the ones who participated!)
    for(MemberList::const_iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
    {
        Player * plr = sObjectMgr.GetPlayer(itr->guid);
        if(plr)
            Stats(plr->GetSession());
    }
}

void ArenaTeam::InspectStats(WorldSession *session, ObjectGuid guid)
{
    ArenaTeamMember* member = GetMember(guid);
    if(!member)
        return;

    WorldPacket data(MSG_INSPECT_ARENA_TEAMS, 8+1+4*6);
    data << guid;                                           // player guid
    data << uint8(GetSlot());                               // slot (0...2)
    data << uint32(GetId());                                // arena team id
    data << uint32(m_stats.rating);                         // rating
    data << uint32(m_stats.games_season);                   // season played
    data << uint32(m_stats.wins_season);                    // season wins
    data << uint32(member->games_season);                   // played (count of all games, that the inspected member participated...)
    data << uint32(member->personal_rating);                // personal rating
    session->SendPacket(&data);
}

void ArenaTeam::SetEmblem(uint32 backgroundColor, uint32 emblemStyle, uint32 emblemColor, uint32 borderStyle, uint32 borderColor)
{
    m_BackgroundColor = backgroundColor;
    m_EmblemStyle = emblemStyle;
    m_EmblemColor = emblemColor;
    m_BorderStyle = borderStyle;
    m_BorderColor = borderColor;

    CharacterDatabase.PExecute("UPDATE arena_team SET BackgroundColor='%u', EmblemStyle='%u', EmblemColor='%u', BorderStyle='%u', BorderColor='%u' WHERE arenateamid='%u'", m_BackgroundColor, m_EmblemStyle, m_EmblemColor, m_BorderStyle, m_BorderColor, m_TeamId);
}

void ArenaTeam::SetStats(uint32 stat_type, uint32 value)
{
    switch(stat_type)
    {
        case STAT_TYPE_RATING:
            m_stats.rating = value;
            CharacterDatabase.PExecute("UPDATE arena_team_stats SET rating = '%u' WHERE arenateamid = '%u'", value, GetId());
            break;
        case STAT_TYPE_GAMES_WEEK:
            m_stats.games_week = value;
            CharacterDatabase.PExecute("UPDATE arena_team_stats SET games_week = '%u' WHERE arenateamid = '%u'", value, GetId());
            break;
        case STAT_TYPE_WINS_WEEK:
            m_stats.wins_week = value;
            CharacterDatabase.PExecute("UPDATE arena_team_stats SET wins_week = '%u' WHERE arenateamid = '%u'", value, GetId());
            break;
        case STAT_TYPE_GAMES_SEASON:
            m_stats.games_season = value;
            CharacterDatabase.PExecute("UPDATE arena_team_stats SET games_season = '%u' WHERE arenateamid = '%u'", value, GetId());
            break;
        case STAT_TYPE_WINS_SEASON:
            m_stats.wins_season = value;
            CharacterDatabase.PExecute("UPDATE arena_team_stats SET wins_season = '%u' WHERE arenateamid = '%u'", value, GetId());
            break;
        case STAT_TYPE_RANK:
            m_stats.rank = value;
            CharacterDatabase.PExecute("UPDATE arena_team_stats SET rank = '%u' WHERE arenateamid = '%u'", value, GetId());
            break;
        default:
            DEBUG_LOG("unknown stat type in ArenaTeam::SetStats() %u", stat_type);
            break;
    }
}

uint32 ArenaTeam::GetAverageMMR(Group* group) const
{
    if (!group)
        return 0;

    uint32 matchMakerRating = 0;
    uint32 playerDivider = 0;
    for (MemberList::const_iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
    {
        // Skip if player is not online
        if (!ObjectAccessor::FindPlayer(itr->guid))
            continue;

        // Skip if player is not member of group
        if (!group->IsMember(itr->guid))
            continue;

        matchMakerRating += itr->MatchMakerRating;
        ++playerDivider;
    }

    // x/0 = crash
    if (playerDivider == 0)
        playerDivider = 1;

    matchMakerRating /= playerDivider;

    return matchMakerRating;
}

int32 ArenaTeam::GetMatchmakerRatingMod(uint32 ownRating, uint32 opponentRating, bool won /*, float& confidence_factor*/)
{
    // 'Chance' calculation - to beat the opponent
    // This is a simulation. Not much info on how it really works
    float chance = GetChanceAgainst(ownRating, opponentRating);
    float won_mod = (won) ? 1.0f : 0.0f;
    float mod = won_mod - chance;

    // Work in progress:
    /*
    // This is a simulation, as there is not much info on how it really works
    float confidence_mod = min(1.0f - fabs(mod), 0.5f);

    // Apply confidence factor to the mod:
    mod *= confidence_factor

    // And only after that update the new confidence factor
    confidence_factor -= ((confidence_factor - 1.0f) * confidence_mod) / confidence_factor;
    */

    // Real rating modification
    mod *= 24.0f;

    return (int32)ceil(mod);
}

int32 ArenaTeam::GetRatingMod(uint32 ownRating, uint32 opponentRating, bool won /*, float confidence_factor*/)
{
    // 'Chance' calculation - to beat the opponent
    // This is a simulation. Not much info on how it really works
    float chance = GetChanceAgainst(ownRating, opponentRating);
    float won_mod = (won) ? 1.0f : 0.0f;

    // Calculate the rating modification
    float mod;

    // TODO: Replace this hack with using the confidence factor (limiting the factor to 2.0f)
    if (won && ownRating < 1300)
    {
        if (ownRating < 1000)
            mod = 48.0f * (won_mod - chance);
        else
            mod = (24.0f + (24.0f * (1300.0f - float(ownRating)) / 300.0f)) * (won_mod - chance);
    }
    else
        mod = 24.0f * (won_mod - chance);

    return (int32)ceil(mod);
}

void ArenaTeam::BroadcastPacket(WorldPacket *packet)
{
    for (MemberList::const_iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
    {
        Player *player = sObjectMgr.GetPlayer(itr->guid);
        if(player)
            player->GetSession()->SendPacket(packet);
    }
}

void ArenaTeam::BroadcastEvent(ArenaTeamEvents event, ObjectGuid guid, char const* str1 /*=NULL*/, char const* str2 /*=NULL*/, char const* str3 /*=NULL*/)
{
    uint8 strCount = !str1 ? 0 : (!str2 ? 1 : (!str3 ? 2 : 3));

    WorldPacket data(SMSG_ARENA_TEAM_EVENT, 1 + 1 + 1*strCount + (!guid ? 0 : 8));
    data << uint8(event);
    data << uint8(strCount);

    if (str3)
    {
        data << str1;
        data << str2;
        data << str3;
    }
    else if (str2)
    {
        data << str1;
        data << str2;
    }
    else if (str1)
        data << str1;

    if (guid)
        data << ObjectGuid(guid);

    BroadcastPacket(&data);

    DEBUG_LOG("WORLD: Sent SMSG_ARENA_TEAM_EVENT");
}

uint8 ArenaTeam::GetSlotByType(ArenaType type )
{
    switch(type)
    {
        case ARENA_TYPE_2v2: return 0;
        case ARENA_TYPE_3v3: return 1;
        case ARENA_TYPE_5v5: return 2;
        default:
            break;
    }
    ERROR_LOG("FATAL: Unknown arena team type %u for some arena team", type);
    return 0xFF;
}

ArenaType ArenaTeam::GetTypeBySlot(uint8 slot)
{
    switch (slot)
    {
        case 0: return ARENA_TYPE_2v2;
        case 1: return ARENA_TYPE_3v3;
        case 2: return ARENA_TYPE_5v5;
        default:
            break;
    }
    sLog.outError("FATAL: Unknown arena team slot %u for some arena team", slot);
    return ArenaType(0xFF);
}

bool ArenaTeam::HaveMember(ObjectGuid guid) const
{
    for (MemberList::const_iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
        if(itr->guid == guid)
            return true;

    return false;
}

float ArenaTeam::GetChanceAgainst(uint32 own_rating, uint32 enemy_rating)
{
    // returns the chance to win against a team with the given rating, used in the rating adjustment calculation
    // ELO system

    if (sWorld.getConfig(CONFIG_UINT32_ARENA_SEASON_ID) >= 6)
        if (enemy_rating < 1000)
            enemy_rating = 1000;
    return 1.0f / (1.0f + exp(log(10.0f) * (float)((float)enemy_rating - (float)own_rating)/650.0f));
}

void ArenaTeam::FinishGame(int32 mod)
{
    if (int32(m_stats.rating) + mod < 0)
        m_stats.rating = 0;
    else
        m_stats.rating += mod;

    m_stats.games_week += 1;
    m_stats.games_season += 1;
    // update team's rank
    m_stats.rank = 1;
    ObjectMgr::ArenaTeamMap::const_iterator i = sObjectMgr.GetArenaTeamMapBegin();
    for ( ; i != sObjectMgr.GetArenaTeamMapEnd(); ++i)
    {
        if (i->second->GetType() == this->m_Type && i->second->GetStats().rating > m_stats.rating)
            ++m_stats.rank;
    }
}

int32 ArenaTeam::WonAgainst(uint32 Own_MMRating, uint32 Opponent_MMRating, int32& rating_change)
{
    // Called when the team has won
    // Change in Matchmaker rating
    int32 mod = GetMatchmakerRatingMod(Own_MMRating, Opponent_MMRating, true);

    // Change in Team Rating
    rating_change = GetRatingMod(m_stats.rating, Opponent_MMRating, true);

    // modify the team stats accordingly
    FinishGame(rating_change);
    m_stats.wins_week += 1;
    m_stats.wins_season += 1;

    // return the rating change, used to display it on the results screen
    return mod;
}

int32 ArenaTeam::LostAgainst(uint32 Own_MMRating, uint32 Opponent_MMRating, int32& rating_change)
{
    // Called when the team has lost
    // Change in Matchmaker Rating
    int32 mod = GetMatchmakerRatingMod(Own_MMRating, Opponent_MMRating, false);

    // Change in Team Rating
    rating_change = GetRatingMod(m_stats.rating, Opponent_MMRating, false);

    // modify the team stats accordingly
    FinishGame(rating_change);

    // return the rating change, used to display it on the results screen
    return mod;
}

void ArenaTeam::MemberLost(Player * player, uint32 againstMatchmakerRating, int32 MatchmakerRatingChange)
{
    // called for each participant of a match after losing
    for(MemberList::iterator itr = m_members.begin(); itr !=  m_members.end(); ++itr)
    {
        if (itr->guid == player->GetObjectGuid())
        {
            // Update personal rating
            int32 mod = GetRatingMod(itr->personal_rating, againstMatchmakerRating, false);
            itr->ModifyPersonalRating(player, mod, GetType());

            // Update matchmaker rating
            itr->ModifyMatchmakerRating(MatchmakerRatingChange, GetSlot());

            // update personal played stats
            itr->games_week += 1;
            itr->games_season += 1;
            // update the unit fields
            player->SetArenaTeamInfoField(GetSlot(), ARENA_TEAM_GAMES_WEEK,  itr->games_week);
            player->SetArenaTeamInfoField(GetSlot(), ARENA_TEAM_GAMES_SEASON,  itr->games_season);
            return;
        }
    }
}

void ArenaTeam::OfflineMemberLost(ObjectGuid guid, uint32 againstMatchmakerRating, int32 MatchmakerRatingChange)
{
    // called for offline player after ending rated arena match!
    for(MemberList::iterator itr = m_members.begin(); itr !=  m_members.end(); ++itr)
    {
        if (itr->guid == guid)
        {
            // update personal rating
            int32 mod = GetRatingMod(itr->personal_rating, againstMatchmakerRating, false);
            itr->ModifyPersonalRating(NULL, mod, GetType());

            // update matchmaker rating
            itr->ModifyMatchmakerRating(MatchmakerRatingChange, GetSlot());

            // update personal played stats
            itr->games_week += 1;
            itr->games_season += 1;
            return;
        }
    }
}

void ArenaTeam::MemberWon(Player* player, uint32 againstMatchmakerRating, int32 MatchmakerRatingChange)
{
    // called for each participant after winning a match
    for(MemberList::iterator itr = m_members.begin(); itr !=  m_members.end(); ++itr)
    {
        if (itr->guid == player->GetObjectGuid())
        {
            // update personal rating
            int32 mod = GetRatingMod(itr->personal_rating, againstMatchmakerRating, true);
            itr->ModifyPersonalRating(player, mod, GetType());

            // update matchmaker rating
            itr->ModifyMatchmakerRating(MatchmakerRatingChange, GetSlot());

            // update personal stats
            itr->games_week += 1;
            itr->games_season += 1;
            itr->wins_season += 1;
            itr->wins_week += 1;
            // update unit fields
            player->SetArenaTeamInfoField(GetSlot(), ARENA_TEAM_GAMES_WEEK, itr->games_week);
            player->SetArenaTeamInfoField(GetSlot(), ARENA_TEAM_GAMES_SEASON, itr->games_season);
            return;
        }
    }
}

void ArenaTeam::SaveToDB()
{
    // save team and member stats to db
    // called after a match has ended, or when calculating arena_points
    CharacterDatabase.BeginTransaction();
    CharacterDatabase.PExecute("UPDATE arena_team_stats SET rating = '%u',games_week = '%u',games_season = '%u',rank = '%u',wins_week = '%u',wins_season = '%u' WHERE arenateamid = '%u'", m_stats.rating, m_stats.games_week, m_stats.games_season, m_stats.rank, m_stats.wins_week, m_stats.wins_season, GetId());
    for(MemberList::const_iterator itr = m_members.begin(); itr !=  m_members.end(); ++itr)
    {
        CharacterDatabase.PExecute("UPDATE arena_team_member SET played_week = '%u', wons_week = '%u', played_season = '%u', wons_season = '%u', personal_rating = '%u' WHERE arenateamid = '%u' AND guid = '%u'", itr->games_week, itr->wins_week, itr->games_season, itr->wins_season, itr->personal_rating, m_TeamId, itr->guid.GetCounter());

        CharacterDatabase.PExecute("REPLACE INTO `character_mmr` (`guid`, `slot`, `mmr`) VALUES ('%u', '%u', '%u')",
            itr->guid.GetCounter(), GetSlot(), itr->MatchMakerRating);
    }
    CharacterDatabase.CommitTransaction();
}

void ArenaTeam::FinishWeek()
{
    m_stats.games_week = 0;                                   // played this week
    m_stats.wins_week = 0;                                    // wins this week
    for(MemberList::iterator itr = m_members.begin(); itr !=  m_members.end(); ++itr)
    {
        itr->games_week = 0;
        itr->wins_week = 0;
    }
}

bool ArenaTeam::IsFighting() const
{
    for (MemberList::const_iterator itr = m_members.begin(); itr != m_members.end(); ++itr)
    {
        if (Player *p = sObjectMgr.GetPlayer(itr->guid))
        {
            if (p->GetMap()->IsBattleArena())
                return true;
        }
    }
    return false;
}
