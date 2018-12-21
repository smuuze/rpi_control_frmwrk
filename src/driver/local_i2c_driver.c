/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "utils/stdmacros.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "cfg_driver_interface.h"
#include "local_msg_buffer.h"
#include "local_i2c_driver.h"
#include "local_module_status.h"
#include "local_mutex.h"

#define LOCAL_I2C_RX_TRACES
#define LOCAL_I2C_TX_TRACES

#if defined TRACES_ENABLED && defined LOCAL_I2C_RX_TRACES
#define LOCAL_I2C_RX_PASS()			PASS()
#define LOCAL_I2C_RX_TRACE_byte(v)		TRACE_byte(v)
#define LOCAL_I2C_RX_TRACE_word(v)		TRACE_word(v)
#define LOCAL_I2C_RX_TRACE_long(v)		TRACE_long(v)
#define LOCAL_I2C_RX_TRACE_N(n,v)		TRACE_N(n,v)
#else
#define LOCAL_I2C_RX_PASS()
#define LOCAL_I2C_RX_TRACE_byte(v)
#define LOCAL_I2C_RX_TRACE_word(v)
#define LOCAL_I2C_RX_TRACE_long(v)
#define LOCAL_I2C_RX_TRACE_N(n,v)
#endif


#if defined TRACES_ENABLED && defined LOCAL_I2C_TX_TRACES
#define LOCAL_I2C_TX_PASS()			PASS()
#define LOCAL_I2C_TX_TRACE_byte(v)		TRACE_byte(v)
#define LOCAL_I2C_TX_TRACE_word(v)		TRACE_word(v)
#define LOCAL_I2C_TX_TRACE_long(v)		TRACE_long(v)
#define LOCAL_I2C_TX_TRACE_N(n,v)		TRACE_N(n,v)
#else
#define LOCAL_I2C_TX_PASS()
#define LOCAL_I2C_TX_TRACE_byte(v)
#define LOCAL_I2C_TX_TRACE_word(v)
#define LOCAL_I2C_TX_TRACE_long(v)
#define LOCAL_I2C_TX_TRACE_N(n,v)
#endif


#define noTRACES
#include <traces.h>


//----- General status codes -----
#define LOCAL_I2C_STATE_WAKEUP					0x01	// -not used yet, since only master mode is implemented- status before the decision of master or slavemode is done
#define LOCAL_I2C_STATE_MASTER_START_COMPLETE			0x08	// Start sent
#define LOCAL_I2C_STATE_MASTER_REPEATED_START_COMPLETE		0x10	// Repeated Start sent
#define LOCAL_I2C_STATE_MASTER_ARBITRATION_LOST			0x38	// Arbitration lost

//----- master transmitter mode (MTM) -----
#define LOCAL_I2C_STATE_MT_SLAW_ACK_RECEIVED			0x18	// SLA+W sent, ACK received
#define LOCAL_I2C_STATE_MT_SLAW_NACK_RECEIVED			0x20	// SLA+W sent, NACK received
#define LOCAL_I2C_STATE_MT_DATA_ACK_RECEIVED			0x28	// DATA sent, ACK received
#define LOCAL_I2C_STATE_MT_DATA_NACK_RECEIVED			0x30	// DATA sent, NACK received

//----- master receiver mode (MRM) -----
#define LOCAL_I2C_STATE_MR_SLAR_ACK_RECEIVED			0x40	// SLA+R sent, ACK received
#define LOCAL_I2C_STATE_MR_SLAR_NACK_RECEIVED			0x48	// SLA+R sent, NACK received
#define LOCAL_I2C_STATE_MR_DATA_BYTE_ACK_RECEIVED		0x50	// DATA received, ACK received
#define LOCAL_I2C_STATE_MR_DATA_BYTE_NACK_RECEIVED		0x58	// DATA received, NACK received

///----- !! status codes for slave modes are not implemented !! -----

//----- Miscellaneous States -----
#define LOCAL_BUS_ERROR		0x00 	// Illegal START or STOP condition has been detected
#define LOCAL_I2C_NO_STATE	0xF8 	// No relevant information available
#define SUCCESSFUL_TRANSFER	0xFF

//----- CONTROL WORDS ---------------

#define I2C_DRIVER_SEND_START_CONDITION()		TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE)

