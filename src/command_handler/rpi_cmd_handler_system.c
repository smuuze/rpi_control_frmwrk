/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	rpi_cmd_handler_system.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "command_management/command_handler_interface.h"
#include "command_management/command_buffer_interface.h"
#include "command_management/answer_buffer_interface.h"
#include "command_management/protocol_interface.h"

#include "common/local_context.h"

#include "command_handler/rpi_command_handler.h"

//-----------------------------------------------------------------------------

u8 rpi_cmd_get_version(PROTOCOL_INTERFACE* p_protocol) {

	TRACE_word(((u16)VERSION_MAJOR << 8) + (u16)(VERSION_MINOR)); // rpi_cmd_get_version()

	p_protocol->answ_buffer->start_write();
	p_protocol->answ_buffer->add_byte(VERSION_MAJOR);
	p_protocol->answ_buffer->add_byte(VERSION_MINOR);
	p_protocol->answ_buffer->stop_write();

	p_protocol->set_finished(CMD_NO_ERR);

	return CMD_NO_ERR;
}
