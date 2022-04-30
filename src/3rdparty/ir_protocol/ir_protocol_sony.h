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
 * @file   ir_protocol_sony.h
 * @author Sebastian Lesse
 * @date   2021 / 03 / 20
 * @brief  Implementation of the SONY ir-protocol
 * 
 *  Frequency:      40 kHz
 * 
 *  Preamble:       2440us Pulse
 * 
 *  Logical-One:    600us Pulse
 *                  1200us Pause
 * 
 *  Logical Zero:   600us Pulse
 *                  600us Pause
 * 
 *  Stop-Bit:       560us Pulse
 * 
 *  Usage:
 * 
 *  The IR-protocl module uses the IR-Protocol-Interface.
 *  By calling ir_protocol_sony_init() a new ir-protocol will
 *  be registered. The ID of this procotol is set to 0x03
 * 
 *  @see 3rdparty/ir_protocol/ir_protocol_interface.h
 * 
 * 
 */

#ifndef _IR_PROTOCOL_SONY_H_
#define _IR_PROTOCOL_SONY_H_

// --------------------------------------------------------------------------------

#include "driver/timer/timer_interface.h"

// --------------------------------------------------------------------------------

#include "3rdparty/ir_protocol/ir_protocol_interface.h"

// --------------------------------------------------------------------------------

/**
 * @brief Initialized the SONY ir-protocol.
 * Registers a new ir-protocol to the ir-protocol-interface
 * 
 */
void ir_protocol_sony_init(void);

// --------------------------------------------------------------------------------

/**
 * @brief Sets the Address of the SOny BD900 Blueray Disc-Player.
 * 
 * @param p_command COmmand where the Device-Address ist set
 */
void ir_protocol_sony_address_bdplayer(IR_COMMON_COMMAND_TYPE* p_command);

// --------------------------------------------------------------------------------

/**
 * @brief Command to Power On/Off the Sony Blueray Dis-Player
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_power(IR_COMMON_COMMAND_TYPE* p_command);

/**
 * @brief Command to start playing the actual disc
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_play(IR_COMMON_COMMAND_TYPE* p_command);

/**
 * @brief Command to stop playing the actual disc
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_stop(IR_COMMON_COMMAND_TYPE* p_command);

/**
 * @brief Command to pause playing the actual disc
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_pause(IR_COMMON_COMMAND_TYPE* p_command);

/**
 * @brief Command to change the actual audio language
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_audio_language(IR_COMMON_COMMAND_TYPE* p_command);

/**
 * @brief Command to change the actual subtitle language
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_subtitle_language(IR_COMMON_COMMAND_TYPE* p_command);

/**
 * @brief Command to eject tha actual blueray disc
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_eject(IR_COMMON_COMMAND_TYPE* p_command);

/**
 * @brief Command to return to the previous menu page
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_return(IR_COMMON_COMMAND_TYPE* p_command);

/**
 * @brief Command to enter the top menu of the actual media disc
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_top_menu(IR_COMMON_COMMAND_TYPE* p_command);

/**
 * @brief Command to activate the pop-up menu of the actual media disc
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_pop_up_menu(IR_COMMON_COMMAND_TYPE* p_command);

/**
 * @brief Command to go to the next chapter
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_next(IR_COMMON_COMMAND_TYPE* p_command);

/**
 * @brief Command to go to the previous chapter
 * 
 * @param p_command pointer to the command where the data is updated
 */
void ir_protocol_sony_cmd_bdplayer_previous(IR_COMMON_COMMAND_TYPE* p_command);

// --------------------------------------------------------------------------------

#endif // _IR_PROTOCOL_SONY_H_
