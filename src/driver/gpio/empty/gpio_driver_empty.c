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
 * @file    gpio_driver_empty.c
 * @author  Sebastian Lesse
 * @date    2022 / 02 / 27
 * @brief   Implementation of an empty GPIO driver
 *          This implementation does nothing at all.
 *          It is only avaialble to provide a GPIO interface to those module,
 *          where the GPIO-interface is nessecary
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

#include "platine/board_common_config.h"
#include "system/system_interface.h"
#include "driver/gpio/gpio_interface.h"

// --------------------------------------------------------------------------------

/**
 * @see driver/gpio/gpio_interface.h#gpio_driver_init
 * 
 */
void gpio_driver_init(void) {
    DEBUG_PASS("gpio_driver_init_pin() - EMPTY");
}

void gpio_driver_deinit(void) {
    
}

/**
 * @see driver/gpio/gpio_interface.h#gpio_driver_init_pin
 * 
 */
void gpio_driver_init_pin(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
    (void) p_pin_descr;
    DEBUG_PASS("gpio_driver_init_pin() - EMPTY");
}

/**
 * @see driver/gpio/gpio_interface.h#gpio_driver_set_direction
 * 
 */
void gpio_driver_set_direction(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, GPIO_DRIVER_DIRECTION direction) {
    (void) p_pin_descr;
    (void) direction;
    DEBUG_PASS("gpio_driver_set_direction() - EMPTY");
}

/**
 * @see driver/gpio/gpio_interface.h#gpio_driver_set_level
 * 
 */
void gpio_driver_set_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, GPIO_DRIVER_LEVEL level) {
    (void) p_pin_descr;
    (void) level;
    DEBUG_PASS("gpio_driver_set_level() - EMPTY");
}

/**
 * @see driver/gpio/gpio_interface.h#gpio_driver_toggle_level
 * 
 */
void gpio_driver_toggle_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
    (void) p_pin_descr;
    DEBUG_PASS("gpio_driver_toggle_level() - EMPTY");
}

/**
 * @see driver/gpio/gpio_interface.h#gpio_driver_get_level
 * 
 */
GPIO_DRIVER_LEVEL gpio_driver_get_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
    (void) p_pin_descr;
    DEBUG_PASS("gpio_driver_get_level() - EMPTY");
    return GPIO_LEVEL_HIGH_Z;
}

/**
 * @see driver/gpio/gpio_interface.h#gpio_driver_print_pin_state
 * 
 */
void gpio_driver_print_pin_state(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
    (void) p_pin_descr;
    DEBUG_PASS("gpio_driver_get_level() - EMPTY");
}

/**
 * @see driver/gpio/gpio_interface.h#gpio_driver_deactivate
 * 
 */
void gpio_driver_deactivate(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
    (void) p_pin_descr;
    DEBUG_PASS("gpio_driver_get_level() - EMPTY");
}

/**
 * @see driver/gpio/gpio_interface.h#gpio_driver_activate
 * 
 */
void gpio_driver_activate(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) {
    (void) p_pin_descr;
    DEBUG_PASS("gpio_driver_get_level() - EMPTY");
}

// --------------------------------------------------------------------------------
