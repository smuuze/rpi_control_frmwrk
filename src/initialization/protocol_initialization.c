/*! \file *********************************************************************

 *****************************************************************************/

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "tracer.h"

//-----------------------------------------------------------------------------

#include "protocol_management/rpi_protocol_handler.h"
#include "expansion/local_ads1115_mcu_task.h"
#include "expansion/local_sht31_mcu_task.h"

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
	#endif

	#ifdef HAS_EXPANSION_BOARD_SENSOR_SHT31_ADS1115
	{
		DEBUG_PASS("protocol_initialization() - local_ads1115_module_init()");
		local_ads1115_module_init(i_system.driver.i2c0);
		
		DEBUG_PASS("protocol_initialization() - local_sht31_module_init()");
		local_sht31_module_init(i_system.driver.i2c0);
	}
	#endif	
}