#define I2C_DRIVER_SEND_SLAVE_ADDRESS(addr)		TWDR = addr;	\
							TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE)

#define I2C_DRIVER_SEND_DATA_BYTE(byte)			TWDR = byte;	\
							TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE)

#define I2C_DRIVER_SEND_STOP_CONDITION()		TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN) | (1 << TWIE)

#define I2C_DRIVER_SEND_ACK()				TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE) | (1<<TWEA)

#define I2C_DRIVER_SEND_NACK()				TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWIE)

#define I2C_DRIVER_SET_BITARTE(bitrate)			TWBR = bitrate

#define I2C_DRIVER_SET_BITRATE_PRESCALER(scaler)	TWSR |= (scaler & 0x03)

#define I2C_DRIVER_GET_MODULE_STATUS()			(TWSR & 0xF8)

#define I2C_DRIVER_SET_DATA_REGISTER(byte)		TWDR = byte

#define I2C_DRIVER_GET_DATA_REGISTER()			TWDR

#define I2C_DRIVER_DISABLE_INTERFACE()			I2C0_SCL_as_INPUT(); I2C0_SCL_OFF();	\
							I2C0_SDA_as_INPUT(); I2C0_SDA_OFF()

#define I2C_DRIVER_ENABLE_INTERFACE()			I2C0_SCL_as_OUTPUT(); I2C0_SCL_ON();	\
							I2C0_SDA_as_OUTPUT(); I2C0_SDA_OFF()

//----- BITRATE SETTINGS ---------------

/*
 * TWBR = 0.5 * (f_cpu / f_scl - 16) * 1 / (4^TWPS)
 */

#define I2C_DRIVER_BITRATE_1KHZ_PRESCALER_64		57
#define I2C_DRIVER_BITRATE_10KHZ_PRESCALER_1		90
#define I2C_DRIVER_BITRATE_100KHZ

//----- BITRATE PRESCALER ---------------

#define I2C_DRIVER_BITRATE_PRESCALER_1			0x00
#define I2C_DRIVER_BITRATE_PRESCALER_4			0x01
#define I2C_DRIVER_BITRATE_PRESCALER_16			0x02
#define I2C_DRIVER_BITRATE_PRESCALER_64			0x03

//----- Bufferbuilding -----
BUILD_LOCAL_MSG_BUFFER(, __local_i2c_tx_buffer, LOCAL_I2C_DRIVER_MAX_NUM_BYTES_TRANSMIT_BUFFER)	// build transmissionbuffer
BUILD_LOCAL_MSG_BUFFER(, __local_i2c_rx_buffer, LOCAL_I2C_DRIVER_MAX_NUM_BYTES_RECEIVE_BUFFER)	// build receivingbuffer


BUILD_MUTEX(i2c_mutex)

//----- RX / TX activ status -----
BUILD_MODULE_STATUS_FAST_VOLATILE(i2c_driver_status, 2)

#define LOCAL_I2C_STATUS_RX_ACTIVE		0
#define LOCAL_I2C_STATUS_TX_ACTIVE		1


/// Bitrate of I2C bus - needs to be looked over for correct setting before letting loose
#define f_cpu	7372800		// 14745600 without prescaling

#if defined f_cpu && f_cpu < 3600000
	#define LOCAL_I2C_BITRATE()	(TWBR = 0x0A)			// if f_cpu < 3,6 MHz, f_i2c should be reduced too
#elif defined f_cpu && f_cpu == 7372800
		#define LOCAL_I2C_BITRATE()	(TWBR = 0x1C)		// f_cpu = 14,7456 / 2	MHz = 7,3728 MHz ; f_i2c = 0,1 MHz
	#elif defined f_cpu && f_cpu == 14745600
		#define LOCAL_I2C_BITRATE()	(TWBR = 0x0A)		// f_cpu = 14,7456 MHz ; f_i2c = 0,4 MHz
	#else
		#define	LOCAL_I2C_BITRATE()	(TWBR = 0x0A)		// standard minimum setting if not otherwise modified
#endif

