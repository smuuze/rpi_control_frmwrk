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
 * @file    rpi_protocol_handler_host.c
 * @author  Sebastian Lesse
 * @date    2023 / 08 / 05
 * @brief   Implementation of the RPI-Protocol for Host
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

#include "system/system_interface.h"
#include "common/common_types.h"

#include "common/local_msg_buffer.h"
#include "common/local_module_status.h"
#include "common/local_mutex.h"
#include "common/common_tools_string.h"

#include "io_management/io_controller.h"

#include "driver/trx_driver_interface.h"
#include "driver/cfg_driver_interface.h"
#include "driver/driver_specific_spi.h"

#include "command_management/protocol_interface.h"
#include "time_management/time_management.h"
#include "common/signal_slot_interface.h"
#include "modules/cfg_file_parser/cfg_file_parser.h"

#include "protocol_management/rpi_protocol_handler.h"

// --------------------------------------------------------------------------------

#ifndef RPI_PROTOCOL_HANDLER_SCHEDULE_INTERVAL_MS
#define RPI_PROTOCOL_HANDLER_SCHEDULE_INTERVAL_MS                   5
#endif

#ifndef RPI_PROTOCOL_HANDLER_WAIT_FOR_CFG_SCHEDULE_INTERVAL_MS
#define RPI_PROTOCOL_HANDLER_WAIT_FOR_CFG_SCHEDULE_INTERVAL_MS      30
#endif

#ifndef RPI_HOST_COMMAND_BUFFER_SIZE
#define RPI_HOST_COMMAND_BUFFER_SIZE                                128
#endif

#ifndef RPI_HOST_RESPONSE_BUFFER_SIZE
#define RPI_HOST_RESPONSE_BUFFER_SIZE                               128
#endif

#ifndef RPI_PROTOCOL_HOST_CLIENT_REQUEST_TIME_MS
#define RPI_PROTOCOL_HOST_CLIENT_REQUEST_TIME_MS                    64
#endif

#ifndef RPI_PROTOCOL_HOST_CLIENT_WAIT_TIMEOUT_MS
#define RPI_PROTOCOL_HOST_CLIENT_WAIT_TIMEOUT_MS                    1505
#endif

#ifndef RPI_PROTOCOL_CLIENT_POLL_INTERVAL_MS
#define RPI_PROTOCOL_CLIENT_POLL_INTERVAL_MS                        50
#endif

// --------------------------------------------------------------------------------

/**
 * @brief 
 */
typedef enum {
    RPI_HOST_WAIT_FOR_USER_CFG,
    RPI_HOST_STATE_SLEEP,
    RPI_HOST_STATE_CLIENT_WAIT_FOR_COMMAND,
    RPI_HOST_STATE_REQUEST_CLIENT,
    RPI_HOST_STATE_WAIT_FOR_CLIENT,
    RPI_HOST_STATE_SEND_COMMAND,
    RPI_HOST_STATE_CLIENT_WAIT_FOR_RESPONSE,
    RPI_HOST_STATE_CLIENT_REQUEST_FOR_RESPONSE,
    RPI_HOST_STATE_RECEIVE_RESPONSE,
    RPI_HOST_STATE_FINISH
} RPI_HOST_STATE_TYPE;

// --------------------------------------------------------------------------------

#define RPI_HOST_STATUS_COMMAND_PENDING                             (1 << 0)
#define RPI_HOST_STATUS_USER_CONFIG_COMPLETE                        (1 << 1)

BUILD_MODULE_STATUS_U8(RPI_HOST_STATUS)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(RPI_OP_TIMER)

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_HOST_COMMAND_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_HOST_RESPONSE_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_HOST_RESPONSE_TIMEOUT_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_HOST_RESPONSE_OVERFLOW_SIGNAL)

// --------------------------------------------------------------------------------

BUILD_LOCAL_MSG_BUFFER( , RPI_HOST_COMMAND_BUFFER,  RPI_HOST_COMMAND_BUFFER_SIZE)
BUILD_LOCAL_MSG_BUFFER( , RPI_HOST_RESPONSE_BUFFER, RPI_HOST_RESPONSE_BUFFER_SIZE)

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
static RPI_HOST_STATE_TYPE rpi_host_state = RPI_HOST_STATE_SLEEP;

