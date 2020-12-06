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

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include <string.h>

// --------------------------------------------------------------------------------

#include "common/signal_slot_interface.h"
#include "ui/command_line/command_line_interface.h"

// --------------------------------------------------------------------------------

typedef void (*COMMAND_LINE_ARGUMENT_PARSER_CALLBACK)	(const char* parameter);

// --------------------------------------------------------------------------------

typedef struct {
	const char* argument;
	COMMAND_LINE_ARGUMENT_PARSER_CALLBACK callback;
} COMMAND_LINE_ARGUMENT_TABLE_TYPE;

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
	{ COMMAND_LINE_ARGUMENT_CONSOLE, &command_line_handler_console }
	
};

#define COMMAND_LINE_INTERFACE_HANDLER_TABLE_SIZE			12

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_INVALID_ARGUMENT_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_INVALID_PARAMETER_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_HELP_REQUESTED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_LCD_ACTIVATED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_CONSOLE_ACTIVATED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_CONFIGURATION_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_MESSAGE_SIGNAL)

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
}

void command_line_interface(int argc, char* argv[]) {

	DEBUG_TRACE_byte((u8)argc, "command_line_interface() - number of argumetns: ");

	for (u8 j = 0; j < argc; j++) {

		DEBUG_TRACE_STR(argv[j], "command_line_interface() - Parsing Argument: ");

		for ( u8 i = 0 ; i < COMMAND_LINE_INTERFACE_HANDLER_TABLE_SIZE; i++) {

			//DEBUG_TRACE_STR(command_line_argument_table[i].argument, "command_line_interface() - Table-Argument to match: ");

			u8 length_cli_argument = strlen(argv[j]);
			u8 length_table_argument = strlen(command_line_argument_table[i].argument);

			if (length_cli_argument != length_table_argument) {
				continue;
			}

			if (memcmp(argv[j], command_line_argument_table[i].argument, length_table_argument) == 0) {

				//DEBUG_PASS("command_line_interface() - Argument is key");

				if (j == argc - 1) {
					command_line_argument_table[i].callback(NULL);
				} else {
					command_line_argument_table[i].callback(argv[j+1]);
				}
			}
		}
	}
}