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
 * @file    key_pad_driver_3x4.c
 * @author  Sebastian Lesse
 * @date    2022 / 12 / 18
 * @brief   Short description of this file
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

#include "driver/key_pad/key_pad_driver.h"
#include "driver/gpio/gpio_interface.h"

// --------------------------------------------------------------------------------

/**
 * @see key_pad_driver.h#key_pad_driver_init
 */
void key_pad_driver_init(void) {

}

// --------------------------------------------------------------------------------

/**
 * @see key_pad_driver.h#key_pad_driver_is_key_pressed
 */
u8 key_pad_driver_is_key_pressed(void) {
    
}

// --------------------------------------------------------------------------------

/**
 * @see key_pad_driver.h#key_pad_driver_get_keys
 */
void key_pad_driver_get_keys(KEY_PAD_KEYS* p_keys) {
    
}

// --------------------------------------------------------------------------------
