
 /*
  * \@file	src/module_test/rpi_protocol_handler/testcase_command_answer_success.c
  * \author	sebastian lesse
  */

#define TRACER_ON

// -------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden! v

// -------------------------------------------------------------------

#include "tracer.h"

// -------------------------------------------------------------------

#include "driver/trx_driver_interface.h"
#include "common/signal_slot_interface.h"
#include "time_management/time_management.h"
#include "driver/communication/test/communication_driver_test.h"

#include "protocol_management/rpi_protocol_handler.h"
#include "command_management/protocol_interface.h"
#include "command_management/command_handler_interface.h"
#include "common/local_mutex.h"

#include <stdio.h>
#include <stdlib.h>

// -------------------------------------------------------------------

#define CHECK_IS_EQUAL(a,b)					if (a != b) {								\
									printf("\nCHECK_IS_EQUAL() - FAILED !!! (%d != %d) - %s:%d\n", a, b, __FILE__, __LINE__);	\
								} do{}while(0)

#define TESTCASE_COMMAND_ANSWER_SUCCESS_TEST_COUNT		10
#define TESTCASE_COMMAND_ANSWER_SUCCESS_SLEEP_TIME_US		50

// -------------------------------------------------------------------

static void testcase_command_answer_success_slot_cmd_received_CALLBACK(void* p_arg);

static void testcase_command_answer_success_slot_start_rx_CALLBACK(void* p_arg);

static void testcase_command_answer_success_slot_start_tx_CALLBACK(void* p_arg);

static void run_module_under_test(u16 run_time_ms);

// -------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(SIGNAL_CMD_RECEIVED, TESTCASE_SUCCESS_SLOT_CMD_RECEIVED, testcase_command_answer_success_slot_cmd_received_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(SIGNAL_TEST_DRIVER_START_RX, TESTCASE_SUCCESS_SLOT_START_RX, testcase_command_answer_success_slot_start_rx_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(SIGNAL_TEST_DRIVER_START_TX, TESTCASE_SUCCESS_SLOT_START_TX, testcase_command_answer_success_slot_start_tx_CALLBACK)

TIME_MGMN_BUILD_STATIC_TIMER_U16(RUN_TIMER)

// -------------------------------------------------------------------

static u8 command_received = 0;

static u8 test_is_active = 0;

static PROTOCOL_INTERFACE* p_act_protocol;

// -------------------------------------------------------------------

u8 testcase_command_answer_success( TRX_DRIVER_INTERFACE* p_com_driver ) {

	DEBUG_PASS("TEST_CASE - command_answer_success - start()");

	TESTCASE_SUCCESS_SLOT_CMD_RECEIVED_connect();
	TESTCASE_SUCCESS_SLOT_START_RX_connect();
	TESTCASE_SUCCESS_SLOT_START_TX_connect();

	u8 test_count = 0;

	while (test_count++ < TESTCASE_COMMAND_ANSWER_SUCCESS_TEST_COUNT) {

		DEBUG_TRACE_byte(test_count, " ------------- RUNNING TST NUMBER: ");
		
		test_is_active = 1;
		command_received = 0;

		READY_INOUT_drive_low(); // set low to request handler for command
		run_module_under_test(50);

		READY_INOUT_pull_up(); // set high-z to let handler response
		run_module_under_test(30);

		if (READY_INOUT_is_high_level()) {
			DEBUG_PASS("TEST_CASE - command_answer_success - Request for Command - READY_PIN is still high !!! ---");
			return 1;
		}

		if (command_received == 0) {
			DEBUG_PASS("TEST_CASE - command_answer_success - command has not been received !!! ---");
			return 1;
		}

		p_act_protocol->answ_buffer->start_write();
		p_act_protocol->answ_buffer->add_byte(VERSION_MAJOR);
		p_act_protocol->answ_buffer->add_byte(VERSION_MINOR);
		p_act_protocol->answ_buffer->stop_write();

		p_act_protocol->set_finished(CMD_NO_ERR);

		run_module_under_test(50);

		READY_INOUT_drive_low(); // set low to request handler for answer
		run_module_under_test(50);

		READY_INOUT_pull_up(); // set high-z to let handler response
		run_module_under_test(50);

		if (READY_INOUT_is_low_level()) {
			DEBUG_PASS("TEST_CASE - command_answer_success - End of conversation - READY_PIN IS STILL HIGH !!! ---");
			return 1;
		}

		u8 test_driver_mutex = p_com_driver->mutex_req();
		if (test_driver_mutex == MUTEX_INVALID_ID) {
			DEBUG_PASS("TEST_CASE - command_answer_success - End of conversation - TEST-DRIVER-MUTEX IS STILL IN USE !!! ---");
			return 1;
		}
		p_com_driver->mutex_rel(test_driver_mutex);

		if (rpi_protocol_task_get_state() != MCU_TASK_SLEEPING) {
			DEBUG_PASS("TEST_CASE - command_answer_success - End of conversation - RPI-PROTOCOL-HANDLER IS NOT SLEEPING !!! ---");
			return 1;
		}

		usleep(TESTCASE_COMMAND_ANSWER_SUCCESS_SLEEP_TIME_US); // reduce cpu-load
	}

	return 0;
}

// -------------------------------------------------------------------

static void testcase_command_answer_success_slot_cmd_received_CALLBACK(void* p_arg) {

	if (test_is_active == 0) {
		return;
	}

	command_received = 1;
	p_act_protocol = (PROTOCOL_INTERFACE*)p_arg;

	DEBUG_TRACE_byte(p_act_protocol->command_code, "TEST_CASE - command_answer_success - signal_slot_callback - cmd_received() - command_code :");
}

static void testcase_command_answer_success_slot_start_rx_CALLBACK(void* p_arg) {

	if (test_is_active == 0) {
		return;
	}

	DEBUG_PASS("TEST_CASE - command_answer_success - signal_slot_callback - start-RX ()");

	u8 command_version[] = {0x01, 0x01};
	test_driver_set_rx_bytes(sizeof(command_version), command_version);
}

static void testcase_command_answer_success_slot_start_tx_CALLBACK(void* p_arg) {

	if (test_is_active == 0) {
		return;
	}

	u8 answer_version[5];

	if (test_driver_get_tx_bytes_num_available() > 5) {
		DEBUG_TRACE_byte(test_driver_get_tx_bytes_num_available(), "TEST_CASE - command_answer_success - signal_slot_callback - start-TX - TO MUCH DATA !!! ---");
		return;
	}

	test_driver_get_tx_bytes(5, answer_version);

	DEBUG_TRACE_N(5, answer_version, "TEST_CASE - command_answer_success - signal_slot_callback - start-TX - ANSWER:");

	CHECK_IS_EQUAL(0x04, answer_version[0]);		// length
	CHECK_IS_EQUAL(0x01, answer_version[1]);		// command-code
	CHECK_IS_EQUAL(0x00, answer_version[2]);		// status
	CHECK_IS_EQUAL(VERSION_MAJOR, answer_version[3]);	// major-version
	CHECK_IS_EQUAL(VERSION_MINOR, answer_version[4]);	// minor-version
}

static void run_module_under_test(u16 run_time_ms) {
	
	u32 run_counter = 0;

	RUN_TIMER_start();
	
	do {
		if (rpi_protocol_task_get_state() == MCU_TASK_RUNNING) {
			rpi_protocol_task_run();
			run_counter += 1;
		}

	} while (RUN_TIMER_is_up(run_time_ms) == 0);

	DEBUG_TRACE_long(run_counter, "TEST_CASE - command_answer_success - run_module() - run-counter: ");
}
