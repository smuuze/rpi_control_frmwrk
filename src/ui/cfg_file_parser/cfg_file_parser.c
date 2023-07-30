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
 * @file    cfg_file_parser.c
 * @author  Sebastian Lesse
 * @date    2020 / 11 / 10
 * @brief   Implementation of the cfg_file_parser interface.
 * 
 */

#define TRACER_ON

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
#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

#include "ui/file_interface/file_interface.h"
#include "ui/cfg_file_parser/cfg_file_parser.h"

// --------------------------------------------------------------------------------

/**
 * @brief states of the task state-machine
 */
typedef enum {
    CFG_FILE_PARSER_TASK_STATE_IDLE,
    CFG_FILE_PARSER_WAIT_FOR_FILE,
    CFG_FILE_PARSER_TASK_STATE_OPEN_FILE,
    CFG_FILE_PARSER_TASK_STATE_LOAD_CONFIGURATION,
    CFG_FILE_PARSER_TASK_STATE_SIGNAL_COMPLETE
} CFG_FILE_PARSER_TASK_STATE_TYPE;

// --------------------------------------------------------------------------------

/**
 * @brief Is set if a new file-path was successfull received via the 
 * CLI_CONFIGURATION_SIGNAL.
 */
#define CFG_FILE_PARSER_NEW_CFG_FILE_SET                (1 << 0)

/**
 * @brief Is set at init time. This status is used to
 * realize a timeout after system start to send the 
 * CFG_PARSER_CFG_COMPLETE_SIGNAL if no cfg-file is given.
 */
#define CFG_FILE_PARSER_STATUS_TIMEOUT_ACTIVE           (1 << 1)

BUILD_MODULE_STATUS_U8(CFG_FILE_PARSER_STATUS)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(CFG_FILE_PARSER_WAIT_FOR_CFG_FILE_TIMER)

// --------------------------------------------------------------------------------

FILE_INTERFACE_CREATE_FILE(CFG_FILE)

// --------------------------------------------------------------------------------

/*
 *
 */
static char cfg_file_path[CFG_FILE_PARSER_MAX_LINE_LENGTH];

// --------------------------------------------------------------------------------

/**
 * @brief Callback to receive a path of a configuration file that will be processed.
 * 
 * @param p_argument path to the configuration file as string.
 */
