/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	app_tasks/thread_read_trace_object.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

#ifdef TRACER_ON
#pragma __WARNING__TRACES_ENABLED__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "cpu.h"

#include <stdio.h>

// --------------------------------------------------------------------------------

#include "common/signal_slot_interface.h"
#include "common/common_types.h"
#include "common/common_tools_number.h"
#include "common/qeue_interface.h"

#include "mcu_task_management/mcu_task_controller.h"
#include "mcu_task_management/thread_interface.h"

#include "tracer/trace_object.h"

#include "driver/trx_driver_interface.h"
#include "driver/cfg_driver_interface.h"

#include "system/system_interface.h"

// ------------------------------------------------------------------------------

#ifndef TRACER_READ_RAW_OBJECT_TIMEOUT_MS
#define TRACER_READ_RAW_OBJECT_TIMEOUT_MS					25
#endif

// ------------------------------------------------------------------------------

QEUE_INTERFACE_INCLUDE_QEUE(RAW_TRACE_OBJECT_QEUE)

// --------------------------------------------------------------------------------

static TRX_DRIVER_INTERFACE* p_com_driver = NULL;

// --------------------------------------------------------------------------------

static u8 main_read_trace_object_raw(TRACE_OBJECT_RAW* p_raw_obj) {

	u16 length = p_com_driver->bytes_available();
	//usart_read_bytes(p_com_driver, TRACE_PARSER_NUM_BYTES_HEADER, p_raw_obj->data, DEFAULT_USART_TIMEOUT_MS);
	if (length < TRACE_PARSER_NUM_BYTES_HEADER) {
		//THREAD_DEBUG_MSG("main_read_trace_object_raw() - usart_read_bytes(HEADER) has FAILED !!!\n");
		return 0;
	}

	p_raw_obj->length = 0;

	// ----- Read Trace-Header -----

	//p_raw_obj->length = length;
	p_com_driver->get_N_bytes(TRACE_PARSER_NUM_BYTES_HEADER, p_raw_obj->data);

	u8 header[] = {TRACE_PARSER_HEADER_BYTE_ARRAY};
	while (memcmp(p_raw_obj->data, header, TRACE_PARSER_NUM_BYTES_HEADER) != 0) {

		if (p_com_driver->bytes_available() == 0) {
			DEBUG_PASS("main_read_trace_object_raw() - Matching header has FAILED !!!");
			return 0;
		}

		p_raw_obj->data[0] = p_raw_obj->data[1];
		p_raw_obj->data[1] = p_raw_obj->data[2];
		p_com_driver->get_N_bytes(1, p_raw_obj->data + 2);
	}
	
	DEBUG_PASS("main_read_trace_object_raw() - HEADER found - reading data");
	p_raw_obj->length += TRACE_PARSER_NUM_BYTES_HEADER;

	// ----- Read Trace-Number-Of-Bytes -----

	length = p_com_driver->bytes_available();
	if (length < TRACE_PARSER_NUM_BYTES_BYTE_COUNT) {

		p_com_driver->wait_for_rx(TRACE_PARSER_NUM_BYTES_BYTE_COUNT, TRACER_READ_RAW_OBJECT_TIMEOUT_MS);
		length = p_com_driver->bytes_available();

		if (length < TRACE_PARSER_NUM_BYTES_BYTE_COUNT) {
			DEBUG_PASS("main_read_trace_object_raw() - Waiting for number of bytes has FAILED !!!");
			p_raw_obj->length = 0;
			return 0;
		}
	}

	p_com_driver->get_N_bytes(TRACE_PARSER_NUM_BYTES_BYTE_COUNT, p_raw_obj->data + TRACE_PARSER_NUM_BYTES_HEADER);

	p_raw_obj->length += TRACE_PARSER_NUM_BYTES_BYTE_COUNT;
	length = common_tools_number_readU16_MSB(p_raw_obj->data + TRACE_PARSER_NUM_BYTES_HEADER);

	DEBUG_TRACE_word(length, "main_read_trace_object_raw() - Number of bytes to read");

	// CHECK FOR BUFFER-OVERFLOW !!! ---

	if (length > TRACE_OBJECT_RAW_DATA_LENGTH - (TRACE_PARSER_NUM_BYTES_BYTE_COUNT + TRACE_PARSER_NUM_BYTES_HEADER)) {
		DEBUG_TRACE_word(length, "main_read_trace_object_raw() - Too much bytes");
		length = TRACE_OBJECT_RAW_DATA_LENGTH - (TRACE_PARSER_NUM_BYTES_BYTE_COUNT + TRACE_PARSER_NUM_BYTES_HEADER);
	}

	// ----- Read Trace-Data -----

	if (p_com_driver->bytes_available() < length) {

		p_com_driver->wait_for_rx(length, TRACER_READ_RAW_OBJECT_TIMEOUT_MS);

		if (p_com_driver->bytes_available() < length) {
			DEBUG_PASS("main_read_trace_object_raw() - Waiting for trace-data has FAILED !!!");
			p_raw_obj->length = 0;
			return 0;
		}
	}

	p_com_driver->get_N_bytes(length, p_raw_obj->data + TRACE_PARSER_NUM_BYTES_HEADER + TRACE_PARSER_NUM_BYTES_BYTE_COUNT);
	if (length == 0) {
		DEBUG_PASS("main_read_trace_object_raw() - usart_read_bytes(PAYLOAD) has FAILED !!!");
		p_raw_obj->length = 0;
		return 0;
	}

	p_raw_obj->length += length;
	return 1;
}

void* thread_read_trace_object_run(void* p_arg) {

	DEBUG_PASS("thread_read_trace_object_run() - Thread started");

	TRACE_OBJECT_RAW raw_obj;

	while (1) {

		usleep(50000); // reduce cpu-load

		if (p_com_driver == NULL) {
			continue;
		}

		if (main_read_trace_object_raw(&raw_obj) == 0) {
			continue;
		}

		if (RAW_TRACE_OBJECT_QEUE_mutex_get() == 0) {
			DEBUG_PASS("thread_read_trace_object_run() - QEUE is busy");
			continue;
		}
		
		if (RAW_TRACE_OBJECT_QEUE_is_full()) {
			DEBUG_PASS("thread_read_trace_object_run() - QEUE is full");
		} else if (RAW_TRACE_OBJECT_QEUE_enqeue(&raw_obj)) {
			//THREAD_DEBUG_MSG("thread_read_trace_object_run() - Object enqeued <<<\n");
		} else {
			DEBUG_PASS("thread_read_trace_object_run() - Object enqeued has FAILED !!!");
		}

		RAW_TRACE_OBJECT_QEUE_mutex_release();
	}

	return NULL;
}

void thread_read_trace_object_terminate(void) {

}

void thread_read_trace_object_set_com_driver(TRX_DRIVER_INTERFACE* p_driver) {
	
	p_com_driver = p_driver; //(const TRX_DRIVER_INTERFACE*) i_system.driver.usart0;
	//p_com_driver->initialize();
	//p_com_driver->configure(&usart0_config);
}

// --------------------------------------------------------------------------------

THREAD_INTERFACE_BUILD_THREAD(READ_TRACE_OBJECT_THREAD, THREAD_PRIORITY_MIDDLE, thread_read_trace_object_run, thread_read_trace_object_terminate)
