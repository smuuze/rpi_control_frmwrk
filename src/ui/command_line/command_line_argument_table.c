/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	command_line_argument_table.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

#ifdef TRACER_ON
#pragma __WARNING__TRACES_ENABLED__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include <string.h>

// --------------------------------------------------------------------------------

#include "common/signal_slot_interface.h"
#include "ui/command_line/command_line_interface.h"
#include "common/common_tools_string.h"

// --------------------------------------------------------------------------------

#include "ui/command_line/command_line_handler_remote_control.h"

// --------------------------------------------------------------------------------

/* */
void command_line_handler_device(const char* parameter);

/* */
void command_line_handler_cfg_file(const char* parameter);

/* */
void command_line_handler_lcd(const char* parameter);

/* */
void command_line_handler_command(const char* parameter);

/* */
void command_line_handler_controller(const char* parameter);

/* */
void command_line_handler_help(const char* parameter);

/* */
void command_line_handler_topic(const char* parameter);

/* */
void command_line_handler_host(const char* parameter);

/* */
void command_line_handler_client(const char* parameter);

/* */
void command_line_handler_message(const char* parameter);

/* */
void command_line_handler_console(const char* parameter);

/* */
void command_line_handler_file(const char* parameter);

/* */
void command_line_handler_path(const char* parameter);

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_INVALID_ARGUMENT_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_INVALID_PARAMETER_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_HELP_REQUESTED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_LCD_ACTIVATED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_CONSOLE_ACTIVATED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_ARGUMENT_FILE_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_ARGUMENT_PATH_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_ARGUMENT_DEVICE_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_CONFIGURATION_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_MESSAGE_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_NO_ARGUMENT_GIVEN_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_UNKNOWN_ARGUMENT_SIGNAL)

// --------------------------------------------------------------------------------

void command_line_handler_unknown_argument(const char* argument) {

	DEBUG_TRACE_STR(argument, "command_line_handler_unknown_argument() - UNKNOWN CLI-ARGUMENT");
		
	char no_option[] = "\0";
	
	COMMAND_LINE_ARGUMENT_TYPE unknown_argument = {
		.argument = argument,
		.option = no_option
	};

	CLI_UNKNOWN_ARGUMENT_SIGNAL_send(&unknown_argument);
}

// --------------------------------------------------------------------------------

static COMMAND_LINE_ARGUMENT_TABLE_TYPE command_line_argument_table[] = {
	{ COMMAND_LINE_ARGUMENT_CFG_FILE, &command_line_handler_cfg_file },
	{ COMMAND_LINE_ARGUMENT_LCD, &command_line_handler_lcd },
	{ COMMAND_LINE_ARGUMENT_CONTROLLER, &command_line_handler_controller },
	{ COMMAND_LINE_ARGUMENT_DEVICE, &command_line_handler_device },
	{ COMMAND_LINE_ARGUMENT_COMMAND, &command_line_handler_command },
	{ COMMAND_LINE_ARGUMENT_MESSAGE, &command_line_handler_message },
	{ COMMAND_LINE_ARGUMENT_TOPIC, &command_line_handler_topic },
	{ COMMAND_LINE_ARGUMENT_HOST, &command_line_handler_host },
	{ COMMAND_LINE_ARGUMENT_CLIENT, &command_line_handler_client },
	{ COMMAND_LINE_ARGUMENT_HELP, &command_line_handler_help },
	{ COMMAND_LINE_ARGUMENT_HELP_SHORT, &command_line_handler_help },
	{ COMMAND_LINE_ARGUMENT_CONSOLE, &command_line_handler_console },
	{ COMMAND_LINE_ARGUMENT_FILE, &command_line_handler_file },
	{ COMMAND_LINE_ARGUMENT_PATH, &command_line_handler_path },

	#ifdef CLI_REMOTE_CONTROL_ARGUMENT_AVAILABLE
	{ COMMAND_LINE_ARGUMENT_REMOTE, &command_line_handler_remote_control },
	#endif
	
	{ COMMAND_LINE_ARGUMENT_UNKNWON, &command_line_handler_unknown_argument }
};

