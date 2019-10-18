/*! \file *********************************************************************

 *****************************************************************************/

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "tracer.h"

//-----------------------------------------------------------------------------

#include "local_clk_driver.h"
#include "local_rtc_driver.h"
#include "driver/gpio/gpio_interface.h"

//-----------------------------------------------------------------------------

void system_initialization(void) {

	gpio_driver_init();
	local_clk_driver_init();
	local_rtc_timer_init();

	#if defined HAS_DRIVER_SPI0 && HAS_DRIVER_SPI0 == 1
	{
		//PASS(); // system_initialization() - Initializing SPI0
		i_system.driver.i2c0->initialize();
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
		i_system.driver.spi0->initialize();
	}
	#endif

	TRACER_RESTART();
}

