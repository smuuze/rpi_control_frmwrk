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
 * @file   ir_protocol_samsun_rpi_cmd_handlerg.c
 * @author Sebastian Lesse
 * @date   2022 / 12 / 24
 * @brief  description
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
#include "command_management/command_handler_interface.h"

// --------------------------------------------------------------------------------

#include "modules/ir/ir_protocol_interface.h"

// --------------------------------------------------------------------------------

#include "modules/ir/ir_protocol_rpi_cmd_handler.h"
#include "modules/ir/ir_protocol_samsung.h"

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(SAMSUNG_IR_CMD_RECEIVED_SIGNAL)

// --------------------------------------------------------------------------------

/**
 * @brief Generates a command for a samsung tv and sends the signal
 * SAMSUNG_IR_CMD_RECEIVED_SIGNAL with the generated command
 * as parameter. 
 * In this context send means, that the generated command is send
 * via the signalslot-interface.
 * This function does not block until the command
 * was transfered over ir-interface.
 * 
 * @param command command to generated for the samsung device.
 * @return	CMD_ERR_INVALID_ARGUMENT if the given command is unknown
 * 		CMD_NO_ERR if the command was generated and send
 */
static inline u8 rpi_cmd_ir_samsung_tv(u8 command);

// --------------------------------------------------------------------------------

u8 rpi_cmd_handler_ir_remote_samsung(u8 device, u8 command) {

	DEBUG_PASS("rpi_cmd_handler_ir_remote_samsung()");

	switch (device) {
		default:		return CMD_ERR_INVALID_ARGUMENT;
		case IR_DEVICE_TV : 	return rpi_cmd_ir_samsung_tv(command);
	}
}

// --------------------------------------------------------------------------------

static inline u8 rpi_cmd_ir_samsung_tv(u8 cmd) {
	
	DEBUG_TRACE_byte(cmd, "rpi_cmd_ir_samsung_tv() - Command:");
	IR_COMMON_COMMAND_TYPE ir_command;

	switch (cmd) {

		default:			return CMD_ERR_INVALID_ARGUMENT;

		case IR_COMMAND_POWER_ON : 	ir_protocol_samsung_cmd_tv_power(&ir_command); break;
		case IR_COMMAND_POWER_OFF :	ir_protocol_samsung_cmd_tv_power(&ir_command); break;

		case IR_COMMAND_VOLUME_UP :	ir_protocol_samsung_cmd_tv_volume_up(&ir_command); break;
		case IR_COMMAND_VOLUME_DOWN :	ir_protocol_samsung_cmd_tv_volume_down(&ir_command); break;
		case IR_COMMAND_MUTE :		ir_protocol_samsung_cmd_tv_volume_mute(&ir_command); break;

		case IR_COMMAND_CHANNEL_UP :	ir_protocol_samsung_cmd_tv_channel_up(&ir_command); break;
		case IR_COMMAND_CHANNEL_DOWN :	ir_protocol_samsung_cmd_tv_channel_down(&ir_command); break;

		case IR_COMMAND_CHANNEL_0 :	ir_protocol_samsung_cmd_tv_channel_0(&ir_command); break;
		case IR_COMMAND_CHANNEL_1 :	ir_protocol_samsung_cmd_tv_channel_1(&ir_command); break;
		case IR_COMMAND_CHANNEL_2 :	ir_protocol_samsung_cmd_tv_channel_2(&ir_command); break;
		case IR_COMMAND_CHANNEL_3 :	ir_protocol_samsung_cmd_tv_channel_3(&ir_command); break;
		case IR_COMMAND_CHANNEL_4 :	ir_protocol_samsung_cmd_tv_channel_4(&ir_command); break;
		case IR_COMMAND_CHANNEL_5 :	ir_protocol_samsung_cmd_tv_channel_5(&ir_command); break;
		case IR_COMMAND_CHANNEL_6 :	ir_protocol_samsung_cmd_tv_channel_6(&ir_command); break;
		case IR_COMMAND_CHANNEL_7 :	ir_protocol_samsung_cmd_tv_channel_7(&ir_command); break;
		case IR_COMMAND_CHANNEL_8 :	ir_protocol_samsung_cmd_tv_channel_8(&ir_command); break;
		case IR_COMMAND_CHANNEL_9 :	ir_protocol_samsung_cmd_tv_channel_9(&ir_command); break;

		case IR_COMMAND_ENTER :		ir_protocol_samsung_cmd_tv_enter(&ir_command); break;
		case IR_COMMAND_EXIT :		ir_protocol_samsung_cmd_tv_exit(&ir_command); break;
		case IR_COMMAND_RETURN :	ir_protocol_samsung_cmd_tv_return(&ir_command); break;
		case IR_COMMAND_ARROW_UP :	ir_protocol_samsung_cmd_tv_arrow_up(&ir_command); break;
		case IR_COMMAND_ARROW_DOWN :	ir_protocol_samsung_cmd_tv_arrow_down(&ir_command); break;
		case IR_COMMAND_ARROW_LEFT :	ir_protocol_samsung_cmd_tv_arrow_left(&ir_command); break;
		case IR_COMMAND_ARROW_RIGHT :	ir_protocol_samsung_cmd_tv_arrow_right(&ir_command); break;

		case IR_COMMAND_PLAY :		ir_protocol_samsung_cmd_tv_play(&ir_command); break;
		case IR_COMMAND_PAUSE :		ir_protocol_samsung_cmd_tv_pause(&ir_command); break;
		case IR_COMMAND_STOP :		ir_protocol_samsung_cmd_tv_stop(&ir_command); break;

		case IR_COMMAND_PROGRAM_GUIDE :	ir_protocol_samsung_cmd_tv_program_guide(&ir_command); break;
		case IR_COMMAND_HOME :		ir_protocol_samsung_cmd_tv_home(&ir_command); break;

		case IR_COMMAND_CHANNEL_LIST :	ir_protocol_samsung_cmd_tv_channel_list(&ir_command); break;
		case IR_COMMAND_SOURCE :	ir_protocol_samsung_cmd_tv_source(&ir_command); break;
	}

	ir_protocol_samsung_address_tv(&ir_command);
    ir_command_samsung_protocol_type(&ir_command);

	IR_CMD_RECEIVED_SIGNAL_send((void*) &ir_command);

	return CMD_NO_ERR;
}
