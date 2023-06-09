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
 * @file    the_pi_hut_pir_sensor.c
 * @author  Sebastian Lesse
 * @date    2023 / 05 / 19
 * @brief   Implementation of the movement_detect_sensor_interface
 *          useable for the pi hut pir sensor
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

#include "driver/movement_detection/movement_detect_sensor_interface.h"
#include "driver/gpio/gpio_interface.h"

// --------------------------------------------------------------------------------

INCLUDE_GPIO_ALIAS(PIR_SENSOR)

// --------------------------------------------------------------------------------

/**
 * @see movement_detect_sensor_interface.h#movement_detect_sensor_power_up
 */
void movement_detect_sensor_power_up(void) {
    DEBUG_PASS("THE_PI_HUT_PIR - POWER_UP");
}

/**
 * @see movement_detect_sensor_interface.h#movement_detect_sensor_power_down
 */
void movement_detect_sensor_power_down(void) {
    DEBUG_PASS("THE_PI_HUT_PIR - POWER_DOWN");
}

/**
 * @see movement_detect_sensor_interface.h#movement_detect_sensor_is_movement
 */
u8 movement_detect_sensor_is_movement(void) {
    DEBUG_PASS("THE_PI_HUT_PIR - IS_MOVEMENT");
    return PIR_SENSOR_is_high_level() ? 1 : 0;
}

/**
 * @see movement_detect_sensor_interface.h#movement_detect_sensor_configure
 */
void movement_detect_sensor_configure(MOVEMENT_DETECT_SENSOR_CFG* p_config) {
    (void) p_config;
    DEBUG_PASS("THE_PI_HUT_PIR - CONFIGURE");
    PIR_SENSOR_pull_down();
}

/**
 * @see movement_detect_sensor_interface.h#movement_detect_sensor_reset
 */
void movement_detect_sensor_reset(void) {
    DEBUG_PASS("THE_PI_HUT_PIR - RESET");
}

// --------------------------------------------------------------------------------
