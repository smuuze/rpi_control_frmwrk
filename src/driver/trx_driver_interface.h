/**
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * @file    trx_driver_interface.h
 * @author  Sebastian Lesse
 * @date    2022 / 04 / 11
 * @brief   Short description of this file
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_trx_driver_interface_
#define _H_trx_driver_interface_

// --------------------------------------------------------------------------------

#include "cfg_driver_interface.h"

// --------------------------------------------------------------------------------

/**
 * @brief enumeration to identify the type of the driver-interface
 * actual supported types are:
 * - SPI
 * - USART
 * - I2C
 * 
 */
typedef enum {
	SPI,
	USART,
	I2C
} TRX_DRIVER_INTERFACE_TYPE;

// --------------------------------------------------------------------------------

/**
 * @brief Tells trx-driver-interface module to start receiving data as long
 * it is stopped by the caller
 * 
 */
#define TRX_DRIVER_INTERFACE_UNLIMITED_RX_LENGTH		0xFFFF

/**
 * @brief Callback to initialize a tr-driver-module.
 * Is only called at system-startup time
 * does not take any arguments.
 * 
 */
typedef void (*TRX_DRIVER_INTERFACE_INITIALIZE_CALLBACK) (void);

/**
 * @brief Callback to configure a trx-driver-module.
 * Call this function with your config-parameters before any operation takes place
 * Every application that uses the trx-driver-module my sets its own configuration.
 * To ensure correect operation set application depending configuration before using.
 * 
 * @param p_configuration configuration to apply on this module,
 * 
 * @see cfg_driver_interface.h#TRX_DRIVER_CONFIGURATION
 * 
 */
typedef void (*TRX_DRIVER_INTERFACE_CONFIGURE_CALLBACK) (TRX_DRIVER_CONFIGURATION* p_configuration);

/**
 * @brief Disables the hw module and canels all current pending operations.
 * Use this function to enter power-safe mode of the trx-driver-module.
 * 
 */
typedef void (*TRX_DRIVER_INTERFACE_MODULE_OFF_CALLBACK) (void);

/**
 * @brief Gets the number of bytes that are ready to be read from the trx-driver-module.
 * @return the number of bytes that are available for reading.
 * 
 */
typedef u16 (*TRX_DRIVER_INTERFACE_BYTES_AVAILABLE_CALLBACK) (void);

/**
 * @brief Read bytes from the trx-driver-module. Data is copied into the given buffer.
 * 
 * @param num_byts_max maximum number of bytes to be copied, must not be larger than the given buffer
 * @param p_buffer_to the destination were the data is copied into
 * 
 * @return the number of bytes that have been copied into the given buffer.
 */
typedef u16 (*TRX_DRIVER_INTERFACE_GET_N_BYTES_CALLBACK) (u16 num_byts_max, u8* p_buffer_to);

/**
 * @brief Copies the given number of bytes from the the given buffer into the tx-devices internal buffer.
 * If the internal buffer is les than the given number of bytes, only the number of bytes that fit into
 * the internal buffer will be copied. This function only copies data, no other operation is started.
 * 
 * @param num_byts_max number of bytes to be copied into the internal buffer.
 * @param p_buffer_to the destination were the data is copied into
 * 
 * @return the number of bytes that have been copied into the given buffer.
 * 
 */
typedef u16 (*TRX_DRIVER_INTERFACE_SET_N_BYTES_CALLBACK) (u16 num_byts_max, const u8* p_buffer_from);

/**
 * @brief starts receiving of data
 * data is received until the stop_rx function is called or the internal buffer is full
 * 
 * @param num_of_rx_bytes
 * 
 */
typedef void (*TRX_DRIVER_INTERFACE_START_RX_CALLBACK)		(u16 num_of_rx_bytes);

/*
 */
typedef void (*TRX_DRIVER_INTERFACE_WAIT_FOR_RX_CALLBACK)	(u16 num_byts_max, u16 timeout_ms);


/*
 */
typedef void (*TRX_DRIVER_INTERFACE_START_TX_CALLBACK)		(void);

/*
 */
typedef void (*TRX_DRIVER_INTERFACE_WAIT_FOR_TX_CALLBACK)	(u16 num_byts_max, u16 timeout_ms);

/*
 */
typedef void (*TRX_DRIVER_INTERFACE_STOP_TX_CALLBACK)		(void);

/*
 */
typedef void (*TRX_DRIVER_INTERFACE_STOP_RX_CALLBACK)		(void);

/*
 *
 */
typedef void (*TRX_DRIVER_INTERFACE_CLEAR_RX_BUFFER_CALLBACK)	(void);

/*
 *
 */
typedef void (*TRX_DRIVER_INTERFACE_CLEAR_TX_BUFFER_CALLBACK)	(void);

/*
 *
 */
