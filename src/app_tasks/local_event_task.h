/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	local_event_task.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _LOCAL_EVENT_MCU_TASK_H_
#define _LOCAL_EVENT_MCU_TASK_H_

// --------------------------------------------------------------------------------

#include "system_interface.h"
#include "mcu_task_interface.h"

// --------------------------------------------------------------------------------

/*!
 *
 */
void local_event_mcu_task_init(void);

/*!
 *
 */
u16 local_event_mcu_task_get_schedule_interval(void);

/*!
 *
 */
MCU_TASK_INTERFACE_TASK_STATE local_event_mcu_task_get_state(void);

/*!
 *
 */
void local_event_mcu_task_run(void);

/*!
 *
 */
void local_event_mcu_task_background_run(void);

/*!
 *
 */
void local_event_mcu_task_sleep(void);

/*!
 *
 */
void local_event_mcu_task_wakeup(void);

/*!
 *
 */
void local_event_mcu_task_finish(void);

/*!
 *
 */
void local_event_mcu_task_terminate(void);


void local_event_add(SYSTEM_EVENT event);


SYSTEM_EVENT local_event_get_next(void);


#endif // _LOCAL_EVENT_MCU_TASK_H_
