
 /*
  * \@file	copro/copro.h
  * \author	sebastian lesse
  */

#ifndef _COPRO_H_
#define _COPRO_H_

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!

//-----------------------------------------------------------------------------

#include "driver/trx_driver_interface.h.h"

//-----------------------------------------------------------------------------

/*!
 *
 */
void copro1_initialize(TRX_DRIVER_INTERFACE* p_com_driver_interface);

/*!
 *
 */
u8 copro1_bytes_available(void);

/*!
 *
 */
u8 copro1_get_N_bytes(u8 num_bytes, u8* p_buffer_to);

/*!
 *
 */
u8 copro1_set_N_bytes(u8 num_bytes, const u8* p_buffer_from);


/*!
 *
 */
u8 copro1_is_ready_for_tx(void);

/*!
 *
 */
void copro1_start_rx(u16 num_of_rx_bytes);

/*!
 *
 */
void copro1_wait_for_rx(u8 num_bytes, u16 timeout_ms);

/*!
 *
 */
void copro1_stop_rx(void);


/*!
 *
 */
u8 copro1_is_ready_for_tx(void);

/*!
 *
 */
void copro1_start_tx(void);

/*!
 *
 */
void copro1_wait_for_tx(u8 num_bytes, u16 timeout_ms);

/*!
 *
 */
void copro1_stop_tx(void);


/*!
 *
 */
void copro1_clear_rx_buffer(void);

/*!
 *
 */
copro1_set_address(u8 addr);

/*!
 *
 */
u8 copro1_mutex_request(void);

/*!
 *
 */
void copro1_mutex_release(u8 m_id);

//-----------------------------------------------------------------------------

#endif // _COPRO1_H_