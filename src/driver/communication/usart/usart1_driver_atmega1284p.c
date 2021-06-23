/*! \file *********************************************************************

 *****************************************************************************/

#define TRACER_OFF

/*-------------------------------------------------------------------------------------------------------------------------------------*/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "cfg_driver_interface.h"
#include "local_msg_buffer.h"
#include "driver/communication/usart/usart1_driver.h"
#include "local_module_status.h"

/*-------------------------------------------------------------------------------------------------------------------------------------*/

#include "tracer.h"

/*-------------------------------------------------------------------------------------------------------------------------------------*/

#define USART1_DRIVER_CLEAR_CONFIG()			UBRR1L = 0; UBRR1H = 0; UCSR1C = 0; UCSR1B = 0

#define USART1_DRIVER_SET_BAUDRATE_9600()		UBRR1L = 47; UBRR1H = 0
#define USART1_DRIVER_SET_BAUDRATE_19200()		UBRR1L = 23; UBRR1H = 0
#define USART1_DRIVER_SET_BAUDRATE_38400()		UBRR1L = 11; UBRR1H = 0
#define USART1_DRIVER_SET_BAUDRATE_115200()		UBRR1L = 3; UBRR1H = 0
#define USART1_DRIVER_SET_BAUDRATE_230400()		UBRR1L = 1; UBRR1H = 0

#define USART1_DRIVER_SET_DATABITS_5()			UCSR1C |= (0 << UCSZ10)
#define USART1_DRIVER_SET_DATABITS_6()			UCSR1C |= (1 << UCSZ10)
#define USART1_DRIVER_SET_DATABITS_7()			UCSR1C |= (2 << UCSZ10)
#define USART1_DRIVER_SET_DATABITS_8()			UCSR1C |= (3 << UCSZ10)
#define USART1_DRIVER_SET_DATABITS_9()			UCSR1C |= (7 << UCSZ10)

#define USART1_DRIVER_SET_STOPBITS_1()			UCSR1C |= (0 << USBS1)
#define USART1_DRIVER_SET_STOPBITS_2()			UCSR1C |= (1 << USBS1)

#define USART1_DRIVER_SET_PARITY_NONE()			UCSR1C |= (0 << UPM10)
#define USART1_DRIVER_SET_PARITY_EVEN()			UCSR1C |= (2 << UPM10)
#define USART1_DRIVER_SET_PARITY_ODD()			UCSR1C |= (3 << UPM10)

#define USART1_DRIVER_ENABLE_TX()			UCSR1B |= (1 << TXEN1)
#define USART1_DRIVER_ENABLE_RX()			UCSR1B |= (1 << RXEN1)

#define USART1_DRIVER_WAIT_UNTIL_TX_RDY()		while ( !( UCSR1A & (1<<UDRE1)) )

#define USART1_DRIVER_GET_BYTE()			UDR1
#define USART1_DRIVER_SET_BYTE(byte)			UDR1 = byte

/*-------------------------------------------------------------------------------------------------------------------------------------*/

BUILD_LOCAL_MSG_BUFFER(static inline, USART1_TX_BUFFER, USART1_DRIVER_MAX_NUM_BYTES_TRANSMIT_BUFFER)
BUILD_LOCAL_MSG_BUFFER(static inline, USART1_RX_BUFFER, USART1_DRIVER_MAX_NUM_BYTES_RECEIVE_BUFFER)

/*-------------------------------------------------------------------------------------------------------------------------------------*/

#define USART1_STATUS_RX_ACTIVE				0
#define USART1_STATUS_TX_ACTIVE				1

BUILD_MODULE_STATUS_FAST_VOLATILE(usart1_status, 2)

/*-------------------------------------------------------------------------------------------------------------------------------------*/

static u16 usart1_num_bytes_rx = 0;

/*-------------------------------------------------------------------------------------------------------------------------------------*/

void usart1_driver_initialize(void) {

}

