 /*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!

//---------- Implementation of Traces -----------------------------------------

#define TRACER_OFF
#include "tracer.h"

//-----------------------------------------------------------------------------

#include "power_management/power_management.h"
#include "io_management/io_controller.h"

//-----------------------------------------------------------------------------

#define POWER_UNIT_5V_POWER_UP_TIME_MS		20

//-----------------------------------------------------------------------------

IO_CONTROLLER_BUILD_INOUT(EXT_POWER_5V, EXT_POWER_01)
POWER_MGMN_BUILD_UNIT(POWER_UNIT_5V, POWER_UNIT_5V_POWER_UP_TIME_MS, EXT_POWER_5V_drive_high, EXT_POWER_5V_drive_low)

//-----------------------------------------------------------------------------

void power_initialization(void) {

	POWER_UNIT_5V_init();
}