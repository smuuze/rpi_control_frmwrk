/**
 * @file 	unittest_ir_remote_sony.c
 * @author 	sebastian lesse (sebastian lesse)
 * @brief 
 * @version 	1.0
 * @date 	2021-03-21
 * 
 * @copyright 	Copyright (c) 2021
 * 
 */

#define TRACER_ON

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"
// --------------------------------------------------------------------------------

#include "../unittest.h"

UT_ACTIVATE()

// --------------------------------------------------------------------------------

#include "common/signal_slot_interface.h"
#include "driver/timer/timer_configuration.h"
#include "initialization/initialization.h"

#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

#include "app_tasks/ir_remote_mcu_task.h"
#include "3rdparty/ir_protocol/ir_protocol_sony.h"

#include "command_handler/rpi_command_handler_ir_remote.h"
#include "command_management/command_handler_interface.h"
#include "command_handler/rpi_cmd_handler_ir_remote_sony.h"

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INITIALIZATION			0
#define TEST_CASE_ID_BUILD_IR_COMMANDS			1
#define TEST_CASE_ID_DEVICE_OPERATION			2
#define TEST_CASE_ID_INVALID_DEVICE_OPERATION		3
#define TEST_CASE_ID_TRANSMIT_IR_COMMAND		4
#define TEST_CASE_ID_CANCEL_IR_COMMAND			5
#define TEST_CASE_ID_TRANSMIT_IR_COMMAND_POWER_ON	6
#define TEST_CASE_ID_TRANSMIT_IR_COMMAND_AUDIO_LANGUAGE	7

// --------------------------------------------------------------------------------

#define UT_SIGNAL_TIMEOUT_MS				100

// --------------------------------------------------------------------------------

SONY_IR_PROTOCOL_COMMAND_TYPE ir_command_received_via_signal;
TIMER_CONFIGURATION_IRQ_COUNTER_IRQ_CALLBACK p_irq_callback;

// --------------------------------------------------------------------------------

static u8 counter_SLOT_SONY_IR_CMD_RECEIVED = 0;
static u8 counter_IR_MOD_OUT_DRIVE_LOW = 0;
static u8 counter_IR_MOD_OUT_DRIVE_HIGH = 0;
static u8 counter_TIMER0_START = 0;
static u8 counter_TIMER0_STOP = 0;
static u8 counter_TIMER1_START = 0;
static u8 counter_TIMER1_STOP = 0;
static u8 counter_IR_MOD_SEQUENCE = 0;

static u8 array_IR_MOD_SEQUENCE[128];

static void unittest_reset_counter(void) {

	counter_SLOT_SONY_IR_CMD_RECEIVED = 0;
	counter_IR_MOD_OUT_DRIVE_LOW = 0;
	counter_IR_MOD_OUT_DRIVE_HIGH = 0;
	counter_TIMER0_START = 0;
	counter_TIMER0_STOP = 0;
	counter_TIMER1_START = 0;
	counter_TIMER1_STOP = 0;
	counter_IR_MOD_SEQUENCE = 0;

	memset(array_IR_MOD_SEQUENCE, 0x00, sizeof(array_IR_MOD_SEQUENCE));
	memset((u8*)&ir_command_received_via_signal, 0x00, sizeof(ir_command_received_via_signal));
}

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(UNITTEST_TIMER)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(TIMEOUT_TIMER)

static unittest_wait_ms(u16 waittime_ms) {

	TIMEOUT_TIMER_start();
	while (TIMEOUT_TIMER_is_up(waittime_ms) == 0) {

	}
}

// --------------------------------------------------------------------------------

// stubs


void IR_MOD_OUT_drive_low(void) {
	DEBUG_PASS("UNITTEST - IR_MOD_OUT_drive_low()");
	counter_IR_MOD_OUT_DRIVE_LOW += 1;

	if (counter_IR_MOD_SEQUENCE < sizeof(array_IR_MOD_SEQUENCE) - 1) {
		array_IR_MOD_SEQUENCE[counter_IR_MOD_SEQUENCE++] = 0;
	}
}

void IR_MOD_OUT_drive_high(void) {
	DEBUG_PASS("UNITTEST - IR_MOD_OUT_drive_high()");
	counter_IR_MOD_OUT_DRIVE_HIGH += 1;

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
	DEBUG_PASS("UNITTEST - timer1_driver_stop()");
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
	DEBUG_PASS("UNITTEST - timer0_driver_stop()");
	counter_TIMER0_STOP += 1;
}

// --------------------------------------------------------------------------------

// slots callbacksstatic SONY_IR_PROTOCOL_COMMAND_TYPE sony_ir_command;

