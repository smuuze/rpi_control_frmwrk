#ifndef _DB_COMMS_H_
#define _DB_COMMS_H_

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
