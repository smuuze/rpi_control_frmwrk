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
#endif

//-----------------------------------------------------------------------------

void signal_slot_initialization(void) {

	#ifdef HAS_SENSOR_UNIT_GM5528
	{
		light_resistor_gm5528_init();
	}
	#endif

}