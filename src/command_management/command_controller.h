#ifndef _COMMAND_CONTROLLER_H_
#define _COMMAND_CONTROLLER_H_

#include "command_handler_interface.h"
#include "mcu_task_interface.h"

/*!
 *
 */
void command_controller_init(void);

/*!
 *
 * @param p_protocol
 */
void command_controller_register_handler(COMMAND_HANDLER_INTERFACE* p_handler);

/*!
 *
 */
u8 command_controller_cmd_is_pending(void);

/*!
 *
 */
void command_controller_handle_command(void);

/*!
 *
 */
u16 command_controller_task_get_schedule_interval(void);

/*!
 *
 * @return
 */
MCU_TASK_INTERFACE_TASK_STATE command_controller_task_get_state(void);

/*!
 *
 */
void command_controller_background_run(void);

#endif // _COMMAND_CONTROLLER_H_
