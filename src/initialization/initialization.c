/*! \file *********************************************************************

 *****************************************************************************/

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "tracer.h"

//-----------------------------------------------------------------------------

#include "local_context.h"
#include "initialization/system_initialization.h"
#include "initialization/power_initialization.h"
#include "initialization/button_initialization.h"
#include "initialization/output_initialization.h"
#include "initialization/protocol_initialization.h"
#include "initialization/command_initialization.h"
#include "initialization/task_initialization.h"
#include "initialization/sensor_initialization.h"

//-----------------------------------------------------------------------------

SYSTEM_T system_context;

//-----------------------------------------------------------------------------

void initialization(void) {

	#ifdef config_WATCHDOG_TIMEOUT_VALUE
	{
		wdt_enable(config_WATCHDOG_TIMEOUT_VALUE);
		watchdog();
	}
	#endif

	system_initialization();

	power_initialization();

	sensor_initialization();

	button_initialization();

	output_initialization();

	protocol_initialization();

	command_initialization();

	task_initialization();
}

