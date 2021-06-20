/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	driver/communication/usart/usart0_driver_raspberry_pi.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

#ifdef TRACER_ON
#pragma __WARNING__TRACES_ENABLED__
#endif

// --------------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "tracer.h"

// --------------------------------------------------------------------------------

#include "cfg_driver_interface.h"
#include "local_msg_buffer.h"
#include "driver/communication/usart/usart0_driver.h"
#include "local_module_status.h"
#include "time_management/time_management.h"
#include "mcu_task_management/thread_interface.h"

// --------------------------------------------------------------------------------

#include <stdio.h>
#include <unistd.h>		//Used for UART
#include <fcntl.h>		//Used for UART
#include <termios.h>		//Used for UART

// --------------------------------------------------------------------------------

#define USART_DRIVER_RX_UNLIMITED	0xFFFF

// --------------------------------------------------------------------------------

BUILD_LOCAL_MSG_BUFFER(static inline, USART0_TX_BUFFER, USART0_DRIVER_MAX_NUM_BYTES_TRANSMIT_BUFFER)
BUILD_LOCAL_MSG_BUFFER(static inline, USART0_RX_BUFFER, USART0_DRIVER_MAX_NUM_BYTES_RECEIVE_BUFFER)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(USART0_TIMEOUT_TIMER)

// --------------------------------------------------------------------------------

#define USART0_STATUS_RX_ACTIVE		0
#define USART0_STATUS_TX_ACTIVE		1
#define USART0_STATUS_INITIALIZED	2
#define USART0_STATUS_TERMINATED	3

BUILD_MODULE_STATUS_FAST_VOLATILE(USART0_STATUS, 2)

// --------------------------------------------------------------------------------

static u16 remote_usart_rx_bytes = 0;

/**
 * @brief 
 * 
 */
static int device_handle_id;

// --------------------------------------------------------------------------------

/**
 * @brief runs in background in a seperate thread, reading incomming data from 
 * the usart interface
 * 
 */
static void usart0_driver_raspberry_pi_background_read(void* p_arg);

/**
 * @brief Termiantes the thread of this module
 * 
 */
static void usart0_driver_raspberry_pi_thread_terminate(void);

// ------------------------------------------------------------------------------

THREAD_INTERFACE_BUILD_THREAD(USART0_DRIVER_RPI_RX_THREAD, THREAD_PRIORITY_MIDDLE, usart0_driver_raspberry_pi_background_read, usart0_driver_raspberry_pi_thread_terminate)

// --------------------------------------------------------------------------------

void usart0_driver_initialize(void) {
	DEBUG_PASS("usart0_driver_initialize()");
}

