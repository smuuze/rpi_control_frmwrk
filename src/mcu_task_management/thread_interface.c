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

// ------------------------------------------------------------------------------

#include "config.h"

// ------------------------------------------------------------------------------

#include "tracer.h"

// ------------------------------------------------------------------------------

#include "cpu.h"

// ------------------------------------------------------------------------------

#include "mcu_task_management/thread_interface.h"

// ------------------------------------------------------------------------------

u8 thread_interface_create_thread(THREAD_INTERFACE_TYPE* p_thread) { 
	
	int err = pthread_create(&p_thread->id, NULL, p_thread->run, NULL);
        if (err != 0) {
		DEBUG_TRACE_long(err, "thread_interface_create_thread() - Starting thread has FAILED !!! ---");
		return 0;
	}

	DEBUG_PASS("thread_interface_create_thread() - Thread started");
	    
	return 0;
}

u8 thread_interface_terminate(THREAD_INTERFACE_TYPE* p_thread) {

	if (p_thread->terminate != NULL) {
		p_thread->terminate();
	}
	return 0;
}
