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
 * @file    log_interface.c
 * @author  Sebastian Lesse
 * @date    2023 / 10 / 15
 * @brief   Short description of this file
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
#include "common/common_tools_datetime.h"
#include "common/qeue_interface.h"
#include "ui/cfg_file_parser/cfg_file_parser.h"
#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

#include "ui/file_interface/file_interface.h"
#include "ui/log_interface/log_interface.h"

// --------------------------------------------------------------------------------

#ifndef LOG_INTERFACE_MAX_MESSAGE_LENGTH
#define LOG_INTERFACE_MAX_MESSAGE_LENGTH            255
#endif

#ifndef LOG_INTERFACE_MAX_LENGTH_DATE_STRING
#define LOG_INTERFACE_MAX_LENGTH_DATE_STRING        32
#endif

#ifndef LOG_INTERFACE_QEUE_SIZE
#define LOG_INTERFACE_QEUE_SIZE                     25
#endif

#ifndef LOG_INTERFACE_TASK_RUN_INTERVAL_MS
#define LOG_INTERFACE_TASK_RUN_INTERVAL_MS          5
#endif

#ifndef LOG_INTERFACE_DEFAULT_LOG_FILE_PATH
#define LOG_INTERFACE_DEFAULT_LOG_FILE_PATH         "logs"
#endif

#ifndef LOG_INTERFACE_MAX_LOG_FILE_SIZE_KB
#define LOG_INTERFACE_MAX_LOG_FILE_SIZE_KB          2048000
#endif

#ifndef LOG_INTERFAC_MAX_LOG_FILE_PATH_LENGTH
#define LOG_INTERFAC_MAX_LOG_FILE_PATH_LENGTH       255
#endif

#ifndef LOG_INTERFACE_CFG_NAME_LOG_FILE_PATH
#define LOG_INTERFACE_CFG_NAME_LOG_FILE_PATH        "LOG_FILE_PATH"
#endif

#ifndef LOG_INTERFACE_QEUE_OVERFLOW_MESSAGE
#define LOG_INTERFACE_QEUE_OVERFLOW_MESSAGE         "LOG-QEUE OVERFLOW_DETECTED"
#endif

#ifndef LOG_INTERFACE_SLEEPTIME
#define LOG_INTERFACE_SLEEPTIME                     1000
#endif

// --------------------------------------------------------------------------------

#define LOG_INTERFACE_TEMP_STRING_LENGTH            5

// --------------------------------------------------------------------------------

#define LOG_INTERFACE_STATUS_QEUE_OVERFLOW          (1 << 0)
#define LOG_INTERFACE_STATUS_USER_CFG_COMPLETE      (1 << 1)

BUILD_MODULE_STATUS_U8(LOG_INTERFACE_STATUS)

// --------------------------------------------------------------------------------

typedef enum {
    LOG_INTERFACE_TASK_STATE_START_UP,
    LOG_INTERFACE_TASK_STATE_IDLE,
    LOG_INTERFACE_TASK_STATE_CHECK_FILE,
    LOG_INTERFACE_TASK_STATE_OPEN_FILE,
    LOG_INTERFACE_TASK_STATE_CLOSE_FILE,
    LOG_INTERFACE_TASK_STATE_WRITE_LOG,
    LOG_INTERFACE_TASK_STATE_SLEEP
} LOG_INTERFACE_TASK_STATE_TYPE;

// --------------------------------------------------------------------------------

/*
 *
 */
static void log_interface_new_cfg_object_SLOT_CALLBACK(const void* p_argument);

/*
 *
 */
static void log_interface_cfg_complete_SLOT_CALLBACK(const void* p_argument);

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(
    CFG_PARSER_CFG_COMPLETE_SIGNAL,
    LOG_INTERFACE_CFG_COMPLETE_SLOT,
    log_interface_cfg_complete_SLOT_CALLBACK
)

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(
    CFG_PARSER_NEW_CFG_OBJECT_SIGNAL,
    LOG_INTERFACE_NEW_CFG_OBJECT_SLOT,
    log_interface_new_cfg_object_SLOT_CALLBACK
)

// --------------------------------------------------------------------------------

FILE_INTERFACE_CREATE_FILE(LOG_FILE)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(LOG_OP_TIMER)

// --------------------------------------------------------------------------------

QEUE_INTERFACE_BUILD_QEUE(
    LOG_QEUE,
    char,
    LOG_INTERFACE_MAX_MESSAGE_LENGTH,
    LOG_INTERFACE_QEUE_SIZE
)

// --------------------------------------------------------------------------------

/*
 *
 */
static void log_interface_task_init(void);

/*
 *
 */
static u16 log_interface_task_get_schedule_interval(void);

/*
 *
 */
