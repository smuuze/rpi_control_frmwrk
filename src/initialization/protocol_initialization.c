/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "rpi_protocol_handler.h"
#include "local_ads1115_mcu_task.h"
#include "local_sht31_mcu_task.h"

#include "system_interface.h"


#if defined (HOST_INTERFACE_SPI) && (HOST_INTERFACE_SPI) == 1
#define RPI_PROTOCOLL_COM_INTERFACE		i_system.driver.spi0
#elif defined (HOST_INTERFACE_USART) && (HOST_INTERFACE_USART) == 1
#define RPI_PROTOCOLL_COM_INTERFACE		i_system.driver.usart0
#endif


void protocol_initialization(void) {

	rpi_protocol_init(RPI_PROTOCOLL_COM_INTERFACE);

	local_ads1115_module_init(i_system.driver.i2c0);
	local_sht31_module_init(i_system.driver.i2c0);
}

