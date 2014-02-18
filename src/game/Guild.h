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

#ifndef MANGOSSERVER_GUILD_H
#define MANGOSSERVER_GUILD_H

#define WITHDRAW_MONEY_UNLIMITED    UI64LIT(0xFFFFFFFFFFFFFFFF)
#define WITHDRAW_SLOT_UNLIMITED     0xFFFFFFFF
#define GUILD_EXPERIENCE_UNCAPPED_LEVEL 20  ///> Hardcoded in client, starting from this level, guild daily experience gain is unlimited.

#include "Common.h"
#include "Item.h"
#include "ObjectAccessor.h"
#include "SharedDefines.h"
#include "ObjectAccessor.h"
#include "DBCStores.h"

class Item;

enum GuildDefaultRanks
{
    //these ranks can be modified, but they cannot be deleted
    GR_GUILDMASTER  = 0,
    GR_OFFICER      = 1,
    GR_VETERAN      = 2,
    GR_MEMBER       = 3,
    GR_INITIATE     = 4,
    //When promoting member server does: rank--;!
    //When demoting member server does: rank++;!
};

enum GuildRankRights
{
    GR_RIGHT_EMPTY                  = 0x00000040,
    GR_RIGHT_GCHATLISTEN            = 0x00000041,
    GR_RIGHT_GCHATSPEAK             = 0x00000042,
    GR_RIGHT_OFFCHATLISTEN          = 0x00000044,
    GR_RIGHT_OFFCHATSPEAK           = 0x00000048,
    GR_RIGHT_PROMOTE                = 0x000000C0,
    GR_RIGHT_DEMOTE                 = 0x00000140,
    GR_RIGHT_INVITE                 = 0x00000050,
    GR_RIGHT_REMOVE                 = 0x00000060,
    GR_RIGHT_SETMOTD                = 0x00001040,
    GR_RIGHT_EPNOTE                 = 0x00002040,
    GR_RIGHT_VIEWOFFNOTE            = 0x00004040,
    GR_RIGHT_EOFFNOTE               = 0x00008040,
    GR_RIGHT_MODIFY_GUILD_INFO      = 0x00010040,
    GR_RIGHT_WITHDRAW_GOLD_LOCK     = 0x00020000,               // remove money withdraw capacity
    GR_RIGHT_WITHDRAW_REPAIR        = 0x00040000,               // withdraw for repair
    GR_RIGHT_WITHDRAW_GOLD          = 0x00080000,               // withdraw gold
    GR_RIGHT_CREATE_GUILD_EVENT     = 0x00100000,               // wotlk
    GR_RIGHT_REQUIRES_AUTHENTICATOR = 0x00200000,
    GR_RIGHT_MODIFY_BANK_TABS       = 0x00400000,               // cata?
    GR_RIGHT_REMOVE_GUILD_EVENT     = 0x00800000,               // wotlk
    GR_RIGHT_ALL                    = 0x00DDF1FF,
};

enum Typecommand
{
    GUILD_CREATE_S  = 0x00,
    GUILD_INVITE_S  = 0x01,
    GUILD_QUIT_S    = 0x03,
    // 0x05?
    GUILD_FOUNDER_S = 0x0E,
    GUILD_UNK1      = 0x14,
    GUILD_UNK2      = 0x15,
};

