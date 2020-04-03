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

#define IR_MANUFACTOR_SAMSUNG		0x01
#define IR_MANUFACTOR_JVC		0x02
#define IR_MANUFACTOR_SONY		0x03

#define IR_DEVICE_TV			0x01
#define IR_DEVICE_RADIO			0x02
#define IR_DEVICE_BLUE_RAY_PLAYER	0x03

#define IR_COMMAND_POWER_ON		0x01
#define IR_COMMAND_POWER_OFF		0x01
#define IR_COMMAND_VOLUME_UP		0x01
#define IR_COMMAND_VOLUME_DOWN		0x01
#define IR_COMMAND_CHANNEL_UP		0x01
#define IR_COMMAND_CHANNEL_DOWN		0x01

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
