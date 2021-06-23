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
#include "driver/communication/usart/usart0_driver.h"
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

#define USART0_DRIVER_CLEAR_CONFIG()			UBRR0L = 0; UBRR0H = 0; UCSR0C = 0; UCSR0B = 0

#define USART0_DRIVER_SET_BAUDRATE_9600()		UBRR0L = 47; UBRR0H = 0
#define USART0_DRIVER_SET_BAUDRATE_19200()		UBRR0L = 23; UBRR0H = 0
#define USART0_DRIVER_SET_BAUDRATE_38400()		UBRR0L = 11; UBRR0H = 0
#define USART0_DRIVER_SET_BAUDRATE_115200()		UBRR0L = 3; UBRR0H = 0
#define USART0_DRIVER_SET_BAUDRATE_230400()		UBRR0L = 1; UBRR0H = 0

#define USART0_DRIVER_SET_DATABITS_5()			UCSR0C |= (0 << UCSZ00)
#define USART0_DRIVER_SET_DATABITS_6()			UCSR0C |= (1 << UCSZ00)
#define USART0_DRIVER_SET_DATABITS_7()			UCSR0C |= (2 << UCSZ00)
#define USART0_DRIVER_SET_DATABITS_8()			UCSR0C |= (3 << UCSZ00)
#define USART0_DRIVER_SET_DATABITS_9()			UCSR0C |= (7 << UCSZ00)

#define USART0_DRIVER_SET_STOPBITS_1()			UCSR0C |= (0 << USBS0)
#define USART0_DRIVER_SET_STOPBITS_2()			UCSR0C |= (1 << USBS0)

#define USART0_DRIVER_SET_PARITY_NONE()			UCSR0C |= (0 << UPM00)
#define USART0_DRIVER_SET_PARITY_EVEN()			UCSR0C |= (2 << UPM00)
#define USART0_DRIVER_SET_PARITY_ODD()			UCSR0C |= (3 << UPM00)

#define USART0_DRIVER_ENABLE_TX()			UCSR0B |= (1 << TXEN0)
#define USART0_DRIVER_ENABLE_RX()			UCSR0B |= (1 << RXEN0)

#define USART0_DRIVER_WAIT_UNTIL_TX_RDY()		while ( !( UCSR0A & (1<<UDRE0)) )

#define USART0_DRIVER_GET_BYTE()			UDR0
#define USART0_DRIVER_SET_BYTE(byte)			UDR0 = byte

/*-------------------------------------------------------------------------------------------------------------------------------------*/

BUILD_LOCAL_MSG_BUFFER(static inline, USART0_TX_BUFFER, USART0_DRIVER_MAX_NUM_BYTES_TRANSMIT_BUFFER)
BUILD_LOCAL_MSG_BUFFER(static inline, USART0_RX_BUFFER, USART0_DRIVER_MAX_NUM_BYTES_RECEIVE_BUFFER)

/*-------------------------------------------------------------------------------------------------------------------------------------*/

#define LOCAL_USART_STATUS_RX_ACTIVE		0
#define LOCAL_USART_STATUS_TX_ACTIVE		1

BUILD_MODULE_STATUS_FAST_VOLATILE(local_usart_status, 2)

/*-------------------------------------------------------------------------------------------------------------------------------------*/

static u16 remote_usart_rx_bytes = 0;

/*-------------------------------------------------------------------------------------------------------------------------------------*/

void usart0_driver_initialize(void) {

}

void usart0_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg) {

	(void) p_cfg;
	PASS();	// local_usart_driver_cfg()

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

	// Baudrate 115200
	//UBRR0L = 1;
	//UBRR0H = 0;

	/* Set frame format: 8data, 1stop bit */
	//UCSR0C = (0 << USBS0) | (3 << UCSZ00);

	/* Enable receiver and transmitter */
	//UCSR0B = (1 << RXEN0) | (1 << TXEN0);// | (1 << RXCIE0) | (1 << TXCIE0);
}

void usart0_driver_power_off(void) {
	PASS(); // local_usart_driver_power_off()
	USART0_RX_BUFFER_clear_all();
	USART0_TX_BUFFER_clear_all();
}

u16 usart0_driver_bytes_available (void) {

	#if defined TRACES_ENABLED && defined LOCAL_USART_RX_TRACES
	{
		u8 bytes_available = USART0_RX_BUFFER_bytes_available();
		if (bytes_available) {
			LOCAL_USART_RX_TRACE_byte(bytes_available); // i2c_driver_bytes_available()
		}
	}
	#endif

	return USART0_RX_BUFFER_bytes_available();
}



