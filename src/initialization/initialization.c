/**
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * @file   initialization.c
 * @author Sebastian Lesse
 * @date   2018 / 11 / 16
 * @brief  Initialization routine
 * 
 */

#define TRACER_OFF

//-----------------------------------------------------------------------------

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

#include "3rdparty/ir_protocol/ir_protocol_nec.h"
#include "3rdparty/ir_protocol/ir_protocol_sony.h"

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

    #ifdef HAS_IR_PROTOCOL_NEC
    {
        ir_protocol_nec_init();
    }
    #endif

    #ifdef HAS_IR_PROTOCOL_SONY
    {
        ir_protocol_sony_init();
    }
    #endif
}

//-----------------------------------------------------------------------------

void deinitialization(void) {
    system_deinitialization();
}

//-----------------------------------------------------------------------------
