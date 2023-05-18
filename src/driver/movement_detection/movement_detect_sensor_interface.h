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
 * @file    movement_detect_sensor_interface.h
 * @author  Sebastian Lesse
 * @date    2023 / 05 / 05
 * @brief   Short description of this file
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_movement_detect_sensor_interface_
#define _H_movement_detect_sensor_interface_

// --------------------------------------------------------------------------------

/**
 * @brief Structure to configure a movment-detection sensor.
 */
typedef struct {
    u8 value;
} MOVEMENT_DETECT_SENSOR_CFG;

// --------------------------------------------------------------------------------

/**
 * @brief Power up the sensor to use it.
 * Hint: This function does nothing, if the current sensor
 * cant be put into power-down mode
 */
extern void movement_detect_sensor_power_up(void);

/**
 * @brief Power down the sensor to safe energy.
 * The sensor cannot be used if powered down.
 * Hint: This function does nothing, if the current sensor
 * cant be put into power-down mode
 */
extern void movement_detect_sensor_power_down(void);

/**
 * @brief Check if the sensor has detected movement.
 * 
 * @return 1 - movement has been detected
 *         0 - no movement is detected
 */
extern u8 movement_detect_sensor_is_movement(void);

/**
 * @brief Configures the sensor with the given configuration values.
 * Hint: This fucntion does nothing if the current sensor cant be configured.
 * @param p_config configuration to apply
 * @see MOVEMENT_DETECT_SENSOR_CFG
 */
extern void movement_detect_sensor_configure(MOVEMENT_DETECT_SENSOR_CFG* p_config);

/**
 * @brief Resets the sensor to its default state.
 * The device needs to be re-configured.
 */
extern void movement_detect_sensor_reset(void);

// --------------------------------------------------------------------------------

#endif // _H_movement_detect_sensor_interface_

// --------------------------------------------------------------------------------
