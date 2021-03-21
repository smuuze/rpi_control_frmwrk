/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	test_tracer_mcu_task.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "time_management.h"
#include "system_interface.h"

#include "trx_driver_interface.h"
#include "mcu_task_interface.h"

// --------------------------------------------------------------------------------

void test_tracer_module_init(TRX_DRIVER_INTERFACE* p_driver) {
	(void) p_driver;
	
	PASS(); // test_tracer_module_init()
}

void test_tracer_task_init(void) {

	PASS(); // test_tracer_task_init()
}

u16 test_tracer_task_get_schdeule_interval(void) {
	return 0;
}

MCU_TASK_INTERFACE_TASK_STATE test_tracer_task_get_state(void) {

	return MCU_TASK_RUNNING;
}


void test_tracer_task_run(void) {

	u8 actual_time_u8 = i_system.time.now_u8();
	TRACE_byte(actual_time_u8); // test_tracer_task_run() -----------------

	u16 actual_time_u16 = i_system.time.now_u16();
	TRACE_word(actual_time_u16); // test_tracer_task_run() -----------------

	u32 actual_time_u32 = i_system.time.now_u32();
	TRACE_long(actual_time_u32); // test_tracer_task_run() -----------------

	u8 actual_time_array[] = {actual_time_u8 + 1, actual_time_u8 + 2, actual_time_u8 + 3, actual_time_u8 + 4, actual_time_u8 + 5, actual_time_u8 + 6, actual_time_u8 + 7, actual_time_u8 + 8};
	TRACE_N(8, actual_time_array); // test_tracer_task_run() -----------------
}

void test_tracer_task_background_run(void) {

}

void test_tracer_task_sleep(void) {

}

void test_tracer_task_wakeup(void) {

}

void test_tracer_task_finish(void) {

}

void test_tracer_task_terminate(void) {

}
