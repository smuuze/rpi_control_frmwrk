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
 * @file    command_controller.c
 * @author  Sebastian Lesse
 * @date    2023 / 10 / 03
 * @brief   Short description of this file
 * 
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "cpu.h"

// --------------------------------------------------------------------------------

#include "command_controller.h"
#include "command_handler_interface.h"
#include "local_context.h"

#include "mcu_task_management/mcu_task_controller.h"
#include "mcu_task_management/mcu_task_interface.h"

// --------------------------------------------------------------------------------

#define COMMAND_CONTROLLER_SCHEDULE_INTERVAL_MS        5

// --------------------------------------------------------------------------------

static COMMAND_HANDLER_INTERFACE* _first_cmd_handler = 0;
static COMMAND_HANDLER_INTERFACE* _act_cmd_handler = 0;
static COMMAND_HANDLER_INTERFACE* _last_cmd_handler = 0;

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
static void command_controller_task_start(void) {
    DEBUG_PASS("command_controller_task_start()");
}

/**
 * @brief 
 * 
 * @return u16 
 */
static u16 command_controller_task_get_schedule_interval(void) {
    return COMMAND_CONTROLLER_SCHEDULE_INTERVAL_MS;
}

/**
 * @brief 
 * 
 * @return MCU_TASK_INTERFACE_TASK_STATE 
 */
static MCU_TASK_INTERFACE_TASK_STATE command_controller_task_get_state(void) {

    _act_cmd_handler = _first_cmd_handler;

    while (_act_cmd_handler != 0) {

        if (_act_cmd_handler->is_requested()) {

            DEBUG_PASS("command_controller_cmd_is_pending() - Command-Handler has been requested");
            return MCU_TASK_RUNNING;
        }

        _act_cmd_handler = _act_cmd_handler->next;
    }

    return MCU_TASK_SLEEPING;
}

/**
 * @brief 
 * 
 */
static void command_controller_task_execute(void) {

    DEBUG_PASS("command_controller_task_execute()");

    if (_act_cmd_handler == 0) {
        DEBUG_PASS("command_controller_task_execute() - No command-handler selected !!! ---");
        return;
    }

    DEBUG_TRACE_byte(_act_cmd_handler->get_table_size(), "command_controller_task_execute() - Processing Command-Handler Table");

    u8 cmd_id = _act_cmd_handler->get_cmd_code();
    u8 cmd_ret_code = 0xFF;

    u8 i = 0;
    while (i < _act_cmd_handler->get_table_size()) {

        if (_act_cmd_handler->command_handler_table[i].command_id == cmd_id) {

            DEBUG_TRACE_byte(cmd_id, "command_controller_task_execute() - Running Command-Handler");
            cmd_ret_code = _act_cmd_handler->command_handler_table[i].handle(_act_cmd_handler->get_protocol());
            break;
        }

        i++;
    }

    if (cmd_ret_code == 0xFF) {
        cmd_ret_code = _act_cmd_handler->default_handler(_act_cmd_handler->get_protocol());
    }

    _act_cmd_handler->unset_reqeust();

    _act_cmd_handler = 0;
}

/**
 * @brief 
 * 
 */
static void command_controller_task_terminate(void) {

}

TASK_CREATE(
    CMD_CTRL_TASK,
    TASK_PRIORITY_MIDDLE,
    command_controller_task_get_schedule_interval,
    command_controller_task_start,
    command_controller_task_execute,
    command_controller_task_get_state,
    command_controller_task_terminate
)

// --------------------------------------------------------------------------------

void command_controller_init(void) {
    DEBUG_PASS("command_controller_init()");
    CMD_CTRL_TASK_init();
}

/**
 * @see command_controller.h#command_controller_register_handler
 */
void command_controller_register_handler(COMMAND_HANDLER_INTERFACE* p_handler) {

    if (_first_cmd_handler == 0) {

        DEBUG_PASS("command_controller_register_handler() - First Handler");
        _first_cmd_handler = p_handler;

    } else {

        DEBUG_PASS("command_controller_register_handler() - New Handler");
        _last_cmd_handler->next = p_handler;
    }

    _last_cmd_handler = p_handler;
    _last_cmd_handler->next = 0;
}

// --------------------------------------------------------------------------------
