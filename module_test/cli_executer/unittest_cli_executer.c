/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	unittest_cli_executer.c
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

#include "common/signal_slot_interface.h"
#include "mcu_task_management/mcu_task_interface.h"

#include "ui/file_interface/file_interface.h"
#include "ui/cfg_file_parser/cfg_file_parser.h"
#include "ui/console/ui_console.h"

#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

#include "app_tasks/cli_executer_task.h"

// --------------------------------------------------------------------------------

#include "../unittest.h"

UT_ACTIVATE()

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INITIALIZE					0
#define TEST_CASE_ID_COMMAND_RECEIVED				1
#define TEST_CASE_ID_DATE_COMMAND_IN_BACKGROUND			2
#define TEST_CASE_ID_DATE_COMMAND_IN_BACKGROUND_WHITE_SPACE	3
#define TEST_CASE_ID_DATE_COMMAND_UNKNOWN			4

// --------------------------------------------------------------------------------

#define MAX_MSG_LENGTH						255

#define TEST_CASE_COMMAND_RECEVIED_DATE					"date"
#define TEST_CASE_COMMAND_RECEVIED_DATE_IN_BACKGROUND			"date&"
#define TEST_CASE_COMMAND_RECEVIED_DATE_IN_BACKGROUND_WHITESPACES	"  date&  "
#define TEST_CASE_COMMAND_RECEIVED_UNKNOWN				"unknown_command"
#define NULL_STRING							"\0"

// --------------------------------------------------------------------------------

char unittest_COMMAND_TO_SEND[MAX_MSG_LENGTH];
char unittest_RESPONSE_RECEIVED[MAX_MSG_LENGTH];

u8 counter_RESPONSE_RECEIVED_SIGANL = 0;
u8 counter_COMMAND_TIMEOUT_SIGNAL = 0;
u8 counter_COMMAND_NOT_FOUND_SIGNAL = 0;

// --------------------------------------------------------------------------------


static void unittest_reset_counter(void) {

	memset(unittest_COMMAND_TO_SEND, '\0', MAX_MSG_LENGTH);
	memset(unittest_RESPONSE_RECEIVED, '\0', MAX_MSG_LENGTH);

	counter_RESPONSE_RECEIVED_SIGANL = 0;
	counter_COMMAND_TIMEOUT_SIGNAL = 0;
	counter_COMMAND_NOT_FOUND_SIGNAL = 0;
}

// --------------------------------------------------------------------------------

// stubs

// --------------------------------------------------------------------------------

// slots

void unittest_CLI_EXECUTER_COMMAND_RESPONSE_SLOT_CALLBACK(const void* p_argument) {
	DEBUG_TRACE_STR((const char*)p_argument, "unittest_CLI_EXECUTER_COMMAND_RESPONSE_SLOT_CALLBACK()");
	counter_RESPONSE_RECEIVED_SIGANL += 1;
}

void unittest_CLI_EXECUTER_COMMAND_TIMEOUT_CALLBACK(const void* p_argument) {
	DEBUG_PASS("unittest_CLI_EXECUTER_COMMAND_TIMEOUT_CALLBACK()");
	counter_COMMAND_TIMEOUT_SIGNAL += 1;
}

void unittest_CLI_EXECUTER_COMMAND_NOT_FOUND_CALLBACK(const void* p_argument) {
	DEBUG_PASS("unittest_CLI_EXECUTER_COMMAND_NOT_FOUND_CALLBACK()");
	counter_COMMAND_NOT_FOUND_SIGNAL += 1;
}

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_EXECUTER_COMMAND_RESPONSE_SIGNAL, UT_CLI_EXECUTER_COMMAND_RESPONSE_SLOT, unittest_CLI_EXECUTER_COMMAND_RESPONSE_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_EXECUTER_COMMAND_TIMEOUT_SIGNAL, UT_CLI_EXECUTER_COMMAND_TIMEOUT_SLOT, unittest_CLI_EXECUTER_COMMAND_TIMEOUT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_EXECUTER_COMMAND_NOT_FOUND_SIGNAL, UT_CLI_EXECUTER_COMMAND_NOT_FOUND_SLOT, unittest_CLI_EXECUTER_COMMAND_NOT_FOUND_CALLBACK)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(UNITTEST_TIMER)

