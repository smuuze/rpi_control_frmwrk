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
 * @file   ir_protocol_samsung.h
 * @author Sebastian Lesse
 * @date   2021 / 12 / 30
 * @brief  Commands that are used by Samsung IR-devices
 * 
 */

#ifndef _IR_PROTOCOL_SAMSUNG_H_
#define _IR_PROTOCOL_SAMSUNG_H_

// --------------------------------------------------------------------------------

#include "driver/timer/timer_interface.h"

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
void ir_protocol_samsung_init(void);

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param p_ir_command 
 */
void ir_command_samsung_protocol_type(IR_COMMON_COMMAND_TYPE* p_ir_command);

// --------------------------------------------------------------------------------

/*
 *
 */
void ir_protocol_samsung_address_tv(IR_COMMON_COMMAND_TYPE* p_command);

// --------------------------------------------------------------------------------

/*
 *
 */
void ir_protocol_samsung_cmd_tv_power(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_volume_up(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_volume_mute(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_volume_down(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_down(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_up(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_0(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_1(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_2(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_3(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_4(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_5(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_6(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_7(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_8(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_9(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_enter(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_exit(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_return(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_arrow_up(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_arrow_down(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_arrow_left(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_arrow_right(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_play(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_pause(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_stop(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_program_guide(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_home(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_channel_list(IR_COMMON_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_source(IR_COMMON_COMMAND_TYPE* p_command);

// --------------------------------------------------------------------------------

#endif // _IR_REMOTE_MCU_TASK_H_
