/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	ir_protocol_samusung_commands.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "common/common_tools_bit_vector.h"

// --------------------------------------------------------------------------------

#include "modules/ir/ir_protocol_interface.h"

// --------------------------------------------------------------------------------

#include "modules/ir/ir_protocol_samsung.h"

// --------------------------------------------------------------------------------

inline void ir_command_samsung_protocol_type(IR_COMMON_COMMAND_TYPE* p_ir_command) {
        p_ir_command->type = IR_PROTOCOL_TYPE_SAMSUNG;
}

// --------------------------------------------------------------------------------

inline void ir_protocol_samsung_address_tv(IR_COMMON_COMMAND_TYPE* p_command) {
	p_command->data_1 = BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0); // 0b11100000;
}

// --------------------------------------------------------------------------------

inline void ir_protocol_samsung_cmd_tv_power(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_power()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(0,1,0,0,0,0,0,0); // 0b01000000;
}

inline void ir_protocol_samsung_cmd_tv_volume_up(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_volume_up()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,0); // 0b11100000;
}

inline void ir_protocol_samsung_cmd_tv_volume_down(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_volume_down()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(1,1,0,1,0,0,0,0); // 0b11010000;
}

inline void ir_protocol_samsung_cmd_tv_volume_mute(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_volume_mute()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(1,1,1,1,0,0,0,0); // 0b11110000;
}

inline void ir_protocol_samsung_cmd_tv_channel_up(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_up()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(0,1,0,0,1,0,0,0); // 0b01001000;
}

inline void ir_protocol_samsung_cmd_tv_channel_down(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_down()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(0,0,0,0,1,0,0,0); // 0b00001000;
}

inline void ir_protocol_samsung_cmd_tv_channel_0(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_0()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(1,0,0,0,1,0,0,0); // 0b10001000;
}

inline void ir_protocol_samsung_cmd_tv_channel_1(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_1()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(0,0,1,0,0,0,0,0); // 0b00100000;
}

inline void ir_protocol_samsung_cmd_tv_channel_2(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_2()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(1,0,1,0,0,0,0,0); // 0b10100000;
}

inline void ir_protocol_samsung_cmd_tv_channel_3(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_3()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(0,1,1,0,0,0,0,0); // 0b01100000;
}

inline void ir_protocol_samsung_cmd_tv_channel_4(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_4()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(0,0,0,1,0,0,0,0); // 0b00010000;
}

inline void ir_protocol_samsung_cmd_tv_channel_5(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_5()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(1,0,0,1,0,0,0,0); // 0b10010000;
}

inline void ir_protocol_samsung_cmd_tv_channel_6(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_6()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(0,1,0,1,0,0,0,0); // 0b01010000;
}

inline void ir_protocol_samsung_cmd_tv_channel_7(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_7()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(1,1,0,1,0,0,0,0); // 0b11010000;
}

inline void ir_protocol_samsung_cmd_tv_channel_8(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_8()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(1,0,1,1,0,0,0,0); // 0b10110000;
}

inline void ir_protocol_samsung_cmd_tv_channel_9(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_9()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(0,1,1,1,0,0,0,0); // 0b01110000;
}

inline void ir_protocol_samsung_cmd_tv_enter(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_enter()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(0,0,0,1,0,1,1,0); // 0b00010110;
}

inline void ir_protocol_samsung_cmd_tv_exit(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_exit()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(1,0,1,1,0,1,0,0); // 0b10110100;
}

inline void ir_protocol_samsung_cmd_tv_return(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_return()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(0,0,0,1,1,0,1,0); // 0b00011010;
}

inline void ir_protocol_samsung_cmd_tv_arrow_up(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_arrow_up()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(0,0,0,0,0,1,1,0); // 0b00000110;
}

inline void ir_protocol_samsung_cmd_tv_arrow_down(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_arrow_down()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(1,0,0,0,0,1,1,0); // 0b10000110;
}

inline void ir_protocol_samsung_cmd_tv_arrow_left(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_arrow_left()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(1,0,1,0,0,1,1,0); // 0b10100110;
}

inline void ir_protocol_samsung_cmd_tv_arrow_right(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_arrow_right()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(0,1,0,0,0,1,1,0); // 0b01000110;
}

inline void ir_protocol_samsung_cmd_tv_play(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_play()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,1,0); // 0b11100010;
}

inline void ir_protocol_samsung_cmd_tv_pause(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_pause()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(0,1,0,1,0,0,1,0); // 0b01010010;
}

inline void ir_protocol_samsung_cmd_tv_stop(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_stop()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(0,1,1,0,0,0,1,0); // 0b01100010;
}

inline void ir_protocol_samsung_cmd_tv_program_guide(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_program_guide()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(1,1,1,1,0,0,1,0); // 0b11110010;
}

inline void ir_protocol_samsung_cmd_tv_home(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_home()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(1,0,0,1,1,1,1,0); // 0b10011110;
}

inline void ir_protocol_samsung_cmd_tv_channel_list(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_home()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(1,1,0,1,0,1,1,0); // 0b11010110;
}

inline void ir_protocol_samsung_cmd_tv_source(IR_COMMON_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_home()");
	p_command->data_2 = BUILD_BIT_VECTOR_U8(1,0,0,0,0,0,0,0); // 0b10000000;
}