/**
 * @brief 
 * 
 */
static MCU_TASK_INTERFACE_TASK_STATE actual_task_state = MCU_TASK_UNINITIALIZED;

/**
 * @brief 
 * 
 */
static TRX_DRIVER_INTERFACE* p_com_driver = 0;

/**
 * @brief 
 * 
 */
static TRX_DRIVER_CONFIGURATION _com_driver_cfg_spi = {
    .module.spi = {
        RPI_PROTOCOL_HANDLER_DRIVER_CFG
    }
};

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param p_argument 
 */
static void rpi_protocol_handler_host_COMMAND_RECEIVED_SLOT_CALLBACK(const void* p_argument) {

    const COMMON_GENERIC_BUFFER_TYPE* p_buffer = (const COMMON_GENERIC_BUFFER_TYPE*) p_argument;
    DEBUG_TRACE_N(p_buffer->length, p_buffer->data, "rpi_protocol_handler_host_COMMAND_RECEIVED_SLOT_CALLBACK()");

    RPI_HOST_COMMAND_BUFFER_start_write();
    RPI_HOST_COMMAND_BUFFER_add_N_bytes(p_buffer->length, p_buffer->data);
    RPI_HOST_COMMAND_BUFFER_stop_write();

    RPI_HOST_STATUS_set(RPI_HOST_STATUS_COMMAND_PENDING);
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(
    RPI_HOST_COMMAND_RECEIVED_SIGNAL,
    RPI_HOST_COMMAND_RECEIVED_SLOT,
    rpi_protocol_handler_host_COMMAND_RECEIVED_SLOT_CALLBACK
)

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param p_argument 
 */
static void rpi_protocol_new_cfg_object_CALLBACK(const void* p_argument) {

    if (p_argument == NULL) {
        DEBUG_PASS("rpi_protocol_new_cfg_object_CALLBACK() - NULL_POINTER_EXCEPTION");
        return;
    }

    CFG_FILE_PARSER_CFG_OBJECT_TYPE* p_cfg_object = (CFG_FILE_PARSER_CFG_OBJECT_TYPE*) p_argument;

    if (common_tools_string_compare(RPI_CONTROL_PREFIX_CFG_STRING, p_cfg_object->key)) {
        DEBUG_TRACE_STR(p_cfg_object->key, "rpi_protocol_new_cfg_object_CALLBACK() - Unknown cfg_object - prefix");
        return;
    }

    if (common_tools_string_compare(RPI_PROTOCOL_SPEED_CFG_STRING, p_cfg_object->key)) {
        
        DEBUG_TRACE_STR(p_cfg_object->value, "rpi_protocol_new_cfg_object_CALLBACK() - RPI_PROTOCOL_SPEED_CFG_STRING :");
        _com_driver_cfg_spi.module.spi.speed = common_tools_string_to_u32(p_cfg_object->value);
        return;
    }

    #ifdef HAS_DRIVER_DEVICE_NAME_SUPPORT
    if (common_tools_string_compare(RPI_PROTOCOL_DEVICE_CFG_STRING, p_cfg_object->key)) {
        
        DEBUG_TRACE_STR(p_cfg_object->value, "rpi_protocol_new_cfg_object_CALLBACK() - RPI_PROTOCOL_DEVICE_CFG_STRING :");
        common_tools_string_clear(_com_driver_cfg_spi.device.name, DRIVER_CFG_DEVICE_NAME_MAX_LENGTH);
        common_tools_string_copy_string(_com_driver_cfg_spi.device.name, p_cfg_object->value, DRIVER_CFG_DEVICE_NAME_MAX_LENGTH);
        return;
    }
    #endif // HAS_DRIVER_DEVICE_NAME_SUPPORT

    DEBUG_TRACE_STR(p_cfg_object->key, "rpi_protocol_new_cfg_object_CALLBACK() - Unknown cfg_object");
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(
    CFG_PARSER_NEW_CFG_OBJECT_SIGNAL,
    RPI_HOST_NEW_CFG_OBJECT_SLOT,
    rpi_protocol_new_cfg_object_CALLBACK
)

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param p_argument 
 */
static void rpi_protocol_cfg_complete_CALLBACK(const void* p_argument) {

    (void) p_argument;

    DEBUG_PASS("rpi_protocol_cfg_complete_CALLBACK()");
    RPI_HOST_STATUS_set(RPI_HOST_STATUS_USER_CONFIG_COMPLETE);
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(
    CFG_PARSER_CFG_COMPLETE_SIGNAL,
    RPI_HOST_CFG_COMPLETE_SLOT,
    rpi_protocol_cfg_complete_CALLBACK
)

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
static void rpi_host_send_command(void) {

    //DEBUG_PASS("rpi_host_send_command()");

    u16 num_bytes = RPI_HOST_COMMAND_BUFFER_bytes_available();
    u8 p_buffer[RPI_HOST_COMMAND_BUFFER_SIZE];
    
    RPI_HOST_COMMAND_BUFFER_start_read();
    RPI_HOST_COMMAND_BUFFER_get_N_bytes(num_bytes, p_buffer);
    RPI_HOST_COMMAND_BUFFER_stop_read();

    p_com_driver->clear_rx_buffer();
    p_com_driver->set_N_bytes(num_bytes, p_buffer);
    
    p_com_driver->start_tx();
}

/**
 * @brief 
 * 
 * @return u8 
 */
static u8 rpi_host_receive_response(void) {

    p_com_driver->start_rx(1);

    if (p_com_driver->bytes_available() == 0) {
        //DEBUG_PASS("rpi_host_receive_response() - No data received");
        return 0;
    }

    COMMON_GENERIC_BUFFER_TYPE buffer;
    buffer.length = 0;

    p_com_driver->get_N_bytes(1, (u8*)&buffer.length);

    if (buffer.length == 0) {
        //DEBUG_PASS("rpi_host_receive_response() - Received Length is zero");
        return 0;
    }

    if (buffer.length > COMMON_TYPES_GENERIC_BUFFER_SIZE) {

        DEBUG_TRACE_word(buffer.length, "rpi_host_receive_response() - Length exeeds buffer-limit");

        buffer.length = COMMON_TYPES_GENERIC_BUFFER_SIZE;
        RPI_HOST_RESPONSE_OVERFLOW_SIGNAL_send(NULL);

        // we will accept as much as possible bytes
    }

    p_com_driver->start_rx(buffer.length);
    p_com_driver->get_N_bytes(buffer.length, buffer.data);
            
    RPI_HOST_RESPONSE_RECEIVED_SIGNAL_send((void*)&buffer);

    return 1;
}

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
static void rpi_protocol_task_start(void) {

    DEBUG_PASS("rpi_protocol_task_start() - HOST - ");

    actual_task_state = MCU_TASK_SLEEPING;
    rpi_host_state = RPI_HOST_WAIT_FOR_USER_CFG;

    RPI_OP_TIMER_start();
}

/**
 * @brief 
 * 
 * @return u16 
 */
static u16 rpi_protocol_task_get_schedule_interval(void) {

    if (actual_task_state != MCU_TASK_SLEEPING) {
        return 0;
    } else {
        return RPI_PROTOCOL_HANDLER_SCHEDULE_INTERVAL_MS;
    }
}

/**
 * @brief 
 * 
 * @return MCU_TASK_INTERFACE_TASK_STATE 
 */
static MCU_TASK_INTERFACE_TASK_STATE rpi_protocol_task_get_state(void) {

    if (rpi_host_state == RPI_HOST_WAIT_FOR_USER_CFG) {
        
        if (RPI_OP_TIMER_is_up(RPI_PROTOCOL_HANDLER_WAIT_FOR_CFG_SCHEDULE_INTERVAL_MS)) {

            DEBUG_TRACE_word(RPI_OP_TIMER_elapsed() ,"rpi_protocol_task_get_state() - Running unconfigured");

            RPI_OP_TIMER_start();
            return MCU_TASK_RUNNING;
        } else {
            return MCU_TASK_SLEEPING;
        }
    }

    if (rpi_host_state != RPI_HOST_STATE_SLEEP) {
        return MCU_TASK_RUNNING;
    }

    if (RPI_HOST_STATUS_is_set(RPI_HOST_STATUS_COMMAND_PENDING)) {
        return MCU_TASK_RUNNING;
    }

    return MCU_TASK_SLEEPING;
}

/**
 * @brief 
 * 
 */
static void rpi_protocol_task_execute(void) {

    //DEBUG_PASS("rpi_protocol_task_execute()");

    u32 client_poll_interval_ms = RPI_PROTOCOL_CLIENT_POLL_INTERVAL_MS;

    switch (rpi_host_state) {

        default:
            
            rpi_host_state = RPI_HOST_WAIT_FOR_USER_CFG;

            // no break;
            /* fall through */

        case RPI_HOST_WAIT_FOR_USER_CFG :

            if (RPI_HOST_STATUS_is_set(RPI_HOST_STATUS_USER_CONFIG_COMPLETE)) {

                DEBUG_PASS("rpi_protocol_task_execute() - RPI_HOST_WAIT_FOR_USER_CFG >> RPI_HOST_STATE_SLEEP");

                //driver_cfg.module.spi = _com_driver_cfg_spi.module.spi;
                p_com_driver->configure(&_com_driver_cfg_spi);
                rpi_host_state = RPI_HOST_STATE_SLEEP;

            } else {
                DEBUG_PASS("rpi_protocol_task_execute() - No configuration available");
            }

            break;

        case RPI_HOST_STATE_SLEEP:

            if (RPI_HOST_STATUS_is_set(RPI_HOST_STATUS_COMMAND_PENDING) == 0) {
                actual_task_state = MCU_TASK_SLEEPING;
                break;
            }

            DEBUG_PASS("rpi_protocol_task_execute() - RPI_HOST_STATE_SLEEP >> RPI_HOST_STATE_REQUEST_CLIENT");

            RPI_HOST_STATUS_unset(RPI_HOST_STATUS_COMMAND_PENDING);
            rpi_host_state = RPI_HOST_STATE_REQUEST_CLIENT;

            // no break;
            /* fall through */

        case RPI_HOST_STATE_REQUEST_CLIENT:

            DEBUG_PASS("rpi_protocol_task_execute() - RPI_HOST_STATE_CLIENT_WAIT_FOR_COMMAND >> RPI_HOST_STATE_CLIENT_WAIT_FOR_COMMAND");

            REQUEST_CLIENT_drive_low();

            rpi_host_state = RPI_HOST_STATE_CLIENT_WAIT_FOR_COMMAND;
            RPI_OP_TIMER_start();

            // no break;
            /* fall through */

        case RPI_HOST_STATE_CLIENT_WAIT_FOR_COMMAND:

            if (RPI_OP_TIMER_is_up(RPI_PROTOCOL_HOST_CLIENT_REQUEST_TIME_MS) == 0) {
                //DEBUG_PASS("rpi_protocol_task_execute() - RPI_HOST_STATE_CLIENT_WAIT_FOR_COMMAND - waiting for client to wake up");
                break;
            }

            DEBUG_PASS("rpi_protocol_task_execute() - RPI_HOST_STATE_REQUEST_CLIENT >> RPI_HOST_STATE_SEND_COMMAND");

            rpi_host_state = RPI_HOST_STATE_SEND_COMMAND;
            RPI_OP_TIMER_start();

            // no break;
            /* fall through */

        case RPI_HOST_STATE_SEND_COMMAND:

            rpi_host_send_command();

            DEBUG_PASS("rpi_protocol_task_execute() - RPI_HOST_STATE_SEND_COMMAND >> RPI_HOST_STATE_CLIENT_WAIT_FOR_RESPONSE");

            client_poll_interval_ms = RPI_PROTOCOL_CLIENT_POLL_INTERVAL_MS;
            rpi_host_state = RPI_HOST_STATE_CLIENT_WAIT_FOR_RESPONSE;
            RPI_OP_TIMER_start();

            // no break;
            /* fall through */

        case RPI_HOST_STATE_CLIENT_WAIT_FOR_RESPONSE:

            if (RPI_OP_TIMER_is_up(RPI_PROTOCOL_HOST_CLIENT_WAIT_TIMEOUT_MS)) {
                DEBUG_PASS("rpi_protocol_task_execute() - TIMEOUT!!! - RPI_HOST_STATE_CLIENT_WAIT_FOR_RESPONSE >> RPI_HOST_STATE_FINISH");
                RPI_HOST_RESPONSE_TIMEOUT_SIGNAL_send(NULL);
                rpi_host_state = RPI_HOST_STATE_FINISH;
                break;
            }

            if (RPI_OP_TIMER_is_up(client_poll_interval_ms) == 0) {
                break;
            }
            
            client_poll_interval_ms += RPI_PROTOCOL_CLIENT_POLL_INTERVAL_MS;

            if (rpi_host_receive_response() == 0) {
                break;
            }

            DEBUG_TRACE_word(RPI_OP_TIMER_elapsed(), "rpi_protocol_task_execute() - RPI_HOST_STATE_CLIENT_WAIT_FOR_RESPONSE >> RPI_HOST_STATE_FINISH - Time elapsed: ");

            rpi_host_state = RPI_HOST_STATE_FINISH;

            RPI_OP_TIMER_start();

            // no break;
            /* fall through */

        case RPI_HOST_STATE_FINISH:

            DEBUG_PASS("rpi_protocol_task_execute() - RPI_HOST_STATE_FINISH >> RPI_HOST_STATE_SLEEP");
            rpi_host_state = RPI_HOST_STATE_SLEEP;
            REQUEST_CLIENT_no_pull();
            break;
    }
}

/**
 * @brief 
 * 
 */
static void rpi_protocol_task_terminate(void) {

}

// --------------------------------------------------------------------------------

TASK_CREATE(
    RPI_PROTOCOL_TASK,
    TASK_PRIORITY_MIDDLE,
    rpi_protocol_task_get_schedule_interval,
    rpi_protocol_task_start,
    rpi_protocol_task_execute,
    rpi_protocol_task_get_state,
    rpi_protocol_task_terminate
)

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param p_driver 
 */
void rpi_protocol_init(TRX_DRIVER_INTERFACE* p_driver) {

    DEBUG_PASS("rpi_protocol_init()");

    RPI_HOST_STATUS_clear_all();

    REQUEST_CLIENT_pull_up();

    p_com_driver = p_driver;

    DEBUG_PASS("rpi_protocol_init() - RPI_HOST_COMMAND_RECEIVED_SIGNAL_init()");
    RPI_HOST_COMMAND_RECEIVED_SIGNAL_init();

    DEBUG_PASS("rpi_protocol_init() - RPI_HOST_RESPONSE_RECEIVED_SIGNAL_init()");
    RPI_HOST_RESPONSE_RECEIVED_SIGNAL_init();

    DEBUG_PASS("rpi_protocol_init() - RPI_HOST_RESPONSE_TIMEOUT_SIGNAL_init()");
    RPI_HOST_RESPONSE_TIMEOUT_SIGNAL_init();

    DEBUG_PASS("rpi_protocol_init() - RPI_HOST_RESPONSE_OVERFLOW_SIGNAL_init()");
    RPI_HOST_RESPONSE_OVERFLOW_SIGNAL_init();

    DEBUG_PASS("rpi_protocol_init() - RPI_HOST_COMMAND_RECEIVED_SLOT_connect()");
    RPI_HOST_COMMAND_RECEIVED_SLOT_connect();
    
    DEBUG_PASS("rpi_protocol_init() - RPI_HOST_NEW_CFG_OBJECT_SLOT_connect()");
    RPI_HOST_NEW_CFG_OBJECT_SLOT_connect();

    DEBUG_PASS("rpi_protocol_init() - RPI_HOST_CFG_COMPLETE_SLOT_connect()");
    RPI_HOST_CFG_COMPLETE_SLOT_connect();

    RPI_PROTOCOL_TASK_init();

    // mcu_task_controller_register_task(&rpi_protocol_task);
}

u8 rpi_protocol_handler_get_actual_state(void) {
    return (u8) rpi_host_state;
}

// --------------------------------------------------------------------------------