#define COMMAND_LINE_INTERFACE_HANDLER_TABLE_SIZE			0xFF

// --------------------------------------------------------------------------------

void command_line_interface_init(void) {

	DEBUG_PASS("command_line_interface_init() - CLI_INVALID_ARGUMENT_SIGNAL_init()");
	CLI_INVALID_ARGUMENT_SIGNAL_init();

	DEBUG_PASS("command_line_interface_init() - CLI_HELP_REQUESTED_SIGNAL_init()");
	CLI_HELP_REQUESTED_SIGNAL_init();

	DEBUG_PASS("command_line_interface_init() - CLI_LCD_ACTIVATED_SIGNAL_init()");
	CLI_LCD_ACTIVATED_SIGNAL_init();

	DEBUG_PASS("command_line_interface_init() - CLI_CONSOLE_ACTIVATED_SIGNAL_init()");
	CLI_CONSOLE_ACTIVATED_SIGNAL_init();

	DEBUG_PASS("command_line_interface_init() - CLI_CONFIGURATION_SIGNAL_init()");
	CLI_CONFIGURATION_SIGNAL_init();

	DEBUG_PASS("command_line_interface_init() - CLI_MESSAGE_SIGNALinit()");
	CLI_MESSAGE_SIGNAL_init();

	DEBUG_PASS("command_line_interface_init() - CLI_UNKNOWN_ARGUMENT_SIGNAL_init()");
	CLI_UNKNOWN_ARGUMENT_SIGNAL_init();

	DEBUG_PASS("command_line_interface_init() - CLI_NO_ARGUMENT_GIVEN_SIGNAL_init()");
	CLI_NO_ARGUMENT_GIVEN_SIGNAL_init();

	DEBUG_PASS("command_line_interface_init() - CLI_ARGUMENT_FILE_SIGNAL_init()");
	CLI_ARGUMENT_FILE_SIGNAL_init();

	DEBUG_PASS("command_line_interface_init() - CLI_ARGUMENT_PATH_SIGNAL_init()");
	CLI_ARGUMENT_PATH_SIGNAL_init();

	DEBUG_PASS("command_line_interface_init() - CLI_ARGUMENT_DEVICE_SIGNAL_init()");
	CLI_ARGUMENT_DEVICE_SIGNAL_init();
}

void command_line_interface(int argc, char* argv[]) {

	DEBUG_TRACE_byte((u8)argc, "command_line_interface() - number of arguments: ");

	if (argc <= 1) {
		CLI_NO_ARGUMENT_GIVEN_SIGNAL_send(NULL);
		return;
	}

	// loop through all given arguments
	for (u8 j = 1; j < argc; j++) {

		if (common_tools_string_starts_with(argv[j], COMMAND_LINE_ARGUMENT_PREFIX_CHAR) == 0) {
			DEBUG_TRACE_STR(argv[j], "command_line_interface() - Not a cli-argument: ");
			continue;
		}

		DEBUG_TRACE_STR(argv[j], "command_line_interface() - Parsing Argument: ");

		// loop through all known options for every given arguments
		for ( u8 i = 0 ; i < COMMAND_LINE_INTERFACE_HANDLER_TABLE_SIZE; i++) {

			if ( common_tools_string_compare(command_line_argument_table[i].argument, COMMAND_LINE_ARGUMENT_UNKNWON) ) {
				command_line_handler_unknown_argument(argv[j]);
				break;
			}

			if ( common_tools_string_compare(argv[j], command_line_argument_table[i].argument) ) {

				//DEBUG_PASS("command_line_interface() - Argument is key");

				if (j == argc - 1) {
					command_line_argument_table[i].callback(NULL);
				} else {
					command_line_argument_table[i].callback(argv[j+1]);
				}

				break;
			}
		}
	}
}
