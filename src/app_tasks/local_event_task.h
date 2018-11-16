#ifndef _LOCAL_EVENT_MCU_TASK_H_
#define _LOCAL_EVENT_MCU_TASK_H_

#include "system_interface.h"

/*!
 *
 */
void local_event_mcu_task_init(void);

/*!
 *
 */
u8 local_event_mcu_task_is_runable(void);

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
