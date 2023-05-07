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
 * @file    movement_detection_controller.h
 * @author  Sebastian Lesse
 * @date    2023 / 05 / 01
 * @brief   Interface to start
 */

// --------------------------------------------------------------------------------

#ifndef _H_movement_detection_controller_
#define _H_movement_detection_controller_

// --------------------------------------------------------------------------------

#include "signal_slot_interface.h"

// --------------------------------------------------------------------------------

/**
 * @brief Interval at which the controller checks
 * the sensor for detected movement
 */
#ifndef MOVEMENT_DETECTION_CONTROLLER_SCHEDULE_INTERVAL_MS
#define MOVEMENT_DETECTION_CONTROLLER_SCHEDULE_INTERVAL_MS              100
#endif

/**
 * @brief Time in milliseconds to wait before verify a movement.
 */
#ifndef MOVEMENT_DETECTION_CONTROLLER_WAIT_TO_VERIFY_TIMEOUT_MS
#define MOVEMENT_DETECTION_CONTROLLER_WAIT_TO_VERIFY_TIMEOUT_MS         500
#endif

/**
 * @brief Time in milliseconds to verify a movement.
 * If this time is up, there is no movement.
 */
#ifndef MOVEMENT_DETECTION_CONTROLLER_VERIFY_TIMEOUT_MS
#define MOVEMENT_DETECTION_CONTROLLER_VERIFY_TIMEOUT_MS                 1000
#endif

/**
 * @brief Pause interval in milliseconds
 * after a successful movement detection.
 */
#ifndef MOVEMENT_DETECTION_CONTROLLER_PAUSE_TIME_MS
#define MOVEMENT_DETECTION_CONTROLLER_PAUSE_TIME_MS                     1000
#endif

// --------------------------------------------------------------------------------

/**
 * @brief Sendind this signal will enable the power down mode
 * of the movement-detection controller. While in power down
 * no movement can be detected. An currently on going operation
 * is finished before the power down mode is entered.
 */
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(MOVEMENT_DETECT_POWER_DOWN_SIGNAL)

/**
 * @brief Sending this signal will disable an currently active power down mode
 * of the movment-detection controller. If the power down mode is not active
 * right now, nothing happens.
 */
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(MOVEMENT_DETECT_POWER_UP_SIGNAL)

// --------------------------------------------------------------------------------

/**
 * @brief Initializes the movement-detection module and starts it.
 */
void movement_detection_controller_init(void);

// --------------------------------------------------------------------------------

#endif // _H_movement_detection_controller_

// --------------------------------------------------------------------------------
