#ifndef _LOCAL_CMD_MCU_TASK_H_
#define _LOCAL_CMD_MCU_TASK_H_

/*!
 *
 */
void local_cmd_mcu_task_init(void);

/*!
 *
 */
u16 local_cmd_task_get_schedule_interval(void);

/*!
 *
 */
MCU_TASK_INTERFACE_TASK_STATE local_cmd_mcu_task_get_state(void);

/*!
 *
 */
void local_cmd_mcu_task_run(void);

/*!
 *
 */
void local_cmd_mcu_task_background_run(void);

/*!
 *
 */
void local_cmd_mcu_task_finish(void);

/*!
 *
 */
void local_cmd_mcu_task_terminate(void);


#endif // _LOCAL_CMD_MCU_TASK_H_
