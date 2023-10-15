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
 * @file    mcu_task_controller.c
 * @author  Sebastian Lesse
 * @date    2022 / 09 / 24
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

#include "driver/rtc/rtc_driver_interface.h"
#include "common/local_module_status.h"

#include "mcu_task_management/mcu_task_interface.h"
#include "mcu_task_management/mcu_task_controller.h"
#include "mcu_task_management/mcu_idle_task.h"

// --------------------------------------------------------------------------------

TASK_CREATE(
    IDLE_TASK,
    TASK_PRIORITY_VERY_LOW,
    mcu_idle_task_get_schedule_interval,
    mcu_idle_task_init,
    mcu_idle_task_run,
    mcu_idle_task_get_state,
    mcu_idle_task_terminate
)

// --------------------------------------------------------------------------------

#define TASL_CONTROLLER_MODULE_STATUS_SIZE      2

BUILD_MODULE_STATUS_FAST(
    TASK_COMTROLLER_STATUS,
    TASL_CONTROLLER_MODULE_STATUS_SIZE
)

/**
 * @brief THis flag enables the task statistic feature
 * E.g. measurement the runtime of a task.
 */
#define TASK_CTRL_STATUS_STATS_ON     0

// --------------------------------------------------------------------------------

/**
 * @brief 
 */
static MCU_TASK_INTERFACE* _first_task = 0;

/**
 * @brief 
 */
static MCU_TASK_INTERFACE* _last_task = 0;

/**
 * @brief 
 */
static u8 _has_task_interval_passed(MCU_TASK_INTERFACE* p_task);

/**
 * @brief 
 */
static void _update_last_run_time(MCU_TASK_INTERFACE* p_task);

/**
 * @brief 
 */
static u32 _minimum_taks_schedule_interval = 0xFFFFFFFF;

// --------------------------------------------------------------------------------

/**
 * @brief Starts iteration over all available tasks.
 * Sets the given iterator-interface to the first element, if available.
 * 
 * @param p_iterator reference to the iterater to be used 
 * @param p_data reference to the memory where the first element is copied into
 * @return 1 if there are at least one element is available, otherweise 0
 */
static u8 mcu_task_controller_iter_start(ITERATOR_INTERFACE* p_iterator, void* p_data) {

    if (_first_task != 0) {

        TASK_CTRL_STATS* p_task = (TASK_CTRL_STATS*)p_data;
        p_task->last_runtime = _first_task->last_active_time;

        p_iterator->is_first = 1;
        p_iterator->is_last = 0;
        p_iterator->is_valid = 1;
        p_iterator->__element = _first_task;

        return 1;

    } else {
        return 0;
    }
}

/**
 * @brief Sets the given iterator to the next element in the current list, 
 * if available.
 * 
 * @param p_iterator reference to the iterater to be used 
 * @param p_data reference to the memory where the next element is copied into
 * @return 1 if there is a next element available, otherwise 0
 */
static u8 mcu_task_controller_iter_next(ITERATOR_INTERFACE* p_iterator, void* p_data) {

    if (p_iterator->is_valid == 0) {
        return 0;
    }

    if (p_iterator->is_last) {
        return 0;
    }

    if (p_iterator->__element == 0) {
        return 0;
    }

    MCU_TASK_INTERFACE* p_act_task = (MCU_TASK_INTERFACE*)p_iterator->__element;
    p_iterator->__element = p_act_task->next_task;

    if (p_iterator->__element == 0) {

        p_iterator->is_first = 0;
        p_iterator->is_last = 0;
        p_iterator->is_valid = 0;
        return 0;
    }

    p_act_task = p_act_task->next_task;

    TASK_CTRL_STATS* p_task = (TASK_CTRL_STATS*)p_data;
    p_task->last_runtime = p_act_task->last_active_time;

    p_iterator->is_first = 0;
    p_iterator->is_last = p_iterator->__element != 0;
    p_iterator->is_valid = 1;

    return 1;
}

