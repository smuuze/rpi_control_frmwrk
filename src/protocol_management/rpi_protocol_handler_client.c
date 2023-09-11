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
 * @file    rpi_protocol_handler_client.c
 * @author  Sebastian Lesse
 * @date    2020 / 12 / 18
 * @brief   Implementation of the RPi Communication Protocol in client mode
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

#include "common/local_msg_buffer.h"
#include "common/local_module_status.h"
#include "common/local_mutex.h"

#include "io_management/io_controller.h"

#include "driver/trx_driver_interface.h"
#include "driver/cfg_driver_interface.h"
#include "driver/driver_specific_spi.h"

#include "command_management/protocol_interface.h"
#include "time_management/time_management.h"
#include "common/signal_slot_interface.h"

#include "protocol_management/rpi_protocol_handler.h"

#include "mcu_task_interface.h"

// --------------------------------------------------------------------------------

#ifndef HAS_GPIO_READY_INOUT
#error DEFINE_PIN___READY_INOUT___IN_YOUR_BOARD_FILE
#endif

// --------------------------------------------------------------------------------

/**
 * @brief Bufersize for temporare operations
 * 
 */
#define RPI_PROTOCOL_HANDLER_TEMP_BUFFER_SIZE                   32

/**
 * @brief Timeout to wait for internal command processing
 * If this timeout is up, command processing is declared as failed.
 * All operations are stopped. No response is send to the host
 * and the sleep state is entered
 */
#define RPI_PROTOCOL_HANDLER_CMD_PROCESSING_TIMEOUT_MS          500

/**
 * @brief Interval in milliseconds at which this task is scheduled
 * The value is set to a low number to let it schedule every clock-tick.
 * Because the system enterns the mcu-sleep mode for a amount of time (e.g. 30ms)
 */
#define RPI_PROTOCOL_HANDLER_SCHEDULE_INTERVAL_MS               5

// --------------------------------------------------------------------------------

/**
 * @brief States of the Rpi-Protocol-Handler Task
 */
typedef enum {
    RPI_STATE_IDLE,
    RPI_STATE_PREPARE,
    RPI_STATE_TRANSMISSION,
    RPI_STATE_COMMAND_RECEIVED,
    RPI_ENTER_SLEEP,
} RPI_PROTOCOL_HANDLER_STATE;

/*!
 *
 */
typedef enum {
    RPI_TRX_STATE_BUSY,
    RPI_TRX_STATE_NO_DATA,
    RPI_TRX_STATE_COMPLETE,
    RPI_TRX_STATE_TIMEOUT
} RPI_TRX_STATE;

// --------------------------------------------------------------------------------

BUILD_LOCAL_MSG_BUFFER( , RPI_COMMAND_BUFFER, 32)
BUILD_LOCAL_MSG_BUFFER( , RPI_ANSWER_BUFFER,  32)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(RPI_OP_TIMER)
TIME_MGMN_BUILD_STATIC_TIMER_U16(RPI_TRX_TIMER)

// --------------------------------------------------------------------------------

#define RPI_STATUS_COMMAND_PENDING                                          (1 << 0)
#define RPI_STATUS_RESPONSE_PENDING                                         (1 << 1)

BUILD_MODULE_STATUS_U16(RPI_STATUS)

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_PROTOCOL_LEAVE_SLEEP_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_PROTOCOL_ENTER_SLEEP_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_PROTOCOL_INVALID_COMMAND_RECEIVED_SIGNAL)

/**
 * @brief Construct a new signal slot interface create signal object
 */
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(RPI_PROTOCOL_COMMAND_RECEIVED_SIGNAL)

// --------------------------------------------------------------------------------

/**
 * @brief This structured is given to the requested command-handler.
 * The command handler reads the command data from the command-buffer
 * and puts the response into the answer-buffer. After the resposne has
 * finished the current command-handler calls the FINISHED-CALLBACK.
 */
static void _set_finished_spi(u8 err_code);

// --------------------------------------------------------------------------------

/**
 * @brief Pointer to the current used communication interface, e.g. spi.
 * The device will be requested during startup and stays requested during the runtime.
 * It will never be released to ensure an exclusive usage.
 */
static TRX_DRIVER_INTERFACE* p_com_driver = 0;

/**
 * @brief Current state of the RPi-Protocol handler.
 */
static RPI_PROTOCOL_HANDLER_STATE current_state_protocol = RPI_STATE_IDLE;

/**
 * @brief Current task state.
 */
static MCU_TASK_INTERFACE_TASK_STATE current_state_task = MCU_TASK_UNINITIALIZED;

