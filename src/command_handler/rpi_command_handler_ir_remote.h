/**
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * @file   rpi_command_handler_ir_remote.h
 * @author Sebastian Lesse
 * @date   2020 / 03 / 31
 * @brief  This file contains definitions that are used by the ir command-handler
 * 
 */

#ifndef _RPI_COMMAND_HANDLER_IR_REMOTE_H_
#define _RPI_COMMAND_HANDLER_IR_REMOTE_H_

// --------------------------------------------------------------------------------

#include "command_management/protocol_interface.h"

// --------------------------------------------------------------------------------

#define IR_MANUFACTOR_SAMSUNG		0x01
#define IR_MANUFACTOR_JVC		0x02
#define IR_MANUFACTOR_SONY		0x03
#define IR_MANUFACTOR_LED_LIGHTS	0x04

#define IR_DEVICE_TV			0x01
#define IR_DEVICE_RADIO			0x02
#define IR_DEVICE_BLUE_RAY_PLAYER	0x03

#define IR_DEVICE_LED_LIGHT_BAUHAUS     0xF1

#define IR_COMMAND_POWER_ON		0x01
#define IR_COMMAND_POWER_OFF		0x02

#define IR_COMMAND_VOLUME_UP		0x03
#define IR_COMMAND_VOLUME_DOWN		0x04

#define IR_COMMAND_CHANNEL_UP		0x05
#define IR_COMMAND_CHANNEL_DOWN		0x06

#define IR_COMMAND_CHANNEL_0		0x07
#define IR_COMMAND_CHANNEL_1		0x08
#define IR_COMMAND_CHANNEL_2		0x09
#define IR_COMMAND_CHANNEL_3		0x0A
#define IR_COMMAND_CHANNEL_4		0x0B
#define IR_COMMAND_CHANNEL_5		0x0C
#define IR_COMMAND_CHANNEL_6		0x0D
#define IR_COMMAND_CHANNEL_7		0x0E
#define IR_COMMAND_CHANNEL_8		0x0F
#define IR_COMMAND_CHANNEL_9		0x10

#define IR_COMMAND_ENTER		0x11
#define IR_COMMAND_EXIT			0x12
#define IR_COMMAND_RETURN		0x13
#define IR_COMMAND_ARROW_UP		0x14
#define IR_COMMAND_ARROW_DOWN		0x15
#define IR_COMMAND_ARROW_LEFT		0x16
#define IR_COMMAND_ARROW_RIGHT		0x17

#define IR_COMMAND_PLAY			0x18
#define IR_COMMAND_PAUSE		0x19
#define IR_COMMAND_STOP			0x1A

#define IR_COMMAND_BASS_UP		0x1B
#define IR_COMMAND_BASS_DOWN		0x1C
#define IR_COMMAND_SOUND_MODE		0x1D

#define IR_COMMAND_MUTE			0x1E

#define IR_COMMAND_PROGRAM_GUIDE	0x1F
#define IR_COMMAND_HOME			0x20
#define IR_COMMAND_CHANNEL_LIST		0x21
#define IR_COMMAND_SOURCE		0x22

#define IR_COMMAND_AUDIO_LANGUAGE	0x23
#define IR_COMMAND_SUBTITLE_LANGUAGE	0x24
#define IR_COMMAND_EJECT		0x25
#define IR_COMMAND_TOP_MENU		0x26
#define IR_COMMAND_POP_UP_MENU		0x27
#define IR_COMMAND_NEXT			0x28
#define IR_COMMAND_PREVIOUS		0x29

#define IR_COMMAND_LIGHT_BRIGHTER       0x2A
#define IR_COMMAND_LIGHT_DARKER         0x2B
#define IR_COMMAND_LIGHT_FLASH          0x2C
#define IR_COMMAND_LIGHT_STROBE         0x2D
#define IR_COMMAND_LIGHT_FADE           0x2E
#define IR_COMMAND_LIGHT_SMOOTH         0x2F
#define IR_COMMAND_LIGHT_WHITE          0x30
#define IR_COMMAND_LIGHT_RED            0x31
#define IR_COMMAND_LIGHT_RED_INDIAN     0x32
#define IR_COMMAND_LIGHT_COPPER         0x33
#define IR_COMMAND_LIGHT_ORANGE         0x34
#define IR_COMMAND_LIGHT_YELLOW         0x35
#define IR_COMMAND_LIGHT_GREEN_FOREST   0x36
#define IR_COMMAND_LIGHT_GREEN          0x37
#define IR_COMMAND_LIGHT_BLUE_AZURE     0x38
#define IR_COMMAND_LIGHT_BLUE_STEEL     0x39
#define IR_COMMAND_LIGHT_JADE           0x3A
#define IR_COMMAND_LIGHT_BLUE           0x3B
#define IR_COMMAND_LIGHT_BLUE_MARINE    0x3C
#define IR_COMMAND_LIGHT_EGGPLANT       0x3D
#define IR_COMMAND_LIGHT_PURPLE         0x3E
#define IR_COMMAND_LIGHT_PINK           0x3F

// --------------------------------------------------------------------------------

u8 rpi_cmd_handler_ir_remote(PROTOCOL_INTERFACE* p_protocol);

// --------------------------------------------------------------------------------

#endif // _RPI_COMMAND_HANDLER_IR_REMOTE_H_
