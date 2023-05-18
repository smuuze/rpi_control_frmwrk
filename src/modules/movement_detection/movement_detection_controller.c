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
#include "signal_slot_interface.h"
#include "time_management/time_management.h"
#include "local_module_status.h"

// --------------------------------------------------------------------------------

#include "modules/movement_detection/movement_detection_controller.h"
#include "driver/movement_detection/movement_detect_sensor_interface.h"

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(MOVE_DETECT_TIMER)

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(MOVEMENT_DETECT_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(MOVEMENT_DETECT_POWER_DOWN_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(MOVEMENT_DETECT_POWER_UP_SIGNAL)

// --------------------------------------------------------------------------------

#define MOVMENT_DETECTION_STATUS_ENTER_POWER_DOWN ( 1 << 0 )
#define MOVMENT_DETECTION_STATUS_LEAVE_POWER_DOWN ( 1 << 1 )

BUILD_MODULE_STATUS_U8(MOVMENT_DETECTION_STATUS)

// --------------------------------------------------------------------------------

/**
 * @brief Callback to enter the power down mode
 * 
 * @param p_argument is ignored
 */
static void movement_detect_signal_power_down_callback(const void* p_argument) {
    (void) p_argument;

    DEBUG_PASS("movement_detect_signal_power_down_callback()");
    MOVMENT_DETECTION_STATUS_set(MOVMENT_DETECTION_STATUS_ENTER_POWER_DOWN);
}

/**
 * @brief Callback to leave power down mode.
 * 
 * @param p_argument is ignored
 */
static void movement_detect_signal_power_up_callback(const void* p_argument) {
    (void) p_argument;

    DEBUG_PASS("movement_detect_signal_power_up_callback()");
    MOVMENT_DETECTION_STATUS_set(MOVMENT_DETECTION_STATUS_LEAVE_POWER_DOWN);
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(
    MOVEMENT_DETECT_POWER_DOWN_SIGNAL,
    MOVEMENT_DETECT_POWER_DOWN_SLOT,
    movement_detect_signal_power_down_callback
)

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(
    MOVEMENT_DETECT_POWER_UP_SIGNAL,
    MOVEMENT_DETECT_POWER_UP_SLOT,
    movement_detect_signal_power_up_callback
)

// --------------------------------------------------------------------------------

/**
 * @brief States of the state-machine
 */
typedef enum {
    MOVEMENT_DETECTION_STATE_SETUP = 0,
    MOVEMENT_DETECTION_STATE_WAIT_FOR_MOVEMENT,
    MOVEMENT_DETECTION_STATE_WAIT_TO_VERIFY,
    MOVEMENT_DETECTION_STATE_VERIFY,
    MOVEMENT_DETECTION_STATE_SIGNALING,
    MOVEMENT_DETECTION_STATE_PAUSE,
    MOVEMENT_DETECTION_STATE_POWER_DOWN
} MOVEMENT_DETECTION_CONTROLLER_STATES;

/**
 * @brief Current state of the state-machine
 */
static MOVEMENT_DETECTION_CONTROLLER_STATES move_detect_state = MOVEMENT_DETECTION_STATE_SETUP;

// --------------------------------------------------------------------------------

/**
 * @brief Configures the sensor to the values of use for the current project
 */
static inline void movement_detect_controller_configure_sensor(void) {
    
    MOVEMENT_DETECT_SENSOR_CFG movement_sensor_config;
    movement_detect_sensor_configure(&movement_sensor_config);
}

// --------------------------------------------------------------------------------

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.start
 */
static void MOVEMENT_DETECT_CONTROLLER_TASK_start(void) {
    move_detect_state = MOVEMENT_DETECTION_STATE_SETUP;
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

    return MCU_TASK_RUNNING;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.run
 */
static void MOVEMENT_DETECT_CONTROLLER_TASK_execute(void) {
    
    switch (move_detect_state) {

        default:
            move_detect_state = MOVEMENT_DETECTION_STATE_SETUP;
            // no break;

        case MOVEMENT_DETECTION_STATE_SETUP:

            DEBUG_PASS("MOVEMENT_DETECT_CONTROLLER_TASK_execute() - CHANGE STATE - SETUP -> WAIT_FOR_MOVEMENT");
            movement_detect_controller_configure_sensor();
            move_detect_state = MOVEMENT_DETECTION_STATE_WAIT_FOR_MOVEMENT;
            break;

        case MOVEMENT_DETECTION_STATE_WAIT_FOR_MOVEMENT:

            if (MOVMENT_DETECTION_STATUS_is_set(MOVMENT_DETECTION_STATUS_ENTER_POWER_DOWN)) {

                DEBUG_PASS("MOVEMENT_DETECT_CONTROLLER_TASK_execute() - CHANGE STATE - SETUP -> POWER_DOWN");
                move_detect_state = MOVEMENT_DETECTION_STATE_POWER_DOWN;

            } else if (movement_detect_sensor_is_movement() != 0) {
                DEBUG_PASS("MOVEMENT_DETECT_CONTROLLER_TASK_execute() - CHANGE STATE - WAIT_FOR_MOVEMENT -> WAIT_TO_VERIFY");
                move_detect_state = MOVEMENT_DETECTION_STATE_WAIT_TO_VERIFY;
                MOVE_DETECT_TIMER_start();
            }

            break;

        case MOVEMENT_DETECTION_STATE_WAIT_TO_VERIFY:

            if (MOVE_DETECT_TIMER_is_up(MOVEMENT_DETECTION_CONTROLLER_WAIT_TO_VERIFY_TIMEOUT_MS)) {
                DEBUG_PASS("MOVEMENT_DETECT_CONTROLLER_TASK_execute() - CHANGE STATE - WAIT_TO_VERIFY -> VERFIY");
                move_detect_state = MOVEMENT_DETECTION_STATE_VERIFY;
                MOVE_DETECT_TIMER_start();
            }

            break;

        case MOVEMENT_DETECTION_STATE_VERIFY:
            
            if (MOVE_DETECT_TIMER_is_up(MOVEMENT_DETECTION_CONTROLLER_VERIFY_TIMEOUT_MS)) {
                
                DEBUG_PASS("MOVEMENT_DETECT_CONTROLLER_TASK_execute() - CHANGE STATE - VERFIY -> WAIT_FOR_MOVEMENT");
                move_detect_state = MOVEMENT_DETECTION_STATE_WAIT_FOR_MOVEMENT;
                MOVE_DETECT_TIMER_stop();

            } else if (movement_detect_sensor_is_movement()) {

                DEBUG_PASS("MOVEMENT_DETECT_CONTROLLER_TASK_execute() - CHANGE STATE - VERFIY -> SIGNALING");
                move_detect_state = MOVEMENT_DETECTION_STATE_SIGNALING;
                MOVE_DETECT_TIMER_stop();
            }

            break;

        case MOVEMENT_DETECTION_STATE_SIGNALING:

            MOVEMENT_DETECT_SIGNAL_send(NULL);

            MOVE_DETECT_TIMER_start();
            DEBUG_PASS("MOVEMENT_DETECT_CONTROLLER_TASK_execute() - CHANGE STATE - SIGNALING -> PAUSE");
            move_detect_state = MOVEMENT_DETECTION_STATE_PAUSE;
            break;

        case MOVEMENT_DETECTION_STATE_PAUSE:
            
            if (MOVE_DETECT_TIMER_is_up(MOVEMENT_DETECTION_CONTROLLER_PAUSE_TIME_MS)) {
                DEBUG_PASS("MOVEMENT_DETECT_CONTROLLER_TASK_execute() - CHANGE STATE - PAUSE -> WAIT_FOR_MOVEMENT");
                move_detect_state = MOVEMENT_DETECTION_STATE_WAIT_FOR_MOVEMENT;
            }

            break;

        case MOVEMENT_DETECTION_STATE_POWER_DOWN:

            if (MOVMENT_DETECTION_STATUS_is_set(MOVMENT_DETECTION_STATUS_ENTER_POWER_DOWN)) {
                MOVMENT_DETECTION_STATUS_unset(MOVMENT_DETECTION_STATUS_ENTER_POWER_DOWN);
                movement_detect_sensor_power_down();
            }

            if (MOVMENT_DETECTION_STATUS_is_set(MOVMENT_DETECTION_STATUS_LEAVE_POWER_DOWN)) {

                MOVMENT_DETECTION_STATUS_unset(MOVMENT_DETECTION_STATUS_LEAVE_POWER_DOWN);
                movement_detect_sensor_power_up();

                DEBUG_PASS("MOVEMENT_DETECT_CONTROLLER_TASK_execute() - CHANGE STATE - POWER_DOWN -> WAIT_FOR_MOVEMENT");
                move_detect_state = MOVEMENT_DETECTION_STATE_WAIT_FOR_MOVEMENT;
            }

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

    DEBUG_PASS("movement_detection_controller_init()");

    MOVEMENT_DETECT_SIGNAL_init();
    MOVEMENT_DETECT_POWER_DOWN_SIGNAL_init();
    MOVEMENT_DETECT_POWER_UP_SIGNAL_init();

    MOVEMENT_DETECT_POWER_DOWN_SLOT_connect();
    MOVEMENT_DETECT_POWER_UP_SLOT_connect();

    MOVEMENT_DETECT_CONTROLLER_TASK_init();
}

// --------------------------------------------------------------------------------
