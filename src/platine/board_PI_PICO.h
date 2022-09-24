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
 * @file    board_PI_PICO.h
 * @author  Sebastian Lesse
 * @date    2022 / 08 / 02
 * @brief   Includes of the RPi Pico GPIO pins to be used in a application
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _BOARD_PI_PICO_H_
#define _BOARD_PI_PICO_H_
// --------------------------------------------------------------------------------

#include "driver/gpio/gpio_interface.h"

// --------------------------------------------------------------------------------

//---------
// PORT A
//---------

INCLUDE_GPIO ( GPIO_00 )
INCLUDE_GPIO ( GPIO_01 )
INCLUDE_GPIO ( GPIO_02 )
INCLUDE_GPIO ( GPIO_03 )
INCLUDE_GPIO ( GPIO_04 )
INCLUDE_GPIO ( GPIO_05 )
INCLUDE_GPIO ( GPIO_06 )
INCLUDE_GPIO ( GPIO_07 )

//---------
// PORT B
//---------

INCLUDE_GPIO ( GPIO_08 )
INCLUDE_GPIO ( GPIO_09 )
INCLUDE_GPIO ( GPIO_10 )
INCLUDE_GPIO ( GPIO_11 )
INCLUDE_GPIO ( GPIO_12 )
INCLUDE_GPIO ( GPIO_13 )
INCLUDE_GPIO ( GPIO_14 )
INCLUDE_GPIO ( GPIO_15 )

//---------
// PORT C
//---------

INCLUDE_GPIO ( GPIO_16 )
INCLUDE_GPIO ( GPIO_17 )
INCLUDE_GPIO ( GPIO_18 )
INCLUDE_GPIO ( GPIO_19 )
INCLUDE_GPIO ( GPIO_20 )
INCLUDE_GPIO ( GPIO_21 )
INCLUDE_GPIO ( GPIO_22 )
INCLUDE_GPIO ( GPIO_23 )

//---------
// PORT D
//---------

INCLUDE_GPIO ( GPIO_24 )
INCLUDE_GPIO ( GPIO_25 )
INCLUDE_GPIO ( GPIO_26 )
INCLUDE_GPIO ( GPIO_27 )
INCLUDE_GPIO ( GPIO_28 )

// --------------------------------------------------------------------------------

#endif // _BOARD_PI_PICO_H_