enum CommandErrors
{
    ERR_PLAYER_NO_MORE_IN_GUILD         = 0x00,
    ERR_GUILD_INTERNAL                  = 0x01,
    ERR_ALREADY_IN_GUILD                = 0x02,
    ERR_ALREADY_IN_GUILD_S              = 0x03,
    ERR_INVITED_TO_GUILD                = 0x04,
    ERR_ALREADY_INVITED_TO_GUILD_S      = 0x05,
    ERR_GUILD_NAME_INVALID              = 0x06,
    ERR_GUILD_NAME_EXISTS_S             = 0x07,
    ERR_GUILD_LEADER_LEAVE              = 0x08,
    ERR_GUILD_PERMISSIONS               = 0x08,
    ERR_GUILD_PLAYER_NOT_IN_GUILD       = 0x09,
    ERR_GUILD_PLAYER_NOT_IN_GUILD_S     = 0x0A,
    ERR_GUILD_PLAYER_NOT_FOUND_S        = 0x0B,
    ERR_GUILD_NOT_ALLIED                = 0x0C,
    ERR_GUILD_RANK_TOO_HIGH_S           = 0x0D,
    ERR_GUILD_RANK_TOO_LOW_S            = 0x0E,
    ERR_GUILD_RANKS_LOCKED              = 0x11,
    ERR_GUILD_RANK_IN_USE               = 0x12,
    ERR_GUILD_IGNORING_YOU_S            = 0x13,
    ERR_GUILD_UNK1                      = 0x14,
    ERR_GUILD_WITHDRAW_LIMIT            = 0x19,
    ERR_GUILD_NOT_ENOUGH_MONEY          = 0x1A,
    ERR_GUILD_BANK_FULL                 = 0x1C,
    ERR_GUILD_ITEM_NOT_FOUND            = 0x1D,
    ERR_GUILD_TOO_MUCH_MONEY            = 0x1F,
    ERR_GUILD_BANK_WRONG_TAB            = 0x20,
    ERR_RANK_REQUIRES_AUTHENTICATOR     = 0x22,
    ERR_GUILD_BANK_VOUCHER_FAILED       = 0x23,
    ERR_GUILD_TRIAL_ACCOUNT             = 0x24,
    ERR_GUILD_UNDELETABLE_DUE_TO_LEVEL  = 0x25,
    ERR_GUILD_MOVE_STARTING             = 0x26,
    ERR_GUILD_REP_TOO_LOW               = 0x27,
};

enum GuildEvents
{
    GE_PROMOTION                    = 0x01,
    GE_DEMOTION                     = 0x02,
    GE_MOTD                         = 0x03,
    GE_JOINED                       = 0x04,
    GE_LEFT                         = 0x05,
    GE_REMOVED                      = 0x06,
    GE_LEADER_IS                    = 0x07,
    GE_LEADER_CHANGED               = 0x08,
    GE_DISBANDED                    = 0x09,
    //GE_TABARDCHANGE               = 0x0A,                 // not exists in 4.3.4
    GE_UPDATE_RANK                  = 0x0B,                 // string, string EVENT_GUILD_ROSTER_UPDATE tab content change?
    GE_CREATE_RANK                  = 0x0C,                 // EVENT_GUILD_ROSTER_UPDATE
    GE_DELETE_RANK                  = 0x0D,
    GE_RANK_ORDER_CHANGE            = 0x0E,
    GE_UNK                          = 0x0F,
    GE_SIGNED_ON                    = 0x10,                 // ERR_FRIEND_ONLINE_SS
    GE_SIGNED_OFF                   = 0x11,                 // ERR_FRIEND_OFFLINE_S
    GE_GUILDBANKBAGSLOTS_CHANGED    = 0x12,                 // EVENT_GUILDBANKBAGSLOTS_CHANGED
    GE_BANKTAB_PURCHASED            = 0x13,                 // EVENT_GUILDBANK_UPDATE_TABS
    GE_BANKTAB_UPDATED              = 0x14,                 // EVENT_GUILDBANK_UPDATE_TABS
    GE_GUILDBANK_UPDATE_MONEY       = 0x15,                 // EVENT_GUILDBANK_UPDATE_MONEY, string 0000000000002710 is 1 gold
    //GE_GUILD_BANK_MONEY_WITHDRAWN = 0x16,                 // not exists in 4.3.4
    GE_GUILDBANK_TEXT_CHANGED       = 0x17                  // EVENT_GUILDBANK_TEXT_CHANGED
};

enum PetitionTurns
{
    PETITION_TURN_OK                    = 0,
    PETITION_TURN_ALREADY_IN_GUILD      = 2,
    PETITION_TURN_NEED_MORE_SIGNATURES  = 4,
    PETITION_TURN_GUILD_PERMISSIONS     = 11,
    PETITION_TURN_GUILD_NAME_INVALID    = 12,
};

enum PetitionSigns
{
    PETITION_SIGN_OK                    = 0,
    PETITION_SIGN_ALREADY_SIGNED        = 1,
    PETITION_SIGN_ALREADY_IN_GUILD      = 2,
    PETITION_SIGN_CANT_SIGN_OWN         = 3,
    PETITION_SIGN_NOT_SAME_SERVER       = 5,
    PETITION_SIGN_PETITION_FULL         = 8,
    PETITION_SIGN_ALREADY_SIGNED_OTHER  = 10,
    PETITION_SIGN_RESTRICTED_ACCOUNT    = 11,
};

