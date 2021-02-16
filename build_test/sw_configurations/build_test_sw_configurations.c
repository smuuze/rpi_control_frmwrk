/**
 * @file 	unittest_json_parser.c
 * @author 	sebastian lesse (sebastian lesse)
 * @brief 
 * @version 	1.0
 * @date 	2021-02-06
 * 
 * @copyright 	Copyright (c) 2021
 * 
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "time_management/time_management.h"
#include "mcu_task_management/mcu_task_interface.h"

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(MAIN_RUN_TIMER)

// --------------------------------------------------------------------------------

int main(void) {

	MAIN_RUN_TIMER_start();

	while (MAIN_RUN_TIMER_is_up(1000) == 0) {
		mcu_task_controller_schedule();
	}

	return 0;
}
