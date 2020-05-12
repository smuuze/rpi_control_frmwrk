/*! \file *********************************************************************

 *****************************************************************************/

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "tracer.h"

//-----------------------------------------------------------------------------

#include "protocol_management/rpi_protocol_handler.h"
#include "system_interface.h"

//-----------------------------------------------------------------------------

void protocol_initialization(void) {

	DEBUG_PASS("protocol_initialization()");
	
	#if defined (HOST_INTERFACE_SPI) && (HOST_INTERFACE_SPI) == 1
	{
		DEBUG_PASS("protocol_initialization() - Set Hostinterface to SPI0");
		rpi_protocol_init(i_system.driver.spi0);
	}
	#elif defined (HOST_INTERFACE_USART) && (HOST_INTERFACE_USART) == 1
	{
		DEBUG_PASS("protocol_initialization() - Set Hostinterface to USART0");
		rpi_protocol_init(i_system.driver.usart0);
	}
	#elif defined (HOST_INTERFACE_I2C) && (HOST_INTERFACE_I2C) == 1
	{
		DEBUG_PASS("protocol_initialization() - Set Hostinterface to I2C");
		rpi_protocol_init(i_system.driver.i2c0);
	}
	#endif
}

