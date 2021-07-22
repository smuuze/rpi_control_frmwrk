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
#include "common/common_types.h"
#include "mcu_task_management/mcu_task_interface.h"

#include "ui/file_interface/file_interface.h"
#include "ui/console/ui_console.h"

#include "time_management/time_management.h"

#include "driver/trx_driver_interface.h"
#include "driver/communication/test/communication_driver_test.h"

// --------------------------------------------------------------------------------

#include "protocol_management/rpi_protocol_handler.h"

// --------------------------------------------------------------------------------

#include "ui/cfg_file_parser/cfg_file_parser.h"

// --------------------------------------------------------------------------------

#include "../unittest.h"

UT_ACTIVATE()

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INITIALIZE					0
#define TEST_CASE_ID_CONFIGURE					1
#define TEST_CASE_ID_RECEIVE_COMMAND				2
#define TEST_CASE_ID_RESPONSE_TIMEOUT				3
#define TEST_CASE_ID_RESPONSE_OVERFLOW				4
#define TEST_CASE_ID_SLEEP_WHILE_UNCONFIGURED			5

// --------------------------------------------------------------------------------

// counter

static u8 counter_RESPONSE_RECEIVED = 0;
static u8 counter_RESPONSE_TIMEOUT = 0;
static u8 counter_RESPONSE_OVERFLOW = 0;
static u8 counter_RESPONSE_RECEIVED_NULL_POINTER_EXCEPTION = 0;
static u8 counter_RESPONSE_OVERFLOW_EXCEPTION = 0;

static COMMON_GENERIC_BUFFER_TYPE response_buffer;

// --------------------------------------------------------------------------------

static void unittest_reset_counter(void) {

	response_buffer.length = 0;
	memset(response_buffer.data, 0x00, COMMON_TYPES_GENERIC_BUFFER_SIZE);

	counter_RESPONSE_RECEIVED = 0;
	counter_RESPONSE_TIMEOUT = 0;
	counter_RESPONSE_OVERFLOW = 0;
	counter_RESPONSE_RECEIVED_NULL_POINTER_EXCEPTION = 0;
	counter_RESPONSE_OVERFLOW_EXCEPTION = 0;
}

// --------------------------------------------------------------------------------

// stubs

// --------------------------------------------------------------------------------

// slots

static void ut_rpi_host_response_received_CALLBACK(const void* p_argument) {
	(void) p_argument;
	counter_RESPONSE_RECEIVED += 1;

	if (p_argument == NULL) {
		counter_RESPONSE_RECEIVED_NULL_POINTER_EXCEPTION += 1;
		return;
	}

	COMMON_GENERIC_BUFFER_TYPE* p_buffer = (COMMON_GENERIC_BUFFER_TYPE*)p_argument;

	if (p_buffer->length > COMMON_TYPES_GENERIC_BUFFER_SIZE) {
		counter_RESPONSE_OVERFLOW_EXCEPTION += 1;
		return;
	}
	
	response_buffer.length = p_buffer->length;
	memcpy(response_buffer.data, p_buffer->data, response_buffer.length);
}

static void ut_rpi_host_response_timeout_CALLBACK(const void* p_argument) {
	(void) p_argument;
	counter_RESPONSE_TIMEOUT += 1;
}

