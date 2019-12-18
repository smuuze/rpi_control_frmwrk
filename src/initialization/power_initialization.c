 /*! \file *********************************************************************

 *****************************************************************************/

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

//-----------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "power_management/power_management.h"
#include "io_management/io_controller.h"

//-----------------------------------------------------------------------------

#define POWER_UNIT_5V_POWER_UP_TIME_MS		20

//-----------------------------------------------------------------------------

#ifdef HAS_GPIO_EXT_POWER_01
POWER_MGMN_BUILD_UNIT(POWER_UNIT_5V, POWER_UNIT_5V_POWER_UP_TIME_MS, EXT_POWER_01_drive_high, EXT_POWER_01_drive_low)
#endif

//-----------------------------------------------------------------------------

void power_initialization(void) {

	#ifdef HAS_GPIO_EXT_POWER_01
	{		
		POWER_UNIT_5V_init();
	}
	#endif
}