static MCU_TASK_INTERFACE_TASK_STATE log_interface_task_get_state(void);

/*
 *
 */
static void log_interface_task_run(void);

/*
 *
 */
static void log_interface_task_terminate(void);

// --------------------------------------------------------------------------------

TASK_CREATE (
    LOG_TASK,
    TASK_PRIORITY_MIDDLE,
    log_interface_task_get_schedule_interval,
    log_interface_task_init,
    log_interface_task_run,
    log_interface_task_get_state,
    log_interface_task_terminate
)

// --------------------------------------------------------------------------------

/*
 *
 */
static char log_file_path[LOG_INTERFAC_MAX_LOG_FILE_PATH_LENGTH];

/*
 *
 */
static LOG_INTERFACE_TASK_STATE_TYPE app_task_state = LOG_INTERFACE_TASK_STATE_IDLE;

// --------------------------------------------------------------------------------

static void log_interface_update_file_name(void) {

    char date_string[LOG_INTERFACE_MAX_MESSAGE_LENGTH];
    common_tools_datetime_now("%Y_%m_%d", date_string, LOG_INTERFACE_MAX_MESSAGE_LENGTH);

    DEBUG_TRACE_STR(date_string, "log_interface_update_file_name() - Datetime:");

    u8 counter = 1;
    char counter_str[LOG_INTERFACE_TEMP_STRING_LENGTH];
    char log_file[LOG_INTERFACE_MAX_MESSAGE_LENGTH];
    
    common_tools_string_from_u8(counter_str, LOG_INTERFACE_TEMP_STRING_LENGTH, counter);

    common_tools_string_copy_string(log_file, log_file_path, LOG_INTERFACE_MAX_MESSAGE_LENGTH);     // base directory
    common_tools_string_append(log_file, date_string, LOG_INTERFACE_MAX_MESSAGE_LENGTH);        // date-string
    common_tools_string_append(log_file, "__", LOG_INTERFACE_MAX_MESSAGE_LENGTH);            // seperator
    common_tools_string_append(log_file, counter_str, LOG_INTERFACE_MAX_MESSAGE_LENGTH);        // log-file number
    common_tools_string_append(log_file, ".log", LOG_INTERFACE_MAX_MESSAGE_LENGTH);            // extension

    LOG_FILE_set_path(log_file);

    while (LOG_FILE_is_existing()) {

        DEBUG_TRACE_STR(log_file, "log_interface_update_file_name() - Log-File is existing: ");

        if (LOG_FILE_get_size() < LOG_INTERFACE_MAX_LOG_FILE_SIZE_KB) {
            DEBUG_PASS("log_interface_update_file_name() - file is less than limit");
            return;
        }

        counter += 1;
        common_tools_string_from_u8(counter_str, LOG_INTERFACE_TEMP_STRING_LENGTH, counter);

        common_tools_string_copy_string(log_file, log_file_path, LOG_INTERFACE_MAX_MESSAGE_LENGTH);     // base directory
        common_tools_string_append(log_file, date_string, LOG_INTERFACE_MAX_MESSAGE_LENGTH);        // date-string
        common_tools_string_append(log_file, "__", LOG_INTERFACE_MAX_MESSAGE_LENGTH);            // seperator
        common_tools_string_append(log_file, counter_str, LOG_INTERFACE_MAX_MESSAGE_LENGTH);        // log-file number
        common_tools_string_append(log_file, ".log", LOG_INTERFACE_MAX_MESSAGE_LENGTH);            // extension

        LOG_FILE_set_path(log_file);
    }
        
    DEBUG_PASS("log_interface_update_file_name() - create file");
    LOG_FILE_create();
}

static void log_interface_process_qeue(void) {

    char next_message[LOG_INTERFACE_MAX_MESSAGE_LENGTH];
    char date_string[LOG_INTERFACE_MAX_LENGTH_DATE_STRING];
    memset(date_string, '\0', LOG_INTERFACE_MAX_LENGTH_DATE_STRING);

    common_tools_datetime_now("%H:%M:%S - ", date_string, LOG_INTERFACE_MAX_LENGTH_DATE_STRING);

    if (LOG_QEUE_deqeue(next_message) ) {

        char new_line[LOG_INTERFACE_MAX_MESSAGE_LENGTH];
        memset(new_line, '\0', LOG_INTERFACE_MAX_MESSAGE_LENGTH);

        common_tools_string_copy_string(new_line, date_string, LOG_INTERFACE_MAX_MESSAGE_LENGTH);
        common_tools_string_append(new_line, next_message, LOG_INTERFACE_MAX_MESSAGE_LENGTH);

        LOG_FILE_append_line(new_line);
    }

    if (LOG_INTERFACE_STATUS_is_set(LOG_INTERFACE_STATUS_QEUE_OVERFLOW)) { 
        
        LOG_INTERFACE_STATUS_unset(LOG_INTERFACE_STATUS_QEUE_OVERFLOW);

        char new_line[LOG_INTERFACE_MAX_MESSAGE_LENGTH];
        memset(new_line, '\0', LOG_INTERFACE_MAX_MESSAGE_LENGTH);

        common_tools_string_copy_string(new_line, date_string, LOG_INTERFACE_MAX_MESSAGE_LENGTH);
        common_tools_string_append(new_line, LOG_INTERFACE_QEUE_OVERFLOW_MESSAGE, LOG_INTERFACE_MAX_MESSAGE_LENGTH);
        
        LOG_FILE_append_line(new_line);
    }
}