ITERATOR_INTERFACE_CREATE(
    MCU_TASK_ITERATOR_INTERFACE,
    mcu_task_controller_iter_start,
    mcu_task_controller_iter_next
)

// --------------------------------------------------------------------------------

static u8 _has_task_interval_passed(MCU_TASK_INTERFACE* p_task) {

    if (p_task->get_schedule_interval() == MCU_TASK_SCHEDULE_NO_TIMEOUT) {
        return 1;
    }

    if (rtc_timer_istimeup_u16(p_task->last_run_time, p_task->new_run_timeout) == 0) {
        return 0;
    }

    return 1;
}

// --------------------------------------------------------------------------------

static void _update_last_run_time(MCU_TASK_INTERFACE* p_task) {

    if (p_task->get_schedule_interval() == MCU_TASK_SCHEDULE_NO_TIMEOUT) {
        return;
    }

    //DEBUG_PASS("_update_last_run_time() 

    u16 actual_time_ms = rtc_timer_gettime_u16();
//    p_task->new_run_timeout = p_task->get_schedule_interval();
//    u16 jitter = actual_time_ms - p_task->last_run_time;
//
//    while (jitter > p_task->get_schedule_interval()) {
//        jitter -= p_task->get_schedule_interval();
//    }
//
//    DEBUG_TRACE_word(p_task->last_run_time, "last runtime");
//    DEBUG_TRACE_word(actual_time_ms, "actual time");
//    DEBUG_TRACE_word(p_task->get_schedule_interval(), "schedule interval");
//    DEBUG_TRACE_word(jitter, "jitter");
//
//    u16 next_scheduled_run    = p_task->last_run_time - jitter;
//    while (next_scheduled_run < actual_time_ms) {
//        next_scheduled_run += p_task->get_schedule_interval();
//    }

    p_task->new_run_timeout = p_task->get_schedule_interval();// - jitter;
    p_task->last_run_time    = actual_time_ms;

//    DEBUG_TRACE_word(p_task->new_run_timeout, "new run timeout");
//    DEBUG_TRACE_word(next_scheduled_run, "nest schedule run");
}

// --------------------------------------------------------------------------------

void mcu_task_controller_init(void) {

    DEBUG_PASS("mcu_task_controller_init()");

    _first_task = 0;
    _last_task = 0;
}

// --------------------------------------------------------------------------------

void mcu_task_controller_register_task(MCU_TASK_INTERFACE* p_mcu_task) {

    static u8 new_task_id = 1;

    p_mcu_task->next_task = 0;
    p_mcu_task->identifier = new_task_id;
    new_task_id += 1;

    if (p_mcu_task->init != NULL) {
        p_mcu_task->init();
    }

    p_mcu_task->last_run_time = rtc_timer_gettime_u16();
    p_mcu_task->new_run_timeout = p_mcu_task->get_schedule_interval();

    if (p_mcu_task->get_schedule_interval() < _minimum_taks_schedule_interval) {
        _minimum_taks_schedule_interval = p_mcu_task->get_schedule_interval();
    }

    if (_first_task == 0) {
        _first_task = p_mcu_task;
        _last_task = p_mcu_task;

    } else {
        _last_task->next_task = p_mcu_task;
        _last_task = p_mcu_task;
    }

    _last_task->last_active_time = 0;

    DEBUG_TRACE_byte(
        _last_task->identifier,
        "mcu_task_controller_register_task() - new task added"
    );
}

// --------------------------------------------------------------------------------

