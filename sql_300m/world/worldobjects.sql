-- Wintergrasp door remove
-- DELETE FROM `gameobject` WHERE `id` = 191810;
-- DELETE FROM `gameobject` WHERE `guid` in (73042, 1695);
-- +-------+--------+-----+-----------+-----------+------------+------------+------------+-------------+-----------+-----------+-----------+--------------+---------------+--------------+-------+
-- | guid  | id     | map | spawnMask | phaseMask | position_x | position_y | position_z | orientation | rotation0 | rotation1 | rotation2 | rotation3    | spawntimesecs | animprogress | state |
-- +-------+--------+-----+-----------+-----------+------------+------------+------------+-------------+-----------+-----------+-----------+--------------+---------------+--------------+-------+
-- | 51165 | 191810 | 571 |         1 |         7 |    5397.11 |    2841.54 |    425.901 |     3.14159 |         0 |         0 |         1 | 1.26759e-006 |           300 |            0 |     1 |
-- +-------+--------+-----+-----------+-----------+------------+------------+------------+-------------+-----------+-----------+-----------+--------------+---------------+--------------+-------+

-- Trial of the Champion door
DELETE FROM `gameobject` WHERE `id` = 195591 AND `map` = 571;
-- +-------+--------+-----+-----------+-----------+------------+------------+------------+-------------+-----------+-----------+-----------+------------+---------------+--------------+-------+
-- | guid  | id     | map | spawnMask | phaseMask | position_x | position_y | position_z | orientation | rotation0 | rotation1 | rotation2 | rotation3  | spawntimesecs | animprogress | state |
-- +-------+--------+-----+-----------+-----------+------------+------------+------------+-------------+-----------+-----------+-----------+------------+---------------+--------------+-------+
-- |  1804 | 195591 | 571 |         1 |         1 |    8570.05 |    792.456 |    558.338 |     3.14159 |         0 |         0 |  0.707107 |   0.707107 |           300 |          100 |     1 |
-- +-------+--------+-----+-----------+-----------+------------+------------+------------+-------------+-----------+-----------+-----------+------------+---------------+--------------+-------+
