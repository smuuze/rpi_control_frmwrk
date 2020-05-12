/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	driver/communication/i2c/i2c0_driver_atmega1284p.c
 * \author	sebastian lesse
 * \brief
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "cfg_driver_interface.h"
#include "local_msg_buffer.h"
#include "local_module_status.h"
#include "local_mutex.h"

#include "driver/communication/i2c/i2c0_driver.h"
#include "driver/communication/i2c/i2c0_driver_status.h"
#include "driver/communication/i2c/i2c0_driver_atmega1284p.h"

// --------------------------------------------------------------------------------

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

// --------------------------------------------------------------------------------

BUILD_LOCAL_MSG_BUFFER(, I2C0_TX_BUFFER, I2C_DRIVER_MAX_NUM_BYTES_TRANSMIT_BUFFER)	// build transmissionbuffer
BUILD_LOCAL_MSG_BUFFER(, I2C0_RX_BUFFER, I2C_DRIVER_MAX_NUM_BYTES_RECEIVE_BUFFER)	// build receivingbuffer

// --------------------------------------------------------------------------------

//BUILD_MODULE_STATUS_FAST_VOLATILE(I2C0_STATUS, 2)

// --------------------------------------------------------------------------------

BUILD_MUTEX(i2c_mutex)

// --------------------------------------------------------------------------------

I2C_BUILD_CFG()

// --------------------------------------------------------------------------------

static u8 i2c0_driver_destination_addr = 0;
static u8 _i2c_rx_counter = 0;

// --------------------------------------------------------------------------------

void i2c0_driver_initialize(void) {

	DEBUG_PASS("i2c0_driver_initialize()");

	I2C0_RX_BUFFER_init();
	I2C0_TX_BUFFER_init();
}

void i2c0_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg) {

	DEBUG_PASS("i2c0_driver_configure()");

	I2C_POWER_UP();

	if (p_cfg->module.i2c.is_master != 0) {

		DEBUG_PASS("i2c0_driver_configure() - MASTER");

		I2C_ENABLE_MASTER_MODE();
		I2C0_STATUS_set(I2C_STATUS_IS_MASTER);

		I2C_SCL_drive_high();
		I2C_SDA_drive_high();

		I2C_DELETE_SLAVE_ADDRESS();
		I2C_DISABLE_ANSWER_TO_GENERAL_CALL();

	} else {

		DEBUG_PASS("i2c0_driver_configure() - SLAVE");

		I2C_DISABLE_MASTER_MODE();
		I2C0_STATUS_unset(I2C_STATUS_IS_MASTER);

		I2C_SCL_pull_up();
		I2C_SDA_pull_up();

		DEBUG_TRACE_byte(p_cfg->module.i2c.slave_addr, "i2c0_driver_configure() - slave-address");
		I2C_SET_SLAVE_ADDRESS(p_cfg->module.i2c.slave_addr);

		// Ack-Response must always be enabled in slave-mode
		p_cfg->module.i2c.enable_ack = 1;
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
			DEBUG_PASS("i2c0_driver_configure() - DRIVER_I2C_BITRATE_1KHZ");
			I2C_BITRATE_1_KHZ();
			break;

		case DRIVER_I2C_BITRATE_10KHZ 	:
			DEBUG_PASS("i2c0_driver_configure() - DRIVER_I2C_BITRATE_10KHZ");
			I2C_BITRATE_10_KHZ();
			break;

		case DRIVER_I2C_BITRATE_100KHZ 	:
			DEBUG_PASS("i2c0_driver_configure() - DRIVER_I2C_BITRATE_100KHZ");
			I2C_BITRATE_100_KHZ();
			break;

		case DRIVER_I2C_BITRATE_1MHZ 	:
			DEBUG_PASS("i2c0_driver_configure() - DRIVER_I2C_BITRATE_1MHZ");
			I2C_BITRATE_1_MHZ();
			break;
	}

	I2C_ENABLE_MODULE();

	I2C_SET_PRESCALER(I2C_DRIVER_BITRATE_PRESCALER_64);
  	I2C_SET_BITRATE(I2C_DRIVER_BITRATE_1KHZ_PRESCALER_64);

	{
		u8 temp = TWCR;
		DEBUG_TRACE_byte(temp, "i2c0_driver_configure() - Control-Register - TWCR:");

		temp = TWAR;
		DEBUG_TRACE_byte(temp, "i2c0_driver_configure() - Adress-Register - TWAR:");

		temp = TWBR;
		DEBUG_TRACE_byte(temp, "i2c0_driver_configure() - Baudrate-Register - TWBR:");

		temp = TWSR;
		DEBUG_TRACE_byte(temp, "i2c0_driver_configure() - Status-Register - TWSR:");

		temp = i2c_cfg_control_reg;
		DEBUG_TRACE_byte(temp, "i2c0_driver_configure() - Status-Register - i2c_cfg_control_reg:");
	}
}

