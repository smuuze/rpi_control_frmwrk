#ifndef _TRX_DRIVER_INTERFACE_H_
#define _TRX_DRIVER_INTERFACE_H_

#include "cfg_driver_interface.h"

typedef enum {
	SPI,
	USART,
	I2C
} TRX_DRIVER_INTERFACE_TYPE;

#define TRX_DRIVER_INTERFACE_UNLIMITED_RX_LENGTH		0xFFFF

/*
 */
typedef void (*TRX_DRIVER_INTERFACE_INITIALIZE_CALLBACK)	(void);

/*
 */
typedef void (*TRX_DRIVER_INTERFACE_CONFIGURE_CALLBACK)		(TRX_DRIVER_CONFIGURATION* p_configuration);

/*
 */
typedef void (*TRX_DRIVER_INTERFACE_MODULE_OFF_CALLBACK)	(void);

/*
 */
typedef u8 (*TRX_DRIVER_INTERFACE_BYTES_AVAILABLE_CALLBACK)	(void);

/*
 */
typedef u8 (*TRX_DRIVER_INTERFACE_GET_N_BYTES_CALLBACK)		(u8 num_byts_max, u8* p_buffer_to);

/*
 */
typedef u8 (*TRX_DRIVER_INTERFACE_SET_N_BYTES_CALLBACK)		(u8 num_byts_max, const u8* p_buffer_from);

/*
 */
typedef void (*TRX_DRIVER_INTERFACE_START_RX_CALLBACK)		(u16 num_of_rx_bytes);

/*
 */
typedef void (*TRX_DRIVER_INTERFACE_WAIT_FOR_RX_CALLBACK)	(u8 num_byts_max, u16 timeout_ms);


/*
 */
typedef void (*TRX_DRIVER_INTERFACE_START_TX_CALLBACK)		(void);

/*
 */
typedef void (*TRX_DRIVER_INTERFACE_WAIT_FOR_TX_CALLBACK)	(u8 num_byts_max, u16 timeout_ms);

/*
 */
typedef void (*TRX_DRIVER_INTERFACE_STOP_TX_CALLBACK)		(void);

/*
 */
typedef void (*TRX_DRIVER_INTERFACE_STOP_RX_CALLBACK)		(void);

/*
 *
 */
typedef void (*TRX_DRIVER_INTERFACE_CLEAR_BUFFER_CALLBACK)	(void);

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

/*
 *
 */


/*
 *
 */
typedef struct TRX_DRIVER_INTERFACE {

	TRX_DRIVER_INTERFACE_TYPE type;

	/* Shall only be called once during boot time
	 */
	TRX_DRIVER_INTERFACE_INITIALIZE_CALLBACK initialize;

	/* call this function with your config-parameters before any operation takes place
	 * you can call this function again before your operation is started, if this module is used as a master
	 */
	TRX_DRIVER_INTERFACE_CONFIGURE_CALLBACK configure;

	/* disables the hw module and canels all current pending operations
	 *
	 */
	TRX_DRIVER_INTERFACE_MODULE_OFF_CALLBACK shut_down;

	/* this function gives information if and how many bytes have been received
	 * since the last time this function was called
	 */
	TRX_DRIVER_INTERFACE_BYTES_AVAILABLE_CALLBACK bytes_available;

	/*
	 * get at maximum n bytes from the trx's modules internal buffer (the sw-buffer)
	 * returns the number of bytes that have been copied into the given buffer
	 */
	TRX_DRIVER_INTERFACE_GET_N_BYTES_CALLBACK get_N_bytes;

	/*
	 * copys at maximum n bytes into the trx-modules' internal buffer
	 * remember the maximum size of the buffer
	 * this function does not start any operation, only the buffer is prepared.
	 * returns the number of byts that have been copied into the given buffer
	 */
	TRX_DRIVER_INTERFACE_SET_N_BYTES_CALLBACK set_N_bytes;

	/*
	 * starts receiving of data
	 * data is received until the stop_rx function is called or the internal buffer is full
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
	 * forces the module to clear all internal buffers and go back to the idle state.
	 */
	TRX_DRIVER_INTERFACE_CLEAR_BUFFER_CALLBACK clear_buffer;

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

#endif // _TRX_DRIVER_INTERFACE_H_
