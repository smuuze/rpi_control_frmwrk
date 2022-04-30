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
 * @file    ir_protocol_sony_commands.c
 * @author  Sebastian Lesse
 * @date    2021 / 03 / 20
 * @brief   Short description of this file
 * 
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "cpu.h"

// --------------------------------------------------------------------------------

#include "common/common_tools_bit_vector.h"

// --------------------------------------------------------------------------------

#include "3rdparty/ir_protocol/ir_protocol_interface.h"

// --------------------------------------------------------------------------------

#include "3rdparty/ir_protocol/ir_protocol_sony.h"

// --------------------------------------------------------------------------------

inline void ir_protocol_sony_address_bdplayer(IR_COMMON_COMMAND_TYPE* p_command) {
	p_command->data_2   = BUILD_BIT_VECTOR_U8(0,0,0,0,1,0,1,1); // 0b00001011;
	p_command->data_3 = BUILD_BIT_VECTOR_U8(0,1,0,0,0,1,1,1); // 0b01000111;
}

// --------------------------------------------------------------------------------

inline void ir_protocol_sony_cmd_bdplayer_power(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_power()");
	p_command->data_1 = BUILD_BIT_VECTOR_U8(1,0,1,0,1,0,0,0); // 0b10101000;
}

inline void ir_protocol_sony_cmd_bdplayer_play(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_play()");
	p_command->data_1 = BUILD_BIT_VECTOR_U8(0,1,0,1,1,0,0,0); // 0b01011000;
}

inline void ir_protocol_sony_cmd_bdplayer_stop(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_stop()");
	p_command->data_1 = BUILD_BIT_VECTOR_U8(0,0,0,1,1,0,0,0); // 0b00011000;
}

inline void ir_protocol_sony_cmd_bdplayer_pause(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_pause()");
	p_command->data_1 = BUILD_BIT_VECTOR_U8(1,0,0,1,1,0,0,0); // 0b10011000;
}

inline void ir_protocol_sony_cmd_bdplayer_audio_language(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_audio_language()");
	p_command->data_1 = BUILD_BIT_VECTOR_U8(0,0,1,0,0,1,1,0); // 0b00100110;
}

inline void ir_protocol_sony_cmd_bdplayer_subtitle_language(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_subtitle_language()");
	p_command->data_1 = BUILD_BIT_VECTOR_U8(1,1,0,0,0,1,1,0); // 0b11000110;
}

inline void ir_protocol_sony_cmd_bdplayer_eject(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_eject()");
	p_command->data_1 = BUILD_BIT_VECTOR_U8(0,1,1,0,1,0,0,0); // 0b01101000;
}

inline void ir_protocol_sony_cmd_bdplayer_return(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_return()");
	p_command->data_1 = BUILD_BIT_VECTOR_U8(1,1,0,0,0,0,1,0); // 0b11000010;
}

inline void ir_protocol_sony_cmd_bdplayer_top_menu(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_top_menu()");
	p_command->data_1 = BUILD_BIT_VECTOR_U8(0,0,1,1,0,1,0,0); // 0b00110100;
}

inline void ir_protocol_sony_cmd_bdplayer_pop_up_menu(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_pop_up_menu()");
	p_command->data_1 = BUILD_BIT_VECTOR_U8(1,0,0,1,0,1,0,0); // 0b10010100;
}

inline void ir_protocol_sony_cmd_bdplayer_next(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_next()");
	p_command->data_1 = BUILD_BIT_VECTOR_U8(0,1,1,0,1,0,1,0); // 0b01101010;
}

inline void ir_protocol_sony_cmd_bdplayer_previous(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_previous()");
	p_command->data_1 = BUILD_BIT_VECTOR_U8(1,1,1,0,1,0,1,0); // 0b11101010;
}
