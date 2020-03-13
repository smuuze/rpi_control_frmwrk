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

#include "mcu_task_management/mcu_task_interface.h"
#include "time_management/time_management.h"

#include "driver/timer/timer0_driver.h"
#include "driver/timer/timer1_driver.h"

#include "driver/timer/timer_interface.h"

// --------------------------------------------------------------------------------

#define IR_REMOTE_TASK_CHANGE_FREQ_INTERVAL_MS		5000

// --------------------------------------------------------------------------------

static TIMER_INTERFACE_TYPE timer_carrier = {
	.init = &timer0_driver_init,
	.configure = &timer0_driver_configure,
	.start = &timer0_driver_start,
	.stop = &timer0_driver_stop
};

static TIMER_INTERFACE_TYPE timer_modulator = {
	.init = &timer1_driver_init,
	.configure = &timer1_driver_configure,
	.start = &timer1_driver_start,
	.stop = &timer1_driver_stop
};

// --------------------------------------------------------------------------------

void ir_remote_task_init(void) {

	DEBUG_PASS("ir_remote_task_init()");

	timer_carrier.init();
	timer_modulator.init();
	
	ir_protocol_samsung_set_timer(&timer_carrier, &timer_modulator);
	
}

u16 ir_remote_task_get_schedule_interval(void) {
	return 0;
}

MCU_TASK_INTERFACE_TASK_STATE ir_remote_task_get_state(void) {

	return MCU_TASK_RUNNING;
}

void ir_remote_task_run(void) {
	
	if (CHANGE_FREQ_TIMER_is_up(IR_REMOTE_TASK_CHANGE_FREQ_INTERVAL_MS) == 0) {
		return;
	}

	CHANGE_FREQ_TIMER_start();
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
