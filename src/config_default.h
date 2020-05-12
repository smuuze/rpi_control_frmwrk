
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

//#define config_SYSTEM_INTERFACE_PROGMEM_GET_BYTE_PROTOTYPE	static u8 local_progmem_get_byte(u8* addr);
//#define config_SYSTEM_INTERFACE_PROGMEM_GET_WORD_PROTOTYPE	static u16 local_progmem_get_word(u8* addr);
//#define config_SYSTEM_INTERFACE_PROGMEM_GET_LONG_PROTOTYPE	static u32 local_progmem_get_long(u8* addr);
//#define config_SYSTEM_INTERFACE_PROGMEM_GET_N_BYTES_PROTOTYPE	static void local_progmem_get_N_bytes(u8* addr, u8* p_buffer_to, u8 num_bytes);

//#define config_SYSTEM_INTERFACE_PROGMEM_GET_BYTE_CALLBACK	local_progmem_get_byte
//#define config_SYSTEM_INTERFACE_PROGMEM_GET_WORD_CALLBACK	local_progmem_get_word
//#define config_SYSTEM_INTERFACE_PROGMEM_GET_LONG_CALLBACK	local_progmem_get_long
//#define config_SYSTEM_INTERFACE_PROGMEM_GET_N_BYTES_CALLBACK	local_progmem_get_N_bytes

#define config_SYSTEM_INTERFACE_IO_INIT_PIN_PROTOTYPE		void gpio_driver_init_pin(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);
#define config_SYSTEM_INTERFACE_IO_SET_PIN_DIR_PROTOTYPE 	void gpio_driver_set_direction(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, GPIO_DRIVER_DIRECTION direction);
#define config_SYSTEM_INTERFACE_IO_SET_PIN_LEVEL_PROTOTYPE 	void gpio_driver_set_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, GPIO_DRIVER_LEVEL level);
#define config_SYSTEM_INTERFACE_IO_TOGGLE_PIN_LEVEL_PROTOTYPE	void gpio_driver_toggle_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);
#define config_SYSTEM_INTERFACE_IO_GET_PIN_LEVEL_PROTOTYPE 	GPIO_DRIVER_LEVEL gpio_driver_get_level(const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);

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

#include "common/common_types.h"
#include "cpu.h"