// --------------------------------------------------------------------------------

void log_interface_init(void) {

    LOG_INTERFACE_STATUS_clear_all();

    LOG_QEUE_init();

    DEBUG_PASS("log_interface_init() - LOG_INTERFACE_CFG_COMPLETE_SLOT_connect()");
    LOG_INTERFACE_CFG_COMPLETE_SLOT_connect();

    DEBUG_PASS("log_interface_init() - LOG_INTERFACE_NEW_CFG_OBJECT_SLOT_connect()");
    LOG_INTERFACE_NEW_CFG_OBJECT_SLOT_connect();

    DEBUG_PASS("log_interface_init() - LOG_TASK_init()");
    LOG_TASK_init();
}

// --------------------------------------------------------------------------------

static void log_interface_task_init(void) {
    
    DEBUG_PASS("log_interface_task_init()");
    app_task_state = LOG_INTERFACE_TASK_STATE_START_UP;
}

static u16 log_interface_task_get_schedule_interval(void) {

    if (LOG_QEUE_is_empty() == 0) {
        return 0;
    }

    return LOG_INTERFACE_TASK_RUN_INTERVAL_MS;
}

static MCU_TASK_INTERFACE_TASK_STATE log_interface_task_get_state(void) {

    if (app_task_state == LOG_INTERFACE_TASK_STATE_SLEEP) {
        if (LOG_OP_TIMER_is_up(LOG_INTERFACE_SLEEPTIME) == 0) {
            return MCU_TASK_SLEEPING;
        }
    }

    if (app_task_state != LOG_INTERFACE_TASK_STATE_IDLE) {
        return MCU_TASK_RUNNING;
    }

    if (LOG_QEUE_is_empty() == 0) {
        DEBUG_PASS("log_interface_task_get_state() - Log-Messages pending");
        return MCU_TASK_RUNNING;
    }
    
    return MCU_TASK_SLEEPING;
}

static void log_interface_task_run(void) {
    
    //DEBUG_PASS("log_interface_task_run()");

    switch (app_task_state) {
        
        default:

            app_task_state = LOG_INTERFACE_TASK_STATE_START_UP;
            // no break;

        case LOG_INTERFACE_TASK_STATE_START_UP:

            if (LOG_INTERFACE_STATUS_is_set(LOG_INTERFACE_STATUS_USER_CFG_COMPLETE) == 0) {
                break;
            }
    
            app_task_state = LOG_INTERFACE_TASK_STATE_IDLE;
            break;

        case LOG_INTERFACE_TASK_STATE_IDLE :

            if (LOG_QEUE_is_empty()) {
                break;
            }

            DEBUG_PASS("log_interface_task_run() - LOG_INTERFACE_TASK_STATE_IDLE > LOG_INTERFACE_TASK_STATE_OPEN_FILE");
            app_task_state = LOG_INTERFACE_TASK_STATE_OPEN_FILE;
            // no break;

        case LOG_INTERFACE_TASK_STATE_CHECK_FILE :

            log_interface_update_file_name();

            break;

        case LOG_INTERFACE_TASK_STATE_OPEN_FILE :

            if (LOG_FILE_open() == 0) {
                DEBUG_PASS("log_interface_task_run() - Open log-file has FAILED !!! ---");
                DEBUG_PASS("log_interface_task_run() - LOG_INTERFACE_TASK_STATE_OPEN_FILE > LOG_INTERFACE_TASK_STATE_SLEEP");
                app_task_state = LOG_INTERFACE_TASK_STATE_SLEEP;
                LOG_OP_TIMER_start();
                break;
            }

            DEBUG_PASS("log_interface_task_run() - LOG_INTERFACE_TASK_STATE_OPEN_FILE > LOG_INTERFACE_TASK_STATE_LOAD_CONFIGURATION");

            app_task_state = LOG_INTERFACE_TASK_STATE_WRITE_LOG;
            // no break;
    

        case LOG_INTERFACE_TASK_STATE_WRITE_LOG :

            DEBUG_PASS("log_interface_task_run() - LOG_INTERFACE_TASK_STATE_LOAD_CONFIGURATION > LOG_INTERFACE_TASK_STATE_IDLE");
            
            log_interface_process_qeue();
            
            if (LOG_QEUE_is_empty()) {
                app_task_state = LOG_INTERFACE_TASK_STATE_CLOSE_FILE;
            }

            break;

        case LOG_INTERFACE_TASK_STATE_CLOSE_FILE:

            LOG_FILE_close();

            DEBUG_PASS("log_interface_task_run() - LOG_INTERFACE_TASK_STATE_LOAD_CONFIGURATION > LOG_INTERFACE_TASK_STATE_IDLE");
            app_task_state = LOG_INTERFACE_TASK_STATE_IDLE;

            break;

        case LOG_INTERFACE_TASK_STATE_SLEEP :

            if (LOG_OP_TIMER_is_up(LOG_INTERFACE_SLEEPTIME) == 0) {
                break;
            }

            DEBUG_PASS("log_interface_task_run() - LOG_INTERFACE_TASK_STATE_SLEEP > LOG_INTERFACE_TASK_STATE_IDLE");
            app_task_state = LOG_INTERFACE_TASK_STATE_IDLE;
    }

}

