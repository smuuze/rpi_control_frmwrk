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
 * @file    rpi_cmd_handler_routing.h
 * @author  Sebastian Lesse
 * @date    2020 / 05 / 26
 * @brief   Short description of this file
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_rpi_cmd_handler_routing_
#define _H_rpi_cmd_handler_routing_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "command_management/protocol_interface.h"

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
void rpi_cmd_handler_routing_init(void);

/**
 * @brief 
 * 
 * @param p_protocol 
 * @return u8 
 */
u8 rpi_cmd_handler_routing(PROTOCOL_INTERFACE* p_protocol);

// --------------------------------------------------------------------------------

#endif // _H_rpi_cmd_handler_routing_

// --------------------------------------------------------------------------------
