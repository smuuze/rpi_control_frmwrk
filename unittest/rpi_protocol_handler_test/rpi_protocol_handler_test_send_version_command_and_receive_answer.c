/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *
 * \author               sebastian lesse
 *
 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include "driver/cfg_driver_interface.h"
#include "driver/trx_driver_interface.h"

#include "driver/gpio/gpio_interface.h"

#include "common/local_context.h"

#include "driver/communication/test/communication_driver_test.h"

#include "protocol_management/rpi_protocol_handler.h"
#include "command_management/command_handler_interface.h"

#include "io_management/io_controller.h"
#include "mcu_task_management/mcu_task_interface.h"

#include "time_management/time_management.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_OFF
#include "tracer.h"

//---------- Static Data ------------------------------------------------------

IO_CONTROLLER_INCLUDE_INOUT(IS_READY)
TIME_MGMN_BUILD_STATIC_TIMER_U16(op_timer)

//---------- Function ---------------------------------------------------------

u8 test_send_version_command_and_receive_answer(void) {

	static u8 test_state = 0;
	static u8 num_byts_received = 0;
	static u8 try_counter = 0;

	u8 trx_data[128];

	switch (test_state) {

		default : //break;

			DEBUG_PASS("\n\n-------------- STARTING-TEST : send version command and receive answer");

		case 1 : // init

			try_counter = 0;

			trx_data[0] = 1;
			trx_data[1] = 1;
			test_driver_set_rx_bytes(2, trx_data);

			op_timer_start();
			DEBUG_TRACE_word(op_timer_start_time(), "Start_time: ");

			test_state = 2;
			DEBUG_PASS("-------------- TEST-STATE : wait for init");

			break;

		case 2 : // wait for init

			if (op_timer_is_up(50) == 0) {
				break;
			}

			test_state = 3;

			DEBUG_TRACE_word(op_timer_elapsed(), " - Time elapsed: ");
			DEBUG_PASS("-------------- TEST-STATE : activate module");

			break;

		case 3 : // activation

			IS_READY_drive_low();

			test_state = 4;
			DEBUG_PASS("-------------- TEST-STATE : wait for activation");

			op_timer_start();
			break;

		case 4 : // wait for activation

			if (IS_READY_is_high_level()) {
				DEBUG_PASS("TEST-FAILED: TEST_STATE_WAIT_FOR_ACTIVATAION !!!");
				DEBUG_PASS("IS_READY cant be high here");
			}

			if (op_timer_is_up(50) == 0) {
				break;
			}

			IS_READY_pull_up();

			test_state = 5;

			DEBUG_TRACE_word(op_timer_elapsed(), " - Time elapsed: ");
			DEBUG_PASS("-------------- TEST-STATE : wait for response");

			op_timer_start();
			break;
			
		case 5 : // wait for response

			if (op_timer_is_up(250)) {

				if (++try_counter < 5) {
					test_state = 3;
					break;
				}

				DEBUG_PASS("TEST-FAILED: TEST_STATE_WAIT_FOR_RESPONSE !!!");

				DEBUG_TRACE_word(op_timer_elapsed(), " - Time elapsed: ");
				DEBUG_TRACE_byte(rpi_protocol_task_get_state(), " - Task-State of Module: ");
				DEBUG_TRACE_byte(rpi_protocol_handler_get_actual_state(), " - OP-State of Module: ");
				IS_READY_print_state();

				return 1;
			}

			if (IS_READY_is_high_level()) {
				break;
			}

			test_state = 6;
			DEBUG_PASS("-------------- TEST-STATE : sending command");

			op_timer_start();
			break;

		case 6 : // send command

			test_state = 7;
			DEBUG_PASS("-------------- TEST-STATE : wait for finish");

			op_timer_start();
			break;

		case 7 : // wait for finish

			if (op_timer_is_up(250)) {
				DEBUG_PASS("TEST-FAILED: waiting for finish !!!");
				return 1;
			}

			if (IS_READY_is_low_level()) {
				break;
			}

			test_state = 8;
			DEBUG_PASS("-------------- TEST-STATE : get answer");

			break;

		case 8 : // get answer

			IS_READY_drive_low();
			num_byts_received = 0;

			test_state = 9;
			DEBUG_PASS("-------------- TEST-STATE : activate");

			op_timer_start();
			break;

		case 9 : // wait for activation

			if (op_timer_is_up(50) == 0) {
				break;
			}

			IS_READY_pull_up();

			test_state = 10;
			DEBUG_PASS("-------------- TEST-STATE : wait for response");

			op_timer_start();
			break;
			
		case 10 : // wait for response

			if (op_timer_is_up(250)) {
				DEBUG_PASS("TEST-FAILED: wait for response on receive answer");
				return 1;
			}

			if (IS_READY_is_high_level()) {
				break;
			}

			test_state = 11;
			DEBUG_PASS("-------------- TEST-STATE : receive answer");

			op_timer_start();
			break;

		case 11 : // get answer

			if (test_driver_get_tx_bytes_num_available() != 0) {

				test_driver_get_tx_bytes(1, trx_data + num_byts_received);
				DEBUG_TRACE_byte(trx_data[num_byts_received], "-------------- TEST-STATE : receive answer - Byte Received: ");

				num_byts_received += 1;
				break;
			}

			test_state = 12;
			DEBUG_PASS("-------------- TEST-STATE : test");

			op_timer_start();
			break;

		case 12 : // test

			if (num_byts_received != 5) {
				DEBUG_TRACE_byte(num_byts_received, "TEST-FAILED: incorrect number of bytes received - Number:");
				return 1;
			}

			test_state = 13;
			DEBUG_PASS("-------------- TEST-STATE : idle");

			op_timer_start();

		case 13 : // do nothing

			if (op_timer_is_up(5000) == 0) {
				break;
			}

			DEBUG_PASS("-------------- TEST-STATE : restart");
			test_state = 0; // restart
			return 0xFF;
	}

	return 0;
}