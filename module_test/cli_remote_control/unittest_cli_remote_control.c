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

#include "common/common_tools_string.h"
#include "ui/command_line/command_line_interface.h"
#include "ui/command_line/command_line_handler_remote_control.h"
#include "common/signal_slot_interface.h"
#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_SAMSUNG_COMMANDS			1
#define TEST_CASE_ID_JVC_COMMANDS			2

// --------------------------------------------------------------------------------

// counters

static u8 counter_RPI_HOST_COMMAND_RECEIVED = 0;

static COMMON_GENERIC_BUFFER_TYPE remote_control_command;

static void unittest_reset_counter(void) {

	counter_RPI_HOST_COMMAND_RECEIVED = 0;

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

	DEBUG_TRACE_N(remote_control_command.length, remote_control_command.data, "ut_RPI_HOST_COMMAND_RECEIVED_CALLBACK()");
}

// --------------------------------------------------------------------------------

// Signals / Slots

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CFG_PARSER_NEW_CFG_OBJECT_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_HOST_COMMAND_RECEIVED_SIGNAL)

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(RPI_HOST_COMMAND_RECEIVED_SIGNAL, UT_RPI_HOST_COMMAND_RECEIVED_SLOT, ut_RPI_HOST_COMMAND_RECEIVED_CALLBACK)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(UNITTEST_TIMER)

// --------------------------------------------------------------------------------


static void ut_get_command_array(COMMON_GENERIC_BUFFER_TYPE* p_buffer, char* cmd_hex_str) {

	p_buffer->length = common_tools_hex_string_to_byte_array(
				cmd_hex_str,
				CLI_REMOTE_CONTROL_CMD_HEX_STR_LENGTH,
				p_buffer->data,
				COMMON_TYPES_GENERIC_BUFFER_SIZE
			);
}


// --------------------------------------------------------------------------------

static void TEST_CASE_samsung_commands(void) {

	UT_START_TEST_CASE("SAMSUNG Commands")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_SAMSUNG_COMMANDS);

		COMMON_GENERIC_BUFFER_TYPE buffer;

		// POWER ON

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_POWER_ON);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_POWER_ON);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// VOLUME DOWN

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_VOLUME_DOWN);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_VOLUME_DOWN);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// VOLUME UP

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_VOLUME_UP);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_VOLUME_UP);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// VOLUME MUTE

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_VOLUME_MUTE);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_VOLUME_MUTE);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// PLAY

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_PLAY);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_PLAY);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// PAUSE

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_PAUSE);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_PAUSE);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// STOP

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_STOP);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_STOP);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// CHANNEL_UP

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_CHANNEL_UP);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_CHANNEL_UP);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// CHANNEL_DOWN

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_CHANNEL_DOWN);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_CHANNEL_DOWN);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// CHANNEL_LIST

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_CHANNEL_LIST);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_CHANNEL_LIST);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// CHANNEL_1

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_CHANNEL_1);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_CHANNEL_1);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// CHANNEL_2

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_CHANNEL_2);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_CHANNEL_2);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// CHANNEL_3

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_CHANNEL_3);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_CHANNEL_3);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// CHANNEL_4

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_CHANNEL_4);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_CHANNEL_4);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// CHANNEL_5

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_CHANNEL_5);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_CHANNEL_5);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// CHANNEL_6

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_CHANNEL_6);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_CHANNEL_6);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// CHANNEL_7

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_CHANNEL_7);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_CHANNEL_7);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// CHANNEL_8

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_CHANNEL_8);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_CHANNEL_8);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// CHANNEL_9

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_CHANNEL_9);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_CHANNEL_9);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// CHANNEL_0

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_CHANNEL_0);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_CHANNEL_0);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// CHANNEL_0

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_CHANNEL_0);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_CHANNEL_0);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// ARROW_UP

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_ARROW_UP);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_ARROW_UP);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// ARROW_DOWN

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_ARROW_DOWN);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_ARROW_DOWN);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// ARROW_LEFT

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_ARROW_LEFT);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_ARROW_LEFT);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// ENTER

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_ENTER);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_ENTER);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// RETURN

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_RETURN);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_RETURN);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// EXIT

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_EXIT);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_EXIT);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// GUIDE

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_GUIDE);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_GUIDE);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// HOME

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_HOME);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_HOME);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// SOURCE

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_SAMSUNG_TV_SOURCE);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_SAMSUNG_TV_SOURCE);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);
	}
	UT_END_TEST_CASE()
}

static void TEST_CASE_jvc_commands(void) {

	UT_START_TEST_CASE("JVC Commands")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_JVC_COMMANDS);

		COMMON_GENERIC_BUFFER_TYPE buffer;

		// POWER ON

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_JVC_RADIO_POWER_ON);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_JVC_RADIO_POWER_ON);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// VOLUME_DOWN

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_JVC_RADIO_VOLUME_DOWN);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_JVC_RADIO_VOLUME_DOWN);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// VOLUME UP

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_JVC_RADIO_VOLUME_UP);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_JVC_RADIO_VOLUME_UP);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// VOLUME MUTE

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_JVC_RADIO_VOLUME_MUTE);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_JVC_RADIO_VOLUME_MUTE);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// PLAY

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_JVC_RADIO_PLAY);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_JVC_RADIO_PLAY);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// PAUSE

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_JVC_RADIO_PAUSE);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_JVC_RADIO_PAUSE);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// STOP

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_JVC_RADIO_STOP);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_JVC_RADIO_STOP);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// BASS_UP

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_JVC_RADIO_BASS_UP);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_JVC_RADIO_BASS_UP);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// BASS_DOWN

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_JVC_RADIO_BASS_DOWN);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_JVC_RADIO_BASS_DOWN);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);

		// SOUND_MODE

		unittest_reset_counter();
		command_line_handler_remote_control(CLI_REMOTE_CONTROL_NAME_JVC_RADIO_SOUND_MODE);
		ut_get_command_array(&buffer, CLI_REMOTE_CONTROL_CMD_HEX_STR_JVC_RADIO_SOUND_MODE);

		UT_CHECK_IS_EQUAL(counter_RPI_HOST_COMMAND_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(remote_control_command.length, buffer.length);
		UT_COMPARE_ARRAY(remote_control_command.data, buffer.data, buffer.length);
	}
	UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

	RPI_HOST_COMMAND_RECEIVED_SIGNAL_init();
	RPI_HOST_COMMAND_RECEIVED_SIGNAL_set_timeout(0);

	UT_START_TESTBENCH("Welcome the the UNITTEST CLI-REMOTE-CONTROL 1.0")
	{
		UT_RPI_HOST_COMMAND_RECEIVED_SLOT_connect();
		
		TEST_CASE_samsung_commands();
		TEST_CASE_jvc_commands();
	}
	UT_END_TESTBENCH()

	return UT_TEST_RESULT();
}
