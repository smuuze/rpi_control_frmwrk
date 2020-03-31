
 /*
  * \@file	src/module_test/rpi_protocol_handler/testbench_rpi_protocol_handler.c
  * \author	sebastian lesse
  */

#define TRACER_ON

// -------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden! v

// -------------------------------------------------------------------

#include "tracer.h"

// -------------------------------------------------------------------

#include "driver/trx_driver_interface.h"
#include "driver/communication/test/communication_driver_test.h"
#include "driver/gpio/gpio_interface.h"

// -------------------------------------------------------------------

#include "protocol_management/rpi_protocol_handler.h"

// -------------------------------------------------------------------

u8 testcase_command_answer_success( TRX_DRIVER_INTERFACE* p_com_driver );

// -------------------------------------------------------------------

static TRX_DRIVER_INTERFACE test_driver = {
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
	&test_driver_clear_rx_buffer, 		//	TRX_DRIVER_INTERFACE_CLEAR_BUFFER_CALLBACK clear_buffer;
	&test_driver_clear_tx_buffer, 		//	TRX_DRIVER_INTERFACE_CLEAR_BUFFER_CALLBACK clear_buffer;
	&test_driver_set_address,		//	TRX_DRIVER_INTERFACE_SET_ADDRESS_CALLBACK set_address;
	&test_driver_mutex_request,		//
	&test_driver_mutex_release		//
};

// -------------------------------------------------------------------

int main( void ) {

	DEBUG_PASS("Hello Module Test - RPi-Protocol-Handler");
			
	gpio_driver_init();

	test_driver.initialize();

	rpi_protocol_init(&test_driver);

	testcase_command_answer_success(&test_driver);

	return 0;
}
