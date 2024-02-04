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
 * @file    mcu_top.c
 * @author  Sebastian Lesse
 * @date    2023 / 10 / 29
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

#include "signal_slot_interface.h"
#include "iterator_interface.h"
#include "mcu_task_interface.h"

#include "protocol_management/json/protocol_json_parser.h"
#include "protocol_management/mqtt/mqtt_interface.h"

#include "common/local_module_status.h"
#include "common/common_tools_string.h"

#include "time_management.h"
#include "modules/cfg_file_parser/cfg_file_parser.h"
#include "ui/file_interface/file_interface.h"

#include "ui/console/ui_console.h"

#include "mcu_top.h"

// --------------------------------------------------------------------------------

/**
 * @brief If there is no configuration available the
 * default schedule interval (2000ms) will be set.
 */
#define MCU_TOP_SCHEDULE_INTERVAL_DEFAULT_MS            2000

/**
 * @brief The interval at which the task is scheduled.
 * This is not the schedule interval used for operation
 */
#define MCU_TOP_TASK_SCHEDULE_INTERFAL                  100

/**
 * @brief Minimum schedule interval in milliseconds
 * is 500 ms
 */
#define MCU_TOP_MIN_TASK_SCHEDULE_INTERVAL              500

/**
 * @brief Maximum number of characters of a single line for out
 * on console and/or file
 */
#define MCU_TOP_MAX_OUTPUT_LINE_LENGTH                  255

/**
 * @brief Maximum number of kBytes of the output file
 */
#define MCU_TOP_OUTPUT_FILE_SIZE_MAX                    10000

/**
 * @brief Minimum number of kBytes of the output file
 */
#define MCU_TOP_OUTPUT_FILE_SIZE_MIN                    100

// --------------------------------------------------------------------------------

#define MCU_TOP_MQTT_GROUP_NAME                         "MCU_TOP"
#define MCU_TOP_MQTT_TASKS_GROUP_NAME                   "TASKS"
#define MCU_TOP_MQTT_NAME_TASK_LOAD_SUM                 "TASK_LOAD_SUM"

// --------------------------------------------------------------------------------

/**
 * @brief Maximum number of task the MCU-TOP module can handle.
 */
#ifndef MCU_TOP_MAX_NUMBER_OF_TASK
#define MCU_TOP_MAX_NUMBER_OF_TASK                      10
#endif

// --------------------------------------------------------------------------------

/**
 * @brief States of the MCU-TOP state-machine
 */
typedef enum {
    MCU_TOP_STATE_SETUP,
    MCU_TOP_STATE_DISABLED,
    MCU_TOP_STATE_IDLE,
    MCU_TOP_STATE_COLLECT_STATS,
    MCU_TOP_STATE_COMPUTE_VALUES,
    MCU_TOP_STATE_WRITE_VALUES
} MCU_TOP_STATES;

// --------------------------------------------------------------------------------

typedef struct {

    /**
     * @brief The cpu-load caused by a task.
     * 
     */
    u8 id;

    /**
     * @brief The overall runtime of a task given by the task-controller
     */
    u64 runtime_overall;

    /**
     * @brief The interval runtime of a task given by the task-controller
     * of the last schedule interval of the mcu-top module
     */
    u64 runtime_interval;

    /**
     * @brief The cpu-load caused by a task.
     * 
     */
    u8 cpu_load;

    /**
     * @brief Name information of a task
     * - length of name
     * - name as character array
     */
    u8 name_length;
    const char* p_name;

} MCU_TOP_TASK_STATS;

// --------------------------------------------------------------------------------

#define MCU_TOP_STATUS_CFG_COMPLETE                                         (1 << 0)
#define MCU_TOP_STATUS_NEW_CFG_VALUE                                        (1 << 1)
#define MCU_TOP_STATUS_CONSOLE_OUTPUT                                       (1 << 2)
#define MCU_TOP_STATUS_IS_ENABLED                                           (1 << 3)
#define MCU_TOP_STATUS_FILE_OUTPUT                                          (1 << 4)
#define MCU_TOP_STATUS_MQTT_OUTPUT                                          (1 << 5)

