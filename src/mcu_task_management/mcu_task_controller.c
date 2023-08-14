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

#include "system/system_interface.h"

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

    p_mcu_task->last_run_time = i_system.time.now_u16();
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

        if (act_task->get_sate() == MCU_TASK_SLEEPING) {
            //DEBUG_TRACE_byte(act_task->identifier, "mcu_task_controller_schedule() - Task is not runnable");
            goto SKIP_TASK_schedule;
        }

        //DEBUG_TRACE_byte(act_task->identifier, "mcu_task_controller_schedule() - Running Task");

        //act_task->last_run_time = i_system.time.now_u16();
        act_task->run();

        //DEBUG_PASS("mcu_task_controller_schedule() - Task complete");

        //SKIP_TASK_schedule :

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

static u8 _has_task_interval_passed(MCU_TASK_INTERFACE* p_task) {

    if (p_task->get_schedule_interval() == MCU_TASK_SCHEDULE_NO_TIMEOUT) {
        return 1;
    }

    if (i_system.time.isup_u16(p_task->last_run_time, p_task->new_run_timeout) == 0) {
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

    u16 actual_time_ms = i_system.time.now_u16();
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
