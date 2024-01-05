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
 * @file    command_controller.h
 * @author  Sebastian Lesse
 * @date    2023 / 10 / 03
 * @brief   Short description of this file
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_command_controller_
#define _H_command_controller_

// --------------------------------------------------------------------------------

#include "command_handler_interface.h"
#include "mcu_task_interface.h"

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
void command_controller_init(void);

/**
 * @brief 
 * 
 * @param p_handler 
 */
void command_controller_register_handler(COMMAND_HANDLER_INTERFACE* p_handler);


// --------------------------------------------------------------------------------

#endif // _H_command_controller_

// --------------------------------------------------------------------------------
