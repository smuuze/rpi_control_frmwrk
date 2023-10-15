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
 * @file    cli_executer_task.c
 * @author  Sebastian Lesse
 * @date    2022 / 02 / 20
 * @see     app_task/cli_executer_task.h
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

#include "mcu_task_management/mcu_task_interface.h"
#include "common/local_module_status.h"
#include "common/signal_slot_interface.h"
#include "common/common_tools_string.h"
#include "common/common_types.h"
#include "time_management/time_management.h"
#include "ui/cfg_file_parser/cfg_file_parser.h"
//#include "protocol_management/mqtt/mqtt_interface.h"
#include "ui/file_interface/file_interface.h"

// --------------------------------------------------------------------------------

#include "app_tasks/cli_executer_task.h"

// --------------------------------------------------------------------------------

#ifndef CLI_EXECUTER_TEST_COMMAND
#define CLI_EXECUTER_TEST_COMMAND                   "which "
#endif

#ifndef CLI_EXECUTER_NEW_LINE_CHAR
#define CLI_EXECUTER_NEW_LINE_CHAR                  '\n'
#endif

#ifndef CLI_EXECUTER_NOT_HANG_UP_CHAR
#define CLI_EXECUTER_NOT_HANG_UP_CHAR               '&'
#endif

// --------------------------------------------------------------------------------

/**
 * @brief Task states that are used by this module
 * 
 */
typedef enum {

    CLI_EXECUTER_TASK_STATE_IDLE,
    CLI_EXECUTER_TASK_TERMINATED,
    CLI_EXECUTER_TASK_STATE_EXECUTE_COMMAND,
    CLI_EXECUTER_TASK_STATE_WAIT_FOR_COMPLETION,
    CLI_EXECUTER_TASK_STATE_CLOSE_PROCESS,
    CLI_EXECUTER_TASK_STATE_CLOSE_ON_FAIL
} CLI_EXECUTER_TASK_STATE_TYPE;

// --------------------------------------------------------------------------------

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.init
 * 
 */
static void cli_executer_task_init(void);

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_schedule_interval
 * 
 */
static u16 cli_executer_task_get_schedule_interval(void);

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_state
 * 
 */
static MCU_TASK_INTERFACE_TASK_STATE cli_executer_task_get_state(void);

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.run
 * 
 */
static void cli_executer_task_run(void);

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.finish
 * 
 */
static void cli_executer_task_finish(void);

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.terminate
 * 
 */
static void cli_executer_task_terminate(void);

// --------------------------------------------------------------------------------

// SLOT CALLBACKS

/**
 * @brief 
 * 
 * @param p_argument 
 */
static void cli_executer_CLI_EXECUTER_COMMAND_RECEIVED_SLOT_CALLBACK(const void* p_argument);

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_EXECUTER_COMMAND_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_EXECUTER_COMMAND_RESPONSE_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_EXECUTER_COMMAND_TIMEOUT_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CLI_EXECUTER_COMMAND_NOT_FOUND_SIGNAL)

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(CLI_EXECUTER_COMMAND_RECEIVED_SIGNAL, CLI_EXECUTER_COMMAND_RECEIVED_SLOT, cli_executer_CLI_EXECUTER_COMMAND_RECEIVED_SLOT_CALLBACK)

// --------------------------------------------------------------------------------

#define CLI_EXECUTER_STATUS_COMMAND_RECEIVED            (1 << 0)

BUILD_MODULE_STATUS_U8(CLI_EXECUTER_STATUS)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(CLI_EXECUTER_OP_TIMER)

// --------------------------------------------------------------------------------

TASK_CREATE (
    CLI_EXE_TASK,
    TASK_PRIORITY_MIDDLE,
    cli_executer_task_get_schedule_interval,
    cli_executer_task_init,
    cli_executer_task_run,
    cli_executer_task_get_state,
    cli_executer_task_terminate
)

/**
 * @brief actual state of this task
 * 
 */
static CLI_EXECUTER_TASK_STATE_TYPE cli_executer_task_state = CLI_EXECUTER_TASK_STATE_IDLE;

/**
 * @brief holds the cli command that has been received by the CLI_EXECUTER_COMMAND_RECEIVED signal
 * 
 */
static char cli_executer_pending_command[CLI_EXECUTER_MAX_MESSAGE_LENGTH];

/**
 * @brief Holda the command response that is send via the CLI_EXECUTER_COMMAND_RESPONSE signal
 * 
 */
static char cli_executer_pending_response[CLI_EXECUTER_MAX_MESSAGE_LENGTH];

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param p_command 
 * @return u8 
 */
