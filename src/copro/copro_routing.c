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
 * @file    copro_routing_mcu_task.c
 * @author  Sebastian Lesse
 * @date    2022 / 02 / 20
 * @see     app_task/copro_routing.h
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

#include "common/signal_slot_interface.h"
#include "common/local_module_status.h"

#include "mcu_task_management/mcu_task_interface.h"

#include "time_management/time_management.h"
#include "driver/timer/timer_interface.h"

#include "command_management/protocol_interface.h"
#include "command_management/command_handler_interface.h"

#include "command_handler/rpi_cmd_handler_routing.h"

#include "copro/copro_interface.h"
#include "copro_routing.h"

// --------------------------------------------------------------------------------

/**
 * @brief Actual task state of the copro routing task
 */
typedef enum {

    /**
     * @brief Task is in idle state and waits for new command
     */
    COPRO_ROUTING_TASK_STATE_IDLE,

    /**
     * @brief task waits until the actual active copro module has powered up
     * @see COPRO_ROUTING_TASK_POWER_UP_TIMEOUT_MS
     */
    COPRO_ROUTING_TASK_STATE_POWER_ON,

    /**
     * @brief Builds the routing command from the actual active protocol
     * and gives it to the actual active copro module
     */
    COPRO_ROUTING_TASK_STATE_PREPARE,

    /**
     * @brief Waits until the actual copro-module
     * has finished transfering the routing command
     * This state also requests the mutex of the actual active
     * copro module
     */
    COPRO_ROUTING_TASK_STATE_TRANSMIT,

    /**
     * @brief Polls the actual active copro module in a regular interval
     * unitl the command answer is available or the timeout has occured.
     * @see COPRO_ROUTING_TASK_POLLING_INTERVAL_MS
     * @see COPRO_ROUTING_TASK_PROCESS_TIMEOUT_MS
     */
    COPRO_ROUTING_TASK_STATE_PROCESS,

    /**
     * @brief Reads the response of the actual active copro from the 
     * and transfer them to the actual active protocol
     */
    COPRO_ROUTING_TASK_STATE_RESPONSE,

    /**
     * @brief This is always the last state in the processing
     * order. Here the mutex of the actual active copro
     * module is released.
     */
    COPRO_ROUTING_TASK_STATE_FINISH,

    /**
     * @brief THis state is entered if a timeout in an other
     * state occured. The actual active protocol
     * is informed by the error-code CMD_ERR_TIMEOUT
     */
    COPRO_ROUTING_TASK_TIMEOUT
    
} COPRO_ROUTING_TASK_STATE;

// --------------------------------------------------------------------------------

/**
 * @brief 
 */
typedef void (*COPRO_TABLE_SIGNAL_CALLBACK)        (const void* p_arg);

// --------------------------------------------------------------------------------

#ifdef COPRO1_AVAILABLE

static PROTOCOL_INTERFACE* p_copro1_protocol = 0;

/**
 * @brief Callback of the COPRO1_ROUTING_COMMAND_SIGNAL
 * This activates transfereing the command given by p_arg
 * to copor1.
 * 
 * @param p_arg pointer to the protocol context containing the command-buffer with a valid command
 * and the response buffer where the answer of the copro1 is stored into.
 */
