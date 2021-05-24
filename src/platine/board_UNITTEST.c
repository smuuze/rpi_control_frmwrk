
#include "platine/board_common_config.h"

// --------------------------------------------------------------------------------


//----------------------------------------------------------------------------------------------------------------------------------
// PORT A
//---------  Name , 		Port, 		Bit,		Idle-State						wiring-Pi

// 	FAKE_GPIO ( 3V3,		GPIO_PORT_A, 	GPIO_PIN_0, 	UNUSEABLE			) // 3V3
// 	FAKE_GPIO ( 5V,			GPIO_PORT_A, 	GPIO_PIN_1, 	UNUSEABLE			) // 5V
	FAKE_GPIO ( I2C_SDA,		GPIO_PORT_A, 	GPIO_PIN_2, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // I2C_SDA
// 	FAKE_GPIO ( 5V,		G	PIO_PORT_A, 	GPIO_PIN_3, 	UNUSEABLE			) // 5V
	FAKE_GPIO ( I2C_SCL,		GPIO_PORT_A, 	GPIO_PIN_4, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // I2C_SCL
// 	FAKE_GPIO ( GND,		GPIO_PORT_A, 	GPIO_PIN_5, 	UNUSEABLE			) // GND
	FAKE_GPIO ( GPIO04, 		GPIO_PORT_A, 	GPIO_PIN_6, 	GPIO_INPUT | GPIO_IDLE_HIGH 	) // GPIO04	GPIO_07
	FAKE_GPIO ( UART_TXD,		GPIO_PORT_A, 	GPIO_PIN_7, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // UART_TXD

//----------------------------------------------------------------------------------------------------------------------------------
// PORT B
//---------  Name , 		Port, 		Bit,		Idle-State						wiring-Pi
//									
// 	FAKE_GPIO ( GND,		GPIO_PORT_B, 	GPIO_PIN_0, 	UNUSEABLE			) // GND
	FAKE_GPIO ( UART_RXD,		GPIO_PORT_B, 	GPIO_PIN_1, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // UART_RXD
	FAKE_GPIO ( GPIO17, 		GPIO_PORT_B, 	GPIO_PIN_2, 	GPIO_INPUT 			) // GPIO17	GPIO_00
	FAKE_GPIO ( GPIO18, 		GPIO_PORT_B, 	GPIO_PIN_3, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO18	GPIO_01
	FAKE_GPIO ( GPIO27, 		GPIO_PORT_B, 	GPIO_PIN_4, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO27	GPIO_02
// 	FAKE_GPIO ( GND,		GPIO_PORT_B, 	GPIO_PIN_5, 	UNUSEABLE			) // GND
	FAKE_GPIO ( GPIO22,		GPIO_PORT_B, 	GPIO_PIN_6, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO22	GPIO_03
	FAKE_GPIO ( GPIO23, 		GPIO_PORT_B, 	GPIO_PIN_7, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO23	GPIO_04

//----------------------------------------------------------------------------------------------------------------------------------
// PORT C
//---------  Name , 		Port, 		Bit,		Idle-State						wiring-Pi

// 	FAKE_GPIO ( 3V3,		GPIO_PORT_C, 	GPIO_PIN_0, 	UNUSEABLE			) // 3V3
	FAKE_GPIO ( GPIO24, 		GPIO_PORT_C, 	GPIO_PIN_1, 	GPIO_INPUT | GPIO_IDLE_HIGH 	) // GPIO24	GPIO_05
	FAKE_GPIO ( SPI_MOSI,		GPIO_PORT_C, 	GPIO_PIN_2, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // SPI_MOSI
// 	FAKE_GPIO ( GND,		GPIO_PORT_C, 	GPIO_PIN_3, 	UNUSEABLE			) // GND
	FAKE_GPIO ( SPI_MISO,		GPIO_PORT_C, 	GPIO_PIN_4, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // SPI_MISO
	FAKE_GPIO ( GPIO25, 		GPIO_PORT_C, 	GPIO_PIN_5, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO25	GPIO_06
	FAKE_GPIO ( SPI_SCLK,	 	GPIO_PORT_C, 	GPIO_PIN_6, 	GPIO_INPUT | GPIO_IDLE_HIGH 	) // SPI_SCLK
	FAKE_GPIO ( SPI_CE0,		GPIO_PORT_C, 	GPIO_PIN_7, 	GPIO_INPUT | GPIO_IDLE_HIGH 	) // SPI_CE0

//----------------------------------------------------------------------------------------------------------------------------------
// PORT D
//---------  Name , 		Port, 		Bit,		Idle-State						wiring-Pi

// 	FAKE_GPIO ( GND,		GPIO_PORT_D, 	GPIO_PIN_0, 	UNUSEABLE			) // GND
	FAKE_GPIO ( SPI_CE1,	 	GPIO_PORT_D, 	GPIO_PIN_1, 	GPIO_INPUT | GPIO_IDLE_HIGH 	) // SPI_CE1
//	FAKE_GPIO ( ID_SD,	 	GPIO_PORT_D, 	GPIO_PIN_2, 	UNUSEABLE 			) // ID_SD
//	FAKE_GPIO ( ID_SC,	 	GPIO_PORT_D, 	GPIO_PIN_3, 	UNUSEABLE		 	) // ID_SC
	FAKE_GPIO ( GPIO05, 		GPIO_PORT_D, 	GPIO_PIN_4, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO05	GPIO_21
// 	FAKE_GPIO ( GND,		GPIO_PORT_D, 	GPIO_PIN_5, 	UNUSEABLE			) // GND
	FAKE_GPIO ( GPIO06, 		GPIO_PORT_D, 	GPIO_PIN_6, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO06	GPIO_22
	FAKE_GPIO ( GPIO12, 		GPIO_PORT_D, 	GPIO_PIN_7, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO12	GPIO_26

//----------------------------------------------------------------------------------------------------------------------------------
// PORT E
//---------  Name , 		Port, 		Bit,		Idle-State						wiring-Pi

//	FAKE_GPIO ( GPIO13, 		GPIO_PORT_E, 	GPIO_PIN_0, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO13	GPIO_23
// 	FAKE_GPIO ( GND,		GPIO_PORT_E, 	GPIO_PIN_1, 	UNUSEABLE			) // GND
	FAKE_GPIO ( GPIO19, 		GPIO_PORT_E, 	GPIO_PIN_2, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO19	GPIO_24
	FAKE_GPIO ( GPIO16, 		GPIO_PORT_E, 	GPIO_PIN_3, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO16	GPIO_27
	FAKE_GPIO ( GPIO26, 		GPIO_PORT_E, 	GPIO_PIN_4, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO26	GPIO_25
	FAKE_GPIO ( GPIO20, 		GPIO_PORT_E, 	GPIO_PIN_5, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO20	GPIO_28
// 	FAKE_GPIO ( GND,		GPIO_PORT_E, 	GPIO_PIN_6, 	UNUSEABLE			) // GND
	FAKE_GPIO ( GPIO21, 		GPIO_PORT_E, 	GPIO_PIN_7, 	GPIO_INPUT | GPIO_IDLE_HIGH	) // GPIO21	GPIO_29

FAKE_GPIO ( READY_INOUT,		GPIO_PORT_A, 	GPIO_PIN_1, 	GPIO_INPUT | GPIO_IDLE_HIGH	)
FAKE_GPIO ( REQUEST_CLIENT,		GPIO_PORT_A, 	GPIO_PIN_0, 	GPIO_INPUT | GPIO_IDLE_HIGH	)

FAKE_GPIO ( IR_CARRIER_IN,		GPIO_PORT_E, 	GPIO_PIN_0, 	GPIO_INPUT | GPIO_IDLE_HIGH	)
FAKE_GPIO ( IR_CARRIER_OUT,		GPIO_PORT_E, 	GPIO_PIN_1, 	GPIO_INPUT | GPIO_IDLE_HIGH	)

GPIO_ALIAS ( 	LCD_RS, 	GPIO19 )
GPIO_ALIAS ( 	LCD_EN, 	GPIO26 )
GPIO_ALIAS ( 	LCD_D4, 	GPIO12 )
GPIO_ALIAS ( 	LCD_D5, 	GPIO16 )
GPIO_ALIAS ( 	LCD_D6, 	GPIO20 )
GPIO_ALIAS ( 	LCD_D7, 	GPIO21 )
