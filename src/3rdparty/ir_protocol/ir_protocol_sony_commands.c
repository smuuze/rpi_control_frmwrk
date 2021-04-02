/**
 * --------------------------------------------------------------------------------
 *
 * @file 	ir_protocol_sony_commands.c
 * @author 	sebastian lesse (sebastian lesse)
 * @brief 
 * @version 	1.0
 * @date 	2021-03-20
 * 
 * @copyright 	Copyright (c) 2021
 *
 * --------------------------------------------------------------------------------
 */

// --------------------------------------------------------------------------------

#define TRACER_OFF

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "common/common_tools_bit_vector.h"

// --------------------------------------------------------------------------------

#include "3rdparty/ir_protocol/ir_protocol_sony.h"

// --------------------------------------------------------------------------------

inline void ir_protocol_sony_address_bdplayer(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	p_command->device   = BUILD_BIT_VECTOR_U8(0,0,0,0,1,0,1,1); // 0b00001011;
	p_command->extended = BUILD_BIT_VECTOR_U8(0,1,0,0,0,1,1,1); // 0b01000111;
}

// --------------------------------------------------------------------------------

inline void ir_protocol_sony_cmd_bdplayer_power(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_power()");
	p_command->command = BUILD_BIT_VECTOR_U8(1,0,1,0,1,0,0,0); // 0b10101000;
}

inline void ir_protocol_sony_cmd_bdplayer_play(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_play()");
	p_command->command = BUILD_BIT_VECTOR_U8(0,1,0,1,1,0,0,0); // 0b01011000;
}

inline void ir_protocol_sony_cmd_bdplayer_stop(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_stop()");
	p_command->command = BUILD_BIT_VECTOR_U8(0,0,0,1,1,0,0,0); // 0b00011000;
}

inline void ir_protocol_sony_cmd_bdplayer_pause(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_pause()");
	p_command->command = BUILD_BIT_VECTOR_U8(1,0,0,1,1,0,0,0); // 0b10011000;
}

inline void ir_protocol_sony_cmd_bdplayer_audio_language(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_audio_language()");
	p_command->command = BUILD_BIT_VECTOR_U8(0,0,1,0,0,1,1,0); // 0b00100110;
}

inline void ir_protocol_sony_cmd_bdplayer_subtitle_language(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_subtitle_language()");
	p_command->command = BUILD_BIT_VECTOR_U8(1,1,0,0,0,1,1,0); // 0b11000110;
}

inline void ir_protocol_sony_cmd_bdplayer_eject(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_eject()");
	p_command->command = BUILD_BIT_VECTOR_U8(0,1,1,0,1,0,0,0); // 0b01101000;
}

inline void ir_protocol_sony_cmd_bdplayer_return(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_return()");
	p_command->command = BUILD_BIT_VECTOR_U8(1,1,0,0,0,0,1,0); // 0b11000010;
}

inline void ir_protocol_sony_cmd_bdplayer_top_menu(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_top_menu()");
	p_command->command = BUILD_BIT_VECTOR_U8(0,0,1,1,0,1,0,0); // 0b00110100;
}

inline void ir_protocol_sony_cmd_bdplayer_pop_up_menu(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_pop_up_menu()");
	p_command->command = BUILD_BIT_VECTOR_U8(1,0,0,1,0,1,0,0); // 0b10010100;
}

inline void ir_protocol_sony_cmd_bdplayer_next(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_next()");
	p_command->command = BUILD_BIT_VECTOR_U8(0,1,1,0,1,0,1,0); // 0b01101010;
}

inline void ir_protocol_sony_cmd_bdplayer_previous(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_sony_cmd_bdplayer_previous()");
	p_command->command = BUILD_BIT_VECTOR_U8(1,1,1,0,1,0,1,0); // 0b11101010;
}
