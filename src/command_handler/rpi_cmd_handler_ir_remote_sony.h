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
 * @file        rpi_cmd_handler_ir_remote_sony.h
 * @author      sebastian lesse (sebastian lesse)
 * @version     1.0
 * @date        2021-03-20
 * @brief       command handler for a SONY ir-command
 * 
 */

#ifndef _RPI_COMMAND_HANDLER_IR_REMOTE_SONY_H_
#define _RPI_COMMAND_HANDLER_IR_REMOTE_SONY_H_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

/**
 * @brief Generate the IR-command for the given device. This function will send
 * 	SONY_IR_CMD_RECEIVED_SIGNAL_send(ir_command) with the valid ir_command
 * 	data, if the command and device are known
 * 
 * @param device Type of device where the command will be send to
 * @param command type of operation that the command will perform
 * @return 	CMD_NO_ERR if the command and device are known
 * 		CMD_ERR_INVALID_ARGUMENT otherwise
 */
u8 rpi_cmd_handler_ir_remote_sony(u8 device, u8 command);

// --------------------------------------------------------------------------------


#endif // _RPI_COMMAND_HANDLER_IR_REMOTE_SAMSUNG_H_
