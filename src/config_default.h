
//----------------------------------------------------------------------------
//      Konfiguration fuer Systemtimer
//----------------------------------------------------------------------------
#ifdef CPU_PRESCALER
  #define config_CPU_CLK_PRESCALER (CPU_PRESCALER)
#else
  #define config_CPU_CLK_PRESCALER 1
#endif

#ifdef config_CPU_CLK_HZ
ASSERT_C(0, config_CPU_CLK_HZ_ALREADY_DEFINED);
#endif

#ifndef config_CPU_CLK_HZ
#define  config_CPU_CLK_HZ  ((CPU_XTAL_HZ)/(config_CPU_CLK_PRESCALER))
#endif

// Minimalwert fuer Timerueberlauf : 32768us (d.h. TCLK <= 2MHz)
// Maximalwert fuer Timerstep : 4us (d.h. TCLK >= 250kHz)
#if   ( config_CPU_CLK_HZ <= 2000000 )
  #define config_SYSTEMTIMER_PRESCALER 1
#elif ( config_CPU_CLK_HZ <= 16000000 )
  #define config_SYSTEMTIMER_PRESCALER 8
#else
  #define config_SYSTEMTIMER_PRESCALER 64
#endif

//----------------------------------------------------------------------------

#ifndef config_RF_CLK_HZ
#define config_RF_CLK_HZ    (125000)
#endif

#if SIO_FOR_HOST == '-'
#define RTOS_H "../src/fakertos/fakertos.h"
#else
#define RTOS_H "fakertos.h"
#endif

#ifndef config_FAKERTOS_TASKYIELD_INIT_PROTO
#define config_FAKERTOS_TASKYIELD_INIT_PROTO
#endif

#ifndef config_FAKERTOS_TASKYIELD_FCT_PROTO
#define config_FAKERTOS_TASKYIELD_FCT_PROTO		\
	void mcu_task_controller_background_run(void);
#endif

#define config_FAKERTOS_TASKYIELD_INIT

#ifndef config_FAKERTOS_TASKYIELD_FUNCTION
#define config_FAKERTOS_TASKYIELD_FUNCTION		mcu_task_controller_background_run();
#endif

// -----------------------------------------------------------------------------

#ifndef config_HAS_ONBOARD_BUTTONS
#define config_HAS_ONBOARD_BUTTONS				0
#endif

#ifndef config_HAS_LED_MATRIX
#define config_HAS_LED_MATRIX				      0
#endif

// -----------------------------------------------------------------------------
// Driver Interface

#define config_I2C_POWER_DOWN_PROTOTYPE				void specific_i2c_power_down(void);
#define config_I2C_POWER_DOWN_FUNCTION_REF			&specific_spi_power_down

#define config_SPI_POWER_DOWN_PROTOTYPE				void specific_spi_power_down(void);
#define config_SPI_POWER_DOWN_FUNCTION_REF			&specific_spi_power_down

// -----------------------------------------------------------------------------
// System Interface

#define config_SYSTEM_INTERFACE_ADD_EVENT_PROTOTYPE		void local_event_add(SYSTEM_EVENT event);
#define config_SYSTEM_INTERFACE_GET_EVENT_PROTOTYPE		SYSTEM_EVENT local_event_get_next(void);

#define config_SYSTEM_INTERFACE_ADD_EVENT_CALLBACK		local_event_add
#define config_SYSTEM_INTERFACE_GET_EVENT_CALLBACK		local_event_get_next

#define config_SYSTEM_INTERFACE_GET_TIME_U8_PROTOTYPE		u8  local_rtc_timer_gettime_u8(void); /*ux8 msticker_get_time_ux8(void);*/
#define config_SYSTEM_INTERFACE_GET_TIME_U16_PROTOTYPE		u16 local_rtc_timer_gettime_u16(void); /*ux16 msticker_get_time_ux16(void);*/
#define config_SYSTEM_INTERFACE_GET_TIME_U32_PROTOTYPE		u32 local_rtc_timer_gettime_u32(void); /*ux32 msticker_get_time_ux32(void);*/

#define config_SYSTEM_INTERFACE_GET_TIME_U8_CALLBACK		local_rtc_timer_gettime_u8 /*msticker_get_time_ux8*/
#define config_SYSTEM_INTERFACE_GET_TIME_U16_CALLBACK		local_rtc_timer_gettime_u16 /*msticker_get_time_ux16*/
#define config_SYSTEM_INTERFACE_GET_TIME_U32_CALLBACK		local_rtc_timer_gettime_u32 /*msticker_get_time_ux32*/

#define config_SYSTEM_INTERFACE_IS_TIME_UP_U8_PROTOTYPE		u8 local_rtc_timer_istimeup_u8(u8 time_reference, u8 time_interval); /*ux8 mstimer_time_is_up_ux8(const ux8 reference_time, const ux8 time_interval);*/
#define config_SYSTEM_INTERFACE_IS_TIME_UP_U16_PROTOTYPE	u8 local_rtc_timer_istimeup_u16(u16 time_reference, u16 time_interval); /*ux8 mstimer_time_is_up_ux16(const ux16 reference_time, const ux16 time_interval);*/
#define config_SYSTEM_INTERFACE_IS_TIME_UP_U32_PROTOTYPE	u8 local_rtc_timer_istimeup_u32(u32 time_reference, u32 time_interval); /*ux8 mstimer_time_is_up_ux32(const ux32 reference_time, const ux32 time_interval);*/

