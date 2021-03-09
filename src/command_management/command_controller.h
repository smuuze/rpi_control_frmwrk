/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	command_controller.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _COMMAND_CONTROLLER_H_
#define _COMMAND_CONTROLLER_H_

//-----------------------------------------------------------------------------

#include "command_handler_interface.h"
#include "mcu_task_interface.h"

//-----------------------------------------------------------------------------

/*!
 *
 */
void command_controller_init(void);

/*!
 *
 * @param p_protocol
 */
void command_controller_register_handler(COMMAND_HANDLER_INTERFACE* p_handler);

//-----------------------------------------------------------------------------

#endif // _COMMAND_CONTROLLER_H_
