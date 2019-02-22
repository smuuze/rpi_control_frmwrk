#ifndef _SYSTEM_INTERFACE_H_
#define _SYSTEM_INTERFACE_H_

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "utils/stdmacros.h"

#include "system_interface.h"


#if defined HAS_DRIVER_SPI0 && HAS_DRIVER_SPI0 == 1

#include "local_spi_driver.h"

config_SPI_POWER_DOWN_PROTOTYPE

/*!
 *
 */
static TRX_DRIVER_INTERFACE spi0_driver = {
	SPI,					//	TRX_DRIVER_INTERFACE_TYPE type;
	&spi_driver_initialize,			//
	&spi_driver_configure, 			//	TRX_DRIVER_INTERFACE_CONFIGURE_CALLBACK configure;
	&spi_driver_power_off, 			//	TRX_DRIVER_INTERFACE_MODULE_OFF_CALLBACK shut_down;
	&spi_driver_bytes_available, 		//	TRX_DRIVER_INTERFACE_BYTES_AVAILABLE_CALLBACK bytes_available;
	&spi_driver_get_N_bytes, 		//	TRX_DRIVER_INTERFACE_GET_N_BYTES_CALLBACK get_N_bytes;
	&spi_driver_set_N_bytes, 		//	TRX_DRIVER_INTERFACE_SET_N_BYTES_CALLBACK set_N_bytes;
	&spi_driver_start_rx, 			//	TRX_DRIVER_INTERFACE_START_RX_CALLBACK start_rx;
	&spi_driver_wait_for_rx,		//
	&spi_driver_stop_rx, 			//	TRX_DRIVER_INTERFACE_STOP_RX_CALLBACK stop_rx;
	&spi_driver_is_ready_for_tx,		//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
	&spi_driver_is_ready_for_rx,		//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
	&spi_driver_start_tx, 			//	TRX_DRIVER_INTERFACE_START_TX_CALLBACK start_tx;
	&spi_driver_wait_for_tx,		//
	&spi_driver_stop_tx, 			//	TRX_DRIVER_INTERFACE_STOP_TX_CALLBACK stop_tx;
	&spi_driver_clear_buffer, 		//	TRX_DRIVER_INTERFACE_CLEAR_BUFFER_CALLBACK clear_buffer;
	&spi_driver_set_address,		//	TRX_DRIVER_INTERFACE_SET_ADDRESS_CALLBACK set_address;
	&spi_driver_mutex_request,		//
	&spi_driver_mutex_release		//
};
#endif // #if defined HAS_DRIVER_SPI0 && HAS_DRIVER_SPI0 == 1

#if defined HAS_DRIVER_USART0 && HAS_DRIVER_USART0 == 1

#include "local_usart_driver.h"

/*!
 *
 */
static TRX_DRIVER_INTERFACE usart0_driver = {
	USART,					//	TRX_DRIVER_INTERFACE_TYPE type;
		&usart_driver_initialize,			//
	&usart_driver_configure, 		//	TRX_DRIVER_INTERFACE_CONFIGURE_CALLBACK configure;
	&usart_driver_power_off, 		//	TRX_DRIVER_INTERFACE_MODULE_OFF_CALLBACK shut_down;
	&usart_driver_bytes_available, 	//	TRX_DRIVER_INTERFACE_BYTES_AVAILABLE_CALLBACK bytes_available;
	&usart_driver_get_N_bytes, 	//	TRX_DRIVER_INTERFACE_GET_N_BYTES_CALLBACK get_N_bytes;
	&usart_driver_set_N_bytes, 	//	TRX_DRIVER_INTERFACE_SET_N_BYTES_CALLBACK set_N_bytes;
	&usart_driver_start_rx, 		//	TRX_DRIVER_INTERFACE_START_RX_CALLBACK start_rx;
		&usart_driver_wait_for_rx,		//
	&usart_driver_stop_rx, 		//	TRX_DRIVER_INTERFACE_STOP_RX_CALLBACK stop_rx;
	&usart_driver_is_ready_for_tx,	//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
		&usart_driver_is_ready_for_rx,		//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
	&usart_driver_start_tx, 		//	TRX_DRIVER_INTERFACE_START_TX_CALLBACK start_tx;
		&usart_driver_wait_for_tx,		//
	&usart_driver_stop_tx, 		//	TRX_DRIVER_INTERFACE_STOP_TX_CALLBACK stop_tx;
	&usart_driver_clear_buffer, 	//	TRX_DRIVER_INTERFACE_CLEAR_BUFFER_CALLBACK clear_buffer;
	&usart_driver_set_address,		//	TRX_DRIVER_INTERFACE_SET_ADDRESS_CALLBACK set_address;
		&usart_driver_mutex_request,		//
		&usart_driver_mutex_release		//
};
#endif // #if defined HAS_DRIVER_I2C0 && HAS_DRIVER_I2C0 == 1

