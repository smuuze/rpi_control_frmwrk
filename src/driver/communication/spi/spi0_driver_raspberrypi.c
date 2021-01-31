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

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

// --------------------------------------------------------------------------------

#include "local_msg_buffer.h"

#include "cfg_driver_interface.h"

#include "driver/communication/spi/spi0_driver_atmega1284p.h"
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

BUILD_MUTEX(SPI0_MUTEX)

// --------------------------------------------------------------------------------

BUILD_LOCAL_MSG_BUFFER( , SPI0_TX_BUFFER, SPI0_DRIVER_TX_BUFFER_SIZE)
BUILD_LOCAL_MSG_BUFFER( , SPI0_RX_BUFFER, SPI0_DRIVER_RX_BUFFER_SIZE)

// --------------------------------------------------------------------------------

#define SPI0_STATUS_RX_ACTIVE		0
#define SPI0_STATUS_TX_ACTIVE		1
#define SPI0_STATUS_INITIALIZED		2

BUILD_MODULE_STATUS_FAST(SPI0_STATUS, 3)

// --------------------------------------------------------------------------------

typedef struct {
	i32 handle;
	u32 mode;
	u32 bits_per_word;
	u32 speed;
} SPI0_CONFIGURATION_TYPE;

// --------------------------------------------------------------------------------

static SPI0_CONFIGURATION_TYPE spi0_cfg;

// --------------------------------------------------------------------------------

void spi0_driver_initialize(void) {

	if (SPI0_STATUS_is_set(SPI0_STATUS_INITIALIZED)) {
		DEBUG_PASS("spi0_driver_initialize() - is already initialzed");
	}

	spi0_cfg.mode = SPI0_DEFAULT_CONFIGURTATION_MODE;
	spi0_cfg.speed = SPI0_DEFAULT_CONFIGURATION_SPEED_HZ;
	spi0_cfg.bits_per_word = SPI0_DRIVER_DEFAULT_BITS_PER_WORD;

	SPI0_STATUS_clear_all();
	SPI0_STATUS_set(SPI0_STATUS_INITIALIZED);
}

/*!
 *
 */
void spi0_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg) {

	DEBUG_PASS("spi0_driver_configure()");

	SPI0_CS_pull_up();

	if (spi0_cfg.handle == SPI0_DEVICE_HANDLE_INVALID) {
		DEBUG_PASS("spi0_driver_configure() - Driver is not initialized");
		return;
	}

	// mode ---------------------------

	switch (p_cfg->module.spi.mode) {

		default : // no break;
		case DRIVER_SPI_MODE_0 :
			DEBUG_PASS("spi0_driver_configure() - DRIVER_SPI_MODE_0");
			break;

		case DRIVER_SPI_MODE_1 :
			DEBUG_PASS("spi0_driver_configure() - DRIVER_SPI_MODE_1");
			break;

		case DRIVER_SPI_MODE_2 :
			DEBUG_PASS("spi0_driver_configure() - DRIVER_SPI_MODE_2");
			break;

		case DRIVER_SPI_MODE_3 :
			DEBUG_PASS("spi0_driver_configure() - DRIVER_SPI_MODE_3");
			break;
	}

	// speed ---------------------------

	if (p_cfg->module.spi.speed > SPI0_DRIVER_MAX_SPEED) {

		DEBUG_TRACE_long(p_cfg->module.spi.speed, "spi0_driver_configure() - Using SPI0_DRIVER_MAX_SPEED - cfg to fast");
		spi0_cfg.speed = SPI0_DRIVER_MAX_SPEED;

	} else if (p_cfg->module.spi.speed < SPI0_DRIVER_MIN_SPEED) {

		DEBUG_TRACE_long(p_cfg->module.spi.speed, "spi0_driver_configure() - Using SPI0_DRIVER_MAX_SPEED - cfg to slow");
		spi0_cfg.speed = SPI0_DRIVER_MIN_SPEED;

	} else {

		DEBUG_TRACE_long(p_cfg->module.spi.speed, "spi0_driver_configure() - Using user configuration : ");
		spi0_cfg.speed = p_cfg->module.spi.speed;
	}

	// device -------------------------
	
	DEBUG_TRACE_STR(p_cfg->module.spi.device, "spi0_driver_configure() - open device :");

	if ((spi0_cfg.handle = (i32)open(p_cfg->module.spi.device, O_RDWR)) >= 0) {
		DEBUG_PASS("spi0_driver_configure() - Device open successfull");
	
	} else {

		DEBUG_TRACE_STR(SPI0_DEFAULT_CONFIGURATION_DEVICE, "spi0_driver_configure() - Open device has FAILED!!! - using default configuration");
		
		if ((spi0_cfg.handle = (i32)open(SPI0_DEFAULT_CONFIGURATION_DEVICE, O_RDWR)) >= 0) {
			DEBUG_PASS("spi0_driver_configure() - Device open successfull");

		} else {
			DEBUG_PASS("spi0_driver_configure() - Device open successfull");
			spi0_cfg.handle = SPI0_DEVICE_HANDLE_INVALID;
		}
	}

	// configuration -----------------
	
	int err_code = ioctl(spi0_cfg.handle, SPI_IOC_WR_MODE32, &spi0_cfg.mode);
	if (err_code) {
		DEBUG_TRACE_byte(err_code , "spi0_driver_configure() - Can't set spi mode - SPI_IOC_WR_MODE32 - error:");
		return;
	}

	err_code = ioctl(spi0_cfg.handle, SPI_IOC_RD_MODE32, &spi0_cfg.mode);
	if (err_code) {
		DEBUG_PASS("spi0_driver_configure() - Can't get spi mode - SPI_IOC_RD_MODE32");
		return;
	}
	
	err_code = ioctl(spi0_cfg.handle, SPI_IOC_WR_BITS_PER_WORD, &spi0_cfg.bits_per_word);
	if (err_code) {
		DEBUG_PASS("spi0_driver_configure() - Can't set bits per word\n");
		return;
	}

	err_code = ioctl(spi0_cfg.handle, SPI_IOC_RD_BITS_PER_WORD, &spi0_cfg.bits_per_word);
	if (err_code) {
		DEBUG_PASS("spi0_driver_configure() - Can't get bits per word - SPI_IOC_RD_BITS_PER_WORD\n");
		return;
	}
	
	err_code = ioctl(spi0_cfg.handle, SPI_IOC_WR_MAX_SPEED_HZ, &spi0_cfg.speed);
	if (err_code) {
		DEBUG_PASS("spi0_driver_configure() - Can't set max speed hz - SPI_IOC_WR_MAX_SPEED_HZ");
		return;
	}

	err_code = ioctl(spi0_cfg.handle, SPI_IOC_RD_MAX_SPEED_HZ, &spi0_cfg.speed);
	if (err_code) {
		DEBUG_PASS("spi0_driver_configure() - Can't get max speed hz - SPI_IOC_RD_MAX_SPEED_HZ");
		return;
	}

	SPI0_STATUS_clear_all();
	SPI0_STATUS_set(SPI0_STATUS_INITIALIZED);
}

