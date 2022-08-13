#ifndef _BOARD_180920_H_
#define _BOARD_180920_H_

//#include "config.h"
#include "driver/gpio/gpio_interface.h"

#define SYS_CRYSTAL_FREQ		14745600
#define SYS_CPU_CLK_PRESCALER		2
#define SYS_CPU_CLK_FREQ		(SYS_CRYSTAL_FREQ / SYS_CPU_CLK_PRESCALER)

//----------------------------------------------------------------------------------------------------------------------------------
//   Port A (0x02
//---------
INCLUDE_GPIO(EXTERN_OUTPUT_04)
INCLUDE_GPIO(EXTERN_OUTPUT_03)
INCLUDE_GPIO(EXTERN_OUTPUT_02)
INCLUDE_GPIO(EXTERN_OUTPUT_01)
INCLUDE_GPIO(EVENT_OUTPUT)
INCLUDE_GPIO(EXT_POWER_01)
INCLUDE_GPIO(MCU_PIN_31)
INCLUDE_GPIO(MCU_PIN_30)

#define HAS_GPIO_EXTERN_OUTPUT_01	1
#define HAS_GPIO_EXTERN_OUTPUT_02	1
#define HAS_GPIO_EXTERN_OUTPUT_03	1
#define HAS_GPIO_EXTERN_OUTPUT_04	1

#define HAS_GPIO_EVENT_OUTPUT		1
#define HAS_GPIO_EXT_POWER_01		1

//----------------------------------------------------------------------------------------------------------------------------------
//    Port B (0x05)
//---------
INCLUDE_GPIO(EXTERN_INPUT_01)
INCLUDE_GPIO(EXTERN_INPUT_02)
INCLUDE_GPIO(EXTERN_INPUT_03)
INCLUDE_GPIO(EXTERN_INPUT_04)
INCLUDE_GPIO(SPI0_CE)
INCLUDE_GPIO(SPI0_MOSI)
INCLUDE_GPIO(SPI0_MISO)
INCLUDE_GPIO(SPI0_SCK)

#define HAS_GPIO_EXTERN_INPUT_01	1
#define HAS_GPIO_EXTERN_INPUT_02	1
#define HAS_GPIO_EXTERN_INPUT_03	1
#define HAS_GPIO_EXTERN_INPUT_04	1

//----------------------------------------------------------------------------------------------------------------------------------
//   Port C (0x08)
//---------
INCLUDE_GPIO(I2C_SCL)
INCLUDE_GPIO(I2C_SDA)
INCLUDE_GPIO(EXT_POWER_02)
INCLUDE_GPIO(EXT_RESET)
INCLUDE_GPIO(MCU_PIN_23)
INCLUDE_GPIO(MCU_PIN_24)
INCLUDE_GPIO(MCU_PIN_25)
INCLUDE_GPIO(MCU_PIN_26)

#define HAS_GPIO_I2C_SCL		1
#define HAS_GPIO_I2C_SDA		1
#define HAS_GPIO_EXT_POWER_02		1
#define HAS_GPIO_EXT_RESET		1

//----------------------------------------------------------------------------------------------------------------------------------
//   Port D (0x0B)
//---------
INCLUDE_GPIO(UART0_RX)
INCLUDE_GPIO(UART0_TX)
INCLUDE_GPIO(UART1_RX)
INCLUDE_GPIO(UART1_TX)
INCLUDE_GPIO(MASTER_SPI_CLK)
INCLUDE_GPIO(MCU_PIN_14)
INCLUDE_GPIO(TRACE_OUT)
INCLUDE_GPIO(READY_INOUT)

#define HAS_GPIO_UART0_RX		1
#define HAS_GPIO_UART0_TX		1
#define HAS_GPIO_UART1_RX		1
#define HAS_GPIO_UART1_TX		1
#define HAS_GPIO_MASTER_SPI_CLK		1
#define HAS_GPIO_TRACE_OUT		1
#define HAS_GPIO_READY_INOUT		1


#endif // _BOARD_180920_H_
