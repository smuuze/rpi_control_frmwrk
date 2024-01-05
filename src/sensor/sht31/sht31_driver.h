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
 * @file    sht31_interface.h
 * @author  Sebastian Lesse
 * @date    2023 / 08 / 19
 * @brief   Interface to the Temperature and Humidity Sensor SHT31
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_sht31_driver_
#define _H_sht31_driver_

// --------------------------------------------------------------------------------

#include "trx_driver_interface.h"
#include "mcu_task_interface.h"

// --------------------------------------------------------------------------------

#define SHT31_OPERATION_TIMEOUT_MS                                  250
#define SHT31_STARTUP_TIME_MS                                       10

// --------------------------------------------------------------------------------

#define SHT31_COMMAND_CLEAR_STATUS_REGISTER                         0x3041
#define SHT31_COMMAND_WITH_CLK_STRECHTING_HIGH_REPEATABILITY        0x2C06
#define SHT31_COMMAND_WITH_CLK_STRECHTING_MEDIUM_REPEATABILITY      0x2C0D
#define SHT31_COMMAND_WITH_CLK_STRECHTING_LOW_REPEATABILITY         0x2C10
#define SHT31_COMMAND_NO_CLK_STRECHTING_HIGH_REPEATABILITY          0x2400
#define SHT31_COMMAND_NO_CLK_STRECHTING_MEDIUM_REPEATABILITY        0x2408
#define SHT31_COMMAND_NO_CLK_STRECHTING_LOW_REPEATABILITY           0x2416

// --------------------------------------------------------------------------------

#define SHT31_SET_COMMAND(command, p_buffer)    p_buffer[0] = (u8)(command >> 8); \
                                                p_buffer[1] = (u8)(command &0x00FF);

#define SHT31_COMMAND_STATUS_REGISTER                               {0xF3, 0x20}

// --------------------------------------------------------------------------------

#define SHT31_MEASUREMENT_COMMAND_LENGTH                            2
#define SHT31_MEASRUEMENT_ANSWER_LENGTH                             6

#define SHT31_TEMPERATURE_FIXPOINT_FACTOR_M                         0x00AF
#define SHT31_TEMPERATURE_FIXPOINT_FACTOR_B                         0x002D
#define SHT31_TEMPERATURE_CORRECTION_OFFSET                         ((i8)-4)

// --------------------------------------------------------------------------------

#define SHT31_HUMIDITY_FIXPOINT_FACTOR_M                            0x0064

#define SHT31_BUS_ADDRESS_1                                         0x44
#define SHT31_BUS_ADDRESS_2                                         0x45

#define SHT31_INDEX_OF_TEMPERATURE_MSB_IN_ANSWER                    0
#define SHT31_INDEX_OF_TEMPERATURE_LSB_IN_ANSWER                    1
#define SHT31_INDEX_OF_HUMIDITY_MSB_IN_ANSWER                       3
#define SHT31_INDEX_OF_HUMIDITY_LSB_IN_ANSWER                       4

#define SHT31_MAXIMUM_MEASSUREABLE_TEMPERATURE                      127
#define SHT31_MAXIMUM_MEASSUREABLE_HUMIDITY                         100

#define SHT31_MEASUREMENT_TIME_LOW_REPEATABILITY_MS                 (15 + 1) /* including tolerance */

#define SHT31_ADC_TO_TEMPERATURE(p_u8_buffer)

// --------------------------------------------------------------------------------

/*!
 *
 * @param p_driver
 */
void sht31_driver_init(TRX_DRIVER_INTERFACE* p_driver);

// --------------------------------------------------------------------------------

#endif // _H_sht31_driver_

// --------------------------------------------------------------------------------