enum GuildBankRights
{
    GUILD_BANK_RIGHT_VIEW_TAB       = 0x01,
    GUILD_BANK_RIGHT_PUT_ITEM       = 0x02,
    GUILD_BANK_RIGHT_UPDATE_TEXT    = 0x04,

    GUILD_BANK_RIGHT_DEPOSIT_ITEM   = GUILD_BANK_RIGHT_VIEW_TAB | GUILD_BANK_RIGHT_PUT_ITEM,
    GUILD_BANK_RIGHT_FULL           = 0xFF,
};

enum GuildBankEventLogTypes
{
    GUILD_BANK_LOG_DEPOSIT_ITEM         = 1,
    GUILD_BANK_LOG_WITHDRAW_ITEM        = 2,
    GUILD_BANK_LOG_MOVE_ITEM            = 3,
    GUILD_BANK_LOG_DEPOSIT_MONEY        = 4,
    GUILD_BANK_LOG_WITHDRAW_MONEY       = 5,
    GUILD_BANK_LOG_REPAIR_MONEY         = 6,
    GUILD_BANK_LOG_MOVE_ITEM2           = 7,
    GUILD_BANK_LOG_UNK1                 = 8,
    GUILD_BANK_LOG_BUY_SLOT             = 9,
    GUILD_BANK_LOG_CASH_FLOW_DEPOSIT    = 10,
};

enum GuildEventLogTypes
{
    GUILD_EVENT_LOG_INVITE_PLAYER     = 1,
    GUILD_EVENT_LOG_JOIN_GUILD        = 2,
    GUILD_EVENT_LOG_PROMOTE_PLAYER    = 3,
    GUILD_EVENT_LOG_DEMOTE_PLAYER     = 4,
    GUILD_EVENT_LOG_UNINVITE_PLAYER   = 5,
    GUILD_EVENT_LOG_LEAVE_GUILD       = 6,
};

enum GuildEmblem
{
    ERR_GUILDEMBLEM_SUCCESS               = 0,
    ERR_GUILDEMBLEM_INVALID_TABARD_COLORS = 1,
    ERR_GUILDEMBLEM_NOGUILD               = 2,
    ERR_GUILDEMBLEM_NOTGUILDMASTER        = 3,
    ERR_GUILDEMBLEM_NOTENOUGHMONEY        = 4,
    ERR_GUILDEMBLEM_INVALIDVENDOR         = 5
};

enum GuildMemberFlags
{
    GUILDMEMBER_STATUS_NONE      = 0x0000,
    GUILDMEMBER_STATUS_ONLINE    = 0x0001,
    GUILDMEMBER_STATUS_AFK       = 0x0002,
    GUILDMEMBER_STATUS_DND       = 0x0004,
    GUILDMEMBER_STATUS_MOBILE    = 0x0008,
};

enum GuildNews
{
    GUILD_NEWS_GUILD_ACHIEVEMENT      = 0,
    GUILD_NEWS_PLAYER_ACHIEVEMENT     = 1,
    GUILD_NEWS_DUNGEON_ENCOUNTER      = 2, // Todo Implement
    GUILD_NEWS_ITEM_LOOTED            = 3,
    GUILD_NEWS_ITEM_CRAFTED           = 4,
    GUILD_NEWS_ITEM_PURCHASED         = 5,
    GUILD_NEWS_LEVEL_UP               = 6,
};

struct GuildNewsEventLogEntry
{
    GuildNews EventType;
    time_t Date;
    uint64 PlayerGuid;
    uint32 Flags;
    uint32 Data;

    void WriteData(uint32 guid, WorldPacket* data);
};

uint32 const MinNewsItemLevel[MAX_CONTENT] = { 61, 90, 200, 353 };

inline uint64 GetGuildBankTabPrice(uint8 Index)
{
    switch(Index)
    {
        case 0: return 100;
        case 1: return 250;
        case 2: return 500;
        case 3: return 1000;
        case 4: return 2500;
        case 5: return 5000;
        default:
            return 0;
    }
}

