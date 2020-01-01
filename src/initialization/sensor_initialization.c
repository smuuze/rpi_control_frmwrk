/*! \file *********************************************************************

 *****************************************************************************/

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "tracer.h"

//-----------------------------------------------------------------------------

#include "system_interface.h"

#if defined (HAS_LIGHT_SENSOR_GM5528) && ((HAS_LIGHT_SENSOR_GM5528) == 1)
#include "driver/sensor/light_resistor_gm5528.h"
#endif

#include "expansion/local_ads1115_mcu_task.h"
#include "expansion/local_sht31_mcu_task.h"

//-----------------------------------------------------------------------------

void sensor_initialization(void) {

	DEBUG_PASS("sensor_initialization()");

	#ifdef HAS_EXPANSION_BOARD_SENSOR_SHT31_ADS1115
	{
		DEBUG_PASS("protocol_initialization() - local_ads1115_module_init()");
		local_ads1115_module_init(i_system.driver.i2c0);
		
		DEBUG_PASS("protocol_initialization() - local_sht31_module_init()");
		local_sht31_module_init(i_system.driver.i2c0);
	}
	#endif	

	#if defined (HAS_LIGHT_SENSOR_GM5528) && ((HAS_LIGHT_SENSOR_GM5528) == 1)
	{
		DEBUG_PASS("sensor_initialization() - light_resistor_gm5528_init()");
		light_resistor_gm5528_init();
	}
	#endif

}