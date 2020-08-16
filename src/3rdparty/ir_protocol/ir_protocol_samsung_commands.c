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

#include "3rdparty/ir_protocol/ir_protocol_samsung.h"

// --------------------------------------------------------------------------------

inline void ir_protocol_samsung_address_tv(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	p_command->address = 0b11100000;
}

// --------------------------------------------------------------------------------

inline void ir_protocol_samsung_cmd_tv_power(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_power()");
	p_command->control = 0b01000000;
}

inline void ir_protocol_samsung_cmd_tv_volume_up(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_volume_up()");
	p_command->control = 0b11100000;
}

inline void ir_protocol_samsung_cmd_tv_volume_down(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_volume_down()");
	p_command->control = 0b11010000;
}

inline void ir_protocol_samsung_cmd_tv_volume_mute(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_volume_mute()");
	p_command->control = 0b11110000;
}

inline void ir_protocol_samsung_cmd_tv_channel_up(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_up()");
	p_command->control = 0b01001000;
}

inline void ir_protocol_samsung_cmd_tv_channel_down(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_down()");
	p_command->control = 0b00001000;
}

inline void ir_protocol_samsung_cmd_tv_channel_0(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_0()");
	p_command->control = 0b10001000;
}

inline void ir_protocol_samsung_cmd_tv_channel_1(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_1()");
	p_command->control = 0b00100000;
}

inline void ir_protocol_samsung_cmd_tv_channel_2(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_2()");
	p_command->control = 0b10100000;
}

inline void ir_protocol_samsung_cmd_tv_channel_3(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_3()");
	p_command->control = 0b01100000;
}

inline void ir_protocol_samsung_cmd_tv_channel_4(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_4()");
	p_command->control = 0b00010000;
}

inline void ir_protocol_samsung_cmd_tv_channel_5(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_5()");
	p_command->control = 0b10010000;
}

inline void ir_protocol_samsung_cmd_tv_channel_6(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_6()");
	p_command->control = 0b01010000;
}

inline void ir_protocol_samsung_cmd_tv_channel_7(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_7()");
	p_command->control = 0b11010000;
}

inline void ir_protocol_samsung_cmd_tv_channel_8(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_8()");
	p_command->control = 0b10110000;
}

inline void ir_protocol_samsung_cmd_tv_channel_9(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_channel_9()");
	p_command->control = 0b01110000;
}

inline void ir_protocol_samsung_cmd_tv_enter(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_enter()");
	p_command->control = 0b00010110;
}

inline void ir_protocol_samsung_cmd_tv_exit(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_exit()");
	p_command->control = 0b10110100;
}

inline void ir_protocol_samsung_cmd_tv_return(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_return()");
	p_command->control = 0b00011010;
}

inline void ir_protocol_samsung_cmd_tv_arrow_up(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_arrow_up()");
	p_command->control = 0b00000110;
}

inline void ir_protocol_samsung_cmd_tv_arrow_down(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_arrow_down()");
	p_command->control = 0b10000110;
}

inline void ir_protocol_samsung_cmd_tv_arrow_left(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_arrow_left()");
	p_command->control = 0b10100110;
}

inline void ir_protocol_samsung_cmd_tv_arrow_right(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_arrow_right()");
	p_command->control = 0b01000110;
}

inline void ir_protocol_samsung_cmd_tv_play(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_play()");
	p_command->control = 0b11100010;
}

inline void ir_protocol_samsung_cmd_tv_pause(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_pause()");
	p_command->control = 0b01010010;
}

inline void ir_protocol_samsung_cmd_tv_stop(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_samsung_cmd_tv_stop()");
	p_command->control = 0b01100010;
}