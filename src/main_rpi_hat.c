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

	PASS(); // main_init() - Initialization done
}

//#include "io_management/io_controller.h"
//IO_CONTROLLER_INCLUDE_INOUT(IS_READY)

#include "cfg_driver_interface.h"
#include "time_management/time_management.h"

#include "driver/communication/usart/usart0_driver.h"

TIME_MGMN_BUILD_STATIC_TIMER_U16(PERIODE_TIMER)

#define UART_WELCOME_TEXT			"Hello UART !\n\0"

int main( void ) {	

	main_init();

	TRX_DRIVER_CONFIGURATION driver_cfg;
	memset(&driver_cfg, 0x00, sizeof(driver_cfg));
	driver_cfg.module.usart.baudrate = BAUDRATE_230400;


	u8 usart_mutex_id = usart0_driver_mutex_request();
	if (usart_mutex_id) {
		usart0_driver_clear_buffer();
		usart0_driver_configure(&driver_cfg);
		usart0_driver_set_N_bytes(sizeof(UART_WELCOME_TEXT), UART_WELCOME_TEXT);
		usart0_driver_start_tx();
		usart0_driver_mutex_release(usart_mutex_id);
	}


	PERIODE_TIMER_start();

	//IS_READY_init();
	//IS_READY_pull_up();
	//while(1){watchdog();}

	for (;;) {

		if (PERIODE_TIMER_is_up(5000)) {

			u8 usart_mutex_id = usart0_driver_mutex_request();
			if (usart_mutex_id) {
				DEBUG_PASS("main() - Going to send UART1 message");
				
				usart0_driver_clear_buffer();
				usart0_driver_set_N_bytes(sizeof(UART_WELCOME_TEXT), UART_WELCOME_TEXT);
				usart0_driver_start_tx();
				usart0_driver_mutex_release(usart_mutex_id);

				PERIODE_TIMER_start();
			}

		}

		

		mcu_task_controller_schedule();
		task_yield();
		watchdog();
	}
}
