/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	usart0_driver_unittest.c
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

#include <string.h>

// --------------------------------------------------------------------------------

#include "cfg_driver_interface.h"
#include "local_mutex.h"

// --------------------------------------------------------------------------------

void usart0_driver_initialize(void) {
	
}

/*!
 *
 */
void usart0_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg) {

	DEBUG_PASS("usart0_driver_configure()");
}

/*!
 *
 */
void usart0_driver_power_off(void) {

}

u8 usart0_driver_bytes_available(void) {
	return 0;
}


u8 usart0_driver_get_N_bytes(u8 num_bytes, u8* p_buffer_to) {

	(void)num_bytes;
	(void)p_buffer_to;

	return 0;
}


u8 usart0_driver_set_N_bytes(u8 num_bytes, const u8* p_buffer_from) {

	(void)num_bytes;
	(void)p_buffer_from;

	return 0;
}

u8 usart0_driver_is_ready_for_tx(void) {
	return 0;
}

u8 usart0_driver_is_ready_for_rx(void) {
	return 0;
}


void usart0_driver_start_rx(u16 num_of_rx_bytes) {
	(void) num_of_rx_bytes;
}

void usart0_driver_wait_for_rx(u8 num_bytes, u16 timeout_ms) {
	(void) num_bytes;
	(void) timeout_ms;

}

void usart0_driver_stop_rx(void) {

}

void usart0_driver_start_tx(void) {

}

void usart0_driver_wait_for_tx(u8 num_bytes, u16 timeout_ms) {
	(void) num_bytes;
	(void) timeout_ms;

}

void usart0_driver_stop_tx(void) {

}

void usart0_driver_clear_rx_buffer(void) {

}


void usart0_driver_clear_tx_buffer(void) {

}


void usart0_driver_set_address (u8 addr) {

}

u8 usart0_driver_mutex_request(void) {

	return MUTEX_INVALID_ID;
}

void usart0_driver_mutex_release(u8 m_id) {
	(void) m_id;
}