/*!
 *
 */
void spi0_driver_power_off(void) {

	if (spi0_cfg.handle == SPI0_DEVICE_HANDLE_INVALID) {
		DEBUG_PASS("spi0_driver_power_off() - device is not open");
		return;
	}
		
	DEBUG_PASS("spi0_driver_power_off()");

	close(spi0_cfg.handle);
	spi0_cfg.handle = SPI0_DEVICE_HANDLE_INVALID;
}

u8 spi0_driver_bytes_available(void) {
	return SPI0_RX_BUFFER_bytes_available();
}


u8 spi0_driver_get_N_bytes(u8 num_bytes, u8* p_buffer_to) {

	SPI0_RX_BUFFER_start_read();
	u16 num_bytes_read = SPI0_RX_BUFFER_get_N_bytes(num_bytes, p_buffer_to);
	SPI0_RX_BUFFER_stop_read();

	DEBUG_TRACE_N(num_bytes, p_buffer_to, "spi0_driver_get_N_bytes()");

	return num_bytes_read;
}


u8 spi0_driver_set_N_bytes(u8 num_bytes, const u8* p_buffer_from) {

	if (num_bytes > SPI0_TX_BUFFER_size()) {
		num_bytes = SPI0_TX_BUFFER_size();
	}

	DEBUG_TRACE_N(num_bytes, p_buffer_from, "spi0_driver_set_N_bytes()");

	SPI0_TX_BUFFER_start_write(); // this will delete all data added so far
	SPI0_TX_BUFFER_add_N_bytes(num_bytes, p_buffer_from);
	SPI0_TX_BUFFER_stop_write();

	return num_bytes;
}

u8 spi0_driver_is_ready_for_tx(void) {
	return SPI0_STATUS_is_set(SPI0_STATUS_TX_ACTIVE) == 0 ? 1 : 0;
}

u8 spi0_driver_is_ready_for_rx(void) {
	return SPI0_STATUS_is_set(SPI0_STATUS_RX_ACTIVE) == 0 ? 1 : 0;
}