/**
 * @brief Current Mutex-ID of the communication-interface
 * Used to lock the usage of the communication interface.
 * Hint: the communication device remains requested during the runtime
 */
u8 driver_mutex_id = 0;

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
static COMMAND_BUFFER_INTERFACE rpi_cmd_handler_command_buffer = {
    &RPI_COMMAND_BUFFER_start_read,
    &RPI_COMMAND_BUFFER_bytes_available, 
    &RPI_COMMAND_BUFFER_get_byte,
    &RPI_COMMAND_BUFFER_get_word,
    &RPI_COMMAND_BUFFER_get_long,
    &RPI_COMMAND_BUFFER_get_N_bytes,
    &RPI_COMMAND_BUFFER_stop_read
};

/*!
 *
 */
static ANSWER_BUFFER_INTERFACE rpi_cmd_handler_answer_buffer = {
    &RPI_ANSWER_BUFFER_start_write,
    &RPI_ANSWER_BUFFER_add_byte,
    &RPI_ANSWER_BUFFER_add_word,
    &RPI_ANSWER_BUFFER_add_long,
    &RPI_ANSWER_BUFFER_add_N_bytes,
    &RPI_ANSWER_BUFFER_stop_write,
};

// --------------------------------------------------------------------------------

/**
 * @see rpi_protocol_handler_client.c#rpi_protocol_spi_interface
 */
static PROTOCOL_INTERFACE rpi_protocol_spi_interface = {

    0, // commnand-length
    0, // command-code
    0, // answer-status
    0, // arrival-time
    &_set_finished_spi,
    &rpi_cmd_handler_command_buffer,
    &rpi_cmd_handler_answer_buffer
};

// --------------------------------------------------------------------------------

/**
 * @brief Callback for the currently requested command handler.
 * Is called after the requested command has finished.
 * This is used as trigger to detect a fiished command-processing.
 * 
 * @param err_code Return value of the requested command handler
 */
static void _set_finished_spi(u8 err_code) {

    if (RPI_STATUS_is_set(RPI_STATUS_COMMAND_PENDING) == 0) {
        return;
    }

    //DEBUG_TRACE_byte(err_code, "_set_finished_spi() - Error: ");

    rpi_protocol_spi_interface.answer_status = err_code;

    RPI_STATUS_unset(RPI_STATUS_COMMAND_PENDING);
    RPI_STATUS_set(RPI_STATUS_RESPONSE_PENDING);
    //rpi_cmd_handler_set_unrequested();
}

// --------------------------------------------------------------------------------

/**
 * @brief Cancels receiving of a new command telegram.
 * Writing into the command buffer is stopped.
 * 
 * @param state will be returned by this function
 * @return state
 */
static inline RPI_TRX_STATE rpi_protocol_cancel_receive_command(RPI_TRX_STATE state) {
    RPI_COMMAND_BUFFER_stop_write();
    return state;
}

/**
 * @brief Receives a new command via the communication device, if available.
 * Receiving a telegram is devided into the following steps:
 * 
 *  - Receive the number of bytes of the incomming command telegram
 * 
 *  - Wait for communication device to receive some more bytes of the command telegram
 * 
 *  - Read received bytes of the command telegram from the communication device
 *    and receive the rest of the command telegram, if necessary.
 * 
 *  - Set the module status RPI_STATUS_COMMAND_PENDING in
 *    case a new command telegram was received successful
 * 
 * @return RPI_TRX_STATE 
 */
