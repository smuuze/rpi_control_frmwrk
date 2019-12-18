//--------------------------------------------------------------------------------------------------------------
//       Pin-Map   :  RPi-Control Board v1
//--------------------------------------------------------------------------------------------------------------

#include "platine/board_common_config.h"

//----------------------------------------------------------------------------------------------------------------------------------
//   Port A (0x02
//---------  Name , Port, Bit,	  Idle-State
BUILD_GPIO(EXTERN_OUTPUT_04,	GPIO_PORT_A, 	GPIO_PIN_0, 	GPIO_OUTPUT | GPIO_IDLE_LOW		)	// has extern Pull-Up resistor
BUILD_GPIO(EXTERN_OUTPUT_03,	GPIO_PORT_A, 	GPIO_PIN_1, 	GPIO_OUTPUT | GPIO_IDLE_LOW		)	// has extern Pull-Up resistor
BUILD_GPIO(EXTERN_OUTPUT_02,	GPIO_PORT_A, 	GPIO_PIN_2, 	GPIO_OUTPUT | GPIO_IDLE_LOW		)	// has extern Pull-Up resistor
BUILD_GPIO(EXTERN_OUTPUT_01,	GPIO_PORT_A, 	GPIO_PIN_3, 	GPIO_OUTPUT | GPIO_IDLE_LOW		)	// has extern Pull-Up resistor
BUILD_GPIO(EVENT_OUTPUT,	GPIO_PORT_A, 	GPIO_PIN_4, 	GPIO_OUTPUT | GPIO_IDLE_LOW		)	// has extern Pull-Up resistor
BUILD_GPIO(EXT_POWER_01,	GPIO_PORT_A, 	GPIO_PIN_5, 	GPIO_OUTPUT | GPIO_IDLE_LOW		)	// has extern Pull-Up resistor
BUILD_GPIO(MCU_PIN_31,		GPIO_PORT_A, 	GPIO_PIN_6, 	GPIO_INPUT | GPIO_IDLE_HIGH 		)
BUILD_GPIO(MCU_PIN_30, 		GPIO_PORT_A, 	GPIO_PIN_7, 	GPIO_INPUT | GPIO_IDLE_HIGH 		)

//----------------------------------------------------------------------------------------------------------------------------------
//    Port B (0x05)
//---------  Name , 		Port, 		Bit,		Idle-State
BUILD_GPIO(EXTERN_INPUT_01, 	GPIO_PORT_B, 	GPIO_PIN_0, 	GPIO_INVERTED | GPIO_IDLE_HIGH_Z 	)
BUILD_GPIO(EXTERN_INPUT_02, 	GPIO_PORT_B, 	GPIO_PIN_1, 	GPIO_INVERTED | GPIO_IDLE_HIGH_Z 	)
BUILD_GPIO(EXTERN_INPUT_03, 	GPIO_PORT_B, 	GPIO_PIN_2, 	GPIO_INVERTED | GPIO_IDLE_HIGH_Z	)
BUILD_GPIO(EXTERN_INPUT_04, 	GPIO_PORT_B, 	GPIO_PIN_3, 	GPIO_INVERTED | GPIO_IDLE_HIGH_Z	)
BUILD_GPIO(SPI0_CE,		GPIO_PORT_B, 	GPIO_PIN_4, 	GPIO_INPUT  | GPIO_IDLE_HIGH_Z		)
BUILD_GPIO(SPI0_MOSI, 		GPIO_PORT_B, 	GPIO_PIN_5, 	GPIO_INPUT  | GPIO_IDLE_HIGH_Z		)
BUILD_GPIO(SPI0_MISO, 		GPIO_PORT_B, 	GPIO_PIN_6, 	GPIO_OUTPUT | GPIO_IDLE_LOW 		)
BUILD_GPIO(SPI0_SCK, 		GPIO_PORT_B, 	GPIO_PIN_7, 	GPIO_INPUT  | GPIO_IDLE_HIGH_Z		)

//----------------------------------------------------------------------------------------------------------------------------------
//   Port C (0x08)
//---------  Name , Port, Bit,	  Idle-State
BUILD_GPIO(I2C_SCL, 		GPIO_PORT_C, 	GPIO_PIN_0, 	GPIO_IDLE_HIGH_Z			)	// has extern Pull-Up resistor
BUILD_GPIO(I2C_SDA, 		GPIO_PORT_C, 	GPIO_PIN_1, 	GPIO_IDLE_HIGH_Z			)	// has extern Pull-Up resistor
BUILD_GPIO(EXT_POWER_02, 	GPIO_PORT_C, 	GPIO_PIN_2, 	GPIO_OUTPUT | GPIO_IDLE_LOW		)
BUILD_GPIO(EXT_RESET, 		GPIO_PORT_C, 	GPIO_PIN_3, 	GPIO_INVERTED | GPIO_IDLE_LOW		)
BUILD_GPIO(MCU_PIN_23, 		GPIO_PORT_C, 	GPIO_PIN_4, 	GPIO_INPUT | GPIO_IDLE_HIGH		)
BUILD_GPIO(MCU_PIN_24, 		GPIO_PORT_C, 	GPIO_PIN_5, 	GPIO_INPUT | GPIO_IDLE_HIGH		)
BUILD_GPIO(MCU_PIN_25, 		GPIO_PORT_C, 	GPIO_PIN_6, 	GPIO_INPUT | GPIO_IDLE_HIGH		)
BUILD_GPIO(MCU_PIN_26, 		GPIO_PORT_C, 	GPIO_PIN_7, 	GPIO_INPUT | GPIO_IDLE_HIGH		)

//----------------------------------------------------------------------------------------------------------------------------------
//   Port D (0x0B)
//---------  Name , Port, Bit,	  Idle-State
BUILD_GPIO(UART0_RX, 		GPIO_PORT_D, 	GPIO_PIN_0, 	GPIO_INPUT | GPIO_IDLE_HIGH		)
BUILD_GPIO(UART0_TX, 		GPIO_PORT_D, 	GPIO_PIN_1, 	GPIO_INPUT | GPIO_IDLE_HIGH		)
BUILD_GPIO(MASTER_SPI_MOSI, 	GPIO_PORT_D, 	GPIO_PIN_2, 	GPIO_INPUT | GPIO_IDLE_HIGH		)
BUILD_GPIO(MASTER_SPI_MISO, 	GPIO_PORT_D, 	GPIO_PIN_3, 	GPIO_INPUT | GPIO_IDLE_HIGH		)
BUILD_GPIO(MASTER_SPI_CLK, 	GPIO_PORT_D, 	GPIO_PIN_4, 	GPIO_INPUT | GPIO_IDLE_HIGH		)
BUILD_GPIO(MCU_PIN_14, 		GPIO_PORT_D, 	GPIO_PIN_5, 	GPIO_INPUT | GPIO_IDLE_HIGH		)
BUILD_GPIO(TRACE_OUT, 		GPIO_PORT_D, 	GPIO_PIN_6, 	GPIO_OUTPUT | GPIO_IDLE_LOW		)
BUILD_GPIO(READY_INOUT, 	GPIO_PORT_D, 	GPIO_PIN_7, 	GPIO_OUTPUT | GPIO_IDLE_LOW		)
