/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	rpi_command_handler_ir_remote.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _RPI_COMMAND_HANDLER_IR_REMOTE_H_
#define _RPI_COMMAND_HANDLER_IR_REMOTE_H_

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
void ir_protocol_samsung_cmd_tv_power(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_volume_up(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

/*
 *
 */
void ir_protocol_samsung_cmd_tv_volume_down(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command);

// --------------------------------------------------------------------------------

#endif // _RPI_COMMAND_HANDLER_IR_REMOTE_H_