#if defined HAS_DRIVER_I2C0 && HAS_DRIVER_I2C0 == 1

config_I2C_POWER_DOWN_PROTOTYPE

#include "local_i2c_driver.h"

/*!
 *
 */
static TRX_DRIVER_INTERFACE i2c0_driver = {
	I2C,					//	TRX_DRIVER_INTERFACE_TYPE type;
	&i2c_driver_initialize,
	&i2c_driver_configure, 			//	TRX_DRIVER_INTERFACE_CONFIGURE_CALLBACK configure;
	config_I2C_POWER_DOWN_FUNCTION_REF,	//	TRX_DRIVER_INTERFACE_MODULE_OFF_CALLBACK shut_down;
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
#endif // #if defined HAS_DRIVER_I2C0 && HAS_DRIVER_I2C0 == 1


#ifndef config_SYSTEM_INTERFACE_GET_TIME_U8_PROTOTYPE
#define config_SYSTEM_INTERFACE_GET_TIME_U8_PROTOTYPE
#endif

#ifndef config_SYSTEM_INTERFACE_GET_TIME_U16_PROTOTYPE
#define config_SYSTEM_INTERFACE_GET_TIME_U16_PROTOTYPE
#endif

#ifndef config_SYSTEM_INTERFACE_GET_TIME_U32_PROTOTYPE
#define config_SYSTEM_INTERFACE_GET_TIME_U32_PROTOTYPE
#endif

#ifndef config_SYSTEM_INTERFACE_IS_TIME_UP_U8_PROTOTYPE
#define config_SYSTEM_INTERFACE_IS_TIME_UP_U8_PROTOTYPE
#endif

#ifndef config_SYSTEM_INTERFACE_IS_TIME_UP_U16_PROTOTYPE
#define config_SYSTEM_INTERFACE_IS_TIME_UP_U16_PROTOTYPE
#endif

#ifndef config_SYSTEM_INTERFACE_IS_TIME_UP_U32_PROTOTYPE
#define config_SYSTEM_INTERFACE_IS_TIME_UP_U32_PROTOTYPE
#endif

#ifndef config_SYSTEM_INTERFACE_ADD_EVENT_PROTOTYPE
#define config_SYSTEM_INTERFACE_ADD_EVENT_PROTOTYPE
#endif

#ifndef config_SYSTEM_INTERFACE_GET_EVENT_PROTOTYPE
#define config_SYSTEM_INTERFACE_GET_EVENT_PROTOTYPE
#endif

#ifndef config_SYSTEM_INTERFACE_PROGMEM_GET_BYTE_PROTOTYPE
#define config_SYSTEM_INTERFACE_PROGMEM_GET_BYTE_PROTOTYPE
#endif

#ifndef config_SYSTEM_INTERFACE_PROGMEM_GET_WORD_PROTOTYPE
#define config_SYSTEM_INTERFACE_PROGMEM_GET_WORD_PROTOTYPE
#endif

#ifndef config_SYSTEM_INTERFACE_PROGMEM_GET_LONG_PROTOTYPE
#define config_SYSTEM_INTERFACE_PROGMEM_GET_LONG_PROTOTYPE
#endif

#ifndef config_SYSTEM_INTERFACE_PROGMEM_GET_N_BYTES_PROTOTYPE
#define config_SYSTEM_INTERFACE_PROGMEM_GET_N_BYTES_PROTOTYPE
#endif


#ifndef config_INTERFACE_IO_INIT_PIN_PROTOTYPE
#define config_INTERFACE_IO_INIT_PIN_PROTOTYPE
#endif

#ifndef config_INTERFACE_IO_SET_PIN_DIR_PROTOTYPE
#define config_INTERFACE_IO_SET_PIN_DIR_PROTOTYPE
#endif

#ifndef config_SYSTEM_INTERFACE_IO_SET_PIN_LEVEL_PROTOTYPE
#define config_SYSTEM_INTERFACE_IO_SET_PIN_LEVEL_PROTOTYPE
#endif

#ifndef config_SYSTEM_INTERFACE_IO_GET_PIN_LEVEL_PROTOTYPE
#define config_SYSTEM_INTERFACE_IO_GET_PIN_LEVEL_PROTOTYPE
#endif



#ifndef config_SYSTEM_INTERFACE_GET_TIME_U8_CALLBACK
static u8 __system_interface_get_time_u8_dummy(void) { return 0;}
#define config_SYSTEM_INTERFACE_GET_TIME_U8_CALLBACK	__system_interface_get_time_u8_dummy
#endif

#ifndef config_SYSTEM_INTERFACE_GET_TIME_U16_CALLBACK
static u16 __system_interface_get_time_u16_dummy(void) { return 0;}
#define config_SYSTEM_INTERFACE_GET_TIME_U16_CALLBACK	__system_interface_get_time_u16_dummy
#endif

#ifndef config_SYSTEM_INTERFACE_GET_TIME_U32_CALLBACK
static u32 __system_interface_get_time_u32_dummy(void) { return 0;}
#define config_SYSTEM_INTERFACE_GET_TIME_U32_CALLBACK	__system_interface_get_time_u32_dummy
#endif

#ifndef config_SYSTEM_INTERFACE_IS_TIME_UP_U8_CALLBACK
static u8 __system_interface_is_time_up_u8_dummy(u8 ref_time, u8 time_interval) { (void) ref_time; (void) time_interval; return 0; }
#define config_SYSTEM_INTERFACE_IS_TIME_UP_U8_CALLBACK	__system_interface_is_time_up_u8_dummy
#endif

#ifndef config_SYSTEM_INTERFACE_IS_TIME_UP_U16_CALLBACK
static u8 __system_interface_is_time_up_u16_dummy(u16 ref_time, u16 time_interval) { (void) ref_time; (void) time_interval; return 0; }
#define config_SYSTEM_INTERFACE_IS_TIME_UP_U16_CALLBACK	__system_interface_is_time_up_u16_dummy
#endif

#ifndef config_SYSTEM_INTERFACE_IS_TIME_UP_U32_CALLBACK
static u8 __system_interface_is_time_up_u32_dummy(u32 ref_time, u32 time_interval) { (void) ref_time; (void) time_interval; return 0; }
#define config_SYSTEM_INTERFACE_IS_TIME_UP_U32_CALLBACK	__system_interface_is_time_up_u32_dummy
#endif

#ifndef config_SYSTEM_INTERFACE_ADD_EVENT_CALLBACK
static void __system_interface_add_event_dummy(SYSTEM_EVENT event) { (void) event; }
#define config_SYSTEM_INTERFACE_ADD_EVENT_CALLBACK	__system_interface_add_event_dummy
#endif

#ifndef config_SYSTEM_INTERFACE_GET_EVENT_CALLBACK
static SYSTEM_EVENT __system_interface_get_event_dummy(void) { return SYS_EVT_NO_EVENT; }
#define config_SYSTEM_INTERFACE_GET_EVENT_CALLBACK	__system_interface_get_event_dummy
#endif


#ifndef config_SYSTEM_INTERFACE_PROGMEM_GET_BYTE_CALLBACK
static u8 __system_interface_progmem_get_byte(u8* addr) { (void) addr; return 0;}
#define config_SYSTEM_INTERFACE_PROGMEM_GET_BYTE_CALLBACK	__system_interface_progmem_get_byte
#endif

#ifndef config_SYSTEM_INTERFACE_PROGMEM_GET_WORD_CALLBACK
static u16 __system_interface_progmem_get_word(u8* addr) { (void) addr; return 0; }
#define config_SYSTEM_INTERFACE_PROGMEM_GET_WORD_CALLBACK	__system_interface_progmem_get_word
#endif

#ifndef config_SYSTEM_INTERFACE_PROGMEM_GET_LONG_CALLBACK
static u32 __system_interface_progmem_get_long(u8* addr) { (void) addr; return 0; }
#define config_SYSTEM_INTERFACE_PROGMEM_GET_LONG_CALLBACK	__system_interface_progmem_get_long
#endif

#ifndef config_SYSTEM_INTERFACE_PROGMEM_GET_N_BYTES_CALLBACK
static void __system_interface_progmem_get_N_bytes(u8* addr, u8* p_buffer_to, u8 num_bytes) { (void) addr; (void) p_buffer_to; (void) num_bytes; }
#define config_SYSTEM_INTERFACE_PROGMEM_GET_N_BYTES_CALLBACK	__system_interface_progmem_get_N_bytes
#endif


#ifndef config_SYSTEM_INTERFACE_IO_INIT_PIN_CALLBACK
void __system_interface_gpio_init_pin(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) { (void) p_pin_descr}
#define config_SYSTEM_INTERFACE_IO_INIT_PIN_CALLBACK __system_interface_gpio_init_pin
#endif

#ifndef config_SYSTEM_INTERFACE_IO_SET_PIN_DIR_CALLBACK
void __system_interface_gpio_set_direction(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, SYSTEM_INTERFACE_GPIO_DIRECTION direction) { (void) p_pin_descr; (void) direction; }
#define config_SYSTEM_INTERFACE_IO_SET_PIN_DIR_CALLBACK __system_interface_gpio_set_direction
#endif

#ifndef config_SYSTEM_INTERFACE_IO_SET_PIN_LEVEL_CALLBACK
void __system_interface_gpio_set_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, SYSTEM_INTERFACE_IO_PIN_LEVEL level) { (void) p_pin_descr; (void) level; }
#define config_SYSTEM_INTERFACE_IO_SET_PIN_LEVEL_CALLBACK __system_interface_gpio_set_level
#endif

