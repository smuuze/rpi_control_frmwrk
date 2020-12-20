//--------------------------------------------------------------------------------------------------------------
//       Pin-Map   :  Raspberry Pi 40 Pin Header (since RPI B1 Plus)
//--------------------------------------------------------------------------------------------------------------

#include "platine/board_common_config.h"


//----------------------------------------------------------------------------------------------------------------------------------
// PORT A
//---------  Name , 		Port, 		Bit,		Idle-State						wiring-Pi

// 	BUILD_GPIO ( 3V3,		GPIO_PORT_A, 	GPIO_PIN_0, 	UNUSEABLE			) // 3V3
// 	BUILD_GPIO ( 5V,		GPIO_PORT_A, 	GPIO_PIN_1, 	UNUSEABLE			) // 5V
	BUILD_GPIO ( I2C_SDA,		GPIO_PORT_A, 	GPIO_PIN_2, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // I2C_SDA
// 	BUILD_GPIO ( 5V,		GPIO_PORT_A, 	GPIO_PIN_3, 	UNUSEABLE			) // 5V
	BUILD_GPIO ( I2C_SCL,		GPIO_PORT_A, 	GPIO_PIN_4, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // I2C_SCL
// 	BUILD_GPIO ( GND,		GPIO_PORT_A, 	GPIO_PIN_5, 	UNUSEABLE			) // GND
	BUILD_GPIO ( GPIO04, 		GPIO_PORT_A, 	GPIO_PIN_6, 	GPIO_INPUT | GPIO_IDLE_HIGH 	) // GPIO04	GPIO_07
	BUILD_GPIO ( UART_TXD,		GPIO_PORT_A, 	GPIO_PIN_7, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // UART_TXD

//----------------------------------------------------------------------------------------------------------------------------------
// PORT B
//---------  Name , 		Port, 		Bit,		Idle-State						wiring-Pi
//									
// 	BUILD_GPIO ( GND,		GPIO_PORT_B, 	GPIO_PIN_0, 	UNUSEABLE			) // GND
	BUILD_GPIO ( UART_RXD,		GPIO_PORT_B, 	GPIO_PIN_1, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // UART_RXD
	BUILD_GPIO ( GPIO17, 		GPIO_PORT_B, 	GPIO_PIN_2, 	GPIO_INPUT 			) // GPIO17	GPIO_00
	BUILD_GPIO ( GPIO18, 		GPIO_PORT_B, 	GPIO_PIN_3, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO18	GPIO_01
	BUILD_GPIO ( GPIO27, 		GPIO_PORT_B, 	GPIO_PIN_4, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO27	GPIO_02
// 	BUILD_GPIO ( GND,		GPIO_PORT_B, 	GPIO_PIN_5, 	UNUSEABLE			) // GND
	BUILD_GPIO ( GPIO22,		GPIO_PORT_B, 	GPIO_PIN_6, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO22	GPIO_03
	BUILD_GPIO ( GPIO23, 		GPIO_PORT_B, 	GPIO_PIN_7, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO23	GPIO_04

//----------------------------------------------------------------------------------------------------------------------------------
// PORT C
//---------  Name , 		Port, 		Bit,		Idle-State						wiring-Pi

// 	BUILD_GPIO ( 3V3,		GPIO_PORT_C, 	GPIO_PIN_0, 	UNUSEABLE			) // 3V3
	BUILD_GPIO ( GPIO24, 		GPIO_PORT_C, 	GPIO_PIN_1, 	GPIO_INPUT | GPIO_IDLE_HIGH 	) // GPIO24	GPIO_05
	BUILD_GPIO ( SPI_MOSI,		GPIO_PORT_C, 	GPIO_PIN_2, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // SPI_MOSI
// 	BUILD_GPIO ( GND,		GPIO_PORT_C, 	GPIO_PIN_3, 	UNUSEABLE			) // GND
	BUILD_GPIO ( SPI_MISO,		GPIO_PORT_C, 	GPIO_PIN_4, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // SPI_MISO
	BUILD_GPIO ( GPIO25, 		GPIO_PORT_C, 	GPIO_PIN_5, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO25	GPIO_06
	BUILD_GPIO ( SPI_SCLK,	 	GPIO_PORT_C, 	GPIO_PIN_6, 	GPIO_INPUT | GPIO_IDLE_HIGH 	) // SPI_SCLK
	BUILD_GPIO ( SPI_CE0,		GPIO_PORT_C, 	GPIO_PIN_7, 	GPIO_INPUT | GPIO_IDLE_HIGH 	) // SPI_CE0

//----------------------------------------------------------------------------------------------------------------------------------
// PORT D
//---------  Name , 		Port, 		Bit,		Idle-State						wiring-Pi

// 	BUILD_GPIO ( GND,		GPIO_PORT_D, 	GPIO_PIN_0, 	UNUSEABLE			) // GND
	BUILD_GPIO ( SPI_CE1,	 	GPIO_PORT_D, 	GPIO_PIN_1, 	GPIO_INPUT | GPIO_IDLE_HIGH 	) // SPI_CE1
//	BUILD_GPIO ( ID_SD,	 	GPIO_PORT_D, 	GPIO_PIN_2, 	UNUSEABLE 			) // ID_SD
//	BUILD_GPIO ( ID_SC,	 	GPIO_PORT_D, 	GPIO_PIN_3, 	UNUSEABLE		 	) // ID_SC
	BUILD_GPIO ( GPIO05, 		GPIO_PORT_D, 	GPIO_PIN_4, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO05	GPIO_21
// 	BUILD_GPIO ( GND,		GPIO_PORT_D, 	GPIO_PIN_5, 	UNUSEABLE			) // GND
	BUILD_GPIO ( GPIO06, 		GPIO_PORT_D, 	GPIO_PIN_6, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO06	GPIO_22
	BUILD_GPIO ( GPIO12, 		GPIO_PORT_D, 	GPIO_PIN_7, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO12	GPIO_26

//----------------------------------------------------------------------------------------------------------------------------------
// PORT E
//---------  Name , 		Port, 		Bit,		Idle-State						wiring-Pi

	BUILD_GPIO ( GPIO13, 		GPIO_PORT_E, 	GPIO_PIN_0, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO13	GPIO_23
// 	BUILD_GPIO ( GND,		GPIO_PORT_E, 	GPIO_PIN_1, 	UNUSEABLE			) // GND
	BUILD_GPIO ( GPIO19, 		GPIO_PORT_E, 	GPIO_PIN_2, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO19	GPIO_24
	BUILD_GPIO ( GPIO16, 		GPIO_PORT_E, 	GPIO_PIN_3, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO16	GPIO_27
	BUILD_GPIO ( GPIO26, 		GPIO_PORT_E, 	GPIO_PIN_4, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO26	GPIO_25
	BUILD_GPIO ( GPIO20, 		GPIO_PORT_E, 	GPIO_PIN_5, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO20	GPIO_28
// 	BUILD_GPIO ( GND,		GPIO_PORT_E, 	GPIO_PIN_6, 	UNUSEABLE			) // GND
	BUILD_GPIO ( GPIO21, 		GPIO_PORT_E, 	GPIO_PIN_7, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO21	GPIO_29

//----------------------------------------------------------------------------------------------------------------------------------
// ALIAS - WiringPi
//---------  		Alias-Name , 	Existing-Name

	GPIO_ALIAS (	GPIO_07, 	GPIO04)

	GPIO_ALIAS (	GPIO_00, 	GPIO17 )
	GPIO_ALIAS (	GPIO_01, 	GPIO18 )
	GPIO_ALIAS (	GPIO_02, 	GPIO27 )
	GPIO_ALIAS (	GPIO_03, 	GPIO22 )
	GPIO_ALIAS (	GPIO_04, 	GPIO23 )

	GPIO_ALIAS (	GPIO_05, 	GPIO24 )
	GPIO_ALIAS (	GPIO_06, 	GPIO25 )

	GPIO_ALIAS ( 	GPIO_21, 	GPIO05 )
	GPIO_ALIAS ( 	GPIO_22, 	GPIO06 )
	GPIO_ALIAS ( 	GPIO_26, 	GPIO12 )

	GPIO_ALIAS ( 	GPIO_23, 	GPIO13 )
	GPIO_ALIAS ( 	GPIO_24, 	GPIO19 )
	GPIO_ALIAS ( 	GPIO_27, 	GPIO16 )
	GPIO_ALIAS ( 	GPIO_25, 	GPIO26 )
	GPIO_ALIAS ( 	GPIO_28, 	GPIO20 )
	GPIO_ALIAS ( 	GPIO_29, 	GPIO21 )

//----------------------------------------------------------------------------------------------------------------------------------
// ALIAS - ControlBoaard v2
//---------  		Alias-Name , 	Existing-Name

	GPIO_ALIAS ( 	REQUEST_CLIENT, GPIO17 )
	GPIO_ALIAS ( 	SPI0_CS, 	SPI_CE0 )

//----------------------------------------------------------------------------------------------------------------------------------
// ALIAS - LCD 16x2
//---------  		Alias-Name , 	Existing-Name

	GPIO_ALIAS ( 	LCD_RS, 	GPIO19 )
	GPIO_ALIAS ( 	LCD_EN, 	GPIO26 )
	GPIO_ALIAS ( 	LCD_D4, 	GPIO12 )
	GPIO_ALIAS ( 	LCD_D5, 	GPIO16 )
	GPIO_ALIAS ( 	LCD_D6, 	GPIO20 )
	GPIO_ALIAS ( 	LCD_D7, 	GPIO21 )
	