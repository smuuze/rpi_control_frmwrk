#ifndef _SYS_PIN_MAP_H_
#define _SYS_PIN_MAP_H_

#include "config.h"
#include "board_common_config.h"

#define SYS_CRYSTAL_FREQ		14745600
#define SYS_CPU_CLK_PRESCALER		2
#define SYS_CPU_CLK_FREQ		(SYS_CRYSTAL_FREQ / SYS_CPU_CLK_PRESCALER)

//----------------------------------------------------------------------------------------------------------------------------------
//   Port A (0x02
//---------  Name , Port, Bit,	  Idle-State
INCLUDE_GPIO(EXTERN_OUTPUT_04)
INCLUDE_GPIO(EXTERN_OUTPUT_03)
INCLUDE_GPIO(EXTERN_OUTPUT_02)
INCLUDE_GPIO(EXTERN_OUTPUT_01)
INCLUDE_GPIO(EVENT_OUTPUT)
INCLUDE_GPIO(EXT_POWER_01)
INCLUDE_GPIO(MCU_PIN_31)
INCLUDE_GPIO(MCU_PIN_30)

//----------------------------------------------------------------------------------------------------------------------------------
//    Port B (0x05)
//---------  Name , Port, Bit,	  Idle-State
INCLUDE_GPIO(EXTERN_INPUT_01)
INCLUDE_GPIO(EXTERN_INPUT_02)
INCLUDE_GPIO(EXTERN_INPUT_03)
INCLUDE_GPIO(EXTERN_INPUT_04)
INCLUDE_GPIO(HOST_SPI_CE)
INCLUDE_GPIO(HOST_SPI_MOSI)
INCLUDE_GPIO(HOST_SPI_MISO)
INCLUDE_GPIO(HOST_SPI_SCK)

//----------------------------------------------------------------------------------------------------------------------------------
//   Port C (0x08)
//---------  Name , Port, Bit,	  Idle-State
INCLUDE_GPIO(I2C_SCL)
INCLUDE_GPIO(I2C_SDA)
INCLUDE_GPIO(EXT_POWER_02)
INCLUDE_GPIO(EXT_RESET)
INCLUDE_GPIO(MCU_PIN_23)
INCLUDE_GPIO(MCU_PIN_24)
INCLUDE_GPIO(MCU_PIN_25)
INCLUDE_GPIO(MCU_PIN_26)

//----------------------------------------------------------------------------------------------------------------------------------
//   Port D (0x0B)
//---------  Name , Port, Bit,	  Idle-State
INCLUDE_GPIO(UART0_RX)
INCLUDE_GPIO(UART0_TX)
INCLUDE_GPIO(MASTER_SPI_MOSI)
INCLUDE_GPIO(MASTER_SPI_MISO)
INCLUDE_GPIO(MASTER_SPI_CLK)
INCLUDE_GPIO(MCU_PIN_14)
INCLUDE_GPIO(TRACE_OUT)
INCLUDE_GPIO(READY_INOUT)


#endif
