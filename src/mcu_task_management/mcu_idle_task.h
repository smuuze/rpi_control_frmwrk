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
 * @file    mcu_idle_task.h
 * @author  Sebastian Lesse
 * @date    2022 / 12 / 12
 * @brief   Function definition of the idle task
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _MCU_IDLE_TASK_H_
#define _MCU_IDLE_TASK_H_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "mcu_task_interface.h"

// --------------------------------------------------------------------------------

/**
 * @brief 
 */
void mcu_idle_task_init(void);

/**
 * @brief 
 * 
 * @return u16 
 */
u16 mcu_idle_task_get_schedule_interval(void);

/**
 * @brief 
 * 
 * @return MCU_TASK_INTERFACE_TASK_STATE 
 */
MCU_TASK_INTERFACE_TASK_STATE mcu_idle_task_get_state(void);

/**
 * @brief 
 */
void mcu_idle_task_run(void);

/**
 * @brief 
 */
void mcu_idle_task_background_run(void);

/**
 * @brief 
 */
void mcu_idle_task_sleep(void);

/**
 * @brief 
 */
void mcu_idle_task_wakeup(void);

/**
 * @brief 
 */
void mcu_idle_task_terminate(void);

// --------------------------------------------------------------------------------

#endif // _MCU_IDLE_TASK_H_

// --------------------------------------------------------------------------------
