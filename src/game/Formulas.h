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

#ifndef MANGOS_FORMULAS_H
#define MANGOS_FORMULAS_H

#include "World.h"

namespace MaNGOS
{
    namespace Honor
    {
        inline float hk_honor_at_level(uint32 level, float mod = 1.0f)
        {
            return mod * level * 1.55f;
        }
    }
    namespace XP
    {
        enum XPColorChar { RED, ORANGE, YELLOW, GREEN, GRAY };

        inline uint32 GetGrayLevel(uint32 pl_level)
        {
            if( pl_level <= 5 )
                return 0;
            else if( pl_level <= 39 )
                return pl_level - 5 - pl_level/10;
            else if( pl_level <= 59 )
                return pl_level - 1 - pl_level/5;
            else
                return pl_level - 9;
        }

        inline XPColorChar GetColorCode(uint32 pl_level, uint32 mob_level)
        {
            if( mob_level >= pl_level + 5 )
                return RED;
            else if( mob_level >= pl_level + 3 )
                return ORANGE;
            else if( mob_level >= pl_level - 2 )
                return YELLOW;
            else if( mob_level > GetGrayLevel(pl_level) )
                return GREEN;
            else
                return GRAY;
        }

        inline uint32 GetZeroDifference(uint32 pl_level)
        {
            if( pl_level < 8 )  return 5;
            if( pl_level < 10 ) return 6;
            if( pl_level < 12 ) return 7;
            if( pl_level < 16 ) return 8;
            if( pl_level < 20 ) return 9;
            if( pl_level < 30 ) return 11;
            if( pl_level < 40 ) return 12;
            if( pl_level < 45 ) return 13;
            if( pl_level < 50 ) return 14;
            if( pl_level < 55 ) return 15;
            if( pl_level < 60 ) return 16;
            return 17;
        }

        inline uint32 BaseGain(uint32 pl_level, uint32 mob_level, ContentLevels content)
        {
            uint32 nBaseExp;
            switch(content)
            {
                case CONTENT_1_60:  nBaseExp = 45;  break;
                case CONTENT_61_70: nBaseExp = 235; break;
                case CONTENT_71_80: nBaseExp = 580; break;
                case CONTENT_81_85: nBaseExp = 1878; break;
                default:
                    ERROR_LOG("BaseGain: Unsupported content level %u",content);
                    nBaseExp = 45;  break;
            }

            if( mob_level >= pl_level )
            {
                uint32 nLevelDiff = mob_level - pl_level;
                if (nLevelDiff > 4)
                    nLevelDiff = 4;
                return ((pl_level*5 + nBaseExp) * (20 + nLevelDiff)/10 + 1)/2;
            }
            else
            {
                uint32 gray_level = GetGrayLevel(pl_level);
                if( mob_level > gray_level )
                {
                    uint32 ZD = GetZeroDifference(pl_level);
                    return (pl_level*5 + nBaseExp) * (ZD + mob_level - pl_level)/ZD;
                }
                return 0;
            }
        }

        inline uint32 Gain(Player *pl, Unit *u)
        {
            if(u->GetTypeId()==TYPEID_UNIT && (
                ((Creature*)u)->IsTotem() || ((Creature*)u)->IsPet() ||
                (((Creature*)u)->GetCreatureInfo()->flags_extra & CREATURE_FLAG_EXTRA_NO_XP_AT_KILL) ))
                return 0;

            uint32 xp_gain = BaseGain(pl->getLevel(), u->getLevel(), GetContentLevelsForMapAndZone(pl->GetMapId(), pl->GetZoneId()));
            if (xp_gain == 0)
                return 0;

            if(u->GetTypeId()==TYPEID_UNIT && ((Creature*)u)->IsElite())
                xp_gain *= 2;

            float xpMod = pl->IsPremiumActive() ? pl->GetPremiumXPModifier() : 1.0f;

            return (uint32)(xp_gain*sWorld.getConfig(CONFIG_FLOAT_RATE_XP_KILL)*xpMod);
        }

        inline float xp_in_group_rate(uint32 count, bool isRaid)
        {
            if(isRaid)
            {
                // FIX ME: must apply decrease modifiers dependent from raid size
                return 1.0f;
            }
            else
            {
                switch(count)
                {
                    case 0:
                    case 1:
                    case 2:
                        return 1.0f;
                    case 3:
                        return 1.166f;
                    case 4:
                        return 1.3f;
                    case 5:
                    default:
                        return 1.4f;
                }
            }
        }
    }
    namespace Currency
    {
        inline float ConquestRatingCalculator(uint32 rate)
        {
           if (rate <= 1500)
               return 1350 * 100.0f; // Default conquest points
           else if (rate > 3000)
               rate = 3000;

           // http://www.arenajunkies.com/topic/179536-conquest-point-cap-vs-personal-rating-chart/page__st__60#entry3085246
           return 1.4326 * ((1511.26 / (1 + 1639.28 / exp(0.00412 * rate))) + 850.15) * 100.0f;
        }

        inline float BgConquestRatingCalculator(uint32 rate)
        {
            // WowWiki: Battleground ratings receive a bonus of 22.2% to the cap they generate, plus 1 for corrections.
            return (ConquestRatingCalculator(rate) * 1.222f) + 50.0f;
        }
    }
}
#endif