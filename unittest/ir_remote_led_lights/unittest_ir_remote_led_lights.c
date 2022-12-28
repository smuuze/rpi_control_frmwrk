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
 * @file   unittest_ir_remote_led_lights.c
 * @author Sebastian Lesse
 * @date   2022 / 01 / 03
 * @brief  Short description of this file
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

#include "../unittest.h"

UT_ACTIVATE()

// --------------------------------------------------------------------------------

#include "common/signal_slot_interface.h"
#include "driver/timer/timer_configuration.h"
#include "initialization/initialization.h"

#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

#include "command_management/command_handler_interface.h"

// --------------------------------------------------------------------------------

#include "modules/ir/ir_protocol_task.h"
#include "modules/ir/ir_protocol_interface.h"

#include "modules/ir/ir_protocol_rpi_cmd_handler.h"
#include "modules/ir/ir_protocol_led_lights_commands.h"
#include "modules/ir/ir_protocol_led_lights_rpi_cmd_handler.h"

// --------------------------------------------------------------------------------

#include "common/common_tools_bit_vector.h"

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INITIALIZATION			0
#define TEST_CASE_ID_BUILD_IR_COMMANDS			1
#define TEST_CASE_ID_COMMAND_PARSING			2
#define TEST_CASE_ID_INVALID_COMMAND_ARGUMENT		3
#define TEST_CASE_ID_TRANSMIT_IR_COMMAND		4
#define TEST_CASE_ID_CANCEL_IR_COMMAND			5
#define TEST_CASE_ID_TRANSMIT_IR_COMMAND_LIGHT_BRIGHTER	6
#define TEST_CASE_ID_TRANSMIT_IR_COMMAND_LIGHT_GREEN	7

// --------------------------------------------------------------------------------

#define UT_SIGNAL_TIMEOUT_MS				100

// --------------------------------------------------------------------------------

#define UT_IR_CMD_ADRESS_INIT_VALUE                     0xFF

// --------------------------------------------------------------------------------

IR_COMMON_COMMAND_TYPE ir_command_received_via_signal;
TIMER_CONFIGURATION_IRQ_COUNTER_IRQ_CALLBACK p_irq_callback;

// --------------------------------------------------------------------------------

static u8 counter_SLOT_IR_CMD_RECEIVED = 0;
static u8 counter_IR_MOD_OUT_DRIVE_LOW = 0;
static u8 counter_IR_MOD_OUT_DRIVE_HIGH = 0;
static u8 counter_TIMER0_START = 0;
static u8 counter_TIMER0_STOP = 0;
static u8 counter_TIMER1_START = 0;
static u8 counter_TIMER1_STOP = 0;
static u16 counter_IR_MOD_SEQUENCE = 0;

static u8 array_IR_MOD_SEQUENCE[2048];

static void unittest_reset_counter(void) {

	counter_SLOT_IR_CMD_RECEIVED = 0;
	counter_IR_MOD_OUT_DRIVE_LOW = 0;
	counter_IR_MOD_OUT_DRIVE_HIGH = 0;
	counter_TIMER0_START = 0;
	counter_TIMER0_STOP = 0;
	counter_TIMER1_START = 0;
	counter_TIMER1_STOP = 0;
	counter_IR_MOD_SEQUENCE = 0;

	ir_command_received_via_signal.type  = 0;
	ir_command_received_via_signal.data_1  = UT_IR_CMD_ADRESS_INIT_VALUE; // because it will be set to 0x00
	ir_command_received_via_signal.data_2  = 0;
	ir_command_received_via_signal.data_3  = 0;
	ir_command_received_via_signal.data_4  = 0;

	memset(array_IR_MOD_SEQUENCE, 0x00, sizeof(array_IR_MOD_SEQUENCE));
}

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(UNITTEST_TIMER)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(TIMEOUT_TIMER)

static void unittest_wait_ms(u16 waittime_ms) {

	TIMEOUT_TIMER_start();
	while (TIMEOUT_TIMER_is_up(waittime_ms) == 0) {

	}
}

// --------------------------------------------------------------------------------

// stubs

static u8 unittest_pin_was_changed = 0;

void IR_MOD_OUT_drive_low(void) {
	//DEBUG_PASS("UNITTEST - IR_MOD_OUT_drive_low()");
	counter_IR_MOD_OUT_DRIVE_LOW += 1;
        unittest_pin_was_changed = 1;

	if (counter_IR_MOD_SEQUENCE < sizeof(array_IR_MOD_SEQUENCE) - 1) {
		array_IR_MOD_SEQUENCE[counter_IR_MOD_SEQUENCE++] = 0;
	}
}

