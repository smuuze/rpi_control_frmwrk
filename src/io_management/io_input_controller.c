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
 * @file    io_input_controller.c
 * @author  Sebastian Lesse
 * @date    2023 / 10 / 03
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

#include "driver/gpio/gpio_interface.h"
#include "system/system_interface.h"
#include "time_management/time_management.h"
#include "io_management/io_input_controller.h"
#include "mcu_task_management/mcu_task_interface.h"

// --------------------------------------------------------------------------------

#define IO_INPUT_CONTROLLER_TASK_RUN_INTERVAL_MS    30

#define BUTTON_IS_DOWN(p_button_state)                                          \
    (p_button_state->down == 1 ? 1 : 0)

#define BUTTON_IS_RELEASED(p_button_state, timestamp)                           \
    (p_button_state->released == 1 && p_button_state->__release_time + IO_BUTTON_RELEASED_TIMESPAN_MS < timestamp)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(task_timer)

// --------------------------------------------------------------------------------

/*!
 *
 */
static IO_INPUT_DESCRIPTOR* p_first_button = 0;

/*!
 *
 */
static IO_INPUT_DESCRIPTOR* p_last_button = 0;

// --------------------------------------------------------------------------------

static void _init_state(IO_INPUT_DESCRIPTOR* p_button_state) {

    p_button_state->__down_time = 0;
    p_button_state->__press_time = 0;
    p_button_state->__release_time = 0;

    p_button_state->down = 0;
    p_button_state->pressed = 0;
    p_button_state->released = 0;
}

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
static void io_input_controller_task_start(void) {
    DEBUG_PASS("io_input_controller_task_start()");
    task_timer_start();
}

/**
 * @brief 
 * 
 * @return u16 
 */
static u16 io_input_controller_task_get_schedule_interval(void) {
    return 5;
}

/**
 * @brief 
 * 
 * @return MCU_TASK_INTERFACE_TASK_STATE 
 */
static MCU_TASK_INTERFACE_TASK_STATE io_input_controller_task_get_state(void) {

    if (task_timer_is_up(IO_INPUT_CONTROLLER_TASK_RUN_INTERVAL_MS) != 0) {
        return MCU_TASK_RUNNING;
    } else {
        return MCU_TASK_SLEEPING;
    }
}

/**
 * @brief 
 * 
 */
static void io_input_controller_task_execute(void) {

    IO_INPUT_DESCRIPTOR* p_act_button = p_first_button;

    while (p_act_button != 0) {
        io_input_controller_check_state(p_act_button);
        p_act_button = p_act_button->__next_button;
    }

    task_timer_start();
}

/**
 * @brief 
 * 
 */
static void io_input_controller_task_terminate(void) {

}

TASK_CREATE(
    IO_INPUT_TASK,
    TASK_PRIORITY_MIDDLE,
    io_input_controller_task_get_schedule_interval,
    io_input_controller_task_start,
    io_input_controller_task_execute,
    io_input_controller_task_get_state,
    io_input_controller_task_terminate
)

// --------------------------------------------------------------------------------

void io_input_controller_init(void) {
    DEBUG_PASS("io_input_controller_init()");
    IO_INPUT_TASK_init();
}

u8 io_input_controller_register_input(IO_INPUT_DESCRIPTOR* p_new_button) {

    PASS(); // io_input_controller_register_input()

    if (p_first_button == 0) {

        p_first_button = p_new_button;
        p_last_button = p_new_button;
        p_last_button->__next_button = 0;
        p_last_button->id = 1;

        TRACE_byte(p_last_button->id); // button_watcher_register_button() - first button registered

    } else {

        u8 prev_id = p_last_button->id;
        p_last_button->__next_button = p_new_button;
        p_last_button = p_last_button->__next_button;
        p_last_button->id = prev_id + 1;
        p_last_button->__next_button = 0;

        TRACE_byte(p_last_button->id); // button_watcher_register_button() - new button registered
    }

    _init_state(p_last_button);

    return p_last_button->id;
}

