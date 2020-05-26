/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	app_task/copro_routing_mcu_task.h
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _COPRO_ROUTING_MCU_TASK_H_
#define _COPRO_ROUTING_MCU_TASK_H_

#include "mcu_task_interface.h"

/*!
 *
 */
void copro_routing_task_init(void);

/*!
 *
 */
u16 copro_routing_task_get_schedule_interval(void);

/*!
 *
 */
MCU_TASK_INTERFACE_TASK_STATE copro_routing_task_get_state(void);

/*!
 *
 */
void copro_routing_task_run(void);

/*!
 *
 */
void copro_routing_task_background_run(void);

/*!
 *
 */
void copro_routing_task_sleep(void);

/*!
 *
 */
void copro_routing_task_wakeup(void);

/*!
 *
 */
void copro_routing_task_finish(void);

/*!
 *
 */
void copro_routing_task_terminate(void);


#endif // _COPRO_ROUTING_MCU_TASK_H_
