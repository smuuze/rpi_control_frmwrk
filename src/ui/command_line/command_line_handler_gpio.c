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
 * @file    command_line_handler_gpio.c
 * @author  Sebastian Lesse
 * @date    2022 / 05 / 19
 * @brief   Implements the command liena rgument handler of the -gpio argument
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

#include "signal_slot_interface.h"
#include "command_line_handler_gpio.h"
#include "command_line_interface.h"

#include "common/common_tools_string.h"

// --------------------------------------------------------------------------------

#define CLI_HANDLER_GPIO_ARGUMENT_SPLITTER              ':'
#define CLI_HANDLER_GPIO_OP_STRING_MAX_LEN              10

#define CLI_HANDLER_GPIO_OP_STRING_READ_UPPER_CASE      "READ"
#define CLI_HANDLER_GPIO_OP_STRING_READ_LOWER_CASE      "read"

#define CLI_HANDLER_GPIO_OP_STRING_WRITE_UPPER_CASE     "WRITE"
#define CLI_HANDLER_GPIO_OP_STRING_WRITE_LOWER_CASE     "write"

#define CLI_HANDLER_GPIO_OP_STRING_ALL_UPPER_CASE       "ALL"
#define CLI_HANDLER_GPIO_OP_STRING_ALL_LOWER_CASE       "all"

#define CLI_HANDLER_GPIO_MODE_STRING_OUT_UPPER_CASE     "OUT"
#define CLI_HANDLER_GPIO_MODE_STRING_OUT_LOWER_CASE     "out"

#define CLI_HANDLER_GPIO_MODE_STRING_IN_UPPER_CASE      "IN"
#define CLI_HANDLER_GPIO_MODE_STRING_IN_LOWER_CASE      "in"

#define CLI_HANDLER_GPIO_MODE_STRING_HIGH_UPPER_CASE    "HIGH"
#define CLI_HANDLER_GPIO_MODE_STRING_HIGH_LOWER_CASE    "high"

#define CLI_HANDLER_GPIO_MODE_STRING_LOW_UPPER_CASE     "LOW"
#define CLI_HANDLER_GPIO_MODE_STRING_LOW_LOWER_CASE     "low"

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CLI_ARGUMENT_GPIO_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CLI_INVALID_PARAMETER_SIGNAL)

// --------------------------------------------------------------------------------

/**
 * @brief Makes the given cli command invalid by setting all values to 255
 * 
 * @param p_argument gpio-command to ake invalid
 */
static void command_line_handler_set_invalid(GPIO_CLI_ARGUMENT* p_argument) {
    p_argument->operation = CLI_HANDLER_OP_INVALID;
    p_argument->pin = CLI_HANDLER_GPIO_PIN_INVALID;
    p_argument->mode = CLI_HANDLER_GPIO_DIRECTION_INVALID;
    p_argument->level = CLI_HANDLER_GPIO_LEVEL_INVALID;
}

// --------------------------------------------------------------------------------

/**
 * @see ui/command_line/command_line_handler_gpio.h#command_line_handler_gpio
 */
