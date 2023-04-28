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
 * @file    clock_driver_macos.c
 * @author  Sebastian Lesse
 * @date    2023 / 04 / 09
 * @brief   Implementation of a clock-driver usable for the Raspberry Pi
 * 
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "cpu.h"

// --------------------------------------------------------------------------------

#include BOARD_DESCRIPTION_FILE

// --------------------------------------------------------------------------------

#include "driver/clock/clock_driver_interface.h"

// --------------------------------------------------------------------------------

/**
 * @see driver/clock/clock_driver_interface.h
 */
void clock_driver_init(void) {
    DEBUG_PASS("clock_driver_init()");
}

// --------------------------------------------------------------------------------