void i2c0_driver_power_off(void) {

	DEBUG_PASS("i2c0_driver_power_off()");

	I2C_SCL_no_pull();
	I2C_SDA_no_pull();

	I2C_DISABLE_MODULE();
	I2C_POWER_DOWN();
}

u8 i2c0_driver_bytes_available (void) {

	#if defined TRACER_ENABLED
	{
		u8 bytes_available = I2C0_RX_BUFFER_bytes_available();
		if (bytes_available) {
			I2C_RX_TRACE_byte(bytes_available); // i2c_driver_bytes_available()
		}
	}
	#endif

	return I2C0_RX_BUFFER_bytes_available();
}

u8 i2c0_driver_get_N_bytes (u8 num_bytes, u8* p_buffer_to) {

	DEBUG_PASS("local_i2c_driver_get_N_bytes()");

	u8 num_bytes_available = I2C0_RX_BUFFER_bytes_available();
	if (num_bytes < num_bytes_available) {
		num_bytes_available = num_bytes;
	}

	I2C0_RX_BUFFER_start_read();
	I2C0_RX_BUFFER_get_N_bytes(num_bytes_available, p_buffer_to);
	I2C0_RX_BUFFER_stop_read();

	return num_bytes_available;
}

u8 i2c0_driver_set_N_bytes (u8 num_bytes, const u8* p_buffer_from) {

	if (num_bytes > I2C0_TX_BUFFER_size()) {
		num_bytes = I2C0_TX_BUFFER_size();
	}

	DEBUG_TRACE_N(num_bytes, p_buffer_from, "local_i2c_driver_set_N_bytes()");

	I2C0_TX_BUFFER_start_write(); // this will delete all data added so far
	I2C0_TX_BUFFER_add_N_bytes(num_bytes, p_buffer_from);
	I2C0_TX_BUFFER_stop_write();

	return num_bytes;
}

u8 i2c0_driver_is_ready_for_tx (void) {

	if (I2C0_STATUS_is_set(I2C_STATUS_TX_ACTIVE) != 0) {
		return 0;

	} else if (I2C0_STATUS_is_set(I2C_STATUS_RX_ACTIVE) != 0) {
		return 0;

	} else {
		return 1;
	}
}

u8 i2c0_driver_is_ready_for_rx(void) {

	if (I2C0_STATUS_is_set(I2C_STATUS_TX_ACTIVE) != 0) {
		return 0;

	} else if (I2C0_STATUS_is_set(I2C_STATUS_RX_ACTIVE) != 0) {
		return 0;

	} else {
		return 1;
	}
}

void i2c0_driver_start_rx (u16 num_of_rx_bytes) {

	_i2c_rx_counter = num_of_rx_bytes;

	I2C0_RX_BUFFER_start_write();
	I2C0_STATUS_set(I2C_STATUS_RX_ACTIVE);

	if (I2C0_STATUS_is_set(I2C_STATUS_IS_MASTER)) {
		
		DEBUG_TRACE_word(num_of_rx_bytes, "i2c0_driver_start_rx() - MASTER");
		i2c0_driver_destination_addr = i2c0_driver_destination_addr | (0x01);
		I2C_DRIVER_SEND_START_CONDITION();

	} else {
		
		DEBUG_TRACE_word(num_of_rx_bytes, "i2c0_driver_start_rx() - SLAVE");
	}
}

