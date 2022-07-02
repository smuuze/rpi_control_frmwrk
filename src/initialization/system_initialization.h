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
 * @file    system_initialization.h
 * @author  Sebastian Lesse
 * @date    2022 / 06 / 05
 * @brief   Short description of this file
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_system_initialization_
#define _H_system_initialization_

// --------------------------------------------------------------------------------

/**
 * @brief Initializes system resources. Including:
 * - gpio
 * - rtc
 * - system-clock
 * - spi
 * - usart
 * - i2c
 * 
 */
void system_initialization(void);

// --------------------------------------------------------------------------------

/**
 * @brief De-initializes system resources
 * 
 */
void system_deinitialization(void);

// --------------------------------------------------------------------------------

#endif // _H_system_initialization_

// --------------------------------------------------------------------------------

