/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	app_tasks/message_executer_task.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

// --------------------------------------------------------------------------------

#ifndef _MESSAGE_EXECUTER_TASK_H_
#define _MESSAGE_EXECUTER_TASK_H_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#ifndef COMMAND_FILE_PATH_CFG_NAME
#define COMMAND_FILE_PATH_CFG_NAME					"COMMAND_FILE_PATH"
#endif

#ifndef REPORT_FILE_PATH_CFG_NAME
#define REPORT_FILE_PATH_CFG_NAME					"REPORT_FILE_PATH"
#endif

#ifndef REPORT_INTERVAL_CFG_NAME
#define REPORT_INTERVAL_CFG_NAME					"SCHEDULE_INTERVAL_REPORT_MS"
#endif

// --------------------------------------------------------------------------------

void msg_executer_init(void);

// --------------------------------------------------------------------------------

#ifndef MSG_EXECUTER_TASK_SCHEDULE_INTERVAL_MS
#define MSG_EXECUTER_TASK_SCHEDULE_INTERVAL_MS				30
#endif

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(MSG_EXECUTER_FILE_OPEN_FAILED_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(MSG_EXECUTER_RESPONSE_TIMEOUT_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(MSG_EXECUTER_RESPONSE_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(MSG_EXECUTER_EXECUTION_SUCCEEDED_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(MSG_EXECUTER_EXECUTION_FAILED_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(MSG_EXECUTER_INVALID_COMMAND_SIGNAL)

// --------------------------------------------------------------------------------

#endif // _MESSAGE_EXECUTER_TASK_H_
