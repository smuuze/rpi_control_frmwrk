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

#include "command_management/protocol_interface.h"

// --------------------------------------------------------------------------------

#define IR_MANUFACTOR_SAMSUNG		0x01
#define IR_MANUFACTOR_JVC		0x02
#define IR_MANUFACTOR_SONY		0x03

#define IR_DEVICE_TV			0x01
#define IR_DEVICE_RADIO			0x02
#define IR_DEVICE_BLUE_RAY_PLAYER	0x03

#define IR_COMMAND_POWER_ON		0x01
#define IR_COMMAND_POWER_OFF		0x02

#define IR_COMMAND_VOLUME_UP		0x03
#define IR_COMMAND_VOLUME_DOWN		0x04

#define IR_COMMAND_CHANNEL_UP		0x05
#define IR_COMMAND_CHANNEL_DOWN		0x06

#define IR_COMMAND_CHANNEL_0		0x07
#define IR_COMMAND_CHANNEL_1		0x08
#define IR_COMMAND_CHANNEL_2		0x09
#define IR_COMMAND_CHANNEL_3		0x0A
#define IR_COMMAND_CHANNEL_4		0x0B
#define IR_COMMAND_CHANNEL_5		0x0C
#define IR_COMMAND_CHANNEL_6		0x0D
#define IR_COMMAND_CHANNEL_7		0x0E
#define IR_COMMAND_CHANNEL_8		0x0F
#define IR_COMMAND_CHANNEL_9		0x10

#define IR_COMMAND_ENTER		0x11
#define IR_COMMAND_EXIT			0x12
#define IR_COMMAND_RETURN		0x13
#define IR_COMMAND_ARROW_UP		0x14
#define IR_COMMAND_ARROW_DOWN		0x15
#define IR_COMMAND_ARROW_LEFT		0x16
#define IR_COMMAND_ARROW_RIGHT		0x17

#define IR_COMMAND_PLAY			0x18
#define IR_COMMAND_PAUSE		0x19
#define IR_COMMAND_STOP			0x1A

// --------------------------------------------------------------------------------

u8 rpi_cmd_handler_ir_remote(PROTOCOL_INTERFACE* p_protocol);

// --------------------------------------------------------------------------------

#endif // _RPI_COMMAND_HANDLER_IR_REMOTE_H_