static void copro1_slot_CMD_RECEIVED(const void* p_arg) {

    if (p_copro1_protocol != 0) {
        DEBUG_PASS("copro1_slot_CMD_RECEIVED() - COPRO1 is busy");
        return;
    }

    DEBUG_PASS("copro1_slot_CMD_RECEIVED()");
    p_copro1_protocol = (PROTOCOL_INTERFACE*) p_arg;
}

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(COPRO1_ROUTING_COMMAND_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(COPRO1_ROUTING_RESPONSE_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(COPRO1_ROUTING_COMMAND_SIGNAL, COPRO1_ROUTING_COMMAND_SLOT, copro1_slot_CMD_RECEIVED)

COPRO_INTERFACE_INCLUDE(COPRO1)

#endif

// --------------------------------------------------------------------------------

/**
 * @brief Pointer to the actual active copro protocol
 * Points to 0 if there is no protocol is active right now.
 * 
 */
static PROTOCOL_INTERFACE* p_scheduled_protocol = 0;

/**
 * @brief Pointer to thea ctual active copro object.
 * Points to 0 if there is no copro is active right now
 * 
 */
static COPRO_INTERFACE_OBJECT* p_copro_obj = 0;

/**
 * @brief Pointer to the response signal of the actual copro object
 * If the reponse of the copro is received the response signal of this copro
 * is called.
 * Points to COPROx_ROUTING_RESPONSE_SIGNAL_send where x is
 * the number of the actual active copro object
 */
static COPRO_TABLE_SIGNAL_CALLBACK p_response_callback = 0;

/**
 * @brief Actual state of this task
 */
static COPRO_ROUTING_TASK_STATE task_state;

// --------------------------------------------------------------------------------

/**
 * @brief Timer object to check for timeout on
 * procesing copro communication
 */
TIME_MGMN_BUILD_STATIC_TIMER_U16(COPRO_OP_TIMER)

/**
 * @brief Timer object to realize the polling interval
 * in the wait for response state
 */
TIME_MGMN_BUILD_STATIC_TIMER_U16(COPRO_WAIT_TIMER)

// --------------------------------------------------------------------------------

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.init
 */
void copro_routing_task_start(void) {

    DEBUG_PASS("copro_routing_task_start()");

    #ifdef COPRO1_AVAILABLE
    {
        COPRO1_ROUTING_COMMAND_SIGNAL_init();
        COPRO1_ROUTING_RESPONSE_SIGNAL_init();
        COPRO1_ROUTING_COMMAND_SLOT_connect();
    }
    #endif
    
    // only for debugging --- EVENT_OUTPUT_drive_low();

    task_state = COPRO_ROUTING_TASK_STATE_IDLE;
    p_response_callback = 0;
    p_scheduled_protocol = 0;
    p_copro_obj = 0;

    DEBUG_PASS("copro_routing_task_start() - rpi_cmd_handler_routing_init()");
    rpi_cmd_handler_routing_init();
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_schedule_interval
 */
u16 copro_routing_task_get_schedule_interval(void) {
    return COPRO_ROUTING_TASK_RUN_INTERVAL_MS;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_state
 */
MCU_TASK_INTERFACE_TASK_STATE copro_routing_task_get_state(void) {

    if (task_state != COPRO_ROUTING_TASK_STATE_IDLE) {
        return MCU_TASK_RUNNING;
    }

    if (p_scheduled_protocol != 0) {
        return MCU_TASK_RUNNING;
    }

    #ifdef COPRO1_AVAILABLE
    if (p_copro1_protocol != 0) {

        DEBUG_PASS("copro_routing_task_get_state() - COPRO1 selected");

        p_scheduled_protocol = p_copro1_protocol;
        p_copro_obj = (COPRO_INTERFACE_OBJECT*) &COPRO1;
        p_response_callback = &COPRO1_ROUTING_RESPONSE_SIGNAL_send;
        p_copro1_protocol = 0;

        return MCU_TASK_RUNNING;
    }
    #endif

    return MCU_TASK_SLEEPING;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.run
 */
void copro_routing_task_execute(void) {

    //DEBUG_PASS("copro_routing_task_execute()");

    u8 num_bytes = 0;
    u8 t_data_buffer[COPRO_ROUTING_TASK_TEMP_DATA_BUFFER_SIZE];

    switch (task_state) {

        default: // no break;
        case COPRO_ROUTING_TASK_STATE_IDLE :

            if (p_scheduled_protocol == 0 || p_copro_obj == 0) {
                DEBUG_PASS("copro_routing_task_execute() - No copro selected -> what's wrong ?!?");
                break;
            }

            // only for debugging --- EVENT_OUTPUT_drive_low();
                
            DEBUG_PASS("copro_routing_task_execute() - COPRO_ROUTING_TASK_STATE_IDLE -> COPRO_ROUTING_TASK_STATE_PREPARE");
            task_state = COPRO_ROUTING_TASK_STATE_POWER_ON;

            p_copro_obj->power_on();

            // no break;

        case COPRO_ROUTING_TASK_STATE_POWER_ON :

            if (p_copro_obj->power_is_on() == 0) {
                //DEBUG_PASS("copro_routing_task_execute() - COPRO_ROUTING_TASK_STATE_POWER_ON - Waiting for power-management");
                break;
            }

            task_state = COPRO_ROUTING_TASK_STATE_PREPARE;
            COPRO_OP_TIMER_start();

            // no break;

        case COPRO_ROUTING_TASK_STATE_PREPARE :

            if (COPRO_OP_TIMER_is_up(COPRO_ROUTING_TASK_PREPARE_TIMEOUT_MS)) {
                DEBUG_PASS("copro_routing_task_execute() - COPRO_ROUTING_TASK_STATE_PREPARE -> COPRO_ROUTING_TASK_TIMEOUT --- !!! --- !!! ---");
                task_state = COPRO_ROUTING_TASK_TIMEOUT;
                break;
            }

            if (p_copro_obj->mutex_req() == 0)  {
                DEBUG_PASS("copro_routing_task_execute() - p_copro_obj->mutex_req() has FAILED !!! ---");
                break;
            }
            
            num_bytes = p_scheduled_protocol->cmd_buffer->bytes_available();
            p_scheduled_protocol->cmd_buffer->start_read();
            p_scheduled_protocol->cmd_buffer->get_N_bytes(num_bytes, t_data_buffer);
            p_scheduled_protocol->cmd_buffer->stop_read();

            DEBUG_TRACE_N(num_bytes, t_data_buffer, "copro_routing_task_execute() - Routing Command");

            p_copro_obj->clear_tx_buffer();
            p_copro_obj->set_N_bytes(num_bytes, t_data_buffer);
            p_copro_obj->start_tx();

            DEBUG_PASS("copro_routing_task_execute() - COPRO_ROUTING_TASK_STATE_PREPARE -> COPRO_ROUTING_TASK_STATE_TRANSMIT");
            task_state = COPRO_ROUTING_TASK_STATE_TRANSMIT;

            COPRO_OP_TIMER_start();
            // no break;

        case COPRO_ROUTING_TASK_STATE_TRANSMIT :

            if (COPRO_OP_TIMER_is_up(COPRO_ROUTING_TASK_TRANSMIT_TIMEOUT_MS)) {
                DEBUG_PASS("copro_routing_task_execute() - COPRO_ROUTING_TASK_STATE_TRANSMIT -> COPRO_ROUTING_TASK_TIMEOUT --- !!! --- !!! ---");
                task_state = COPRO_ROUTING_TASK_TIMEOUT;
                break;
            }

            if (p_copro_obj->is_ready_for_tx() == 0) {
                break;
            }

            p_copro_obj->stop_tx();

            DEBUG_PASS("copro_routing_task_execute() - COPRO_ROUTING_TASK_STATE_TRANSMIT -> COPRO_ROUTING_TASK_STATE_PROCESS");
            task_state = COPRO_ROUTING_TASK_STATE_PROCESS;

            COPRO_OP_TIMER_start();
            COPRO_WAIT_TIMER_start();
            // no break;


        case COPRO_ROUTING_TASK_STATE_PROCESS :

            if (COPRO_OP_TIMER_is_up(COPRO_ROUTING_TASK_PROCESS_TIMEOUT_MS)) {
                DEBUG_PASS("copro_routing_task_execute() - COPRO_ROUTING_TASK_STATE_PROCESS -> COPRO_ROUTING_TASK_TIMEOUT --- !!! --- !!! ---");
                task_state = COPRO_ROUTING_TASK_TIMEOUT;
                break;
            }

            if (COPRO_WAIT_TIMER_is_up(COPRO_ROUTING_TASK_POLLING_INTERVAL_MS) == 0) {
                break;
            }

            DEBUG_TRACE_word(COPRO_WAIT_TIMER_elapsed(), "copro_routing_task_execute() - COPRO_ROUTING_TASK_STATE_PROCESS - Polling COPRO for response");

            COPRO_WAIT_TIMER_start();

            // only for debugging --- EVENT_OUTPUT_drive_high();

            p_copro_obj->clear_rx_buffer();
            p_copro_obj->start_rx(1); // get length of answer
            p_copro_obj->wait_for_rx(1, 5);
            p_copro_obj->stop_rx();

            // only for debugging --- EVENT_OUTPUT_drive_low();

            if (p_copro_obj->bytes_available() == 0) {
                break;
            }

            num_bytes = p_copro_obj->get_N_bytes(1, t_data_buffer);

            if (t_data_buffer[0] == 0) {
                break;
            }

            DEBUG_TRACE_byte(t_data_buffer[0], "copro_routing_task_execute() - Number of bytes to read from Copro");

            p_copro_obj->start_rx(t_data_buffer[0]); // get length of answer
            p_copro_obj->wait_for_rx(t_data_buffer[0], 50);
            p_copro_obj->stop_rx();

            DEBUG_PASS("copro_routing_task_execute() - COPRO_ROUTING_TASK_STATE_PROCESS -> COPRO_ROUTING_TASK_STATE_RESPONSE");
            task_state = COPRO_ROUTING_TASK_STATE_RESPONSE;

            break;

        case COPRO_ROUTING_TASK_STATE_RESPONSE :

            num_bytes = p_copro_obj->get_N_bytes(COPRO_ROUTING_TASK_TEMP_DATA_BUFFER_SIZE, t_data_buffer + 1);

            if (num_bytes == 0) {
                DEBUG_PASS("copro_routing_task_execute() - COPRO_ROUTING_TASK_STATE_RESPONSE -> COPRO_ROUTING_TASK_TIMEOUT --- !!! --- !!! ---");
                task_state = COPRO_ROUTING_TASK_TIMEOUT;
                COPRO_WAIT_TIMER_start();
                break;
            }

            p_scheduled_protocol->answ_buffer->start_write();
            p_scheduled_protocol->answ_buffer->add_N_bytes(t_data_buffer[0], t_data_buffer + 1);
            p_scheduled_protocol->answ_buffer->stop_write();

            DEBUG_TRACE_N(t_data_buffer[0] + 1, t_data_buffer, "copro_routing_task_execute() - COPRO_ROUTING_TASK_STATE_RESPONSE - Data received:");

            if (p_response_callback != 0) {
                DEBUG_PASS("copro_routing_task_execute() - p_response_callback()");
                p_response_callback((void*) p_scheduled_protocol);
            }

            DEBUG_PASS("copro_routing_task_execute() - COPRO_ROUTING_TASK_STATE_RESPONSE -> COPRO_ROUTING_TASK_STATE_FINISH");
            task_state = COPRO_ROUTING_TASK_STATE_FINISH;

            break;

        case COPRO_ROUTING_TASK_TIMEOUT :

            p_scheduled_protocol->set_finished(CMD_ERR_TIMEOUT);

            DEBUG_PASS("copro_routing_task_execute() - COPRO_ROUTING_TASK_TIMEOUT -> COPRO_ROUTING_TASK_STATE_FINISH");
            task_state = COPRO_ROUTING_TASK_STATE_FINISH;

            break;

        case COPRO_ROUTING_TASK_STATE_FINISH :

            p_scheduled_protocol = 0;

            p_copro_obj->mutex_rel();
            p_copro_obj->power_off();
            p_copro_obj = 0;

            DEBUG_PASS("copro_routing_task_execute() - COPRO_ROUTING_TASK_STATE_FINISH -> COPRO_ROUTING_TASK_STATE_IDLE");
            task_state = COPRO_ROUTING_TASK_STATE_IDLE;

            break;
    }
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.background_run
 */
void copro_routing_task_background_run(void) {
    // nothing to do
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.termiante
 */
void copro_routing_task_terminate(void) {
    // nothing to do
}

// --------------------------------------------------------------------------------

TASK_CREATE(
    COPRO_ROUTING_TASK,
    TASK_PRIORITY_MIDDLE,
    copro_routing_task_get_schedule_interval,
    copro_routing_task_start,
    copro_routing_task_execute,
    copro_routing_task_get_state,
    copro_routing_task_terminate
)

// --------------------------------------------------------------------------------

/**
 * @see copro_routing.h
 */
void copro_routing_init(void) {
    COPRO_ROUTING_TASK_init();
}

// --------------------------------------------------------------------------------
