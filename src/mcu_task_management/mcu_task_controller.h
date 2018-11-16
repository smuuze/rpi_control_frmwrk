#ifndef _MCU_TASK_CONTROLLER_H_
#define _MCU_TASK_CONTROLLER_H_

#include "mcu_task_interface.h"

/*!
 *
 */
void mcu_task_controller_init(void);

/*!
 *
 */
void mcu_task_controller_register_task(MCU_TASK_INTERFACE* p_mcu_task);

/*!
 *
 */
void mcu_task_controller_schedule(void);

/*!
 *
 */
void mcu_task_controller_sleep(void);

/*!
 *
 */
void mcu_task_controller_wakeup(void);

#endif // _MCU_TASK_CONTROLLER_H_