#ifndef config_SYSTEM_INTERFACE_IO_GET_PIN_LEVEL_CALLBACK
SYSTEM_INTERFACE_IO_PIN_LEVEL __system_interface_gpio_get_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr) { (void) p_pin_descr; return GPIO_LEVEL_HIGH_Z; }
#define config_SYSTEM_INTERFACE_IO_GET_PIN_LEVEL_CALLBACK __system_interface_gpio_get_level
#endif



config_SYSTEM_INTERFACE_GET_TIME_U8_PROTOTYPE
config_SYSTEM_INTERFACE_GET_TIME_U16_PROTOTYPE
config_SYSTEM_INTERFACE_GET_TIME_U32_PROTOTYPE
config_SYSTEM_INTERFACE_IS_TIME_UP_U8_PROTOTYPE
config_SYSTEM_INTERFACE_IS_TIME_UP_U16_PROTOTYPE
config_SYSTEM_INTERFACE_IS_TIME_UP_U32_PROTOTYPE

config_SYSTEM_INTERFACE_ADD_EVENT_PROTOTYPE
config_SYSTEM_INTERFACE_GET_EVENT_PROTOTYPE

config_SYSTEM_INTERFACE_PROGMEM_GET_BYTE_PROTOTYPE
config_SYSTEM_INTERFACE_PROGMEM_GET_WORD_PROTOTYPE
config_SYSTEM_INTERFACE_PROGMEM_GET_LONG_PROTOTYPE
config_SYSTEM_INTERFACE_PROGMEM_GET_N_BYTES_PROTOTYPE

