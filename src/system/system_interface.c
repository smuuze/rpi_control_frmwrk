/*! 
 * --------------------------------------------------------------------------------
 *
 * 	@file		system/system_interface.c
 * 	@author		sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!

//-----------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "system_interface.h"

//-----------------------------------------------------------------------------


#if defined HAS_DRIVER_SPI0 && HAS_DRIVER_SPI0 == 1

#include "driver/communication/spi/spi0_driver.h"

config_SPI_POWER_DOWN_PROTOTYPE

/*!
 *
 */
static TRX_DRIVER_INTERFACE spi0_driver = {
	SPI,					//	TRX_DRIVER_INTERFACE_TYPE type;
	&spi0_driver_initialize,			//
	&spi0_driver_configure, 			//	TRX_DRIVER_INTERFACE_CONFIGURE_CALLBACK configure;
	&spi0_driver_power_off, 			//	TRX_DRIVER_INTERFACE_MODULE_OFF_CALLBACK shut_down;
	&spi0_driver_bytes_available, 		//	TRX_DRIVER_INTERFACE_BYTES_AVAILABLE_CALLBACK bytes_available;
	&spi0_driver_get_N_bytes, 		//	TRX_DRIVER_INTERFACE_GET_N_BYTES_CALLBACK get_N_bytes;
	&spi0_driver_set_N_bytes, 		//	TRX_DRIVER_INTERFACE_SET_N_BYTES_CALLBACK set_N_bytes;
	&spi0_driver_start_rx, 			//	TRX_DRIVER_INTERFACE_START_RX_CALLBACK start_rx;
	&spi0_driver_wait_for_rx,		//
	&spi0_driver_stop_rx, 			//	TRX_DRIVER_INTERFACE_STOP_RX_CALLBACK stop_rx;
	&spi0_driver_is_ready_for_tx,		//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
	&spi0_driver_is_ready_for_rx,		//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
	&spi0_driver_start_tx, 			//	TRX_DRIVER_INTERFACE_START_TX_CALLBACK start_tx;
	&spi0_driver_wait_for_tx,		//
	&spi0_driver_stop_tx, 			//	TRX_DRIVER_INTERFACE_STOP_TX_CALLBACK stop_tx;
	&spi0_driver_clear_rx_buffer, 		//	TRX_DRIVER_INTERFACE_CLEAR_BUFFER_CALLBACK clear_buffer;
	&spi0_driver_clear_tx_buffer, 		//	TRX_DRIVER_INTERFACE_CLEAR_BUFFER_CALLBACK clear_buffer;
	&spi0_driver_set_address,		//	TRX_DRIVER_INTERFACE_SET_ADDRESS_CALLBACK set_address;
	&spi0_driver_mutex_request,		//
	&spi0_driver_mutex_release		//
};
#endif // #if defined HAS_DRIVER_SPI0 && HAS_DRIVER_SPI0 == 1

//-----------------------------------------------------------------------------

#if defined HAS_DRIVER_USART0 && HAS_DRIVER_USART0 == 1

#include "driver/communication/usart/usart0_driver.h"

/*!
 *
 */
static TRX_DRIVER_INTERFACE usart0_driver = {
	USART,					//	TRX_DRIVER_INTERFACE_TYPE type;
	&usart0_driver_initialize,			//
	&usart0_driver_configure, 		//	TRX_DRIVER_INTERFACE_CONFIGURE_CALLBACK configure;
	&usart0_driver_power_off, 		//	TRX_DRIVER_INTERFACE_MODULE_OFF_CALLBACK shut_down;
	&usart0_driver_bytes_available, 	//	TRX_DRIVER_INTERFACE_BYTES_AVAILABLE_CALLBACK bytes_available;
	&usart0_driver_get_N_bytes, 		//	TRX_DRIVER_INTERFACE_GET_N_BYTES_CALLBACK get_N_bytes;
	&usart0_driver_set_N_bytes, 		//	TRX_DRIVER_INTERFACE_SET_N_BYTES_CALLBACK set_N_bytes;
	&usart0_driver_start_rx, 		//	TRX_DRIVER_INTERFACE_START_RX_CALLBACK start_rx;
	&usart0_driver_wait_for_rx,		//
	&usart0_driver_stop_rx, 		//	TRX_DRIVER_INTERFACE_STOP_RX_CALLBACK stop_rx;
	&usart0_driver_is_ready_for_tx,		//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
	&usart0_driver_is_ready_for_rx,		//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
	&usart0_driver_start_tx, 		//	TRX_DRIVER_INTERFACE_START_TX_CALLBACK start_tx;
	&usart0_driver_wait_for_tx,		//
	&usart0_driver_stop_tx, 		//	TRX_DRIVER_INTERFACE_STOP_TX_CALLBACK stop_tx;
	&usart0_driver_clear_rx_buffer, 	//	TRX_DRIVER_INTERFACE_CLEAR_BUFFER_CALLBACK clear_buffer;
	&usart0_driver_clear_tx_buffer, 	//	TRX_DRIVER_INTERFACE_CLEAR_BUFFER_CALLBACK clear_buffer;
	&usart0_driver_set_address,		//	TRX_DRIVER_INTERFACE_SET_ADDRESS_CALLBACK set_address;
	&usart0_driver_mutex_request,		//
	&usart0_driver_mutex_release		//
};
#endif // HAS_DRIVER_USART0 && HAS_DRIVER_USART0 == 1

