/*! \file *********************************************************************

 *****************************************************************************/

#define TRACER_OFF

/*-------------------------------------------------------------------------------------------------------------------------------------*/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "cfg_driver_interface.h"
#include "local_msg_buffer.h"
#include "driver/communication/usart/usart1_driver_atmega1284p.h"
#include "local_module_status.h"

/*-------------------------------------------------------------------------------------------------------------------------------------*/

#include "tracer.h"

/*-------------------------------------------------------------------------------------------------------------------------------------*/

BUILD_LOCAL_MSG_BUFFER(static inline, usart1_tx_buffer, LOCAL_USART_DRIVER_MAX_NUM_BYTES_TRANSMIT_BUFFER)
BUILD_LOCAL_MSG_BUFFER(static inline, usart1_rx_buffer, LOCAL_USART_DRIVER_MAX_NUM_BYTES_RECEIVE_BUFFER)

BUILD_MODULE_STATUS_FAST_VOLATILE(usart1_status, 2)

#define USART1_STATUS_RX_ACTIVE		(1 << 0)
#define USART1_STATUS_TX_ACTIVE		(1 << 1)

/*-------------------------------------------------------------------------------------------------------------------------------------*/

#define USART1_DRIVER_SET_BAUDRATE_9600()		UBRR1L = 47; UBRR1H = 0
#define USART1_DRIVER_SET_BAUDRATE_19200()		UBRR1L = 23; UBRR1H = 0
#define USART1_DRIVER_SET_BAUDRATE_38400()		UBRR1L = 11; UBRR1H = 0
#define USART1_DRIVER_SET_BAUDRATE_115200()		UBRR1L = 3; UBRR1H = 0
#define USART1_DRIVER_SET_BAUDRATE_230400()		UBRR1L = 1; UBRR1H = 0

/*-------------------------------------------------------------------------------------------------------------------------------------*/

static u16 remote_usart_rx_bytes = 0;

void usart1_driver_initialize(void) {

}

void usart1_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg) {

	(void) p_cfg;
	PASS();	// local_usart_driver_cfg()

	usart1_rx_buffer_init();
	usart1_tx_buffer_init();

	usart_driver_clear_buffer();

	// Baudrate 115200
	//UBRR1L = 1;
	//UBRR1H = 0;
	switch (p_cfg->usart.baud_rate) {
		case BAUDRATE_9600:	USART1_DRIVER_SET_BAUDRATE_9600(); break;
		case BAUDRATE_19200:	USART1_DRIVER_SET_BAUDRATE_19200(); break;
		case BAUDRATE_38400:	USART1_DRIVER_SET_BAUDRATE_38400(); break;
		case BAUDRATE_115200:	USART1_DRIVER_SET_BAUDRATE_115200(); break;
		case BAUDRATE_230400:	USART1_DRIVER_SET_BAUDRATE_230400(); break;
	}

	/* Set frame format: 8data, 1stop bit */
	UCSR1C = (0 << USBS1) | (3 << UCSZ10);

	/* Enable receiver and transmitter */
	UCSR1B = (1 << RXEN1) | (1 << TXEN1);// | (1 << RXCIE1) | (1 << TXCIE1);
}

void usart1_driver_power_off(void) {
	PASS(); // local_usart_driver_power_off()
	usart1_rx_buffer_clear_all();
	usart1_tx_buffer_clear_all();
}

u8 usart1_driver_bytes_available (void) {
	#if defined TRACES_ENABLED && defined LOCAL_USART_RX_TRACES
	{
		u8 bytes_available = usart1_rx_buffer_bytes_available();
		if (bytes_available) {
			USART1_RX_TRACE_byte(bytes_available); // i2c_driver_bytes_available()
		}
	}
	#endif

	return usart1_rx_buffer_bytes_available();
}

