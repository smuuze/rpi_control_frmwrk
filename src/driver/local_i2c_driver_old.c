/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "utils/stdmacros.h"
#include RTOS_H

#include <avr/io.h>
#include <avr/interrupt.h>

#include "local_msg_buffer.h"
#include "local_i2c_driver.h"

#define LOCAL_I2C_STATE_MASTER_START					0x01

#define LOCAL_I2C_STATE_MASTER_START_COMPLETE				0x08
#define LOCAL_I2C_STATE_MASTER_REPEATED_START_COMPLETE			0x10

#define LOCAL_I2C_STATE_MASTER_SLAW_ACK_RECEIVED			0x18
#define LOCAL_I2C_STATE_MASTER_SLAW_NOACK_RECEIVED			0x20
#define LOCAL_I2C_STATE_MASTER_DATA_ACK_RECEIVED			0x28
#define LOCAL_I2C_STATE_MASTER_DATA_NOACK_RECEIVED			0x30
#define LOCAL_I2C_STATE_MASTER_ARBITRATION_LOST				0x38

#define LOCAL_I2C_STATE_SLAR_ACK_RECEIVED				0x40
#define LOCAL_I2C_STATE_SLAR_NOACK_RECEIVED				0x48
#define LOCAL_I2C_STATE_DATA_BYTE_ACK_RECEIVED				0x50
#define LOCAL_I2C_STATE_DATA_BYTE_NOACK_RECEIVED			0x58

#define LOCAL_I2C_STATE_INITIATE_WRITE_OP_COMPLETE			0x10
#define LOCAL_I2C_STATE_NOT_ACK_RECEIVED				0x20

#define LOCAL_I2C_INITIATE_WRITE_OP					(1 << 0)
#define LOCAL_I2C_INITIATE_READ_OP					(0)


BUILD_LOCAL_MSG_BUFFER(static inline, __i2c_transmit_buffer, LOCAL_I2C_DRIVER_MAX_NUM_BYTES_TRANSMIT_BUFFER)
BUILD_LOCAL_MSG_BUFFER(static inline, __i2c_receive_buffer, LOCAL_I2C_DRIVER_MAX_NUM_BYTES_RECEIVE_BUFFER)

/*!
 *
 */
static u8 __i2c_remote_address;

/*!
 *
 */
static u8 __i2c_op_mode;

/*!
 *
 */
static u8 __i2c_actual_data_byte;

/*!
 *
 */
static u8 __i2c_retry_counter;

static void __i2c_state_machine(u8 status_code) {

	switch (status_code) {
		case LOCAL_I2C_STATE_MASTER_START :

			if (__i2c_op_mode == LOCAL_I2C_DRIVER_OP_MODE_MASTER) {
				TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) | (1 << TWIE);
			} else {
				TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
			}

			__i2c_retry_counter = 0;
			break;

		case LOCAL_I2C_STATE_MASTER_START_COMPLETE :	/* no break */
		case LOCAL_I2C_STATE_MASTER_REPEATED_START_COMPLETE :

			TWDR = __i2c_remote_address;
			break;

		case LOCAL_I2C_STATE_MASTER_SLAW_NOACK_RECEIVED	 : /* no break */
		case LOCAL_I2C_STATE_SLAR_NOACK_RECEIVED :

			if (__i2c_retry_counter < LOCAL_I2C_DRIVER_MAX_NUMBER_START_RETRY) {
				__i2c_retry_counter += 1;
				// repeated start will be transmitted
				TWCR = (1 << TWSTA) | (1 << TWINT) | (1 << TWIE);
			} else {
				// stop-condition will be transmitted
				TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWIE);
			}

			break;

		case LOCAL_I2C_STATE_MASTER_SLAW_ACK_RECEIVED : /* no break */
		case LOCAL_I2C_STATE_MASTER_DATA_ACK_RECEIVED :

			if (__i2c_transmit_buffer_bytes_available()) {
				// writing next data byte
				__i2c_actual_data_byte = __i2c_transmit_buffer_get_byte();
				TWDR = __i2c_actual_data_byte;
			} else {
				// writing stop-condition
				TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN) | (1 << TWIE);
			}

			__i2c_retry_counter = 0;

			break;

		case LOCAL_I2C_STATE_MASTER_DATA_NOACK_RECEIVED	 :

			if (__i2c_retry_counter < LOCAL_I2C_DRIVER_MAX_NUMBER_START_RETRY) {
				__i2c_retry_counter += 1;
				// retransmit data-byte
				TWDR = __i2c_actual_data_byte;
			} else {
				// stop-condition will be transmitted
				TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWIE);
			}

			break;

		case LOCAL_I2C_STATE_SLAR_ACK_RECEIVED :
			// going to receive data-byte
			TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWIE);
			break;

		case LOCAL_I2C_STATE_DATA_BYTE_ACK_RECEIVED :
			// more data available
			__i2c_receive_buffer_add_byte(TWDR);
			TWCR = (1 << TWINT) | (1 << TWEA) | (1 << TWIE);
			break;

		case LOCAL_I2C_STATE_DATA_BYTE_NOACK_RECEIVED :
			// no ore data available
			__i2c_receive_buffer_add_byte(TWDR);
			break;

		case LOCAL_I2C_STATE_MASTER_ARBITRATION_LOST :

			break;
	}
}

