/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	unittest_rpi_protocl_client.c
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

#include "driver/trx_driver_interface.h"
#include "driver/communication/test/communication_driver_test.h"

#include "command_management/protocol_interface.h"

// --------------------------------------------------------------------------------

#include "protocol_management/rpi_protocol_handler.h"

// --------------------------------------------------------------------------------

#include "../unittest.h"

UT_ACTIVATE()

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INITIALIZE					0
#define TEST_CASE_ID_COMMAND_RECEIVED				1

// --------------------------------------------------------------------------------

u8 counter_LEAVE_SLEEP_SIGANL = 0;
u8 counter_ENTER_SLEEP_SIGNAL = 0;
u8 counter_COMMAND_RECEIVED_SIGNAL = 0;
u8 counter_INVALID_COMMAND_RECEIVED_SIGNAL = 0;

// --------------------------------------------------------------------------------

static void unittest_reset_counter(void) {

	counter_LEAVE_SLEEP_SIGANL = 0;
	counter_ENTER_SLEEP_SIGNAL = 0;
	counter_COMMAND_RECEIVED_SIGNAL = 0;
	counter_INVALID_COMMAND_RECEIVED_SIGNAL = 0;
}

// --------------------------------------------------------------------------------

// stubs

// --------------------------------------------------------------------------------

// slots

static PROTOCOL_INTERFACE* protocol_interface = NULL;

void unittest_RPI_PROTOCOL_LEAVE_SLEEP_SLOT_CALLBACK(const void* p_argument) {
	DEBUG_PASS("unittest_RPI_PROTOCOL_LEAVE_SLEEP_SLOT_CALLBACK()");
	counter_LEAVE_SLEEP_SIGANL += 1;
}

void unittest_RPI_PROTOCOL_ENTER_SLEEP_SLOT_CALLBACK(const void* p_argument) {
	DEBUG_PASS("unittest_RPI_PROTOCOL_ENTER_SLEEP_SLOT_CALLBACK()");
	counter_ENTER_SLEEP_SIGNAL += 1;
}

void unittest_RPI_PROTOCOL_COMMAND_RECEIVED_SLOT_CALLBACK(const void* p_argument) {
	DEBUG_PASS("unittest_RPI_PROTOCOL_COMMAND_RECEIVED_SLOT_CALLBACK()");
	counter_COMMAND_RECEIVED_SIGNAL += 1;

	protocol_interface = (PROTOCOL_INTERFACE*)p_argument;
}

