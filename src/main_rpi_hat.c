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

#include "cfg_driver_interface.h"
#include "time_management/time_management.h"

TIME_MGMN_BUILD_STATIC_TIMER_U16(PERIODE_TIMER)

int main( void ) {	

	main_init();

	TRX_DRIVER_CONFIGURATION driver_cfg;
	memset(&driver_cfg, 0x00, sizeof(driver_cfg));

	u8 byte_array[] = {'H','e','l','l','o',' ', 'U','A','R','T','!','\n'};

	//usart1_driver_configure(&driver_cfg);
	//usart1_driver_set_N_bytes (sizeof(byte_array), byte_array);
	//usart1_driver_start_tx();

	PERIODE_TIMER_start();

	//IS_READY_init();
	//IS_READY_pull_up();
	//while(1){watchdog();}

	for (;;) {

		if (PERIODE_TIMER_is_up(5000)) {

			DEBUG_PASS("main() - Going to send UART1 message");
			
			//usart1_driver_set_N_bytes (sizeof(byte_array), byte_array);
			//usart1_driver_start_tx();
			PERIODE_TIMER_start();
		}

		mcu_task_controller_schedule();
		task_yield();
		watchdog();
	}
}
