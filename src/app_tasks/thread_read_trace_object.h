/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	thread_read_trace_object.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */
 
#ifndef _THREAD_READ_TRACE_OBJECT_H_
#define _THREAD_READ_TRACE_OBJECT_H_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "mcu_task_management/thread_interface.h"

// --------------------------------------------------------------------------------

#include "driver/trx_driver_interface.h"

// --------------------------------------------------------------------------------

/*!
 *
 */
void* thread_read_trace_object_run(void* p_arg);

/*!
 *
 */
void thread_read_trace_object_terminate(void);

/**
 * @brief sets the communication driver to use for reading traces.
 * This module will not work unless a driver has been set.
 * The driver needs to be initialized and configured.
 * 
 * @param p_driver pointer to the driver-interface to use (already initialized and configured)
 */
void thread_read_trace_object_set_com_driver(TRX_DRIVER_INTERFACE* p_driver);

// --------------------------------------------------------------------------------

/*!
 *
 */
THREAD_INTERFACE_INCLUDE_THREAD(READ_TRACE_OBJECT_THREAD)


#endif // _THREAD_READ_TRACE_OBJECT_H_
