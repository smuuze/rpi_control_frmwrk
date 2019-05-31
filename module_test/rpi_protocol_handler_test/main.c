/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *
 * \author               sebastian lesse
 *
 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include "driver/cfg_driver_interface.h"
#include "driver/trx_driver_interface.h"

#include "driver/gpio/gpio_interface.h"

#include "common/local_context.h"

#include "driver/communication/test/communication_driver_test.h"

#include "protocol_management/rpi_protocol_handler.h"
#include "command_management/command_handler_interface.h"

#include "io_management/io_controller.h"
#include "mcu_task_management/mcu_task_interface.h"

#include "time_management/time_management.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_ON
#include "tracer.h"

//---------- Type Definitions -------------------------------------------------

typedef enum {
	TEST_CASE_SEND_VERSION_COMMAND_AND_GET_ANSWER,
	TEST_CASE_ACTIVATE_READY_PIN
} TEST_CASE_TYPE;

//---------- Test-Case Prototype ----------------------------------------------

u8 test_send_version_command_and_receive_answer(void);
u8 test_activate_ready_pin(void);

//---------- Static Data ------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(run_interval_timer)

SYSTEM_T system_context;

//---------- Function ---------------------------------------------------------

int main( void ) {

	DEBUG_PASS("main() - Hello Module Test - RPi-Protocol-Handler");
	
	TRX_DRIVER_INTERFACE test_driver = {
		SPI,					//	TRX_DRIVER_INTERFACE_TYPE type;
		&test_driver_initialize,
		&test_driver_configure, 		//	TRX_DRIVER_INTERFACE_CONFIGURE_CALLBACK configure;
		&test_driver_power_off,			//	TRX_DRIVER_INTERFACE_MODULE_OFF_CALLBACK shut_down;
		&test_driver_bytes_available, 		//	TRX_DRIVER_INTERFACE_BYTES_AVAILABLE_CALLBACK bytes_available;
		&test_driver_get_N_bytes, 		//	TRX_DRIVER_INTERFACE_GET_N_BYTES_CALLBACK get_N_bytes;
		&test_driver_set_N_bytes, 		//	TRX_DRIVER_INTERFACE_SET_N_BYTES_CALLBACK set_N_bytes;
		&test_driver_start_rx, 			//	TRX_DRIVER_INTERFACE_START_RX_CALLBACK start_rx;
		&test_driver_wait_for_rx,		//
		&test_driver_stop_rx, 			//	TRX_DRIVER_INTERFACE_STOP_RX_CALLBACK stop_rx;
		&test_driver_is_ready_for_tx,		//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
		&test_driver_is_ready_for_rx,		//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
		&test_driver_start_tx, 			//	TRX_DRIVER_INTERFACE_START_TX_CALLBACK start_tx;
		&test_driver_wait_for_tx,		//
		&test_driver_stop_tx, 			//	TRX_DRIVER_INTERFACE_STOP_TX_CALLBACK stop_tx;
		&test_driver_clear_buffer, 		//	TRX_DRIVER_INTERFACE_CLEAR_BUFFER_CALLBACK clear_buffer;
		&test_driver_set_address,		//	TRX_DRIVER_INTERFACE_SET_ADDRESS_CALLBACK set_address;
		&test_driver_mutex_request,		//
		&test_driver_mutex_release		//
	};
			
	gpio_driver_init();
	rpi_protocol_init(&test_driver);

	run_interval_timer_start();

	TEST_CASE_TYPE test_case = TEST_CASE_SEND_VERSION_COMMAND_AND_GET_ANSWER;
	u8 error_code = 0;

	for (;;) {  // Endlosschleife
					
		usleep(5000);

		switch (test_case) {

			default:
			case TEST_CASE_SEND_VERSION_COMMAND_AND_GET_ANSWER:

				error_code = test_send_version_command_and_receive_answer();
				if (error_code == 0xFF) {
					test_case = TEST_CASE_ACTIVATE_READY_PIN;
					error_code = 0;
				}  

				break;

			case TEST_CASE_ACTIVATE_READY_PIN:

				error_code = test_activate_ready_pin();
				if (error_code == 0xFF) {
					test_case = TEST_CASE_SEND_VERSION_COMMAND_AND_GET_ANSWER;
					error_code = 0;
				}  

				break;
		}

		if (error_code != 0) {
			DEBUG_TRACE_byte(error_code, "--- main() - Test-Case has FAILED !!! --- Error:");
			return error_code;
		}

		if (run_interval_timer_is_up(25)) {

			run_interval_timer_start();

			if (rpi_protocol_task_get_state() == MCU_TASK_RUNNING) {
				rpi_protocol_task_run();
			}
		}
	}

	return 0;
}

//---------- Dummy Implementation ---------------------------------------------

#include "command_management/protocol_interface.h"

void rpi_cmd_handler_set_request(PROTOCOL_INTERFACE* p_protocol_handler) {
	DEBUG_TRACE_byte(p_protocol_handler->command_code, "rpi_cmd_handler_set_request()");


	if (p_protocol_handler->command_code == 0x01) {

		DEBUG_PASS("rpi_cmd_handler_set_request() - CMD: Version");

		p_protocol_handler->answ_buffer->start_write();
		p_protocol_handler->answ_buffer->add_word(0x0301);
		p_protocol_handler->answ_buffer->stop_write();

		p_protocol_handler->set_finished(CMD_NO_ERR);
	}
}

void rpi_cmd_handler_set_unrequested(void) {
	DEBUG_PASS("rpi_cmd_handler_set_unrequested()");
}