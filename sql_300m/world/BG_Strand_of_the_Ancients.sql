SET NAMES 'utf8';

UPDATE `gameobject_template` SET `faction` = 0, `flags` = `flags` &~16 WHERE `entry` = 192829;

DELETE FROM `gameobject` WHERE `map` = 607;
DELETE FROM `creature` WHERE `map` = 607;

-- Strand of the Ancients 
-- Rigger Sparklight
UPDATE `creature_template` SET `npcflag` = 0 WHERE `entry` IN (29260, 29262);

-- Seaforium bombs
UPDATE `gameobject_template` SET `faction` = 35 WHERE `entry` = 190753;
UPDATE `gameobject_template` SET `data2` = 1, `data4` = 1, `data7` = 10 WHERE `entry` = 190752;

-- Doors
UPDATE `gameobject_template` SET `faction` = 14 WHERE `entry` IN (192549, 190727, 190726, 190723, 190724, 190722);

-- Support Vehichles
-- Battleground Demolisher
UPDATE `creature_template` SET `npcflag` = 0, `minlevel` = 70, `maxlevel` = 70, `minhealth` = 60000, `maxhealth` = 60000, `spell1` = 52338, `spell2` = 60206, `mechanic_immune_mask` = 1|2|8|0x10|0x20|0x80|0x200|0x800|0x2000|0x10000|0x800000|0x1000000|0x4000000|0x20000000, `RegenHealth` = 0, `speed_walk` = 1, `speed_run` = 1.14286, unit_flags = 0x8000, flags_extra = flags_extra | 0x1000, mingold=0 ,maxgold = 0 WHERE `entry` = 28781;
UPDATE `creature_template` SET `npcflag` = 0, `minlevel` = 80, `maxlevel` = 80, `minhealth` = 130000, `maxhealth` = 130000, `spell1` = 52338, `spell2` = 60206, `mechanic_immune_mask` = 1|2|8|0x10|0x20|0x80|0x200|0x800|0x2000|0x10000|0x800000|0x1000000|0x4000000|0x20000000, `RegenHealth` = 0, `speed_walk` = 1, `speed_run` = 1.14286, unit_flags = 0x8000, flags_extra = flags_extra | 0x1000, mingold=0, maxgold = 0 WHERE `entry` = 32796;

-- Demolisher
DELETE FROM creature_template_addon WHERE entry IN (28781, 32796);
INSERT INTO creature_template_addon  VALUES 
(28781, 0, 0, 0, 0, 0, 0, 52455),
(32796, 0, 0, 0, 0, 0, 0, 52455);

DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` IN (28781, 32796);
INSERT INTO `npc_spellclick_spells` VALUES
(28781, 60683, 0, 0, 0, 1),
(32796, 60683, 0, 0, 0, 1);
UPDATE `creature_template` SET `difficulty_entry_1` = 32796 WHERE `entry` = 28781;
UPDATE `creature_template` SET `armor` = 13622 WHERE `entry` = 32796;
-- Antipersonnel Cannon
UPDATE `creature_template` SET `npcflag` = 0, `minlevel` = 70, `maxlevel` = 70, `unit_flags`=`unit_flags`|4|8|4096, `minhealth` = 44910, `maxhealth` = 44910, `faction_A` = 35, `faction_H` = 35, `spell1` = 49872, `mechanic_immune_mask` = `mechanic_immune_mask`|32|64, `RegenHealth` = 0, unit_flags = 0x8004, flags_extra = flags_extra | 0x1000, mingold = 0,maxgold = 0 WHERE `entry` = 27894;
UPDATE `creature_template` SET `npcflag` = 0, `minlevel` = 80, `maxlevel` = 80, `unit_flags`=`unit_flags`|4|8|4096, `minhealth` = 63000, `maxhealth` = 63000, `faction_A` = 35, `faction_H` = 35, `spell1` = 49872, `mechanic_immune_mask` = `mechanic_immune_mask`|32|64, `RegenHealth` = 0, unit_flags = 0x8004, flags_extra = flags_extra | 0x1000, mingold = 0,maxgold = 0 WHERE `entry` = 32795;
DELETE FROM `npc_spellclick_spells` WHERE `npc_entry` IN (27894, 32795);
INSERT INTO `npc_spellclick_spells` VALUES
(27894, 60682, 0, 0, 0, 1),
(32795, 60682, 0, 0, 0, 1);
UPDATE `creature_template` SET `difficulty_entry_1` = 32795 WHERE `entry` = 27894;
UPDATE `creature_template` SET `armor` = 13622 WHERE `entry` = 32795;

DELETE FROM `battleground_events` WHERE `map` = 607;
INSERT INTO `battleground_events` VALUES
(607, 254, 0, 'Door'),
(607, 5, 0, 'Spirit for ALLIANCE defender'),
(607, 5, 1, 'Spirit for HORDE defender'),
(607, 6, 0, 'Bomb Beach'),
(607, 7, 0, 'Spawn turrets'),
(607, 8, 0, 'Spawn all GO'),
(607, 9, 0, 'West Graveyard Captured - Add vehicles'),
(607, 10, 0, 'East Graveyard Captured - Add vehicles'),

(607, 11, 0, 'Spawn bombs after green gates'),
(607, 12, 0, 'Spawn bombs after blue gates'),
(607, 13, 0, 'Spawn bombs after violet gates'),
(607, 14, 0, 'Spawn bombs after red gates'),
(607, 15, 0, 'Spawn bombs after yellow gates'),

(607, 16, 0, 'Add demolishers to beach West'),
(607, 17, 0, 'Add demolishers to beach East'),

(607, 18, 0, 'Add goblin after Green'),
(607, 19, 0, 'Add goblin after Blue'),

(607, 22, 0, 'Spawn Green Sigil'),
(607, 23, 0, 'Spawn Blue Sigil'),
(607, 24, 0, 'Spawn Purple Sigil'),
(607, 25, 0, 'Spawn Red Sigil'),
(607, 26, 0, 'Spawn Yellow Sigil'),

(607, 0, 0, 'Alliance West Graveyard'),
(607, 0, 1, 'Horde West Graveyard'),
(607, 1, 0, 'Alliance East Graveyard'),
(607, 1, 1, 'Horde East Graveyard'),
(607, 2, 0, 'Alliance South Graveyard'),
(607, 2, 1, 'Horde South Graveyard');

-- Sigils
INSERT INTO `gameobject` (`guid`, `id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
(701179, 192687, 607, 3, 1415.57, 105.176, 41.5615, 5.44516, 0, 0, 0, 0, 10, 0, 0),
(701180, 192689, 607, 3, 1433.34, -216.488, 43.4646, 0.871786, 0, 0, 0, 0, 10, 0, 0),
(701181, 192691, 607, 3, 1218.8, 78.7113, 65.5368, 5.64543, 0, 0, 0, 0, 10, 0, 0),
(701182, 192690, 607, 3, 1231.73, -210.455, 67.8883, 0.512855, 0, 0, 0, 0, 10, 0, 0),
(701183, 192685, 607, 3, 1057.88, -107.601, 94.7255, 0.0471208, 0, 0, 0, 0, 10, 0, 0);
DELETE FROM `gameobject_battleground` WHERE `guid` BETWEEN 701179 AND 701183;
INSERT INTO `gameobject_battleground` VALUES
(701179, 22, 0),
(701180, 23, 0),
(701181, 24, 0),
(701182, 25, 0),
(701183, 26, 0);

