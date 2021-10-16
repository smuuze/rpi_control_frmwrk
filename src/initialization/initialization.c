/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	initialization.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

#ifdef TRACER_ON
#pragma __WARNING__TRACES_ENABLED__
#endif

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!

//-----------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "cpu.h"

//-----------------------------------------------------------------------------

#include "local_context.h"

#include "initialization/system_initialization.h"
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

#include "power_management/power_management_interface.h"

//-----------------------------------------------------------------------------

SYSTEM_T system_context;

//-----------------------------------------------------------------------------

void initialization(void) {

	watchdog_enable();
	watchdog();

	system_initialization();

	task_initialization();

	#ifdef HAS_POWER_MANAGEMENT_MODULE
	{
		power_management_initialization();
	}
	#endif

	sensor_initialization();

	button_initialization();

	output_initialization();

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