struct GuildEventLogEntry
{
    uint8  EventType;
    uint32 PlayerGuid1;
    uint32 PlayerGuid2;
    uint8  NewRank;
    uint64 TimeStamp;

    void WriteData(WorldPacket& data, ByteBuffer& buffer);
};

struct GuildBankEventLogEntry
{
    uint8  EventType;
    uint32 PlayerGuid;
    uint32 ItemOrMoney;
    uint8  ItemStackCount;
    uint8  DestTabId;
    uint64 TimeStamp;

    bool isMoneyEvent() const
    {
        return EventType == GUILD_BANK_LOG_DEPOSIT_MONEY ||
            EventType == GUILD_BANK_LOG_WITHDRAW_MONEY ||
            EventType == GUILD_BANK_LOG_REPAIR_MONEY;
    }

    void WriteData(WorldPacket& data, ByteBuffer& buffer);
};

struct GuildBankTab
{
    GuildBankTab() { memset(Slots, 0, GUILD_BANK_MAX_SLOTS * sizeof(Item*)); }

    Item* Slots[GUILD_BANK_MAX_SLOTS];
    std::string Name;
    std::string Icon;
    std::string Text;
};

struct GuildItemPosCount
{
    GuildItemPosCount(uint8 _slot, uint32 _count) : Slot(_slot), Count(_count) {}

    bool isContainedIn(std::vector<GuildItemPosCount> const& vec) const;

    uint8 Slot;
    uint32 Count;
};
typedef std::vector<GuildItemPosCount> GuildItemPosCountVec;

#define MAX_GUILD_PROFESSIONS 2

struct MemberSlot
{
    struct ProfessionInfo
    {
        ProfessionInfo() : skillId(0), value(0), rank(0) { }
        ProfessionInfo(uint32 _skillId, uint16 _value, uint8 _rank) : skillId(_skillId), value(_value), rank(_rank) { }

        uint32 skillId;
        uint16 value;
        uint8 rank;
    };

    void SetMemberStats(Player* player, bool save = true);
    void UpdateLogoutTime();
    void SetPNOTE(std::string pnote);
    void SetOFFNOTE(std::string offnote);
    void ChangeRank(uint32 newRank);
    uint32 GetMaximumWeeklyReputation() const;

    ObjectGuid guid;
    uint32 accountId;
    std::string Name;
    uint32 RankId;
    uint8 Level;
    uint8 Class;
    uint32 ZoneId;
    uint64 LogoutTime;
    uint32 thisWeekReputation;
    uint32 AchievementPoints;
    std::string Pnote;
    std::string OFFnote;
    uint32 BankResetTimeMoney;
    uint32 BankRemMoney;
    uint32 BankResetTimeTab[GUILD_BANK_MAX_TABS];
    uint32 BankRemSlotsTab[GUILD_BANK_MAX_TABS];

    ProfessionInfo m_professions[MAX_GUILD_PROFESSIONS];
};

struct RankInfo
{
    RankInfo(const std::string& _name, uint32 _rights, uint32 _money) : Name(_name), Rights(_rights), BankMoneyPerDay(_money)
    {
        for(uint8 i = 0; i < GUILD_BANK_MAX_TABS; ++i)
        {
            TabRight[i] = 0;
            TabSlotPerDay[i] = 0;
        }
    }

    std::string Name;
    uint32 Rights;
    uint64 BankMoneyPerDay;
    uint32 TabRight[GUILD_BANK_MAX_TABS];
    uint32 TabSlotPerDay[GUILD_BANK_MAX_TABS];
};

class Guild
{
    public:
        /** These are actually ordered lists. The first element is the oldest entry.*/
        typedef std::list<GuildEventLogEntry> GuildEventLog;
        typedef std::list<GuildBankEventLogEntry> GuildBankEventLog;
        typedef std::map<uint32, GuildNewsEventLogEntry> GuildNewsEventLog;

        Guild();
        ~Guild();

        bool Create(Player* leader, std::string gname);
        void CreateDefaultGuildRanks(int locale_idx);
        void Disband();

        void DeleteGuildBankItems(bool alsoInDB = false);
        typedef UNORDERED_MAP<uint32, MemberSlot> MemberList;
        typedef std::vector<RankInfo> RankList;