INSERT INTO `gameobject` (`guid`, `id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
-- Titan Relic
(701001, 192829, 607, 3, 836.502, -108.811, 127.587, 0.121379, 0, 0, 0, 0, 5, 0, 1),
-- Doors
-- Yellow
(701002, 190727, 607, 3, 1054.47, -107.76, 82.16, 0.06, 0, 0, 0, 0, 600, 0, 1),
-- Red
(701003, 190726, 607, 3, 1228.62, -212.12, 55.34, 0.48, 0, 0, 0, 0, 600, 0, 1),
-- Purple
(701004, 190723, 607, 3, 1215.82, 80.64, 53.38, 5.68, 0, 0, 0, 0, 600, 0, 1),
-- Sapphire
(701005, 190724, 607, 3, 1431.05, -219.21, 30.89, 0.83, 0, 0, 0, 0, 600, 0, 1),
-- Emerald
(701006, 190722, 607, 3, 1413.15, 107.78, 28.69, 5.42, 0, 0, 0, 0, 600, 0, 1),
-- Ancient
(701007, 192549, 607, 3, 873.3, -108.286, 117.171, 0.00894308, 0, 0, 0.00447152, 0.99999, 600, 0, 1);
DELETE FROM `gameobject_battleground` WHERE `guid` BETWEEN 701001 AND 701007;
INSERT INTO `gameobject_battleground` VALUES
(701001, 8, 0),
(701002, 8, 0),
(701003, 8, 0),
(701004, 8, 0),
(701005, 8, 0),
(701006, 8, 0),
(701007, 8, 0);
-- Rigger Sparklight
DELETE FROM `creature` WHERE `guid` BETWEEN 701001 AND 701016 OR `guid` BETWEEN 701027 AND 701030;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`) VALUES
(701001, 29260, 607, 3, 1348.52, -300.26, 30.99, 1.49),
(701002, 29262, 607, 3, 1358.20, 195.87, 30.93, 3.84),
-- Antipersonnel Cannon
(701003, 27894, 607, 3, 1421.94, -196.53, 42.18, 0.90),
(701004, 27894, 607, 3, 1455.09, -219.92, 41.95, 1.03),
(701005, 27894, 607, 3, 1405.33, 84.25, 41.18, 5.40),
(701006, 27894, 607, 3, 1436.51, 110.02, 41.40, 5.32),
(701007, 27894, 607, 3, 1236.33, 92.17, 64.96, 5.68),
(701008, 27894, 607, 3, 1215.04, 57.73, 64.73, 5.71),
(701009, 27894, 607, 3, 1232.35, -187.34, 66.94, 0.37),
(701010, 27894, 607, 3, 1249.95, -223.74, 66.72, 0.43),
(701011, 27894, 607, 3, 1068.82, -127.38, 96.44, 0.06),
(701012, 27894, 607, 3, 1068.35, -87.04, 93.80, 6.24),
-- Battleground Demolisher
(701013, 28781, 607, 3, 1579.20, -159.83, 3.77, 3.10),
(701014, 28781, 607, 3, 1610.55, -117.52, 8.77, 2.44),
(701015, 28781, 607, 3, 1578.30, 99.06, 1.98, 3.38),
(701016, 28781, 607, 3, 1618.60, 62.26, 7.17, 3.61),
(701027, 28781, 607, 3, 1353.28, 224.092, 35.2432, 4.35363),
(701028, 28781, 607, 3, 1347.65, 208.805, 34.2892, 4.39378),
(701029, 28781, 607, 3, 1371.2, -317.169, 34.9982, 1.93504),
(701030, 28781, 607, 3, 1365.52, -301.854, 34.0439, 1.91998);
DELETE FROM `creature_battleground` WHERE `guid` BETWEEN 701001 AND 701016 OR `guid` BETWEEN 701027 AND 701030;
INSERT INTO `creature_battleground` VALUES
(701001, 19, 0),
(701002, 18, 0),
(701003, 7, 0),
(701004, 7, 0),
(701005, 7, 0),
(701006, 7, 0),
(701007, 7, 0),
(701008, 7, 0),
(701009, 7, 0),
(701010, 7, 0),
(701011, 7, 0),
(701012, 7, 0),
(701013, 17, 0),
(701014, 17, 0),
(701015, 16, 0),
(701016, 16, 0),
(701027, 9, 0),
(701028, 9, 0),
(701029, 10, 0),
(701030, 10, 0);
-- Horde and Alliance Spirit Guide
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`) VALUES
(701017, 13117, 607, 3, 1448.78, -52.9605, 5.74272, 0.0530517),
(701018, 13116, 607, 3, 1448.78, -52.9605, 5.74272, 0.0530517),
(701019, 13117, 607, 3, 1389.22, 205.673, 32.0597, 4.445),
(701020, 13116, 607, 3, 1389.22, 205.673, 32.0597, 4.445),
(701021, 13117, 607, 3, 1400.49, -293.963, 32.0608, 2.08096),
(701022, 13116, 607, 3, 1400.49, -293.963, 32.0608, 2.08096),
(701023, 13117, 607, 3, 1111.58, 6.40605, 69.2963, 6.20037),
(701024, 13116, 607, 3, 1111.58, 6.40605, 69.2963, 6.20037),
(701025, 13117, 607, 3, 952.963, -192.742, 92.3584, 0.35779),
(701026, 13116, 607, 3, 952.963, -192.742, 92.3584, 0.35779);
DELETE FROM `creature_battleground` WHERE `guid` BETWEEN 701017 AND 701026;
INSERT INTO `creature_battleground` VALUES
(701017, 5, 0),
(701018, 5, 1),
(701025, 5, 1),
(701026, 5, 0),
(701019, 0, 1),
(701020, 0, 0),
(701021, 1, 1),
(701022, 1, 0),
(701023, 2, 1),
(701024, 2, 0);

DELETE FROM `mangos_string` WHERE `entry` BETWEEN 20000 AND 20030;
INSERT INTO `mangos_string` (`entry`, `content_default`, `content_loc8`) VALUES
(20000, 'The battle for Strand of the Ancients begins in 2 minutes.', 'Битва за Берег Древних начнется через 2 минуты'),
(20001, 'The battle for Strand of the Ancients begins in 1 minute.', 'Битва за Берег Древних начнется через 1 минуту'),
(20002, 'The battle for Strand of the Ancients begins in 30 seconds. Prepare yourselves!', 'Битва за Берег Древних начнется через 30 секунд. Приготовьтесь!'),
(20003, 'Let the battle for Strand of the Ancients begin!', 'Пусть сражение за Берег Древних начнется!'),

(20004, 'Round 2 of the Battle for the Strand of the Ancients begins in 2 minutes.', 'Второй раунд сражения за Берег Древних начнется через 2 минуты.'),
(20005, 'Round 2 of the Battle for the Strand of the Ancients begins in 1 minute.', 'Второй раунд сражения за Берег Древних начнется через 1 минуту.'),
(20006, 'Round 2 begins in 30 seconds. Prepare yourselves!', 'Второй раунд начнется через 30 секунд. Приготовьтесь!'),

(20007, 'The keep is under attack!', 'Крепость атакуют!'),
(20008, 'Emerald Green Door is under attack!', 'Врата Зеленого Изумруда подверглись нападению!'),
(20009, 'Sapphire Blue Door is under attack!', 'Врата Синего Сапфира подверглись нападению!'),
(20010, 'Amethyst Purple Door is under attack!', 'Врата Лилового Аметиста подверглись нападению!'),
(20011, 'Red Sun Door is under attack!', 'Врата Красного Солнца подверглись нападению!'),
(20012, 'Yellow Moon Door is under attack!', 'Врата Желтой Луны подверглись нападению!'),

(20013, 'The Horde has taken East Graveyard!', 'Восточное кладбище захвачено Ордой!'),
(20014, 'The Horde has taken West Graveyard!', 'Западное кладбище захвачено Ордой!'),
(20015, 'The Horde has taken South Graveyard!', 'Южное кладбище захвачено Ордой!'),
(20016, 'The Alliance has taken East Graveyard!', 'Восточное кладбище захвачено Альянсом!'),
(20017, 'The Alliance has taken West Graveyard!', 'Западное кладбище захвачено Альянсом!'),
(20018, 'The Alliance has taken South Graveyard!', 'Южное кладбище захвачено Альянсом!'),

(20019, 'The chamber has been breached! The titan relic is vulnerable!', 'В стене пролом! Реликвии грозит опасность!'),
(20020, 'Emerald Green Door has been destroyed!', 'Врата Зеленого Изумруда разрушены!'),
(20021, 'Sapphire Blue Door has been destroyed!', 'Врата Синего Сапфира разрушены!'),
(20022, 'Amethyst Purple Door has been destroyed!', 'Врата Лилового Аметиста разрушены!'),
(20023, 'Red Sun Door has been destroyed!', 'Врата Красного Солнца разрушены!'),
(20024, 'Yellow Moon Door has been destroyed!', 'Врата Желтой Луны разрушены!'),

(20025, 'The Alliance has captured the titan portal!', 'Альянс захватил портал титанов!'),
(20026, 'The Horde has captured the titan portal!', 'Орда захватила портал титанов!'),

(20027, 'Round 1 - finished!', 'Раунд 1 - завершен!'),
(20028, 'The battle for the Strand of the Ancients has ended!', 'Сражение за Берег Древних завершилось!'),

(20029, 'Sparklight''s vehicle emporium is open for business!', 'Компания Искросвета ждет деловых предложений!'),
(20030, 'Rigspark''s vehicle shop is open for business!', 'Магазин Такелажника ждет клиентов!');

INSERT INTO `gameobject` (`guid`, `id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
(701008, 180058, 607, 3, 1337.23, -157.79, 30.8947, 3.51587, 0, 0, 0, 0, 86400, 100, 1), -- Alliance Banner East
(701010, 180060, 607, 3, 1337.23, -157.79, 30.8947, 3.51587, 0, 0, 0, 0, 86400, 100, 1), -- Horde Banner East
(701012, 180100, 607, 3, 1337.23, -157.79, 30.8947, 3.51587, 0, 0, 0, 0, 86400, 100, 1), -- Alliance Banner Aura East
(701013, 180101, 607, 3, 1337.23, -157.79, 30.8947, 3.51587, 0, 0, 0, 0, 86400, 100, 1), -- Horde Banner Aura East
(701014, 191311, 607, 3, 1337.23, -157.79, 30.8947, 3.51587, 0, 0, 0, 0, 86400, 100, 1), -- Flagpole East

