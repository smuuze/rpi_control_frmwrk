/*! 
 * ------------------------------------------------------------------------------
 *
 * \file	thread_interface.c
 * \brief
 * \author	sebastian lesse
 *
 * ------------------------------------------------------------------------------
 */

#define TRACER_OFF

#ifdef TRACER_ON
#pragma __WARNING__TRACES_ENABLED__
#endif

// ------------------------------------------------------------------------------

#include "config.h"

// ------------------------------------------------------------------------------

#include "tracer.h"

// ------------------------------------------------------------------------------

#include "cpu.h"

// ------------------------------------------------------------------------------

#include "mcu_task_management/thread_interface.h"

// ------------------------------------------------------------------------------

void thread_interface_build(THREAD_INTERFACE_TYPE* p_thread,
	THREAD_INTERFACE_PRITORITY prio, THREAD_INTERFACE_INIT_CALLBACK p_init,
	THREAD_INTERFACE_RUN_FUNCTION_CALLBACK p_run, THREAD_INTERFACE_TERMINATE_CALLBACK p_terminate) {

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
	}
}
