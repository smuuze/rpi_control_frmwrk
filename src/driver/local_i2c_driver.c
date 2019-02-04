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
#include "io_controller.h"

#include "driver_specific_i2c.h"

#define noI2C_RX_TRACES
#define noI2C_TX_TRACES
#define noI2C_ISR_TRACES

#if defined TRACES_ENABLED && defined I2C_RX_TRACES
#define I2C_RX_PASS()			PASS()
#define I2C_RX_TRACE_byte(v)		TRACE_byte(v)
#define I2C_RX_TRACE_word(v)		TRACE_word(v)
#define I2C_RX_TRACE_long(v)		TRACE_long(v)
#define I2C_RX_TRACE_N(n,v)		TRACE_N(n,v)
#else
#define I2C_RX_PASS()
#define I2C_RX_TRACE_byte(v)
#define I2C_RX_TRACE_word(v)
#define I2C_RX_TRACE_long(v)
#define I2C_RX_TRACE_N(n,v)
#endif


#if defined TRACES_ENABLED && defined I2C_TX_TRACES
#define I2C_TX_PASS()			PASS()
#define I2C_TX_TRACE_byte(v)		TRACE_byte(v)
#define I2C_TX_TRACE_word(v)		TRACE_word(v)
#define I2C_TX_TRACE_long(v)		TRACE_long(v)
#define I2C_TX_TRACE_N(n,v)		TRACE_N(n,v)
#else
#define I2C_TX_PASS()
#define I2C_TX_TRACE_byte(v)
#define I2C_TX_TRACE_word(v)
#define I2C_TX_TRACE_long(v)
#define I2C_TX_TRACE_N(n,v)
#endif


#if defined TRACES_ENABLED && defined I2C_ISR_TRACES
#define I2C_ISR_PASS()			PASS()
#define I2C_ISR_TRACE_byte(v)		TRACE_byte(v)
#define I2C_ISR_TRACE_word(v)		TRACE_word(v)
#define I2C_ISR_TRACE_long(v)		TRACE_long(v)
#define I2C_ISR_TRACE_N(n,v)		TRACE_N(n,v)
#else
#define I2C_ISR_PASS()
#define I2C_ISR_TRACE_byte(v)
#define I2C_ISR_TRACE_word(v)
#define I2C_ISR_TRACE_long(v)
#define I2C_ISR_TRACE_N(n,v)
#endif

#define noTRACES
#include <traces.h>

//----- Bufferbuilding -----
BUILD_LOCAL_MSG_BUFFER(, i2c_tx_buffer, I2C_DRIVER_MAX_NUM_BYTES_TRANSMIT_BUFFER)	// build transmissionbuffer
BUILD_LOCAL_MSG_BUFFER(, i2c_rx_buffer, I2C_DRIVER_MAX_NUM_BYTES_RECEIVE_BUFFER)	// build receivingbuffer


BUILD_MUTEX(i2c_mutex)

//----- RX / TX activ status -----
BUILD_MODULE_STATUS_FAST_VOLATILE(i2c_driver_status, 2)

IO_CONTROLLER_BUILD_INOUT(MASTER_I2C_SCL, I2C_SCL)
IO_CONTROLLER_BUILD_INOUT(MASTER_I2C_SDA, I2C_SDA)

I2C_BUILD_CFG()

#define I2C_STATUS_RX_ACTIVE		0
#define I2C_STATUS_TX_ACTIVE		1

