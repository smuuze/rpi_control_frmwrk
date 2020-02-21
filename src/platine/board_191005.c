//--------------------------------------------------------------------------------------------------------------
//       Pin-Map   :  RPi-IR-Board v1
//--------------------------------------------------------------------------------------------------------------

#include "platine/board_common_config.h"


//----------------------------------------------------------------------------------------------------------------------------------
//   Port A (0x02
//---------  Name , Port, Bit,	  Idle-State
BUILD_GPIO ( LED_GREEN,		GPIO_PORT_A, 	GPIO_PIN_0, 	GPIO_OUTPUT | GPIO_INVERTED | GPIO_IDLE_HIGH		)
BUILD_GPIO ( LED_BLUE,		GPIO_PORT_A, 	GPIO_PIN_1, 	GPIO_OUTPUT | GPIO_INVERTED | GPIO_IDLE_HIGH		)
BUILD_GPIO ( LED_RED,		GPIO_PORT_A, 	GPIO_PIN_2, 	GPIO_OUTPUT | GPIO_INVERTED | GPIO_IDLE_HIGH		)
BUILD_GPIO ( UNUSED_A3,		GPIO_PORT_A, 	GPIO_PIN_3, 	GPIO_INPUT | GPIO_IDLE_HIGH				)
BUILD_GPIO ( UNUSED_A4,		GPIO_PORT_A, 	GPIO_PIN_4, 	GPIO_INPUT | GPIO_IDLE_HIGH				)
BUILD_GPIO ( UNUSED_A5,		GPIO_PORT_A, 	GPIO_PIN_5, 	GPIO_INPUT | GPIO_IDLE_HIGH				)
BUILD_GPIO ( UNUSED_A6,		GPIO_PORT_A, 	GPIO_PIN_6, 	GPIO_INPUT | GPIO_IDLE_HIGH 				)
BUILD_GPIO ( UNUSED_A7, 	GPIO_PORT_A, 	GPIO_PIN_7, 	GPIO_INPUT | GPIO_IDLE_HIGH 				)

//----------------------------------------------------------------------------------------------------------------------------------
//    Port B (0x05)
//---------  Name , 		Port, 		Bit,		Idle-State
BUILD_GPIO ( UNUSED_B0, 	GPIO_PORT_B, 	GPIO_PIN_0, 	GPIO_INPUT | GPIO_IDLE_HIGH 				)
BUILD_GPIO ( IR_CARRIER_IN, 	GPIO_PORT_B, 	GPIO_PIN_1, 	GPIO_INPUT 						)
BUILD_GPIO ( UNUSED_B2, 	GPIO_PORT_B, 	GPIO_PIN_2, 	GPIO_INPUT | GPIO_IDLE_HIGH				)
BUILD_GPIO ( IR_CARRIER_OUT, 	GPIO_PORT_B, 	GPIO_PIN_3, 	GPIO_OUTPUT | GPIO_IDLE_HIGH				)
BUILD_GPIO ( IR_MOD_OUT,	GPIO_PORT_B, 	GPIO_PIN_4, 	GPIO_OUTPUT  | GPIO_IDLE_HIGH				)
BUILD_GPIO ( PROG_MOSI, 	GPIO_PORT_B, 	GPIO_PIN_5, 	GPIO_INPUT | GPIO_IDLE_HIGH				)
BUILD_GPIO ( PROG_MISO, 	GPIO_PORT_B, 	GPIO_PIN_6, 	GPIO_INPUT | GPIO_IDLE_HIGH 				)
BUILD_GPIO ( PROG_SCK, 		GPIO_PORT_B, 	GPIO_PIN_7, 	GPIO_INPUT | GPIO_IDLE_HIGH				)

//----------------------------------------------------------------------------------------------------------------------------------
//   Port C (0x08)
//---------  Name , Port, Bit,	  Idle-State
BUILD_GPIO ( I2C_SCL, 		GPIO_PORT_C, 	GPIO_PIN_0, 	GPIO_IDLE_HIGH_Z					)
BUILD_GPIO ( I2C_SDA, 		GPIO_PORT_C, 	GPIO_PIN_1, 	GPIO_IDLE_HIGH_Z					)
BUILD_GPIO ( UNUSED_C2, 	GPIO_PORT_C, 	GPIO_PIN_2, 	GPIO_INPUT | GPIO_IDLE_HIGH				)
BUILD_GPIO ( UNUSED_C3, 	GPIO_PORT_C, 	GPIO_PIN_3, 	GPIO_INPUT | GPIO_IDLE_HIGH				)
BUILD_GPIO ( UNUSED_C4, 	GPIO_PORT_C, 	GPIO_PIN_4, 	GPIO_INPUT | GPIO_IDLE_HIGH				)
BUILD_GPIO ( UNUSED_C5, 	GPIO_PORT_C, 	GPIO_PIN_5, 	GPIO_INPUT | GPIO_IDLE_HIGH				)
BUILD_GPIO ( XT0, 		GPIO_PORT_C, 	GPIO_PIN_6, 	GPIO_INPUT | GPIO_IDLE_HIGH				)
BUILD_GPIO ( XT1, 		GPIO_PORT_C, 	GPIO_PIN_7, 	GPIO_INPUT | GPIO_IDLE_HIGH				)

//----------------------------------------------------------------------------------------------------------------------------------
//   Port D (0x0B)
//---------  Name , Port, Bit,	  Idle-State
BUILD_GPIO ( UART0_RX, 		GPIO_PORT_D, 	GPIO_PIN_0, 	GPIO_INPUT | GPIO_IDLE_HIGH				)
BUILD_GPIO ( UART0_TX, 		GPIO_PORT_D, 	GPIO_PIN_1, 	GPIO_INPUT | GPIO_IDLE_HIGH				)
BUILD_GPIO ( UNUSED_D2, 	GPIO_PORT_D, 	GPIO_PIN_2, 	GPIO_INPUT | GPIO_IDLE_HIGH				)
BUILD_GPIO ( UNUSED_D3, 	GPIO_PORT_D, 	GPIO_PIN_3, 	GPIO_INPUT | GPIO_IDLE_HIGH				)
BUILD_GPIO ( UNUSED_D4, 	GPIO_PORT_D, 	GPIO_PIN_4, 	GPIO_INPUT | GPIO_IDLE_HIGH				)
BUILD_GPIO ( UNUSED_D5, 	GPIO_PORT_D, 	GPIO_PIN_5, 	GPIO_INPUT | GPIO_IDLE_HIGH				)
BUILD_GPIO ( UNUSED_D6, 	GPIO_PORT_D, 	GPIO_PIN_6, 	GPIO_INPUT | GPIO_IDLE_HIGH				)
BUILD_GPIO ( UNUSED_D7, 	GPIO_PORT_D, 	GPIO_PIN_7, 	GPIO_INPUT | GPIO_IDLE_HIGH				)
