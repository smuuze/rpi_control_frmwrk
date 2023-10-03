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
 * @file    local_event_task.h
 * @author  Sebastian Lesse
 * @date    2021 / 03 / 21
 * @brief   Short description of this file
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_local_event_task_
#define _H_local_event_task_

// --------------------------------------------------------------------------------

#include "system_interface.h"

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
void event_controller_init(void);

/**
 * @brief 
 * 
 * @param event 
 */
void local_event_add(SYSTEM_EVENT event);

/**
 * @brief 
 * 
 * @return SYSTEM_EVENT 
 */
SYSTEM_EVENT local_event_get_next(void);

// --------------------------------------------------------------------------------

#endif // _H_local_event_task_

// --------------------------------------------------------------------------------