static RPI_TRX_STATE rpi_protocol_receive_command(void) {

    rpi_protocol_spi_interface.command_length = 0;

    RPI_TRX_STATE error_code = RPI_TRX_STATE_COMPLETE;

    RPI_TRX_TIMER_start();

    while (p_com_driver->bytes_available() != 0) {

        if (RPI_TRX_TIMER_is_up(5)) {
            DEBUG_PASS("rpi_protocol_receive_command() - Receiving command-length has FAILED (TIMEOUT) !!! ---");
            return rpi_protocol_cancel_receive_command(RPI_TRX_STATE_TIMEOUT);
        }

        p_com_driver->get_N_bytes(1, (u8*)&rpi_protocol_spi_interface.command_length);
        if (rpi_protocol_spi_interface.command_length == 0) {
            //DEBUG_PASS("rpi_protocol_receive_command() - Zero byte received");
            continue;
        }

        if (rpi_protocol_spi_interface.command_length == 0xFF) {
            //DEBUG_PASS("rpi_protocol_receive_command() - 0xFF byte received");
            rpi_protocol_spi_interface.command_length = 0;
            continue;
        }

        if (rpi_protocol_spi_interface.command_length != 0) {
            DEBUG_TRACE_byte(rpi_protocol_spi_interface.command_length, "rpi_protocol_receive_command() - Command-Length: ");
            break;
        }
    }

    if (rpi_protocol_spi_interface.command_length == 0) {
        //DEBUG_PASS("rpi_protocol_receive_command() - No command-data received");
        return rpi_protocol_cancel_receive_command(RPI_TRX_STATE_NO_DATA);
    }
            
    RPI_TRX_TIMER_start();

    while (p_com_driver->bytes_available() == 0) {

        if (RPI_TRX_TIMER_is_up(10)) {
            DEBUG_TRACE_byte(rpi_protocol_spi_interface.command_length, "rpi_protocol_receive_command() - Receiving command-data has FAILED (TIMEOUT) !!! ---");
            return rpi_protocol_cancel_receive_command(RPI_TRX_STATE_TIMEOUT);
        }
    }

    p_com_driver->get_N_bytes(1, (u8*)&rpi_protocol_spi_interface.command_code);

    DEBUG_TRACE_byte(rpi_protocol_spi_interface.command_length ,"rpi_protocol_receive_command() - Command-Length: ");
    DEBUG_TRACE_byte(rpi_protocol_spi_interface.command_code ,"rpi_protocol_receive_command() - Command-Code: ");

    RPI_COMMAND_BUFFER_clear_all();
    RPI_COMMAND_BUFFER_start_write();
            
    RPI_TRX_TIMER_start();

    u16 bytes_remain = (u16)rpi_protocol_spi_interface.command_length - 1; // command already received

    while (bytes_remain) {

        if (RPI_TRX_TIMER_is_up(50)) {
            DEBUG_TRACE_byte(rpi_protocol_spi_interface.command_length, "rpi_protocol_receive_command() - Receiving command-data has FAILED (TIMEOUT) !!! ---");
            return rpi_protocol_cancel_receive_command(RPI_TRX_STATE_TIMEOUT);
        }

        if (p_com_driver->bytes_available() != 0) {
            u8 t_buffer[RPI_PROTOCOL_HANDLER_TEMP_BUFFER_SIZE];
            u16 read_count = RPI_PROTOCOL_HANDLER_TEMP_BUFFER_SIZE;

            if (read_count > p_com_driver->bytes_available()) {
                read_count = p_com_driver->bytes_available();
            }

            if (read_count > bytes_remain) {
                read_count = bytes_remain;
            }

            read_count = p_com_driver->get_N_bytes(read_count, t_buffer);
            bytes_remain -= read_count;

            RPI_COMMAND_BUFFER_add_N_bytes(read_count, t_buffer);

            DEBUG_TRACE_N(read_count, t_buffer ,"rpi_protocol_receive_command() - Command-Data: ");
        }
    }

    RPI_COMMAND_BUFFER_stop_write();
    RPI_STATUS_set(RPI_STATUS_COMMAND_PENDING);

    return error_code;

}

// --------------------------------------------------------------------------------

/*!
 *
 */
static RPI_TRX_STATE rpi_protocol_transmit_answer(void) {

    //DEBUG_PASS("rpi_protocol_transmit_answer()");

    #ifdef TRACES_ENABLED
    rpi_protocol_spi_interface.arrival_time = time_mgmnt_gettime_u16- rpi_protocol_spi_interface.arrival_time;
    DEBUG_TRACE_word(rpi_protocol_spi_interface.arrival_time, "rpi_protocol_transmit_answer() - Time past since command has arrived and processed");
    #endif

    u8 answer_header[] = {
        (RPI_ANSWER_BUFFER_bytes_available() + 2),
        (rpi_protocol_spi_interface.command_code),
        (rpi_protocol_spi_interface.answer_status)
    };

    p_com_driver->clear_tx_buffer();
    p_com_driver->set_N_bytes(RPI_PROTOCOL_HEADER_LENGTH_RESPONSE, answer_header);

    u16 bytes_to_send = RPI_PROTOCOL_HEADER_LENGTH_RESPONSE;

    RPI_ANSWER_BUFFER_start_read();
    u16 bytes_left = RPI_ANSWER_BUFFER_bytes_available();

    while (bytes_left != 0) {

        if (RPI_ANSWER_BUFFER_bytes_free() == 0) {
            break;
        }

        u16 read_length = RPI_PROTOCOL_HANDLER_TEMP_BUFFER_SIZE;
        u8 t_buffer[RPI_PROTOCOL_HANDLER_TEMP_BUFFER_SIZE];

        if (read_length > bytes_left) {
            read_length = bytes_left;
        }

        if (RPI_ANSWER_BUFFER_bytes_free() < read_length) {
            read_length = RPI_ANSWER_BUFFER_bytes_free();
        }

        RPI_ANSWER_BUFFER_get_N_bytes(read_length, t_buffer);
        p_com_driver->set_N_bytes(read_length, t_buffer);

        bytes_left -= read_length;
        bytes_to_send += read_length;
    }

    RPI_ANSWER_BUFFER_stop_read();
    RPI_STATUS_unset(RPI_STATUS_RESPONSE_PENDING);

    RPI_TRX_STATE error_code = RPI_TRX_STATE_COMPLETE;

    //DEBUG_PASS("rpi_protocol_transmit_answer() - answer is on it`s way");

    p_com_driver->start_tx();

    return error_code;
}