static u8 cli_executer_test_process(const char* p_command);

/**
 * @brief 
 * 
 * @param p_response 
 * @param max_response_length 
 */
static void cli_executer_prepare_response(char* p_response, u8 max_response_length);

/**
 * @brief 
 * 
 * @param p_command 
 * @param p_response 
 * @param max_response_length 
 * @param timeout 
 * @return u8 
 */
static u8 cli_executer_open_process(const char* p_command, char* p_response, u8 max_response_length, u16 timeout);

static u32 counter_poepn_for_test = 0;

// --------------------------------------------------------------------------------

/**
 * @see app_task/cli_executer_task.h#cli_executer_init
 * 
 */
void cli_executer_init(void) {
    
    DEBUG_PASS("cli_executer_init()");

    CLI_EXECUTER_STATUS_clear_all();

    DEBUG_PASS("cli_executer_init() - Initialize signals");

    CLI_EXECUTER_COMMAND_RECEIVED_SIGNAL_init();
    CLI_EXECUTER_COMMAND_RESPONSE_SIGNAL_init();
    CLI_EXECUTER_COMMAND_TIMEOUT_SIGNAL_init();
    CLI_EXECUTER_COMMAND_NOT_FOUND_SIGNAL_init();

    DEBUG_PASS("cli_executer_init() - Connect slots");

    CLI_EXECUTER_COMMAND_RECEIVED_SLOT_connect();

    DEBUG_PASS("cli_executer_init() - Register task");

    CLI_EXE_TASK_init();
}

// --------------------------------------------------------------------------------

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.init
 * 
 */
