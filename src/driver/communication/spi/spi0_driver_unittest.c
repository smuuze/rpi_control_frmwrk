/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	spi0_driver_unittest.c
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

#include "driver/communication/spi/spi0_driver.h"
#include "local_module_status.h"
#include "local_mutex.h"
#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

#define SPI0_DRIVER_MAX_SPEED				DRIVER_SPI_SPEED_HZ_153600
#define SPI0_DRIVER_MIN_SPEED				DRIVER_SPI_SPEED_HZ_1000

// --------------------------------------------------------------------------------

#ifndef SPI0_DEFAULT_CONFIGURTATION_MODE 
#define SPI0_DEFAULT_CONFIGURTATION_MODE		0x03
#endif

#ifndef SPI0_DEFAULT_CONFIGURATION_SPEED_HZ		
#define SPI0_DEFAULT_CONFIGURATION_SPEED_HZ		SPI0_DRIVER_MAX_SPEED
#endif

#ifndef SPI0_DEFAULT_CONFIGURATION_DEVICE		
#define SPI0_DEFAULT_CONFIGURATION_DEVICE		"/dev/spidev0.0"
#endif

#ifndef SPI0_DRIVER_TX_BUFFER_SIZE
#define SPI0_DRIVER_TX_BUFFER_SIZE			128
#endif

#ifndef SPI0_DRIVER_RX_BUFFER_SIZE
#define SPI0_DRIVER_RX_BUFFER_SIZE			128
#endif

#ifndef SPI0_DRIVER_DEFAULT_BITS_PER_WORD
#define SPI0_DRIVER_DEFAULT_BITS_PER_WORD		8
#endif

// --------------------------------------------------------------------------------

#define SPI0_DEVICE_HANDLE_INVALID			-1

// --------------------------------------------------------------------------------

void spi0_driver_initialize(void) {
	
}

/*!
 *
 */
void spi0_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg) {

	DEBUG_PASS("spi0_driver_configure()");
}

/*!
 *
 */
void spi0_driver_power_off(void) {

}

u16 spi0_driver_bytes_available(void) {
	return 0;
}


u16 spi0_driver_get_N_bytes(u16 num_bytes, u8* p_buffer_to) {

	(void)num_bytes;
	(void)p_buffer_to;

	return 0;
}


u16 spi0_driver_set_N_bytes(u16 num_bytes, const u8* p_buffer_from) {

	(void)num_bytes;
	(void)p_buffer_from;

	return 0;
}

u8 spi0_driver_is_ready_for_tx(void) {
	return 0;
}

u8 spi0_driver_is_ready_for_rx(void) {
	return 0;
}


void spi0_driver_start_rx(u16 num_of_rx_bytes) {
	(void) num_of_rx_bytes;
}

void spi0_driver_wait_for_rx(u16 num_bytes, u16 timeout_ms) {
	(void) num_bytes;
	(void) timeout_ms;

}

void spi0_driver_stop_rx(void) {

}

void spi0_driver_start_tx(void) {

}

void spi0_driver_wait_for_tx(u16 num_bytes, u16 timeout_ms) {
	(void) num_bytes;
	(void) timeout_ms;

}

void spi0_driver_stop_tx(void) {

}

void spi0_driver_clear_rx_buffer(void) {

}


void spi0_driver_clear_tx_buffer(void) {

}


void spi0_driver_set_address (u8 addr) {

}

u8 spi0_driver_mutex_request(void) {

	return MUTEX_INVALID_ID;
}

void spi0_driver_mutex_release(u8 m_id) {
	(void) m_id;
}
