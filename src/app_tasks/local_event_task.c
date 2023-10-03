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
 * @file    local_event_task.c
 * @author  Sebastian Lesse
 * @date    2021 / 03 / 21
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

#include "system_interface.h"
#include "local_event_task.h"
#include "time_management.h"

#include "specific.h"
#include "local_context.h"
#include "io_controller.h"
#include "mcu_task_management/mcu_task_interface.h"

// --------------------------------------------------------------------------------

#define EVENT_RISE_TIME_MS                                                  50
#define EVENT_TIMEOUT_MS                                                    100
#define EVENT_QEUE_MAX_SIZE                                                 10

// --------------------------------------------------------------------------------

#define EVENT_TASK_SCHEDULE_INTERVAL_MS                                     5

// --------------------------------------------------------------------------------

typedef struct EVENT_QEUE_ELEMENT {
    SYSTEM_EVENT event_id;
    u32 timestamp;
} EVENT_QEUE_ELEMENT_TYPE;

/*!
 *
 */
typedef enum {
    EVENT_STATE_SLEEP = 0x00,
    EVENT_STATE_ACTIVATE,
    EVENT_STATE_RESET_QEUE,
    EVENT_STATE_WAIT_FOR_TIMEOUT,
    EVENT_STATE_FINISH,
} EVENT_HANDLER_STATE;

// --------------------------------------------------------------------------------

/*!
 *
 */
static EVENT_HANDLER_STATE actual_task_state = EVENT_STATE_SLEEP;

/*!
 *
 */
static EVENT_QEUE_ELEMENT_TYPE _event_qeue[EVENT_QEUE_MAX_SIZE];

/*!
 *
 */
static u8 _event_counter = 0;

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U32(operation_timer)

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
static void event_controller_task_start(void) {

}

/**
 * @brief 
 * 
 * @return u16 
 */
static u16 event_controller_task_get_schedule_interval(void) {
    return EVENT_TASK_SCHEDULE_INTERVAL_MS;
}

/**
 * @brief 
 * 
 * @return MCU_TASK_INTERFACE_TASK_STATE 
 */
static MCU_TASK_INTERFACE_TASK_STATE event_controller_task_get_state(void) {

    if (_event_counter != 0 || actual_task_state != EVENT_STATE_SLEEP) {

        PASS(); // event_controller_task_is_runable() --- 
        TRACE_byte(_event_counter); //
        TRACE_byte(actual_task_state); //

        return MCU_TASK_RUNNING;
    }

    return MCU_TASK_SLEEPING;
}

/**
 * @brief 
 * 
 */
static void event_controller_task_execute(void) {

    u8 i = 0;

    switch (actual_task_state) {

        case EVENT_STATE_SLEEP :

            if (_event_counter == 0) {
                break;
            }

            PASS(); // event_controller_task_run() - Event occured !!! ---
            actual_task_state = EVENT_STATE_ACTIVATE;
            // no break;

        case EVENT_STATE_ACTIVATE :

            #ifdef HAS_GPIO_EVENT_OUTPUT
            EVENT_OUTPUT_drive_high();
            #endif

            operation_timer_start();
            actual_task_state = EVENT_STATE_RESET_QEUE;

            // no break;

        case EVENT_STATE_RESET_QEUE :

            for ( ; i < EVENT_QEUE_MAX_SIZE; i++) {

                if (_event_qeue[i].event_id == SYS_EVT_NO_EVENT) {
                    continue;
                }

                if (i_system.time.isup_u32(_event_qeue[i].timestamp, EVENT_RISE_TIME_MS) == 0) {
                    TRACE_byte(_event_qeue[i].event_id); // event_controller_task_run() - Event still active !!! ---
                    continue;
                }

                TRACE_byte(_event_qeue[i].event_id); // event_controller_task_run() - Event removed !!! ---

                _event_qeue[i].event_id = SYS_EVT_NO_EVENT;
                _event_counter--;
            }

            actual_task_state = EVENT_STATE_WAIT_FOR_TIMEOUT;
            // no break;

        case EVENT_STATE_WAIT_FOR_TIMEOUT :

            if (operation_timer_is_up(EVENT_TIMEOUT_MS) == 0) {
                actual_task_state = EVENT_STATE_RESET_QEUE;
                break;
            }

            if (_event_counter != 0) {
                TRACE_byte(_event_counter); // event_controller_task_run() - Event still available
                actual_task_state = EVENT_STATE_ACTIVATE;
                break;
            }

            actual_task_state = EVENT_STATE_FINISH;
            // no break;

        default:
        case EVENT_STATE_FINISH :

            PASS(); // event_controller_task_run() - Event time is over

            #ifdef HAS_GPIO_EVENT_OUTPUT
            EVENT_OUTPUT_drive_low();
            #endif

            actual_task_state = EVENT_STATE_SLEEP;
            break;
    }
}

/**
 * @brief 
 * 
 */
static void event_controller_task_terminate(void) {

}

TASK_CREATE(
    EVENT_CTRL_TASK,
    TASK_PRIORITY_MIDDLE,
    event_controller_task_get_schedule_interval,
    event_controller_task_start,
    event_controller_task_execute,
    event_controller_task_get_state,
    event_controller_task_terminate
)

// --------------------------------------------------------------------------------

void event_controller_init(void) {
    EVENT_CTRL_TASK_init();
}

void local_event_add(SYSTEM_EVENT event) {

    if (_event_counter == EVENT_QEUE_MAX_SIZE) {
        PASS(); // local_event_add() - Event qeue is full
    }

    TRACE_byte((u8)event); // local_event_add()

    u8 i = 0;
    for ( ; i < EVENT_QEUE_MAX_SIZE; i++) {
        if (_event_qeue[i].event_id == SYS_EVT_NO_EVENT) {
            _event_qeue[i].event_id = event;
            _event_qeue[i].timestamp = i_system.time.now_u32();
            _event_counter++;
            return;
        }
    }
}

SYSTEM_EVENT local_event_get_next(void) {
    return SYS_EVT_NO_EVENT;
}

// --------------------------------------------------------------------------------
