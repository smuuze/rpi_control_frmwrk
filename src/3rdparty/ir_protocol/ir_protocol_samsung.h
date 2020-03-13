/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	ir_protocol_samusung.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _IR_PROTOCOL_SAMSUNG_H_
#define _IR_PROTOCOL_SAMSUNG_H_

// --------------------------------------------------------------------------------

#include "driver/timer/timer_interface.h"

// --------------------------------------------------------------------------------

void ir_protocol_samsung_set_timer(TIMER_INTERFACE_TYPE* p_timer_carrier, TIMER_INTERFACE_TYPE* p_timer_modulator);

void ir_protocol_samsung_transmit(u8* p_data);

// --------------------------------------------------------------------------------

#endif // _IR_REMOTE_MCU_TASK_H_