void i2c0_driver_wait_for_rx(u8 num_bytes, u16 timeout_ms) {
	(void) num_bytes;
	(void) timeout_ms;
}

void i2c0_driver_stop_rx (void) {

	if (I2C0_STATUS_is_set(I2C_STATUS_IS_MASTER)) {

		DEBUG_PASS("i2c0_driver_stop_rx() - MASTER");
		I2C_DRIVER_SEND_STOP_CONDITION();

	} else {

		DEBUG_PASS("i2c0_driver_stop_rx() - SLAVE");
	}

	I2C0_STATUS_unset(I2C_STATUS_RX_ACTIVE);
	I2C0_RX_BUFFER_stop_write();
}

void i2c0_driver_start_tx (void) {

	// for TX LSB needs to be '0'
	i2c0_driver_destination_addr = i2c0_driver_destination_addr & ~(0x01);

	I2C0_STATUS_set(I2C_STATUS_TX_ACTIVE);
	I2C0_TX_BUFFER_start_read();

	if (I2C0_STATUS_is_set(I2C_STATUS_IS_MASTER)) {

		DEBUG_PASS("i2c0_driver_start_tx() - MASTER");
		I2C_DRIVER_SEND_START_CONDITION();

	} else {

		DEBUG_PASS("i2c0_driver_start_tx() - SLAVE");
	}
}

void i2c0_driver_wait_for_tx(u8 num_bytes, u16 timeout_ms) {
	(void) num_bytes;
	(void) timeout_ms;
}

void i2c0_driver_stop_tx (void) {

	if (I2C0_STATUS_is_set(I2C_STATUS_IS_MASTER)) {

		DEBUG_PASS("i2c0_driver_stop_tx() - MASTER");
		I2C_DRIVER_SEND_STOP_CONDITION();

	} else {
		
		DEBUG_PASS("i2c0_driver_stop_tx() - SLAVE");
	}

	I2C0_TX_BUFFER_stop_read();
	I2C0_STATUS_unset(I2C_STATUS_TX_ACTIVE);
}

void i2c0_driver_clear_rx_buffer(void) {
	I2C0_RX_BUFFER_clear_all();
}

void i2c0_driver_clear_tx_buffer(void) {
	I2C0_TX_BUFFER_clear_all();
}

void i2c0_driver_set_address (u8 addr) {

	TRACE_byte(addr); // local_i2c_driver_set_address()
	i2c0_driver_destination_addr = addr << 1;
}

u8 i2c0_driver_mutex_request(void) {
	if (i2c_mutex_is_requested() != 0) {
		return MUTEX_INVALID_ID;
	}

	return i2c_mutex_request();
}

void i2c0_driver_mutex_release(u8 m_id) {
	i2c_mutex_release(m_id);
}