/*!-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/

static u8 _i2c_slave_address = 0;
static u8 _i2c_rx_counter = 0;

void i2c_driver_initialize(void) {

	PASS();	// i2c_driver_initialize()

	i2c_rx_buffer_init();
	i2c_tx_buffer_init();

	MASTER_I2C_SCL_init();
	MASTER_I2C_SDA_init();
}

void i2c_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg) {

	PASS();	// local_i2c_driver_cfg()

	I2C_POWER_UP();

	if (p_cfg->module.i2c.is_master != 0) {

		I2C_ENABLE_MASTER_MODE(); PASS(); //

		MASTER_I2C_SCL_drive_high();
		MASTER_I2C_SDA_drive_high();

		I2C_DELETE_SLAVE_ADDRESS();
		I2C_DISABLE_ANSWER_TO_GENERAL_CALL();

	} else {

		I2C_DISABLE_MASTER_MODE(); PASS(); //

		MASTER_I2C_SCL_pull_up();
		MASTER_I2C_SDA_pull_up();

		TRACE_byte(p_cfg->module.i2c.slave_addr); // i2c_driver_configure
		I2C_SET_SLAVE_ADDRESS(p_cfg->module.i2c.slave_addr);
	}

	if (p_cfg->module.i2c.answer_general_call) {
		I2C_ENABLE_ANSWER_TO_GENERAL_CALL(); PASS(); //
	} else {
		I2C_DISABLE_ANSWER_TO_GENERAL_CALL(); PASS(); //
	}

	if (p_cfg->module.i2c.enable_ack) {
		I2C_ENABLE_ACK_RESPONSE(); PASS(); //
	} else {
		I2C_DISABLE_ACK_RESPONSE(); PASS(); //
	}

	if (p_cfg->module.i2c.interrupt_enable) {
		I2C_ENABLE_IRQ(); PASS(); //
	} else {
		I2C_DISABLE_IRQ(); PASS(); //
	}

	switch (p_cfg->module.i2c.bit_rate) {
		default:
		case DRIVER_I2C_BITRATE_1KHZ 	:
			I2C_BITRATE_1_KHZ(); PASS(); //
			break;

		case DRIVER_I2C_BITRATE_10KHZ 	:
			I2C_BITRATE_10_KHZ(); PASS(); //
			break;

		case DRIVER_I2C_BITRATE_100KHZ 	:
			I2C_BITRATE_100_KHZ(); PASS(); //
			break;

		case DRIVER_I2C_BITRATE_1MHZ 	:
			I2C_BITRATE_1_MHZ(); PASS(); //
			break;
	}

	I2C_ENABLE_MODULE();

	I2C_SET_PRESCALER(I2C_DRIVER_BITRATE_PRESCALER_64);
  	I2C_SET_BITRATE(I2C_DRIVER_BITRATE_1KHZ_PRESCALER_64);

	{
		u8 temp = TWCR;
		TRACE_byte(temp); // SPI - CONTROL-REGISTER

		temp = TWSR;
		TRACE_byte(temp); // SPI - STATUS-REGISTER

		temp = PRR0;
		TRACE_byte(temp); // SPI - PRR0-REGISTER

		temp = PRR1;
		TRACE_byte(temp); // SPI - PRR1-REGISTER

		temp = SREG;
		TRACE_byte(temp); // Global Status-Register
	}
}

void i2c_driver_power_off(void) {

	PASS(); // local_i2c_driver_power_off()

	MASTER_I2C_SCL_no_pull();
	MASTER_I2C_SDA_no_pull();

	I2C_DISABLE_MODULE();
	I2C_POWER_DOWN();
}

u8 i2c_driver_bytes_available (void) {

	#if defined TRACES_ENABLED && defined I2C_RX_TRACES
	{
		u8 bytes_available = i2c_rx_buffer_bytes_available();
		if (bytes_available) {
			I2C_RX_TRACE_byte(bytes_available); // i2c_driver_bytes_available()
		}
	}
	#endif

	return i2c_rx_buffer_bytes_available();
}

u8 i2c_driver_get_N_bytes (u8 num_bytes, u8* p_buffer_to) {

	PASS();	// local_i2c_driver_get_N_bytes()

	u8 num_bytes_available = i2c_rx_buffer_bytes_available();
	if (num_bytes < num_bytes_available) {
		num_bytes_available = num_bytes;
	}

	i2c_rx_buffer_start_read();
	i2c_rx_buffer_get_N_bytes(num_bytes_available, p_buffer_to);
	i2c_rx_buffer_stop_read();

	return num_bytes_available;
}

u8 i2c_driver_set_N_bytes (u8 num_bytes, const u8* p_buffer_from) {

	if (num_bytes > i2c_tx_buffer_size()) {
		num_bytes = i2c_tx_buffer_size();
	}

	TRACE_N(num_bytes, p_buffer_from); // local_i2c_driver_set_N_bytes()

	i2c_tx_buffer_start_write(); // this will delete all data added so far
	i2c_tx_buffer_add_N_bytes(num_bytes, p_buffer_from);
	i2c_tx_buffer_stop_write();

	return num_bytes;
}

u8 i2c_driver_is_ready_for_tx (void) {

	if (i2c_driver_status_is_set(I2C_STATUS_TX_ACTIVE) != 0) {
		return 0;

	} else if (i2c_driver_status_is_set(I2C_STATUS_RX_ACTIVE) != 0) {
		return 0;

	} else {
		return 1;
	}
}

u8 i2c_driver_is_ready_for_rx(void) {

	if (i2c_driver_status_is_set(I2C_STATUS_TX_ACTIVE) != 0) {
		return 0;

	} else if (i2c_driver_status_is_set(I2C_STATUS_RX_ACTIVE) != 0) {
		return 0;

	} else {
		return 1;
	}
}

void i2c_driver_start_rx (u16 num_of_rx_bytes) {

	TRACE_byte(num_of_rx_bytes); // local_i2c_driver_start_rx()

	_i2c_slave_address = _i2c_slave_address | (0x01);
	_i2c_rx_counter = num_of_rx_bytes;

	i2c_rx_buffer_start_write();
	i2c_driver_status_set(I2C_STATUS_RX_ACTIVE);

	I2C_DRIVER_SEND_START_CONDITION();
}

void i2c_driver_wait_for_rx(u8 num_bytes, u16 timeout_ms) {
	(void) num_bytes;
	(void) timeout_ms;
}

void i2c_driver_stop_rx (void) {

	PASS(); // local_i2c_driver_stop_rx()

	// TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN)
	I2C_DRIVER_SEND_STOP_CONDITION();

	i2c_driver_status_unset(I2C_STATUS_RX_ACTIVE);
	i2c_rx_buffer_stop_write();
}

void i2c_driver_start_tx (void) {

	PASS(); // local_i2c_driver_start_tx()

	// for TX LSB needs to be '0'
	_i2c_slave_address = _i2c_slave_address & ~(0x01);

	i2c_driver_status_set(I2C_STATUS_TX_ACTIVE);
	i2c_tx_buffer_start_read();

	I2C_DRIVER_SEND_START_CONDITION();
}

void i2c_driver_wait_for_tx(u8 num_bytes, u16 timeout_ms) {
	(void) num_bytes;
	(void) timeout_ms;
}

void i2c_driver_stop_tx (void) {

	I2C_TX_PASS(); // local_i2c_driver_stop_tx()

	I2C_DRIVER_SEND_STOP_CONDITION();

	i2c_tx_buffer_stop_read();
	i2c_driver_status_unset(I2C_STATUS_TX_ACTIVE);
}

void i2c_driver_clear_buffer (void) {

	i2c_rx_buffer_clear_all();
	i2c_tx_buffer_clear_all();
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
	I2C_ISR_TRACE_byte(i2c_module_status); // ------ ISR(TWI_vect) -----

	u8 data_byte = 0;

	switch (i2c_module_status) {

		case I2C_STATE_MASTER_START_COMPLETE:
		case I2C_STATE_MASTER_REPEATED_START_COMPLETE:

			I2C_DRIVER_SEND_SLAVE_ADDRESS(_i2c_slave_address);

			break;

		case I2C_STATE_MT_SLAW_ACK_RECEIVED:
		case I2C_STATE_MT_DATA_ACK_RECEIVED:

			if (i2c_tx_buffer_bytes_available() != 0) {

				data_byte = i2c_tx_buffer_get_byte();
				I2C_DRIVER_SEND_DATA_BYTE(data_byte);

				I2C_ISR_TRACE_byte(data_byte); // Sending next Data-Byte

			} else {

				I2C_DRIVER_SEND_STOP_CONDITION();
				i2c_driver_status_unset(I2C_STATUS_TX_ACTIVE);

				I2C_ISR_PASS(); // No more Data to send - sending stop-condition
			}

			break;

		case I2C_STATE_MR_DATA_BYTE_ACK_RECEIVED:

			data_byte = I2C_DRIVER_GET_DATA_REGISTER();
			i2c_rx_buffer_add_byte(data_byte); // write DATA into buffer

			I2C_ISR_TRACE_byte(data_byte); // Data-Byte received
			// no break

		case I2C_STATE_MR_SLAR_ACK_RECEIVED:

			if (_i2c_rx_counter > 0) {

				I2C_DRIVER_SEND_ACK();

				_i2c_rx_counter--;
				I2C_ISR_TRACE_byte(_i2c_rx_counter); // More bytes to receive - sending ack

			} else {

//				I2C_DRIVER_SEND_STOP_CONDITION();
				I2C_DRIVER_SEND_NACK();
				i2c_driver_status_unset(I2C_STATUS_RX_ACTIVE);
				I2C_ISR_PASS(); // No more bytes to receive - sending nack
			}

			break;

		case I2C_STATE_MASTER_ARBITRATION_LOST:

			I2C_ISR_PASS(); // I2C_STATE_MASTER_ARBITRATION_LOST - not implemented yet
			break;

		case LOCAL_BUS_ERROR:

			I2C_ISR_PASS(); // LOCAL_BUS_ERROR - not implemented yet
			break;

		case I2C_STATE_MR_DATA_BYTE_NACK_RECEIVED:

			I2C_ISR_PASS(); // NACK-Received - Reading Databyte from Module
			data_byte = I2C_DRIVER_GET_DATA_REGISTER();
			// no break

		case I2C_STATE_MR_SLAR_NACK_RECEIVED:
		case I2C_STATE_MT_SLAW_NACK_RECEIVED:
		case I2C_STATE_MT_DATA_NACK_RECEIVED:

			I2C_ISR_PASS(); // NACK received - Sending Stop-Condition

			I2C_DRIVER_SEND_STOP_CONDITION();
			i2c_driver_status_unset(I2C_STATUS_TX_ACTIVE);

			break;
	}
}
