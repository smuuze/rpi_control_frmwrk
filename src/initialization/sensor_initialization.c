 /*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!

//---------- Implementation of Traces -----------------------------------------

#define TRACER_OFF
#include "tracer.h"

//-----------------------------------------------------------------------------

#if defined (HAS_LIGHT_SENSOR_GM5528) && ((HAS_LIGHT_SENSOR_GM5528) == 1)
#include "driver/sensor/light_resistor_gm5528.h"
#endif

//-----------------------------------------------------------------------------

void sensor_initialization(void) {

	#if defined (HAS_LIGHT_SENSOR_GM5528) && ((HAS_LIGHT_SENSOR_GM5528) == 1)
	{
		light_resistor_gm5528_init();
	}
	#endif

}