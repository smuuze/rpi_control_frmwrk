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
 * @file   cli_executer_task.h
 * @author Sebastian Lesse
 * @date   2022 / 02 / 20
 * @brief  Processes cli user commands.
 * 
 * Input signals:
 * 
 * - CLI_EXECUTER_COMMAND_RECEIVED_SIGNAL - command to execute as string
 * 
 * Output signals:
 * 
 * - CLI_EXECUTER_COMMAND_RESPONSE_SIGNAL - response from command as string
 * - CLI_EXECUTER_COMMAND_TIMEOUT_SIGNAL - Command has timed out with command string as argument
 * - CLI_EXECUTER_COMMAND_NOT_FOUND_SIGNAL - command not found with command string as argument
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_cli_executer_task_
#define _H_cli_executer_task_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

/**
 * @brief Maximum length of the command to execute
 * 
 */
#ifndef CLI_EXECUTER_MAX_MESSAGE_LENGTH
#define CLI_EXECUTER_MAX_MESSAGE_LENGTH             255
#endif

/**
 * @brief The default scheduling interval of the cli-executer module
 * is 30 milliseconds
 * 
 */
#ifndef CLI_EXECUTER_TASK_SCHEDULE_INTERVAL_MS
#define CLI_EXECUTER_TASK_SCHEDULE_INTERVAL_MS      30
#endif

/**
 * @brief Maximum time to process a command
 * 
 */
#ifndef CLI_EXECUTER_EXECUTION_TIMEOUT_MS
#define CLI_EXECUTER_EXECUTION_TIMEOUT_MS           250
#endif

// --------------------------------------------------------------------------------

/**
 * @brief Initializes the cli-exution module.
 * This function register the cli-executer task to the mcu-task-controller
 * and initializes the following signals:
 * 
 * - CLI_EXECUTER_COMMAND_RECEIVED_SIGNAL
 * - CLI_EXECUTER_COMMAND_RESPONSE_SIGNAL
 * - CLI_EXECUTER_COMMAND_TIMEOUT_SIGNAL
 * - CLI_EXECUTER_COMMAND_NOT_FOUND_SIGNAL
 * 
 * The following slots will be connected within this function:
 * 
 * - CLI_EXECUTER_COMMAND_RECEIVED_SLOT
 * 
 */
void cli_executer_init(void);

// --------------------------------------------------------------------------------

/**
 * @brief The cli exeucter module will receive
 * new commands via this signal.
 * @argument (const char*) command to execute as string.
 * Leading and trailing whitespcaes will be removed.
 * Maximum length is CLI_EXECUTER_MAX_MESSAGE_LENGTH
 * 
 */
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CLI_EXECUTER_COMMAND_RECEIVED_SIGNAL)

// --------------------------------------------------------------------------------

#endif // _H_cli_executer_task_

// --------------------------------------------------------------------------------
