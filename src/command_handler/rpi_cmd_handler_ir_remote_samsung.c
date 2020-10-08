/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	command_handler/rpi_cmd_hander_ir_remote_samsung.c
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_ON

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "common/signal_slot_interface.h"
#include "command_management/command_handler_interface.h"
#include "command_handler/rpi_command_handler_ir_remote.h"
#include "3rdparty/ir_protocol/ir_protocol_samsung.h"

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(SAMSUNG_IR_CMD_RECEIVED_SIGNAL)

// --------------------------------------------------------------------------------

static inline u8 rpi_cmd_ir_samsung_tv(u8 command);

// --------------------------------------------------------------------------------

u8 rpi_cmd_handler_ir_remote_samsung(u8 device, u8 command) {

	DEBUG_PASS("rpi_cmd_handler_ir_remote_samsung()");

	switch (device) {
		default:		return CMD_ERR_INVALID_ARGUMENT;
		case IR_DEVICE_TV : 	return rpi_cmd_ir_samsung_tv(command);
	}
}

// --------------------------------------------------------------------------------

static inline u8 rpi_cmd_ir_samsung_tv(u8 cmd) {
	
	DEBUG_TRACE_byte(cmd, "rpi_cmd_ir_samsung_tv() - Command:");
	SAMSUNG_IR_PROTOCOL_COMMAND_TYPE ir_command;

	switch (cmd) {

		default:			return CMD_ERR_INVALID_ARGUMENT;

		case IR_COMMAND_POWER_ON : 	ir_protocol_samsung_cmd_tv_power(&ir_command); break;
		case IR_COMMAND_POWER_OFF :	ir_protocol_samsung_cmd_tv_power(&ir_command); break;

		case IR_COMMAND_VOLUME_UP :	ir_protocol_samsung_cmd_tv_volume_up(&ir_command); break;
		case IR_COMMAND_VOLUME_DOWN :	ir_protocol_samsung_cmd_tv_volume_down(&ir_command); break;

		case IR_COMMAND_CHANNEL_UP :	ir_protocol_samsung_cmd_tv_channel_up(&ir_command); break;
		case IR_COMMAND_CHANNEL_DOWN :	ir_protocol_samsung_cmd_tv_channel_down(&ir_command); break;

		case IR_COMMAND_CHANNEL_0 :	ir_protocol_samsung_cmd_tv_channel_0(&ir_command); break;
		case IR_COMMAND_CHANNEL_1 :	ir_protocol_samsung_cmd_tv_channel_1(&ir_command); break;
		case IR_COMMAND_CHANNEL_2 :	ir_protocol_samsung_cmd_tv_channel_2(&ir_command); break;
		case IR_COMMAND_CHANNEL_3 :	ir_protocol_samsung_cmd_tv_channel_3(&ir_command); break;
		case IR_COMMAND_CHANNEL_4 :	ir_protocol_samsung_cmd_tv_channel_4(&ir_command); break;
		case IR_COMMAND_CHANNEL_5 :	ir_protocol_samsung_cmd_tv_channel_5(&ir_command); break;
		case IR_COMMAND_CHANNEL_6 :	ir_protocol_samsung_cmd_tv_channel_6(&ir_command); break;
		case IR_COMMAND_CHANNEL_7 :	ir_protocol_samsung_cmd_tv_channel_7(&ir_command); break;
		case IR_COMMAND_CHANNEL_8 :	ir_protocol_samsung_cmd_tv_channel_8(&ir_command); break;
		case IR_COMMAND_CHANNEL_9 :	ir_protocol_samsung_cmd_tv_channel_9(&ir_command); break;

		case IR_COMMAND_ENTER :		ir_protocol_samsung_cmd_tv_enter(&ir_command); break;
		case IR_COMMAND_EXIT :		ir_protocol_samsung_cmd_tv_exit(&ir_command); break;
		case IR_COMMAND_RETURN :	ir_protocol_samsung_cmd_tv_return(&ir_command); break;
		case IR_COMMAND_ARROW_UP :	ir_protocol_samsung_cmd_tv_arrow_up(&ir_command); break;
		case IR_COMMAND_ARROW_DOWN :	ir_protocol_samsung_cmd_tv_arrow_down(&ir_command); break;
		case IR_COMMAND_ARROW_LEFT :	ir_protocol_samsung_cmd_tv_arrow_left(&ir_command); break;
		case IR_COMMAND_ARROW_RIGHT :	ir_protocol_samsung_cmd_tv_arrow_right(&ir_command); break;

		case IR_COMMAND_PLAY :		ir_protocol_samsung_cmd_tv_play(&ir_command); break;
		case IR_COMMAND_PAUSE :		ir_protocol_samsung_cmd_tv_pause(&ir_command); break;
		case IR_COMMAND_STOP :		ir_protocol_samsung_cmd_tv_stop(&ir_command); break;
	}

	ir_protocol_samsung_address_tv(&ir_command);

	SAMSUNG_IR_CMD_RECEIVED_SIGNAL_send((void*) &ir_command);

	return CMD_NO_ERR;
}