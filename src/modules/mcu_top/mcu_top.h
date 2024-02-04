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
 * @file    mcu_top.h
 * @author  Sebastian Lesse
 * @date    2023 / 12 / 05
 * @brief   Short description of this file
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_mcu_top_
#define _H_mcu_top_

// --------------------------------------------------------------------------------

#define MCU_TOP_CFG_NAME_SCHEDULE_INTERVAL              "MCU_TOP_SCHEDULE_INTERVAL"
#define MCU_TOP_CFG_NAME_OUTPUT_FILE                    "MCU_TOP_OUTPUT_FILE"
#define MCU_TOP_CFG_NAME_OUTPUT_CONSOLE                 "MCU_TOP_OUTPUT_CONSOLE"
#define MCU_TOP_CFG_NAME_OUTPUT_MQTT                    "MCU_TOP_OUTPUT_MQTT"

// --------------------------------------------------------------------------------

/**
 * @brief Initializes the mcu-top module.
 * This funtion only needs to be called once.
 */
void mcu_top_init(void);

/**
 * @brief Set or unset the enable status of the mcu-top task.
 * If disabled the mcu-top task will not perform any operation.
 * 
 * @param enabled 1: enabled the mcu-top task / 0: disable the mcu-top task
 */
void mcu_top_set_enabled(u8 enabled);

// --------------------------------------------------------------------------------

#endif // _H_mcu_top_

// --------------------------------------------------------------------------------
