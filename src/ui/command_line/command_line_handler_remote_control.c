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

static inline u8 command_line_handler_remote_control_is_jvc_command(const char* cmd_name, COMMON_GENERIC_BUFFER_TYPE* p_buffer) {

	if ( common_tools_string_compare(cmd_name, CLI_REMOTE_CONTROL_NAME_JVC_RADIO_POWER_ON) ) {

		DEBUG_PASS("command_line_handler_remote_control_is_jvc_command() - CLI_REMOTE_CONTROL_NAME_JVC_RADIO_POWER_ON");

		p_buffer->length = common_tools_hex_string_to_byte_array(
			CLI_REMOTE_CONTROL_CMD_JVC_RADIO_POWER_ON_HEX_STR,
			common_tools_string_length(CLI_REMOTE_CONTROL_CMD_JVC_RADIO_POWER_ON_HEX_STR),
			p_buffer->data,
			COMMON_TYPES_GENERIC_BUFFER_SIZE
		);

		return 1;

	} else if ( common_tools_string_compare(cmd_name, CLI_REMOTE_CONTROL_NAME_JVC_RADIO_VOLUME_DOWN) ) {

		DEBUG_PASS("command_line_handler_remote_control_is_jvc_command() - CLI_REMOTE_CONTROL_NAME_JVC_RADIO_VOLUME_DOWN");

		p_buffer->length = common_tools_hex_string_to_byte_array(
			CLI_REMOTE_CONTROL_CMD_JVC_RADIO_VOLUME_DOWN_HEX_STR,
			common_tools_string_length(CLI_REMOTE_CONTROL_CMD_JVC_RADIO_VOLUME_DOWN_HEX_STR),
			p_buffer->data,
			COMMON_TYPES_GENERIC_BUFFER_SIZE
		);

		return 1;

	} else if ( common_tools_string_compare(cmd_name, CLI_REMOTE_CONTROL_NAME_JVC_RADIO_VOLUME_UP) ) {

		DEBUG_PASS("command_line_handler_remote_control_is_jvc_command() - CLI_REMOTE_CONTROL_NAME_JVC_RADIO_VOLUME_UP");

		p_buffer->length = common_tools_hex_string_to_byte_array(
			CLI_REMOTE_CONTROL_CMD_JVC_RADIO_VOLUME_UP_HEX_STR,
			common_tools_string_length(CLI_REMOTE_CONTROL_CMD_JVC_RADIO_VOLUME_UP_HEX_STR),
			p_buffer->data,
			COMMON_TYPES_GENERIC_BUFFER_SIZE
		);

		return 1;

	} else if ( common_tools_string_compare(cmd_name, CLI_REMOTE_CONTROL_NAME_JVC_RADIO_VOLUME_MUTE) ) {

		DEBUG_PASS("command_line_handler_remote_control_is_jvc_command() - CLI_REMOTE_CONTROL_NAME_JVC_RADIO_VOLUME_MUTE");

		p_buffer->length = common_tools_hex_string_to_byte_array(
			CLI_REMOTE_CONTROL_CMD_JVC_RADIO_VOLUME_MUTE_HEX_STR,
			common_tools_string_length(CLI_REMOTE_CONTROL_CMD_JVC_RADIO_VOLUME_MUTE_HEX_STR),
			p_buffer->data,
			COMMON_TYPES_GENERIC_BUFFER_SIZE
		);

		return 1;

	} else if ( common_tools_string_compare(cmd_name, CLI_REMOTE_CONTROL_NAME_JVC_RADIO_PLAY) ) {

		DEBUG_PASS("command_line_handler_remote_control_is_jvc_command() - CLI_REMOTE_CONTROL_NAME_JVC_RADIO_PLAY");

		p_buffer->length = common_tools_hex_string_to_byte_array(
			CLI_REMOTE_CONTROL_CMD_JVC_RADIO_PLAY_HEX_STR,
			common_tools_string_length(CLI_REMOTE_CONTROL_CMD_JVC_RADIO_PLAY_HEX_STR),
			p_buffer->data,
			COMMON_TYPES_GENERIC_BUFFER_SIZE
		);

		return 1;

	} else if ( common_tools_string_compare(cmd_name, CLI_REMOTE_CONTROL_NAME_JVC_RADIO_PAUSE) ) {

		DEBUG_PASS("command_line_handler_remote_control_is_jvc_command() - CLI_REMOTE_CONTROL_NAME_JVC_RADIO_PAUSE");

		p_buffer->length = common_tools_hex_string_to_byte_array(
			CLI_REMOTE_CONTROL_CMD_JVC_RADIO_PAUSE_HEX_STR,
			common_tools_string_length(CLI_REMOTE_CONTROL_CMD_JVC_RADIO_PAUSE_HEX_STR),
			p_buffer->data,
			COMMON_TYPES_GENERIC_BUFFER_SIZE
		);

		return 1;

	} else if ( common_tools_string_compare(cmd_name, CLI_REMOTE_CONTROL_NAME_JVC_RADIO_STOP) ) {

		DEBUG_PASS("command_line_handler_remote_control_is_jvc_command() - CLI_REMOTE_CONTROL_NAME_JVC_RADIO_STOP");

		p_buffer->length = common_tools_hex_string_to_byte_array(
			CLI_REMOTE_CONTROL_CMD_JVC_RADIO_STOP_HEX_STR,
			common_tools_string_length(CLI_REMOTE_CONTROL_CMD_JVC_RADIO_STOP_HEX_STR),
			p_buffer->data,
			COMMON_TYPES_GENERIC_BUFFER_SIZE
		);

		return 1;

	} else if ( common_tools_string_compare(cmd_name, CLI_REMOTE_CONTROL_NAME_JVC_RADIO_BASS_UP) ) {

		DEBUG_PASS("command_line_handler_remote_control_is_jvc_command() - CLI_REMOTE_CONTROL_NAME_JVC_RADIO_BASS_UP");

		p_buffer->length = common_tools_hex_string_to_byte_array(
			CLI_REMOTE_CONTROL_CMD_JVC_RADIO_BASS_UP_HEX_STR,
			common_tools_string_length(CLI_REMOTE_CONTROL_CMD_JVC_RADIO_BASS_UP_HEX_STR),
			p_buffer->data,
			COMMON_TYPES_GENERIC_BUFFER_SIZE
		);

		return 1;

	} else if ( common_tools_string_compare(cmd_name, CLI_REMOTE_CONTROL_NAME_JVC_RADIO_BASS_DOWN) ) {

		DEBUG_PASS("command_line_handler_remote_control_is_jvc_command() - CLI_REMOTE_CONTROL_NAME_JVC_RADIO_BASS_DOWN");

		p_buffer->length = common_tools_hex_string_to_byte_array(
			CLI_REMOTE_CONTROL_CMD_JVC_RADIO_BASS_DOWN_HEX_STR,
			common_tools_string_length(CLI_REMOTE_CONTROL_CMD_JVC_RADIO_BASS_DOWN_HEX_STR),
			p_buffer->data,
			COMMON_TYPES_GENERIC_BUFFER_SIZE
		);

		return 1;

	} else if ( common_tools_string_compare(cmd_name, CLI_REMOTE_CONTROL_NAME_JVC_RADIO_SOUND_MODE) ) {

		DEBUG_PASS("command_line_handler_remote_control_is_jvc_command() - CLI_REMOTE_CONTROL_NAME_JVC_RADIO_SOUND_MODE");

		p_buffer->length = common_tools_hex_string_to_byte_array(
			CLI_REMOTE_CONTROL_CMD_JVC_RADIO_SOUND_MODE_HEX_STR,
			common_tools_string_length(CLI_REMOTE_CONTROL_CMD_JVC_RADIO_SOUND_MODE_HEX_STR),
			p_buffer->data,
			COMMON_TYPES_GENERIC_BUFFER_SIZE
		);

		return 1;
	}

	DEBUG_PASS("command_line_handler_remote_control_is_jvc_command() - This is not a JVC remote-control command");
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
		DEBUG_PASS("command_line_handler_command() - INVALID PARAMETER - not a multiple of 2");
		CLI_INVALID_PARAMETER_SIGNAL_send((void*)COMMAND_LINE_ARGUMENT_COMMAND);
		return;
	}

	DEBUG_TRACE_STR(parameter, "command_line_handler_remote_control() - parameter:");
	
	u8 command_available = 0;
	COMMON_GENERIC_BUFFER_TYPE buffer;

	if (command_line_handler_remote_control_is_jvc_command(parameter, &buffer)) {
		DEBUG_PASS("command_line_handler_remote_control() - this a JVC remote-control command");
		command_available = 1;
	}

	if (command_available) {
		RPI_HOST_COMMAND_RECEIVED_SIGNAL_send(&buffer);
	}
}