u8 usart1_driver_get_N_bytes (u8 num_bytes, u8* p_buffer_to) {

	PASS();	// local_usart_driver_get_N_bytes()

	u8 num_bytes_available = usart1_rx_buffer_bytes_available();

	if (num_bytes < num_bytes_available) {
		num_bytes_available = num_bytes;
	}

	usart1_rx_buffer_start_read();
	usart1_rx_buffer_get_N_bytes(num_bytes_available, p_buffer_to);
	usart1_rx_buffer_stop_read();

	TRACE_N(num_bytes_available, p_buffer_to); // local_usart_driver_bytes_available()

	return num_bytes_available;
}

u8 usart1_driver_set_N_bytes (u8 num_bytes, const u8* p_buffer_from) {
	if (num_bytes > usart1_tx_buffer_size()) {
		num_bytes = usart1_tx_buffer_size();
	}

	TRACE_N(num_bytes, p_buffer_from); // local_usart_driver_bytes_available()

	usart1_tx_buffer_start_write(); // this will delete all data added so far
	usart1_tx_buffer_add_N_bytes(num_bytes, p_buffer_from);
	usart1_tx_buffer_stop_write();

	return num_bytes;
}

u8 usart1_driver_is_ready_for_rx(void) {
	return 1;
}

void usart1_driver_start_rx (u16 num_of_rx_bytes) {

	TRACE_word(num_of_rx_bytes); // local_usart_driver_start_rx()

	remote_usart_rx_bytes = num_of_rx_bytes;

	usart1_rx_buffer_start_write();
	usart1_status_set(USART1_STATUS_RX_ACTIVE);

	LOCAL_USART_STARTSIGNAL(); PASS(); // START sent
}

void usart1_driver_wait_for_rx(u8 num_bytes, u16 timeout_ms) {
	(void) num_bytes;
	(void) timeout_ms;
}

void usart1_driver_stop_rx (void) {

	PASS(); // local_usart_driver_stop_rx()

	usart1_status_unset(USART1_STATUS_RX_ACTIVE);
	usart1_rx_buffer_stop_write();
}

u8 usart1_driver_is_ready_for_tx (void) {

	return usart1_status_is_set(USART1_STATUS_TX_ACTIVE) == 0 ? 1 : 0;
}

void usart1_driver_start_tx (void) {

	PASS(); // local_usart_driver_start_tx()

	usart1_status_set(USART1_STATUS_TX_ACTIVE);
	usart1_tx_buffer_start_read();

	while (usart1_tx_buffer_bytes_available() > 0) {

		while ( !( UCSR0A & (1<<UDRE0)) );

		u8 byte = usart1_tx_buffer_get_byte();
		LOCAL_USART_TX_TRACE_byte(byte); // local_usart_driver_start_tx
		UDR0 = byte;
	}

	usart1_tx_buffer_stop_read();

//	UDR0 = usart1_tx_buffer_get_byte();
}

void usart1_driver_wait_for_tx(u8 num_bytes, u16 timeout_ms) {
	(void) num_bytes;
	(void) timeout_ms;
}

void usart1_driver_stop_tx (void) {

	PASS(); // local_usart_driver_stop_tx()

	usart1_tx_buffer_stop_read();
	usart1_status_unset(USART1_STATUS_TX_ACTIVE);
}

void usart1_driver_clear_buffer (void) {

	PASS(); // local_usart_driver_clear_buffer()
	usart1_rx_buffer_clear_all();
	usart1_tx_buffer_clear_all();
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

	PASS(); // TX complete

	if (usart1_tx_buffer_bytes_available() > 0) {
		UDR1 = usart1_tx_buffer_get_byte();

	} else {
		usart1_driver_stop_tx();
	}
}


ISR(USART1_RX_vect) {

	PASS(); // RX complete

	if (remote_usart_rx_bytes != 0) {

		u8 byte = UDR1;
		usart1_rx_buffer_add_byte(byte);

		if (remote_usart_rx_bytes != TRX_DRIVER_INTERFACE_UNLIMITED_RX_LENGTH) {

			if (remote_usart_rx_bytes-- == 0) {
				usart1_driver_stop_rx();
			}
		}
	}



}
