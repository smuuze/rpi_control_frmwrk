/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *
 * \author               sebastian lesse
 *
 *****************************************************************************/

#define TRACER_ON

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
