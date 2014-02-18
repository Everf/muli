ALTER TABLE db_version CHANGE COLUMN required_9967_01_mangos_spell_proc_event required_9977_01_mangos_spell_proc_event bit;

DELETE FROM `spell_proc_event` WHERE `entry` IN (16180,16196,16198);
INSERT INTO `spell_proc_event` VALUES
(16180,0x00000000,11,0x000001C0,0x00000000,0x00000010,0x00000000,0x00000002,0.000000,0.000000, 0),
(16196,0x00000000,11,0x000001C0,0x00000000,0x00000010,0x00000000,0x00000002,0.000000,0.000000, 0),
(16198,0x00000000,11,0x000001C0,0x00000000,0x00000010,0x00000000,0x00000002,0.000000,0.000000, 0);
