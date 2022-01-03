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
 * @file   ir_command_led_lights.c
 * @author Sebastian Lesse
 * @date   2021 / 12 / 30
 * @brief  Commands that are used by LED-Light devices
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
#include "3rdparty/ir_protocol/ir_protocol_interface.h"

// --------------------------------------------------------------------------------

void ir_command_led_lights_bauhaus_protocol_type(IR_COMMON_COMMAND_TYPE* p_ir_command) {
        p_ir_command->type = IR_PROTOCOL_TYPE_NEC;
}

void ir_command_led_lights_bauhaus_address(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_1 = BUILD_BIT_VECTOR_U8(0,0,0,0, 0,0,0,0); // 0x00
}

void ir_command_led_lights_bauhaus_cmd_on(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(1,1,1,0, 0,0,0,0); // 0xE0
}

void ir_command_led_lights_bauhaus_cmd_off(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(0,1,1,0, 0,0,0,0); // 0x60
}

void ir_command_led_lights_bauhaus_cmd_brighter(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(1,0,1,0, 0,0,0,0); // 0xA0
}

void ir_command_led_lights_bauhaus_cmd_darker(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(0,0,1,0, 0,0,0,0); // 0x20
}

void ir_command_led_lights_bauhaus_cmd_flash(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(1,1,1,1, 0,0,0,0); // 0xF0
}

void ir_command_led_lights_bauhaus_cmd_strobe(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(1,1,1,0, 1,0,0,0); // 0xE8
}

void ir_command_led_lights_bauhaus_cmd_fade(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(1,1,0,1, 1,0,0,0); // 0xD8
}

void ir_command_led_lights_bauhaus_cmd_smooth(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(1,1,0,0, 1,0,0,0); // 0xC8
}

void ir_command_led_lights_bauhaus_cmd_white(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(1,1,0,1, 0,0,0,0); // 0xD0
}

void ir_command_led_lights_bauhaus_cmd_red(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(1,0,0,1, 0,0,0,0); // 0x90
}

void ir_command_led_lights_bauhaus_cmd_red_indian(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(1,0,1,1, 0,0,0,0); // 0xB0
}

void ir_command_led_lights_bauhaus_cmd_copper(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(1,0,1,0, 1,0,0,0); // 0xA8
}

void ir_command_led_lights_bauhaus_cmd_orange(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(1,0,0,1, 1,0,0,0); // 0x98
}

void ir_command_led_lights_bauhaus_cmd_yellow(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(1,0,0,0, 1,0,0,0); // 0x88
}

void ir_command_led_lights_bauhaus_cmd_green_forest(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(0,0,0,1, 0,0,0,0); // 0x10
}

void ir_command_led_lights_bauhaus_cmd_green(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(0,0,1,1, 0,0,0,0); // 0x48
}

void ir_command_led_lights_bauhaus_cmd_blue_azure(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(0,0,1,0, 1,0,0,0); // 0x28
}

void ir_command_led_lights_bauhaus_cmd_blue_steel(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(0,0,0,1, 1,0,0,0); // 0x18
}

void ir_command_led_lights_bauhaus_cmd_jade(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(0,0,0,0, 1,0,0,0); // 0x08
}

void ir_command_led_lights_bauhaus_cmd_blue(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(0,1,0,1, 0,0,0,0); // 0x50
}

void ir_command_led_lights_bauhaus_cmd_blue_marine(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(0,1,1,1, 0,0,0,0); // 0x70
}

void ir_command_led_lights_bauhaus_cmd_eggplant(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(0,1,1,0, 1,0,0,0); // 0x68
}

void ir_command_led_lights_bauhaus_cmd_purple(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(0,1,0,1, 1,0,0,0); // 0x58
}

void ir_command_led_lights_bauhaus_cmd_pink(IR_COMMON_COMMAND_TYPE* p_ir_command) {
	p_ir_command->data_2 = BUILD_BIT_VECTOR_U8(0,1,0,0, 1,0,0,0); // 0x48
}

// --------------------------------------------------------------------------------