static void ut_rpi_host_response_overflow_CALLBACK(const void* p_argument) {
	(void) p_argument;
	counter_RESPONSE_OVERFLOW += 1;
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(RPI_HOST_RESPONSE_RECEIVED_SIGNAL, UT_RPI_HOST_RESPONSE_RECEIVED_SLOT, ut_rpi_host_response_received_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(RPI_HOST_RESPONSE_TIMEOUT_SIGNAL, UT_RPI_HOST_RESPONSE_TIMEOUT_SLOT, ut_rpi_host_response_timeout_CALLBACK)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(RPI_HOST_RESPONSE_OVERFLOW_SIGNAL, UT_RPI_HOST_RESPONSE_OVERFLOW_SLOT, ut_rpi_host_response_overflow_CALLBACK)

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CFG_PARSER_CFG_COMPLETE_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CFG_PARSER_NEW_CFG_OBJECT_SIGNAL)

SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(RPI_HOST_COMMAND_RECEIVED_SIGNAL)

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

static void UNITTEST_rpi_protocol_host_init(void) {
	
	UT_START_TEST_CASE("Rpi-Protocol-Host - Initialize")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);

		unittest_reset_counter();
		rpi_protocol_init(&unitetst_trx_driver);

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();
		}

		UT_CHECK_IS_EQUAL(counter_RESPONSE_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_TIMEOUT, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_OVERFLOW, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_RECEIVED_NULL_POINTER_EXCEPTION, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_OVERFLOW_EXCEPTION, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_rpi_protocol_host_sleep_while_unconfigured(void) {
	
	UT_START_TEST_CASE("Rpi-Protocol-Host - Sleep while unconfigured")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_SLEEP_WHILE_UNCONFIGURED);

		unittest_reset_counter();

		UNITTEST_TIMER_start();

		u8 counter_times_awake = 0;

		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();

			if (rpi_protocol_handler_get_actual_state() != MCU_TASK_SLEEPING) {
				counter_times_awake = counter_times_awake < 255 ? counter_times_awake + 1: counter_times_awake;
			}
		}

		UT_CHECK_IS_EQUAL(counter_times_awake, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_TIMEOUT, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_OVERFLOW, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_RECEIVED_NULL_POINTER_EXCEPTION, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_OVERFLOW_EXCEPTION, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_rpi_protocol_host_configure(void) {
	
	UT_START_TEST_CASE("Rpi-Protocol-Host - Configure")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_CONFIGURE);

		unittest_reset_counter();

		CFG_FILE_PARSER_CFG_OBJECT_TYPE log_file_path_cfg_obj	 	= { .key = "COM_SPEED_HZ"	, .value = "38600" 		};
		CFG_FILE_PARSER_CFG_OBJECT_TYPE unknown_cfg_obj 		= { .key = "COM_DEVICE"		, .value = "/dev/spi0.0"	};
		CFG_FILE_PARSER_CFG_OBJECT_TYPE invalid_cfg_obj 		= { .key = "INVALID"		, .value = "I am invalid"	};

		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((void*)&log_file_path_cfg_obj);
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((void*)&unknown_cfg_obj);
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((void*)&invalid_cfg_obj);
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send(NULL);

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();
		}

		CFG_PARSER_CFG_COMPLETE_SIGNAL_send(NULL);

		UNITTEST_TIMER_start();
		while (UNITTEST_TIMER_is_up(250) == 0) {
			mcu_task_controller_schedule();
		}

		UNITTEST_TIMER_stop();

		UT_CHECK_IS_EQUAL(counter_RESPONSE_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_TIMEOUT, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_OVERFLOW, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_RECEIVED_NULL_POINTER_EXCEPTION, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_OVERFLOW_EXCEPTION, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_rpi_protocol_host_receive_command(void) {
	
	UT_START_TEST_CASE("Rpi-Protocol-Host - Receive Command")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_RECEIVE_COMMAND);

		unittest_reset_counter();

		COMMON_GENERIC_BUFFER_TYPE cmd_buffer = {
			.length = 2,
			.data = {0x01, 0x01}
		};

		u8 bytes_received[] = {0x04, 0x01, 0x000, 0x05, 0x00};

		RPI_HOST_COMMAND_RECEIVED_SIGNAL_send((const COMMON_GENERIC_BUFFER_TYPE*) &cmd_buffer);

		u8 response_send = 0;

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(1000) == 0) {
			mcu_task_controller_schedule();

			if (response_send == 0 && UNITTEST_TIMER_is_up(250)) {
				response_send = 1;
				test_driver_set_rx_bytes(5, bytes_received);
			}

		}

		UT_CHECK_IS_EQUAL(response_buffer.length, bytes_received[0]);
		UT_COMPARE_ARRAY(response_buffer.data, &bytes_received[1], response_buffer.length);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_TIMEOUT, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_OVERFLOW, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_RECEIVED_NULL_POINTER_EXCEPTION, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_OVERFLOW_EXCEPTION, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_rpi_protocol_host_response_overflow(void) {
	
	UT_START_TEST_CASE("Rpi-Protocol-Host - Response Overflow")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_RESPONSE_OVERFLOW);

		unittest_reset_counter();

		COMMON_GENERIC_BUFFER_TYPE cmd_buffer = {
			.length = 2,
			.data = {0x01, 0x01}
		};

		u8 bytes_received[] = {0xFF, 0x01, 0x000, 0x05, 0x00};
		u8 num_bytes = (u8)sizeof(bytes_received);

		RPI_HOST_COMMAND_RECEIVED_SIGNAL_send((const COMMON_GENERIC_BUFFER_TYPE*) &cmd_buffer);

		u8 response_send = 0;

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(1000) == 0) {
			mcu_task_controller_schedule();

			if (response_send == 0 && UNITTEST_TIMER_is_up(250)) {
				response_send = 1;
				test_driver_set_rx_bytes(num_bytes, bytes_received);
			}
		}

		UT_CHECK_IS_EQUAL(response_buffer.length, COMMON_TYPES_GENERIC_BUFFER_SIZE);
		//UT_COMPARE_ARRAY(response_buffer.data, bytes_received + 1, response_buffer.length);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_RECEIVED, 1);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_TIMEOUT, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_OVERFLOW, 1);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_RECEIVED_NULL_POINTER_EXCEPTION, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_OVERFLOW_EXCEPTION, 0);
	}
	UT_END_TEST_CASE()
}