BUILD_MODULE_STATUS_U8(MCU_TOP_STATUS)

/**
 * @brief This is a convinience macro only. It can be used to chek if
 * the MCU_TOP_STATUS_CFG_COMPLETE and/or the MCU_TOP_STATUS_NEW_CFG_VALUE is set.
 * Do not use this macro to access a particualr status.
 */
#define MCU_TOP_STATUS_SETUP_PENDING   (MCU_TOP_STATUS_CFG_COMPLETE | MCU_TOP_STATUS_NEW_CFG_VALUE)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(MCU_TOP_TIMER)

// --------------------------------------------------------------------------------

FILE_INTERFACE_CREATE_FILE(MCU_TOP_OUTPUT_FILE)

// --------------------------------------------------------------------------------

JSON_PARSER_CREATE_OBJECT(MCU_TOP_JSON_OBJ)

// --------------------------------------------------------------------------------

/**
 * @brief If a new file names is configured, it will be remembered using this variable.
 * The mcu-top state machine detects a new value in the IDLE state via the
 * MCU_TOP_STATUS_NEW_CFG_VALUE. The new name is set in the SETUP state.
 */
static char mcu_top_new_file_name[FILE_PATH_MAX_STRING_LENGTH];

/**
 * @brief If a new schedule interval is configured, it will be remembered using this variable.
 * The mcu-top state machine detects a new value in the IDLE state via the
 * MCU_TOP_STATUS_NEW_CFG_VALUE. The new name is set in the SETUP state.
 */
static u16 mcu_top_new_schedule_interval = 0;

/**
 * @brief Callback to handle the CFG_PARSER_NEW_CFG_OBJECT_SIGNAL event in mcu_top
 * 
 * @param argument - const reference to an temporary instance of CFG_FILE_PARSER_CFG_OBJECT_TYPE
 */