void io_input_controller_check_state(IO_INPUT_DESCRIPTOR* p_button_state) {

    //PASS(); // io_input_controller_check_state() ----------------

    GPIO_DRIVER_LEVEL level = i_system.io.get_level(p_button_state->pin_descriptor); //p_button_state->__pin_state();

    if (level == GPIO_LEVEL_HIGH) {

        if (p_button_state->down == 0) {

            p_button_state->down = 1;
            p_button_state->__down_time = i_system.time.now_u32();

            p_button_state->pressed = 1;
            p_button_state->__press_time = i_system.time.now_u16();

            p_button_state->released = 0;
            p_button_state->__release_time = 0;

            TRACE_byte(p_button_state->id); // button_watcher_check_state() - button just has been pressed

            i_system.event.add(SYS_EVT_INPUT_CHANGED);

        }

        if (p_button_state->down != 0 && p_button_state->pressed != 0) {

            if (i_system.time.isup_u16(p_button_state->__press_time, IO_BUTTON_PRESSED_TIMESPAN_MS)) {
                p_button_state->pressed = 0;
                TRACE_byte(p_button_state->id); // button_watcher_check_state() - button in active mode
            }
        }

    } else {

        if (p_button_state->down != 0 && p_button_state->released == 0) {

            p_button_state->released = 1;
            p_button_state->__release_time = i_system.time.now_u16();

            TRACE_byte(p_button_state->id); // button_watcher_check_state() - button just has been released
            TRACE_long(p_button_state->__down_time); // button_watcher_check_state() - Down Time in MS

            i_system.event.add(SYS_EVT_INPUT_CHANGED);

        } else if (p_button_state->__release_time != 0 && i_system.time.isup_u16(p_button_state->__release_time, IO_BUTTON_RELEASED_TIMESPAN_MS)) {

            p_button_state->released = 0;
            p_button_state->__release_time = 0;
            p_button_state->__press_time = 0;

            TRACE_byte(p_button_state->id); // button_watcher_check_state() - button in inactive mode
        }

        p_button_state->down = 0;
        p_button_state->pressed = 0;
        p_button_state->__press_time = 0;
        p_button_state->__down_time = 0;
    }
}

void io_input_controller_get_state(u8 button_id, IO_INPUT_STATE* p_button_state) {

    IO_INPUT_DESCRIPTOR* p_act_button = p_first_button;

    while (p_act_button != 0) {

        if (p_act_button->id == button_id) {

            p_button_state->down = p_act_button->down;
            p_button_state->pressed = p_act_button->pressed;
            p_button_state->released = p_act_button->released;

            if (p_button_state->down) {
                p_button_state->down_time = i_system.time.now_u32() - p_act_button->__down_time;
            } else {
                p_button_state->down_time = 0;
            }

            // states have been read, so these events are out-dated
            p_act_button->released = 0;
            p_act_button->pressed = 0;

            return;
        }

        p_act_button = p_act_button->__next_button;
    }

    p_button_state->down = 0;
    p_button_state->pressed = 0;
    p_button_state->released = 0;
    p_button_state->down_time = 0;
}

// --------------------------------------------------------------------------------

void io_input_controller_get_iterator(ITERATOR_INTERFACE* p_iterator) {

    if (p_first_button == 0) {
        p_iterator->is_valid = 0;
        p_iterator->is_last  = 0;
        p_iterator->is_first = 0;
        return;
    }

    p_iterator->__element = (void*) p_first_button;
    p_iterator->is_first = 1;
    p_iterator->is_valid = 1;

    if (p_first_button->__next_button != 0) {
        p_iterator->is_last = 0;
    } else {
        p_iterator->is_last = 1;
    }
}

void io_input_controller_iterator_get_element(ITERATOR_INTERFACE* p_iterator, IO_INPUT_STATE* p_button_state) {

    if (p_iterator->is_valid == 0) {
        return;
    }

    IO_INPUT_DESCRIPTOR* p_button = (IO_INPUT_DESCRIPTOR*)p_iterator->__element;

    p_button_state->id = p_button->id;
    p_button_state->down = p_button->down;
    p_button_state->pressed = p_button->pressed;
    p_button_state->released = p_button->released;

    if (p_button_state->down) {
        p_button_state->down_time = i_system.time.now_u32() - p_button->__down_time;
    } else {
        p_button_state->down_time = 0;
    }
}

void io_input_controller_iterator_get_next(ITERATOR_INTERFACE* p_iterator) {

    IO_INPUT_DESCRIPTOR* p_button = (IO_INPUT_DESCRIPTOR*)p_iterator->__element;

    if (p_button == 0) {
        p_iterator->is_valid = 0;
        p_iterator->is_last  = 0;
        p_iterator->is_first = 0;
        return;
    }

    if (p_button->__next_button == 0) {
        p_iterator->is_valid = 0;
        p_iterator->is_last  = 0;
        p_iterator->is_first = 0;
        return;
    }

    p_button = p_button->__next_button;

    p_iterator->__element = (void*) p_button;
    p_iterator->is_first = 0;
    p_iterator->is_valid = 1;

    if (p_button->__next_button != 0) {
        p_iterator->is_last = 0;
    } else {
        p_iterator->is_last = 1;
    }
}

// --------------------------------------------------------------------------------
