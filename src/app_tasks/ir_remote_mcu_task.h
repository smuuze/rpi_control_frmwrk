#ifndef _IR_REMOTE_MCU_TASK_H_
#define _IR_REMOTE_MCU_TASK_H_

/*!
 *
 */
void ir_remote_task_init(void);

/*!
 *
 */
u8 ir_remote_task_is_runable(void);

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
