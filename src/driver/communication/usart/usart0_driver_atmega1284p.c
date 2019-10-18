/*! \file *********************************************************************

 *****************************************************************************/

#define TRACER_OFF

/*-------------------------------------------------------------------------------------------------------------------------------------*/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "tracer.h"

/*-------------------------------------------------------------------------------------------------------------------------------------*/

#include "cfg_driver_interface.h"
#include "local_msg_buffer.h"
#include "local_usart_driver.h"
#include "local_module_status.h"

/*-------------------------------------------------------------------------------------------------------------------------------------*/

#define USART_DRIVER_RX_UNLIMITED	0xFFFF

/*-------------------------------------------------------------------------------------------------------------------------------------*/

#define LOCAL_USART_RX_TRACES
#define LOCAL_USART_TX_TRACES

#if defined TRACES_ENABLED && defined LOCAL_USART_RX_TRACES
#define LOCAL_USART_RX_PASS()			PASS()
#define LOCAL_USART_RX_TRACE_byte(v)		TRACE_byte(v)
#define LOCAL_USART_RX_TRACE_word(v)		TRACE_word(v)
#define LOCAL_USART_RX_TRACE_long(v)		TRACE_long(v)
#define LOCAL_USART_RX_TRACE_N(n,v)		TRACE_N(n,v)
#else
#define LOCAL_USART_RX_PASS()
#define LOCAL_USART_RX_TRACE_byte(v)
#define LOCAL_USART_RX_TRACE_word(v)
#define LOCAL_USART_RX_TRACE_long(v)
#define LOCAL_USART_RX_TRACE_N(n,v)
#endif


#if defined TRACES_ENABLED && defined LOCAL_USART_TX_TRACES
#define LOCAL_USART_TX_PASS()			PASS()
#define LOCAL_USART_TX_TRACE_byte(v)		TRACE_byte(v)
#define LOCAL_USART_TX_TRACE_word(v)		TRACE_word(v)
#define LOCAL_USART_TX_TRACE_long(v)		TRACE_long(v)
#define LOCAL_USART_TX_TRACE_N(n,v)		TRACE_N(n,v)
#else
#define LOCAL_USART_TX_PASS()
#define LOCAL_USART_TX_TRACE_byte(v)
#define LOCAL_USART_TX_TRACE_word(v)
#define LOCAL_USART_TX_TRACE_long(v)
#define LOCAL_USART_TX_TRACE_N(n,v)
#endif

/*-------------------------------------------------------------------------------------------------------------------------------------*/

#define LOCAL_USART_STATUS_RX_ACTIVE		(1 << 0)
#define LOCAL_USART_STATUS_TX_ACTIVE		(1 << 1)

/*-------------------------------------------------------------------------------------------------------------------------------------*/

BUILD_LOCAL_MSG_BUFFER(static inline, __local_usart_tx_buffer, LOCAL_USART_DRIVER_MAX_NUM_BYTES_TRANSMIT_BUFFER)
BUILD_LOCAL_MSG_BUFFER(static inline, __local_usart_rx_buffer, LOCAL_USART_DRIVER_MAX_NUM_BYTES_RECEIVE_BUFFER)

BUILD_MODULE_STATUS_FAST_VOLATILE(local_usart_status, 2)

/*-------------------------------------------------------------------------------------------------------------------------------------*/

static u16 remote_usart_rx_bytes = 0;

/*-------------------------------------------------------------------------------------------------------------------------------------*/

void usart_driver_initialize(void) {

}

void usart_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg) {

	(void) p_cfg;
	PASS();	// local_usart_driver_cfg()

	__local_usart_rx_buffer_init();
	__local_usart_tx_buffer_init();

	usart_driver_clear_buffer();

	// Baudrate 115200
	UBRR0L = 1;
	UBRR0H = 0;

	/* Set frame format: 8data, 1stop bit */
	UCSR0C = (0 << USBS0) | (3 << UCSZ00);

	/* Enable receiver and transmitter */
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);// | (1 << RXCIE0) | (1 << TXCIE0);
}

void usart_driver_power_off(void) {
	PASS(); // local_usart_driver_power_off()
	__local_usart_rx_buffer_clear_all();
	__local_usart_tx_buffer_clear_all();
}

u8 usart_driver_bytes_available (void) {

	#if defined TRACES_ENABLED && defined LOCAL_USART_RX_TRACES
	{
		u8 bytes_available = __local_usart_rx_buffer_bytes_available();
		if (bytes_available) {
			LOCAL_USART_RX_TRACE_byte(bytes_available); // i2c_driver_bytes_available()
		}
	}
	#endif

	return __local_usart_rx_buffer_bytes_available();
}



