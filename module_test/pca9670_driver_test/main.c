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
#include "expansion/driver_PCA9670.h"

//---------- Implementation of Traces -----------------------------------------

void i2c_driver_initialize(void) {

}

void i2c_driver_configure(TRX_DRIVER_CONFIGURATION* p_cfg) {

	(void) p_cfg;
}

void i2c_driver_power_off(void) {

}

u8 i2c_driver_bytes_available (void) {
	return 0;
}

u8 i2c_driver_get_N_bytes (u8 num_bytes, u8* p_buffer_to) {
	return 0;
}

u8 i2c_driver_set_N_bytes (u8 num_bytes, const u8* p_buffer_from) {
	return 0;
}

u8 i2c_driver_is_ready_for_tx (void) {
	return 0;
}

u8 i2c_driver_is_ready_for_rx(void) {
	return 0;
}

void i2c_driver_start_rx (u16 num_of_rx_bytes) {

}

void i2c_driver_wait_for_rx(u8 num_bytes, u16 timeout_ms) {
	(void) num_bytes;
	(void) timeout_ms;
}

void i2c_driver_stop_rx (void) {

}

void i2c_driver_start_tx (void) {

}

void i2c_driver_wait_for_tx(u8 num_bytes, u16 timeout_ms) {
	(void) num_bytes;
	(void) timeout_ms;
}

void i2c_driver_stop_tx (void) {

}

void i2c_driver_clear_buffer (void) {

}

void i2c_driver_set_address (u8 addr) {
	(void) addr;
}

u8 i2c_driver_mutex_request(void) {
	return 1;
}

void i2c_driver_mutex_release(u8 m_id) {

}

PCA9670_BUILD_INSTANCE(PCA9670_test_instance, 0x20)

int main( void ) {

	
	TRX_DRIVER_INTERFACE i2c0_driver = {
		I2C,					//	TRX_DRIVER_INTERFACE_TYPE type;
		&i2c_driver_initialize,
		&i2c_driver_configure, 			//	TRX_DRIVER_INTERFACE_CONFIGURE_CALLBACK configure;
		&i2c_driver_power_off,			//	TRX_DRIVER_INTERFACE_MODULE_OFF_CALLBACK shut_down;
		&i2c_driver_bytes_available, 		//	TRX_DRIVER_INTERFACE_BYTES_AVAILABLE_CALLBACK bytes_available;
		&i2c_driver_get_N_bytes, 		//	TRX_DRIVER_INTERFACE_GET_N_BYTES_CALLBACK get_N_bytes;
		&i2c_driver_set_N_bytes, 		//	TRX_DRIVER_INTERFACE_SET_N_BYTES_CALLBACK set_N_bytes;
		&i2c_driver_start_rx, 			//	TRX_DRIVER_INTERFACE_START_RX_CALLBACK start_rx;
		&i2c_driver_wait_for_rx,		//
		&i2c_driver_stop_rx, 			//	TRX_DRIVER_INTERFACE_STOP_RX_CALLBACK stop_rx;
		&i2c_driver_is_ready_for_tx,		//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
		&i2c_driver_is_ready_for_rx,		//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
		&i2c_driver_start_tx, 			//	TRX_DRIVER_INTERFACE_START_TX_CALLBACK start_tx;
		&i2c_driver_wait_for_tx,		//
		&i2c_driver_stop_tx, 			//	TRX_DRIVER_INTERFACE_STOP_TX_CALLBACK stop_tx;
		&i2c_driver_clear_buffer, 		//	TRX_DRIVER_INTERFACE_CLEAR_BUFFER_CALLBACK clear_buffer;
		&i2c_driver_set_address,		//	TRX_DRIVER_INTERFACE_SET_ADDRESS_CALLBACK set_address;
		&i2c_driver_mutex_request,		//
		&i2c_driver_mutex_release		//
	};

	pca9670_init(&i2c0_driver);
	
	PCA9670_test_instance_init();

	for (;;) {  // Endlosschleife
	
	}
	
	return 0;
}
