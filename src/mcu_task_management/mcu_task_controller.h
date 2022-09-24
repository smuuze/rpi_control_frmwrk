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
 * @file    mcu_task_controller.h
 * @author  Sebastian Lesse
 * @date    2022 / 09 / 24
 * @brief   Short description of this file
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_mcu_task_controller_
#define _H_mcu_task_controller_

// --------------------------------------------------------------------------------

#include "mcu_task_interface.h"

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
void mcu_task_controller_init(void);

/**
 * @brief 
 * 
 * @param p_mcu_task 
 */
void mcu_task_controller_register_task(MCU_TASK_INTERFACE* p_mcu_task);

/**
 * @brief 
 * 
 */
void mcu_task_controller_schedule(void);

/**
 * @brief 
 * 
 */
void mcu_task_controller_sleep(void);

/**
 * @brief 
 * 
 */
void mcu_task_controller_wakeup(void);

/**
 * @brief 
 * 
 */
void mcu_task_controller_background_run(void);

/**
 * @brief 
 * 
 */
void mcu_task_controller_terminate_all(void);

// --------------------------------------------------------------------------------

#endif // _H_mcu_task_controller_

// --------------------------------------------------------------------------------
