 /*
  * \@file	driver/communication/test/communication_driver_test.c
  * \author	sebastian lesse
  */

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!

//-----------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "common/local_module_status.h"
#include "common/local_mutex.h"
#include "common/local_msg_buffer.h"
#include "common/signal_slot_interface.h"
#include "time_management/time_management.h"

#include "driver/communication/test/communication_driver_test.h"

//-----------------------------------------------------------------------------

#define TEST_DRIVER_STATUS_RX_ACTIVE	0
#define TEST_DRIVER_STATUS_TX_ACTIVE	1

//-----------------------------------------------------------------------------

BUILD_MUTEX(test_mutex)

BUILD_LOCAL_MSG_BUFFER( , TEST_TX_BUFFER, 128)
BUILD_LOCAL_MSG_BUFFER( , TEST_RX_BUFFER, 128)

BUILD_MODULE_STATUS_FAST(test_driver_status, 2)

TIME_MGMN_BUILD_STATIC_TIMER_U16(TRX_TIMER)

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(SIGNAL_TEST_DRIVER_START_RX)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(SIGNAL_TEST_DRIVER_START_TX)

//-----------------------------------------------------------------------------


void test_driver_initialize(void) {

	DEBUG_PASS("test_driver_initialize()");

	TEST_RX_BUFFER_init();
	TEST_TX_BUFFER_init();

	SIGNAL_TEST_DRIVER_START_RX_init();
	SIGNAL_TEST_DRIVER_START_TX_init();
}

void test_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg) {

	(void) p_cfg;
}

void test_driver_power_off(void) {

}

u8 test_driver_bytes_available (void) {

	u8 bytes_available = TEST_RX_BUFFER_bytes_available();
	DEBUG_TRACE_byte(bytes_available, "test_driver_bytes_available()");

	return bytes_available;
}

u8 test_driver_get_N_bytes (u8 num_bytes, u8* p_buffer_to) {

	TEST_RX_BUFFER_start_read();
	u16 num_bytes_read = TEST_RX_BUFFER_get_N_bytes(num_bytes, p_buffer_to);
	TEST_RX_BUFFER_stop_read();

	DEBUG_TRACE_N(num_bytes_read, p_buffer_to, "test_driver_get_N_bytes()");

	return num_bytes_read;
}

u8 test_driver_set_N_bytes (u8 num_bytes, const u8* p_buffer_from) {

	if (num_bytes > TEST_TX_BUFFER_size()) {
		num_bytes = TEST_TX_BUFFER_size();
	}

	DEBUG_TRACE_byte(num_bytes, "test_driver_set_N_bytes() - Number of bytes: ");

	//DEBUG_TRACE_N(num_bytes, p_buffer_from, "test_driver_set_N_bytes()");

	TEST_TX_BUFFER_start_write(); // this will delete all data added so far
	TEST_TX_BUFFER_add_N_bytes(num_bytes, p_buffer_from);
	TEST_TX_BUFFER_stop_write();

	return num_bytes;
}

u8 test_driver_is_ready_for_tx (void) {
	return test_driver_status_is_set(TEST_DRIVER_STATUS_TX_ACTIVE) == 0 ? 1 : 0;
}

u8 test_driver_is_ready_for_rx(void) {
	return test_driver_status_is_set(TEST_DRIVER_STATUS_RX_ACTIVE) == 0 ? 1 : 0;
}

void test_driver_start_rx (u16 num_of_rx_bytes) {

	(void) num_of_rx_bytes;

	DEBUG_PASS("test_driver_start_rx()");

	TEST_RX_BUFFER_start_write();
	test_driver_status_set(TEST_DRIVER_STATUS_RX_ACTIVE);

	SIGNAL_TEST_DRIVER_START_RX_send((void*) &num_of_rx_bytes);
}

void test_driver_wait_for_rx(u8 num_bytes, u16 timeout_ms) {
			
	DEBUG_TRACE_byte(num_bytes, "test_driver_wait_for_rx() - Num byts to send: ");
	DEBUG_TRACE_byte(TEST_RX_BUFFER_bytes_available(), "test_driver_wait_for_rx() - Num byts available: ");

	TRX_TIMER_start();

	while (TEST_RX_BUFFER_bytes_available() < num_bytes) {

		if (TRX_TIMER_is_up(timeout_ms)) {
			DEBUG_PASS("test_driver_wait_for_rx() - Time is up");
			break;
		}
	}
}

