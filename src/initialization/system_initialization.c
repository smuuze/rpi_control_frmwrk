/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	system_initialization.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_ON

// --------------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!

// --------------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "driver/clock/clock_driver_interface.h"
#include "driver/rtc/rtc_driver_interface.h"
#include "driver/gpio/gpio_interface.h"
#include "copro/copro_interface.h"

//-----------------------------------------------------------------------------

void system_initialization(void) {

	#ifdef HAS_DRIVER_GPIO
	{
		gpio_driver_init();
	}
	#endif

	clock_driver_init();
	rtc_driver_init();

	#if defined HAS_DRIVER_SPI0 && HAS_DRIVER_SPI0 == 1
	{
		//PASS(); // system_initialization() - Initializing SPI0
		i_system.driver.spi0->initialize();
	}
	#endif

	#if defined HAS_DRIVER_USART0 && HAS_DRIVER_USART0 == 1
	{
		//PASS(); // system_initialization() - Initializing USART0
		i_system.driver.usart0->initialize();
	}
	#endif

	#if defined HAS_DRIVER_USART1 && HAS_DRIVER_USART1 == 1
	{
		//PASS(); // system_initialization() - Initializing USART0
		i_system.driver.usart1->initialize();
	}
	#endif

	#if defined HAS_DRIVER_I2C0 && HAS_DRIVER_I2C0 == 1
	{
		//PASS(); // system_initialization() - Initializing I2C0
		i_system.driver.i2c0->initialize();
	}
	#endif

	TRACER_RESTART();

	#ifdef COPRO_INTERFACE_AVAILABLE
	{
		copro_interface_initialize();
	}
	#endif
}
