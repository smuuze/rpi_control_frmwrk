/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	ir_remote_mcu_task.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _IR_REMOTE_MCU_TASK_H_
#define _IR_REMOTE_MCU_TASK_H_

#include "mcu_task_interface.h"

/*!
 *
 */
void ir_remote_task_init(void);

/*!
 *
 */
u16 ir_remote_task_get_schedule_interval(void);

/*!
 *
 */
MCU_TASK_INTERFACE_TASK_STATE ir_remote_task_get_state(void);

/*!
 *
 */
void ir_remote_task_run(void);

/*!
 *
 */
void ir_remote_task_background_run(void);

/*!
 *
 */
void ir_remote_task_sleep(void);

/*!
 *
 */
void ir_remote_task_wakeup(void);

/*!
 *
 */
void ir_remote_task_finish(void);

/*!
 *
 */
void ir_remote_task_terminate(void);


#endif // _IR_REMOTE_MCU_TASK_H_
