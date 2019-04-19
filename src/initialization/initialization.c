/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "local_context.h"
#include "system_initialization.h"
#include "button_initialization.h"
#include "output_initialization.h"
#include "protocol_initialization.h"
#include "command_initialization.h"
#include "task_initialization.h"

SYSTEM_T system_context;

void initialization(void) {

	system_initialization();

	button_initialization();
	output_initialization();

	protocol_initialization();
	command_initialization();

	task_initialization();
}

