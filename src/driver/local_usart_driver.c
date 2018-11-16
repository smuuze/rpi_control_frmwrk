/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "utils/stdmacros.h"
#include RTOS_H

#include <avr/io.h>
#include <avr/interrupt.h>

#include "cfg_driver_interface.h"
#include "local_msg_buffer.h"
#include "local_usart_driver.h"
#include "local_module_status.h"


/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

///----- status codes -----

//----- General status codes -----
#define LOCAL_USART_STATE_WAKEUP					0x01	// -not used yet, since only master mode is implemented- status before the decision of master or slavemode is done
#define LOCAL_USART_STATE_MASTER_START_COMPLETE			0x08	// Start sent
#define LOCAL_USART_STATE_MASTER_REPEATED_START_COMPLETE		0x10	// Repeated Start sent
#define LOCAL_USART_STATE_MASTER_ARBITRATION_LOST			0x38	// Arbitration lost

//----- master transmitter mode (MTM) -----
#define LOCAL_USART_STATE_MT_SLAW_ACK_RECEIVED			0x18	// SLA+W sent, ACK received
#define LOCAL_USART_STATE_MT_SLAW_NACK_RECEIVED			0x20	// SLA+W sent, NACK received
#define LOCAL_USART_STATE_MT_DATA_ACK_RECEIVED			0x28	// DATA sent, ACK received
#define LOCAL_USART_STATE_MT_DATA_NACK_RECEIVED			0x30	// DATA sent, NACK received

//----- master receiver mode (MRM) -----
#define LOCAL_USART_STATE_MR_SLAR_ACK_RECEIVED			0x40	// SLA+R sent, ACK received
#define LOCAL_USART_STATE_MR_SLAR_NACK_RECEIVED			0x48	// SLA+R sent, NACK received
#define LOCAL_USART_STATE_MR_DATA_BYTE_ACK_RECEIVED		0x50	// DATA received, ACK received
#define LOCAL_USART_STATE_MR_DATA_BYTE_NACK_RECEIVED		0x58	// DATA received, NACK received

///----- !! status codes for slave modes are not implemented !! -----

//----- Miscellaneous States -----
#define LOCAL_BUS_ERROR		0x00 	// Illegal START or STOP condition has been detected
#define LOCAL_USART_NO_STATE	0xF8 	// No relevant information available
#define SUCCESSFUL_TRANSFER	0xFF

#define USART_DRIVER_RX_UNLIMITED	0xFFFF

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

//----- Bitsetting for mastercommunication -----
#define ENABLE_LOCAL_USART_INTERFACE()		TWCR = (1 << TWEN) | (1 << TWIE)	// enable interface and release i2c pins, disable interrupt, no signal request
#define LOCAL_USART_STARTSIGNAL()			TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE)	// send START or REPEATED START signal, enable I2C and interrupts, clear TWINT flag
#define LOCAL_USART_STOPSIGNAL()			TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN)		// send STOP signal, enable I2C and interrupts, clear TWINT flag
#define I2C_DRIVER_START_DATA_TRANSFER()		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE)			// MTM
#define LOCAL_USART_SEND_SLAR_ACK()		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1<<TWEA)	// MRM
#define LOCAL_USART_SEND_SLAR_NACK()		TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE)			// MRM

//---- diverse definitions -----

#define LOCAL_USART_STATEMASK	(TWSR & 0xF8)	// I2C status (masked)

#define DEFAULT_DATA_CONTENT()	(TWDR = 0xFF)
#define LOCAL_USART_R_W_BIT	0	// Bit position for R/W bit in "address byte"
#define LOCAL_USART_ADR_BITS	1	// Bit position for LSB of the slave address bits in the init byte

#define LOCAL_USART_DRIVER_OP_MODE_MASTER		1	// not used yet, since only master mode is implemented
#define LOCAL_USART_DRIVER_OP_MODE_SLAVE		2	// -"-

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

//---------- Implementations of Traces ----------
#define noTRACES
#include <traces.h>

#define noLOCAL_USART_RX_TRACES
#define noLOCAL_USART_TX_TRACES

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

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

//----- Bufferbuilding -----
BUILD_LOCAL_MSG_BUFFER(static inline, __local_usart_tx_buffer, LOCAL_USART_DRIVER_MAX_NUM_BYTES_TRANSMIT_BUFFER)	// build transmissionbuffer
BUILD_LOCAL_MSG_BUFFER(static inline, __local_usart_rx_buffer, LOCAL_USART_DRIVER_MAX_NUM_BYTES_RECEIVE_BUFFER)	// build receivingbuffer



//----- RX / TX activ status -----
BUILD_VOLATILE_MODULE_STATUS(u8, local_usart_status)

#define LOCAL_USART_STATUS_RX_ACTIVE		(1 << 0)
#define LOCAL_USART_STATUS_TX_ACTIVE		(1 << 1)