void IR_MOD_OUT_drive_high(void) {
	//DEBUG_PASS("UNITTEST - IR_MOD_OUT_drive_high()");
	counter_IR_MOD_OUT_DRIVE_HIGH += 1;
        unittest_pin_was_changed = 1;

	if (counter_IR_MOD_SEQUENCE < sizeof(array_IR_MOD_SEQUENCE) - 1) {
		array_IR_MOD_SEQUENCE[counter_IR_MOD_SEQUENCE++] = 1;
	}
}

void timer1_driver_init(void) {
	
}

void timer1_driver_deinit(void) {
	
}

void timer1_driver_configure(TIMER_CONFIGURATION_TYPE* p_configuration) {

	if (p_configuration->irq_callback != NULL) {
		DEBUG_PASS("UNITTEST - timer1_driver_configure() - callback != NULL");
		p_irq_callback = p_configuration->irq_callback;
	}
}

void timer1_driver_start(u32 time_us) {
	DEBUG_PASS("UNITTEST - timer1_driver_start()");
	counter_TIMER1_START += 1;
}

void timer1_driver_stop(void) {
	//DEBUG_PASS("UNITTEST - timer1_driver_stop()");
	counter_TIMER1_STOP += 1;
}

void timer0_driver_init(void) {
	
}

void timer0_driver_deinit(void) {
	
}
void timer0_driver_configure(TIMER_CONFIGURATION_TYPE* p_configuration) {

	if (p_configuration->irq_callback != NULL) {
		DEBUG_PASS("UNITTEST - timer0_driver_configure() - callback != NULL");
	}
}
void timer0_driver_start(u32 time_us) {
	DEBUG_PASS("UNITTEST - timer0_driver_start()");
	counter_TIMER0_START += 1;
	
}
void timer0_driver_stop(void) {
	//DEBUG_PASS("UNITTEST - timer0_driver_stop()");
	counter_TIMER0_STOP += 1;
}

// --------------------------------------------------------------------------------

// slots callbacksstatic IR_COMMON_COMMAND sony_ir_command;

static void uinittest_slot_IR_CMD_RECEIVED(const void* p_arg) {

	DEBUG_PASS("uinittest_slot_SONY_IR_CMD_RECEIVED()");

	counter_SLOT_IR_CMD_RECEIVED += 1;

	const IR_COMMON_COMMAND_TYPE* p_command = (const IR_COMMON_COMMAND_TYPE*) p_arg;
	ir_command_received_via_signal.type    = p_command->type;
	ir_command_received_via_signal.data_1  = p_command->data_1;
	ir_command_received_via_signal.data_2  = p_command->data_2;
	ir_command_received_via_signal.data_3  = p_command->data_3;
	ir_command_received_via_signal.data_4  = p_command->data_4;
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(IR_CMD_RECEIVED_SIGNAL, UT_IR_CMD_RECEIVED_SLOT, uinittest_slot_IR_CMD_RECEIVED)

// --------------------------------------------------------------------------------

// Signals / Slots

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_PROTOCOL_COMMAND_RECEIVED_SIGNAL)

SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(IR_CMD_RECEIVED_SIGNAL)

// --------------------------------------------------------------------------------

static void TEST_CASE_initialization(void) {

	UT_START_TEST_CASE("Initialization")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZATION);
		unittest_reset_counter();

		initialization();
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_ir_command_parsing(void) {

	UT_START_TEST_CASE("IR Command Parsing")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_COMMAND_PARSING);

		unittest_reset_counter();
		u8 err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_POWER_ON);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0xE0);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_POWER_OFF);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0x60);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_BRIGHTER);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0xA0);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_DARKER);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0x20);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_FLASH);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0xF0);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_STROBE);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0xE8);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_FADE);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0xD8);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_SMOOTH);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0xC8);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_WHITE);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0xD0);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_RED);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0x90);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_RED_INDIAN);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0xB0);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_COPPER);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0xA8);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_ORANGE);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0x98);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_YELLOW);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0x88);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_GREEN_FOREST);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0x10);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_GREEN);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0x30);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_BLUE_AZURE);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0x28);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_BLUE_STEEL);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0x18);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_JADE);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0x08);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_BLUE);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0x50);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_BLUE_MARINE);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0x70);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_EGGPLANT);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0x68);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_PURPLE);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0x58);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_PINK);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0x48);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_ir_invalid_command_argument(void) {

	UT_START_TEST_CASE("Invalid Command Argument")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INVALID_COMMAND_ARGUMENT);

		unittest_reset_counter();
		u8 err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_TV, IR_COMMAND_POWER_ON);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_ERR_INVALID_ARGUMENT);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, UT_IR_CMD_ADRESS_INIT_VALUE);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, 0);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_EJECT);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_ERR_INVALID_ARGUMENT);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, UT_IR_CMD_ADRESS_INIT_VALUE);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_cancel_ir_command(void) {

    UNITTEST_TIMER_start();

    while (UNITTEST_TIMER_is_up(2500) == 0) {
        mcu_task_controller_schedule();

        if (p_irq_callback != 0) {
            p_irq_callback();
        }
    }
}

