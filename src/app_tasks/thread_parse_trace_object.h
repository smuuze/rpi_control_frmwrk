/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	thread_parse_trace_object.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */
 
#ifndef _THREAD_PARSE_TRACE_OBJECT_H_
#define _THREAD_PARSE_TRACE_OBJECT_H_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "mcu_task_management/thread_interface.h"

/*!
 *
 */
void* thread_parse_trace_object_run(void* p_arg);

/*!
 *
 */
void thread_parse_trace_object_terminate(void);

/*!
 *
 */
THREAD_INTERFACE_INCLUDE_THREAD(PARSE_TRACE_OBJECT_THREAD)


#endif // _SHC_THREAD_PARSE_TRACE_OBJECT_H_