// --------------------------------------------------------------------------------

/**
 * @see  mcu_task_interface.h#MCU_TASK_INTERFACE.start
 */
static void rpi_protocol_task_start(void) {

    DEBUG_PASS("rpi_protocol_task_start()");
}

/**
 * @see  mcu_task_interface.h#MCU_TASK_INTERFACE.start
 */
static u16 rpi_protocol_task_get_schedule_interval(void) {
    if (current_state_task != MCU_TASK_SLEEPING) {
        return 0;
    } else {
        return RPI_PROTOCOL_HANDLER_SCHEDULE_INTERVAL_MS;
    }
}

/**
 * @see  mcu_task_interface.h#MCU_TASK_INTERFACE.get_state
 */
static MCU_TASK_INTERFACE_TASK_STATE rpi_protocol_task_get_state(void) {

    u8 is_ready = READY_INOUT_is_low_level();

    if ((current_state_task == MCU_TASK_SLEEPING) && is_ready) {
        DEBUG_PASS("rpi_protocol_task_get_state() - Start-Signal detected - will enter Task-State RUNNING");
        current_state_task = MCU_TASK_RUNNING;
    }

    return current_state_task;
}

/**
 * @see  mcu_task_interface.h#MCU_TASK_INTERFACE.execute
 */
static void rpi_protocol_task_execute(void) {

    RPI_TRX_STATE trx_state = RPI_TRX_STATE_BUSY;

    switch (current_state_protocol) {

        default : 

            current_state_protocol = RPI_STATE_IDLE;
            // no break;

        case RPI_STATE_IDLE :

            if (READY_INOUT_is_low_level() == 0) {
                DEBUG_PASS("rpi_protocol_task_execute() - RPI_STATE_IDLE - No request - FALSE ALARM");
                current_state_task = MCU_TASK_SLEEPING;
                break;
            }

            DEBUG_PASS("rpi_protocol_task_execute() - RPI_STATE_IDLE -> RPI_STATE_PREPARE");
            RPI_PROTOCOL_LEAVE_SLEEP_SIGNAL_send(NULL);
            current_state_protocol = RPI_STATE_PREPARE;
            // no break;

        case RPI_STATE_PREPARE :

            //DEBUG_PASS("rpi_protocol_task_execute() - RPI_STATE_PREPARE -> RPI_STATE_PREPARE");

            p_com_driver->configure(&driver_cfg);
            p_com_driver->clear_rx_buffer();
            p_com_driver->start_rx(TRX_DRIVER_INTERFACE_UNLIMITED_RX_LENGTH);

            current_state_protocol = RPI_STATE_TRANSMISSION;

            // no break;

        case RPI_STATE_TRANSMISSION :
            
            if (READY_INOUT_is_low_level() == 0) {

                DEBUG_PASS("rpi_protocol_task_execute() - not requested anymore - RPI_STATE_TRANSMISSION -> RPI_ENTER_SLEEP");

                RPI_OP_TIMER_start();
                current_state_protocol = RPI_ENTER_SLEEP;

                break;
            }

            if (RPI_STATUS_is_set(RPI_STATUS_RESPONSE_PENDING)) {
                DEBUG_PASS("rpi_protocol_task_execute() - RPI_STATE_TRANSMISSION - Response available");
                RPI_STATUS_unset(RPI_STATUS_RESPONSE_PENDING);
                rpi_protocol_transmit_answer();
            }

            // block until command rx complete, device busy or timeout
            trx_state = rpi_protocol_receive_command();
            
            if (trx_state == RPI_TRX_STATE_TIMEOUT) {

                DEBUG_PASS("rpi_protocol_task_execute() - RPI_STATE_TRANSMISSION - invalid caommand received");
                RPI_PROTOCOL_INVALID_COMMAND_RECEIVED_SIGNAL_send(NULL);
                break;

            } else if (trx_state == RPI_TRX_STATE_COMPLETE) {

            } else {
                break;
            }

            DEBUG_PASS("rpi_protocol_task_execute() - command received - RPI_STATE_TRANSMISSION -> RPI_STATE_COMMAND_RECEIVED");
            current_state_protocol = RPI_STATE_COMMAND_RECEIVED;

            // no break;

        case RPI_STATE_COMMAND_RECEIVED :

            RPI_PROTOCOL_COMMAND_RECEIVED_SIGNAL_send((const void*)((const PROTOCOL_INTERFACE*)&rpi_protocol_spi_interface));
            p_com_driver->clear_rx_buffer();

            DEBUG_PASS("rpi_protocol_task_execute() - change state - RPI_STATE_COMMAND_RECEIVED -> RPI_STATE_TRANSMISSION");
            current_state_protocol = RPI_STATE_TRANSMISSION;
            break;

        case RPI_ENTER_SLEEP:

            if (READY_INOUT_is_high_level() == 0) {

                DEBUG_PASS("rpi_protocol_task_execute() - re-request - RPI_ENTER_SLEEP -> RPI_STATE_TRANSMISSION");

                current_state_protocol = RPI_STATE_TRANSMISSION;
                break;
            }

            if (RPI_OP_TIMER_is_up(RPI_PROTOCOL_HANDLER_CMD_PROCESSING_TIMEOUT_MS) == 0) {
                //DEBUG_PASS("rpi_protocol_task_execute() - RPI_STATE_PROCESS_COMMAND - still waiting for re-request");
                break;
            }

            DEBUG_PASS("rpi_protocol_task_execute() - request-timeout - RPI_STATE_FINISH -> RPI_STATE_IDLE");

            p_com_driver->stop_rx();
            p_com_driver->stop_tx();
            p_com_driver->shut_down();

            current_state_protocol = RPI_STATE_IDLE;
            current_state_task = MCU_TASK_SLEEPING;

            RPI_PROTOCOL_ENTER_SLEEP_SIGNAL_send(NULL);

            break;
    }
}

