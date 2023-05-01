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
 * @file    movement_detection_controller.c
 * @author  Sebastian Lesse
 * @date    2023 / 05 / 01
 * @brief   Realizes a controller module to detect movement.
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

#include "mcu_task_interface.h"
#include "signal_slot_interface"
#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

/**
 * @brief Interval at which the controller checks the sensor for detected movement
 */
#define MOVEMENT_DETECTION_CONTROLLER_SCHEDULE_INTERVAL_MS              100

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(MOVE_DETECT_TIMER)

// --------------------------------------------------------------------------------

typedef enum {
    MOVEMENT_DETECTION_STATE_SETUP = 0,
    MOVEMENT_DETECTION_STATE_WAIT,
    MOVEMENT_DETECTION_STATE_VERIFY,
    MOVEMENT_DETECTION_STATE_SIGNALING,
    MOVEMENT_DETECTION_STATE_PAUSE
} MOVEMENT_DETECTION_CONTROLLER_STATES;

static MOVEMENT_DETECTION_CONTROLLER_STATES move_detect_state = MOVEMENT_DETECTION_STATE_SETUP;

// --------------------------------------------------------------------------------

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.start
 */
static void MOVEMENT_DETECT_CONTROLLER_TASK_start(void) {
    
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_schedule_interval
 */
static u16 MOVEMENT_DETECT_CONTROLLER_TASK_get_schedule_interval(void) {
    return MOVEMENT_DETECTION_CONTROLLER_SCHEDULE_INTERVAL_MS;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_state
 */
static MCU_TASK_INTERFACE_TASK_STATE MOVEMENT_DETECT_CONTROLLER_task_get_state(void) {
    
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.run
 */
static void MOVEMENT_DETECT_CONTROLLER_TASK_execute(void) {
    
    switch (move_detect_state) {

        default:
            move_detect_state = MOVEMENT_DETECTION_STATE_SETUP;
            break;

        case MOVEMENT_DETECTION_STATE_SETUP:
            move_detect_state = MOVEMENT_DETECTION_STATE_WAIT;
            break;

        case MOVEMENT_DETECTION_STATE_WAIT:
            move_detect_state = MOVEMENT_DETECTION_STATE_VERIFY;
            break;

        case MOVEMENT_DETECTION_STATE_VERIFY:
            move_detect_state = MOVEMENT_DETECTION_STATE_SIGNALING;
            break;

        case MOVEMENT_DETECTION_STATE_SIGNALING:
            move_detect_state = MOVEMENT_DETECTION_STATE_PAUSE;
            break;

        case MOVEMENT_DETECTION_STATE_PAUSE:
            move_detect_state = MOVEMENT_DETECTION_STATE_SETUP;
            break;

    }
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.terminate
 */
static void MOVEMENT_DETECT_CONTROLLER_TASK_terminate(void) {

}

// --------------------------------------------------------------------------------

TASK_CREATE(
   MOVEMENT_DETECT_CONTROLLER_TASK,
   TASK_PRIORITY_MIDDLE,
   MOVEMENT_DETECT_CONTROLLER_TASK_get_schedule_interval,
   MOVEMENT_DETECT_CONTROLLER_TASK_start,
   MOVEMENT_DETECT_CONTROLLER_TASK_execute,
   MOVEMENT_DETECT_CONTROLLER_task_get_state,
   MOVEMENT_DETECT_CONTROLLER_TASK_terminate
)

// --------------------------------------------------------------------------------

/**
 * @see movement_detection_interface.h#init
 */
void movement_detection_controller_init(void) {
    MOVEMENT_DETECT_CONTROLLER_TASK_init();
}

// --------------------------------------------------------------------------------
