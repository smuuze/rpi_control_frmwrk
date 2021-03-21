/**
 * --------------------------------------------------------------------------------
 *
 * @file 	ir_remote_mcu_task.h
 * @author 	sebastian lesse (sebastian lesse)
 * @brief 
 * @version 	1.0
 * @date 	2021-03-21
 * 
 * @copyright 	Copyright (c) 2021
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _IR_REMOTE_MCU_TASK_H_
#define _IR_REMOTE_MCU_TASK_H_

// --------------------------------------------------------------------------------

#include "mcu_task_interface.h"

// --------------------------------------------------------------------------------

/**
 * @brief 	Initializes the IR-Remote Task-Module
 * 		and registers the task to the MCU Task-Controller
 * 
 */
void ir_remote_app_task_init(void);

// --------------------------------------------------------------------------------

#endif // _IR_REMOTE_MCU_TASK_H_
