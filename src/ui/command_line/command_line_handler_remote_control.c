/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	command_line_handler_remote_control.c
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "common/signal_slot_interface.h"
#include "ui/command_line/command_line_interface.h"
#include "common/common_tools_string.h"

// --------------------------------------------------------------------------------

#include "ui/command_line/command_line_handler_remote_control.h"

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CLI_INVALID_PARAMETER_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(RPI_HOST_COMMAND_RECEIVED_SIGNAL)

// --------------------------------------------------------------------------------

/**
 * @brief Table for Samsung commands and it's HEX-Strings
 * 
 */
static CLI_REMOTE_CONTROL_COMMAND_PAIR cmd_array_samsung[] = { CLI_REMOTE_CONTROL_CMD_ARRAY_SAMSUNG };

/**
 * @brief Table for JVC commands and it's HEX-Strings
 * 
 */
static CLI_REMOTE_CONTROL_COMMAND_PAIR cmd_array_jvc[] = { CLI_REMOTE_CONTROL_CMD_ARRAY_JVC };

// --------------------------------------------------------------------------------

static u8 command_line_handler_remote_control_get_command(const char* cmd_name, CLI_REMOTE_CONTROL_COMMAND_PAIR* p_cmd_table, u16 table_length, COMMON_GENERIC_BUFFER_TYPE* p_buffer) {

	DEBUG_TRACE_word(table_length, "command_line_handler_remote_control_get_command() - Table-Size:");

	u16 i = 0;
	for ( ; i < table_length ; i++ ) {

		if ( common_tools_string_compare(p_cmd_table[i].cmd_name, cmd_name) ) {

			DEBUG_TRACE_STR(p_cmd_table[i].cmd_name, "command_line_handler_remote_control_get_command() - Command Found");

			p_buffer->length = common_tools_hex_string_to_byte_array(
				p_cmd_table[i].cmd_hex_str,
				CLI_REMOTE_CONTROL_CMD_HEX_STR_LENGTH,
				p_buffer->data,
				COMMON_TYPES_GENERIC_BUFFER_SIZE
			);

			return 1;
		} 
	}

	return 0;
}

// --------------------------------------------------------------------------------

void command_line_handler_remote_control(const char* parameter) {

	if (parameter == NULL) {
		DEBUG_PASS("command_line_handler_command() - INVALID PARAMETER - parameter is NULL");
		CLI_INVALID_PARAMETER_SIGNAL_send((void*)COMMAND_LINE_ARGUMENT_COMMAND);
		return;
	}

	if (common_tools_string_length(parameter) == 0) {
		DEBUG_PASS("command_line_handler_command() - INVALID PARAMETER - parameter has length of zero");
		CLI_INVALID_PARAMETER_SIGNAL_send((void*)COMMAND_LINE_ARGUMENT_COMMAND);
		return;
	}

	DEBUG_TRACE_STR(parameter, "command_line_handler_remote_control() - parameter:");
	
	u8 command_available = 0;
	COMMON_GENERIC_BUFFER_TYPE buffer;

	if ( command_line_handler_remote_control_get_command(parameter, cmd_array_jvc, CLI_REMOTE_SIZEOF_COMMAND_PAIR_TABLE(cmd_array_jvc), &buffer) ) {

		DEBUG_PASS("command_line_handler_remote_control() - this a JVC remote-control command");
		command_available = 1;

	} else if ( command_line_handler_remote_control_get_command(parameter, cmd_array_samsung, CLI_REMOTE_SIZEOF_COMMAND_PAIR_TABLE(cmd_array_samsung), &buffer) ) {

		DEBUG_PASS("command_line_handler_remote_control() - this a SAMSUNG remote-control command");
		command_available = 1;
	}

	if (command_available) {
		RPI_HOST_COMMAND_RECEIVED_SIGNAL_send(&buffer);
	}
}