u16 usart0_driver_get_N_bytes (u16 num_bytes, u8* p_buffer_to) {

	PASS();	// local_usart_driver_get_N_bytes()

	u16 num_bytes_available = USART0_RX_BUFFER_bytes_available();

	if (num_bytes < num_bytes_available) {
		num_bytes_available = num_bytes;
	}

	USART0_RX_BUFFER_start_read();
	USART0_RX_BUFFER_get_N_bytes(num_bytes_available, p_buffer_to);
	USART0_RX_BUFFER_stop_read();

	LOCAL_USART_RX_TRACE_N(num_bytes_available, p_buffer_to); // local_usart_driver_bytes_available()

	return num_bytes_available;
}

u16 usart0_driver_set_N_bytes (u16 num_bytes, const u8* p_buffer_from) {
	if (num_bytes > USART0_TX_BUFFER_size()) {
		num_bytes = USART0_TX_BUFFER_size();
	}

	LOCAL_USART_TX_TRACE_N(num_bytes, p_buffer_from); // local_usart_driver_bytes_available()

	USART0_TX_BUFFER_start_write(); // this will delete all data added so far
	USART0_TX_BUFFER_add_N_bytes(num_bytes, p_buffer_from);
	USART0_TX_BUFFER_stop_write();

	return num_bytes;
}

u8 usart0_driver_is_ready_for_rx(void) {
	return 1;
}

void usart0_driver_start_rx (u16 num_of_rx_bytes) {

	LOCAL_USART_RX_TRACE_word(num_of_rx_bytes); // local_usart_driver_start_rx()

	remote_usart_rx_bytes = num_of_rx_bytes;

	USART0_RX_BUFFER_start_write();
	local_usart_status_set(LOCAL_USART_STATUS_RX_ACTIVE);
}

void usart0_driver_wait_for_rx(u16 num_bytes, u16 timeout_ms) {
	(void) num_bytes;
	(void) timeout_ms;
}

void usart0_driver_stop_rx (void) {

	LOCAL_USART_RX_PASS(); // local_usart_driver_stop_rx()

	local_usart_status_unset(LOCAL_USART_STATUS_RX_ACTIVE);
	USART0_RX_BUFFER_stop_write();
}

u8 usart0_driver_is_ready_for_tx (void) {

	return local_usart_status_is_set(LOCAL_USART_STATUS_TX_ACTIVE) == 0 ? 1 : 0;
}

void usart0_driver_start_tx (void) {

	LOCAL_USART_TX_PASS(); // local_usart_driver_start_tx()

	local_usart_status_set(LOCAL_USART_STATUS_TX_ACTIVE);
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

void usart0_driver_stop_tx (void) {

	LOCAL_USART_TX_PASS(); // local_usart_driver_stop_tx()

	USART0_TX_BUFFER_stop_read();
	local_usart_status_unset(LOCAL_USART_STATUS_TX_ACTIVE);
}

/*
 * forces the module to clear the receive buffer
 */
void usart0_driver_clear_rx_buffer(void) {
	USART0_RX_BUFFER_clear_all();
}

/*
 * forces the module to clear the transmit buffer
 */
void usart0_driver_clear_tx_buffer(void) {
	USART0_TX_BUFFER_clear_all();
}

void usart0_driver_set_address (u8 addr) {
	(void) addr;
}

u8 usart0_driver_mutex_request(void) {
	return 1;
}

void usart0_driver_mutex_release(u8 m_id) {
	(void) m_id;
}


ISR(USART0_TX_vect) {

	PASS(); // TX complete

	if (USART0_TX_BUFFER_bytes_available() > 0) {
		USART0_DRIVER_SET_BYTE(USART0_TX_BUFFER_get_byte());

	} else {
		usart0_driver_stop_tx();
	}
}


ISR(USART0_RX_vect) {

	PASS(); // RX complete

	if (remote_usart_rx_bytes != 0) {

		u8 byte = USART0_DRIVER_GET_BYTE();
		USART0_RX_BUFFER_add_byte(byte);

		if (remote_usart_rx_bytes != TRX_DRIVER_INTERFACE_UNLIMITED_RX_LENGTH) {

			if (remote_usart_rx_bytes-- == 0) {
				usart0_driver_stop_rx();
			}
		}
	}
}