void unittest_RPI_PROTOCOL_INVALID_COMMAND_RECEIVED_SLOT_CALLBACK(const void* p_argument) {
	DEBUG_PASS("unittest_RPI_PROTOCOL_INVALID_COMMAND_RECEIVED_SLOT_CALLBACK()");
	counter_INVALID_COMMAND_RECEIVED_SIGNAL += 1;
}

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(RPI_PROTOCOL_LEAVE_SLEEP_SIGNAL, UT_RPI_PROTOCOL_LEAVE_SLEEP_SLOT, unittest_RPI_PROTOCOL_LEAVE_SLEEP_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(RPI_PROTOCOL_ENTER_SLEEP_SIGNAL, UT_RPI_PROTOCOL_ENTER_SLEEP_SLOT, unittest_RPI_PROTOCOL_ENTER_SLEEP_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(RPI_PROTOCOL_COMMAND_RECEIVED_SIGNAL, UT_RPI_PROTOCOL_COMMAND_RECEIVED_SLOT, unittest_RPI_PROTOCOL_COMMAND_RECEIVED_SLOT_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(RPI_PROTOCOL_INVALID_COMMAND_RECEIVED_SIGNAL, UT_RPI_PROTOCOL_INVALID_COMMAND_RECEIVED_SLOT, unittest_RPI_PROTOCOL_INVALID_COMMAND_RECEIVED_SLOT_CALLBACK)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(UNITTEST_TIMER)

// --------------------------------------------------------------------------------

static TRX_DRIVER_INTERFACE unitetst_trx_driver = {
	SPI,					//	TRX_DRIVER_INTERFACE_TYPE type;
	&test_driver_initialize,
	&test_driver_configure, 		//	TRX_DRIVER_INTERFACE_CONFIGURE_CALLBACK configure;
	&test_driver_power_off,			//	TRX_DRIVER_INTERFACE_MODULE_OFF_CALLBACK shut_down;
	&test_driver_bytes_available, 		//	TRX_DRIVER_INTERFACE_BYTES_AVAILABLE_CALLBACK bytes_available;
	&test_driver_get_N_bytes, 		//	TRX_DRIVER_INTERFACE_GET_N_BYTES_CALLBACK get_N_bytes;
	&test_driver_set_N_bytes, 		//	TRX_DRIVER_INTERFACE_SET_N_BYTES_CALLBACK set_N_bytes;
	&test_driver_start_rx, 			//	TRX_DRIVER_INTERFACE_START_RX_CALLBACK start_rx;
	&test_driver_wait_for_rx,		//
	&test_driver_stop_rx, 			//	TRX_DRIVER_INTERFACE_STOP_RX_CALLBACK stop_rx;
	&test_driver_is_ready_for_tx,		//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
	&test_driver_is_ready_for_rx,		//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
	&test_driver_start_tx, 			//	TRX_DRIVER_INTERFACE_START_TX_CALLBACK start_tx;
	&test_driver_wait_for_tx,		//
	&test_driver_stop_tx, 			//	TRX_DRIVER_INTERFACE_STOP_TX_CALLBACK stop_tx;
	&test_driver_clear_rx_buffer, 		//	TRX_DRIVER_INTERFACE_CLEAR_BUFFER_CALLBACK clear_buffer;
	&test_driver_clear_tx_buffer, 		//	TRX_DRIVER_INTERFACE_CLEAR_BUFFER_CALLBACK clear_buffer;
	&test_driver_set_address,		//	TRX_DRIVER_INTERFACE_SET_ADDRESS_CALLBACK set_address;
	&test_driver_mutex_request,		//
	&test_driver_mutex_release		//
};

// --------------------------------------------------------------------------------

static void UNITTEST_rpi_protocol_init(void) {
	
	UT_START_TEST_CASE("RPi-Protocol Handler - request")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);

		unittest_reset_counter();
		rpi_protocol_init(&unitetst_trx_driver);

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_LEAVE_SLEEP_SIGANL, 0);
		UT_CHECK_IS_EQUAL(counter_ENTER_SLEEP_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_COMMAND_RECEIVED_SIGNAL, 0);
		UT_CHECK_IS_EQUAL(counter_INVALID_COMMAND_RECEIVED_SIGNAL, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_rpi_protocol_request(void) {
	
	UT_START_TEST_CASE("RPi-Protocol Handler - request")
	{	
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
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_rpi_protocol_send_version_command(void) {
	
	UT_START_TEST_CASE("RPi-Protocol Handler - COMMAND(get_version)")
	{	
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
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_rpi_protocol_receive_response(void) {
	
	UT_START_TEST_CASE("RPi-Protocol Handler - RESPONSE(get_version)")
	{	
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
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_rpi_protocol_send_get_light_command(void) {
	
	UT_START_TEST_CASE("RPi-Protocol Handler - COMMAND(get_light)")
	{	
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
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_rpi_protocol_rerequest(void) {
	
	UT_START_TEST_CASE("RPi-Protocol Handler - rerequest")
	{	
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
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_rpi_protocol_invalid_length_received(void) {
	
	UT_START_TEST_CASE("RPi-Protocol Handler - invalid command length")
	{	
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
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_rpi_protocol_length_without_command_received(void) {
	
	UT_START_TEST_CASE("RPi-Protocol Handler - length without command")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);

		unittest_reset_counter();

		UNITTEST_TIMER_start();

		u8 buffer[] = {0x02};
		test_driver_set_rx_bytes(1, buffer);

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
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_rpi_protocol_maximum_length_command_received(void) {

}

static void UNITTEST_rpi_protocol_command_without_length_received(void) {
	
	UT_START_TEST_CASE("RPi-Protocol Handler - command without length")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);

		unittest_reset_counter();

		UNITTEST_TIMER_start();

		u8 buffer[] = {0x0C, 0x01, 0x01, 0x01};
		test_driver_set_rx_bytes(4, buffer);

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
	}
	UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

	//TRACER_DISABLE();

	UT_START_TESTBENCH("Welcome the the UNITTEST for msg-executer v1.0")
	{
		UNITTEST_rpi_protocol_init();

		UT_RPI_PROTOCOL_COMMAND_RECEIVED_SLOT_connect();
		UT_RPI_PROTOCOL_LEAVE_SLEEP_SLOT_connect();
		UT_RPI_PROTOCOL_ENTER_SLEEP_SLOT_connect();
		UT_RPI_PROTOCOL_INVALID_COMMAND_RECEIVED_SLOT_connect();

		UNITTEST_rpi_protocol_request();
		UNITTEST_rpi_protocol_send_version_command();
		UNITTEST_rpi_protocol_receive_response();
		UNITTEST_rpi_protocol_send_get_light_command();

		UNITTEST_rpi_protocol_rerequest();

		UNITTEST_rpi_protocol_invalid_length_received();
		UNITTEST_rpi_protocol_length_without_command_received();
		UNITTEST_rpi_protocol_maximum_length_command_received();
		UNITTEST_rpi_protocol_command_without_length_received();
	}
	UT_END_TESTBENCH()

	return UT_TEST_RESULT();
}

// --------------------------------------------------------------------------------