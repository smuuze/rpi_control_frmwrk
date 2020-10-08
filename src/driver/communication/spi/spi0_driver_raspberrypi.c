/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	spi0_driver_raspberrypi.c
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

#include "local_msg_buffer.h"

#include "cfg_driver_interface.h"

#include "driver/communication/spi/spi0_driver_atmega1284p.h"
#include "local_module_status.h"
#include "local_mutex.h"
#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

BUILD_MUTEX(MUTEX_SPI0)

// --------------------------------------------------------------------------------

#define SPI0_STATUS_RX_ACTIVE	0
#define SPI0_STATUS_TX_ACTIVE	1

BUILD_MODULE_STATUS_FAST(SPI0_STATUS, 2)

// --------------------------------------------------------------------------------

void spi_driver_initialize(void) {

}

/*!
 *
 */
void spi_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg) {

}

/*!
 *
 */
void spi_driver_power_off(void) {

}

u8 spi_driver_bytes_available(void) {
	return 0;
}


u8 spi_driver_get_N_bytes(u8 num_bytes, u8* p_buffer_to) {
	return 0;
}


u8 spi_driver_set_N_bytes(u8 num_bytes, const u8* p_buffer_from) {
	return 0;
}

u8 spi_driver_is_ready_for_tx(void) {
	return SPI0_STATUS_is_set(SPI0_STATUS_TX_ACTIVE) == 0 ? 1 : 0;
}

u8 spi_driver_is_ready_for_rx(void) {
	return SPI0_STATUS_is_set(SPI0_STATUS_RX_ACTIVE) == 0 ? 1 : 0;
}


void spi_driver_start_rx(u16 num_of_rx_bytes) {

}

void spi_driver_wait_for_rx(u8 num_bytes, u16 timeout_ms) {

}

void spi_driver_stop_rx(void) {

}

void spi_driver_start_tx(void) {

}

void spi_driver_wait_for_tx(u8 num_bytes, u16 timeout_ms) {

}

void spi_driver_stop_tx(void) {

}

void spi_driver_clear_rx_buffer(void) {

}


void spi_driver_clear_tx_buffer(void) {

}


void spi_driver_set_address (u8 addr) {

}

u8 spi_driver_mutex_request(void) {

	if (MUTEX_SPI0_is_requested() != 0) {
		return MUTEX_INVALID_ID;
	}

	return MUTEX_SPI0_request();
}

void spi_driver_mutex_release(u8 m_id) {
	MUTEX_SPI0_release(m_id);
}