        uint32 GetId() const { return m_Id; }
        ObjectGuid GetObjectGuid() const { return ObjectGuid(HIGHGUID_GUILD, 0, m_Id); }
        ObjectGuid GetLeaderGuid() const { return m_LeaderGuid; }
        std::string const& GetName() const { return m_Name; }
        std::string const& GetMOTD() const { return MOTD; }
        std::string const& GetGINFO() const { return GINFO; }

        time_t GetCreatedDate() const { return m_CreatedDate; }

        uint32 GetEmblemStyle() const { return m_EmblemStyle; }
        uint32 GetEmblemColor() const { return m_EmblemColor; }
        uint32 GetBorderStyle() const { return m_BorderStyle; }
        uint32 GetBorderColor() const { return m_BorderColor; }
        uint32 GetBackgroundColor() const { return m_BackgroundColor; }

        void SetLeader(ObjectGuid guid);
        bool AddMember(ObjectGuid plGuid, uint32 plRank);
        bool DelMember(ObjectGuid guid, bool isDisbanding = false);
        //lowest rank is the count of ranks - 1 (the highest rank_id in table)
        uint32 GetLowestRank() const { return m_Ranks.size() - 1; }

        void SetMOTD(std::string motd);
        void SetGINFO(std::string ginfo);
        void SetEmblem(uint32 emblemStyle, uint32 emblemColor, uint32 borderStyle, uint32 borderColor, uint32 backgroundColor);

        uint32 GetMemberSize() const { return members.size(); }
        uint32 GetAccountsNumber();

        bool LoadGuildFromDB(QueryResult *guildDataResult);
        void SaveToDB();
        bool CheckGuildStructure();
        bool LoadRanksFromDB(QueryResult *guildRanksResult);
        bool LoadMembersFromDB(QueryResult *guildMembersResult);

        void BroadcastToGuild(WorldSession* session, const std::string& msg, uint32 language = LANG_UNIVERSAL);
        void BroadcastAddonToGuild(WorldSession* session, const std::string& msg, const std::string& prefix);
        void BroadcastToOfficers(WorldSession* session, const std::string& msg, uint32 language = LANG_UNIVERSAL);
        void BroadcastAddonToOfficers(WorldSession* session, const std::string& msg, const std::string& prefix);
        void BroadcastPacketToRank(WorldPacket* packet, uint32 rankId);
        void BroadcastPacket(WorldPacket* packet);

        void BroadcastEvent(GuildEvents event, ObjectGuid guid, char const* str1 = NULL, char const* str2 = NULL, char const* str3 = NULL);
        void BroadcastEvent(GuildEvents event, char const* str1 = NULL, char const* str2 = NULL, char const* str3 = NULL)
        {
            BroadcastEvent(event, ObjectGuid(), str1, str2, str3);
        }

        template<class Do>
        void BroadcastWorker(Do& _do, Player* except = NULL)
        {
            for(MemberList::iterator itr = members.begin(); itr != members.end(); ++itr)
                if(Player *player = ObjectAccessor::FindPlayer(ObjectGuid(HIGHGUID_PLAYER, itr->first)))
                    if(player != except)
                        _do(player);
        }

        void CreateRank(std::string name, uint32 rights);
        void DelRank(uint32 rankId);
        void SwitchRank(uint32 rankId, bool up);
        std::string GetRankName(uint32 rankId);
        uint32 GetRankRights(uint32 rankId);
        uint32 GetRanksSize() const { return m_Ranks.size(); }

        void SetRankName(uint32 rankId, std::string name);
        void SetRankRights(uint32 rankId, uint32 rights);
        bool HasRankRight(uint32 rankId, uint32 right)
        {
            return ((GetRankRights(rankId) & right) != GR_RIGHT_EMPTY) ? true : false;
        }

        bool HasMembersWithRank(uint32 rankId) const
        {
            for (MemberList::const_iterator itr = members.begin(); itr != members.end(); ++itr)
                if (itr->second.RankId == rankId)
                    return true;

            return false;
        }

        int32 GetRank(ObjectGuid guid)
        {
            MemberSlot* slot = GetMemberSlot(guid);
            return slot ? slot->RankId : -1;
        }

