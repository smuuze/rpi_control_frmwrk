/**
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * @file   ir_remote_mcu_task.h
 * @author Sebastian Lesse
 * @date   2021 / 12 / 30
 * @brief  Processes incomming ir-commands.
 * 
 *      Concept:
 * 
 *      A new ir-command is received via the signal IR_CMD_RECEIVED_SIGNAL.
 *      The signal is send by a command-handler.
 *      The including command is stored temporarily.
 *      The module-status updated, IR_REMOTE_TASK_STATUS_CMD_PENDING + IR_REMOTE_TASK_STATUS_CMD_RECEIVED
 *      The pending ir-command is processed next time this module is scheduled.
 *      The pending command then is matched against all available ir-protocols.
 *      If the requested protocol is available the command is given to the ir-protocol-module.
 *      This module checks for complete operation of the ir-protocol-module on every schedule.
 *      While the ir-protocol-module is busy no new ir-command can be processed.
 * 
 */

#ifndef _IR_REMOTE_MCU_TASK_H_
#define _IR_REMOTE_MCU_TASK_H_

// --------------------------------------------------------------------------------

#include "mcu_task_interface.h"

// --------------------------------------------------------------------------------

/**
 * @brief Initializes the IR-Remote Task-Module
 * and registers the task to the MCU Task-Controller
 * 
 */
void ir_remote_app_task_init(void);

// --------------------------------------------------------------------------------

#endif // _IR_REMOTE_MCU_TASK_H_