static void TEST_CASE_ir_transmit_ir_light_brighter(void) {

	UT_START_TEST_CASE("Transmit IR-Command Light Brighter")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_TRANSMIT_IR_COMMAND_LIGHT_BRIGHTER);

		unittest_reset_counter();
		u8 err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_BRIGHTER);

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(1000) == 0) {
            mcu_task_controller_schedule();

            if (p_irq_callback != 0) {
                p_irq_callback();
            }
		}

		u8 compare_MOD_ARRAY[] = {

                        // drive low before begin
                        0x00,
                        
                        // Preamble Pulse - 9000us - 16 x 560us
                         0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01  ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01, 0x01       // Index: 16

                        // Preamble Puse - 4500us - 8 x 560us
                        ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00                                                         // Index: 24

                         // Address 0x00 - Upper Nibble
                        ,0x01 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00                                                         // Index: 32

                         // Address 0x00 - Lower Nibble
                        ,0x01 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00                                                         // Index: 40

                         // Inverted Address 0xFF - Upper Nibble
                        ,0x01 ,0x00 ,0x00 ,0x00, 0x01 ,0x00 ,0x00 ,0x00  ,0x01 ,0x00 ,0x00 ,0x00, 0x01 ,0x00 ,0x00 ,0x00        // Index: 56

                         // Inverted Address 0xFF - Lower Nibble
                        ,0x01 ,0x00 ,0x00 ,0x00, 0x01 ,0x00 ,0x00 ,0x00  ,0x01 ,0x00 ,0x00 ,0x00, 0x01 ,0x00 ,0x00 ,0x00        // Index: 72

                        // Data 0xA0 (Bits: 1 0 1 0  x x x x ) - Upper Nibble
                        ,0x01 ,0x00 ,0x00 ,0x00, 0x01 ,0x00, 0x01 ,0x00  ,0x00 ,0x00 ,0x01 ,0x00                                // Index: 84

                        // Data 0xA0 (Bits: x x x x  0 0 0 0 ) - Lower Nibble
                        ,0x01 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00                                                         // Index: 92

                        // Inverted Data 0x5F (Bits: 0 1 0 1  x x x x ) - Upper Nibble
                        ,0x01 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00 ,0x01 ,0x00  ,0x01 ,0x00 ,0x00 ,0x00                                // Index: 104

                        // Inverted Data 0x5F (Bits: x x x x  1 1 1 1 ) - Lower Nibble
                        ,0x01 ,0x00 ,0x00 ,0x00, 0x01 ,0x00 ,0x00 ,0x00  ,0x01 ,0x00 ,0x00 ,0x00, 0x01 ,0x00 ,0x00 ,0x00        // Index: 120

                        // Stop Bit Pulse
                        ,0x01                                                                                                   // Index: 121

                        // set to low at the end of the signal
                        ,0x00                                                                                                   // Index: 122

                        // Is set by the ir-mcu-task to prepare power-down mode
                        ,0x00                                                                                                   // Index: 123

                        // Logical 1 :,0x01 ,0x00 ,0x00 ,0x00
                        // Logical 0: ,0x01 ,0x00
		};
		
		u16 expected_array_length = sizeof(compare_MOD_ARRAY);

		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);

		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0xA0);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

        UT_CHECK_IS_EQUAL(counter_TIMER0_STOP, 1);
        UT_CHECK_IS_EQUAL(counter_TIMER0_START, 1);

        UT_CHECK_IS_EQUAL(counter_TIMER1_STOP, 1);
        UT_CHECK_IS_EQUAL(counter_TIMER1_START, 1);

		UT_CHECK_IS_EQUAL(counter_IR_MOD_SEQUENCE, expected_array_length);
		UT_CHECK_IS_EQUAL(counter_IR_MOD_OUT_DRIVE_LOW, 75); // 74 times a low pin level
		UT_CHECK_IS_EQUAL(counter_IR_MOD_OUT_DRIVE_HIGH, 49); // 49 times a high pin level

		UT_COMPARE_ARRAY(array_IR_MOD_SEQUENCE, compare_MOD_ARRAY, expected_array_length);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_ir_transmit_ir_command_LIGHT_GREEN(void) {

	UT_START_TEST_CASE("Transmit IR-Command Light Green")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_TRANSMIT_IR_COMMAND_LIGHT_GREEN);

		unittest_reset_counter();
		u8 err_code = rpi_cmd_handler_ir_remote_led_lights(IR_DEVICE_LED_LIGHT_BAUHAUS, IR_COMMAND_LIGHT_GREEN);

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(1000) == 0) {
            mcu_task_controller_schedule();

            if (p_irq_callback != 0) {
                p_irq_callback();
            }
		}

		u8 compare_MOD_ARRAY[] = {

                        // drive low before begin
                        0x00,
                        
                        // Preamble Pulse - 9000us - 16 x 560us
                         0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01  ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01 ,0x01, 0x01       // Index: 16

                        // Preamble Puse - 4500us - 8 x 560us
                        ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00                                                         // Index: 24

                         // Address 0x00 - Upper Nibble
                        ,0x01 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00                                                         // Index: 32

                         // Address 0x00 - Lower Nibble
                        ,0x01 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00                                                         // Index: 40

                         // Inverted Address 0xFF - Upper Nibble
                        ,0x01 ,0x00 ,0x00 ,0x00, 0x01 ,0x00 ,0x00 ,0x00  ,0x01 ,0x00 ,0x00 ,0x00, 0x01 ,0x00 ,0x00 ,0x00        // Index: 56

                         // Inverted Address 0xFF - Lower Nibble
                        ,0x01 ,0x00 ,0x00 ,0x00, 0x01 ,0x00 ,0x00 ,0x00  ,0x01 ,0x00 ,0x00 ,0x00, 0x01 ,0x00 ,0x00 ,0x00        // Index: 72

                        // Data 0xA0 (Bits: 0 0 1 1  x x x x ) - Upper Nibble
                        ,0x01 ,0x00, 0x01 ,0x00, 0x01 ,0x00 ,0x00 ,0x00  ,0x01 ,0x00 ,0x00 ,0x00                                // Index: 84

                        // Data 0xA0 (Bits: x x x x  0 0 0 0 ) - Lower Nibble
                        ,0x01 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00 ,0x01 ,0x00                                                         // Index: 92

                        // Inverted Data 0x5F (Bits: 1 1 0 0  x x x x ) - Upper Nibble
                        ,0x01 ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00  ,0x01 ,0x00 ,0x01 ,0x00                                // Index: 104

                        // Inverted Data 0x5F (Bits: x x x x  1 1 1 1 ) - Lower Nibble
                        ,0x01 ,0x00 ,0x00 ,0x00, 0x01 ,0x00 ,0x00 ,0x00  ,0x01 ,0x00 ,0x00 ,0x00, 0x01 ,0x00 ,0x00 ,0x00        // Index: 120

                        // Stop Bit Pulse
                        ,0x01                                                                                                   // Index: 121

                        // set to low at the end of the signal
                        ,0x00                                                                                                   // Index: 122

                        // Is set by the ir-mcu-task to prepare power-down mode
                        ,0x00                                                                                                   // Index: 123

                        // Logical 1 :,0x01 ,0x00 ,0x00 ,0x00 
                        // Logical 0: ,0x01 ,0x00 
		};
		
		u16 expected_array_length = sizeof(compare_MOD_ARRAY);

		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_IR_CMD_RECEIVED, 1);

		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_1, 0x00);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.data_2, 0x30);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.type, IR_PROTOCOL_TYPE_NEC);

        UT_CHECK_IS_EQUAL(counter_TIMER0_STOP, 1);
        UT_CHECK_IS_EQUAL(counter_TIMER0_START, 1);

        UT_CHECK_IS_EQUAL(counter_TIMER1_STOP, 1);
        UT_CHECK_IS_EQUAL(counter_TIMER1_START, 1);

		UT_CHECK_IS_EQUAL(counter_IR_MOD_SEQUENCE, expected_array_length);
		UT_CHECK_IS_EQUAL(counter_IR_MOD_OUT_DRIVE_LOW, 75); // 74 times a low pin level
		UT_CHECK_IS_EQUAL(counter_IR_MOD_OUT_DRIVE_HIGH, 49); // 49 times a high pin level

		UT_COMPARE_ARRAY(array_IR_MOD_SEQUENCE, compare_MOD_ARRAY, expected_array_length);
	}
	UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

	UT_START_TESTBENCH("Welcome the the UNITTEST for IR-Remote Sony 1.0")
	{
		TEST_CASE_initialization();

		UT_IR_CMD_RECEIVED_SLOT_connect();

		TEST_CASE_ir_command_parsing();
		TEST_CASE_ir_invalid_command_argument();
                TEST_CASE_cancel_ir_command();
		TEST_CASE_ir_transmit_ir_light_brighter();
		TEST_CASE_ir_transmit_ir_command_LIGHT_GREEN();
	}
	UT_END_TESTBENCH()

	return UT_TEST_RESULT();
}