config_SYSTEM_INTERFACE_IO_INIT_PIN_PROTOTYPE
config_SYSTEM_INTERFACE_IO_SET_PIN_DIR_PROTOTYPE
config_SYSTEM_INTERFACE_IO_SET_PIN_LEVEL_PROTOTYPE
config_SYSTEM_INTERFACE_IO_TOGGLE_PIN_LEVEL_PROTOTYPE
config_SYSTEM_INTERFACE_IO_GET_PIN_LEVEL_PROTOTYPE


const SYSTEM_INTERFACE i_system = {
	{
		&config_SYSTEM_INTERFACE_GET_TIME_U8_CALLBACK,
		&config_SYSTEM_INTERFACE_GET_TIME_U16_CALLBACK,
		&config_SYSTEM_INTERFACE_GET_TIME_U32_CALLBACK,

		&config_SYSTEM_INTERFACE_IS_TIME_UP_U8_CALLBACK,
		&config_SYSTEM_INTERFACE_IS_TIME_UP_U16_CALLBACK,
		&config_SYSTEM_INTERFACE_IS_TIME_UP_U32_CALLBACK
	},
	{
		&config_SYSTEM_INTERFACE_ADD_EVENT_CALLBACK,
		&config_SYSTEM_INTERFACE_GET_EVENT_CALLBACK
	},
	{
		&config_SYSTEM_INTERFACE_PROGMEM_GET_BYTE_CALLBACK,
		&config_SYSTEM_INTERFACE_PROGMEM_GET_WORD_CALLBACK,
		&config_SYSTEM_INTERFACE_PROGMEM_GET_LONG_CALLBACK,
		&config_SYSTEM_INTERFACE_PROGMEM_GET_N_BYTES_CALLBACK
	},
	{
		&config_SYSTEM_INTERFACE_IO_INIT_PIN_CALLBACK,
		&config_SYSTEM_INTERFACE_IO_SET_PIN_DIR_CALLBACK,
		&config_SYSTEM_INTERFACE_IO_SET_PIN_LEVEL_CALLBACK,
		&config_SYSTEM_INTERFACE_IO_TOGGLE_PIN_LEVEL_CALLBACK,
		&config_SYSTEM_INTERFACE_IO_GET_PIN_LEVEL_CALLBACK
	},
	{
		#if defined HAS_DRIVER_SPI0 && HAS_DRIVER_SPI0 == 1
		&spi0_driver,
		#endif

		#if defined HAS_DRIVER_USART0 && HAS_DRIVER_USART0 == 1
		&usart0_driver,
		#endif

		#if defined HAS_DRIVER_I2C0 && HAS_DRIVER_I2C0 == 1
		&i2c0_driver
		#endif
	}
};

#endif // _PROTOCOL_DRIVER_INTERFACE_H_