u8 usart_driver_get_N_bytes (u8 num_bytes, u8* p_buffer_to) {

	PASS();	// local_usart_driver_get_N_bytes()

	u8 num_bytes_available = __local_usart_rx_buffer_bytes_available();

	if (num_bytes < num_bytes_available) {
		num_bytes_available = num_bytes;
	}

	__local_usart_rx_buffer_start_read();
	__local_usart_rx_buffer_get_N_bytes(num_bytes_available, p_buffer_to);
	__local_usart_rx_buffer_stop_read();

	LOCAL_USART_RX_TRACE_N(num_bytes_available, p_buffer_to); // local_usart_driver_bytes_available()

	return num_bytes_available;
}

u8 usart_driver_set_N_bytes (u8 num_bytes, const u8* p_buffer_from) {
	if (num_bytes > __local_usart_tx_buffer_size()) {
		num_bytes = __local_usart_tx_buffer_size();
	}

	LOCAL_USART_TX_TRACE_N(num_bytes, p_buffer_from); // local_usart_driver_bytes_available()

	__local_usart_tx_buffer_start_write(); // this will delete all data added so far
	__local_usart_tx_buffer_add_N_bytes(num_bytes, p_buffer_from);
	__local_usart_tx_buffer_stop_write();

	return num_bytes;
}

u8 usart_driver_is_ready_for_rx(void) {
	return 1;
}

void usart_driver_start_rx (u16 num_of_rx_bytes) {

	LOCAL_USART_RX_TRACE_word(num_of_rx_bytes); // local_usart_driver_start_rx()

	remote_usart_rx_bytes = num_of_rx_bytes;

	__local_usart_rx_buffer_start_write();
	local_usart_status_set(LOCAL_USART_STATUS_RX_ACTIVE);

	LOCAL_USART_STARTSIGNAL(); PASS(); // START sent
}

void usart_driver_wait_for_rx(u8 num_bytes, u16 timeout_ms) {
	(void) num_bytes;
	(void) timeout_ms;
}

void usart_driver_stop_rx (void) {

	LOCAL_USART_RX_PASS(); // local_usart_driver_stop_rx()

	local_usart_status_unset(LOCAL_USART_STATUS_RX_ACTIVE);
	__local_usart_rx_buffer_stop_write();
}

u8 usart_driver_is_ready_for_tx (void) {

	return local_usart_status_is_set(LOCAL_USART_STATUS_TX_ACTIVE) == 0 ? 1 : 0;
}

void usart_driver_start_tx (void) {

	LOCAL_USART_TX_PASS(); // local_usart_driver_start_tx()

	local_usart_status_set(LOCAL_USART_STATUS_TX_ACTIVE);
	__local_usart_tx_buffer_start_read();

	while (__local_usart_tx_buffer_bytes_available() > 0) {

		while ( !( UCSR0A & (1<<UDRE0)) );

		u8 byte = __local_usart_tx_buffer_get_byte();
		LOCAL_USART_TX_TRACE_byte(byte); // local_usart_driver_start_tx
		UDR0 = byte;
	}

	__local_usart_tx_buffer_stop_read();

//	UDR0 = __local_usart_tx_buffer_get_byte();
}

void usart_driver_wait_for_tx(u8 num_bytes, u16 timeout_ms) {
	(void) num_bytes;
	(void) timeout_ms;
}

void usart_driver_stop_tx (void) {

	LOCAL_USART_TX_PASS(); // local_usart_driver_stop_tx()

	__local_usart_tx_buffer_stop_read();
	local_usart_status_unset(LOCAL_USART_STATUS_TX_ACTIVE);
}

void usart_driver_clear_buffer (void) {

	PASS(); // local_usart_driver_clear_buffer()
	__local_usart_rx_buffer_clear_all();
	__local_usart_tx_buffer_clear_all();
}

void usart_driver_set_address (u8 addr) {
	(void) addr;
}

u8 usart_driver_mutex_request(void) {
	return 1;
}

void usart_driver_mutex_release(u8 m_id) {
	(void) m_id;
}


ISR(USART0_TX_vect) {

	PASS(); // TX complete

	if (__local_usart_tx_buffer_bytes_available() > 0) {
		UDR0 = __local_usart_tx_buffer_get_byte();

	} else {
		usart_driver_stop_tx();
	}
}


ISR(USART0_RX_vect) {

	PASS(); // RX complete

	if (remote_usart_rx_bytes != 0) {

		u8 byte = UDR0;
		__local_usart_rx_buffer_add_byte(byte);

		if (remote_usart_rx_bytes != TRX_DRIVER_INTERFACE_UNLIMITED_RX_LENGTH) {

			if (remote_usart_rx_bytes-- == 0) {
				usart_driver_stop_rx();
			}
		}
	}
}
