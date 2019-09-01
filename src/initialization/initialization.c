/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "local_context.h"
#include "initialization/system_initialization.h"
#include "initialization/power_initialization.h"
#include "initialization/button_initialization.h"
#include "initialization/output_initialization.h"
#include "initialization/protocol_initialization.h"
#include "initialization/command_initialization.h"
#include "initialization/task_initialization.h"
#include "initialization/signal_slot_initialization.h"

SYSTEM_T system_context;

void initialization(void) {

	wdt_enable(WDTO_8S);
	watchdog();

	system_initialization();

	power_initialization();

	button_initialization();

	output_initialization();

	protocol_initialization();

	command_initialization();

	task_initialization();

	signal_slot_initialization();
}

