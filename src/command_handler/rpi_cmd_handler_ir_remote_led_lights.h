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
 * @file   rpi_cmd_handler_ir_remote_led_lights.h
 * @author Sebastian Lesse
 * @date   2021 / 12 / 28
 * @brief  Command handler to generate ir-commands of LED-light devices
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_rpi_cmd_handler_ir_remote_led_lights_
#define _H_rpi_cmd_handler_ir_remote_led_lights_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

/**
 * @brief Generates and sends a ir-command to a NEC device.
 * This functions sends the LED_LIGHTS_IR_CMD_RECEIVED_SIGNAL signal
 * with the generates ir-command as a parameter.
 * In this context send means, that the generated command is send
 * via the signalslot-interface.
 * This function does not block until the command was transfered over ir-interface.
 * 
 * @param device the type of device for which the command is generated.
 * @param command the comamnd to genearate
 * @return 	CMD_NO_ERR if the command was generated ans send.
 * 		CMD_ERR_INVALID_ARGUMENT if the type of the command is unknown or
 * 		if the device is unknown or unsupported
 */
u8 rpi_cmd_handler_ir_remote_led_lights(u8 device, u8 command);

// --------------------------------------------------------------------------------

#endif // _H_rpi_cmd_handler_ir_remote_led_lights_

// --------------------------------------------------------------------------------
