/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	main_rpi_hat.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_ON

// --------------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "driver/timer/timer0_driver.h"

// --------------------------------------------------------------------------------

void ir_remote_task_init(void) {
	timer0_driver_init();
}

u8 ir_remote_task_is_runable(void) {
	return 0;
}

void ir_remote_task_run(void) {
	
}

void ir_remote_task_background_run(void) {

}

void ir_remote_task_sleep(void) {

}

void ir_remote_task_wakeup(void) {

}

void ir_remote_task_finish(void) {

}

void ir_remote_task_terminate(void) {

}