static void log_interface_task_terminate(void) {

}

// --------------------------------------------------------------------------------

static void log_interface_new_cfg_object_SLOT_CALLBACK(const void* p_argument) {

    if (LOG_INTERFACE_STATUS_is_set(LOG_INTERFACE_STATUS_USER_CFG_COMPLETE)) {
        DEBUG_PASS("log_interface_new_cfg_object_SLOT_CALLBACK() - Configuration cant be changed after start-up");
        return;
    }

    if (p_argument == NULL) {
        DEBUG_PASS("log_interface_new_cfg_object_SLOT_CALLBACK() - NULL-POINTER-EXCEPTION");
        return;
    }

    const CFG_FILE_PARSER_CFG_OBJECT_TYPE* p_cfg_object = (const CFG_FILE_PARSER_CFG_OBJECT_TYPE*) p_argument;

    if (common_tools_string_compare(LOG_INTERFACE_CFG_NAME_LOG_FILE_PATH, p_cfg_object->key)) {

        DEBUG_TRACE_STR(p_cfg_object->value, "log_interface_new_cfg_object_SLOT_CALLBACK() - LOG_FILE_PATH_CFG_NAME cfg-object");

        common_tools_string_copy_string(log_file_path, p_cfg_object->value, LOG_INTERFAC_MAX_LOG_FILE_PATH_LENGTH - 1);

        if (common_tools_string_ends_with(p_cfg_object->value, '/') == 0) {
            common_tools_string_append(log_file_path, "/", LOG_INTERFAC_MAX_LOG_FILE_PATH_LENGTH);
        }
    }
}

static void log_interface_cfg_complete_SLOT_CALLBACK(const void* p_argument) {
    LOG_INTERFACE_STATUS_set(LOG_INTERFACE_STATUS_USER_CFG_COMPLETE);
}

// --------------------------------------------------------------------------------

void log_message(const char* message) {

    if (LOG_QEUE_is_full()) {
        DEBUG_PASS("log_message() - The Qeue is full");
        LOG_INTERFACE_STATUS_set(LOG_INTERFACE_STATUS_QEUE_OVERFLOW);
        return;
    }

    char new_message[LOG_INTERFACE_MAX_MESSAGE_LENGTH];
    memset(new_message, '\0', LOG_INTERFACE_MAX_MESSAGE_LENGTH);
    
    common_tools_string_copy_string(new_message, message, LOG_INTERFACE_MAX_MESSAGE_LENGTH);

    LOG_QEUE_enqeue(new_message);

    DEBUG_TRACE_STR(new_message, "log_message() - New log-message: ");
}

void log_message_string(const char* message, const char* p_string) {

    if (LOG_QEUE_is_full()) {
        DEBUG_PASS("log_message() - The Qeue is full");
        LOG_INTERFACE_STATUS_set(LOG_INTERFACE_STATUS_QEUE_OVERFLOW);
        return;
    }

    char new_message[LOG_INTERFACE_MAX_MESSAGE_LENGTH];
    memset(new_message, '\0', LOG_INTERFACE_MAX_MESSAGE_LENGTH);

    common_tools_string_copy_string(new_message, message, LOG_INTERFACE_MAX_MESSAGE_LENGTH);
    common_tools_string_append(new_message, p_string, LOG_INTERFACE_MAX_MESSAGE_LENGTH);

    log_message(new_message);
}
