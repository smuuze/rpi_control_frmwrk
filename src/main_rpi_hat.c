/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *
 * \author               sebastian lesse
 *
 *****************************************************************************/

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "tracer.h"

// --------------------------------------------------------------------------------

#include "initialization.h"
#include "mcu_task_controller.h"

// --------------------------------------------------------------------------------

#ifdef  config_FAKERTOS_TASKYIELD_INIT_PROTO
config_FAKERTOS_TASKYIELD_INIT_PROTO
#endif

// --------------------------------------------------------------------------------
void task_yield(void) {
	config_FAKERTOS_TASKYIELD_FUNCTION
}

// --------------------------------------------------------------------------------

void main_init(void) {

	cli();
	initialization();
	sei();

	PASS(); // main_init() - Initialization done
}

//#include "io_management/io_controller.h"
//IO_CONTROLLER_INCLUDE_INOUT(IS_READY)

int main( void ) {	

	main_init();

	//IS_READY_init();
	//IS_READY_pull_up();
	//while(1){watchdog();}

	for (;;) {

		mcu_task_controller_schedule();
		task_yield();
		watchdog();
	}
}
