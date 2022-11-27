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
 * @file   rpi_cmd_handler_ir_remote_led_lights.c
 * @author Sebastian Lesse
 * @date   2021 / 12 / 28
 * @brief  Module to handle ir led-lights commands
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

#include "command_management/command_handler_interface.h"
#include "command_handler/rpi_command_handler_ir_remote.h"

#include "3rdparty/ir_protocol/ir_protocol_interface.h"
#include "3rdparty/ir_protocol/ir_command_led_lights.h"

// --------------------------------------------------------------------------------

/**
 * @brief Generates a command for a samsung tv and sends the signal
 * NEC_IR_CMD_RECEIVED_SIGNAL with the generated command
 * as parameter. 
 * In this context send means, that the generated command is send
 * via the signalslot-interface.
 * This function does not block until the command
 * was transfered over ir-interface.
 * 
 * @param command command to generated for the samsung device.
 * @return      CMD_ERR_INVALID_ARGUMENT if the given command is unknown
 *              CMD_NO_ERR if the command was generated and send
 */
static inline u8 rpi_cmd_ir_led_lights_bauhaus(u8 command);

// --------------------------------------------------------------------------------

u8 rpi_cmd_handler_ir_remote_led_lights(u8 device, u8 command) {

    DEBUG_TRACE_byte(device, "rpi_cmd_handler_ir_remote_led_lights() - Device:");

    switch (device) {
        default:
            DEBUG_TRACE_byte(device, "rpi_cmd_handler_ir_remote_led_lights() - Unknown Device:");
            return CMD_ERR_INVALID_ARGUMENT;

        case IR_DEVICE_LED_LIGHT_BAUHAUS :
            return rpi_cmd_ir_led_lights_bauhaus(command);
    }
}

// --------------------------------------------------------------------------------

static inline u8 rpi_cmd_ir_led_lights_bauhaus(u8 cmd) {
    
    DEBUG_TRACE_byte(cmd, "rpi_cmd_ir_led_lights_bauhaus() - Command:");

    IR_COMMON_COMMAND_TYPE ir_command;

    switch (cmd) {

        default:                                DEBUG_TRACE_byte(cmd, "rpi_cmd_handler_ir_remote_led_lights() - Unknown command:");
                                                return CMD_ERR_INVALID_ARGUMENT;

        case IR_COMMAND_POWER_ON :              ir_command_led_lights_bauhaus_cmd_on(&ir_command); break;
        case IR_COMMAND_POWER_OFF :             ir_command_led_lights_bauhaus_cmd_off(&ir_command); break;
        case IR_COMMAND_LIGHT_BRIGHTER :        ir_command_led_lights_bauhaus_cmd_brighter(&ir_command); break;
        case IR_COMMAND_LIGHT_DARKER :          ir_command_led_lights_bauhaus_cmd_darker(&ir_command); break;
        case IR_COMMAND_LIGHT_FLASH :           ir_command_led_lights_bauhaus_cmd_flash(&ir_command); break;
        case IR_COMMAND_LIGHT_STROBE :          ir_command_led_lights_bauhaus_cmd_strobe(&ir_command); break;
        case IR_COMMAND_LIGHT_FADE :            ir_command_led_lights_bauhaus_cmd_fade(&ir_command); break;
        case IR_COMMAND_LIGHT_SMOOTH :          ir_command_led_lights_bauhaus_cmd_smooth(&ir_command); break;
        case IR_COMMAND_LIGHT_WHITE :           ir_command_led_lights_bauhaus_cmd_white(&ir_command); break;
        case IR_COMMAND_LIGHT_RED :             ir_command_led_lights_bauhaus_cmd_red(&ir_command); break;
        case IR_COMMAND_LIGHT_RED_INDIAN :      ir_command_led_lights_bauhaus_cmd_red_indian(&ir_command); break;
        case IR_COMMAND_LIGHT_COPPER :          ir_command_led_lights_bauhaus_cmd_copper(&ir_command); break;
        case IR_COMMAND_LIGHT_ORANGE :          ir_command_led_lights_bauhaus_cmd_orange(&ir_command); break;
        case IR_COMMAND_LIGHT_YELLOW :          ir_command_led_lights_bauhaus_cmd_yellow(&ir_command); break;
        case IR_COMMAND_LIGHT_GREEN_FOREST :    ir_command_led_lights_bauhaus_cmd_green_forest(&ir_command); break;
        case IR_COMMAND_LIGHT_GREEN :           ir_command_led_lights_bauhaus_cmd_green(&ir_command); break;
        case IR_COMMAND_LIGHT_BLUE_AZURE :      ir_command_led_lights_bauhaus_cmd_blue_azure(&ir_command); break;
        case IR_COMMAND_LIGHT_BLUE_STEEL :      ir_command_led_lights_bauhaus_cmd_blue_steel(&ir_command); break;
        case IR_COMMAND_LIGHT_JADE :            ir_command_led_lights_bauhaus_cmd_jade(&ir_command); break;
        case IR_COMMAND_LIGHT_BLUE :            ir_command_led_lights_bauhaus_cmd_blue(&ir_command); break;
        case IR_COMMAND_LIGHT_BLUE_MARINE :     ir_command_led_lights_bauhaus_cmd_blue_marine(&ir_command); break;
        case IR_COMMAND_LIGHT_EGGPLANT :        ir_command_led_lights_bauhaus_cmd_eggplant(&ir_command); break;
        case IR_COMMAND_LIGHT_PURPLE :          ir_command_led_lights_bauhaus_cmd_purple(&ir_command); break;
        case IR_COMMAND_LIGHT_PINK :            ir_command_led_lights_bauhaus_cmd_pink(&ir_command); break;
    }

    ir_command_led_lights_bauhaus_address(&ir_command);
    ir_command_led_lights_bauhaus_protocol_type(&ir_command);

    IR_CMD_RECEIVED_SIGNAL_send((void*) &ir_command);
    return CMD_NO_ERR;
}

// --------------------------------------------------------------------------------