void usart1_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg) {

	USART1_RX_BUFFER_init();
	USART1_TX_BUFFER_init();

	USART1_DRIVER_CLEAR_CONFIG();

	switch (p_cfg->module.usart.baudrate) {
		default:
		case BAUDRATE_9600:	USART1_DRIVER_SET_BAUDRATE_9600();   DEBUG_PASS("usart1_driver_configure()"); break;
		case BAUDRATE_19200:	USART1_DRIVER_SET_BAUDRATE_19200();  DEBUG_PASS("usart1_driver_configure()"); break;
		case BAUDRATE_38400:	USART1_DRIVER_SET_BAUDRATE_38400();  DEBUG_PASS("usart1_driver_configure()"); break;
		case BAUDRATE_115200:	USART1_DRIVER_SET_BAUDRATE_115200(); DEBUG_PASS("usart1_driver_configure()"); break;
		case BAUDRATE_230400:	USART1_DRIVER_SET_BAUDRATE_230400(); DEBUG_PASS("usart1_driver_configure()"); break;
	}

	switch (p_cfg->module.usart.databits) {
		default:
		case DATABITS_8:	USART1_DRIVER_SET_DATABITS_8(); DEBUG_PASS("usart1_driver_configure()"); break;
		case DATABITS_5:	USART1_DRIVER_SET_DATABITS_5(); DEBUG_PASS("usart1_driver_configure()"); break;
		case DATABITS_6:	USART1_DRIVER_SET_DATABITS_6(); DEBUG_PASS("usart1_driver_configure()"); break;
		case DATABITS_7:	USART1_DRIVER_SET_DATABITS_7(); DEBUG_PASS("usart1_driver_configure()"); break;
		case DATABITS_9:	USART1_DRIVER_SET_DATABITS_9(); DEBUG_PASS("usart1_driver_configure()"); break;
	}

	switch (p_cfg->module.usart.stopbits) {
		default:
		case STOPBITS_1:	USART1_DRIVER_SET_STOPBITS_1(); DEBUG_PASS("usart1_driver_configure()"); break;
		case STOPBITS_2:	USART1_DRIVER_SET_STOPBITS_2(); DEBUG_PASS("usart1_driver_configure()"); break;
	}

	switch (p_cfg->module.usart.parity) {
		default:
		case PARITY_NONE:	USART1_DRIVER_SET_PARITY_NONE(); DEBUG_PASS("usart1_driver_configure()"); break;
		case PARITY_EVEN:	USART1_DRIVER_SET_PARITY_EVEN(); DEBUG_PASS("usart1_driver_configure()"); break;
		case PARITY_ODD:	USART1_DRIVER_SET_PARITY_ODD();  DEBUG_PASS("usart1_driver_configure()"); break;
	}

	USART1_DRIVER_ENABLE_TX();
	USART1_DRIVER_ENABLE_RX();

	DEBUG_TRACE_byte(UBRR1L, "usart1_driver_configure() - UBRR1L");
	DEBUG_TRACE_byte(UBRR1H, "usart1_driver_configure() - UBRR1H");
	DEBUG_TRACE_byte(UCSR1C, "usart1_driver_configure() - UCSR1C");
	DEBUG_TRACE_byte(UCSR1B, "usart1_driver_configure() - UCSR1B");
	DEBUG_TRACE_byte(UCSR1A, "usart1_driver_configure() - UCSR1A");

	DEBUG_TRACE_byte(UBRR0L, "usart1_driver_configure() - UBRR0L");
	DEBUG_TRACE_byte(UBRR0H, "usart1_driver_configure() - UBRR0H");
	DEBUG_TRACE_byte(UCSR0C, "usart1_driver_configure() - UCSR0C");
	DEBUG_TRACE_byte(UCSR0B, "usart1_driver_configure() - UCSR0B");
	DEBUG_TRACE_byte(UCSR0A, "usart1_driver_configure() - UCSR0A");
}

void usart1_driver_power_off(void) {
	
	USART1_RX_BUFFER_clear_all();
	USART1_TX_BUFFER_clear_all();
}

u16 usart1_driver_bytes_available (void) {
	#if defined TRACES_ENABLED && defined LOCAL_USART_RX_TRACES
	{
		u8 bytes_available = USART1_RX_BUFFER_bytes_available();
		if (bytes_available) {
			USART1_RX_TRACE_byte(bytes_available);
		}
	}
	#endif

	return USART1_RX_BUFFER_bytes_available();
}