(701015, 180058, 607, 3, 1315.55, 11.8483, 31.2072, 2.85221, 0, 0, 0, 0, 86400, 100, 1), -- Alliance Banner West
(701017, 180060, 607, 3, 1315.55, 11.8483, 31.2072, 2.85221, 0, 0, 0, 0, 86400, 100, 1), -- Horde Banner West
(701019, 180100, 607, 3, 1315.55, 11.8483, 31.2072, 2.85221, 0, 0, 0, 0, 86400, 100, 1), -- Alliance Banner Aura West
(701020, 180101, 607, 3, 1315.55, 11.8483, 31.2072, 2.85221, 0, 0, 0, 0, 86400, 100, 1), -- Horde Banner Aura West
(701021, 191311, 607, 3, 1315.55, 11.8483, 31.2072, 2.85221, 0, 0, 0, 0, 86400, 100, 1), -- Flagpole West

(701022, 180058, 607, 3, 1206.8, -68.2148, 70.0845, 3.14279, 0, 0, 0, 0, 86400, 100, 1), -- Alliance Banner South
(701024, 180060, 607, 3, 1206.8, -68.2148, 70.0845, 3.14279, 0, 0, 0, 0, 86400, 100, 1), -- Horde Banner South
(701026, 180100, 607, 3, 1206.8, -68.2148, 70.0845, 3.14279, 0, 0, 0, 0, 86400, 100, 1), -- Alliance Banner Aura South
(701027, 180101, 607, 3, 1206.8, -68.2148, 70.0845, 3.14279, 0, 0, 0, 0, 86400, 100, 1), -- Horde Banner Aura South
(701028, 191311, 607, 3, 1206.8, -68.2148, 70.0845, 3.14279, 0, 0, 0, 0, 86400, 100, 1); -- Flagpole South

