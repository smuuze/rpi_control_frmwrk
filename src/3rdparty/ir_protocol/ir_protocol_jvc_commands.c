/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	ir_protocol_jvc_commands.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

#ifdef TRACER_ON
#pragma __WARNING__TRACES_ENABLED__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "common/common_tools_bit_vector.h"

// --------------------------------------------------------------------------------

#include "3rdparty/ir_protocol/ir_protocol_jvc.h"

// --------------------------------------------------------------------------------

inline void ir_protocol_jvc_address_radio(JVC_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	p_command->address = BUILD_BIT_VECTOR_U8(1,1,1,1,1,0,0,1); // 0b11111001;
}

// --------------------------------------------------------------------------------

inline void ir_protocol_jvc_cmd_radio_power(JVC_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_jvc_cmd_radio_power()");
	p_command->control = BUILD_BIT_VECTOR_U8(0,0,0,0,0,0,0,0); // 0b00000000;
}

inline void ir_protocol_jvc_cmd_radio_volume_up(JVC_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_jvc_cmd_radio_volume_up()");
	p_command->control = BUILD_BIT_VECTOR_U8(1,0,0,0,0,0,0,0); // 0b10000000;
}

inline void ir_protocol_jvc_cmd_radio_volume_down(JVC_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_jvc_cmd_radio_volume_down()");
	p_command->control = BUILD_BIT_VECTOR_U8(0,1,0,0,0,0,0,0); // 0b01000000;
}

inline void ir_protocol_jvc_cmd_radio_volume_mute(JVC_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_jvc_cmd_radio_volume_mute()");
	p_command->control = BUILD_BIT_VECTOR_U8(1,1,0,0,0,0,0,0); // 0b11000000;
}

inline void ir_protocol_jvc_cmd_radio_play(JVC_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_jvc_cmd_radio_play()");
	p_command->control = BUILD_BIT_VECTOR_U8(1,1,0,1,0,0,0,0); // 0b11010000;
}

inline void ir_protocol_jvc_cmd_radio_pause(JVC_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_jvc_cmd_radio_pause()");
	p_command->control = BUILD_BIT_VECTOR_U8(1,1,0,1,0,0,0,0); // 0b11010000;
}

inline void ir_protocol_jvc_cmd_radio_stop(JVC_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_jvc_cmd_radio_stop()");
	p_command->control = BUILD_BIT_VECTOR_U8(1,0,0,0,1,1,1,0); // 0b10001110;
}

inline void ir_protocol_jvc_cmd_radio_bass_up(JVC_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_jvc_cmd_radio_bass_up()");
	p_command->control = BUILD_BIT_VECTOR_U8(1,1,1,0,0,0,0,1); // 0b11100001;
}

inline void ir_protocol_jvc_cmd_radio_bass_down(JVC_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_jvc_cmd_radio_bass_down()");
	p_command->control = BUILD_BIT_VECTOR_U8(0,0,0,1,0,0,0,1); // 0b00010001;
}

inline void ir_protocol_jvc_cmd_radio_sound_mode(JVC_IR_PROTOCOL_COMMAND_TYPE* p_command) {
	DEBUG_PASS("ir_protocol_jvc_cmd_radio_sound_mode()");
	p_command->control = BUILD_BIT_VECTOR_U8(1,1,1,1,0,0,0,1); // 0b11110001;
}