//-----------------------------------------------------------------------------

#if defined HAS_DRIVER_USART1 && HAS_DRIVER_USART1 == 1

#include "driver/communication/usart/usart1_driver.h"

/*!
 *
 */
static TRX_DRIVER_INTERFACE usart1_driver = {
	USART,					//	TRX_DRIVER_INTERFACE_TYPE type;
	&usart1_driver_initialize,		//
	&usart1_driver_configure, 		//	TRX_DRIVER_INTERFACE_CONFIGURE_CALLBACK configure;
	&usart1_driver_power_off, 		//	TRX_DRIVER_INTERFACE_MODULE_OFF_CALLBACK shut_down;
	&usart1_driver_bytes_available, 	//	TRX_DRIVER_INTERFACE_BYTES_AVAILABLE_CALLBACK bytes_available;
	&usart1_driver_get_N_bytes, 		//	TRX_DRIVER_INTERFACE_GET_N_BYTES_CALLBACK get_N_bytes;
	&usart1_driver_set_N_bytes, 		//	TRX_DRIVER_INTERFACE_SET_N_BYTES_CALLBACK set_N_bytes;
	&usart1_driver_start_rx, 		//	TRX_DRIVER_INTERFACE_START_RX_CALLBACK start_rx;
	&usart1_driver_wait_for_rx,		//
	&usart1_driver_stop_rx, 		//	TRX_DRIVER_INTERFACE_STOP_RX_CALLBACK stop_rx;
	&usart1_driver_is_ready_for_tx,		//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
	&usart1_driver_is_ready_for_rx,		//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
	&usart1_driver_start_tx, 		//	TRX_DRIVER_INTERFACE_START_TX_CALLBACK start_tx;
	&usart1_driver_wait_for_tx,		//
	&usart1_driver_stop_tx, 		//	TRX_DRIVER_INTERFACE_STOP_TX_CALLBACK stop_tx;
	&usart1_driver_clear_rx_buffer, 	//	TRX_DRIVER_INTERFACE_CLEAR_BUFFER_CALLBACK clear_buffer;
	&usart1_driver_clear_tx_buffer, 	//	TRX_DRIVER_INTERFACE_CLEAR_BUFFER_CALLBACK clear_buffer;
	&usart1_driver_set_address,		//	TRX_DRIVER_INTERFACE_SET_ADDRESS_CALLBACK set_address;
	&usart1_driver_mutex_request,		//
	&usart1_driver_mutex_release		//
};
#endif // HAS_DRIVER_USART1 && HAS_DRIVER_USART1 == 1

//-----------------------------------------------------------------------------

#if defined HAS_DRIVER_I2C0 && HAS_DRIVER_I2C0 == 1

config_I2C_POWER_DOWN_PROTOTYPE

#include "driver/communication/i2c/i2c0_driver.h"

/*!
 *
 */
