#ifndef _LOCAL_SPI_DRIVER_H_
#define _LOCAL_SPI_DRIVER_H_


#ifndef LOCAL_SPI_DRIVER_MAX_NUM_BYTES_RECEIVE_BUFFER
#define LOCAL_SPI_DRIVER_MAX_NUM_BYTES_RECEIVE_BUFFER	512
#endif

#ifndef LOCAL_SPI_DRIVER_MAX_NUM_BYTES_TRANSMIT_BUFFER
#define LOCAL_SPI_DRIVER_MAX_NUM_BYTES_TRANSMIT_BUFFER	128
#endif

#include "cfg_driver_interface.h"

/*!
 *
 */
void spi_driver_initialize(void);

/*!
 *
 */
void spi_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg);

/*!
 *
 */
void spi_driver_power_off(void);

/* this function gives information if and how many bytes have been received
 * since the last time this function was called
 */
u8 spi_driver_bytes_available(void);

/*
 * get at maximum n bytes from the trx's modules internal buffer (the sw-buffer)
 * returns the number of bytes that have been copied into the given buffer
 */
u8 spi_driver_get_N_bytes(u8 num_bytes, u8* p_buffer_to);

/*
 * copys at maximum n bytes into the trx-modules' internal buffer
 * remember the maximum size of the buffer
 * this function does not start any operation, only the buffer is prepared.
 * returns the number of byts that have been copied into the given buffer
 */
u8 spi_driver_set_N_bytes(u8 num_bytes, const u8* p_buffer_from);

/*!
 *
 * @return
 */
u8 spi_driver_is_ready_for_tx(void);

/*!
 *
 * @return
 */
u8 spi_driver_is_ready_for_rx(void);

/*
 * starts receiving of data
 * data is received until the stop_rx function is called or the internal buffer is full
 */
void spi_driver_start_rx(u16 num_of_rx_bytes);

/*
 * stops any active rx-operation
 * internal buffer will left unchanged
 */
void spi_driver_stop_rx(void);

/*
 * starts transmitting of actual available bytes within the tx-buffer
 * if the tx-buffer is empty the module will go back to idle state
 */
void spi_driver_start_tx(void);

/*
 * stops any active tx-operation, internal buffer will left unchanged
 */
void spi_driver_stop_tx(void);

/*
 * forces the module to clear all internal buffers and go back to the idle state.
 */
void spi_driver_clear_buffer(void);

/*
 *
 */
void spi_driver_set_address (u8 addr);

/*!
 *
 * @return
 */
u8 spi_driver_mutex_request(void) ;

/*!
 *
 * @param m_id
 */
void spi_driver_mutex_release(u8 m_id);

#endif // _LOCAL_SPI_DRIVER_H_
