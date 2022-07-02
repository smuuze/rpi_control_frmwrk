/**
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * @file    system_initialization.c
 * @author  Sebastian Lesse
 * @date    2022 / 06 / 05
 * @brief   Short description of this file
 * 
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "cpu.h"

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

//-----------------------------------------------------------------------------

void system_deinitialization(void) {

	#ifdef HAS_DRIVER_GPIO
	{
		gpio_driver_deinit();
	}
	#endif

}

//-----------------------------------------------------------------------------
