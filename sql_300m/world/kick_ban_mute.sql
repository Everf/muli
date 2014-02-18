UPDATE `command` SET `help` = 'Syntax: .kick [$charactername] [$reason]\r\n\r\nKick the given character name from the world. If no character name is provided then the selected player (except for yourself) will be kicked. If no [$reason] is provided, command will fail.' WHERE `name` = 'kick';
UPDATE `command` SET `help` = 'Syntax: .mute [$playerName] $timeInMinutes [$reason]\r\n\r\nDisable chat messaging for any character from account of character $playerName (or currently selected) at $timeInMinutes minutes. Player can be offline. If no [$reason] is provided, command will fail.' WHERE `name` = 'mute';
