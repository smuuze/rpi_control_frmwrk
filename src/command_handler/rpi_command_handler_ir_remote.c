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
 * @file   rpi_command_handler_ir_remote.c
 * @author Sebastian Lesse
 * @date   2020 / 03 / 31
 * @brief  Command handling module for ir-control-commands
 * 
 */

// --------------------------------------------------------------------------------

#define TRACER_OFF

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

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

// --------------------------------------------------------------------------------

#include "command_handler/rpi_command_handler_ir_remote.h"
#include "command_handler/rpi_cmd_handler_ir_remote_samsung.h"
#include "command_handler/rpi_cmd_handler_ir_remote_jvc.h"
#include "command_handler/rpi_cmd_handler_ir_remote_sony.h"
#include "command_handler/rpi_cmd_handler_ir_remote_led_lights.h"

// --------------------------------------------------------------------------------

/**
 * @brief Pointer type of a ir-command handler callback
 * 
 */
typedef u8 (*IR_CMD_HANDLER_CALLBACK)         (u8, u8);

/**
 * @brief Structure of a single command-handler table entry
 * 
 */
typedef struct IR_COMMAND_TABLE {

    /**
     * @brief id of the ir-protocol to use
     * 
     */
    u8 protocol_id;

    /**
     * @brief Callback to execute on matching ir-command id
     * 
     */
    IR_CMD_HANDLER_CALLBACK p_callback;
} IR_COMMAND_TABLE;

#define IR_CMD_HANDLER_MAX_SIZE_TABLE 255

// --------------------------------------------------------------------------------

/**
 * @brief Default ir-command handler if a unknown manufactor was given
 * 
 * @return CMD_ERR_INVALID_ARGUMENT
 */
static u8 rpi_cmd_handler_ir_invalid_argument(u8 device, u8 command) {
    (void) device;
    (void) command;
    return CMD_ERR_INVALID_ARGUMENT;
}

// --------------------------------------------------------------------------------

/**
 * @brief table of available ir-commands
 * 
 */
static IR_COMMAND_TABLE ir_command_table[] = {

    #ifdef RPI_CMD_HANDLER_IR_REMOTE_SAMSUNG_AVAILABLE
    {IR_MANUFACTOR_SAMSUNG, &rpi_cmd_handler_ir_remote_samsung},
    #endif

    #ifdef RPI_CMD_HANDLER_IR_REMOTE_JVC_AVAILABLE
    {IR_MANUFACTOR_JVC, &rpi_cmd_handler_ir_remote_jvc},
    #endif

    #ifdef RPI_CMD_HANDLER_IR_REMOTE_SONY_AVAILABLE
    {IR_MANUFACTOR_SONY, &rpi_cmd_handler_ir_remote_sony},
    #endif

    #ifdef RPI_CMD_HANDLER_IR_REMOTE_LED_LIGHTS_AVAILABLE
    {IR_MANUFACTOR_LED_LIGHTS, &rpi_cmd_handler_ir_remote_led_lights},
    #endif

    {0x00, &rpi_cmd_handler_ir_invalid_argument} // must always be the last one
};

// --------------------------------------------------------------------------------

u8 rpi_cmd_handler_ir_remote(PROTOCOL_INTERFACE* p_protocol) {

    if (p_protocol->cmd_buffer->bytes_available() == 0) {

        DEBUG_PASS("rpi_cmd_handler_ir_remote() - No Argument given !!! ---");

        p_protocol->set_finished(CMD_ERR_INVALID_COMMAND);
        return CMD_ERR_INVALID_COMMAND;
    }

    DEBUG_PASS("rpi_cmd_handler_ir_remote()");

    p_protocol->cmd_buffer->start_read();

    u8 manufactor = p_protocol->cmd_buffer->get_byte();    // samsung / jvc / sony / ...
    u8 device     = p_protocol->cmd_buffer->get_byte();    // tv / radio / blueray-player / ..
    u8 command    = p_protocol->cmd_buffer->get_byte();    // on/off, vol_up/vol_down, mute, ...

    DEBUG_TRACE_byte(manufactor, "rpi_cmd_handler_ir_remote() - Manufactor:");
    DEBUG_TRACE_byte(device,     "rpi_cmd_handler_ir_remote() - Device:");
    DEBUG_TRACE_byte(command,    "rpi_cmd_handler_ir_remote() - Command:");

    p_protocol->cmd_buffer->stop_read();

    u8 err_code = CMD_NO_ERR;

    u8 i = 0;
    while (i < IR_CMD_HANDLER_MAX_SIZE_TABLE) {

        if (ir_command_table[i].protocol_id == manufactor || ir_command_table[i].protocol_id == 0x00) {
            err_code = ir_command_table[i].p_callback(device, command);
            break;
        }

        i++;
    }

    p_protocol->set_finished(err_code);
    return err_code;
}

// --------------------------------------------------------------------------------
