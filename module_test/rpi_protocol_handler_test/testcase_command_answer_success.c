
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

// -------------------------------------------------------------------

static void testcase_command_answer_success_slot_cmd_received_CALLBACK(void* p_arg);

static void run_module_under_test(u16 run_time_ms);

// -------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(SIGNAL_CMD_RECEIVED, TESTCASE_SUCCESS_SLOT_CMD_RECEIVED, testcase_command_answer_success_slot_cmd_received_CALLBACK)

TIME_MGMN_BUILD_STATIC_TIMER_U16(RUN_TIMER)

// -------------------------------------------------------------------

u8 testcase_command_answer_success( TRX_DRIVER_INTERFACE* p_com_driver ) {

	DEBUG_PASS("TEST_CASE - command_answer_success - start()");

	TESTCASE_SUCCESS_SLOT_CMD_RECEIVED_connect();

	{
		u8 command_version[] = {0x01, 0x01};

		READY_INOUT_drive_low();
		run_module_under_test(50);

		READY_INOUT_pull_up();
		run_module_under_test(25);

		test_driver_set_rx_bytes(sizeof(command_version), command_version);
		run_module_under_test(2500);
	}


	return 0;
}

// -------------------------------------------------------------------

static void testcase_command_answer_success_slot_cmd_received_CALLBACK(void* p_arg) {
	DEBUG_PASS("TEST_CASE - command_answer_success - signal_slot_callback - cmd_received ()");
}

static void run_module_under_test(u16 run_time_ms) {

	DEBUG_PASS("TEST_CASE - command_answer_success - run_module()");
	
	RUN_TIMER_start();
	
	while (RUN_TIMER_is_up(run_time_ms) == 0) {
		if (rpi_protocol_task_get_state() == MCU_TASK_RUNNING) {
			rpi_protocol_task_run();
		}
	}
}
