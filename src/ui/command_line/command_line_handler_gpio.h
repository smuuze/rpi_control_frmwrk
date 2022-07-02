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
 * @file    command_line_handler_gpio.h
 * @author  Sebastian Lesse
 * @date    2022 / 05 / 19
 * @brief   Definitions of command-line hlper to realize
 *          gpio arguments for number, level and mode
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_command_line_helper_gpio_
#define _H_command_line_helper_gpio_

// --------------------------------------------------------------------------------

#include "cpu.h"

// --------------------------------------------------------------------------------

#define CLI_HANDLER_OP_READ                                             1
#define CLI_HANDLER_OP_WRITE                                            2
#define CLI_HANDLER_OP_INVALID                                          255

#define CLI_HANDLER_GPIO_DIRECTION_OUT                                  1
#define CLI_HANDLER_GPIO_DIRECTION_IN                                   2
#define CLI_HANDLER_GPIO_DIRECTION_INVALID                              255

#define CLI_HANDLER_GPIO_LEVEL_HIGH                                     1
#define CLI_HANDLER_GPIO_LEVEL_LOW                                      2
#define CLI_HANDLER_GPIO_LEVEL_INVALID                                  255

#define CLI_HANDLER_GPIO_PIN_ALL                                        254
#define CLI_HANDLER_GPIO_PIN_INVALID                                    255

// --------------------------------------------------------------------------------

/**
 * @brief Structure to transfer aparsed gpio cli command
 * 
 */
typedef struct GPIO_CLI_ARGUMENT_STRUCT {
    
    /**
     * @brief mode of pgio operation.
     * 0: read operation
     * 1: write operation
     * 
     */
    u8 operation;

    /**
     * @brief number of the gpio-pin of interest
     * 
     */
    u8 pin;

    /**
     * @brief Mode the gpio pin will have after operation
     * 0: set to input
     * 1: set to output
     * 
     */
    u8 mode;

    /**
     * @brief Level the gpio will have after operation
     * 0: LOW
     * 1: HIGH
     * 
     */
    u8 level;

} GPIO_CLI_ARGUMENT;

// --------------------------------------------------------------------------------

/**
 * @brief Processes the -gpio argument
 * in the form <program> -gpio <OP>:<NUM>[:<MODE>:<LEVEL>]
 * 
 * <OP>     gpio operation READ, WRITE (case insensitive)
 * <NUM>    gpio pin number
 * <MODE>   mode the gpio is set to, only needed if <OP>=WRITE
 * <LEVEL>  level the gpio will be set to HIGH or LOW (case insensitive)
 * 
 * This function will parse the given argument string and fill a instance
 * of GPIO_CLI_ARGUMENT. This instance will be send via the XXX_SIGNAL.
 * 
 * If the argument cannot be parsed successful CLI_INVALID_PARAMETER_SIGNAL
 * is send.
 * 
 * @param argument the decimal number of the gpio pin of intereset as string
 */
void command_line_handler_gpio(const char* argument);

// --------------------------------------------------------------------------------

#endif // _H_command_line_helper_gpio_

// --------------------------------------------------------------------------------
