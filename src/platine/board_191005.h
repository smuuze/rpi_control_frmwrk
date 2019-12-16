
#ifndef _BOARD_191005_H_
#define _BOARD_191005_H_

#include "config.h"
#include "board_common_config.h"

#define SYS_CRYSTAL_FREQ		14745600
#define SYS_CPU_CLK_PRESCALER		2
#define SYS_CPU_CLK_FREQ		(SYS_CRYSTAL_FREQ / SYS_CPU_CLK_PRESCALER)

//----------------------------------------------------------------------------------------------------------------------------------
//   Port A (0x02
//---------
INCLUDE_GPIO(LED_GREEN)
INCLUDE_GPIO(LED_BLUE)
INCLUDE_GPIO(LED_RED)
INCLUDE_GPIO(UNUSED_A3)
INCLUDE_GPIO(UNUSED_A4)
INCLUDE_GPIO(UNUSED_A5)
INCLUDE_GPIO(UNUSED_A6)
INCLUDE_GPIO(UNUSED_A7)

#define HAS_GPIO_LED_GREEN		1
#define HAS_GPIO_LED_BLUE		1
#define HAS_GPIO_LED_RED		1

//----------------------------------------------------------------------------------------------------------------------------------
//    Port B (0x05)
//---------
INCLUDE_GPIO(UNUSED_B0)
INCLUDE_GPIO(IR_CARRIER_IN)
INCLUDE_GPIO(UNUSED_B2)
INCLUDE_GPIO(IR_CARRIER_OUT)
INCLUDE_GPIO(IR_MOD_OUT)
INCLUDE_GPIO(PROG_MOSI)
INCLUDE_GPIO(PROG_MISO)
INCLUDE_GPIO(PROG_SCK)

#define HAS_GPIO_IR_CARRIER		1
#define HAS_GPIO_IR_MOD			1

//----------------------------------------------------------------------------------------------------------------------------------
//   Port C (0x08)
//---------
INCLUDE_GPIO(I2C_SCL)
INCLUDE_GPIO(I2C_SDA)
INCLUDE_GPIO(UNUSED_C2)
INCLUDE_GPIO(UNUSED_C3)
INCLUDE_GPIO(UNUSED_C4)
INCLUDE_GPIO(UNUSED_C5)
INCLUDE_GPIO(XT0)
INCLUDE_GPIO(XT1)

//----------------------------------------------------------------------------------------------------------------------------------
//   Port D (0x0B)
//---------
INCLUDE_GPIO(UART0_RX)
INCLUDE_GPIO(UART0_TX)
INCLUDE_GPIO(UNUSED_D2)
INCLUDE_GPIO(UNUSED_D3)
INCLUDE_GPIO(UNUSED_D4)
INCLUDE_GPIO(UNUSED_D5)
INCLUDE_GPIO(UNUSED_D6)
INCLUDE_GPIO(UNUSED_D7)


#endif // _BOARD_191005_H_