        MemberSlot* GetMemberSlot(ObjectGuid guid)
        {
            MemberList::iterator itr = members.find(guid.GetCounter());
            return itr != members.end() ? &itr->second : NULL;
        }

        MemberSlot* GetMemberSlot(const std::string& name)
        {
            for(MemberList::iterator itr = members.begin(); itr != members.end(); ++itr)
                if(itr->second.Name == name)
                    return &itr->second;

            return NULL;
        }

        void Roster(WorldSession* session = NULL);          // NULL = broadcast
        void Query(WorldSession* session);
        void QueryRanks(WorldSession* session);

        // News EventLog
        void LoadGuildNewsEventLogFromDB();
        void SendNewsEventLog(WorldSession* session);
        void LogNewsEvent(GuildNews eventType, time_t date, uint64 playerGuid, uint32 flags, uint32 data);
        GuildNewsEventLogEntry* GetNewsById(uint32 id)
        {
            GuildNewsEventLog::iterator itr = m_GuildNewsEventLog.find(id);
            if (itr != m_GuildNewsEventLog.end())
                return &itr->second;
            else
                return NULL;
        }

        // Guild EventLog
        void LoadGuildEventLogFromDB();
        void DisplayGuildEventLog(WorldSession *session);
        void LogGuildEvent(uint8 EventType, ObjectGuid playerGuid1, ObjectGuid playerGuid2 = ObjectGuid(), uint8 newRank = 0);

        // ** Guild bank **
        // Content & item deposit/withdraw
        void DisplayGuildBankContent(WorldSession *session, uint8 TabId);
        void DisplayGuildBankMoneyUpdate(WorldSession *session);

        void SwapItems( Player * pl, uint8 BankTab, uint8 BankTabSlot, uint8 BankTabDst, uint8 BankTabSlotDst, uint32 SplitedAmount);
        void MoveFromBankToChar( Player * pl, uint8 BankTab, uint8 BankTabSlot, uint8 PlayerBag, uint8 PlayerSlot, uint32 SplitedAmount);
        void MoveFromCharToBank( Player * pl, uint8 PlayerBag, uint8 PlayerSlot, uint8 BankTab, uint8 BankTabSlot, uint32 SplitedAmount);

        // Tabs
        void DisplayGuildBankTabsInfo(WorldSession *session);
        void CreateNewBankTab();
        void SetGuildBankTabText(uint8 TabId, std::string text);
        void SendGuildBankTabText(WorldSession *session, uint8 TabId);
        void SetGuildBankTabInfo(uint8 TabId, std::string name, std::string icon);
        uint8 GetPurchasedTabs() const { return m_TabListMap.size(); }
        uint32 GetBankRights(uint32 rankId, uint8 TabId) const;
        bool IsMemberHaveRights(uint32 LowGuid, uint8 TabId,uint32 rights) const;
        bool CanMemberViewTab(uint32 LowGuid, uint8 TabId) const;
        // Load
        void LoadGuildBankFromDB();
        // Money deposit/withdraw
        void SendMoneyInfo(WorldSession* session, uint32 LowGuid);
        bool MemberMoneyWithdraw(uint64 amount, uint32 LowGuid);
        uint64 GetGuildBankMoney() { return m_GuildBankMoney; }
        void SetBankMoney(int64 money);
        void SetThisWeekReputation(ObjectGuid playerGuid, uint32 amt);
        void HandleCashFlow(uint64 money, Player* player);
        // per days
        bool MemberItemWithdraw(uint8 TabId, uint32 LowGuid);
        uint32 GetMemberSlotWithdrawRem(uint32 LowGuid, uint8 TabId);
        uint64 GetMemberMoneyWithdrawRem(uint32 LowGuid);
        void SetBankMoneyPerDay(uint32 rankId, uint64 money);
        void SetBankRightsAndSlots(uint32 rankId, uint8 TabId, uint32 right, uint32 SlotPerDay, bool db);
        uint64 GetBankMoneyPerDay(uint32 rankId);
        uint32 GetBankSlotPerDay(uint32 rankId, uint8 TabId);
        // rights per day
        bool LoadBankRightsFromDB(QueryResult *guildBankTabRightsResult);
        // Guild Bank Event Logs
        void LoadGuildBankEventLogFromDB();
        void DisplayGuildBankLogs(WorldSession *session, uint8 TabId);
        void LogBankEvent(uint8 EventType, uint8 TabId, uint32 PlayerGuidLow, uint32 ItemOrMoney, uint8 ItemStackCount=0, uint8 DestTabId=0);
        bool AddGBankItemToDB(uint32 GuildId, uint32 BankTab , uint32 BankTabSlot , uint32 GUIDLow, uint32 Entry );

