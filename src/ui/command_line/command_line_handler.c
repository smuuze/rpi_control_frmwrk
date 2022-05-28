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
 * @file   command_line_handler.c
 * @author Sebastian Lesse
 * @date   2022 / 01 / 01
 * @brief  Implementation of common command line argument handler
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

#include "cpu.h"

// --------------------------------------------------------------------------------

#include <string.h>

// --------------------------------------------------------------------------------

#include "common/signal_slot_interface.h"
#include "ui/command_line/command_line_interface.h"
#include "common/common_tools_string.h"

#include "ui/cfg_file_parser/cfg_file_parser.h"

#ifdef HAS_PROTOCOL_MQTT
#include "protocol_management/mqtt/mqtt_interface.h"
#endif

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CLI_INVALID_ARGUMENT_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CLI_INVALID_PARAMETER_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CLI_HELP_REQUESTED_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CLI_LCD_ACTIVATED_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CLI_CONSOLE_ACTIVATED_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CLI_CONFIGURATION_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CLI_ARGUMENT_FILE_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CLI_ARGUMENT_PATH_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CLI_ARGUMENT_DEVICE_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CLI_ARGUMENT_N_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CLI_ARGUMENT_ADDR_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CLI_MESSAGE_SIGNAL)

SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(RPI_HOST_COMMAND_RECEIVED_SIGNAL)

// --------------------------------------------------------------------------------

void command_line_handler_device(const char* parameter) {
    DEBUG_PASS("command_line_handler_device()");
    CLI_ARGUMENT_DEVICE_SIGNAL_send((const void*)parameter);
}

// --------------------------------------------------------------------------------

void command_line_handler_cfg_file(const char* parameter) {
    DEBUG_PASS("command_line_handler_cfg_file()");
    CLI_CONFIGURATION_SIGNAL_send((void*)parameter);
}

// --------------------------------------------------------------------------------

void command_line_handler_lcd(const char* parameter) {
    DEBUG_PASS("command_line_handler_lcd()");
    CLI_LCD_ACTIVATED_SIGNAL_send(NULL);
}

// --------------------------------------------------------------------------------

void command_line_handler_console(const char* parameter) {
    DEBUG_PASS("command_line_handler_console()");
    CLI_CONSOLE_ACTIVATED_SIGNAL_send(NULL);
}

// --------------------------------------------------------------------------------

void command_line_handler_file(const char* parameter) {
    DEBUG_PASS("command_line_handler_file()");
    CLI_ARGUMENT_FILE_SIGNAL_send((const void*) parameter);
}

// --------------------------------------------------------------------------------

void command_line_handler_path(const char* parameter) {
    DEBUG_PASS("command_line_handler_path()");
    CLI_ARGUMENT_PATH_SIGNAL_send((const void*) parameter);
}

// --------------------------------------------------------------------------------

void command_line_handler_command(const char* parameter) {

    if (parameter == NULL) {
        DEBUG_PASS("command_line_handler_command() - INVALID PARAMETER - parameter is NULL");
        CLI_INVALID_PARAMETER_SIGNAL_send((void*)COMMAND_LINE_ARGUMENT_COMMAND);
        return;
    }

    if (strlen(parameter) % 2) {
        DEBUG_PASS("command_line_handler_command() - INVALID PARAMETER - not a multiple of 2");
        CLI_INVALID_PARAMETER_SIGNAL_send((void*)COMMAND_LINE_ARGUMENT_COMMAND);
        return;
    }

    if (strlen(parameter) / 2 > COMMON_TYPES_GENERIC_BUFFER_SIZE) {
        DEBUG_PASS("command_line_handler_command() - INVALID PARAMETER - parameter too long");
        CLI_INVALID_PARAMETER_SIGNAL_send((void*)COMMAND_LINE_ARGUMENT_COMMAND);
        return;
    }

    DEBUG_TRACE_STR(parameter, "command_line_handler_command()");

    #ifdef HAS_MANAGEMENT_MODULE_RPI_PROTOCOL
    {
        COMMON_GENERIC_BUFFER_TYPE buffer;

        buffer.length = common_tools_hex_string_to_byte_array(parameter, strlen(parameter), buffer.data, COMMON_TYPES_GENERIC_BUFFER_SIZE);

        RPI_HOST_COMMAND_RECEIVED_SIGNAL_send(&buffer);
    }
    #endif

    DEBUG_PASS("command_line_handler_command() - Done");
}

void command_line_handler_controller(const char* parameter) {
    DEBUG_PASS("command_line_handler_command()");
}

// --------------------------------------------------------------------------------

void command_line_handler_help(const char* parameter) {
    DEBUG_PASS("command_line_handler_help()");
    CLI_HELP_REQUESTED_SIGNAL_send(NULL);
}

