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
 * @file    ir_protocol_jvc.h
 * @author  Sebastian Lesse
 * @date    2022 / 12 / 19
 * @brief   Short description of this file
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_ir_protocol_jvc_
#define _H_ir_protocol_jvc_

// --------------------------------------------------------------------------------

#include "driver/timer/timer_interface.h"

// --------------------------------------------------------------------------------

#include "modules/ir/ir_protocol_interface.h"

// --------------------------------------------------------------------------------

/**
 * @brief Initialized the JVC ir-protocol.
 * Registers a new ir-protocol to the ir-protocol-interface
 * 
 */
void ir_protocol_jvc_init(void);

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param p_ir_command 
 */
void ir_command_jvc_protocol_type(IR_COMMON_COMMAND_TYPE* p_ir_command);

// --------------------------------------------------------------------------------

/*
 *
 */
void ir_protocol_jvc_address_tv(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_jvc_address_radio(IR_COMMON_COMMAND_TYPE* p_command);

// --------------------------------------------------------------------------------

/*
 *
 */
void ir_protocol_jvc_cmd_radio_power(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_jvc_cmd_radio_volume_up(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_jvc_cmd_radio_volume_down(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_jvc_cmd_radio_volume_mute(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_jvc_cmd_radio_channel_down(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_jvc_cmd_radio_play(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_jvc_cmd_radio_pause(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_jvc_cmd_radio_stop(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_jvc_cmd_radio_bass_up(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_jvc_cmd_radio_bass_down(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_jvc_cmd_radio_sound_mode(IR_COMMON_COMMAND_TYPE* p_command);

// --------------------------------------------------------------------------------

#endif // _H_ir_protocol_jvc_