static void uinittest_slot_SONY_IR_CMD_RECEIVED(const void* p_arg) {

	DEBUG_PASS("uinittest_slot_SONY_IR_CMD_RECEIVED()");

	counter_SLOT_SONY_IR_CMD_RECEIVED += 1;

	const SONY_IR_PROTOCOL_COMMAND_TYPE* p_command = (const SONY_IR_PROTOCOL_COMMAND_TYPE*) p_arg;
	ir_command_received_via_signal.command  = p_command->command;
	ir_command_received_via_signal.device   = p_command->device;
	ir_command_received_via_signal.extended = p_command->extended;
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(SONY_IR_CMD_RECEIVED_SIGNAL, UT_SONY_IR_CMD_RECEIVED_SLOT, uinittest_slot_SONY_IR_CMD_RECEIVED)

// --------------------------------------------------------------------------------

// Signals / Slots

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_PROTOCOL_COMMAND_RECEIVED_SIGNAL)

SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(SONY_IR_CMD_RECEIVED_SIGNAL)

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

static void TEST_CASE_ir_command_build(void) {

	UT_START_TEST_CASE("Build IR-Commands")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_BUILD_IR_COMMANDS);
		unittest_reset_counter();

		SONY_IR_PROTOCOL_COMMAND_TYPE ir_command_sony;

		ir_protocol_sony_address_bdplayer(&ir_command_sony);
		UT_CHECK_IS_EQUAL(ir_command_sony.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_sony.extended, 0b01000111);

		ir_protocol_sony_cmd_bdplayer_power(&ir_command_sony);
		UT_CHECK_IS_EQUAL(ir_command_sony.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_sony.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_sony.command, 0b10101000);

		ir_protocol_sony_cmd_bdplayer_play(&ir_command_sony);
		UT_CHECK_IS_EQUAL(ir_command_sony.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_sony.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_sony.command, 0b01011000);

		ir_protocol_sony_cmd_bdplayer_stop(&ir_command_sony);
		UT_CHECK_IS_EQUAL(ir_command_sony.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_sony.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_sony.command, 0b00011000);

		ir_protocol_sony_cmd_bdplayer_pause(&ir_command_sony);
		UT_CHECK_IS_EQUAL(ir_command_sony.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_sony.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_sony.command, 0b10011000);

		ir_protocol_sony_cmd_bdplayer_audio_language(&ir_command_sony);
		UT_CHECK_IS_EQUAL(ir_command_sony.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_sony.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_sony.command, 0b00100110);

		ir_protocol_sony_cmd_bdplayer_subtitle_language(&ir_command_sony);
		UT_CHECK_IS_EQUAL(ir_command_sony.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_sony.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_sony.command, 0b11000110);

		ir_protocol_sony_cmd_bdplayer_eject(&ir_command_sony);
		UT_CHECK_IS_EQUAL(ir_command_sony.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_sony.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_sony.command, 0b01101000);

		ir_protocol_sony_cmd_bdplayer_return(&ir_command_sony);
		UT_CHECK_IS_EQUAL(ir_command_sony.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_sony.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_sony.command, 0b11000010);

		ir_protocol_sony_cmd_bdplayer_top_menu(&ir_command_sony);
		UT_CHECK_IS_EQUAL(ir_command_sony.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_sony.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_sony.command, 0b00110100);

		ir_protocol_sony_cmd_bdplayer_pop_up_menu(&ir_command_sony);
		UT_CHECK_IS_EQUAL(ir_command_sony.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_sony.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_sony.command, 0b10010100);

		ir_protocol_sony_cmd_bdplayer_next(&ir_command_sony);
		UT_CHECK_IS_EQUAL(ir_command_sony.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_sony.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_sony.command, 0b01101010);

		ir_protocol_sony_cmd_bdplayer_previous(&ir_command_sony);
		UT_CHECK_IS_EQUAL(ir_command_sony.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_sony.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_sony.command, 0b11101010);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_ir_device_operation(void) {

	UT_START_TEST_CASE("Device Operation")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_DEVICE_OPERATION);

		unittest_reset_counter();
		u8 err_code = rpi_cmd_handler_ir_remote_sony(IR_DEVICE_BLUE_RAY_PLAYER, IR_COMMAND_POWER_ON);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_SONY_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.command, 0b10101000);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_sony(IR_DEVICE_BLUE_RAY_PLAYER, IR_COMMAND_PLAY);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_SONY_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.command, 0b01011000);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_sony(IR_DEVICE_BLUE_RAY_PLAYER, IR_COMMAND_STOP);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_SONY_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.command, 0b00011000);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_sony(IR_DEVICE_BLUE_RAY_PLAYER, IR_COMMAND_PAUSE);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_SONY_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.command, 0b10011000);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_sony(IR_DEVICE_BLUE_RAY_PLAYER, IR_COMMAND_AUDIO_LANGUAGE);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_SONY_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.command, 0b00100110);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_sony(IR_DEVICE_BLUE_RAY_PLAYER, IR_COMMAND_SUBTITLE_LANGUAGE);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_SONY_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.command, 0b11000110);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_sony(IR_DEVICE_BLUE_RAY_PLAYER, IR_COMMAND_EJECT);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_SONY_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.command, 0b01101000);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_sony(IR_DEVICE_BLUE_RAY_PLAYER, IR_COMMAND_RETURN);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_SONY_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.command, 0b11000010);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_sony(IR_DEVICE_BLUE_RAY_PLAYER, IR_COMMAND_TOP_MENU);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_SONY_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.command, 0b00110100);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_sony(IR_DEVICE_BLUE_RAY_PLAYER, IR_COMMAND_POP_UP_MENU);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_SONY_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.command, 0b10010100);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_sony(IR_DEVICE_BLUE_RAY_PLAYER, IR_COMMAND_NEXT);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_SONY_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.command, 0b01101010);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_sony(IR_DEVICE_BLUE_RAY_PLAYER, IR_COMMAND_PREVIOUS);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_SONY_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.command, 0b11101010);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_ir_invalid_device_operation(void) {

	UT_START_TEST_CASE("Invalid Device Operation")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INVALID_DEVICE_OPERATION);

		unittest_reset_counter();
		u8 err_code = rpi_cmd_handler_ir_remote_sony(IR_DEVICE_TV, IR_COMMAND_EJECT);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_ERR_INVALID_ARGUMENT);
		UT_CHECK_IS_EQUAL(counter_SLOT_SONY_IR_CMD_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.device, 0);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.extended, 0);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.command, 0);

		unittest_reset_counter();
		err_code = rpi_cmd_handler_ir_remote_sony(IR_DEVICE_BLUE_RAY_PLAYER, IR_COMMAND_PROGRAM_GUIDE);
		unittest_wait_ms(UT_SIGNAL_TIMEOUT_MS);
		
		UT_CHECK_IS_EQUAL(err_code, CMD_ERR_INVALID_ARGUMENT);
		UT_CHECK_IS_EQUAL(counter_SLOT_SONY_IR_CMD_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.device, 0);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.extended, 0);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.command, 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_ir_cancel_ir_command(void) {

	UT_START_TEST_CASE("Cancel IR-Command")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_CANCEL_IR_COMMAND);

		unittest_reset_counter();

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();

			if (counter_TIMER1_START != 0) {
				if (counter_TIMER1_STOP < 2) {
					if (p_irq_callback != NULL) {
						p_irq_callback();
					}
				}
			}
		}
		
		UT_CHECK_IS_EQUAL(ir_protocol_sony_is_busy(), 0);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.device, 0);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.extended, 0);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.command, 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_ir_transmit_ir_command_power_on(void) {

	UT_START_TEST_CASE("Transmit IR-Command Power On")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_TRANSMIT_IR_COMMAND_POWER_ON);

		unittest_reset_counter();
		u8 err_code = rpi_cmd_handler_ir_remote_sony(IR_DEVICE_BLUE_RAY_PLAYER, IR_COMMAND_POWER_ON);

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();

			if (counter_TIMER1_START != 0) {
				if (counter_TIMER1_STOP < 2) {
					if (p_irq_callback != NULL) {
						p_irq_callback();
					}
				}
			}
		}

		u8 compare_MOD_ARRAY[] = {0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00};
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_SONY_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.command, 0b10101000);
		UT_CHECK_IS_EQUAL(counter_IR_MOD_SEQUENCE, sizeof(compare_MOD_ARRAY));
		UT_COMPARE_ARRAY(array_IR_MOD_SEQUENCE, compare_MOD_ARRAY, counter_IR_MOD_SEQUENCE);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_ir_transmit_ir_command_audio_language(void) {

	UT_START_TEST_CASE("Transmit IR-Command Audio Language")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_TRANSMIT_IR_COMMAND_AUDIO_LANGUAGE);

		unittest_reset_counter();
		u8 err_code = rpi_cmd_handler_ir_remote_sony(IR_DEVICE_BLUE_RAY_PLAYER, IR_COMMAND_AUDIO_LANGUAGE);

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();

			if (counter_TIMER1_START != 0) {
				if (counter_TIMER1_STOP < 2) {
					if (p_irq_callback != NULL) {
						p_irq_callback();
					}
				}
			}
		}

		u8 compare_MOD_ARRAY[] = {0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00};
		
		UT_CHECK_IS_EQUAL(err_code, CMD_NO_ERR);
		UT_CHECK_IS_EQUAL(counter_SLOT_SONY_IR_CMD_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.device, 0b00001011);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.extended, 0b01000111);
		UT_CHECK_IS_EQUAL(ir_command_received_via_signal.command, 0b00100110);
		UT_CHECK_IS_EQUAL(counter_IR_MOD_SEQUENCE, sizeof(compare_MOD_ARRAY));
		UT_COMPARE_ARRAY(array_IR_MOD_SEQUENCE, compare_MOD_ARRAY, counter_IR_MOD_SEQUENCE);
	}
	UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

	UT_START_TESTBENCH("Welcome the the UNITTEST for IR-Remote Sony 1.0")
	{
		TEST_CASE_initialization();

		UT_SONY_IR_CMD_RECEIVED_SLOT_connect();

		TEST_CASE_ir_command_build();
		TEST_CASE_ir_device_operation();
		TEST_CASE_ir_invalid_device_operation();
		TEST_CASE_ir_cancel_ir_command();
		TEST_CASE_ir_transmit_ir_command_power_on();
		TEST_CASE_ir_transmit_ir_command_audio_language();
	}
	UT_END_TESTBENCH()

	return UT_TEST_RESULT();
}
