
#ifndef _BOARD_RASPBERRYPI_H_
#define _BOARD_RASPBERRYPI_H_

#include "config.h"
#include "driver/gpio/gpio_interface.h"

//----------------------------------------------------------------------------------------------------------------------------------
// PORT A
//---------

	INCLUDE_GPIO ( I2C_SDA ) // I2C_SDA
	INCLUDE_GPIO ( I2C_SCL ) // I2C_SCL
	INCLUDE_GPIO ( GPIO04 ) // GPIO04
	INCLUDE_GPIO ( UART_TXD ) // UART_TXD

//----------------------------------------------------------------------------------------------------------------------------------
// PORT B
//---------

	INCLUDE_GPIO ( UART_RXD ) // UART_RXD
	INCLUDE_GPIO ( GPIO17 ) // GPIO17
	INCLUDE_GPIO ( GPIO18 ) // GPIO18
	INCLUDE_GPIO ( GPIO27 ) // GPIO27
	INCLUDE_GPIO ( GPIO22 ) // GPIO22
	INCLUDE_GPIO ( GPIO23 ) // GPIO23

//----------------------------------------------------------------------------------------------------------------------------------
// PORT C
//---------

	INCLUDE_GPIO ( GPIO24 ) // GPIO24
	INCLUDE_GPIO ( SPI_MOSI ) // SPI_MOSI
	INCLUDE_GPIO ( SPI_MISO ) // SPI_MISO
	INCLUDE_GPIO ( GPIO25 ) // GPIO25
	INCLUDE_GPIO ( SPI_SCLK ) // SPI_SCLK
	INCLUDE_GPIO ( SPI_CE0 ) // SPI_CE0

//----------------------------------------------------------------------------------------------------------------------------------
// PORT D
//---------

	INCLUDE_GPIO ( SPI_CE1 ) // SPI_CE1
	INCLUDE_GPIO ( GPIO05 ) // GPIO05
	INCLUDE_GPIO ( GPIO06 ) // GPIO06
	INCLUDE_GPIO ( GPIO12 ) // GPIO12

//----------------------------------------------------------------------------------------------------------------------------------
// PORT E
//---------

	INCLUDE_GPIO ( GPIO13 ) // GPIO13
	INCLUDE_GPIO ( GPIO19 ) // GPIO19
	INCLUDE_GPIO ( GPIO16 ) // GPIO16
	INCLUDE_GPIO ( GPIO26 ) // GPIO26
	INCLUDE_GPIO ( GPIO20 ) // GPIO20
	INCLUDE_GPIO ( GPIO21 ) // GPIO21

//----------------------------------------------------------------------------------------------------------------------------------
// ALIAS - ControlBoaard v2
//---------

	INCLUDE_GPIO_ALIAS ( REQUEST_CLIENT ) // REQUEST_CLIENT	
	INCLUDE_GPIO_ALIAS ( SPI0_CS )

//----------------------------------------------------------------------------------------------------------------------------------
// ALIAS - LCD 16x2
//---------

	INCLUDE_GPIO ( 	LCD_RS )
	INCLUDE_GPIO ( 	LCD_EN )
	INCLUDE_GPIO ( 	LCD_D4 )
	INCLUDE_GPIO ( 	LCD_D5 )
	INCLUDE_GPIO ( 	LCD_D6 )
	INCLUDE_GPIO ( 	LCD_D7 )

#endif // _BOARD_RASPBERRYPI_H_
