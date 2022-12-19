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
 * @file    ir_protocol_jvc_commands.c
 * @author  Sebastian Lesse
 * @date    2022 / 12 / 19
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

#include "modules/ir/ir_protocol_jvc.h"

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

// --------------------------------------------------------------------------------
