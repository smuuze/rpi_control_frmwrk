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
#include "driver/timer/timer0_driver.h"
#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

#define IR_REMOTE_TASK_CHANGE_FREQ_INTERVAL_MS		5000

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(CHANGE_FREQ_TIMER)

static TIMER_CONFIGURATION_TYPE timer_config;

// --------------------------------------------------------------------------------

void timer0_channela_irq_callback(void) {
	IR_CARRIER_OUT_toggle_level();
}

// --------------------------------------------------------------------------------

void ir_remote_task_init(void) {

	DEBUG_PASS("ir_remote_task_init()");
	
	timer_config.frequency = TIMER_FREQUENCY_36kHz;
	timer_config.irq_callback = 0;
	timer_config.mode = TIMER_MODE_FREQUENCY;

	timer0_driver_init();
	timer0_driver_configure(&timer_config);
	timer0_driver_start();

	CHANGE_FREQ_TIMER_start();
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

	if (timer_config.frequency == TIMER_FREQUENCY_36kHz) {
		timer_config.frequency = TIMER_FREQUENCY_38kHz;

	} else if (timer_config.frequency == TIMER_FREQUENCY_38kHz) {
		timer_config.frequency = TIMER_FREQUENCY_42kHz;

	} else {
		timer_config.frequency = TIMER_FREQUENCY_36kHz;
	}

	timer0_driver_configure(&timer_config);
	timer0_driver_start();
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