/**
 * @see  mcu_task_interface.h#MCU_TASK_INTERFACE.terminate
 */
static void rpi_protocol_task_terminate(void) {

    DEBUG_PASS("rpi_protocol_task_terminate()");
}

// --------------------------------------------------------------------------------

TASK_CREATE (
    RPI_PROTOCOL_TASK,
    TASK_PRIORITY_MIDDLE,
    rpi_protocol_task_get_schedule_interval,
    rpi_protocol_task_start,
    rpi_protocol_task_execute,
    rpi_protocol_task_get_state,
    rpi_protocol_task_terminate
)

// --------------------------------------------------------------------------------

void rpi_protocol_init(TRX_DRIVER_INTERFACE* p_driver) {

    READY_INOUT_drive_low();

    DEBUG_PASS("rpi_protocol_init() - CLIENT - ");

    RPI_STATUS_clear_all();

    rpi_protocol_spi_interface.command_length = 0;
    rpi_protocol_spi_interface.arrival_time = 0;

    RPI_COMMAND_BUFFER_init();
    RPI_ANSWER_BUFFER_init();

    RPI_PROTOCOL_COMMAND_RECEIVED_SIGNAL_init();
    RPI_PROTOCOL_LEAVE_SLEEP_SIGNAL_init();
    RPI_PROTOCOL_ENTER_SLEEP_SIGNAL_init();
    RPI_PROTOCOL_INVALID_COMMAND_RECEIVED_SIGNAL_init();

    TRX_DRIVER_CONFIGURATION driver_cfg = {
        .module = {
            .spi = {
                RPI_PROTOCOL_HANDLER_DRIVER_CFG
            }
        }
    };

    p_com_driver = p_driver;

    driver_mutex_id = p_com_driver->mutex_req();
    if (driver_mutex_id == MUTEX_INVALID_ID) {
        DEBUG_PASS("rpi_protocol_init() - Requesting MUTEX has FAILED !!! ---");
    }

    p_com_driver->configure(&driver_cfg);

    current_state_task = MCU_TASK_SLEEPING;
    current_state_protocol = RPI_STATE_IDLE;

    DEBUG_PASS("rpi_protocol_init() - FINISH");

    READY_INOUT_pull_up();

    RPI_PROTOCOL_TASK_init();
}

// --------------------------------------------------------------------------------
