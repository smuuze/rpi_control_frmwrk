 /*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!

//---------- Implementation of Traces -----------------------------------------

#define TRACER_OFF
#include "tracer.h"

//-----------------------------------------------------------------------------

#include "common/signal_slot_interface.h"

//-----------------------------------------------------------------------------

#ifdef HAS_SENSOR_UNIT_GM5528
#include "expansion/light_resistor_gm5528.h"
SLOT_CREATE(ADS1115_NEW_VALUES_SIGNAL, LIGHT_RESISTOR_GM5528_SLOT, light_resistor_gm5528_callback)
#endif

//-----------------------------------------------------------------------------

void signal_slot_initialization(void) {

	#ifdef HAS_SENSOR_UNIT_GM5528
	{
		light_resistor_gm5528_init();
		LIGHT_RESISTOR_GM5528_SLOT_connect();
	}
	#endif

}