void test_driver_stop_rx (void) {

	DEBUG_PASS("test_driver_stop_rx()");

	test_driver_status_unset(TEST_DRIVER_STATUS_RX_ACTIVE);
	TEST_RX_BUFFER_stop_write();
}

void test_driver_start_tx (void) {

	u8 num_bytes = TEST_TX_BUFFER_bytes_available();
	if (num_bytes == 0) {
		DEBUG_PASS("test_driver_start_tx() - No bytes available to transmit");
		return;
	}

	DEBUG_TRACE_byte(num_bytes, "test_driver_start_tx() - Number of bytes: ");

	test_driver_status_set(TEST_DRIVER_STATUS_TX_ACTIVE);
	TEST_TX_BUFFER_start_read();

	SIGNAL_TEST_DRIVER_START_TX_send((void*) &num_bytes);
}

void test_driver_wait_for_tx(u8 num_bytes, u16 timeout_ms) {
			
	DEBUG_TRACE_byte(num_bytes, "test_driver_wait_for_tx() - Num byts to send: ");
	DEBUG_TRACE_byte(TEST_TX_BUFFER_bytes_available(), "test_driver_wait_for_tx() - Num byts available: ");

	TRX_TIMER_start();

	while (TRX_TIMER_is_up(timeout_ms) == 0) {

	}

	if (TEST_TX_BUFFER_bytes_available() != 0) {
		DEBUG_PASS("test_driver_wait_for_tx() - still bytes left ...");
		return;
	}

	test_driver_status_unset(TEST_DRIVER_STATUS_TX_ACTIVE);
}

void test_driver_stop_tx (void) {

	DEBUG_PASS("test_driver_stop_tx()");

	TEST_TX_BUFFER_stop_read();
	test_driver_status_unset(TEST_DRIVER_STATUS_TX_ACTIVE);
}

void test_driver_clear_rx_buffer (void) {
	DEBUG_PASS("test_driver_clear_rx_buffer()");
	TEST_RX_BUFFER_clear_all();
}

void test_driver_clear_tx_buffer (void) {
	DEBUG_PASS("test_driver_clear_tx_buffer()");
	TEST_TX_BUFFER_clear_all();
}

void test_driver_set_address (u8 addr) {
	DEBUG_TRACE_byte(addr, "test_driver_set_address()");
	(void) addr;
}

u8 test_driver_mutex_request(void) {

	if (test_mutex_is_requested() != 0) {
		DEBUG_PASS("test_driver_mutex_request() - Mutex is already requested");
		return MUTEX_INVALID_ID;
	}

	DEBUG_PASS(" >>> test_driver_mutex_request() - Requesting Mutex");
	return test_mutex_request();
}

void test_driver_mutex_release(u8 m_id) {

	DEBUG_PASS(" <<< test_driver_mutex_release() - Releasing Mutex");
	test_mutex_release(m_id);
}


void test_driver_set_rx_bytes(u8 num_bytes, u8* p_buffer_from) {

	if (num_bytes > TEST_RX_BUFFER_size()) {
		num_bytes = TEST_RX_BUFFER_size();
	}

	DEBUG_TRACE_N(num_bytes, p_buffer_from, "test_driver_set_rx_bytes()");

	TEST_RX_BUFFER_start_write(); // this will delete all data added so far
	TEST_RX_BUFFER_add_N_bytes(num_bytes, p_buffer_from);
	TEST_RX_BUFFER_stop_write();
}

u8 test_driver_get_tx_bytes(u8 num_bytes, u8* p_buffer_to) {

	DEBUG_TRACE_byte(num_bytes, "test_driver_clear_rx_buffer()");

	TEST_TX_BUFFER_start_read();
	u16 num_bytes_read = TEST_TX_BUFFER_get_N_bytes(num_bytes, p_buffer_to);
	TEST_TX_BUFFER_stop_read();

	DEBUG_TRACE_N(num_bytes_read, p_buffer_to, "test_driver_get_tx_bytes()");

	if (TEST_TX_BUFFER_bytes_available() == 0) {
		test_driver_stop_tx();
	}

	return num_bytes_read;
}

u8 test_driver_get_tx_bytes_num_available(void) {

	DEBUG_TRACE_byte(TEST_TX_BUFFER_bytes_available(), "test_driver_get_tx_bytes_num_available()");
	return TEST_TX_BUFFER_bytes_available();
}