ISR(TWI_vect) {

	u8 i2c_module_status = I2C_DRIVER_GET_MODULE_STATUS();

	u8 data_byte = 0;

	switch (i2c_module_status) {

		case I2C_STATE_MASTER_START_COMPLETE:
		case I2C_STATE_MASTER_REPEATED_START_COMPLETE:

			I2C_DRIVER_SEND_SLAVE_ADDRESS(i2c0_driver_destination_addr);

			break;

		case I2C_STATE_MT_SLAW_ACK_RECEIVED:
		case I2C_STATE_MT_DATA_ACK_RECEIVED:

			if (I2C0_TX_BUFFER_bytes_available() != 0) {

				data_byte = I2C0_TX_BUFFER_get_byte();
				I2C_DRIVER_SEND_DATA_BYTE(data_byte);

				I2C_ISR_TRACE_byte(data_byte); // Sending next Data-Byte

			} else {

				I2C_DRIVER_SEND_STOP_CONDITION();
				I2C0_STATUS_unset(I2C_STATUS_TX_ACTIVE);

				I2C_ISR_PASS(); // No more Data to send - sending stop-condition
			}

			break;

		case I2C_STATE_MR_DATA_BYTE_ACK_RECEIVED:

			data_byte = I2C_DRIVER_GET_DATA_REGISTER();
			I2C0_RX_BUFFER_add_byte(data_byte); // write DATA into buffer

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
				I2C0_STATUS_unset(I2C_STATUS_RX_ACTIVE);
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
			I2C0_STATUS_unset(I2C_STATUS_TX_ACTIVE);

			break;

		case I2C_STATE_SR_OWN_ADDR_RECEIVED:

			I2C_DRIVER_ENABLE_ACK_ON_NEXT_BYTE();
			I2C0_RX_BUFFER_start_write();
			DEBUG_PASS("ISR(TWI_vect) - I2C_STATE_SR_OWN_ADDR_RECEIVED");
			break;

		case I2C_STATE_SR_GENERAL_CALL_RECEIVED:

			I2C_DRIVER_ENABLE_ACK_ON_NEXT_BYTE();
			DEBUG_PASS("ISR(TWI_vect) - I2C_STATE_SR_GENERAL_CALL_RECEIVED");
			break;

		case I2C_STATE_SR_DATA_RECEIVED_ACK_RETURNED:

			data_byte = I2C_DRIVER_GET_DATA_REGISTER();
			I2C0_RX_BUFFER_add_byte(data_byte);
			I2C_DRIVER_ENABLE_ACK_ON_NEXT_BYTE();

			DEBUG_TRACE_byte(data_byte, "ISR(TWI_vect) - I2C_STATE_SR_DATA_RECEIVED_ACK_RETURNED");
			break;

		case I2C_STATE_SR_DATA_RECEIVED_NACK_RETURNED:

			data_byte = I2C_DRIVER_GET_DATA_REGISTER();
			I2C0_RX_BUFFER_add_byte(data_byte);
			I2C_DRIVER_ENABLE_ACK_ON_NEXT_BYTE();

			DEBUG_TRACE_byte(data_byte, "ISR(TWI_vect) - I2C_STATE_SR_DATA_RECEIVED_NACK_RETURNED");
			break;

		case I2C_STATE_SR_STOP_REPEATED_START_RECEIVED:
			I2C0_RX_BUFFER_stop_write();
			I2C_DRIVER_ENABLE_ACK_ON_NEXT_BYTE();
			DEBUG_PASS("ISR(TWI_vect) - I2C_STATE_SR_STOP_REPEATED_START_RECEIVED");
			break;

		case I2C_STATE_ST_OWN_ADDR_RECEIVED: // 0xA8
		
			I2C_DRIVER_SET_DATA_REGISTER(I2C0_TX_BUFFER_get_byte());
			I2C_DRIVER_ENABLE_ACK_ON_NEXT_BYTE();

			DEBUG_PASS("ISR(TWI_vect) - I2C_STATE_ST_OWN_ADDR_RECEIVED");
			break;

		case I2C_STATE_ST_DATA_TRANSMITTED_ACK_RECEIVED: // 0xB8
			I2C_DRIVER_SET_DATA_REGISTER(I2C0_TX_BUFFER_get_byte());
			I2C_DRIVER_ENABLE_ACK_ON_NEXT_BYTE();
			DEBUG_PASS("ISR(TWI_vect) - I2C_STATE_ST_DATA_TRANSMITTED_ACK_RECEIVED");
			break;

		case I2C_STATE_ST_DATA_TRANSMITTED_NACK_RECEIVED: // 0xC0
			I2C0_TX_BUFFER_stop_read();
			I2C_DRIVER_ENABLE_ACK_ON_NEXT_BYTE();
			DEBUG_PASS("ISR(TWI_vect) - I2C_STATE_ST_DATA_TRANSMITTED_NACK_RECEIVED");
			break;

		default:
			DEBUG_TRACE_byte(i2c_module_status, "ISR(TWI_vect) - unhandled Status-Code:");
			break;
	}
}
