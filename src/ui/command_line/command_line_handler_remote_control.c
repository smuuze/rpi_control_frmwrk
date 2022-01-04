/**
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * @file   command_line_handler_remote_control.c
 * @author Sebastian Lesse
 * @date   2022 / 05 / 23
 * @brief  Handles the -remote command-line argument
 * 
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

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
 * @brief Table for all ir commands and it's HEX-Strings
 * 
 */
static CLI_REMOTE_CONTROL_COMMAND_PAIR cmd_array[] = { CLI_REMOTE_CONTROL_CMD_ARRAY_ALL };

// --------------------------------------------------------------------------------

/**
 * @brief Checks if the given cli-parameter is a valid ir-command.
 * 
 * @param cmd_name name of the ir-command to look for
 * @param p_cmd_table table of all available ir-commands where to search for the given cmd-name
 * @param table_length number of elements inside of cmd_table
 * @param p_buffer destination where the hex-string of the requested ir-command is copied into
 * @return 1 iof cmd_name was found, otherwise 0
 */
static u8 command_line_handler_remote_control_get_command(const char* cmd_name, CLI_REMOTE_CONTROL_COMMAND_PAIR* p_cmd_table, u16 table_length, COMMON_GENERIC_BUFFER_TYPE* p_buffer) {

    DEBUG_TRACE_word(table_length, "command_line_handler_remote_control_get_command() - Table-Size:");

    u16 i = 0;
    for ( ; i < table_length ; i++ ) {

        if ( common_tools_string_compare(p_cmd_table[i].cmd_name, cmd_name) ) {

            DEBUG_TRACE_STR(p_cmd_table[i].cmd_name, "command_line_handler_remote_control_get_command() - Command Found");

            p_buffer->length = common_tools_hex_string_to_byte_array(
                p_cmd_table[i].cmd_hex_str,
                CLI_REMOTE_CONTROL_CMD_HEX_STR_LENGTH, // always has the same length for every command
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
        CLI_INVALID_PARAMETER_SIGNAL_send((void*)COMMAND_LINE_ARGUMENT_REMOTE);
        return;
    }

    if (common_tools_string_length(parameter) == 0) {
        DEBUG_PASS("command_line_handler_command() - INVALID PARAMETER - parameter has length of zero");
        CLI_INVALID_PARAMETER_SIGNAL_send((void*)COMMAND_LINE_ARGUMENT_REMOTE);
        return;
    }

    DEBUG_TRACE_STR(parameter, "command_line_handler_remote_control() - parameter:");
    
    COMMON_GENERIC_BUFFER_TYPE buffer;

    if ( command_line_handler_remote_control_get_command(parameter, cmd_array, CLI_REMOTE_SIZEOF_COMMAND_PAIR_TABLE(cmd_array), &buffer) ) {

        DEBUG_PASS("command_line_handler_remote_control() - this a valid remote-control command");
        RPI_HOST_COMMAND_RECEIVED_SIGNAL_send(&buffer);

    } else {

        DEBUG_PASS("command_line_handler_remote_control() - this a invalid remote-control command");
        CLI_INVALID_PARAMETER_SIGNAL_send((void*)COMMAND_LINE_ARGUMENT_REMOTE);
    }
}
