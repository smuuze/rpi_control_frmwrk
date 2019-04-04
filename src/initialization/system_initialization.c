/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!

#include "local_clk_driver.h"
#include "local_rtc_driver.h"
#include "local_gpio_driver.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_OFF
#include "tracer.h"

//-----------------------------------------------------------------------------

void system_initialization(void) {

	local_clk_driver_init();

	#if defined HAS_DRIVER_SPI0 && HAS_DRIVER_SPI0 == 1
	i_system.driver.i2c0->initialize();
	#endif

	#if defined HAS_DRIVER_SPI0 && HAS_DRIVER_USART0 == 1
	i_system.driver.usart0->initialize();
	#endif

	#if defined HAS_DRIVER_SPI0 && HAS_DRIVER_I2C0 == 1
	i_system.driver.spi0->initialize();
	#endif

	TRACER_RESTART();

	local_rtc_timer_init();
	gpio_driver_init();
}

