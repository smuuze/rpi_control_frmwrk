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
 * @file    ir_protocol_jvc_rpi_cmd_handler.c
 * @author  Sebastian Lesse
 * @date    2022 / 12 / 19
 * @brief   Short description of this file
 * 
 */

#define TRACER_ON

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

#include "common/signal_slot_interface.h"
#include "command_management/command_handler_interface.h"

// --------------------------------------------------------------------------------

#include "modules/ir/ir_protocol_interface.h"

// --------------------------------------------------------------------------------

#include "modules/ir/ir_protocol_rpi_cmd_handler.h"
#include "modules/ir/ir_protocol_jvc.h"

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param command 
 * @return u8 
 */
static inline u8 rpi_cmd_ir_jvc_radio(u8 command);

// --------------------------------------------------------------------------------

u8 rpi_cmd_handler_ir_remote_jvc(u8 device, u8 command) {

    DEBUG_PASS("rpi_cmd_handler_ir_remote_jvc()");

    switch (device) {
        default:
        
            DEBUG_TRACE_byte(
                device,
                "rpi_cmd_handler_ir_remote_jvc() - Unknown device"
            );
            return CMD_ERR_INVALID_ARGUMENT;

        case IR_DEVICE_RADIO :
            return rpi_cmd_ir_jvc_radio(command);
    }
}

// --------------------------------------------------------------------------------

static inline u8 rpi_cmd_ir_jvc_radio(u8 cmd) {
    
    DEBUG_TRACE_byte(cmd, "rpi_cmd_ir_jvc_radio() - Command:");
    IR_COMMON_COMMAND_TYPE ir_command;

    switch (cmd) {

        default:
            DEBUG_TRACE_byte(cmd, "rpi_cmd_ir_jvc_radio() - Unknown Command:");
            return CMD_ERR_INVALID_ARGUMENT;

        case IR_COMMAND_POWER_ON :
            ir_protocol_jvc_cmd_radio_power(&ir_command);
            break;

        case IR_COMMAND_POWER_OFF :
            ir_protocol_jvc_cmd_radio_power(&ir_command);
            break;

        case IR_COMMAND_VOLUME_UP :
            ir_protocol_jvc_cmd_radio_volume_up(&ir_command);
            break;

        case IR_COMMAND_VOLUME_DOWN :
            ir_protocol_jvc_cmd_radio_volume_down(&ir_command);
            break;

        case IR_COMMAND_PLAY :
            ir_protocol_jvc_cmd_radio_play(&ir_command);
            break;

        case IR_COMMAND_PAUSE :
            ir_protocol_jvc_cmd_radio_pause(&ir_command);
            break;

        case IR_COMMAND_STOP :
            ir_protocol_jvc_cmd_radio_stop(&ir_command);
            break;

        case IR_COMMAND_BASS_UP :
            ir_protocol_jvc_cmd_radio_bass_up(&ir_command);
            break;

        case IR_COMMAND_BASS_DOWN :
            ir_protocol_jvc_cmd_radio_bass_down(&ir_command);
            break;

        case IR_COMMAND_SOUND_MODE :
            ir_protocol_jvc_cmd_radio_sound_mode(&ir_command);
            break;

        case IR_COMMAND_MUTE :
            ir_protocol_jvc_cmd_radio_volume_mute(&ir_command);
            break;
    }

    ir_protocol_jvc_address_radio(&ir_command);
    ir_command_jvc_protocol_type(&ir_command);

    IR_CMD_RECEIVED_SIGNAL_send((void*) &ir_command);

    return CMD_NO_ERR;
}

// --------------------------------------------------------------------------------