static void UNITTEST_rpi_protocol_host_response_timeout(void) {
	
	UT_START_TEST_CASE("Rpi-Protocol-Host - Response Timeout")
	{	
		UT_SET_TEST_CASE_ID(TEST_CASE_ID_RESPONSE_TIMEOUT);

		unittest_reset_counter();

		COMMON_GENERIC_BUFFER_TYPE cmd_buffer = {
			.length = 2,
			.data = {0x01, 0x01}
		};

		RPI_HOST_COMMAND_RECEIVED_SIGNAL_send((const COMMON_GENERIC_BUFFER_TYPE*) &cmd_buffer);

		UNITTEST_TIMER_start();

		while (UNITTEST_TIMER_is_up(RPI_PROTOCOL_HOST_CLIENT_WAIT_TIMEOUT_MS + 500) == 0) {
			mcu_task_controller_schedule();

		}

		UT_CHECK_IS_EQUAL(response_buffer.length, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_RECEIVED, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_TIMEOUT, 1);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_RECEIVED_NULL_POINTER_EXCEPTION, 0);
		UT_CHECK_IS_EQUAL(counter_RESPONSE_OVERFLOW_EXCEPTION, 0);
	}
	UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

	TRACER_DISABLE();

	UT_START_TESTBENCH("Welcome the the UNITTEST for log-interface v1.0")
	{
		CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_init();
		CFG_PARSER_CFG_COMPLETE_SIGNAL_init();
	
		UNITTEST_rpi_protocol_host_init();
	TRACER_ENABLE();
		UNITTEST_rpi_protocol_host_sleep_while_unconfigured();
	TRACER_DISABLE();

		UT_RPI_HOST_RESPONSE_RECEIVED_SLOT_connect();
		UT_RPI_HOST_RESPONSE_TIMEOUT_SLOT_connect();
		UT_RPI_HOST_RESPONSE_OVERFLOW_SLOT_connect();

		UNITTEST_rpi_protocol_host_configure();
		UNITTEST_rpi_protocol_host_receive_command();
		UNITTEST_rpi_protocol_host_response_overflow();
		UNITTEST_rpi_protocol_host_response_timeout();
	}
	UT_END_TESTBENCH()

	return UT_TEST_RESULT();
}

// --------------------------------------------------------------------------------
