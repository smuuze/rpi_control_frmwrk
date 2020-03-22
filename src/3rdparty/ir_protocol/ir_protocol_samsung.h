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

/*
 *
 */
typedef struct {
	u8 address;
	u8 control;
} SAMSUNG_IR_PROTOCOL_COMMAND_TYPE;

// --------------------------------------------------------------------------------

/*
 *
 */
void ir_protocol_samsung_set_timer(TIMER_INTERFACE_TYPE* p_timer_carrier, TIMER_INTERFACE_TYPE* p_timer_modulator);

/*
 *
 */
void ir_protocol_samsung_transmit(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

// --------------------------------------------------------------------------------

/*
 *
 */
void ir_protocol_samsung_cmd_power(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_volume_up(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_volume_down(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

// --------------------------------------------------------------------------------

#endif // _IR_REMOTE_MCU_TASK_H_