/*------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

/// Bitrate of I2C bus - needs to be looked over for correct setting before letting loose
#define f_cpu	7372800		// 14745600 without prescaling

#if defined f_cpu && f_cpu < 3600000
	#define LOCAL_USART_BITRATE()	(TWBR = 0x0A)			// if f_cpu < 3,6 MHz, f_i2c should be reduced too
#elif defined f_cpu && f_cpu == 7372800
		#define LOCAL_USART_BITRATE()	(TWBR = 0x1C)		// f_cpu = 14,7456 / 2	MHz = 7,3728 MHz ; f_i2c = 0,1 MHz
	#elif defined f_cpu && f_cpu == 14745600
		#define LOCAL_USART_BITRATE()	(TWBR = 0x0A)		// f_cpu = 14,7456 MHz ; f_i2c = 0,4 MHz
	#else
		#define	LOCAL_USART_BITRATE()	(TWBR = 0x0A)		// standard minimum setting if not otherwise modified
#endif

/*!-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

static u16 remote_usart_rx_bytes = 0;
/*
static u8 __local_usart_op_mode;
*/


void local_usart_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg) {

	(void) p_cfg;
	PASS();	// local_usart_driver_cfg()

	__local_usart_rx_buffer_init();
	__local_usart_tx_buffer_init();

	local_usart_driver_clear_buffer();

	// Baudrate 9600
	UBRR0L = 47;
	UBRR0H = 0;

	/* Enable receiver and transmitter */
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0) | (1 << TXCIE0);

	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);
}

void local_usart_driver_power_off(void) {
	PASS(); // local_usart_driver_power_off()
	__local_usart_rx_buffer_clear_all();
	__local_usart_tx_buffer_clear_all();
}

u8 local_usart_driver_bytes_available (void) {
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



u8 local_usart_driver_get_N_bytes (u8 num_bytes, u8* p_buffer_to) {

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

u8 local_usart_driver_set_N_bytes (u8 num_bytes, const u8* p_buffer_from) {
	if (num_bytes > __local_usart_tx_buffer_size()) {
		num_bytes = __local_usart_tx_buffer_size();
	}

	LOCAL_USART_TX_TRACE_N(num_bytes, p_buffer_from); // local_usart_driver_bytes_available()

	__local_usart_tx_buffer_start_write(); // this will delete all data added so far
	__local_usart_tx_buffer_add_N_bytes(num_bytes, p_buffer_from);
	__local_usart_tx_buffer_stop_write();

	return num_bytes;
}

u8 local_usart_driver_is_ready_for_tx (void) {

	return local_usart_status_is_set(LOCAL_USART_STATUS_TX_ACTIVE) == 0 ? 1 : 0;
}

void local_usart_driver_start_rx (u16 num_of_rx_bytes) {

	LOCAL_USART_RX_PASS(); // local_usart_driver_start_rx()

	remote_usart_rx_bytes = num_of_rx_bytes;

	__local_usart_rx_buffer_start_write();
	local_usart_status_set(LOCAL_USART_STATUS_RX_ACTIVE);

	LOCAL_USART_STARTSIGNAL(); PASS(); // START sent
}

void local_usart_driver_stop_rx (void) {

	LOCAL_USART_RX_PASS(); // local_usart_driver_stop_rx()

	local_usart_status_unset(LOCAL_USART_STATUS_RX_ACTIVE);
	__local_usart_rx_buffer_stop_write();
}

void local_usart_driver_start_tx (void) {

	LOCAL_USART_TX_PASS(); // local_usart_driver_start_tx()

	local_usart_status_set(LOCAL_USART_STATUS_TX_ACTIVE);
	__local_usart_tx_buffer_start_read();

	UDR0 = __local_usart_tx_buffer_get_byte();
}

void local_usart_driver_stop_tx (void) {

	LOCAL_USART_TX_PASS(); // local_usart_driver_stop_tx()

	__local_usart_tx_buffer_stop_read();
	local_usart_status_unset(LOCAL_USART_STATUS_TX_ACTIVE);
}

void local_usart_driver_clear_buffer (void) {

	PASS(); // local_usart_driver_clear_buffer()
	__local_usart_rx_buffer_clear_all();
	__local_usart_tx_buffer_clear_all();
}

void local_usart_driver_set_address (u8 addr) {
	(void) addr;
}


ISR(USART0_TX_vect) {

	PASS(); // TX complete

	if (__local_usart_tx_buffer_bytes_available() > 0) {
		UDR0 = __local_usart_tx_buffer_get_byte();

	} else {
		local_usart_driver_stop_tx();
	}
}


ISR(USART0_RX_vect) {

	PASS(); // RX complete

	if (remote_usart_rx_bytes != 0) {

		u8 byte = UDR0;
		__local_usart_rx_buffer_add_byte(byte);

		if (remote_usart_rx_bytes != TRX_DRIVER_INTERFACE_UNLIMITED_RX_LENGTH) {

			if (remote_usart_rx_bytes-- == 0) {
				local_usart_driver_stop_rx();
			}
		}
	}



}
