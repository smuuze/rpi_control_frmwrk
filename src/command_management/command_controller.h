#ifndef _COMMAND_CONTROLLER_H_
#define _COMMAND_CONTROLLER_H_

#include "command_handler_interface.h"

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
void command_controller_background_run(void);

#endif // _COMMAND_CONTROLLER_H_