void spi0_driver_start_rx(u16 num_of_rx_bytes) {

	if (spi0_cfg.handle == SPI0_DEVICE_HANDLE_INVALID) {
		DEBUG_PASS("spi0_driver_start_rx() - device is not open !!! --- !!! --- !!!");
		return;
	}
		
	DEBUG_TRACE_word(num_of_rx_bytes, "spi0_driver_start_rx()");
	
	u8 tmp_tx_buffer[SPI0_DRIVER_TX_BUFFER_SIZE];	
	u8 tmp_rx_buffer[SPI0_DRIVER_RX_BUFFER_SIZE];

	if (num_of_rx_bytes > SPI0_DRIVER_RX_BUFFER_SIZE) {
		num_of_rx_bytes = SPI0_DRIVER_TX_BUFFER_SIZE;
	}

	memset(tmp_tx_buffer, 0x00, num_of_rx_bytes);

	struct spi_ioc_transfer spi_tr = {
		.tx_buf = (unsigned long)tmp_tx_buffer,
		.rx_buf = (unsigned long)tmp_rx_buffer,
		.len = num_of_rx_bytes,
		.delay_usecs = 0,
		.speed_hz = spi0_cfg.speed,
		.bits_per_word = spi0_cfg.bits_per_word,
		.tx_nbits = 0
	};
	
	SPI0_CS_drive_low();
	int err_code = (u8)ioctl(spi0_cfg.handle, SPI_IOC_MESSAGE(1), &spi_tr);
	SPI0_CS_pull_up();

	DEBUG_TRACE_N(num_of_rx_bytes, tmp_rx_buffer, "spi0_driver_start_rx() - data received");

	SPI0_RX_BUFFER_start_write(); // this will delete all data added so far
	SPI0_RX_BUFFER_add_N_bytes(num_of_rx_bytes, tmp_rx_buffer);
	SPI0_RX_BUFFER_stop_write();
	
	if (err_code) {
		DEBUG_TRACE_byte(err_code , "spi0_driver_start_rx() - SPI-TRANSFER has FAILED !!! - error:");
	}
}

void spi0_driver_wait_for_rx(u8 num_bytes, u16 timeout_ms) {

}

void spi0_driver_stop_rx(void) {

}

void spi0_driver_start_tx(void) {

	if (spi0_cfg.handle == SPI0_DEVICE_HANDLE_INVALID) {
		DEBUG_PASS("spi0_driver_start_tx() - device is not open !!! --- !!! --- !!!");
		return;
	}
	
	if (SPI0_TX_BUFFER_bytes_available() == 0) {
		DEBUG_PASS("spi0_driver_start_tx() - Nothing to trasnmit - device is not open !!! --- !!! --- !!!");
		return;
	}
		
	DEBUG_PASS("spi0_driver_start_tx()");
	
	u8 tmp_tx_buffer[SPI0_DRIVER_TX_BUFFER_SIZE];	
	u8 tmp_rx_buffer[SPI0_DRIVER_RX_BUFFER_SIZE];

	size_t num_bytes = (size_t) SPI0_TX_BUFFER_bytes_available();
	if (num_bytes > SPI0_DRIVER_TX_BUFFER_SIZE) {
		num_bytes = SPI0_DRIVER_TX_BUFFER_SIZE;
	}

	SPI0_TX_BUFFER_start_read();
	num_bytes = (size_t) SPI0_TX_BUFFER_get_N_bytes(num_bytes, tmp_tx_buffer);
	SPI0_TX_BUFFER_stop_read();
		
	DEBUG_TRACE_word((u16)num_bytes, "spi0_driver_start_tx() - Number of bytes to transmit: ");
	DEBUG_TRACE_N(num_bytes, tmp_tx_buffer, "spi0_driver_start_tx() - data to transmit");

	struct spi_ioc_transfer spi_tr = {
		.tx_buf = (unsigned long)tmp_tx_buffer,
		.rx_buf = (unsigned long)tmp_rx_buffer,
		.len = num_bytes,
		.delay_usecs = 0,
		.speed_hz = spi0_cfg.speed,
		.bits_per_word = spi0_cfg.bits_per_word,
		.tx_nbits = 0
	};
	
	SPI0_CS_drive_low();
	int err_code = (u8)ioctl(spi0_cfg.handle, SPI_IOC_MESSAGE(1), &spi_tr);
	SPI0_CS_pull_up();

	DEBUG_TRACE_N(num_bytes, tmp_rx_buffer, "spi0_driver_start_tx() - data received");
	
	if (err_code) {
		DEBUG_TRACE_byte(err_code , "spi0_driver_start_tx() - SPI-TRANSFER has FAILED !!! - error:");
	}
}

void spi0_driver_wait_for_tx(u8 num_bytes, u16 timeout_ms) {

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

	if (SPI0_MUTEX_is_requested() != 0) {
		return MUTEX_INVALID_ID;
	}

	return SPI0_MUTEX_request();
}

void spi0_driver_mutex_release(u8 m_id) {
	SPI0_MUTEX_release(m_id);
}