u16 usart1_driver_get_N_bytes (u16 num_bytes, u8* p_buffer_to) {

	u16 num_bytes_available = USART1_RX_BUFFER_bytes_available();

	if (num_bytes < num_bytes_available) {
		num_bytes_available = num_bytes;
	}

	USART1_RX_BUFFER_start_read();
	USART1_RX_BUFFER_get_N_bytes(num_bytes_available, p_buffer_to);
	USART1_RX_BUFFER_stop_read();

	return num_bytes_available;
}

u16 usart1_driver_set_N_bytes (u16 num_bytes, const u8* p_buffer_from) {
	if (num_bytes > USART1_TX_BUFFER_size()) {
		num_bytes = USART1_TX_BUFFER_size();
	}

	DEBUG_TRACE_N(num_bytes, (u8*)p_buffer_from, "usart1_driver_set_N_bytes() - Adding bytes to buffer");

	USART1_TX_BUFFER_start_write(); // this will delete all data added so far
	USART1_TX_BUFFER_add_N_bytes(num_bytes, p_buffer_from);
	USART1_TX_BUFFER_stop_write();

	return num_bytes;
}

u8 usart1_driver_is_ready_for_rx(void) {
	return 1;
}

void usart1_driver_start_rx (u16 num_of_rx_bytes) {

	usart1_num_bytes_rx = num_of_rx_bytes;

	USART1_RX_BUFFER_start_write();
	usart1_status_set(USART1_STATUS_RX_ACTIVE);
}

void usart1_driver_wait_for_rx(u16 num_bytes, u16 timeout_ms) {
	(void) num_bytes;
	(void) timeout_ms;
}

void usart1_driver_stop_rx (void) {

	usart1_status_unset(USART1_STATUS_RX_ACTIVE);
	USART1_RX_BUFFER_stop_write();
}

u8 usart1_driver_is_ready_for_tx (void) {

	return usart1_status_is_set(USART1_STATUS_TX_ACTIVE) == 0 ? 1 : 0;
}

void usart1_driver_start_tx (void) {

	DEBUG_PASS("usart1_driver_start_tx()");

	usart1_status_set(USART1_STATUS_TX_ACTIVE);
	USART1_TX_BUFFER_start_read();

	while (USART1_TX_BUFFER_bytes_available() > 0) {

		USART1_DRIVER_WAIT_UNTIL_TX_RDY();

		u8 byte = USART1_TX_BUFFER_get_byte();
		USART1_DRIVER_SET_BYTE(byte);
	}

	USART1_TX_BUFFER_stop_read();

	DEBUG_PASS("usart1_driver_start_tx() - Finished");
}

void usart1_driver_wait_for_tx(u16 num_bytes, u16 timeout_ms) {
	(void) num_bytes;
	(void) timeout_ms;
}

void usart1_driver_stop_tx (void) {

	USART1_TX_BUFFER_stop_read();
	usart1_status_unset(USART1_STATUS_TX_ACTIVE);
}

void usart1_driver_clear_rx_buffer(void) {
	USART1_RX_BUFFER_clear_all();
}

void usart1_driver_clear_tx_buffer(void) {
	USART1_TX_BUFFER_clear_all();
}

void usart1_driver_set_address (u8 addr) {
	(void) addr;
}

u8 usart1_driver_mutex_request(void) {
	return 1;
}

void usart1_driver_mutex_release(u8 m_id) {
	(void) m_id;
}


ISR(USART1_TX_vect) {

	if (USART1_TX_BUFFER_bytes_available() > 0) {
		USART1_DRIVER_SET_BYTE(USART1_TX_BUFFER_get_byte());

	} else {
		usart1_driver_stop_tx();
	}
}


ISR(USART1_RX_vect) {

	if (usart1_num_bytes_rx != 0) {

		u8 byte = USART1_DRIVER_GET_BYTE();
		USART1_RX_BUFFER_add_byte(byte);

		if (usart1_num_bytes_rx != TRX_DRIVER_INTERFACE_UNLIMITED_RX_LENGTH) {

			if (usart1_num_bytes_rx-- == 0) {
				usart1_driver_stop_rx();
			}
		}
	}



}
