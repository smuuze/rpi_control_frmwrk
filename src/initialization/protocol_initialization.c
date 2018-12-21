/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "local_spi_driver.h"
#include "local_i2c_driver.h"
#include "local_usart_driver.h"

#include "rpi_protocol_handler.h"
#include "local_ads1115_mcu_task.h"
#include "local_sht31_mcu_task.h"

config_I2C_POWER_DOWN_PROTOTYPE
config_SPI_POWER_DOWN_PROTOTYPE


/*!
 *
 */
TRX_DRIVER_INTERFACE spi_driver = {
	SPI,				//	TRX_DRIVER_INTERFACE_TYPE type;;
	&spi_driver_initialize,		//
	&spi_driver_configure, 		//	TRX_DRIVER_INTERFACE_CONFIGURE_CALLBACK configure;
	&spi_driver_power_off, 		//	TRX_DRIVER_INTERFACE_MODULE_OFF_CALLBACK shut_down;
	&spi_driver_bytes_available, 	//	TRX_DRIVER_INTERFACE_BYTES_AVAILABLE_CALLBACK bytes_available;
	&spi_driver_get_N_bytes, 	//	TRX_DRIVER_INTERFACE_GET_N_BYTES_CALLBACK get_N_bytes;
	&spi_driver_set_N_bytes, 	//	TRX_DRIVER_INTERFACE_SET_N_BYTES_CALLBACK set_N_bytes;
	&spi_driver_start_rx, 		//	TRX_DRIVER_INTERFACE_START_RX_CALLBACK start_rx;
	&spi_driver_stop_rx, 		//	TRX_DRIVER_INTERFACE_STOP_RX_CALLBACK stop_rx;
	&spi_driver_is_ready_for_tx,	//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
	&spi_driver_is_ready_for_rx,	//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
	&spi_driver_start_tx, 		//	TRX_DRIVER_INTERFACE_START_TX_CALLBACK start_tx;
	&spi_driver_stop_tx, 		//	TRX_DRIVER_INTERFACE_STOP_TX_CALLBACK stop_tx;
	&spi_driver_clear_buffer, 	//	TRX_DRIVER_INTERFACE_CLEAR_BUFFER_CALLBACK clear_buffer;
	&spi_driver_set_address,	//	TRX_DRIVER_INTERFACE_SET_ADDRESS_CALLBACK set_address;
	&spi_driver_mutex_request,	//
	&spi_driver_mutex_release	//
};

/*!
 *
 */
TRX_DRIVER_INTERFACE local_i2c_driver = {
	I2C,					//	TRX_DRIVER_INTERFACE_TYPE type;;
	&i2c_driver_initialize,
	&i2c_driver_configure, 		//	TRX_DRIVER_INTERFACE_CONFIGURE_CALLBACK configure;
	config_I2C_POWER_DOWN_FUNCTION_REF,	//	TRX_DRIVER_INTERFACE_MODULE_OFF_CALLBACK shut_down;
	&i2c_driver_bytes_available, 	//	TRX_DRIVER_INTERFACE_BYTES_AVAILABLE_CALLBACK bytes_available;
	&i2c_driver_get_N_bytes, 		//	TRX_DRIVER_INTERFACE_GET_N_BYTES_CALLBACK get_N_bytes;
	&i2c_driver_set_N_bytes, 		//	TRX_DRIVER_INTERFACE_SET_N_BYTES_CALLBACK set_N_bytes;
	&i2c_driver_start_rx, 		//	TRX_DRIVER_INTERFACE_START_RX_CALLBACK start_rx;
	&i2c_driver_stop_rx, 		//	TRX_DRIVER_INTERFACE_STOP_RX_CALLBACK stop_rx;
	&i2c_driver_is_ready_for_tx,	//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
	&i2c_driver_is_ready_for_rx,	//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
	&i2c_driver_start_tx, 		//	TRX_DRIVER_INTERFACE_START_TX_CALLBACK start_tx;
	&i2c_driver_stop_tx, 		//	TRX_DRIVER_INTERFACE_STOP_TX_CALLBACK stop_tx;
	&i2c_driver_clear_buffer, 	//	TRX_DRIVER_INTERFACE_CLEAR_BUFFER_CALLBACK clear_buffer;
	&i2c_driver_set_address,		//	TRX_DRIVER_INTERFACE_SET_ADDRESS_CALLBACK set_address;
	&i2c_driver_mutex_request,	//
	&i2c_driver_mutex_release		//
};

#if HOST_DRIVER == 'U'
/*!
 *
 */
TRX_DRIVER_INTERFACE local_usart_driver = {
	I2C,					//	TRX_DRIVER_INTERFACE_TYPE type;;
	&local_usart_driver_configure, 		//	TRX_DRIVER_INTERFACE_CONFIGURE_CALLBACK configure;
	&local_usart_driver_power_off, 		//	TRX_DRIVER_INTERFACE_MODULE_OFF_CALLBACK shut_down;
	&local_usart_driver_bytes_available, 	//	TRX_DRIVER_INTERFACE_BYTES_AVAILABLE_CALLBACK bytes_available;
	&local_usart_driver_get_N_bytes, 		//	TRX_DRIVER_INTERFACE_GET_N_BYTES_CALLBACK get_N_bytes;
	&local_usart_driver_set_N_bytes, 		//	TRX_DRIVER_INTERFACE_SET_N_BYTES_CALLBACK set_N_bytes;
	&local_usart_driver_start_rx, 		//	TRX_DRIVER_INTERFACE_START_RX_CALLBACK start_rx;
	&local_usart_driver_stop_rx, 		//	TRX_DRIVER_INTERFACE_STOP_RX_CALLBACK stop_rx;
	&local_usart_driver_is_ready_for_tx,	//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
	&local_usart_driver_start_tx, 		//	TRX_DRIVER_INTERFACE_START_TX_CALLBACK start_tx;
	&local_usart_driver_stop_tx, 		//	TRX_DRIVER_INTERFACE_STOP_TX_CALLBACK stop_tx;
	&local_usart_driver_clear_buffer, 	//	TRX_DRIVER_INTERFACE_CLEAR_BUFFER_CALLBACK clear_buffer;
	&local_usart_driver_set_address,		//	TRX_DRIVER_INTERFACE_SET_ADDRESS_CALLBACK set_address;
};
#endif


void protocol_initialization(void) {

#if HOST_DRIVER == 'U'
	rpi_protocol_init(&local_usart_driver);
#else
	spi_driver.initialize();
	rpi_protocol_init(&spi_driver);
#endif

	local_ads1115_module_init(&local_i2c_driver);
	local_sht31_module_init(&local_i2c_driver);
}