// --------------------------------------------------------------------------------

static void UNITTEST_cli_executer_init(void) {
	
	UT_START_TEST_CASE("CLI_EXECUTER_INITIALIZE")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);

		unittest_reset_counter();

		cli_executer_init();

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_RESPONSE_RECEIVED_SIGANL, 0);
		UT_CHECK_IS_EQUAL(counter_COMMAND_TIMEOUT_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_COMMAND_NOT_FOUND_SIGNAL, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_cli_executer_execution_command(void) {
	
	UT_START_TEST_CASE("CLI_EXECUTER_EXECUTION_COMMAND")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_COMMAND_RECEIVED);

		unittest_reset_counter();

		CLI_EXECUTER_COMMAND_RECEIVED_SIGNAL_send(TEST_CASE_COMMAND_RECEVIED_DATE);

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_RESPONSE_RECEIVED_SIGANL, 1);
		UT_CHECK_IS_EQUAL(counter_COMMAND_TIMEOUT_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_COMMAND_NOT_FOUND_SIGNAL, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_cli_executer_execution_command_in_background(void) {
	
	UT_START_TEST_CASE("CLI_EXECUTER_EXECUTION_COMMAND_IN_BACKGROUND")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_DATE_COMMAND_IN_BACKGROUND);

		unittest_reset_counter();

		CLI_EXECUTER_COMMAND_RECEIVED_SIGNAL_send(TEST_CASE_COMMAND_RECEVIED_DATE_IN_BACKGROUND);

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_RESPONSE_RECEIVED_SIGANL, 1);
		UT_CHECK_IS_EQUAL(counter_COMMAND_TIMEOUT_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_COMMAND_NOT_FOUND_SIGNAL, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_cli_executer_execution_command_in_background_white_space(void) {
	
	UT_START_TEST_CASE("CLI_EXECUTER_EXECUTION_COMMAND_IN_BACKGROUND_WHITE_SPACE")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_DATE_COMMAND_IN_BACKGROUND_WHITE_SPACE);

		unittest_reset_counter();

		CLI_EXECUTER_COMMAND_RECEIVED_SIGNAL_send(TEST_CASE_COMMAND_RECEVIED_DATE_IN_BACKGROUND_WHITESPACES);

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_RESPONSE_RECEIVED_SIGANL, 1);
		UT_CHECK_IS_EQUAL(counter_COMMAND_TIMEOUT_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_COMMAND_NOT_FOUND_SIGNAL, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_cli_executer_execution_command_unknown(void) {
	
	UT_START_TEST_CASE("CLI_EXECUTER_EXECUTION_COMMAND_UNKNOWN")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_DATE_COMMAND_UNKNOWN);

		unittest_reset_counter();

		CLI_EXECUTER_COMMAND_RECEIVED_SIGNAL_send(TEST_CASE_COMMAND_RECEIVED_UNKNOWN);

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_RESPONSE_RECEIVED_SIGANL, 0);
		UT_CHECK_IS_EQUAL(counter_COMMAND_TIMEOUT_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_COMMAND_NOT_FOUND_SIGNAL, 1);
	}
	UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

	//TRACER_DISABLE();

	UT_START_TESTBENCH("Welcome the the UNITTEST for msg-executer v1.0")
	{
		UNITTEST_cli_executer_init();

		UT_CLI_EXECUTER_COMMAND_RESPONSE_SLOT_connect();
		UT_CLI_EXECUTER_COMMAND_TIMEOUT_SLOT_connect();
		UT_CLI_EXECUTER_COMMAND_NOT_FOUND_SLOT_connect();

		UNITTEST_cli_executer_execution_command();
		UNITTEST_cli_executer_execution_command_in_background();
		UNITTEST_cli_executer_execution_command_in_background_white_space();
		UNITTEST_cli_executer_execution_command_unknown();
	}
	UT_END_TESTBENCH()

	return UT_TEST_RESULT();
}

// --------------------------------------------------------------------------------
