#ifndef _MCU_IDLE_TASK_H_
#define _MCU_IDLE_TASK_H_

/*!
 *
 */
void mcu_idle_task_init(void);

/*!
 *
 */
MCU_TASK_INTERFACE_TASK_STATE mcu_idle_task_is_runable(void);

/*!
 *
 */
void mcu_idle_task_run(void);

/*!
 *
 */
void mcu_idle_task_background_run(void);

/*!
 *
 */
void mcu_idle_task_sleep(void);

/*!
 *
 */
void mcu_idle_task_wakeup(void);

/*!
 *
 */
void mcu_idle_task_finish(void);

/*!
 *
 */
void mcu_idle_task_terminate(void);


#endif // _MCU_IDLE_TASK_H_
