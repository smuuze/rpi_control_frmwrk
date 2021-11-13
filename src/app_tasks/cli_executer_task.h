/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	app_tasks/cli_executer_task.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

// --------------------------------------------------------------------------------

#ifndef _CLI_EXECUTER_TASK_H_
#define _CLI_EXECUTER_TASK_H_

// --------------------------------------------------------------------------------

#include "config.h"

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
 * @brief The default scheduling interval of the cli-executer module
 * is 30 milliseconds
 * 
 */
#ifndef CLI_EXECUTER_TASK_SCHEDULE_INTERVAL_MS
#define CLI_EXECUTER_TASK_SCHEDULE_INTERVAL_MS				30
#endif

// --------------------------------------------------------------------------------

/**
 * @brief The cli exeucter module will receive will receive
 * new commands via this signal.
 * 
 */
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CLI_EXECUTER_COMMAND_RECEIVED_SIGNAL)

// --------------------------------------------------------------------------------

#endif // _CLI_EXECUTER_TASK_H_