static TRX_DRIVER_INTERFACE i2c0_driver = {
	I2C,					//	TRX_DRIVER_INTERFACE_TYPE type;
	&i2c0_driver_initialize,
	&i2c0_driver_configure, 		//	TRX_DRIVER_INTERFACE_CONFIGURE_CALLBACK configure;
	&i2c0_driver_power_off,			//	TRX_DRIVER_INTERFACE_MODULE_OFF_CALLBACK shut_down;
	&i2c0_driver_bytes_available, 		//	TRX_DRIVER_INTERFACE_BYTES_AVAILABLE_CALLBACK bytes_available;
	&i2c0_driver_get_N_bytes, 		//	TRX_DRIVER_INTERFACE_GET_N_BYTES_CALLBACK get_N_bytes;
	&i2c0_driver_set_N_bytes, 		//	TRX_DRIVER_INTERFACE_SET_N_BYTES_CALLBACK set_N_bytes;
	&i2c0_driver_start_rx, 			//	TRX_DRIVER_INTERFACE_START_RX_CALLBACK start_rx;
	&i2c0_driver_wait_for_rx,		//
	&i2c0_driver_stop_rx, 			//	TRX_DRIVER_INTERFACE_STOP_RX_CALLBACK stop_rx;
	&i2c0_driver_is_ready_for_tx,		//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
	&i2c0_driver_is_ready_for_rx,		//	TRX_DRIVER_INTERFACE_IS_READ_FOR_TX_CALLBACK is_ready_for_tx;
	&i2c0_driver_start_tx, 			//	TRX_DRIVER_INTERFACE_START_TX_CALLBACK start_tx;
	&i2c0_driver_wait_for_tx,		//
	&i2c0_driver_stop_tx, 			//	TRX_DRIVER_INTERFACE_STOP_TX_CALLBACK stop_tx;
	&i2c0_driver_clear_rx_buffer, 		//	TRX_DRIVER_INTERFACE_CLEAR_BUFFER_CALLBACK clear_buffer;
	&i2c0_driver_clear_tx_buffer, 		//	TRX_DRIVER_INTERFACE_CLEAR_BUFFER_CALLBACK clear_buffer;
	&i2c0_driver_set_address,		//	TRX_DRIVER_INTERFACE_SET_ADDRESS_CALLBACK set_address;
	&i2c0_driver_mutex_request,		//
	&i2c0_driver_mutex_release		//
};
#endif // #if defined HAS_DRIVER_I2C0 && HAS_DRIVER_I2C0 == 1

//-----------------------------------------------------------------------------

#ifdef HAS_DRIVER_RTC0
#include "driver/rtc/rtc_driver_interface.h"
#endif

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

const SYSTEM_INTERFACE i_system = {
	
	#ifdef HAS_DRIVER_RTC0
	.time = {			
		.now_u8 = &local_rtc_timer_gettime_u8,
		.now_u16 = &local_rtc_timer_gettime_u16,
		.now_u32 = &local_rtc_timer_gettime_u32,
		.isup_u8 = &local_rtc_timer_istimeup_u8,
		.isup_u16 = &local_rtc_timer_istimeup_u16,
		.isup_u32 = &local_rtc_timer_istimeup_u32
	},
	#endif

	.event = {
		&config_SYSTEM_INTERFACE_ADD_EVENT_CALLBACK,
		&config_SYSTEM_INTERFACE_GET_EVENT_CALLBACK
	},

	.memory = {
		.progmem = {
			&config_SYSTEM_INTERFACE_PROGMEM_GET_BYTE_CALLBACK,
			&config_SYSTEM_INTERFACE_PROGMEM_GET_WORD_CALLBACK,
			&config_SYSTEM_INTERFACE_PROGMEM_GET_LONG_CALLBACK,
			&config_SYSTEM_INTERFACE_PROGMEM_GET_N_BYTES_CALLBACK
		}
	},

	.io = {
		&config_SYSTEM_INTERFACE_IO_INIT_PIN_CALLBACK,
		&config_SYSTEM_INTERFACE_IO_SET_PIN_DIR_CALLBACK,
		&config_SYSTEM_INTERFACE_IO_SET_PIN_LEVEL_CALLBACK,
		&config_SYSTEM_INTERFACE_IO_TOGGLE_PIN_LEVEL_CALLBACK,
		&config_SYSTEM_INTERFACE_IO_GET_PIN_LEVEL_CALLBACK
	},
	.driver = {
		#if defined HAS_DRIVER_SPI0 && HAS_DRIVER_SPI0 == 1
		&spi0_driver,
		#endif

		#if defined HAS_DRIVER_USART0 && HAS_DRIVER_USART0 == 1
		&usart0_driver,
		#endif

		#if defined HAS_DRIVER_USART1 && HAS_DRIVER_USART1 == 1
		&usart1_driver,
		#endif

		#if defined HAS_DRIVER_I2C0 && HAS_DRIVER_I2C0 == 1
		&i2c0_driver
		#endif
	}
};