static void cfg_file_parser_CFG_FILE_SLOT_CALLBACK(const void* p_argument) {

    if (p_argument == NULL) {
        DEBUG_PASS("cfg_file_parser_CFG_FILE_SLOT_CALLBACK() - NULL_POINTER_EXEPTION");
        return;
    }

    const char* file_path = (const char*)p_argument;

    u16 file_path_length = common_tools_string_length(file_path);

    if (file_path_length > CFG_FILE_PARSER_MAX_LENGTH_PATH) {

        DEBUG_TRACE_byte(
            file_path_length,
            "cfg_file_parser_CFG_FILE_SLOT_CALLBACK() - Path-Length too long"
        );
        return;
    }

    if (file_path_length == 0) {

        DEBUG_TRACE_byte(
            file_path_length,
            "cfg_file_parser_CFG_FILE_SLOT_CALLBACK() - Path-Length is zero"
        );
        return;
    }

    DEBUG_TRACE_STR(file_path, "cfg_file_parser_CFG_FILE_SLOT_CALLBACK()");

    common_tools_string_copy_string(
        cfg_file_path,
        file_path,
        CFG_FILE_PARSER_MAX_LINE_LENGTH
    );

    CFG_FILE_PARSER_STATUS_set(CFG_FILE_PARSER_NEW_CFG_FILE_SET);
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(
    CLI_CONFIGURATION_SIGNAL,
    CFG_PARSER_CFG_FILE_SLOT,
    cfg_file_parser_CFG_FILE_SLOT_CALLBACK
)

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CFG_PARSER_CFG_COMPLETE_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(CFG_PARSER_NEW_CFG_OBJECT_SIGNAL)

/*
 *
 */
static CFG_FILE_PARSER_TASK_STATE_TYPE app_task_state = CFG_FILE_PARSER_TASK_STATE_IDLE;

// --------------------------------------------------------------------------------

static void cfg_file_parser_parse_configuration_line(char* line) {

    DEBUG_TRACE_STR(line, "cfg_file_parser_parse_configuration_line()");

    if (strlen(line) == 0) {
        DEBUG_PASS("cfg_file_parser_parse_configuration_line() - Length of line is zero");
        return;
    }

    if (line[0] == CFG_FILE_PARSER_COMMENT_CHARACTER) {
        DEBUG_PASS("cfg_file_parser_parse_configuration_line() - This is a comment-line");
        return;
    }

    if (common_tools_string_get_char_count(CFG_FILE_PARSER_KEY_VALUE_SPLITTER, line) != 1) {
        DEBUG_PASS("cfg_file_parser_parse_configuration_line() - not a configuration line");
        return;
    }

    CFG_FILE_PARSER_CFG_OBJECT_TYPE cfg_object;

    common_tools_string_split(
        CFG_FILE_PARSER_KEY_VALUE_SPLITTER,
        line,
        cfg_object.key,
        CFG_FILE_PARSER_MAX_LINE_LENGTH,
        cfg_object.value,
        CFG_FILE_PARSER_MAX_LINE_LENGTH
    );

    if (strlen(cfg_object.key) == 0) {
        DEBUG_PASS("cfg_file_parser_parse_configuration_line() - no key in line");
        return;
    }

    if (strlen(cfg_object.value) == 0) {
        DEBUG_PASS("cfg_file_parser_parse_configuration_line() - no value in line");
        return;
    }

    DEBUG_TRACE_STR(cfg_object.key, "cfg_file_parser_parse_configuration_line() - cfg-key: ");
    DEBUG_TRACE_STR(cfg_object.value, "cfg_file_parser_parse_configuration_line() - cfg-value: ");

    CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_send((void*)&cfg_object);
}

static void cfg_file_parser_parse_configuration_file(void) {

    DEBUG_PASS("cfg_file_parser_parse_configuration_file()");

    i16 num_bytes_line = 0;
    char line[CFG_FILE_PARSER_MAX_LINE_LENGTH];    

    do {
        num_bytes_line = CFG_FILE_read_next_line(line, CFG_FILE_PARSER_MAX_LINE_LENGTH);

        if (num_bytes_line > 0) {

            common_tools_string_trim(line);
            cfg_file_parser_parse_configuration_line(line);

        }

    } while (num_bytes_line != -1);
}

// --------------------------------------------------------------------------------

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.init
 */
void cfg_file_parser_task_init(void) {
    
    DEBUG_PASS("cfg_file_parser_task_init()");
    app_task_state = CFG_FILE_PARSER_TASK_STATE_IDLE;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_schedule_interval
 */
u16 cfg_file_parser_task_get_schedule_interval(void) {
    return CFG_FILE_PARSER_TASK_RUN_INTERVAL_MS;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_task_state
 */
MCU_TASK_INTERFACE_TASK_STATE cfg_file_parser_task_get_state(void) {

    if (CFG_FILE_PARSER_STATUS_is_set(CFG_FILE_PARSER_NEW_CFG_FILE_SET)) {
        DEBUG_PASS("cfg_file_parser_task_get_state() - New file has been set");
        return MCU_TASK_RUNNING;
    }

    if (CFG_FILE_PARSER_STATUS_is_set(CFG_FILE_PARSER_STATUS_TIMEOUT_ACTIVE)) {
        DEBUG_PASS("cfg_file_parser_task_get_state() - timeout is active");
        return MCU_TASK_RUNNING;
    }

    if (app_task_state != CFG_FILE_PARSER_WAIT_FOR_FILE) {
        return MCU_TASK_RUNNING;
    }

    //DEBUG_PASS("cfg_file_parser_task_get_state() - SLEEPING");
    
    return MCU_TASK_SLEEPING;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.run
 */
void cfg_file_parser_task_run(void) {
    
    DEBUG_PASS("cfg_file_parser_task_run()");

    switch (app_task_state) {
        
        default:

            app_task_state = CFG_FILE_PARSER_TASK_STATE_IDLE;
            // no break;
            // fall through

        case CFG_FILE_PARSER_TASK_STATE_IDLE :

            if (CFG_FILE_PARSER_WAIT_FOR_CFG_FILE_TIMER_is_active()) {
                if (CFG_FILE_PARSER_WAIT_FOR_CFG_FILE_TIMER_is_up(CFG_FILE_PARSER_WAIT_FOR_CFG_FILE_TIMEOUT_MS)) {

                    DEBUG_PASS("cfg_file_parser_task_run() - CFG_FILE_PARSER_TASK_STATE_IDLE > CFG_FILE_PARSER_TASK_STATE_SIGNAL_COMPLETE");
                    app_task_state = CFG_FILE_PARSER_TASK_STATE_SIGNAL_COMPLETE;
                    break;
                }
            }

            if (CFG_FILE_PARSER_STATUS_is_set(CFG_FILE_PARSER_NEW_CFG_FILE_SET) == 0) {
                break;
            }

            CFG_FILE_PARSER_STATUS_unset(CFG_FILE_PARSER_NEW_CFG_FILE_SET);
            CFG_FILE_PARSER_STATUS_unset(CFG_FILE_PARSER_STATUS_TIMEOUT_ACTIVE);

            DEBUG_PASS("cfg_file_parser_task_run() - CFG_FILE_PARSER_TASK_STATE_IDLE > CFG_FILE_PARSER_TASK_STATE_OPEN_FILE");
            app_task_state = CFG_FILE_PARSER_TASK_STATE_OPEN_FILE;

            // no break;
            // fall through

        case CFG_FILE_PARSER_TASK_STATE_OPEN_FILE :

            if (CFG_FILE_open_path(cfg_file_path) == 0) {
                DEBUG_PASS("cfg_file_parser_task_run() - Open new cfg-file has FAILED !!! ---");
                DEBUG_PASS("cfg_file_parser_task_run() - CFG_FILE_PARSER_TASK_STATE_OPEN_FILE > CFG_FILE_PARSER_TASK_STATE_SIGNAL_COMPLETE");
                app_task_state = CFG_FILE_PARSER_TASK_STATE_SIGNAL_COMPLETE;
                break;
            }

            DEBUG_PASS("cfg_file_parser_task_run() - CFG_FILE_PARSER_TASK_STATE_OPEN_FILE > CFG_FILE_PARSER_TASK_STATE_LOAD_CONFIGURATION");

            app_task_state = CFG_FILE_PARSER_TASK_STATE_LOAD_CONFIGURATION;
            // no break;
            // fall through

        case CFG_FILE_PARSER_TASK_STATE_LOAD_CONFIGURATION :

            cfg_file_parser_parse_configuration_file();

            DEBUG_PASS("cfg_file_parser_task_run() - CFG_FILE_PARSER_TASK_STATE_LOAD_CONFIGURATION > CFG_FILE_PARSER_TASK_STATE_SIGNAL_COMPLETE");
            app_task_state = CFG_FILE_PARSER_TASK_STATE_SIGNAL_COMPLETE;

            break;

        case CFG_FILE_PARSER_TASK_STATE_SIGNAL_COMPLETE :

            CFG_FILE_PARSER_STATUS_unset(CFG_FILE_PARSER_STATUS_TIMEOUT_ACTIVE);
            CFG_FILE_PARSER_WAIT_FOR_CFG_FILE_TIMER_stop();
            CFG_PARSER_CFG_COMPLETE_SIGNAL_send(NULL);

            DEBUG_PASS("cfg_file_parser_task_run() - CFG_FILE_PARSER_TASK_STATE_SIGNAL_COMPLETE > CFG_FILE_PARSER_WAIT_FOR_FILE");
            app_task_state = CFG_FILE_PARSER_WAIT_FOR_FILE;

            break;

        case CFG_FILE_PARSER_WAIT_FOR_FILE :

            if (CFG_FILE_PARSER_STATUS_is_set(CFG_FILE_PARSER_NEW_CFG_FILE_SET)) {
                CFG_FILE_PARSER_STATUS_unset(CFG_FILE_PARSER_NEW_CFG_FILE_SET);

                DEBUG_PASS("cfg_file_parser_task_run() - CFG_FILE_PARSER_WAIT_FOR_FILE > CFG_FILE_PARSER_TASK_STATE_OPEN_FILE");
                app_task_state = CFG_FILE_PARSER_TASK_STATE_OPEN_FILE;
            }

            break;
    }

}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.background_run
 */
void cfg_file_parser_task_background_run(void) {

}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.sleep
 */
void cfg_file_parser_task_sleep(void) {

}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.wakeup
 */
void cfg_file_parser_task_wakeup(void) {

}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.finish
 */
void cfg_file_parser_task_finish(void) {

}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.terminate
 */
void cfg_file_parser_task_terminate(void) {

}

// --------------------------------------------------------------------------------

static MCU_TASK_INTERFACE cfg_file_parser_task = {

    0,                                              // u8 identifier,
    0,                                              // u16 new_run_timeout,
    0,                                              // u16 last_run_time,
    &cfg_file_parser_task_init,                     // MCU_TASK_INTERFACE_INIT_CALLBACK         init,
    &cfg_file_parser_task_get_schedule_interval,    // MCU_TASK_INTERFACE_INIT_CALLBACK         init,
    &cfg_file_parser_task_get_state,                // MCU_TASK_INTERFACE_GET_STATE_CALLBACK    get_sate,
    &cfg_file_parser_task_run,                      // MCU_TASK_INTERFACE_RUN_CALLBACK          run,
    0,                                              // MCU_TASK_INTERFACE_BG_RUN_CALLBACK       background_run,
    0,                                              // MCU_TASK_INTERFACE_SLEEP_CALLBACK        sleep,
    0,                                              // MCU_TASK_INTERFACE_WAKEUP_CALLBACK       wakeup,
    &cfg_file_parser_task_finish,                   // MCU_TASK_INTERFACE_FINISH_CALLBACK       finish,
    &cfg_file_parser_task_terminate,                // MCU_TASK_INTERFACE_TERMINATE_CALLBACK    terminate,
    0                                               // next-task
};

// --------------------------------------------------------------------------------

/**
 * @see cfg_file_parser.h#cfg_file_parser_init
 */
void cfg_file_parser_init(void) {

    CFG_FILE_PARSER_STATUS_clear_all();
    
    DEBUG_PASS("cfg_file_parser_init() - CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_init()");
    CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_init();
    CFG_PARSER_NEW_CFG_OBJECT_SIGNAL_set_timeout(0);

    DEBUG_PASS("cfg_file_parser_init() - CFG_PARSER_CFG_COMPLETE_SIGNAL_init()");
    CFG_PARSER_CFG_COMPLETE_SIGNAL_init();
    CFG_PARSER_CFG_COMPLETE_SIGNAL_set_timeout(0);

    DEBUG_PASS("cfg_file_parser_init() - CFG_PARSER_CFG_FILE_SLOT_connect()");
    CFG_PARSER_CFG_FILE_SLOT_connect();

    DEBUG_PASS("cfg_file_parser_init() - mcu_task_controller_register_task()");
    mcu_task_controller_register_task(&cfg_file_parser_task);

    CFG_FILE_PARSER_WAIT_FOR_CFG_FILE_TIMER_start();

    CFG_FILE_PARSER_STATUS_set(CFG_FILE_PARSER_STATUS_TIMEOUT_ACTIVE);
}

// --------------------------------------------------------------------------------