void usart0_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg) {

	if (USART0_STATUS_is_set(USART0_STATUS_INITIALIZED)) {
		DEBUG_PASS("usart0_driver_configure() - is already initialized");
		return;
	}

	DEBUG_PASS("usart0_driver_configure()");

	USART0_RX_BUFFER_init();
	USART0_TX_BUFFER_init();

	//Open in non blocking read/write mode
	device_handle_id = open(p_cfg->device.name, O_RDWR | O_NOCTTY | O_NDELAY);
	
	//CONFIGURE THE UART
	//The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
	//	Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
	//	CSIZE:- CS5, CS6, CS7, CS8
	//	CLOCAL - Ignore modem status lines
	//	CREAD - Enable receiver
	//	IGNPAR = Ignore characters with parity errors
	//	ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
	//	PARENB - Parity enable
	//	PARODD - Odd parity (else even)
	struct termios options;
	tcgetattr(device_handle_id, &options);

	options.c_cflag = CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;

	switch (p_cfg->module.usart.baudrate) {
		default:
		case BAUDRATE_9600:	options.c_cflag |= B9600;   DEBUG_PASS("usart0_driver_configure() - BAUDRATE_9600"); break;
		case BAUDRATE_19200:	options.c_cflag |= B19200;  DEBUG_PASS("usart0_driver_configure() - BAUDRATE_19200"); break;
		case BAUDRATE_38400:	options.c_cflag |= B38400;  DEBUG_PASS("usart0_driver_configure() - BAUDRATE_38400"); break;
		case BAUDRATE_115200:	options.c_cflag |= B115200; DEBUG_PASS("usart0_driver_configure() - BAUDRATE_115200"); break;
		case BAUDRATE_230400:	options.c_cflag |= B230400; DEBUG_PASS("usart0_driver_configure() - BAUDRATE_230400"); break;
	}

	switch (p_cfg->module.usart.databits) {
		default:
		case DATABITS_8:	options.c_cflag |= CS8; DEBUG_PASS("usart0_driver_configure() - DATABITS_8"); break;
		case DATABITS_5:	options.c_cflag |= CS5; DEBUG_PASS("usart0_driver_configure() - DATABITS_5"); break;
		case DATABITS_6:	options.c_cflag |= CS6; DEBUG_PASS("usart0_driver_configure() - DATABITS_6"); break;
		case DATABITS_7:	options.c_cflag |= CS7; DEBUG_PASS("usart0_driver_configure() - DATABITS_7"); break;
		//case DATABITS_9:	options.c_cflag |= CS8; DEBUG_PASS("usart0_driver_configure()"); break;
	}

	switch (p_cfg->module.usart.stopbits) {
		default:
		case STOPBITS_1:	DEBUG_PASS("usart0_driver_configure() - STOPBITS_1"); break;
		case STOPBITS_2:	options.c_cflag |= CSTOPB; DEBUG_PASS("usart0_driver_configure() - STOPBITS_2"); break;
	}

	switch (p_cfg->module.usart.parity) {
		default:
		case PARITY_NONE:	USART0_DRIVER_SET_PARITY_NONE(); DEBUG_PASS("usart0_driver_configure()"); break;
		case PARITY_EVEN:	options.c_cflag |= PARENB; USART0_DRIVER_SET_PARITY_EVEN(); DEBUG_PASS("usart0_driver_configure()"); break;
		case PARITY_ODD:	options.c_cflag |= (PARENB | PARODD); USART0_DRIVER_SET_PARITY_ODD();  DEBUG_PASS("usart0_driver_configure()"); break;
	}

	tcflush(p_device->_handle_id, TCIFLUSH);
	tcsetattr(p_device->_handle_id, TCSANOW, &options);

	USART0_STATUS_set(USART0_STATUS_INITIALIZED);

	USART0_DRIVER_RPI_RX_THREAD_start();
}

void usart0_driver_power_off(void) {
	DEBUG_PASS("local_usart_driver_power_off()");
	USART0_RX_BUFFER_clear_all();
	USART0_TX_BUFFER_clear_all();

	USART0_DRIVER_RPI_RX_THREAD_terminate();
}

u16 usart0_driver_bytes_available (void) {

	#if defined TRACES_ENABLED && defined LOCAL_USART_RX_TRACES
	{
		u16 bytes_available = USART0_RX_BUFFER_bytes_available();
		if (bytes_available) {
			DEBUG_TRACE_word(bytes_available, "i2c_driver_bytes_available()");
		}
	}
	#endif

	return USART0_RX_BUFFER_bytes_available();
}

u16 usart0_driver_get_N_bytes (u16 num_bytes, u8* p_buffer_to) {

	DEBUG_PASS("usart0_driver_get_N_bytes()");

	u16 num_bytes_available = USART0_RX_BUFFER_bytes_available();

	if (num_bytes < num_bytes_available) {
		num_bytes_available = num_bytes;
	}

	USART0_RX_BUFFER_start_read();
	USART0_RX_BUFFER_get_N_bytes(num_bytes_available, p_buffer_to);
	USART0_RX_BUFFER_stop_read();

	DEBUG_TRACE_N(num_bytes_available, p_buffer_to, "local_usart_driver_bytes_available()");

	return num_bytes_available;
}

u16 usart0_driver_set_N_bytes (u16 num_bytes, const u8* p_buffer_from) {

	if (num_bytes > USART0_TX_BUFFER_size()) {
		num_bytes = USART0_TX_BUFFER_size();
	}

	DEBUG_TRACE_N(num_bytes, p_buffer_from, "local_usart_driver_bytes_available()");

	USART0_TX_BUFFER_start_write(); // this will delete all data added so far
	USART0_TX_BUFFER_add_N_bytes(num_bytes, p_buffer_from);
	USART0_TX_BUFFER_stop_write();

	return num_bytes;
}

