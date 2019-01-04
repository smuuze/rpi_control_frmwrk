#ifndef _SYSTEM_INTERFACE_H_
#define _SYSTEM_INTERFACE_H_

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "utils/stdmacros.h"

#include "system_interface.h"


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

#ifndef config_INTERFACE_IO_SET_PIN_DIR_CALLBACK
void __system_interface_gpio_set_direction(u8 pin_num, SYSTEM_INTERFACE_GPIO_DIRECTION direction) { (void) pin_num; (void) direction; }
#define config_INTERFACE_IO_SET_PIN_DIR_CALLBACK __system_interface_gpio_set_direction
#endif

#ifndef config_SYSTEM_INTERFACE_IO_SET_PIN_LEVEL_CALLBACK
void __system_interface_gpio_set_level(u8 pin_num, SYSTEM_INTERFACE_IO_PIN_LEVEL level) { (void) pin_num; (void) level; }
#define config_SYSTEM_INTERFACE_IO_SET_PIN_LEVEL_CALLBACK __system_interface_gpio_set_level
#endif

#ifndef config_SYSTEM_INTERFACE_IO_GET_PIN_LEVEL_CALLBACK
SYSTEM_INTERFACE_IO_PIN_LEVEL __system_interface_gpio_get_level(u8 pin_num) { (void) pin_num; return GPIO_LEVEL_HIGH_Z; }
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

config_INTERFACE_IO_SET_PIN_DIR_PROTOTYPE
config_SYSTEM_INTERFACE_IO_SET_PIN_LEVEL_PROTOTYPE
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
		config_INTERFACE_IO_SET_PIN_DIR_CALLBACK,
		&config_SYSTEM_INTERFACE_IO_SET_PIN_LEVEL_CALLBACK,
		&config_SYSTEM_INTERFACE_IO_GET_PIN_LEVEL_CALLBACK
	}
};

#endif // _PROTOCOL_DRIVER_INTERFACE_H_