static void mcu_top_new_cfg_object_SLOT_CALLBACK(const void* p_argument) {

    if (p_argument == NULL) {
        DEBUG_PASS("mcu_top_new_cfg_object_SLOT_CALLBACK() - NULL-POINTER-EXCEPTION");
        return;
    }

    const CFG_FILE_PARSER_CFG_OBJECT_TYPE* p_cfg_object = (const CFG_FILE_PARSER_CFG_OBJECT_TYPE*) p_argument;

    // check and apply new schedule interval
    if (common_tools_string_compare(MCU_TOP_CFG_NAME_SCHEDULE_INTERVAL, p_cfg_object->key)) {

        DEBUG_TRACE_STR(
            p_cfg_object->value,
            "mcu_top_new_cfg_object_SLOT_CALLBACK() - new value of MCU_TOP_SCHEDULE_INTERVAL"
        );

        if ( ! common_tools_sting_is_number(p_cfg_object->value) ){
            mcu_top_new_schedule_interval = MCU_TOP_SCHEDULE_INTERVAL_DEFAULT_MS;
            DEBUG_PASS("mcu_top_new_cfg_object_SLOT_CALLBACK() - not a number - apply default value");
            return;
        }

        mcu_top_new_schedule_interval = common_tools_string_to_u16(p_cfg_object->value);

        if (mcu_top_new_schedule_interval < MCU_TOP_MIN_TASK_SCHEDULE_INTERVAL) {
            mcu_top_new_schedule_interval = MCU_TOP_MIN_TASK_SCHEDULE_INTERVAL;
            DEBUG_PASS("mcu_top_new_cfg_object_SLOT_CALLBACK() - value to low - apply minimum value");
        }

        return;
    }

    // check and apply new output file
    if (common_tools_string_compare(MCU_TOP_CFG_NAME_OUTPUT_FILE, p_cfg_object->key)) {

        DEBUG_TRACE_STR(
            p_cfg_object->value,
            "mcu_top_new_cfg_object_SLOT_CALLBACK() - new value of MCU_TOP_OUTPUT_FILE"
        );

        common_tools_string_copy_string(
            mcu_top_new_file_name,
            p_cfg_object->value,
            sizeof(mcu_top_new_file_name)
        );

        MCU_TOP_STATUS_set(MCU_TOP_STATUS_FILE_OUTPUT);
    }

    // check and apply mqtt output
    if (common_tools_string_compare(MCU_TOP_CFG_NAME_OUTPUT_MQTT, p_cfg_object->key)) {

        DEBUG_TRACE_STR(
            p_cfg_object->value,
            "mcu_top_new_cfg_object_SLOT_CALLBACK() - new value of MCU_TOP_OUTPUT_MQTT"
        );

        if ( ! common_tools_sting_is_number(p_cfg_object->value) ){
            DEBUG_PASS("mcu_top_new_cfg_object_SLOT_CALLBACK() - not a number - apply default value");
            MCU_TOP_STATUS_unset(MCU_TOP_STATUS_CONSOLE_OUTPUT);
            return;
        }

        if (common_tools_string_to_u32(p_cfg_object->value) == 1u) {
            MCU_TOP_STATUS_set(MCU_TOP_STATUS_MQTT_OUTPUT);
            DEBUG_PASS("mcu_top_new_cfg_object_SLOT_CALLBACK() - mqtt output activated");

        } else {
            MCU_TOP_STATUS_unset(MCU_TOP_STATUS_MQTT_OUTPUT);
            DEBUG_PASS("mcu_top_new_cfg_object_SLOT_CALLBACK() - mqtt output deactivated");
        }
    }

    // check and apply console output
    if (common_tools_string_compare(MCU_TOP_CFG_NAME_OUTPUT_CONSOLE, p_cfg_object->key)) {

        DEBUG_TRACE_STR(
            p_cfg_object->value,
            "mcu_top_new_cfg_object_SLOT_CALLBACK() - new value of MCU_TOP_OUTPUT_CONSOLE"
        );

        if ( ! common_tools_sting_is_number(p_cfg_object->value) ){
            DEBUG_PASS("mcu_top_new_cfg_object_SLOT_CALLBACK() - not a number - apply default value");
            MCU_TOP_STATUS_unset(MCU_TOP_STATUS_CONSOLE_OUTPUT);
            return;
        }

        if (common_tools_string_to_u32(p_cfg_object->value) == 1u) {
            MCU_TOP_STATUS_set(MCU_TOP_STATUS_CONSOLE_OUTPUT);
            DEBUG_PASS("mcu_top_new_cfg_object_SLOT_CALLBACK() - console output activated");

        } else {
            MCU_TOP_STATUS_unset(MCU_TOP_STATUS_CONSOLE_OUTPUT);
            DEBUG_PASS("mcu_top_new_cfg_object_SLOT_CALLBACK() - console output deactivated");
        }
    }

    // check for relevant configuration data
    // set status flag only on relevant configuration data
    MCU_TOP_STATUS_set(MCU_TOP_STATUS_NEW_CFG_VALUE);
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(
    CFG_PARSER_NEW_CFG_OBJECT_SIGNAL,
    MCU_TOP_CFG_VALUE_SLOT,
    mcu_top_new_cfg_object_SLOT_CALLBACK
)

/**
 * @brief Callback to handle the CFG_PARSER_CFG_COMPLETE_SIGNAL event in mcu_top
 * 
 * @param argument - NULL
 */
static void MCU_TOP_CFG_COMPLETE_SLOT_CALLBACK(const void* p_argument) {
    DEBUG_PASS("MCU_TOP_CFG_COMPLETE_SLOT_CALLBACK()");
    MCU_TOP_STATUS_set(MCU_TOP_STATUS_CFG_COMPLETE);
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(
    CFG_PARSER_CFG_COMPLETE_SIGNAL,
    MCU_TOP_CFG_COMPLETE_SLOT,
    MCU_TOP_CFG_COMPLETE_SLOT_CALLBACK
)

// --------------------------------------------------------------------------------

/**
 * @brief The schedule itnerval at which the task statistics are read.
 */
static u16 mcu_top_schedule_interval = 0;

/**
 * @brief The current state of the MCU-TOP state machine
 */
static MCU_TOP_STATES mcu_top_current_state = MCU_TOP_STATE_SETUP;

/**
 * @brief Temporare storage of the current statistics of all available tasks.
 * Remember to adjust MCU_TOP_MAX_NUMBER_OF_TASK in your config.h
 */
static MCU_TOP_TASK_STATS task_stats_array[MCU_TOP_MAX_NUMBER_OF_TASK];

/**
 * @brief The summary of the task runtime of the current schedule interval.
 */
static u32 task_runtime_interval = 0;

// --------------------------------------------------------------------------------

/**
 * @brief Applys the new configuration that was received via 
 * CFG_PARSER_NEW_CFG_OBJECT_SIGNAL.
 */
static void mcu_top_apply_new_configuration(void) {
    DEBUG_PASS("mcu_top_apply_new_configuration()");

    mcu_top_schedule_interval = mcu_top_new_schedule_interval;
    MCU_TOP_OUTPUT_FILE_set_path(mcu_top_new_file_name);
}

/**
 * @brief Resets the current statistics.
 */
static void mcu_top_reset_statistics(void) {
    DEBUG_PASS("mcu_top_reset_statistics()"); 

    u8 index = 0;

    ITERATE_FOR(

        TASK_CTRL_STATS,
        task_stats,
        MCU_TASK_ITERATOR_INTERFACE_iter(),

        if (index < MCU_TOP_MAX_NUMBER_OF_TASK) {
            task_stats_array[index].id = task_stats.id;
            task_stats_array[index].runtime_overall = 0;
            task_stats_array[index].runtime_interval = 0;
            task_stats_array[index].cpu_load = 0;
            task_stats_array[index].name_length = task_stats.name_length;
            task_stats_array[index].p_name = task_stats.p_name;

            DEBUG_TRACE_STR(task_stats_array[index].p_name, "mcu_top_reset_statistics() - Task:");
        }

        index += 1;
    )
}

/**
 * @brief Iterates over all tasks and get their statistics.
 * The values are stored temporarily.
 */
static void mcu_top_collect_statistics(void) {

    DEBUG_PASS("mcu_top_collect_statistics()");

    task_runtime_interval = 0;

    u8 index = 0;
    ITERATE_FOR(
        TASK_CTRL_STATS,
        task_stats,
        MCU_TASK_ITERATOR_INTERFACE_iter(),

        if (index < MCU_TOP_MAX_NUMBER_OF_TASK) {

            task_stats_array[index].runtime_interval = task_stats.runtime - task_stats_array[index].runtime_overall;
            task_stats_array[index].runtime_overall = task_stats.runtime;

            DEBUG_TRACE_STR(task_stats_array[index].p_name, "mcu_top_reset_statistics() - Task:");
            DEBUG_TRACE_long(task_stats_array[index].runtime_interval, "mcu_top_reset_statistics() - Runtime:");

            task_runtime_interval += task_stats_array[index].runtime_interval;
        }

        index += 1;
    )

    DEBUG_TRACE_long(task_runtime_interval, "mcu_top_reset_statistics() - Overall-Runtime:");
}

/**
 * @brief Processes the collected values and
 * generates detailed information from it.
 */
static void mcu_top_compute_values(void) {
    DEBUG_TRACE_long(mcu_top_schedule_interval, "mcu_top_compute_values() - Interval:");

    for (u8 index = 0; index < mcu_task_controller_task_count(); index += 1) {

        if (index >= MCU_TOP_MAX_NUMBER_OF_TASK) {
            break;
        }

        task_stats_array[index].cpu_load = (task_stats_array[index].runtime_interval * 100) / (task_runtime_interval);

        DEBUG_TRACE_STR(task_stats_array[index].p_name, "Task:");
        DEBUG_TRACE_long(task_stats_array[index].cpu_load, "mcu_top_reset_statistics() - Load:");
    }
}

/**
 * @brief Writes the computed values into a file, if configured.
 * Or on the console, if activated.
 */
static void mcu_top_write_values(void) {

    DEBUG_PASS("mcu_top_write_values()");

    const char header[] = "\n| Task-Name | Runtime[us] | Load[%] |\n"
                            "|-----------|-------------|---------|";

    if (MCU_TOP_STATUS_is_set(MCU_TOP_STATUS_CONSOLE_OUTPUT)){
        console_write_line(header);
    }

    u8 file_existing = 0;
    if (MCU_TOP_STATUS_is_set(MCU_TOP_STATUS_FILE_OUTPUT)) {

        file_existing = MCU_TOP_OUTPUT_FILE_is_existing();
        if (file_existing == 0) {

            file_existing = MCU_TOP_OUTPUT_FILE_create();
            if ( file_existing == 0 ) {
                DEBUG_PASS("mcu_top_write_values() - create output file FAILED !");
            }
        }

        if (file_existing != 0) {
            MCU_TOP_OUTPUT_FILE_append_line(header);
        }
    }

    if (MCU_TOP_STATUS_is_set(MCU_TOP_STATUS_MQTT_OUTPUT)) {
        MCU_TOP_JSON_OBJ_start_group(MCU_TOP_MQTT_GROUP_NAME);
        MCU_TOP_JSON_OBJ_start_group(MCU_TOP_MQTT_TASKS_GROUP_NAME);
    }

    // summarize the load of all task, except the idle-task
    u32 task_load = 0;

    for (u8 index = 0; index < mcu_task_controller_task_count(); index += 1) {

        if (index >= MCU_TOP_MAX_NUMBER_OF_TASK) {
            break;
        }

        char line[MCU_TOP_MAX_OUTPUT_LINE_LENGTH];
        line[0] = '\0';

        common_tools_string_append(line, "| ", MCU_TOP_MAX_OUTPUT_LINE_LENGTH);
        common_tools_string_append(line, task_stats_array[index].p_name, MCU_TOP_MAX_OUTPUT_LINE_LENGTH);
        common_tools_string_append(line, " | ", MCU_TOP_MAX_OUTPUT_LINE_LENGTH);
        common_tools_string_append_number(line, task_stats_array[index].runtime_interval, MCU_TOP_MAX_OUTPUT_LINE_LENGTH);
        common_tools_string_append(line, " | ", MCU_TOP_MAX_OUTPUT_LINE_LENGTH);
        common_tools_string_append_number(line, task_stats_array[index].cpu_load, MCU_TOP_MAX_OUTPUT_LINE_LENGTH);
        common_tools_string_append(line, " |", MCU_TOP_MAX_OUTPUT_LINE_LENGTH);

        
        if (MCU_TOP_STATUS_is_set(MCU_TOP_STATUS_CONSOLE_OUTPUT)) {
            console_write_line(line);
        }

        if (file_existing != 0) {
            MCU_TOP_OUTPUT_FILE_append_line(line);
        }

        if (MCU_TOP_STATUS_is_set(MCU_TOP_STATUS_MQTT_OUTPUT)) {
            MCU_TOP_JSON_OBJ_add_integer(
                task_stats_array[index].p_name,
                task_stats_array[index].cpu_load
            );
        }

        if (index != 0) {
            // we ignore the idle-task here.
            // The idle-task is always the first task in the list.
            task_load += task_stats_array[index].cpu_load;
        }
    }

    if (MCU_TOP_STATUS_is_set(MCU_TOP_STATUS_MQTT_OUTPUT)) {
        MCU_TOP_JSON_OBJ_end_group(); // MCU_TOP_MQTT_TASKS_GROUP_NAME
        MCU_TOP_JSON_OBJ_add_integer(
            MCU_TOP_MQTT_NAME_TASK_LOAD_SUM,
            task_load
        );
        MCU_TOP_JSON_OBJ_finish();
        MQTT_MESSAGE_TO_SEND_SIGNAL_send(MCU_TOP_JSON_OBJ_to_string());
    }
}

// --------------------------------------------------------------------------------

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.start
 */
static void MCU_TOP_TASK_start(void) {
    DEBUG_PASS("MCU_TOP_TASK_start()");
    MCU_TOP_TIMER_start();
    mcu_top_current_state = MCU_TOP_STATE_SETUP;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_schedule_interval
 */
static u16 MCU_TOP_TASK_get_schedule_interval(void) {
    return MCU_TOP_TASK_SCHEDULE_INTERFAL;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_state
 */
static MCU_TASK_INTERFACE_TASK_STATE MCU_TOP_task_get_state(void) {

    if (mcu_top_current_state != MCU_TOP_STATE_IDLE) {
        return MCU_TASK_RUNNING;
    }

    if (MCU_TOP_STATUS_is_set(MCU_TOP_STATUS_SETUP_PENDING)) {
        return MCU_TASK_RUNNING;
    }

    if (MCU_TOP_TIMER_is_up(mcu_top_schedule_interval)) {
        return MCU_TASK_RUNNING;
    }

    return MCU_TASK_SLEEPING;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.run
 */
static void MCU_TOP_TASK_execute(void) {

    switch (mcu_top_current_state) {

        default :
            mcu_top_current_state = MCU_TOP_STATE_IDLE;
            break;

        case MCU_TOP_STATE_SETUP :

            if (MCU_TOP_STATUS_is_set(MCU_TOP_STATUS_NEW_CFG_VALUE)) {
                mcu_top_apply_new_configuration();
                MCU_TOP_STATUS_unset(MCU_TOP_STATUS_NEW_CFG_VALUE);
            }

            if (MCU_TOP_STATUS_is_set(MCU_TOP_STATUS_CFG_COMPLETE)) {
                MCU_TOP_STATUS_unset(MCU_TOP_STATUS_CFG_COMPLETE);
                DEBUG_PASS("MCU_TOP_TASK_execute() - MCU_TOP_STATE_SETUP -> MCU_TOP_STATE_DISABLED");
                mcu_top_current_state = MCU_TOP_STATE_DISABLED;
            }

            break;

        case MCU_TOP_STATE_DISABLED :

            if (MCU_TOP_STATUS_is_set(MCU_TOP_STATUS_IS_ENABLED) == 0) {
                break;
            }

            mcu_top_reset_statistics();

            DEBUG_PASS("MCU_TOP_TASK_execute() - MCU_TOP_STATE_DISABLED -> MCU_TOP_STATE_IDLE");
            mcu_top_current_state = MCU_TOP_STATE_IDLE;

            // no break
            // fall through

        case MCU_TOP_STATE_IDLE :

            if (MCU_TOP_STATUS_is_set(MCU_TOP_STATUS_IS_ENABLED) == 0) {
                DEBUG_PASS("MCU_TOP_TASK_execute() - MCU_TOP_STATE_IDLE -> MCU_TOP_STATE_DISABLED");
                mcu_top_current_state = MCU_TOP_STATE_DISABLED;
            }

            if (MCU_TOP_STATUS_is_set(MCU_TOP_STATUS_NEW_CFG_VALUE)) {

                MCU_TOP_TIMER_stop();
                mcu_top_current_state = MCU_TOP_STATE_SETUP;

                DEBUG_PASS(
                    "MCU_TOP_TASK_execute()"
                    " - MCU_TOP_STATE_IDLE -> MCU_TOP_STATE_SETUP"
                );

                break;
            }

            if (MCU_TOP_TIMER_is_up(mcu_top_schedule_interval)) {
                MCU_TOP_TIMER_start();

                mcu_top_current_state = MCU_TOP_STATE_COLLECT_STATS;

                DEBUG_TRACE_long(
                    time_mgmnt_gettime_u32(),
                    "MCU_TOP_TASK_execute()"
                    " - MCU_TOP_STATE_IDLE -> MCU_TOP_STATE_COLLECT_STATS"
                );
                
                break;
            }

            if ( ! MCU_TOP_TIMER_is_active()) {
                MCU_TOP_TIMER_start();
            }

            break;

        case MCU_TOP_STATE_COLLECT_STATS :

            mcu_top_collect_statistics();
            mcu_top_current_state = MCU_TOP_STATE_COMPUTE_VALUES;

            DEBUG_PASS("MCU_TOP_TASK_execute() - MCU_TOP_STATE_COLLECT_STATS -> MCU_TOP_STATE_COMPUTE_VALUES");
            break;

        case MCU_TOP_STATE_COMPUTE_VALUES :

            mcu_top_compute_values();
            mcu_top_current_state = MCU_TOP_STATE_WRITE_VALUES;

            DEBUG_PASS("MCU_TOP_TASK_execute() - MCU_TOP_STATE_COMPUTE_VALUES -> MCU_TOP_STATE_WRITE_VALUES");
            break;

        case MCU_TOP_STATE_WRITE_VALUES :

            mcu_top_write_values();
            mcu_top_current_state = MCU_TOP_STATE_IDLE;

            DEBUG_PASS("MCU_TOP_TASK_execute() - MCU_TOP_STATE_WRITE_VALUES -> MCU_TOP_STATE_IDLE");
            break;
    }
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.terminate
 */
static void MCU_TOP_TASK_terminate(void) {

}

// --------------------------------------------------------------------------------

TASK_CREATE (
   MCU_TOP_TASK,
   TASK_PRIORITY_MIDDLE,
   MCU_TOP_TASK_get_schedule_interval,
   MCU_TOP_TASK_start,
   MCU_TOP_TASK_execute,
   MCU_TOP_task_get_state,
   MCU_TOP_TASK_terminate
)

// --------------------------------------------------------------------------------

/**
 * @see mcu_top.h#mcu_top_init
 */
void mcu_top_init(void) {

    DEBUG_PASS("mcu_top_init()");

    mcu_top_schedule_interval = MCU_TOP_SCHEDULE_INTERVAL_DEFAULT_MS;
    mcu_top_new_schedule_interval = MCU_TOP_SCHEDULE_INTERVAL_DEFAULT_MS;

    MCU_TOP_CFG_VALUE_SLOT_connect();
    MCU_TOP_CFG_COMPLETE_SLOT_connect();

    MCU_TOP_JSON_OBJ_initialize();

    MCU_TOP_TIMER_stop();
    MCU_TOP_TASK_init();
}

// --------------------------------------------------------------------------------

/**
 * @see mcu_top.h#mcu_top_set_enabled
 */
void mcu_top_set_enabled(u8 enabled) {
    if (enabled) {
        MCU_TOP_STATUS_set(MCU_TOP_STATUS_IS_ENABLED);
        mcu_task_controller_enable_statistics(TASK_CTRL_STATISTIC_ON);
        DEBUG_PASS("mcu_top_set_enabled() - ENABLE");
    } else {
        MCU_TOP_STATUS_unset(MCU_TOP_STATUS_IS_ENABLED);
        mcu_task_controller_enable_statistics(TASK_CTRL_STATISTIC_OFF);
        DEBUG_PASS("mcu_top_set_enabled() - DISABLE");
    }
}

// --------------------------------------------------------------------------------