u8 usart0_driver_is_ready_for_rx(void) {
	if (USART0_STATUS_is_set(USART0_STATUS_INITIALIZED)) {
		return 1;
	}

	return 0;
}

void usart0_driver_start_rx(u16 num_of_rx_bytes) {

	DEBUG_TRACE_word(num_of_rx_bytes, "local_usart_driver_start_rx()");

	remote_usart_rx_bytes = num_of_rx_bytes;
	USART0_STATUS_set(USART0_STATUS_RX_ACTIVE);
}

void usart0_driver_wait_for_rx(u16 num_bytes, u16 timeout_ms) {

	USART0_TIMEOUT_TIMER_start();

	while (USART0_RX_BUFFER_bytes_available() < num_bytes) {

		if (USART0_TIMEOUT_TIMER_is_up(timeout_ms)) {
			DEBUG_PASS("usart0_driver_wait_for_rx() - TIMEOUT !!! ---");
			break;
		}

		usleep(50000); // reduce cpu-load
	}
}

void usart0_driver_stop_rx (void) {

	LOCAL_USART_RX_PASS(); // local_usart_driver_stop_rx()
	USART0_STATUS_unset(USART0_STATUS_RX_ACTIVE);
}

u8 usart0_driver_is_ready_for_tx (void) {

	return USART0_STATUS_is_set(USART0_STATUS_TX_ACTIVE) == 0 ? 1 : 0;
}

void usart0_driver_start_tx (void) {

	LOCAL_USART_TX_PASS(); // local_usart_driver_start_tx()

	USART0_STATUS_set(USART0_STATUS_TX_ACTIVE);
	USART0_TX_BUFFER_start_read();

	while (USART0_TX_BUFFER_bytes_available() > 0) {

		USART0_DRIVER_WAIT_UNTIL_TX_RDY();

		u8 byte = USART0_TX_BUFFER_get_byte();
		LOCAL_USART_TX_TRACE_byte(byte); // local_usart_driver_start_tx
		USART0_DRIVER_SET_BYTE(byte);
	}

	USART0_TX_BUFFER_stop_read();
}

void usart0_driver_wait_for_tx(u16 num_bytes, u16 timeout_ms) {
	(void) num_bytes;
	(void) timeout_ms;
}

void usart0_driver_stop_tx(void) {

	DEBUG_PASS("local_usart_driver_stop_tx()");

	USART0_TX_BUFFER_stop_read();
	USART0_STATUS_unset(USART0_STATUS_TX_ACTIVE);
}

void usart0_driver_clear_rx_buffer(void) {
	USART0_RX_BUFFER_clear_all();
}

void usart0_driver_clear_tx_buffer(void) {
	USART0_TX_BUFFER_clear_all();
}

void usart0_driver_set_address(u8 addr) {
	(void) addr;
}

u8 usart0_driver_mutex_request(void) {
	return 1;
}

void usart0_driver_mutex_release(u8 m_id) {
	(void) m_id;
}

// --------------------------------------------------------------------------------

static void usart0_driver_raspberry_pi_background_read(void* p_arg) {

	(void) p_arg;

	while (1) {

		usleep(50000);

		if (USART0_STATUS_is_set(USART0_STATUS_TERMINATED)) {
			break;
		}

		if (USART0_STATUS_is_set(USART0_STATUS_RX_ACTIVE) == 0) {
			continue;
		}

		unsigned char byte = 0;
		int length = read(device_handle_id, (void*)(&byte), 1);

		if (length > 0) {

			USART0_RX_BUFFER_start_write();
			USART0_RX_BUFFER_add_byte(byte);
			USART0_RX_BUFFER_stop_write();
		}
	}

	close(device_handle_id);
	USART0_STATUS_unset(USART0_STATUS_INITIALIZED);
}

static void usart0_driver_raspberry_pi_thread_terminate(void) {

	DEBUG_PASS("usart0_driver_raspberry_pi_thread_terminate()");
	USART0_STATUS_set(USART0_STATUS_TERMINATED);
}
