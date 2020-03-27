/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	main_rpi_hat.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "initialization/initialization.h"
#include "mcu_task_management/mcu_task_controller.h"

// --------------------------------------------------------------------------------

void task_yield(void) {
	mcu_task_controller_background_run();
}

// --------------------------------------------------------------------------------

void main_init(void) {

	cli();
	initialization();
	sei();

	#ifdef HAS_GPIO_LED_RED
	{
		LED_RED_drive_low();
	}
	#endif

	DEBUG_PASS("main_init() - Initialization done");
}

int main( void ) {

	main_init();

	DEBUG_PASS(config_DEBUG_WELCOME_MESSAGE);

	for (;;) {
		
		mcu_task_controller_schedule();
		task_yield();
		watchdog();
	}
}
