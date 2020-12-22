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
#include "ui/log_interface/log_interface.h"

#include "app_tasks/message_executer_task.h"
#include "app_tasks/cli_executer_task.h"

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

	#ifdef LOG_INTERFACE_AVAILABLE
	{
		log_interface_init();
	}
	#endif

	#ifdef HAS_APP_TASK_MSG_EXECUTER
	{
		msg_executer_init();
	}
	#endif

	#ifdef HAS_APP_TASK_CLI_EXECUTER
	{
		cli_executer_init();
	}
	#endif
}
