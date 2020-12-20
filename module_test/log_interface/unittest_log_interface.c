/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	unittest_log_interface.c
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
#include "ui/console/ui_console.h"

#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

#include "ui/log_interface/log_interface.h"

// --------------------------------------------------------------------------------

#include "../unittest.h"

UT_ACTIVATE()

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INITIALIZE					0
#define TEST_CASE_ID_COMMAND_RECEIVED				1

// --------------------------------------------------------------------------------

// counter

// --------------------------------------------------------------------------------

static void unittest_reset_counter(void) {
}

// --------------------------------------------------------------------------------

// stubs

// --------------------------------------------------------------------------------

// slots

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CFG_PARSER_CFG_COMPLETE_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CFG_PARSER_NEW_CFG_OBJECT_SIGNAL)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(UNITTEST_TIMER)

// --------------------------------------------------------------------------------

static void UNITTEST_log_interface_init(void) {
	
	UT_START_TEST_CASE("Log_interface - Initialie")
	{	
		/*
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);

		unittest_reset_counter();
		log_interface_init(&unitetst_trx_driver);

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_LEAVE_SLEEP_SIGANL, 0);
		UT_CHECK_IS_EQUAL(counter_ENTER_SLEEP_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_COMMAND_RECEIVED_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND_RECEIVED_SIGNAL, 0);
		*/
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_log_interface_request(void) {
	
	UT_START_TEST_CASE("Log_interface - Configure")
	{	
		/*
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);

		unittest_reset_counter();

		UNITTEST_TIMER_start();

		READY_INOUT_drive_low();

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_LEAVE_SLEEP_SIGANL, 1);
		UT_CHECK_IS_EQUAL(counter_ENTER_SLEEP_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_COMMAND_RECEIVED_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND_RECEIVED_SIGNAL, 0);
		*/
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_log_interface_log_message(void) {
	
	UT_START_TEST_CASE("Log_interface - Log-Message")
	{	
		/*
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);

		unittest_reset_counter();

		UNITTEST_TIMER_start();

		u8 buffer[] = {0x01, 0x01};
		test_driver_set_rx_bytes(2, buffer);

		buffer[0] = 0;
		u16 timeout = 10;

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();

			// polling for an answer
			if (UNITTEST_TIMER_is_up(timeout)) {
				timeout += 10;
				test_driver_set_rx_bytes(1, buffer);
			}
		}

		UT_CHECK_IS_EQUAL(counter_LEAVE_SLEEP_SIGANL, 0);
		UT_CHECK_IS_EQUAL(counter_ENTER_SLEEP_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_COMMAND_RECEIVED_SIGNAL, 1);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND_RECEIVED_SIGNAL, 0);
		*/
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_log_interface_write_log_file(void) {
	
	UT_START_TEST_CASE("Log_interface - Write log file")
	{	
		/*
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);

		unittest_reset_counter();

		UNITTEST_TIMER_start();

		if (protocol_interface != NULL) {
			protocol_interface->answ_buffer->start_write();
			protocol_interface->answ_buffer->add_byte(0x05);
			protocol_interface->answ_buffer->add_byte(0x00);
			protocol_interface->answ_buffer->stop_write();
			protocol_interface->set_finished(0);
		}

		u8 num_bytes_available = 0;
		u8 temp_buffer[255];
		u8 ref_buffer[] = {0x04, 0x01, 0x00, 0x05, 0x00};
		u8 counter_RESPONSE_RECEIVED = 0;

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();

			if (test_driver_get_tx_bytes_num_available() != 0) {
				num_bytes_available = test_driver_get_tx_bytes_num_available();
				test_driver_get_tx_bytes(num_bytes_available, temp_buffer);
				counter_RESPONSE_RECEIVED += 1;
			}
		}

		UT_CHECK_IS_EQUAL(counter_LEAVE_SLEEP_SIGANL, 0);
		UT_CHECK_IS_EQUAL(counter_ENTER_SLEEP_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_COMMAND_RECEIVED_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(num_bytes_available, 5);
		UT_COMPARE_ARRAY(temp_buffer, ref_buffer, 5);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND_RECEIVED_SIGNAL, 0);
		*/
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_log_interface_log_file_size_too_big(void) {
	
	UT_START_TEST_CASE("Log_interface - Log-File size too big")
	{	
		/*
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);

		unittest_reset_counter();

		UNITTEST_TIMER_start();

		u8 buffer[] = {0x02, 0x05, 0x04};
		test_driver_set_rx_bytes(3, buffer);

		buffer[0] = 0;
		u16 timeout = 10;

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();

			// polling for an answer
			if (UNITTEST_TIMER_is_up(timeout)) {
				timeout += 10;
				test_driver_set_rx_bytes(1, buffer);
			}
		}

		UT_CHECK_IS_EQUAL(counter_LEAVE_SLEEP_SIGANL, 0);
		UT_CHECK_IS_EQUAL(counter_ENTER_SLEEP_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_COMMAND_RECEIVED_SIGNAL, 1);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND_RECEIVED_SIGNAL, 0);
		*/
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_log_interface_que_overflow(void) {
	
	UT_START_TEST_CASE("Log_interface- Qeue overflow")
	{	
		/*
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);

		unittest_reset_counter();

		UNITTEST_TIMER_start();

		READY_INOUT_pull_up();

		u8 re_requested = 0;

		while (UNITTEST_TIMER_is_up(500) == 0) {

			mcu_task_controller_schedule();

			if (re_requested == 0 && UNITTEST_TIMER_is_up(125)) {
				re_requested = 1;
				READY_INOUT_drive_low();
			}
		}

		UT_CHECK_IS_EQUAL(counter_LEAVE_SLEEP_SIGANL, 0);
		UT_CHECK_IS_EQUAL(counter_ENTER_SLEEP_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_COMMAND_RECEIVED_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND_RECEIVED_SIGNAL, 0);
		*/
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_log_interface_log_file_not_writeable(void) {
	
	UT_START_TEST_CASE("Log_interface - Log-File not writeable")
	{	
		/*
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);

		unittest_reset_counter();

		UNITTEST_TIMER_start();

		u8 buffer[] = {0x02, 0x01};
		test_driver_set_rx_bytes(2, buffer);

		buffer[0] = 0;
		u16 timeout = 10;

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();

			// polling for an answer
			if (UNITTEST_TIMER_is_up(timeout)) {
				timeout += 10;
				test_driver_set_rx_bytes(1, buffer);
			}
		}

		UT_CHECK_IS_EQUAL(counter_LEAVE_SLEEP_SIGANL, 0);
		UT_CHECK_IS_EQUAL(counter_ENTER_SLEEP_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_COMMAND_RECEIVED_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND_RECEIVED_SIGNAL, 1);
		*/
	}
	UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

	//TRACER_DISABLE();

	UT_START_TESTBENCH("Welcome the the UNITTEST for log-interface v1.0")
	{
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_init();
		CFG_PARSER_CFG_COMPLETE_SIGNAL_init();
	
		UNITTEST_log_interface_init();
		UNITTEST_log_interface_log_message();
		UNITTEST_log_interface_write_log_file();
		UNITTEST_log_interface_log_file_size_too_big();
		UNITTEST_log_interface_que_overflow();
		UNITTEST_log_interface_log_file_not_writeable();
	}
	UT_END_TESTBENCH()

	return UT_TEST_RESULT();
}

// --------------------------------------------------------------------------------