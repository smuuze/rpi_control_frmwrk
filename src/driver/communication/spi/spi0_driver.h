/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	spi0_driver.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _spi0_DRIVER_H_
#define _spi0_DRIVER_H_
#endif

//---------------------------------------------------------------------------------

#include "trx_driver_interface.h"

//---------------------------------------------------------------------------------

/*!
 *
 */
void spi0_driver_initialize(void);

/*
 *
 */
void spi0_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg);

/*
 *
 */
void spi0_driver_power_off(void);

/* this function gives information if and how many bytes have been received
 * since the last time this function was called
 */
u16 spi0_driver_bytes_available(void);

/*
 * get at maximum n bytes from the trx's modules internal buffer (the sw-buffer)
 * returns the number of bytes that have been copied into the given buffer
 */
u16 spi0_driver_get_N_bytes(u16 num_bytes, u8* p_buffer_to);

/*
 * copys at maximum n bytes into the trx-modules' internal buffer
 * remember the maximum size of the buffer
 * this function does not start any operation, only the buffer is prepared.
 * returns the number of byts that have been copied into the given buffer
 */
u16 spi0_driver_set_N_bytes(u16 num_bytes, const u8* p_buffer_from);

/*!
 *
 * @return
 */
u8 spi0_driver_is_ready_for_tx(void);

/*!
 *
 * @return
 */
u8 spi0_driver_is_ready_for_rx(void);

/*
 * starts receiving of data
 * data is received until the stop_rx function is called or the internal buffer is full
 * this function also passes the information of how many bytes there
 * are to receive from a device
 */
void spi0_driver_start_rx(u16 num_of_rx_bytes);

/*!
 *
 * @param num_bytes
 * @param timeout_ms
 */
void spi0_driver_wait_for_rx(u16 num_bytes, u16 timeout_ms);

/*
 * stops any active rx-operation
 * internal buffer will left unchanged
 */
void spi0_driver_stop_rx(void);

/*
 * starts transmitting of actual available bytes within the tx-buffer
 * if the tx-buffer is empty the module will go back to idle state
 */
void spi0_driver_start_tx(void);

/*!
 *
 * @param num_bytes
 * @param timeout_ms
 */
void spi0_driver_wait_for_tx(u16 num_bytes, u16 timeout_ms);

/*
 * stops any active tx-operation, internal buffer will left unchanged
 */
void spi0_driver_stop_tx(void);

/*
 * forces the module to clear the receive buffer
 */
void spi0_driver_clear_rx_buffer(void);


/*
 * forces the module to clear the transmit buffer
 */
void spi0_driver_clear_tx_buffer(void);

/* this function allows the initial communication over
 * the address of the slave
 */
void spi0_driver_set_address (u8 addr);

/*!
 *
 * @return
 */
u8 spi0_driver_mutex_request(void);

/*!
 *
 * @param m_id
 */
void spi0_driver_mutex_release(u8 m_id);
