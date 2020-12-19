#ifndef CFG_FILE_PARSER_H_
#define CFG_FILE_PARSER_H_

/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	ui/cfg_file_parser/cfg_file_parser.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "mcu_task_management/mcu_task_interface.h"
#include "common/signal_slot_interface.h"

// --------------------------------------------------------------------------------

#ifndef CFG_FILE_PARSER_MAX_LENGTH_PATH
#define CFG_FILE_PARSER_MAX_LENGTH_PATH				255
#endif

#ifndef CFG_FILE_PARSER_TASK_RUN_INTERVAL_MS
#define CFG_FILE_PARSER_TASK_RUN_INTERVAL_MS			1000
#endif

// --------------------------------------------------------------------------------

/*
 *
 */
typedef struct {
	char key[CFG_FILE_PARSER_MAX_LENGTH_PATH];
	char value[CFG_FILE_PARSER_MAX_LENGTH_PATH];
} CFG_FILE_PARSER_CFG_OBJECT_TYPE;

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CFG_PARSER_CFG_FILE_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CFG_PARSER_NEW_CFG_OBJECT_SIGNAL)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CFG_PARSER_CFG_COMPLETE_SIGNAL)

// --------------------------------------------------------------------------------

/*
 *
 */
void cfg_file_parser_init(void);

// --------------------------------------------------------------------------------

/*
 *
 */
u16 cfg_file_parser_task_get_schedule_interval(void);

/*
 *
 */
void cfg_file_parser_task_init(void);

/*
 *
 */
MCU_TASK_INTERFACE_TASK_STATE cfg_file_parser_task_get_state(void);

/*
 *
 */
void cfg_file_parser_task_run(void);

/*
 *
 */
void cfg_file_parser_task_background_run(void);

/*
 *
 */
void cfg_file_parser_task_sleep(void);

/*
 *
 */
void cfg_file_parser_task_wakeup(void);

/*
 *
 */
void cfg_file_parser_task_finish(void);

/*
 *
 */
void cfg_file_parser_task_terminate(void);

#endif // CFG_FILE_PARSER_H_