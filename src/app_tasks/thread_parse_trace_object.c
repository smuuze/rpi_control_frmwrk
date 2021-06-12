/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	app_tasks/thread_parse_trace_object.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "cpu.h"

#include <stdio.h>

// --------------------------------------------------------------------------------

#include "common/common_types.h"
#include "common/local_module_status.h"
#include "common/qeue_interface.h"

#include "mcu_task_management/mcu_task_controller.h"
#include "mcu_task_management/thread_interface.h"

#include "tracer/trace_object.h"
#include "tracer/trace_parser.h"

// --------------------------------------------------------------------------------

#define PARSE_TRACE_OBJECT_STATUS_TERMINATED		(1 << 0)

BUILD_MODULE_STATUS_U8(PARSE_TRACE_OBJECT_STATUS)

// --------------------------------------------------------------------------------

QEUE_INTERFACE_INCLUDE_QEUE(RAW_TRACE_OBJECT_QEUE)
QEUE_INTERFACE_INCLUDE_QEUE(TRACE_OBJECT_QEUE)

// --------------------------------------------------------------------------------

void* thread_parse_trace_object_run(void* p_arg) {

	DEBUG_PASS("thread_parse_trace_object_run() - Thread started");
	    
	TRACE_OBJECT_RAW raw_obj;
	TRACE_OBJECT trace_obj;

	while (1) {

		usleep(50000); // reduce cpu-load

		if (PARSE_TRACE_OBJECT_STATUS_is_set(PARSE_TRACE_OBJECT_STATUS_TERMINATED)) {
			DEBUG_PASS("thread_parse_trace_object_run() - TERMINATE SIGNAL RECEIVED");
			break;
		}

		if (RAW_TRACE_OBJECT_QEUE_is_empty()) {
			continue;
		}

		if (RAW_TRACE_OBJECT_QEUE_mutex_get() == 0) {
			continue;
		}
			
		u8 object_available = RAW_TRACE_OBJECT_QEUE_deqeue(&raw_obj);
		RAW_TRACE_OBJECT_QEUE_mutex_release();

		if (object_available == 0) {
			continue;
		}

		if (tracer_parse_object(&raw_obj, &trace_obj) == 0) {
			DEBUG_PASS("thread_parse_trace_object_run() - Parsing Trace-Object has FAILED !!!");
			continue;
		}
			
		while (TRACE_OBJECT_QEUE_mutex_get() == 0) {
			//DEBUG_PASS("thread_parse_trace_object_run() - TRACE_OBJECT_QEUE is busy");
			//continue;
			usleep(50000); // reduce cpu-load
		}
			
		if (TRACE_OBJECT_QEUE_is_full()) {
			DEBUG_PASS("thread_parse_trace_object_run() - TRACE_OBJECT_QEUE is full");

		} else if (TRACE_OBJECT_QEUE_enqeue(&trace_obj)) {
			DEBUG_PASS("thread_parse_trace_object_run() - TRACE_OBJECT enqeued <<<");

		} else {
			DEBUG_PASS("thread_parse_trace_object_run() - TRACE_OBJECT enqeued has FAILED !!!");
		}
		
		TRACE_OBJECT_QEUE_mutex_release();
	}

	DEBUG_PASS("thread_parse_trace_object_run() - THREAD FINISHED");

	return NULL;
}

void thread_parse_trace_object_terminate(void) {
	DEBUG_PASS("thread_parse_trace_object_terminate()");
	PARSE_TRACE_OBJECT_STATUS_set(PARSE_TRACE_OBJECT_STATUS_TERMINATED);
}

// ------------------------------------------------------------------------------

THREAD_INTERFACE_BUILD_THREAD(PARSE_TRACE_OBJECT_THREAD, THREAD_PRIORITY_MIDDLE, thread_parse_trace_object_run, thread_parse_trace_object_terminate)
