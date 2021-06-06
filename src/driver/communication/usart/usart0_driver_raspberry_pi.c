/*! \file *********************************************************************

 *****************************************************************************/

#define TRACER_OFF

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

BUILD_MODULE_STATUS_FAST_VOLATILE(USART0_STATUS, 2)

// --------------------------------------------------------------------------------

static u16 remote_usart_rx_bytes = 0;

// --------------------------------------------------------------------------------

void usart0_driver_initialize(void) {
	DEBUG_PASS("usart0_driver_initialize()");
}

void usart0_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg) {

	(void) p_cfg;

	DEBUG_PASS("usart0_driver_configure()");

	USART0_RX_BUFFER_init();
	USART0_TX_BUFFER_init();

	USART0_DRIVER_CLEAR_CONFIG();

	switch (p_cfg->module.usart.baudrate) {
		default:
		case BAUDRATE_9600:	USART0_DRIVER_SET_BAUDRATE_9600();   DEBUG_PASS("usart0_driver_configure()"); break;
		case BAUDRATE_19200:	USART0_DRIVER_SET_BAUDRATE_19200();  DEBUG_PASS("usart0_driver_configure()"); break;
		case BAUDRATE_38400:	USART0_DRIVER_SET_BAUDRATE_38400();  DEBUG_PASS("usart0_driver_configure()"); break;
		case BAUDRATE_115200:	USART0_DRIVER_SET_BAUDRATE_115200(); DEBUG_PASS("usart0_driver_configure()"); break;
		case BAUDRATE_230400:	USART0_DRIVER_SET_BAUDRATE_230400(); DEBUG_PASS("usart0_driver_configure()"); break;
	}

	switch (p_cfg->module.usart.databits) {
		default:
		case DATABITS_8:	USART0_DRIVER_SET_DATABITS_8(); DEBUG_PASS("usart0_driver_configure()"); break;
		case DATABITS_5:	USART0_DRIVER_SET_DATABITS_5(); DEBUG_PASS("usart0_driver_configure()"); break;
		case DATABITS_6:	USART0_DRIVER_SET_DATABITS_6(); DEBUG_PASS("usart0_driver_configure()"); break;
		case DATABITS_7:	USART0_DRIVER_SET_DATABITS_7(); DEBUG_PASS("usart0_driver_configure()"); break;
		case DATABITS_9:	USART0_DRIVER_SET_DATABITS_9(); DEBUG_PASS("usart0_driver_configure()"); break;
	}

	switch (p_cfg->module.usart.stopbits) {
		default:
		case STOPBITS_1:	USART0_DRIVER_SET_STOPBITS_1(); DEBUG_PASS("usart0_driver_configure()"); break;
		case STOPBITS_2:	USART0_DRIVER_SET_STOPBITS_2(); DEBUG_PASS("usart0_driver_configure()"); break;
	}

	switch (p_cfg->module.usart.parity) {
		default:
		case PARITY_NONE:	USART0_DRIVER_SET_PARITY_NONE(); DEBUG_PASS("usart0_driver_configure()"); break;
		case PARITY_EVEN:	USART0_DRIVER_SET_PARITY_EVEN(); DEBUG_PASS("usart0_driver_configure()"); break;
		case PARITY_ODD:	USART0_DRIVER_SET_PARITY_ODD();  DEBUG_PASS("usart0_driver_configure()"); break;
	}

	USART0_DRIVER_ENABLE_TX();
	USART0_DRIVER_ENABLE_RX();
}

void usart0_driver_power_off(void) {
	DEBUG_PASS("local_usart_driver_power_off()");
	USART0_RX_BUFFER_clear_all();
	USART0_TX_BUFFER_clear_all();
}

u8 usart0_driver_bytes_available (void) {

	#if defined TRACES_ENABLED && defined LOCAL_USART_RX_TRACES
	{
		u8 bytes_available = USART0_RX_BUFFER_bytes_available();
		if (bytes_available) {
			DEBUG_TRACE_byte(bytes_available, "i2c_driver_bytes_available()");
		}
	}
	#endif

	return USART0_RX_BUFFER_bytes_available();
}

u8 usart0_driver_get_N_bytes (u8 num_bytes, u8* p_buffer_to) {

	DEBUG_PASS("usart0_driver_get_N_bytes()");

	u8 num_bytes_available = USART0_RX_BUFFER_bytes_available();

	if (num_bytes < num_bytes_available) {
		num_bytes_available = num_bytes;
	}

	USART0_RX_BUFFER_start_read();
	USART0_RX_BUFFER_get_N_bytes(num_bytes_available, p_buffer_to);
	USART0_RX_BUFFER_stop_read();

	DEBUG_TRACE_N(num_bytes_available, p_buffer_to, "local_usart_driver_bytes_available()");

	return num_bytes_available;
}

u8 usart0_driver_set_N_bytes (u8 num_bytes, const u8* p_buffer_from) {

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
	return 1;
}

void usart0_driver_start_rx(u16 num_of_rx_bytes) {

	DEBUG_TRACE_word(num_of_rx_bytes, "local_usart_driver_start_rx()");

	remote_usart_rx_bytes = num_of_rx_bytes;

	USART0_RX_BUFFER_start_write();
	USART0_STATUS_set(USART0_STATUS_RX_ACTIVE);
}

void usart0_driver_wait_for_rx(u8 num_bytes, u16 timeout_ms) {

	USART0_TIMEOUT_TIMER_start();

	while (usart0_driver_bytes_available() < num_bytes) {

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
	USART0_RX_BUFFER_stop_write();
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

void usart0_driver_wait_for_tx(u8 num_bytes, u16 timeout_ms) {
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