typedef u8 (*TRX_DRIVER_INTERFACE_IS_READ_FOR_TRX_CALLBACK)	(void);

/*
 *
 */
typedef void (*TRX_DRIVER_INTERFACE_SET_ADDRESS_CALLBACK)	(u8 addr);

/*
 *
 */
typedef u8 (*TRX_DRIVER_INTERFACE_REQUEST_MUTEX_CALLBACK)	(void);

/*
 *
 */
typedef void (*TRX_DRIVER_INTERFACE_RELEASE_MUTEX_CALLBACK)	(u8 m_id);

//-----------------------------------------------------------------------------

/*
 *
 */
typedef struct TRX_DRIVER_INTERFACE {

    /**
     * @see trx_driver_interface.h#TRX_DRIVER_INTERFACE_TYPE
     * 
     */
    TRX_DRIVER_INTERFACE_TYPE type;

    /**
     * @see trx_driver_interface.h#TRX_DRIVER_INTERFACE_INITIALIZE_CALLBACK
     * 
     */
    TRX_DRIVER_INTERFACE_INITIALIZE_CALLBACK initialize;

    /**
     * @see trx_driver_interface.h#TRX_DRIVER_INTERFACE_CONFIGURE_CALLBACK
     * 
     */
    TRX_DRIVER_INTERFACE_CONFIGURE_CALLBACK configure;

    /**
     * @see trx_driver_interface.h#TRX_DRIVER_INTERFACE_MODULE_OFF_CALLBACK
     * 
     */
    TRX_DRIVER_INTERFACE_MODULE_OFF_CALLBACK shut_down;

    /**
     * @see trx_driver_interface.h#TRX_DRIVER_INTERFACE_BYTES_AVAILABLE_CALLBACK
     * 
     */
    TRX_DRIVER_INTERFACE_BYTES_AVAILABLE_CALLBACK bytes_available;

    /**
     * @see trx_driver_interface.h#TRX_DRIVER_INTERFACE_GET_N_BYTES_CALLBACK
     * 
     */
    TRX_DRIVER_INTERFACE_GET_N_BYTES_CALLBACK get_N_bytes;

    /**
     * @see trx_driver_interface.h#TRX_DRIVER_INTERFACE_SET_N_BYTES_CALLBACK
     * 
     */
    TRX_DRIVER_INTERFACE_SET_N_BYTES_CALLBACK set_N_bytes;

    /**
     * @see trx_driver_interface.h#	TRX_DRIVER_INTERFACE_START_RX_CALLBACK start_rx;
     * 
     */
	TRX_DRIVER_INTERFACE_START_RX_CALLBACK start_rx;

	/*
	 *
	 */
	TRX_DRIVER_INTERFACE_WAIT_FOR_RX_CALLBACK wait_for_rx;


	/*
	 * stops any active rx-operation
	 * internal buffer will left unchanged
	 */
	TRX_DRIVER_INTERFACE_STOP_RX_CALLBACK stop_rx;

	/*
	 *
	 */
	TRX_DRIVER_INTERFACE_IS_READ_FOR_TRX_CALLBACK is_ready_for_tx;

	/*
	 *
	 */
	TRX_DRIVER_INTERFACE_IS_READ_FOR_TRX_CALLBACK is_ready_for_rx;

	/*
	 * starts transmitting of actual available bytes within the tx-buffer
	 * if the tx-buffer is empty the module will go back to idle state
	 */
	TRX_DRIVER_INTERFACE_START_TX_CALLBACK start_tx;

	/*
	 *
	 */
	TRX_DRIVER_INTERFACE_WAIT_FOR_TX_CALLBACK wait_for_tx;


	/*
	 * stops any active tx-operation, internal buffer will left unchanged
	 */
	TRX_DRIVER_INTERFACE_STOP_TX_CALLBACK stop_tx;

	/*
	 * forces the module to clear the internal receive buffer
	 */
	TRX_DRIVER_INTERFACE_CLEAR_RX_BUFFER_CALLBACK clear_rx_buffer;

	/*
	 * forces the module to clear the internal transmit buffer
	 */
	TRX_DRIVER_INTERFACE_CLEAR_RX_BUFFER_CALLBACK clear_tx_buffer;

	/*
	 * sets address of a device connected via bus if necessary
	 */
	TRX_DRIVER_INTERFACE_SET_ADDRESS_CALLBACK set_address;

	/*
 	 *
 	 */
	TRX_DRIVER_INTERFACE_REQUEST_MUTEX_CALLBACK mutex_req;

	/*
	 *
	 */
	TRX_DRIVER_INTERFACE_RELEASE_MUTEX_CALLBACK mutex_rel;


} TRX_DRIVER_INTERFACE;

// --------------------------------------------------------------------------------

#endif // _H_trx_driver_interface_