DELETE FROM `gameobject_battleground` WHERE `guid` BETWEEN 701008 AND 701171;
INSERT INTO `gameobject_battleground` VALUES
(701008, 1, 0),
(701010, 1, 1),
(701012, 1, 0),
(701013, 1, 1),
(701014, 8, 0),

(701015, 0, 0),
(701017, 0, 1),
(701019, 0, 0),
(701020, 0, 1),
(701021, 8, 0),

(701022, 2, 0),
(701024, 2, 1),
(701026, 2, 0),
(701027, 2, 1),
(701028, 8, 0),

(701029, 8, 0),
(701030, 8, 0),
(701031, 8, 0),
(701032, 8, 0),
(701033, 8, 0),

-- Beach Alliance
(701052, 6, 0),
(701053, 6, 0),
(701054, 6, 0),
(701055, 6, 0),
(701056, 6, 0),
(701057, 6, 0),
(701058, 6, 0),
(701059, 6, 0),
(701060, 6, 0),
(701061, 6, 0),
(701062, 6, 0),
(701063, 6, 0),
(701064, 6, 0),
(701065, 6, 0),
(701066, 6, 0),
(701067, 6, 0),
(701068, 6, 0),
(701069, 6, 0),
(701070, 6, 0),
(701071, 6, 0),
(701072, 6, 0),
(701073, 6, 0),
(701074, 6, 0),

-- Green Alliance
(701098, 11, 0),
(701099, 11, 0), 
(701100, 11, 0),
(701101, 11, 0),
(701102, 11, 0),
(701103, 11, 0),
(701104, 11, 0),
(701105, 11, 0),
(701106, 11, 0),
(701107, 11, 0),

-- Blue Alliance
(701118, 12, 0),
(701119, 12, 0),
(701120, 12, 0),
(701121, 12, 0),
(701122, 12, 0),
(701123, 12, 0),
(701124, 12, 0),

-- Purple Alliance
(701132, 13, 0),
(701133, 13, 0),
(701134, 13, 0),
(701135, 13, 0),
(701136, 13, 0),
(701137, 13, 0),

-- Red Alliance
(701144, 14, 0),
(701145, 14, 0),
(701146, 14, 0),
(701147, 14, 0),
(701148, 14, 0),
(701149, 14, 0),

-- Yellow Alliance
(701156, 15, 0),
(701157, 15, 0),
(701158, 15, 0),
(701159, 15, 0),
(701160, 15, 0),
(701161, 15, 0),
(701162, 15, 0);