#define config_SYSTEM_INTERFACE_IS_TIME_UP_U8_CALLBACK		local_rtc_timer_istimeup_u8 /*mstimer_time_is_up_ux8*/
#define config_SYSTEM_INTERFACE_IS_TIME_UP_U16_CALLBACK		local_rtc_timer_istimeup_u16 /*mstimer_time_is_up_ux16*/
#define config_SYSTEM_INTERFACE_IS_TIME_UP_U32_CALLBACK		local_rtc_timer_istimeup_u32 /*mstimer_time_is_up_ux32*/

//#define config_SYSTEM_INTERFACE_PROGMEM_GET_BYTE_PROTOTYPE	static u8 local_progmem_get_byte(u8* addr);
//#define config_SYSTEM_INTERFACE_PROGMEM_GET_WORD_PROTOTYPE	static u16 local_progmem_get_word(u8* addr);
//#define config_SYSTEM_INTERFACE_PROGMEM_GET_LONG_PROTOTYPE	static u32 local_progmem_get_long(u8* addr);
//#define config_SYSTEM_INTERFACE_PROGMEM_GET_N_BYTES_PROTOTYPE	static void local_progmem_get_N_bytes(u8* addr, u8* p_buffer_to, u8 num_bytes);

//#define config_SYSTEM_INTERFACE_PROGMEM_GET_BYTE_CALLBACK	local_progmem_get_byte
//#define config_SYSTEM_INTERFACE_PROGMEM_GET_WORD_CALLBACK	local_progmem_get_word
//#define config_SYSTEM_INTERFACE_PROGMEM_GET_LONG_CALLBACK	local_progmem_get_long
//#define config_SYSTEM_INTERFACE_PROGMEM_GET_N_BYTES_CALLBACK	local_progmem_get_N_bytes

#define config_SYSTEM_INTERFACE_IO_INIT_PIN_PROTOTYPE		void gpio_driver_init_pin(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);
#define config_SYSTEM_INTERFACE_IO_SET_PIN_DIR_PROTOTYPE 	void gpio_driver_set_direction(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, SYSTEM_INTERFACE_GPIO_DIRECTION direction);
#define config_SYSTEM_INTERFACE_IO_SET_PIN_LEVEL_PROTOTYPE 	void gpio_driver_set_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, SYSTEM_INTERFACE_GPIO_LEVEL level);
#define config_SYSTEM_INTERFACE_IO_TOGGLE_PIN_LEVEL_PROTOTYPE	void gpio_driver_toggle_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);
#define config_SYSTEM_INTERFACE_IO_GET_PIN_LEVEL_PROTOTYPE 	SYSTEM_INTERFACE_GPIO_LEVEL gpio_driver_get_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);

#define config_SYSTEM_INTERFACE_IO_INIT_PIN_CALLBACK		gpio_driver_init_pin
#define config_SYSTEM_INTERFACE_IO_SET_PIN_DIR_CALLBACK 	gpio_driver_set_direction
#define config_SYSTEM_INTERFACE_IO_SET_PIN_LEVEL_CALLBACK	gpio_driver_set_level
#define config_SYSTEM_INTERFACE_IO_TOGGLE_PIN_LEVEL_CALLBACK	gpio_driver_toggle_level
#define config_SYSTEM_INTERFACE_IO_GET_PIN_LEVEL_CALLBACK 	gpio_driver_get_level


///-----------------------------------------------------------------------------

#define config_IS_READY_ENABLE					IS_READY_as_OUTPUT();		\
								IS_READY_OFF()
#define config_IS_READY_IDLE            IS_READY_as_OUTPUT();		\
								IS_READY_ON();
#define config_IS_READY_DISABLE					IS_READY_as_INPUT();		\
								IS_READY_OFF()

///-----------------------------------------------------------------------------

#if !defined (config_OFFLINE_MODE_ENTER_FUNC_PROTO) && !defined (config_OFFLINE_MODE_LEAVE_FUNC_PROTO)
#define config_OFFLINE_MODE_ENTER_FUNC_PROTO
#define config_OFFLINE_MODE_ENTER_FUNC_CALL
#endif

#if !defined (config_OFFLINE_MODE_LEAVE_FUNC_PROTO) && !defined (config_OFFLINE_MODE_LEAVE_FUNC_CALL)
#define config_OFFLINE_MODE_LEAVE_FUNC_PROTO
#define config_OFFLINE_MODE_LEAVE_FUNC_CALL
#endif

#if !defined config_DEBUS_SET_ACTIVE_FUNC_PROTO && !defined config_DEBUS_SET_ACTIVE_FUNC_CALL && !defined config_DEBUS_SET_INACTIVE_FUNC_CALL
#define config_DEBUS_SET_ACTIVE_FUNC_PROTO
#define config_DEBUS_SET_ACTIVE_FUNC_CALL
#define config_DEBUS_SET_INACTIVE_FUNC_CALL
#endif

///-----------------------------------------------------------------------------

#include "common/common_types.h"
#include "cpu.h"