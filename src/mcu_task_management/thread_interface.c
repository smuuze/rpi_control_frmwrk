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
 * @file    thread_interface.c
 * @author  Sebastian Lesse
 * @date    2022 / 07 / 16
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

#include "mcu_task_management/thread_interface.h"

// --------------------------------------------------------------------------------

void thread_interface_build(
    THREAD_INTERFACE_TYPE* p_thread,
    THREAD_INTERFACE_PRITORITY prio,
    THREAD_INTERFACE_INIT_CALLBACK p_init,
    THREAD_INTERFACE_RUN_FUNCTION_CALLBACK p_run,
    THREAD_INTERFACE_TERMINATE_CALLBACK p_terminate) {

    DEBUG_PASS("thread_interface_build()");

    p_thread->priority = prio;
    p_thread->init = p_init;
    p_thread->run = p_run;
    p_thread->terminate = p_terminate;
}

void thread_interface_init(THREAD_INTERFACE_TYPE* p_thread) {

    if (p_thread->init != NULL) {
        p_thread->init();
    }
}

u8 thread_interface_create_thread(THREAD_INTERFACE_TYPE* p_thread) {
    
    if (p_thread->run == NULL) {
        DEBUG_PASS("thread_interface_create_thread() - NULL_POINTER_EXCEPTION - run()");
        return 0;
    }

    int err = pthread_create(&p_thread->id, NULL, p_thread->run, NULL);
        if (err != 0) {
        DEBUG_TRACE_long(err, "thread_interface_create_thread() - Starting thread has FAILED !!! ---");
        return 0;
    }

    DEBUG_PASS("thread_interface_create_thread() - Thread started");
        
    return 1;
}

void thread_interface_terminate(THREAD_INTERFACE_TYPE* p_thread) {

    if (p_thread->terminate != NULL) {
        p_thread->terminate();
        pthread_join(p_thread->id, NULL);
        DEBUG_TRACE_long(p_thread->exit_status, "thread_interface_terminate() - Thread finished with: ");
    }
}

// --------------------------------------------------------------------------------
