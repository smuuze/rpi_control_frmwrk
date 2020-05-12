/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	command_handler/rpi_command_handler_ir_remote.c
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

#include "command_management/command_handler_interface.h"
#include "command_management/command_buffer_interface.h"
#include "command_management/answer_buffer_interface.h"
#include "command_management/protocol_interface.h"

#include "command_management/command_handler_interface.h"

#include "command_handler/rpi_command_handler_ir_remote.h"
#include "command_handler/rpi_cmd_handler_ir_remote_samsung.h"

// --------------------------------------------------------------------------------

u8 rpi_cmd_handler_ir_remote(PROTOCOL_INTERFACE* p_protocol) {

	if (p_protocol->cmd_buffer->bytes_available() == 0) {

		DEBUG_PASS("rpi_cmd_handler_ir_remote() - No Argument given !!! ---");

		p_protocol->set_finished(CMD_ERR_INVALID_COMMAND);
		return CMD_ERR_INVALID_COMMAND;
	}

	DEBUG_PASS("rpi_cmd_handler_ir_remote()");

	p_protocol->cmd_buffer->start_read();

	u8 manufactor = p_protocol->cmd_buffer->get_byte();	// samsung / jvc / sony / ...
	u8 device     = p_protocol->cmd_buffer->get_byte();	// tv / radio / blueray-player / ..
	u8 command    = p_protocol->cmd_buffer->get_byte();	// on/off, vol_up/vol_down, mute, ...

	DEBUG_TRACE_byte(manufactor, "rpi_cmd_handler_ir_remote() - Manufactor:");
	DEBUG_TRACE_byte(device,     "rpi_cmd_handler_ir_remote() - Device:");
	DEBUG_TRACE_byte(command,    "rpi_cmd_handler_ir_remote() - Command:");

	p_protocol->cmd_buffer->stop_read();

	u8 err_code = CMD_NO_ERR;

	switch (manufactor) {
		default :			err_code = CMD_ERR_INVALID_ARGUMENT; break;
		case IR_MANUFACTOR_SAMSUNG :	err_code = rpi_cmd_handler_ir_remote_samsung(device, command);

	}

	p_protocol->set_finished(err_code);
	return err_code;
}

// --------------------------------------------------------------------------------