// --------------------------------------------------------------------------------

#ifdef HAS_PROTOCOL_MQTT
void command_line_handler_topic(const char* parameter) {
    DEBUG_PASS("command_line_handler_topic()");

    CFG_FILE_PARSER_CFG_OBJECT_TYPE cfg_obj = {
        .key = MQTT_TOPIC_CFG_STRING
    };

    memset(cfg_obj.value, '\0', CFG_FILE_PARSER_MAX_LENGTH_PATH);

    u16 length = strlen(parameter);

    if (length > CFG_FILE_PARSER_MAX_LENGTH_PATH - 1) {
        length = CFG_FILE_PARSER_MAX_LENGTH_PATH - 1;
    }

    memcpy(cfg_obj.value, parameter, length);

    CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send(&cfg_obj);
}
#endif // #ifdef HAS_PROTOCOL_MQTT

// --------------------------------------------------------------------------------

#ifdef HAS_PROTOCOL_MQTT
void command_line_handler_host(const char* parameter) {
    DEBUG_TRACE_STR(parameter, "command_line_handler_host() - ");

    CFG_FILE_PARSER_CFG_OBJECT_TYPE cfg_obj = {
        .key = MQTT_HOST_CFG_STRING
    };

    memset(cfg_obj.value, '\0', CFG_FILE_PARSER_MAX_LENGTH_PATH);

    u16 length = strlen(parameter);

    if (length > CFG_FILE_PARSER_MAX_LENGTH_PATH - 1) {
        length = CFG_FILE_PARSER_MAX_LENGTH_PATH - 1;
    }

    memcpy(cfg_obj.value, parameter, length);

    CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send(&cfg_obj);
}
#endif // #ifdef HAS_PROTOCOL_MQTT

// --------------------------------------------------------------------------------

#ifdef HAS_PROTOCOL_MQTT
void command_line_handler_client(const char* parameter) {
    DEBUG_TRACE_STR(parameter, "command_line_handler_client() - ");

    CFG_FILE_PARSER_CFG_OBJECT_TYPE cfg_obj = {
        .key = MQTT_CLIENT_ID_CFG_STRING
    };

    memset(cfg_obj.value, '\0', CFG_FILE_PARSER_MAX_LENGTH_PATH);

    u16 length = strlen(parameter);

    if (length > CFG_FILE_PARSER_MAX_LENGTH_PATH - 1) {
        length = CFG_FILE_PARSER_MAX_LENGTH_PATH - 1;
    }

    memcpy(cfg_obj.value, parameter, length);

    CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send(&cfg_obj);
}
#endif // #ifdef HAS_PROTOCOL_MQTT

// --------------------------------------------------------------------------------

void command_line_handler_message(const char* parameter) {
    DEBUG_TRACE_STR(parameter, "command_line_handler_message() - ");
    CLI_MESSAGE_SIGNAL_send(parameter);
}

// --------------------------------------------------------------------------------

void command_line_handler_n(const char* parameter) {

    if (parameter == NULL) {
        DEBUG_PASS("command_line_handler_n() - INVALID PARAMETER - parameter is NULL");
        CLI_INVALID_PARAMETER_SIGNAL_send((void*)COMMAND_LINE_ARGUMENT_N);
        return;
    }

    if (common_tools_sting_is_number(parameter) == 0) {
        DEBUG_PASS("command_line_handler_n() - INVALID PARAMETER - parameter is not a number");
        CLI_INVALID_PARAMETER_SIGNAL_send((void*)COMMAND_LINE_ARGUMENT_N);
        return;
    }

    DEBUG_TRACE_STR(parameter, "command_line_handler_n() - ");

    u32 n = common_tools_string_to_u32(parameter);
    CLI_ARGUMENT_N_SIGNAL_send(&n);
}

// --------------------------------------------------------------------------------

void command_line_handler_addr(const char* parameter) {

    if (parameter == NULL) {
        DEBUG_PASS("command_line_handler_addr() - INVALID PARAMETER - parameter is NULL");
        CLI_INVALID_PARAMETER_SIGNAL_send((void*)COMMAND_LINE_ARUGMENT_ADDR);
        return;
    }

    if (common_tools_sting_is_number(parameter) == 0) {
        DEBUG_PASS("command_line_handler_addr() - INVALID PARAMETER - parameter is not a number");
        CLI_INVALID_PARAMETER_SIGNAL_send((void*)COMMAND_LINE_ARUGMENT_ADDR);
        return;
    }

    DEBUG_TRACE_STR(parameter, "command_line_handler_addr() - ");

    u32 addr = common_tools_string_to_u32(parameter);
    CLI_ARGUMENT_ADDR_SIGNAL_send(&addr);
}




// --------------------------------------------------------------------------------