        // Guild leveling
        uint32 GetLevel() const { return m_Level; }
        void GiveXP(int64 xp, Player* source);
        void TakeXP(int64 xp, Player* source);
        uint64 GetExperience() const { return m_Experience; }
        uint64 GetTodayExperience() const { return m_TodayExperience; }
        void SendGuildXP(Player* player);
        void ResetDailyExperience();
        void ResetReputationCaps();

        void HandleGuildPartyRequest(WorldSession* session);
        void SendReputationWeeklyCap(Player* player);
        void OnLogin(Player* player);

        AchievementMgr<Guild>& GetAchievementMgr() { return m_achievementMgr; }
        AchievementMgr<Guild> const& GetAchievementMgr() const { return m_achievementMgr; }

    protected:
        void AddRank(const std::string& name,uint32 rights,uint32 money);

        uint32 m_Id;
        std::string m_Name;
        ObjectGuid m_LeaderGuid;
        std::string MOTD;
        std::string GINFO;
        time_t m_CreatedDate;

        uint32 m_EmblemStyle;
        uint32 m_EmblemColor;
        uint32 m_BorderStyle;
        uint32 m_BorderColor;
        uint32 m_BackgroundColor;
        uint32 m_accountsNumber;                            // 0 used as marker for need lazy calculation at request

        RankList m_Ranks;

        MemberList members;

        typedef std::vector<GuildBankTab*> TabListMap;
        TabListMap m_TabListMap;

        GuildEventLog m_GuildEventLog;
        GuildNewsEventLog m_GuildNewsEventLog;
        GuildBankEventLog m_GuildBankEventLog_Money;
        GuildBankEventLog m_GuildBankEventLog_Item[GUILD_BANK_MAX_TABS];

        uint32 m_GuildEventLogNextGuid;
        uint32 m_GuildNewsEventLogNextGuid;
        uint32 m_GuildBankEventLogNextGuid_Money;
        uint32 m_GuildBankEventLogNextGuid_Item[GUILD_BANK_MAX_TABS];

        uint64 m_GuildBankMoney;

        AchievementMgr<Guild> m_achievementMgr;

        // Guild leveling
        uint32 m_Level;
        uint64 m_Experience;
        uint64 m_TodayExperience;

    private:
        void UpdateAccountsNumber() { m_accountsNumber = 0;}// mark for lazy calculation at request in GetAccountsNumber
        void _ChangeRank(ObjectGuid guid, MemberSlot* slot, uint32 newRank);

        // used only from high level Swap/Move functions
        Item*  GetItem(uint8 TabId, uint8 SlotId);
        InventoryResult CanStoreItem( uint8 tab, uint8 slot, GuildItemPosCountVec& dest, uint32 count, Item *pItem, bool swap = false) const;
        Item*  StoreItem( uint8 tab, GuildItemPosCountVec const& pos, Item *pItem );
        void   RemoveItem(uint8 tab, uint8 slot );
        void   DisplayGuildBankContentUpdate(uint8 TabId, int32 slot1, int32 slot2 = -1);
        void   DisplayGuildBankContentUpdate(uint8 TabId, GuildItemPosCountVec const& slots);

        // internal common parts for CanStore/StoreItem functions
        void AppendDisplayGuildBankSlot(WorldPacket& data, ByteBuffer& buffer, GuildBankTab const* tab, int32 slot);
        InventoryResult _CanStoreItem_InSpecificSlot(uint8 tab, uint8 slot, GuildItemPosCountVec& dest, uint32& count, bool swap, Item* pSrcItem) const;
        InventoryResult _CanStoreItem_InTab(uint8 tab, GuildItemPosCountVec& dest, uint32& count, bool merge, Item* pSrcItem, uint8 skip_slot) const;
        Item* _StoreItem(uint8 tab, uint8 slot, Item* pItem, uint32 count, bool clone);
};
#endif