/*!
 *
 */
static void __i2c_start_write(void) {
	__i2c_remote_address |= LOCAL_I2C_INITIATE_WRITE_OP;
	__i2c_state_machine(LOCAL_I2C_STATE_MASTER_START);
}

static void __i2c_start_read(void) {
	__i2c_remote_address |= LOCAL_I2C_INITIATE_READ_OP;
	__i2c_state_machine(LOCAL_I2C_STATE_MASTER_START);
}


void local_i2c_driver_init(LOCAL_I2C_DRIVER_CFG* p_i2c_cfg) {

	__i2c_op_mode = p_i2c_cfg->op_mode;

	TWBR = p_i2c_cfg->bit_rate;

	if (p_i2c_cfg->op_mode == LOCAL_I2C_DRIVER_OP_MODE_SLAVE) {
		TWAR = (p_i2c_cfg->slave_own_address << 1) | (p_i2c_cfg->slave_broadcast_response);
	}

}


void local_i2c_driver_power_off(void) {

}


void local_i2c_driver_set_address(u8 addr) {
	__i2c_remote_address = (addr << 1);
}


void local_i2c_driver_write_byte(u8 byte) {
	__i2c_transmit_buffer_add_byte(byte);
	__i2c_start_write();
}


void local_i2c_driver_write_word(u16 word) {
	__i2c_transmit_buffer_add_word(word);
	__i2c_start_write();
}


void local_i2c_driver_write_long(u32 long_integer) {
	__i2c_transmit_buffer_add_long(long_integer);
	__i2c_start_write();
}


void local_i2c_driver_write_N_bytes(u8 num_bytes, u8* p_buffer_from) {
	__i2c_transmit_buffer_add_N_bytes(num_bytes, p_buffer_from);
	__i2c_start_write();
}


u8 local_i2c_driver_bytes_available(void) {
	return __i2c_transmit_buffer_bytes_available();
}


u8 local_i2c_driver_read_byte(void) {
	return __i2c_receive_buffer_get_byte();
}


u16 local_i2c_driver_read_word(void) {
	return __i2c_receive_buffer_get_word();
}


u32 local_i2c_driver_read_long(void) {
	return __i2c_receive_buffer_get_long();
}


void local_i2c_driver_read_N_bytes(u8 num_bytes, u8* p_buffer_to) {
	__i2c_receive_buffer_get_N_bytes(num_bytes, p_buffer_to);
}


ISR(TWI_vect) {
	__i2c_state_machine((TWSR & 0xFC));
}
