#ifndef _DB_COMMS_H_
#define _DB_COMMS_H_

/*!
 *
 */
void debus_task_init(void);

/*!
 *
 * @return
 */
MCU_TASK_INTERFACE_TASK_STATE debus_task_get_state(void);

/*!
 *
 */
void debus_task_run(void);


/*!
 *
 */
void debus_task_background_run(void);


/*!
 *
 */
void debus_host_com_init(void);

/*!
 *
 */
void debus_polling_handler(void);

/*!
 *
 * @return
 */
u8 debus_host_com_timeout(void);

/*!
 *
 */
void debus_task_schedule(void);

#define FILEOPERATION_MSG_PENDING 0x01
#define FILE_GET_RESULT    0x12

#endif