static void cli_executer_task_init(void) {
    
    DEBUG_PASS("cli_executer_task_init()");
    cli_executer_task_state = CLI_EXECUTER_TASK_STATE_IDLE;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_schedule_interval
 * 
 */
static u16 cli_executer_task_get_schedule_interval(void) {

    //DEBUG_PASS("cli_executer_task_get_schedule_interval()");
    return CLI_EXECUTER_TASK_SCHEDULE_INTERVAL_MS;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_state
 * 
 */
static MCU_TASK_INTERFACE_TASK_STATE cli_executer_task_get_state(void) {

    if (cli_executer_task_state == CLI_EXECUTER_TASK_TERMINATED) {
        return MCU_TASK_SLEEPING;
    }
    
    if (cli_executer_task_state != CLI_EXECUTER_TASK_STATE_IDLE) {
        //DEBUG_PASS("cli_executer_task_get_state() - RUNNING");
        return MCU_TASK_RUNNING;
    }

    if (CLI_EXECUTER_STATUS_is_set(CLI_EXECUTER_STATUS_COMMAND_RECEIVED)) {
        DEBUG_PASS("cli_executer_task_get_state() - COMMAND RECEIVED");
        return MCU_TASK_RUNNING;
    }

    return MCU_TASK_SLEEPING;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.run
 * 
 */
static void cli_executer_task_run(void) {

    switch (cli_executer_task_state) {

        default:

            cli_executer_task_state = CLI_EXECUTER_TASK_STATE_IDLE;

        case CLI_EXECUTER_TASK_STATE_IDLE :

            if (CLI_EXECUTER_STATUS_is_set(CLI_EXECUTER_STATUS_COMMAND_RECEIVED)) {

                CLI_EXECUTER_STATUS_unset(CLI_EXECUTER_STATUS_COMMAND_RECEIVED);

                DEBUG_PASS("cli_executer_task_run() - CLI_EXECUTER_TASK_STATE_IDLE >> CLI_EXECUTER_TASK_STATE_EXECUTE_COMMAND");
                cli_executer_task_state = CLI_EXECUTER_TASK_STATE_EXECUTE_COMMAND;
                break;
            }

            break;

        case CLI_EXECUTER_TASK_STATE_EXECUTE_COMMAND:

            cli_executer_prepare_response(cli_executer_pending_response, CLI_EXECUTER_MAX_MESSAGE_LENGTH);
                
            if ( ! cli_executer_open_process(cli_executer_pending_command, cli_executer_pending_response, CLI_EXECUTER_MAX_MESSAGE_LENGTH, CLI_EXECUTER_EXECUTION_TIMEOUT_MS)) {
                DEBUG_PASS("cli_executer_task_run() - CLI_EXECUTER_TASK_STATE_EXECUTE_COMMAND >> CLI_EXECUTER_TASK_STATE_IDLE");
                cli_executer_task_state = CLI_EXECUTER_TASK_STATE_CLOSE_ON_FAIL;
                break;
            }

            CLI_EXECUTER_OP_TIMER_start();

            DEBUG_PASS("cli_executer_task_run() - CLI_EXECUTER_TASK_STATE_EXECUTE_COMMAND >> CLI_EXECUTER_TASK_STATE_CLOSE_PROCESS");
            cli_executer_task_state = CLI_EXECUTER_TASK_STATE_CLOSE_PROCESS;

            break;

        case CLI_EXECUTER_TASK_STATE_CLOSE_PROCESS:

            CLI_EXECUTER_COMMAND_RESPONSE_SIGNAL_send(cli_executer_pending_response);

            DEBUG_PASS("cli_executer_task_run() - CLI_EXECUTER_TASK_STATE_CLOSE_PROCESS >> CLI_EXECUTER_TASK_STATE_IDLE");
            cli_executer_task_state = CLI_EXECUTER_TASK_STATE_IDLE;
            break;
        
        case CLI_EXECUTER_TASK_STATE_CLOSE_ON_FAIL :

            CLI_EXECUTER_COMMAND_NOT_FOUND_SIGNAL_send(cli_executer_pending_command);

            DEBUG_PASS("cli_executer_task_run() - CLI_EXECUTER_TASK_STATE_CLOSE_ON_FAIL >> CLI_EXECUTER_TASK_STATE_IDLE");
            cli_executer_task_state = CLI_EXECUTER_TASK_STATE_IDLE;

            break;
    }
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.finish
 * 
 */
static void cli_executer_task_finish(void) {
    // nothing to do here
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.terminate
 * 
 */
static void cli_executer_task_terminate(void) {
    cli_executer_task_state = CLI_EXECUTER_TASK_TERMINATED;
}

// --------------------------------------------------------------------------------

/**
 * @see app_task/cli_executer_task.c#cli_executer_test_process
 */
static u8 cli_executer_test_process(const char* p_command) {
        
    TRACER_ENABLE();

    char command_without_arguments[CLI_EXECUTER_MAX_MESSAGE_LENGTH];
    char command_test[CLI_EXECUTER_MAX_MESSAGE_LENGTH];

    common_tools_string_clear(command_test, CLI_EXECUTER_MAX_MESSAGE_LENGTH);
    common_tools_string_clear(command_without_arguments, CLI_EXECUTER_MAX_MESSAGE_LENGTH);

    common_tools_string_split(' ', p_command, command_without_arguments, CLI_EXECUTER_MAX_MESSAGE_LENGTH, NULL, 0);

    if (common_tools_string_ends_with(command_without_arguments, CLI_EXECUTER_NOT_HANG_UP_CHAR)) {
        DEBUG_PASS("cli_executer_test_process() - No hang-Up command needs special handling");
        common_tools_string_remove_last_character(command_without_arguments);
    }

    common_tools_string_append(command_test, CLI_EXECUTER_TEST_COMMAND, CLI_EXECUTER_MAX_MESSAGE_LENGTH);
    common_tools_string_append(command_test, command_without_arguments, CLI_EXECUTER_MAX_MESSAGE_LENGTH);

    DEBUG_TRACE_STR(command_test, "cli_executer_test_process() - Testing command: ");

    DEBUG_TRACE_long(counter_poepn_for_test, "cli_executer_test_process() - Openings:");

    FILE* test_pipe = popen(command_test, "r");

    if ( test_pipe == NULL ) {
        DEBUG_PASS("cli_executer_test_process() - popen() has FAILED !!!");
        goto cli_executer_test_process_NOT_AVAILABLE_EXIT;
    }

    counter_poepn_for_test += 1;

    char t_buffer[CLI_EXECUTER_MAX_MESSAGE_LENGTH];
    common_tools_string_clear(t_buffer, CLI_EXECUTER_MAX_MESSAGE_LENGTH);
    u8 counter = 0;

    while (counter < CLI_EXECUTER_MAX_MESSAGE_LENGTH - 1) {

        char character = fgetc(test_pipe);
        if (character == EOF) {
            DEBUG_PASS("-- EOF --");
            break;
        }

        if (character == CLI_EXECUTER_NEW_LINE_CHAR) {
            DEBUG_PASS("-- NEW_LINE --");
            break;
        }

        t_buffer[counter++] = character;
    }
    
    pclose(test_pipe);

    DEBUG_TRACE_STR(t_buffer, "cli_executer_test_process() - Output: ");

    if (strlen(t_buffer) == 0) {
        DEBUG_PASS("cli_executer_test_process() - Command seems to be unknown - length of output is zero");
        goto cli_executer_test_process_NOT_AVAILABLE_EXIT;
    }
    
    // sh: 1: /home/shc/scripts/sys_temp.sh: not found
    if (common_tools_string_contains(t_buffer, "not found")) {
        DEBUG_PASS("cli_executer_test_process() - SHELL says: command not found!");
        goto cli_executer_test_process_NOT_AVAILABLE_EXIT;
    }

    if (common_tools_string_contains(t_buffer, command_without_arguments) == 0) {
        DEBUG_TRACE_STR(command_without_arguments, "cli_executer_test_process() - Command path not in output");
        goto cli_executer_test_process_NOT_AVAILABLE_EXIT;
    }    

    DEBUG_PASS("cli_executer_test_process() - Command seems to be available");
    TRACER_DISABLE();
    return 1;

    cli_executer_test_process_NOT_AVAILABLE_EXIT:
        DEBUG_PASS("cli_executer_test_process() - Command not available");
        TRACER_DISABLE();
        return 0;
}

/**
 * @see app_task/cli_executer_task.c#cli_executer_prepare_response
 */
static void cli_executer_prepare_response(char* p_response, u8 max_response_length) {
    DEBUG_PASS("cli_executer_prepare_response()");
    common_tools_string_clear(p_response, max_response_length);
}

/**
 * @see app_task/cli_executer_task.c#cli_executer_open_process
 */
static u8 cli_executer_open_process(const char* p_command, char* p_response, u8 max_response_length, u16 timeout) {
    
    DEBUG_PASS("cli_executer_open_process() - Openings:");    

    if ( ! cli_executer_test_process(p_command) ) {
        DEBUG_PASS("cli_executer_open_process() - Command not found !!!");
        return 0;
    }

    DEBUG_PASS("cli_executer_open_process() - Open Process");

    if (common_tools_string_ends_with(p_command, CLI_EXECUTER_NOT_HANG_UP_CHAR)) {

        DEBUG_PASS("cli_executer_open_process() - This is a NO_HANG_UP command - Ignoring output");

        if (system(p_command) < 0) {
            DEBUG_PASS("cli_executer_open_process() - system() has FAIELD !!! --");
            return 0;
        }

        return 1;
    }
    
    FILE* p_process_pipe = popen(p_command, "r"); // Send the command, popen exits immediately    

    if ( p_process_pipe == NULL ) {
        DEBUG_PASS("cli_executer_open_process() - popen() has FAILED !!!");
        return 0;
    }

    u8 length = 0;

    while(!feof(p_process_pipe) ) {
    
        DEBUG_PASS("cli_executer_open_process() - Get Output of process");

        char t_buffer[CLI_EXECUTER_MAX_MESSAGE_LENGTH];
        common_tools_string_clear(t_buffer, CLI_EXECUTER_MAX_MESSAGE_LENGTH);

        if(fgets(t_buffer, CLI_EXECUTER_MAX_MESSAGE_LENGTH, p_process_pipe) == NULL) {
            break;
        }
        
        DEBUG_TRACE_STR(t_buffer, "cli_executer_open_process() - Output of process");

        u8 byte_count = strlen(t_buffer);

        if (byte_count > max_response_length - length) {
            byte_count = max_response_length - length;
        }

        memcpy(p_response + length, t_buffer, byte_count);
        length += byte_count;

        if (length == max_response_length) {
            break;
        }
    }

    pclose(p_process_pipe);

    common_tools_string_trim(p_response);
    DEBUG_TRACE_STR(p_response, "cli_executer_wait_for_process() - Process Output:");

    return 1;
}

// --------------------------------------------------------------------------------

/**
 * @see app_task/cli_executer_task.c#cli_executer_CLI_EXECUTER_COMMAND_RECEIVED_SLOT_CALLBACK
 */
static void cli_executer_CLI_EXECUTER_COMMAND_RECEIVED_SLOT_CALLBACK(const void* p_argument) {

    if (p_argument == NULL) {
        DEBUG_PASS("cli_executer_cfg_object_CALLBACK() - NULL-POINTER-EXCEPTION");
        return;
    }

    const char* p_command = (const char*) p_argument;

    common_tools_string_copy_string(cli_executer_pending_command, p_command, CLI_EXECUTER_MAX_MESSAGE_LENGTH);
    common_tools_string_trim(cli_executer_pending_command);

    CLI_EXECUTER_STATUS_set(CLI_EXECUTER_STATUS_COMMAND_RECEIVED);

    DEBUG_TRACE_STR(cli_executer_pending_command, "cli_executer_CLI_EXECUTER_COMMAND_RECEIVED_SLOT_CALLBACK() - Command received");
}

// --------------------------------------------------------------------------------
