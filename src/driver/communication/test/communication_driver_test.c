/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "driver/communication/test/communication_driver_test.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_OFF
#include "tracer.h"

//-----------------------------------------------------------------------------


void test_driver_initialize(void) {

}

void test_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg) {

	(void) p_cfg;
}

void test_driver_power_off(void) {

}

u8 test_driver_bytes_available (void) {
	return 0;
}

u8 test_driver_get_N_bytes (u8 num_bytes, u8* p_buffer_to) {
	return 0;
}

u8 test_driver_set_N_bytes (u8 num_bytes, const u8* p_buffer_from) {
	return 0;
}

u8 test_driver_is_ready_for_tx (void) {
	return 0;
}

u8 test_driver_is_ready_for_rx(void) {
	return 0;
}

void test_driver_start_rx (u16 num_of_rx_bytes) {

}

void test_driver_wait_for_rx(u8 num_bytes, u16 timeout_ms) {
	(void) num_bytes;
	(void) timeout_ms;
}

void test_driver_stop_rx (void) {

}

void test_driver_start_tx (void) {

}

void test_driver_wait_for_tx(u8 num_bytes, u16 timeout_ms) {
	(void) num_bytes;
	(void) timeout_ms;
}

void test_driver_stop_tx (void) {

}

void test_driver_clear_buffer (void) {

}

void test_driver_set_address (u8 addr) {
	(void) addr;
}

u8 test_driver_mutex_request(void) {
	return 1;
}

void test_driver_mutex_release(u8 m_id) {

}