-- Defender's Portal
INSERT INTO `gameobject` (`guid`, `id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
(701029, 191575, 607, 3, 1468.12, -225.7, 30.8969, 5.68042, 0, 0, 0, 0, 25, 0, 1),
(701030, 191575, 607, 3, 1394.07, 72.3632, 31.0541, 0.818809, 0, 0, 0, 0, 25, 0, 1),
(701031, 191575, 607, 3, 1216.12, 47.7665, 54.2785, 4.05465, 0, 0, 0, 0, 25, 0, 1),
(701032, 191575, 607, 3, 1255.73, -233.153, 56.4357, 5.01833, 0, 0, 0, 0, 25, 0, 1),
(701033, 191575, 607, 3, 1065.02, -89.9522, 81.0758, 1.58771, 0, 0, 0, 0, 25, 0, 1);

-- Portal spell targets
UPDATE `creature_template` SET `faction_A` = 35, `faction_H` = 35 WHERE `entry` = 23472;
INSERT INTO `creature` (`guid`, `id`, `map`, `spawnMask`, `phaseMask`, `position_x`, `position_y`, `position_z`, `orientation`) VALUES
(701031, 23472, 607, 3, 65535, 1468.12, -225.7, 30.8969, 0),
(701032, 23472, 607, 3, 65535, 1394.07, 72.3632, 31.0541, 0),
(701033, 23472, 607, 3, 65535, 1216.12, 47.7665, 54.2785, 0),
(701034, 23472, 607, 3, 65535, 1255.73, -233.153, 56.4357, 0),
(701035, 23472, 607, 3, 65535, 1065.02, -89.9522, 81.0758, 0);

-- Seaforium Bomb
INSERT INTO `gameobject` (`guid`, `id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
-- Beach East
(701052, 190753, 607, 3, 1583.28, -104.72, 8.45841, 4.11749, 0, 0, 0.883295, -0.468817, 25, 0, 1),
(701053, 190753, 607, 3, 1584.26, -104.55, 8.45841, 0.05794, 0, 0, 0.883295, -0.468817, 25, 0, 1),
(701054, 190753, 607, 3, 1583.75, -104.04, 8.45841, 0.55711, 0, 0, 0.879586, -0.475739, 25, 0, 1),
(701055, 190753, 607, 3, 1583.70, -104.37, 9.0199, 0.73950, 0, 0, 0.879586, -0.475739, 25, 0, 1),

(701056, 190753, 607, 3, 1581.54, -101.63, 8.45841, 3.64, 0, 0, 0.882004, -0.471242, 25, 0, 1),
(701057, 190753, 607, 3, 1581.27, -99.52, 8.45695, 2.82, 0, 0, 0.882004, -0.471242, 25, 0, 1),

(701058, 190753, 607, 3, 1586.63, -93.14, 8.45554, 4.11749, 0, 0, 0.883295, -0.468817, 25, 0, 1),
(701059, 190753, 607, 3, 1587.61, -92.97, 8.45554, 0.05794, 0, 0, 0.883295, -0.468817, 25, 0, 1),
(701060, 190753, 607, 3, 1587.1, -92.46, 8.45554, 0.55711, 0, 0, 0.879586, -0.475739, 25, 0, 1),
(701061, 190753, 607, 3, 1587.05, -92.79, 9.01703, 0.7395, 0, 0, 0.879586, -0.475739, 25, 0, 1),

(701062, 190753, 607, 3, 1593.867676, -95.207649, 8.930139, 5.148026, 0, 0, 0.879586, -0.475739, 25, 0, 1),
-- Beach West
(701063, 190753, 607, 3, 1592.038208, 47.215462, 7.54, 4.697, 0, 0, 0.926792, 0.375575, 25, 0, 1),
(701064, 190753, 607, 3, 1593.074097, 47.386536, 7.54, 4.874, 0, 0, 0.931866, 0.362801, 25, 0, 1),
(701065, 190753, 607, 3, 1594.055908, 47.542202, 7.54, 5.21, 0, 0, 0.931866, 0.362801, 25, 0, 1),
(701066, 190753, 607, 3, 1593.730103, 46.354111, 7.54, 4.43297, 0, 0, 0.932577, 0.360971, 25, 0, 1),
(701067, 190753, 607, 3, 1592.452515, 46.513336, 7.54, 4.55192, 0, 0, 0.92306, 0.384655, 25, 0, 1),
(701068, 190753, 607, 3, 1588.786377, 34.616062, 7.16443, 4.17941, 0, 0, 0.92826, 0.371932, 25, 0, 1),
(701069, 190753, 607, 3, 1588.619507, 36.450588, 7.16443, 4.73941, 0, 0, 0.92826, 0.371932, 25, 0, 1),

(701070, 190753, 607, 3, 1598.302979, 36.280224, 7.16443, 4.47941, 0, 0, 0.92826, 0.371932, 25, 0, 1),
(701071, 190753, 607, 3, 1597.302124, 36.842915, 7.16443, 4.71941, 0, 0, 0.92826, 0.371932, 25, 0, 1),
(701072, 190753, 607, 3, 1596.201660, 36.546272, 7.16443, 4.37941, 0, 0, 0.92826, 0.371932, 25, 0, 1),

(701073, 190753, 607, 3, 1596.306396, 35.674969, 7.16443, 2.28941, 0, 0, 0.92826, 0.371932, 25, 0, 1),
(701074, 190753, 607, 3, 1597.718384, 35.938400, 7.66443, 2.27941, 0, 0, 0.92826, 0.371932, 25, 0, 1),

-- Green
(701098, 190753, 607, 3, 1370.324585, 196.551956, 30.935431, 4.697, 0, 0, 0.926792, 0.375575, 25, 0, 1),
(701099, 190753, 607, 3, 1370.241089, 197.760376, 30.935431, 6.248453, 0, 0, 0.931866, 0.362801, 25, 0, 1),
(701100, 190753, 607, 3, 1369.273560, 197.193878, 30.935431, 0.046862, 0, 0, 0.931866, 0.362801, 25, 0, 1),
(701101, 190753, 607, 3, 1371.252441, 197.286682, 30.935431, 0.046, 0, 0, 0.932577, 0.360971, 25, 0, 1),
(701102, 190753, 607, 3, 1374.097900, 195.987305, 31.524376, 0.876679, 0, 0, 0.92306, 0.384655, 25, 0, 1),
(701103, 190753, 607, 3, 1373.698242, 194.413696, 31.839048, 0.886279, 0, 0, 0.92826, 0.371932, 25, 0, 1),

(701104, 190753, 607, 3, 1335.242920, 212.090393, 30.995716, 1.155060, 0, 0, 0.92826, 0.371932, 25, 0, 1),
(701105, 190753, 607, 3, 1333.740601, 211.051025, 31.051619, 1.956166, 0, 0, 0.92826, 0.371932, 25, 0, 1),
(701106, 190753, 607, 3, 1335.969604, 210.753418, 31.019062, 1.034196, 0, 0, 0.92826, 0.371932, 25, 0, 1),
(701107, 190753, 607, 3, 1334.065063, 209.280823, 31.053814, 4.873221, 0, 0, 0.92826, 0.371932, 25, 0, 1),

-- Blue
(701118, 190753, 607, 3, 1340.376465, -306.640839, 30.956491, 4.342641, 0, 0, 0.926792, 0.375575, 25, 0, 1),
(701119, 190753, 607, 3, 1339.068237, -305.234009, 30.937275, 3.483503, 0, 0, 0.931866, 0.362801, 25, 0, 1),
(701120, 190753, 607, 3, 1338.374268, -303.105438, 30.901798, 3.075533, 0, 0, 0.931866, 0.362801, 25, 0, 1),

(701121, 190753, 607, 3, 1352.307373, -302.273041, 30.929461, 3.965175, 0, 0, 0.932577, 0.360971, 25, 0, 1),
(701122, 190753, 607, 3, 1353.041382, -301.150757, 30.911558, 4.320348, 0, 0, 0.92306, 0.384655, 25, 0, 1),
(701123, 190753, 607, 3, 1351.548218, -301.025513, 30.936314, 3.542804, 0, 0, 0.92826, 0.371932, 25, 0, 1),
(701124, 190753, 607, 3, 1352.494995, -299.972656, 30.918425, 0.706223, 0, 0, 0.92826, 0.371932, 25, 0, 1),

-- Purple
(701132, 190753, 607, 3, 1096.439331, 1.359248, 70.196198, 3.624045, 0, 0, 0.926792, 0.375575, 25, 0, 1),
(701133, 190753, 607, 3, 1097.976807, 0.693974, 70.138557, 3.619245, 0, 0, 0.931866, 0.362801, 25, 0, 1),
(701134, 190753, 607, 3, 1096.697998, 0.159275, 70.245857, 3.537652, 0, 0, 0.931866, 0.362801, 25, 0, 1),
(701135, 190753, 607, 3, 1098.384399, -0.714502, 70.261620, 3.537652, 0, 0, 0.932577, 0.360971, 25, 0, 1),

(701136, 190753, 607, 3, 1093.925171, -4.205487, 70.759872, 4.615394, 0, 0, 0.92306, 0.384655, 25, 0, 1),
(701137, 190753, 607, 3, 1093.925171, -4.205487, 70.759872, 4.615394, 0, 0, 0.92826, 0.371932, 25, 0, 1),

-- Red
(701144, 190753, 607, 3, 1094.586792, -6.722581, 70.820816, 4.135604, 0, 0, 0.92826, 0.371932, 25, 0, 1),
(701145, 190753, 607, 3, 1096.048462, -7.574701, 70.807709, 5.978671, 0, 0, 0.92826, 0.371932, 25, 0, 1),

(701146, 190753, 607, 3, 1097.053589, -11.828032, 71.080185, 3.857224, 0, 0, 0.92826, 0.371932, 25, 0, 1),
(701147, 190753, 607, 3, 1098.486694, -11.939654, 71.269783, 4.045372, 0, 0, 0.92826, 0.371932, 25, 0, 1),
(701148, 190753, 607, 3, 1098.407471, -9.854460, 71.009056, 3.277427, 0, 0, 0.92826, 0.371932, 25, 0, 1),
(701149, 190753, 607, 3, 1100.043457, -10.433746, 1.94, 71.205605, 0, 0, 0.92826, 0.371932, 25, 0, 1),

-- Yellow
(701156, 190753, 607, 3, 968.194824, -3.428298, 86.961067, 2.727828, 0, 0, 0.92826, 0.371932, 25, 0, 1),
(701157, 190753, 607, 3, 967.326904, -3.403672, 86.991386, 2.327828, 0, 0, 0.92826, 0.371932, 25, 0, 1),
(701158, 190753, 607, 3, 968.483276, -2.244385, 86.949226, 3.857224, 0, 0, 0.92826, 0.371932, 25, 0, 1),
(701159, 190753, 607, 3, 968.588196, -3.294916, 86.946976, 0.639745, 0, 0, 0.92826, 0.371932, 25, 0, 1),

(701160, 190753, 607, 3, 973.005615, 0.572900, 86.818848, 2.627589, 0, 0, 0.92826, 0.371932, 25, 0, 1),
(701161, 190753, 607, 3, 974.614441, 3.053979, 86.815460, 0.555359, 0, 0, 0.92826, 0.371932, 25, 0, 1),
(701162, 190753, 607, 3, 975.782959, 1.613286, 86.780426, 5.393849, 0, 0, 0.92826, 0.371932, 25, 0, 1);

-- Buffs
INSERT INTO `gameobject` (`guid`, `id`, `map`, `spawnMask`, `position_x`, `position_y`, `position_z`, `orientation`, `rotation0`, `rotation1`, `rotation2`, `rotation3`, `spawntimesecs`, `animprogress`, `state`) VALUES
(701172, 179904, 607, 3, 1367.997681, 219.862671, 37.068218, 4.379767, 0, 0, 0, 0, 180, 100, 1),
(701173, 179904, 607, 3, 1357.883545, -324.820709, 36.830833, 1.960740, 0, 0, 0, 0, 180, 100, 1),
(701174, 179904, 607, 3, 982.089417, -4.904296, 86.692604, 2.116946, 0, 0, 0, 0, 180, 100, 1);

UPDATE `creature_template` SET ScriptName = 'npc_sa_cannon' WHERE `entry` IN (27894, 32795);
UPDATE `creature_template` SET ScriptName = 'npc_sa_demolisher' WHERE `entry` IN (28781, 32796);
UPDATE `creature_template` SET ScriptName = '' WHERE `entry` IN (29260, 29262);
UPDATE `creature_template` SET ScriptName = 'npc_sa_bomb' WHERE `entry` = 50000;
UPDATE `gameobject_template` SET ScriptName = 'go_def_portal' WHERE `entry` = 191575;

DELETE FROM `achievement_criteria_requirement` WHERE `criteria_id` IN (7636, 7740, 8758, 8759);
INSERT INTO `achievement_criteria_requirement` VALUES
(7636, 0, 0, 0),
(7740, 0, 0, 0),
(8758, 0, 0, 0),
(8759, 0, 0, 0);

DELETE FROM `playercreateinfo_spell` WHERE `Spell` = 1843;
INSERT INTO `playercreateinfo_spell` VALUES
(1, 1, 1843, 'Disarm'),
(1, 2, 1843, 'Disarm'),
(1, 4, 1843, 'Disarm'),
(1, 5, 1843, 'Disarm'),
(1, 6, 1843, 'Disarm'),
(1, 8, 1843, 'Disarm'),
(1, 9, 1843, 'Disarm'),
(2, 1, 1843, 'Disarm'),
(2, 3, 1843, 'Disarm'),
(2, 4, 1843, 'Disarm'),
(2, 6, 1843, 'Disarm'),
(2, 7, 1843, 'Disarm'),
(2, 9, 1843, 'Disarm'),
(3, 1, 1843, 'Disarm'),
(3, 2, 1843, 'Disarm'),
(3, 3, 1843, 'Disarm'),
(3, 4, 1843, 'Disarm'),
(3, 5, 1843, 'Disarm'),
(3, 6, 1843, 'Disarm'),
(4, 1, 1843, 'Disarm'),
(4, 3, 1843, 'Disarm'),
(4, 4, 1843, 'Disarm'),
(4, 5, 1843, 'Disarm'),
(4, 6, 1843, 'Disarm'),
(4, 11, 1843, 'Disarm'),
(5, 1, 1843, 'Disarm'),
(5, 4, 1843, 'Disarm'),
(5, 5, 1843, 'Disarm'),
(5, 6, 1843, 'Disarm'),
(5, 8, 1843, 'Disarm'),
(5, 9, 1843, 'Disarm'),
(6, 1, 1843, 'Disarm'),
(6, 3, 1843, 'Disarm'),
(6, 6, 1843, 'Disarm'),
(6, 7, 1843, 'Disarm'),
(6, 11, 1843, 'Disarm'),
(7, 1, 1843, 'Disarm'),
(7, 4, 1843, 'Disarm'),
(7, 6, 1843, 'Disarm'),
(7, 8, 1843, 'Disarm'),
(7, 9, 1843, 'Disarm'),
(8, 1, 1843, 'Disarm'),
(8, 3, 1843, 'Disarm'),
(8, 4, 1843, 'Disarm'),
(8, 5, 1843, 'Disarm'),
(8, 6, 1843, 'Disarm'),
(8, 7, 1843, 'Disarm'),
(8, 8, 1843, 'Disarm'),
(10, 6, 1843, 'Disarm'),
(11, 1, 1843, 'Disarm'),
(11, 2, 1843, 'Disarm'),
(11, 3, 1843, 'Disarm'),
(11, 5, 1843, 'Disarm'),
(11, 6, 1843, 'Disarm'),
(11, 7, 1843, 'Disarm'),
(11, 8, 1843, 'Disarm');