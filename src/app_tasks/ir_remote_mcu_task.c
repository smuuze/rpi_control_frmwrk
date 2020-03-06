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

// --------------------------------------------------------------------------------

#define IR_REMOTE_TASK_CHANGE_FREQ_INTERVAL_MS		5000

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(CHANGE_FREQ_TIMER)

static TIMER_CONFIGURATION_TYPE timer_config;

// --------------------------------------------------------------------------------

void ir_remote_app_task_irq_callback(void) {

	IR_MOD_OUT_toggle_level();
	//timer1_driver_start(10000); // 0.01 sec = 10 ms = 10000 us
	//timer1_driver_start(560); // 560 us
	//timer1_driver_start(4000); // 4000 us
	timer1_driver_start(80); // 80 us
}

// --------------------------------------------------------------------------------

void ir_remote_task_init(void) {

	DEBUG_PASS("ir_remote_task_init()");

	IR_MOD_OUT_drive_low();

	LED_GREEN_drive_low();
	LED_BLUE_drive_high();
	
	timer_config.frequency = TIMER_FREQUENCY_NONE;
	timer_config.irq_callback = &ir_remote_app_task_irq_callback;
	timer_config.mode = TIMER_MODE_TIMER;
	timer_config.time_interval = TIMER_TIME_INTERVAL_80us;

	timer1_driver_init();
	timer1_driver_configure(&timer_config);
	timer1_driver_start(10000); // 0.01 sec = 10 ms = 10000 us
	
	timer_config.frequency = TIMER_FREQUENCY_36kHz;
	timer_config.irq_callback = 0;
	timer_config.mode = TIMER_MODE_FREQUENCY;

	timer0_driver_init();
	timer0_driver_configure(&timer_config);
	timer0_driver_start(TIME_CONFIGURATION_RUN_FOREVER);

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

	switch (timer_config.frequency) {

		default:
		case TIMER_FREQUENCY_36kHz :
			timer_config.frequency = TIMER_FREQUENCY_38kHz;
			break;

		case TIMER_FREQUENCY_38kHz :
			timer_config.frequency = TIMER_FREQUENCY_42kHz;
			break;

		case TIMER_FREQUENCY_42kHz :
			timer_config.frequency = TIMER_FREQUENCY_36kHz;
			break;
	}
	
	timer_config.irq_callback = 0;
	timer_config.mode = TIMER_MODE_FREQUENCY;
	timer0_driver_configure(&timer_config);
	timer0_driver_start(TIME_CONFIGURATION_RUN_FOREVER);
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
