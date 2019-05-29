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


#include "driver/cfg_driver_interface.h"
#include "driver/trx_driver_interface.h"
#include "common/local_context.h"

#include "driver/communication/test/communication_driver_test.h"

//---------- Implementation of Traces -----------------------------------------

//PCA9670_BUILD_INSTANCE(PCA9670_test_instance, 0x20)

SYSTEM_T system_context;

int main( void ) {

	
	TRX_DRIVER_INTERFACE test_driver = {
		SPI,					//	TRX_DRIVER_INTERFACE_TYPE type;
		&test_driver_initialize,
		&test_driver_configure, 			//	TRX_DRIVER_INTERFACE_CONFIGURE_CALLBACK configure;
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

	//pca9670_init(&test_driver);
	
	//PCA9670_test_instance_init();

	for (;;) {  // Endlosschleife
	
	}
	
	return 0;
}
