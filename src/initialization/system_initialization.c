/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!

#include "local_clk_driver.h"
#include "local_rtc_driver.h"
#include "driver/gpio/gpio_interface.h"

#include "io_management/io_controller.h"
#include "power_management/power_management.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_OFF
#include "tracer.h"

//-----------------------------------------------------------------------------

IO_CONTROLLER_BUILD_INOUT(EXPANSION_BOARD_POWER_PIN, EXT_POWER_01)
POWER_MGMN_BUILD_UNIT(EXPANSION_BOARD_POWER, 10, &EXPANSION_BOARD_POWER_PIN_drive_high, &EXPANSION_BOARD_POWER_PIN_drive_low)

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
	
	EXPANSION_BOARD_POWER_PIN_init();
	EXPANSION_BOARD_POWER_init();
}

