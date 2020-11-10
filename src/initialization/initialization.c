/*! \file *********************************************************************

 *****************************************************************************/

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "tracer.h"

//-----------------------------------------------------------------------------

#include "cpu.h"

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
#include "ui/command_line/command_line_interface.h"
#include "ui/cfg_file_parser/cfg_file_parser.h"

//-----------------------------------------------------------------------------

SYSTEM_T system_context;

//-----------------------------------------------------------------------------

void initialization(void) {

	watchdog_enable();
	watchdog();

	system_initialization();

	power_initialization();

	sensor_initialization();

	button_initialization();

	output_initialization();

	task_initialization();

	protocol_initialization();

	command_initialization();

	#ifdef CLI_AVAILABLE
	{
		command_line_interface_init();
	}
	#endif

	#ifdef CFG_FILE_PARSER_AVAILABLE
	{
		cfg_file_parser_init();
	}
	#endif
}

