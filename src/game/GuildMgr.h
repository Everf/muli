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

#ifndef _GUILDMGR_H
#define _GUILDMGR_H

#include "Common.h"
#include "Policies/Singleton.h"

#define GUILD_REP_FACTION 1168

class Guild;
class ObjectGuid;

struct GuildReward
{
    int32 Racemask;
    uint64 Price;
    uint32 AchievementId;
    uint8 Standing;
};

typedef std::map<uint32, GuildReward> GuildRewards;

class GuildMgr
{
        typedef UNORDERED_MAP<uint32, Guild*> GuildMap;

        GuildMap m_GuildMap;

        std::vector<uint64> GuildXPperLevel;
        GuildRewards m_GuildRewards;

    public:
        GuildMgr();
        ~GuildMgr();

        void AddGuild(Guild* guild);
        void RemoveGuild(uint32 guildId);
        void RemoveGuild(ObjectGuid guildGuid);

        Guild* GetGuildById(uint32 guildId) const;
        Guild* GetGuildByGuid(ObjectGuid guildGuid) const;
        Guild* GetGuildByName(std::string const& name) const;
        Guild* GetGuildByLeader(ObjectGuid const& guid) const;
        std::string GetGuildNameById(uint32 guildId) const;
        std::string GetGuildNameByGuid(ObjectGuid guildGuid) const;

        void LoadGuildXpForLevel();
        void LoadGuildRewards();
        void LoadGuilds();
        void SaveGuilds();

        void ResetExperienceCaps();
        void ResetReputationCaps();

        uint32 GetXPForGuildLevel(uint8 level) const;
        GuildRewards const& GetGuildRewards() const { return m_GuildRewards; }

        GuildMap const& GetGuildMap() const { return m_GuildMap; }
};

#define sGuildMgr MaNGOS::Singleton<GuildMgr>::Instance()

#endif // _GUILDMGR_H
