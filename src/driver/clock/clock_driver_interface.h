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
 * @file    clock_driver_interface.h
 * @author  Sebastian Lesse
 * @date    2022 / 09 / 24
 * @brief   Interface to clock functions of the currently used cpu
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_clock_driver_interface_
#define _H_clock_driver_interface_

// --------------------------------------------------------------------------------

#define CLK_DRIVER_CLK_PRESCALER_1          0x00
#define CLK_DRIVER_CLK_PRESCALER_2          0x01
#define CLK_DRIVER_CLK_PRESCALER_4          0x02
#define CLK_DRIVER_CLK_PRESCALER_8          0x03
#define CLK_DRIVER_CLK_PRESCALER_16         0x04
#define CLK_DRIVER_CLK_PRESCALER_32         0x05
#define CLK_DRIVER_CLK_PRESCALER_64         0x06
#define CLK_DRIVER_CLK_PRESCALER_128        0x07
#define CLK_DRIVER_CLK_PRESCALER_256        0x08

// --------------------------------------------------------------------------------

/**
 * @brief Initializes the clock driver of the currently used cpu.
 * respect the order of the initialization sequence of your cpu
 * (clock before peripherials)
 * 
 */
void clock_driver_init(void);

// --------------------------------------------------------------------------------

#endif // _H_clock_driver_interface_

// --------------------------------------------------------------------------------