/*!-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

static u8 _i2c_slave_address = 0;
static u8 _i2c_rx_counter = 0;

void i2c_driver_initialize(void) {

	PASS();	// i2c_driver_initialize()

	__local_i2c_rx_buffer_init();
	__local_i2c_tx_buffer_init();
}

void i2c_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg) {

	(void) p_cfg;

	PASS();	// local_i2c_driver_cfg()

	I2C_DRIVER_ENABLE_INTERFACE();

  	I2C_DRIVER_SET_BITARTE(I2C_DRIVER_BITRATE_1KHZ_PRESCALER_64);
	I2C_DRIVER_SET_BITRATE_PRESCALER(I2C_DRIVER_BITRATE_PRESCALER_64);
	//TWBR = 0x05;
}

void i2c_driver_power_off(void) {
	PASS(); // local_i2c_driver_power_off()

	I2C_DISABLE_INTERFACE();
}

u8 i2c_driver_bytes_available (void) {

	#if defined TRACES_ENABLED && defined LOCAL_I2C_RX_TRACES
	{
		u8 bytes_available = __local_i2c_rx_buffer_bytes_available();
		if (bytes_available) {
			LOCAL_I2C_RX_TRACE_byte(bytes_available); // i2c_driver_bytes_available()
		}
	}
	#endif

	return __local_i2c_rx_buffer_bytes_available();
}

u8 i2c_driver_get_N_bytes (u8 num_bytes, u8* p_buffer_to) {

	PASS();	// local_i2c_driver_get_N_bytes()

	u8 num_bytes_available = __local_i2c_rx_buffer_bytes_available();
	if (num_bytes < num_bytes_available) {
		num_bytes_available = num_bytes;
	}

	__local_i2c_rx_buffer_start_read();
	__local_i2c_rx_buffer_get_N_bytes(num_bytes_available, p_buffer_to);
	__local_i2c_rx_buffer_stop_read();

	return num_bytes_available;
}

u8 i2c_driver_set_N_bytes (u8 num_bytes, const u8* p_buffer_from) {

	if (num_bytes > __local_i2c_tx_buffer_size()) {
		num_bytes = __local_i2c_tx_buffer_size();
	}

	TRACE_N(num_bytes, p_buffer_from); // local_i2c_driver_set_N_bytes()

	__local_i2c_tx_buffer_start_write(); // this will delete all data added so far
	__local_i2c_tx_buffer_add_N_bytes(num_bytes, p_buffer_from);
	__local_i2c_tx_buffer_stop_write();

	return num_bytes;
}

u8 i2c_driver_is_ready_for_tx (void) {

	if (i2c_driver_status_is_set(LOCAL_I2C_STATUS_TX_ACTIVE) != 0) {
		return 0;

	} else if (i2c_driver_status_is_set(LOCAL_I2C_STATUS_RX_ACTIVE) != 0) {
		return 0;

	} else {
		return 1;
	}
}

u8 i2c_driver_is_ready_for_rx(void) {

	if (i2c_driver_status_is_set(LOCAL_I2C_STATUS_TX_ACTIVE) != 0) {
		return 0;

	} else if (i2c_driver_status_is_set(LOCAL_I2C_STATUS_RX_ACTIVE) != 0) {
		return 0;

	} else {
		return 1;
	}
}

void i2c_driver_start_rx (u16 num_of_rx_bytes) {

	TRACE_byte(num_of_rx_bytes); // local_i2c_driver_start_rx()

	_i2c_slave_address = _i2c_slave_address | (0x01);
	_i2c_rx_counter = num_of_rx_bytes;

	__local_i2c_rx_buffer_start_write();
	i2c_driver_status_set(LOCAL_I2C_STATUS_RX_ACTIVE);

	I2C_DRIVER_SEND_START_CONDITION();
}

void i2c_driver_stop_rx (void) {

	PASS(); // local_i2c_driver_stop_rx()

	// TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN)
	I2C_DRIVER_SEND_STOP_CONDITION();

	i2c_driver_status_unset(LOCAL_I2C_STATUS_RX_ACTIVE);
	__local_i2c_rx_buffer_stop_write();
}

void i2c_driver_start_tx (void) {

	PASS(); // local_i2c_driver_start_tx()

	// for TX LSB needs to be '0'
	_i2c_slave_address = _i2c_slave_address & ~(0x01);

	i2c_driver_status_set(LOCAL_I2C_STATUS_TX_ACTIVE);
	__local_i2c_tx_buffer_start_read();

	I2C_DRIVER_SEND_START_CONDITION();
}

void i2c_driver_stop_tx (void) {

	LOCAL_I2C_TX_PASS(); // local_i2c_driver_stop_tx()

	I2C_DRIVER_SEND_STOP_CONDITION();

	__local_i2c_tx_buffer_stop_read();
	i2c_driver_status_unset(LOCAL_I2C_STATUS_TX_ACTIVE);
}

void i2c_driver_clear_buffer (void) {

	__local_i2c_rx_buffer_clear_all();
	__local_i2c_tx_buffer_clear_all();
}

void i2c_driver_set_address (u8 addr) {

	TRACE_byte(addr); // local_i2c_driver_set_address()
	_i2c_slave_address = addr << 1;
}

u8 i2c_driver_mutex_request(void) {
	if (i2c_mutex_is_requested() != 0) {
		return MUTEX_INVALID_ID;
	}

	return i2c_mutex_request();
}

void i2c_driver_mutex_release(u8 m_id) {
	i2c_mutex_release(m_id);
}

ISR(TWI_vect) {

	u8 i2c_module_status = I2C_DRIVER_GET_MODULE_STATUS();
	TRACE_byte(i2c_module_status); // ------ ISR(TWI_vect) -----

	u8 data_byte = 0;

	switch (i2c_module_status) {

		case LOCAL_I2C_STATE_MASTER_START_COMPLETE:
		case LOCAL_I2C_STATE_MASTER_REPEATED_START_COMPLETE:

			I2C_DRIVER_SEND_SLAVE_ADDRESS(_i2c_slave_address);

			break;

		case LOCAL_I2C_STATE_MT_SLAW_ACK_RECEIVED:
		case LOCAL_I2C_STATE_MT_DATA_ACK_RECEIVED:

			if (__local_i2c_tx_buffer_bytes_available() != 0) {

				data_byte = __local_i2c_tx_buffer_get_byte();
				I2C_DRIVER_SEND_DATA_BYTE(data_byte);

				TRACE_byte(data_byte); // Sending next Data-Byte

			} else {

				I2C_DRIVER_SEND_STOP_CONDITION();
				i2c_driver_status_unset(LOCAL_I2C_STATUS_TX_ACTIVE);

				PASS(); // No more Data to send - sending stop-condition
			}

			break;

		case LOCAL_I2C_STATE_MR_DATA_BYTE_ACK_RECEIVED:

			data_byte = I2C_DRIVER_GET_DATA_REGISTER();
			__local_i2c_rx_buffer_add_byte(data_byte); // write DATA into buffer

			TRACE_byte(data_byte); // Data-Byte received
			// no break

		case LOCAL_I2C_STATE_MR_SLAR_ACK_RECEIVED:

			if (_i2c_rx_counter > 0) {

				I2C_DRIVER_SEND_ACK();

				_i2c_rx_counter--;
				TRACE_byte(_i2c_rx_counter); // More bytes to receive - sending ack

			} else {

//				I2C_DRIVER_SEND_STOP_CONDITION();
				I2C_DRIVER_SEND_NACK();
				i2c_driver_status_unset(LOCAL_I2C_STATUS_RX_ACTIVE);
				PASS(); // No more bytes to receive - sending nack
			}

			break;

		case LOCAL_I2C_STATE_MASTER_ARBITRATION_LOST:

			PASS(); // LOCAL_I2C_STATE_MASTER_ARBITRATION_LOST - not implemented yet
			break;

		case LOCAL_BUS_ERROR:

			PASS(); // LOCAL_BUS_ERROR - not implemented yet
			break;

		case LOCAL_I2C_STATE_MR_DATA_BYTE_NACK_RECEIVED:

			PASS(); // NACK-Received - Reading Databyte from Module
			data_byte = I2C_DRIVER_GET_DATA_REGISTER();
			// no break

		case LOCAL_I2C_STATE_MR_SLAR_NACK_RECEIVED:
		case LOCAL_I2C_STATE_MT_SLAW_NACK_RECEIVED:
		case LOCAL_I2C_STATE_MT_DATA_NACK_RECEIVED:

			PASS(); // NACK received - Sending Stop-Condition

			I2C_DRIVER_SEND_STOP_CONDITION();
			i2c_driver_status_unset(LOCAL_I2C_STATUS_TX_ACTIVE);

			break;
	}
}
