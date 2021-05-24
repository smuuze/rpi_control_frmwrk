/**
 * @file 	unittest_json_parser.c
 * @author 	sebastian lesse (sebastian lesse)
 * @brief 
 * @version 	1.0
 * @date 	2021-02-06
 * 
 * @copyright 	Copyright (c) 2021
 * 
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"
// --------------------------------------------------------------------------------

#include "../unittest.h"

UT_ACTIVATE()

// --------------------------------------------------------------------------------

#include <string.h>

#include "ui/command_line/command_line_interface.h"
#include "ui/command_line/command_line_handler_remote_control.h"
#include "common/signal_slot_interface.h"
#include "time_management/time_management.h"
#include "mcu_task_management/mcu_task_interface.h"

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INITIALIZE			1
#define TEST_CASE_ID_UNKNOWN_COMMAND		2
#define TEST_CASE_ID_JVC_RADIO_REMOTE_CONTROL	3
#define TEST_CASE_ID_NO_ARGUMENT_GIVEN		4

// --------------------------------------------------------------------------------

// counters

static u8 counter_RPI_HOST_COMMAND_RECEIVED = 0;
static u8 counter_CLI_INVALID_ARGUMENT = 0;
static u8 counter_CLI_INVALID_PARAMETER = 0;
static u8 counter_CLI_HELP_REQUESTED = 0;
static u8 counter_CLI_LCD_ACTIVATED = 0;
static u8 counter_CLI_CONSOLE_ACTIVATED = 0;
static u8 counter_CLI_CONFIGURATION = 0;
static u8 counter_CLI_MESSAGE = 0;
static u8 counter_CLI_UNKNOWN_ARGUMENT = 0;
static u8 counter_CLI_NO_ARGUMENT_GIVEN = 0;

static COMMON_GENERIC_BUFFER_TYPE remote_control_command;

static void unittest_reset_counter(void) {

	counter_RPI_HOST_COMMAND_RECEIVED = 0;
	counter_CLI_INVALID_ARGUMENT = 0;
	counter_CLI_INVALID_PARAMETER = 0;
	counter_CLI_HELP_REQUESTED = 0;
	counter_CLI_LCD_ACTIVATED = 0;
	counter_CLI_CONSOLE_ACTIVATED = 0;
	counter_CLI_CONFIGURATION = 0;
	counter_CLI_MESSAGE = 0;
	counter_CLI_UNKNOWN_ARGUMENT = 0;
	counter_CLI_NO_ARGUMENT_GIVEN = 0;

	remote_control_command.length = 0;
	memset(remote_control_command.data, 0x00, COMMON_TYPES_GENERIC_BUFFER_SIZE);
}

// --------------------------------------------------------------------------------

// stubs

// --------------------------------------------------------------------------------

// slots callbacks

static void ut_RPI_HOST_COMMAND_RECEIVED_CALLBACK(const void* p_argument) {
	(void) p_argument;
	counter_RPI_HOST_COMMAND_RECEIVED += 1;

	COMMON_GENERIC_BUFFER_TYPE* p_cmd_buffer = (COMMON_GENERIC_BUFFER_TYPE*) p_argument;

	remote_control_command.length = p_cmd_buffer->length;
	memcpy(remote_control_command.data, p_cmd_buffer->data, p_cmd_buffer->length);

	DEBUG_PASS("ut_RPI_HOST_COMMAND_RECEIVED_CALLBACK()");
}

static void ut_CLI_INVALID_ARGUMENT_CALLBACK(const void* p_argument) {
	(void) p_argument;
	counter_CLI_INVALID_ARGUMENT += 1;
}

static void ut_CLI_INVALID_PARAMETER_CALLBACK(const void* p_argument) {
	(void) p_argument;
	counter_CLI_INVALID_PARAMETER += 1;
}

static void ut_CLI_HELP_REQUESTED_CALLBACK(const void* p_argument) {
	(void) p_argument;
	counter_CLI_HELP_REQUESTED += 1;
}

static void ut_CLI_LCD_ACTIVATED_CALLBACK(const void* p_argument) {
	(void) p_argument;
	counter_CLI_LCD_ACTIVATED += 1;
}

static void ut_CLI_CONSOLE_ACTIVATED_CALLBACK(const void* p_argument) {
	(void) p_argument;
	counter_CLI_CONSOLE_ACTIVATED += 1;
}

static void ut_CLI_CONFIGURATION_CALLBACK(const void* p_argument) {
	(void) p_argument;
	counter_CLI_CONFIGURATION += 1;
}

static void ut_CLI_MESSAGE_CALLBACK(const void* p_argument) {
	(void) p_argument;
	counter_CLI_MESSAGE += 1;
}

static void ut_CLI_NO_ARGUMENT_GIVEN_CALLBACK(const void* p_argument) {
	(void) p_argument;
	DEBUG_PASS("ut_CLI_NO_ARGUMENT_GIVEN_CALLBACK()");
	counter_CLI_NO_ARGUMENT_GIVEN += 1;
}

static void ut_CLI_UNKNOWN_ARGUMENT_CALLBACK(const void* p_argument) {
	(void) p_argument;
	counter_CLI_UNKNOWN_ARGUMENT += 1;

	__UNUSED__ const COMMAND_LINE_ARGUMENT_TYPE* unknown_argument = (COMMAND_LINE_ARGUMENT_TYPE*) p_argument;

	DEBUG_TRACE_STR(unknown_argument->argument, "ut_CLI_UNKNOWN_ARGUMENT_CALLBACK() - UNKNOWN CLI-ARGUMENT: ");
}

// --------------------------------------------------------------------------------

// Signals / Slots

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CFG_PARSER_NEW_CFG_OBJECT_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_HOST_COMMAND_RECEIVED_SIGNAL)

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(RPI_HOST_COMMAND_RECEIVED_SIGNAL, UT_RPI_HOST_COMMAND_RECEIVED_SLOT, ut_RPI_HOST_COMMAND_RECEIVED_CALLBACK)

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_INVALID_ARGUMENT_SIGNAL, 	UT_CLI_INVALID_ARGUMENT_SLOT, 	ut_CLI_INVALID_ARGUMENT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_INVALID_PARAMETER_SIGNAL, UT_CLI_INVALID_PARAMETER_SLOT, 	ut_CLI_INVALID_PARAMETER_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_HELP_REQUESTED_SIGNAL, 	UT_CLI_HELP_REQUESTED_SLOT, 	ut_CLI_HELP_REQUESTED_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_LCD_ACTIVATED_SIGNAL, 	UT_CLI_LCD_ACTIVATED_SLOT, 	ut_CLI_LCD_ACTIVATED_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_CONSOLE_ACTIVATED_SIGNAL, UT_CLI_CONSOLE_ACTIVATED_SLOT, 	ut_CLI_CONSOLE_ACTIVATED_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_CONFIGURATION_SIGNAL, 	UT_CLI_CONFIGURATION_SLOT, 	ut_CLI_CONFIGURATION_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_MESSAGE_SIGNAL, 		UT_CLI_MESSAGE_SLOT, 		ut_CLI_MESSAGE_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_UNKNOWN_ARGUMENT_SIGNAL, 	UT_CLI_UNKNOWN_ARGUMENT_SLOT, 	ut_CLI_UNKNOWN_ARGUMENT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_NO_ARGUMENT_GIVEN_SIGNAL, UT_CLI_NO_ARGUMENT_GIVEN_SLOT, 	ut_CLI_NO_ARGUMENT_GIVEN_CALLBACK)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(UNITTEST_TIMER)

// --------------------------------------------------------------------------------

static void TEST_CASE_initialization(void) {

	UT_START_TEST_CASE("initialization")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);
		unittest_reset_counter();

		command_line_interface_init();

		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_HELP_REQUESTED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_LCD_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONSOLE_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONFIGURATION, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_MESSAGE, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_UNKNOWN_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_NO_ARGUMENT_GIVEN, 0);
	}
	UT_END_TEST_CASE()

}

static void TEST_CASE_unknown_argument(void) {

	UT_START_TEST_CASE("only unknown argument")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_UNKNOWN_COMMAND);
		unittest_reset_counter();

		int argc = 2;
		char* argv[] = {"-eins", "zwei"};

		command_line_interface(argc, argv);

		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_HELP_REQUESTED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_LCD_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONSOLE_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONFIGURATION, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_MESSAGE, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_UNKNOWN_ARGUMENT, 1);
		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_NO_ARGUMENT_GIVEN, 0);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_no_argument_given(void) {

	UT_START_TEST_CASE("no argument given")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_NO_ARGUMENT_GIVEN);
		unittest_reset_counter();

		int argc = 0;
		char* argv[] = {"-eins"};

		command_line_interface(argc, argv);

		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_HELP_REQUESTED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_LCD_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONSOLE_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONFIGURATION, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_MESSAGE, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_UNKNOWN_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_NO_ARGUMENT_GIVEN, 1);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_jvc_radio_remote_control(void) {

	UT_START_TEST_CASE("jvc radio remote-control")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_JVC_RADIO_REMOTE_CONTROL);

		int argc = 2;
		char* arg_cmd_power_on[] 	= {COMMAND_LINE_ARGUMENT_REMOTE, CLI_REMOTE_CONTROL_NAME_JVC_RADIO_POWER_ON};
		char* arg_cmd_volume_down[] 	= {COMMAND_LINE_ARGUMENT_REMOTE, CLI_REMOTE_CONTROL_NAME_JVC_RADIO_VOLUME_DOWN};
		char* arg_cmd_volume_up[] 	= {COMMAND_LINE_ARGUMENT_REMOTE, CLI_REMOTE_CONTROL_NAME_JVC_RADIO_VOLUME_UP};
		char* arg_cmd_volume_mute[]	= {COMMAND_LINE_ARGUMENT_REMOTE, CLI_REMOTE_CONTROL_NAME_JVC_RADIO_VOLUME_MUTE};
		char* arg_cmd_play[] 		= {COMMAND_LINE_ARGUMENT_REMOTE, CLI_REMOTE_CONTROL_NAME_JVC_RADIO_PLAY};
		char* arg_cmd_pause[] 		= {COMMAND_LINE_ARGUMENT_REMOTE, CLI_REMOTE_CONTROL_NAME_JVC_RADIO_PAUSE};
		char* arg_cmd_stop[] 		= {COMMAND_LINE_ARGUMENT_REMOTE, CLI_REMOTE_CONTROL_NAME_JVC_RADIO_STOP};
		char* arg_cmd_bass_up[] 	= {COMMAND_LINE_ARGUMENT_REMOTE, CLI_REMOTE_CONTROL_NAME_JVC_RADIO_BASS_UP};
		char* arg_cmd_bass_down[] 	= {COMMAND_LINE_ARGUMENT_REMOTE, CLI_REMOTE_CONTROL_NAME_JVC_RADIO_BASS_DOWN};
		char* arg_cmd_sound_mode[] 	= {COMMAND_LINE_ARGUMENT_REMOTE, CLI_REMOTE_CONTROL_NAME_JVC_RADIO_SOUND_MODE};

		u8 cmd_power_on[]		= { 0x07, 0x0C, 0x11, 0x04, 0x0B, 0x02, 0x02, 0x01 };
		u8 cmd_volume_down[]		= { 0x07, 0x0C, 0x11, 0x04, 0x0B, 0x02, 0x02, 0x04 };
		u8 cmd_volume_up[]		= { 0x07, 0x0C, 0x11, 0x04, 0x0B, 0x02, 0x02, 0x03 };
		u8 cmd_volume_mute[]		= { 0x07, 0x0C, 0x11, 0x04, 0x0B, 0x02, 0x02, 0x1E };
		u8 cmd_play[]			= { 0x07, 0x0C, 0x11, 0x04, 0x0B, 0x02, 0x02, 0x18 };
		u8 cmd_pause[]			= { 0x07, 0x0C, 0x11, 0x04, 0x0B, 0x02, 0x02, 0x19 };
		u8 cmd_stop[]			= { 0x07, 0x0C, 0x11, 0x04, 0x0B, 0x02, 0x02, 0x1A };
		u8 cmd_bass_up[]		= { 0x07, 0x0C, 0x11, 0x04, 0x0B, 0x02, 0x02, 0x1B };
		u8 cmd_bass_down[]		= { 0x07, 0x0C, 0x11, 0x04, 0x0B, 0x02, 0x02, 0x1C };
		u8 cmd_sound_mode[]		= { 0x07, 0x0C, 0x11, 0x04, 0x0B, 0x02, 0x02, 0x1D };

		// --- POWER_ON ---

		unittest_reset_counter();
		command_line_interface(argc, arg_cmd_power_on);

		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_HELP_REQUESTED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_LCD_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONSOLE_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONFIGURATION, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_MESSAGE, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_UNKNOWN_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_CLI_NO_ARGUMENT_GIVEN, 0);

		UT_CHECK_IS_EQUAL(remote_control_command.length, sizeof(cmd_power_on));
		UT_COMPARE_ARRAY(remote_control_command.data, cmd_power_on, remote_control_command.length);

		// --- VOLUME_DOWN ---

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(250) == 0) { mcu_task_controller_schedule(); }

		unittest_reset_counter();
		command_line_interface(argc, arg_cmd_volume_down);

		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_HELP_REQUESTED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_LCD_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONSOLE_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONFIGURATION, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_MESSAGE, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_UNKNOWN_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_CLI_NO_ARGUMENT_GIVEN, 0);

		UT_CHECK_IS_EQUAL(remote_control_command.length, sizeof(cmd_volume_down));
		UT_COMPARE_ARRAY(remote_control_command.data, cmd_volume_down, remote_control_command.length);

		// --- VOLUME_UP ---

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(250) == 0) { mcu_task_controller_schedule(); }

		unittest_reset_counter();
		command_line_interface(argc, arg_cmd_volume_up);

		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_HELP_REQUESTED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_LCD_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONSOLE_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONFIGURATION, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_MESSAGE, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_UNKNOWN_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_CLI_NO_ARGUMENT_GIVEN, 0);

		UT_CHECK_IS_EQUAL(remote_control_command.length, sizeof(cmd_volume_up));
		UT_COMPARE_ARRAY(remote_control_command.data, cmd_volume_up, remote_control_command.length);

		// --- VOLUME_MUTE ---

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(250) == 0) { mcu_task_controller_schedule(); }

		unittest_reset_counter();
		command_line_interface(argc, arg_cmd_volume_mute);

		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_HELP_REQUESTED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_LCD_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONSOLE_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONFIGURATION, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_MESSAGE, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_UNKNOWN_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_CLI_NO_ARGUMENT_GIVEN, 0);

		UT_CHECK_IS_EQUAL(remote_control_command.length, sizeof(cmd_volume_mute));
		UT_COMPARE_ARRAY(remote_control_command.data, cmd_volume_mute, remote_control_command.length);

		// --- PLAY ---

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(250) == 0) { mcu_task_controller_schedule(); }

		unittest_reset_counter();
		command_line_interface(argc, arg_cmd_play);

		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_HELP_REQUESTED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_LCD_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONSOLE_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONFIGURATION, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_MESSAGE, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_UNKNOWN_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_CLI_NO_ARGUMENT_GIVEN, 0);

		UT_CHECK_IS_EQUAL(remote_control_command.length, sizeof(cmd_play));
		UT_COMPARE_ARRAY(remote_control_command.data, cmd_play, remote_control_command.length);

		// --- PAUSE ---

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(250) == 0) { mcu_task_controller_schedule(); }

		unittest_reset_counter();
		command_line_interface(argc, arg_cmd_pause);

		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_HELP_REQUESTED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_LCD_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONSOLE_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONFIGURATION, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_MESSAGE, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_UNKNOWN_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_CLI_NO_ARGUMENT_GIVEN, 0);

		UT_CHECK_IS_EQUAL(remote_control_command.length, sizeof(cmd_pause));
		UT_COMPARE_ARRAY(remote_control_command.data, cmd_pause, remote_control_command.length);

		// --- STOP ---

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(250) == 0) { mcu_task_controller_schedule(); }

		unittest_reset_counter();
		command_line_interface(argc, arg_cmd_stop);

		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_HELP_REQUESTED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_LCD_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONSOLE_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONFIGURATION, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_MESSAGE, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_UNKNOWN_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_CLI_NO_ARGUMENT_GIVEN, 0);

		UT_CHECK_IS_EQUAL(remote_control_command.length, sizeof(cmd_stop));
		UT_COMPARE_ARRAY(remote_control_command.data, cmd_stop, remote_control_command.length);

		// --- BASS_UP ---

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(250) == 0) { mcu_task_controller_schedule(); }

		unittest_reset_counter();
		command_line_interface(argc, arg_cmd_bass_up);

		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_HELP_REQUESTED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_LCD_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONSOLE_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONFIGURATION, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_MESSAGE, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_UNKNOWN_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_CLI_NO_ARGUMENT_GIVEN, 0);

		UT_CHECK_IS_EQUAL(remote_control_command.length, sizeof(cmd_volume_down));
		UT_COMPARE_ARRAY(remote_control_command.data, cmd_bass_up, remote_control_command.length);

		// --- BASS_DOWN ---

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(250) == 0) { mcu_task_controller_schedule(); }

		unittest_reset_counter();
		command_line_interface(argc, arg_cmd_bass_down);

		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_HELP_REQUESTED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_LCD_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONSOLE_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONFIGURATION, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_MESSAGE, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_UNKNOWN_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_CLI_NO_ARGUMENT_GIVEN, 0);

		UT_CHECK_IS_EQUAL(remote_control_command.length, sizeof(cmd_bass_down));
		UT_COMPARE_ARRAY(remote_control_command.data, cmd_bass_down, remote_control_command.length);

		// --- SOUND_MODE ---

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(250) == 0) { mcu_task_controller_schedule(); }

		unittest_reset_counter();
		command_line_interface(argc, arg_cmd_sound_mode);

		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_INVALID_PARAMETER, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_HELP_REQUESTED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_LCD_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONSOLE_ACTIVATED, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_CONFIGURATION, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_MESSAGE, 0);
		UT_CHECK_IS_EQUAL(counter_CLI_UNKNOWN_ARGUMENT, 0);
		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_CLI_NO_ARGUMENT_GIVEN, 0);

		UT_CHECK_IS_EQUAL(remote_control_command.length, sizeof(cmd_sound_mode));
		UT_COMPARE_ARRAY(remote_control_command.data, cmd_sound_mode, remote_control_command.length);

	}
	UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

	CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_init();
	RPI_HOST_COMMAND_RECEIVED_SIGNAL_init();

	UT_START_TESTBENCH("Welcome the the UNITTEST CLI-ARGUMENT 1.0")
	{
		TEST_CASE_initialization();

		UT_CLI_INVALID_ARGUMENT_SLOT_connect();
		UT_CLI_INVALID_PARAMETER_SLOT_connect();
		UT_CLI_HELP_REQUESTED_SLOT_connect();
		UT_CLI_LCD_ACTIVATED_SLOT_connect();
		UT_CLI_LCD_ACTIVATED_SLOT_connect();
		UT_CLI_CONSOLE_ACTIVATED_SLOT_connect();
		UT_CLI_CONFIGURATION_SLOT_connect();
		UT_CLI_MESSAGE_SLOT_connect();
		UT_CLI_UNKNOWN_ARGUMENT_SLOT_connect();
		UT_CLI_NO_ARGUMENT_GIVEN_SLOT_connect();

		UT_RPI_HOST_COMMAND_RECEIVED_SLOT_connect();
		
		TEST_CASE_no_argument_given();
		TEST_CASE_unknown_argument();
		TEST_CASE_jvc_radio_remote_control();
	}
	UT_END_TESTBENCH()

	return UT_TEST_RESULT();
}