void mcu_task_controller_schedule(void) {

    MCU_TASK_INTERFACE* act_task = _first_task;

    u8 system_is_on_idle = 1;

    //DEBUG_PASS("// mcu_task_controller_schedule()

    while (act_task != 0) {

        if (_has_task_interval_passed(act_task) == 0) {
            goto SKIP_TASK_schedule;
        }

        _update_last_run_time(act_task);

        if (act_task->get_sate() != MCU_TASK_SLEEPING) {
            //DEBUG_TRACE_byte(act_task->identifier, "mcu_task_controller_schedule() - Task is not runnable");
            goto SKIP_TASK_schedule;
        }

        //DEBUG_TRACE_byte(act_task->identifier, "mcu_task_controller_schedule() - Running Task");
 
        if (TASK_COMTROLLER_STATUS_is_set(TASK_CTRL_STATUS_STATS_ON)) {
            act_task->last_active_time = rtc_timer_get_usec();
            act_task->run();
            act_task->last_active_time += (rtc_timer_get_usec() - act_task->last_active_time);
        } else {
            act_task->run();
        }

        //DEBUG_PASS("mcu_task_controller_schedule() - Task complete");

        if (act_task->get_sate() != MCU_TASK_SLEEPING) {
            system_is_on_idle = 0;
            DEBUG_PASS("mcu_task_controller_schedule() - Task is still active");
        }

        /*
        if (!system_is_on_idle) {
            DEBUG_PASS("mcu_task_controller_schedule() - System will stay awake");
        }
        */

        SKIP_TASK_schedule :

        act_task = act_task->next_task;
    }

    if (system_is_on_idle != 0) {
        //DEBUG_PASS("mcu_task_controller_schedule() xxxxxxx SYSTEM GOING TO SLEEP xxxxxxxxxx");
        IDLE_TASK_run();
    }
}

// --------------------------------------------------------------------------------

void mcu_task_controller_sleep(void) {

    MCU_TASK_INTERFACE* act_task = _first_task;

    while (act_task != 0) {

        if (act_task->sleep == 0) {
            goto SKIP_TASK_sleep;
        }

        act_task->sleep();

        SKIP_TASK_sleep :
        act_task = act_task->next_task;
    }
}

// --------------------------------------------------------------------------------

void mcu_task_controller_wakeup(void) {

    MCU_TASK_INTERFACE* act_task = _first_task;

    while (act_task != 0) {

        if (act_task->wakeup == 0) {
            goto SKIP_TASK_wakeup;
        }

        act_task->wakeup();

        SKIP_TASK_wakeup :
        act_task = act_task->next_task;
    }
}

// --------------------------------------------------------------------------------

void mcu_task_controller_terminate_all(void) {

    MCU_TASK_INTERFACE* act_task = _first_task;

    while (act_task != 0) {

        if (act_task->terminate == 0) {
            goto SKIP_TASK_terminate;
        }

        act_task->terminate();

        SKIP_TASK_terminate :
        act_task = act_task->next_task;
    }
}

// --------------------------------------------------------------------------------

void mcu_task_controller_background_run(void) {

    MCU_TASK_INTERFACE* act_task = _first_task;

    while (act_task != 0) {

        if (act_task->background_run == 0) {
            goto SKIP_TASK_background_run;
        }

        act_task->background_run();

        SKIP_TASK_background_run :
        act_task = act_task->next_task;
    }
}

// --------------------------------------------------------------------------------

void test_iteration(void) {

    ITERATE_FOR(
        TASK_CTRL_STATS,
        task_stats,
        MCU_TASK_ITERATOR_INTERFACE_iter(),

        do{}while(0);
    )
}

// --------------------------------------------------------------------------------

/**
 * @see mcu_task_controller.h#mcu_task_controller_enable_statistics
 */
void mcu_task_controller_enable_statistics(TASK_CTRL_STATISTIC_EN enable) {
    if (enable == TASK_CTRL_STATISTIC_ON) {
        TASK_COMTROLLER_STATUS_set(TASK_CTRL_STATUS_STATS_ON);
    } else {
        TASK_COMTROLLER_STATUS_unset(TASK_CTRL_STATUS_STATS_ON);
    }
}

// --------------------------------------------------------------------------------