void command_line_handler_gpio(const char* argument) {

    if (argument == NULL) {
        DEBUG_PASS("command_line_handler_gpio() - argument is NULL");
        CLI_INVALID_PARAMETER_SIGNAL_send((void*)COMMAND_LINE_ARGUMENT_GPIO);
    }

    GPIO_CLI_ARGUMENT gpio_cmd;
    command_line_handler_set_invalid(&gpio_cmd);

    DEBUG_TRACE_STR(argument, "command_line_handler_gpio() - argument:");

    /**
     * @brief format of argumetn is: 
     * 
     * <OP>:<NUM>[:<MODE>:<LEVEL>]
     * 
     * <OP>     gpio operation READ, WRITE (case insensitive)
     * <NUM>    gpio pin number
     * <MODE>   mode the gpio is set to, only needed if <OP>=WRITE
     * <LEVEL>  level the gpio will be set to HIGH or LOW (case insensitive)
     * 
     */

    u16 index = 0;

    // get operation
    if (common_tools_string_is_in_front(&argument[index], CLI_HANDLER_GPIO_OP_STRING_READ_UPPER_CASE)
            || common_tools_string_is_in_front(&argument[index], CLI_HANDLER_GPIO_OP_STRING_READ_LOWER_CASE)) {

        DEBUG_PASS("command_line_handler_gpio() - is read operation");

        index += 5; // length of string "read:"
        gpio_cmd.operation = CLI_HANDLER_OP_READ;

    } else if (common_tools_string_is_in_front(&argument[index], CLI_HANDLER_GPIO_OP_STRING_WRITE_UPPER_CASE)
            || common_tools_string_is_in_front(&argument[index], CLI_HANDLER_GPIO_OP_STRING_WRITE_LOWER_CASE)) {

        DEBUG_PASS("command_line_handler_gpio() - is write operation");
        
        index += 6; // length of string "write:"
        gpio_cmd.operation = CLI_HANDLER_OP_WRITE;

    } else {

        DEBUG_PASS("command_line_handler_gpio() - invalid operation");
        CLI_INVALID_PARAMETER_SIGNAL_send((void*)COMMAND_LINE_ARGUMENT_GPIO);
        return;
    }

    // check if we need to read all pins
    if (gpio_cmd.operation == CLI_HANDLER_OP_READ) {
        if (common_tools_string_is_in_front(&argument[index], CLI_HANDLER_GPIO_OP_STRING_ALL_UPPER_CASE)
            || common_tools_string_is_in_front(&argument[index], CLI_HANDLER_GPIO_OP_STRING_ALL_LOWER_CASE)) {

            DEBUG_PASS("command_line_handler_gpio() - read all pins");
            gpio_cmd.pin = CLI_HANDLER_GPIO_PIN_ALL;
            
            CLI_ARGUMENT_GPIO_SIGNAL_send((void*)&gpio_cmd);
            return;
        }
    }

    // skip leading zeros
    for (u8 length = common_tools_string_length(argument); index < length; index++) {
        if (argument[index] != '0') {
            break;
        }
    }

    // get the pin number
    char number_str[] = {0,0,0,0};
    u8 length = common_tools_string_split(
        CLI_HANDLER_GPIO_ARGUMENT_SPLITTER,
        &argument[index],
        number_str,
        sizeof(number_str),
        NULL,
        0
    );

    if (common_tools_sting_is_number(number_str) == 0) {
        DEBUG_PASS("command_line_handler_gpio() - invalid pin-number");
        CLI_INVALID_PARAMETER_SIGNAL_send((void*)COMMAND_LINE_ARGUMENT_GPIO);
        return;
    }

    gpio_cmd.pin = common_tools_to_u8(number_str);
    index += length + 1; // +1 because of splitter
    
    DEBUG_TRACE_byte(gpio_cmd.pin, "command_line_handler_gpio() - pin-num:");

    // if this is a read-command we don't care for direction and level
    if (gpio_cmd.operation == CLI_HANDLER_OP_READ) {
        CLI_ARGUMENT_GPIO_SIGNAL_send((void*)&gpio_cmd);
        return;
    }

    // get the mode
    if (common_tools_string_is_in_front(&argument[index], CLI_HANDLER_GPIO_MODE_STRING_OUT_UPPER_CASE)
            || common_tools_string_is_in_front(&argument[index], CLI_HANDLER_GPIO_MODE_STRING_OUT_LOWER_CASE)) {

        DEBUG_PASS("command_line_handler_gpio() - mode is out");

        index += 4; // length of string "out:"
        gpio_cmd.mode = CLI_HANDLER_GPIO_DIRECTION_OUT;

    } else if (common_tools_string_is_in_front(&argument[index], CLI_HANDLER_GPIO_MODE_STRING_IN_UPPER_CASE)
            || common_tools_string_is_in_front(&argument[index], CLI_HANDLER_GPIO_MODE_STRING_IN_LOWER_CASE)) {

        DEBUG_PASS("command_line_handler_gpio() - mode is in");
        
        index += 3; // length of string "in:"
        gpio_cmd.mode = CLI_HANDLER_GPIO_DIRECTION_IN;

    } else {

        DEBUG_PASS("command_line_handler_gpio() - invalid mode");
        CLI_INVALID_PARAMETER_SIGNAL_send((void*)COMMAND_LINE_ARGUMENT_GPIO);
        return;
    }

    // get the level
    if (common_tools_string_is_in_front(&argument[index], CLI_HANDLER_GPIO_MODE_STRING_HIGH_UPPER_CASE)
            || common_tools_string_is_in_front(&argument[index], CLI_HANDLER_GPIO_MODE_STRING_HIGH_LOWER_CASE)) {

        DEBUG_PASS("command_line_handler_gpio() - level is high");

        gpio_cmd.level = CLI_HANDLER_GPIO_LEVEL_HIGH;

    } else if (common_tools_string_is_in_front(&argument[index], CLI_HANDLER_GPIO_MODE_STRING_LOW_UPPER_CASE)
            || common_tools_string_is_in_front(&argument[index], CLI_HANDLER_GPIO_MODE_STRING_LOW_LOWER_CASE)) {

        DEBUG_PASS("command_line_handler_gpio() - level is low");
        
        gpio_cmd.level = CLI_HANDLER_GPIO_LEVEL_LOW;

    } else {

        DEBUG_PASS("command_line_handler_gpio() - invalid level");
        CLI_INVALID_PARAMETER_SIGNAL_send((void*)COMMAND_LINE_ARGUMENT_GPIO);
        return;
    }

    CLI_ARGUMENT_GPIO_SIGNAL_send((void*)&gpio_cmd);

}

// --------------------------------------------------------------------------------
