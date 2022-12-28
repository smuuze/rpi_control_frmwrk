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
 * @file    ir_remote_mcu_task.c
 * @author  Sebastian Lesse
 * @date    2021 / 12 / 30
 * @see     ir_protocol_task.h
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
#include "common/qeue_interface.h"

// --------------------------------------------------------------------------------

#include "mcu_task_management/mcu_task_interface.h"
#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

#include "driver/timer/timer0_driver.h"
#include "driver/timer/timer1_driver.h"
#include "driver/timer/timer_interface.h"

// --------------------------------------------------------------------------------

#include "modules/ir/ir_protocol_interface.h"

#include "modules/ir/ir_protocol_nec.h"
#include "modules/ir/ir_protocol_sony.h"
#include "modules/ir/ir_protocol_jvc.h"
#include "modules/ir/ir_protocol_samsung.h"

// --------------------------------------------------------------------------------

#define IR_REMOTE_TASK_RUN_INTERVAL_MS              100
#define IR_REMOTE_TASK_CHANGE_FREQ_INTERVAL_MS      2000

// --------------------------------------------------------------------------------

#define IR_PROTOCOL_TASK_MAX_QUEUE_SIZE             5

QEUE_INTERFACE_BUILD_QEUE(
    IR_COMMAND_QUEUE,
    IR_COMMON_COMMAND_TYPE,
    sizeof(IR_COMMON_COMMAND_TYPE),
    IR_PROTOCOL_TASK_MAX_QUEUE_SIZE
)

// --------------------------------------------------------------------------------

/**
 * @brief 
 */
typedef enum {
    IR_PROTOCOL_TASK_STATE_IDLE,
    IR_PROTOCOL_TASK_STATE_LOAD_PROTOCOL,
    IR_PROTOCOL_TASK_STATE_START,
    IR_PROTOCOL_TASK_STATE_TRANSMIT,
    IR_PROTOCOL_TASK_STATE_FINISH
} IR_PROTOCOL_TASK_STATE;

// --------------------------------------------------------------------------------

/**
 * @brief Interface to the timer-module that is used to
 * generate the base-carrier frequency of a ir-signal
 * 
 */
static TIMER_INTERFACE_TYPE timer_carrier = {
    .init = &timer0_driver_init,
    .configure = &timer0_driver_configure,
    .start = &timer0_driver_start,
    .stop = &timer0_driver_stop
};

/**
 * @brief Interface to the timer-module that is used to
 * generate the modulation-signal interval
 * 
 */
static TIMER_INTERFACE_TYPE timer_modulator = {
    .init = &timer1_driver_init,
    .configure = &timer1_driver_configure,
    .start = &timer1_driver_start,
    .stop = &timer1_driver_stop
};

/**
 * @brief 
 */
static IR_PROTOCOL_TASK_STATE task_state = IR_PROTOCOL_TASK_STATE_IDLE;

// --------------------------------------------------------------------------------

/**
 * @see  mcu_task_interface.h#MCU_TASK_INTERFACE.init
 * 
 */
static void ir_remote_task_init(void);

/**
 * @see  mcu_task_interface.h#MCU_TASK_INTERFACE.get_schedule_interval
 * 
 */
static u16 ir_remote_task_get_schedule_interval(void);

/**
 * @see mcu_task_interface.h#MCU_TASK_INTERFACE.get_state
 * 
 */
static MCU_TASK_INTERFACE_TASK_STATE ir_remote_task_get_state(void);

/**
 * @see  mcu_task_interface.h#MCU_TASK_INTERFACE.get_state
 * 
 */
static void ir_remote_task_run(void);

/**
 * @see  mcu_task_interface.h#MCU_TASK_INTERFACE.terminate
 * 
 */
__UNUSED__ static void ir_remote_task_terminate(void);

/**
 * @brief Task-interface object that is registered at the mcu task manager
 */
TASK_CREATE (
    IR_PROTOCOL_TASK,
    TASK_PRIORITY_MIDDLE,
    ir_remote_task_get_schedule_interval,
    ir_remote_task_init,
    ir_remote_task_run,
    ir_remote_task_get_state,
    ir_remote_task_terminate
)

// --------------------------------------------------------------------------------

/**
 * @brief Always points to the first available ir-protocol.
 * 
 */
static IR_PROTOCOL_GENERATOR_TYPE* p_ir_protocol_first = 0;

/**
 * @brief Always points to the last available ir_protocol.
 * 
 */
static IR_PROTOCOL_GENERATOR_TYPE* p_ir_protocol_last = 0;

// --------------------------------------------------------------------------------

/**
 * @brief Pointer to the actual active protocol
 * 
 */
static IR_PROTOCOL_GENERATOR_TYPE* p_act_protocol = 0;

/**
 * @brief 
 * 
 */
static IR_COMMON_COMMAND_TYPE ir_command;

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param p_ir_protocol 
 */
static void ir_remote_task_transmit_start(
    const IR_PROTOCOL_GENERATOR_TYPE* p_ir_protocol,
    const IR_COMMON_COMMAND_TYPE* p_ir_command
) {

    DEBUG_PASS("ir_remote_task_transmit_start()");

    p_ir_protocol->transmit_prepare(p_ir_command);

    TIMER_CONFIGURATION_TYPE timer_config;
    
    timer_config.frequency = TIMER_FREQUENCY_NONE;
    timer_config.irq_callback = p_ir_protocol->transmit_irq;
    timer_config.mode = TIMER_MODE_TIMER;
    timer_config.time_interval = p_ir_protocol->get_mod_interval();

    timer_modulator.configure(&timer_config);
    
    timer_config.frequency = p_ir_protocol->get_frequency();
    timer_config.irq_callback = 0;
    timer_config.mode = TIMER_MODE_FREQUENCY;

    timer_carrier.configure(&timer_config);
    
    timer_carrier.start(TIME_CONFIGURATION_RUN_FOREVER);
    timer_modulator.start(TIME_CONFIGURATION_RUN_FOREVER);

    p_ir_protocol->transmit_start();
}

/**
 * @brief 
 * 
 * @param p_ir_protocol 
 */
static void ir_protocol_task_load_protocol(
    IR_PROTOCOL_GENERATOR_TYPE* p_ir_protocol,
    const IR_COMMON_COMMAND_TYPE* p_ir_command
) {

    p_act_protocol = p_ir_protocol_first;

    while (p_act_protocol != 0) {

        if (p_act_protocol->uid == p_ir_command->type) {
            break;
        }

        p_act_protocol = p_act_protocol->_p_next;
    }
}

/**
 * @brief 
 */
static void ir_protocol_task_finish(void) {

    p_act_protocol = 0;

    timer_carrier.stop();
    timer_modulator.stop();
    
    IR_CARRIER_IN_no_pull();
    IR_CARRIER_OUT_drive_low();
    IR_MOD_OUT_drive_low();
}

// --------------------------------------------------------------------------------

/**
 * @brief Slot to receive a new ir-command
 * The ir-command is stored temporarily and will be processed within task-schedule.
 * 
 * @param p_arg pointer to the new ir-command of type IR_COMMON_COMMAND_TYPE
 */
static void ir_remote_task_slot_IR_CMD_RECEIVED(const void* p_arg) {

    const IR_COMMON_COMMAND_TYPE* p_command = (const IR_COMMON_COMMAND_TYPE*) p_arg;

    if (IR_COMMAND_QUEUE_is_full()) {
        DEBUG_PASS("ir_remote_task_slot_IR_CMD_RECEIVED() - QUEUE IS FULL !!!");
        return;
    }

    IR_COMMAND_QUEUE_enqeue(p_command);
    // IR_REMOTE_TASK_STATUS_set(IR_REMOTE_TASK_STATUS_CMD_PENDING | IR_REMOTE_TASK_STATUS_CMD_RECEIVED);

    DEBUG_TRACE_byte(p_command->type, "ir_remote_task_slot_IR_CMD_RECEIVED() - ir_command.type");
}

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(IR_CMD_RECEIVED_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(IR_CMD_RECEIVED_SIGNAL, IR_CMD_RECEIVED_SLOT, ir_remote_task_slot_IR_CMD_RECEIVED)

// --------------------------------------------------------------------------------

/**
 * @see  ir_protocol_task.h#ir_protocol_init
 * 
 */
void ir_protocol_init(void) {

    DEBUG_PASS("ir_remote_app_task_init()");

    IR_CARRIER_IN_no_pull();
    IR_CARRIER_OUT_drive_low();
    IR_MOD_OUT_drive_low();

    timer_carrier.init();
    timer_modulator.init();

    IR_CMD_RECEIVED_SIGNAL_init();
    IR_CMD_RECEIVED_SIGNAL_set_timeout(0);
    IR_CMD_RECEIVED_SLOT_connect();

    IR_COMMAND_QUEUE_init();
    
    #ifdef HAS_IR_PROTOCOL_JVC
    {
        DEBUG_PASS("ir_remote_task_init() - JVC");
        ir_protocol_jvc_init();
    }
    #endif

    #ifdef HAS_IR_PROTOCOL_NEC
    {
        DEBUG_PASS("ir_remote_task_init() - NEC");
        ir_protocol_nec_init();
    }
    #endif

    #ifdef HAS_IR_PROTOCOL_SONY
    {
        DEBUG_PASS("ir_remote_task_init() - SONY");
        ir_protocol_sony_init();
    }
    #endif

    #ifdef HAS_IR_PROTOCOL_SAMSUNG
    {
        DEBUG_PASS("ir_remote_task_init() - SAMSUNG");
        ir_protocol_samsung_init();
    }
    #endif

    IR_PROTOCOL_TASK_init();
    
    task_state = IR_PROTOCOL_TASK_STATE_IDLE;
}

// --------------------------------------------------------------------------------

/**
 * @see  app_task/ir_remote_mcu_task.h#ir_protocol_interface_register_ir_protocol
 */
void ir_protocol_interface_register_ir_protocol(IR_PROTOCOL_GENERATOR_TYPE* p_ir_protocol) {

    p_ir_protocol->_p_next = 0;

    if (p_ir_protocol_first == 0) {

        p_ir_protocol_first = p_ir_protocol;
        p_ir_protocol_last = p_ir_protocol;

    } else {

        p_ir_protocol_last->_p_next = p_ir_protocol;
        p_ir_protocol_last = p_ir_protocol;
    }

    // p_ir_protocol_last->set_timer(&timer_carrier, &timer_modulator);

    DEBUG_TRACE_byte(p_ir_protocol->uid, "ir_protocol_interface_register_ir_protocol() - new ir-protocol added");
}

// --------------------------------------------------------------------------------

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.init
 */
static void ir_remote_task_init(void) {

    DEBUG_PASS("ir_remote_task_init()");
    // IR_REMOTE_TASK_STATUS_clear_all();
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_schedule_interval
 */
static u16 ir_remote_task_get_schedule_interval(void) {

    if (task_state != IR_PROTOCOL_TASK_STATE_IDLE) {
        return 0;

    } else if (IR_COMMAND_QUEUE_is_empty() == 0) {
        return MCU_TASK_RUNNING;

    } else {
        return IR_REMOTE_TASK_RUN_INTERVAL_MS;
    }
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_state
 */
static MCU_TASK_INTERFACE_TASK_STATE ir_remote_task_get_state(void) {

    if (task_state != IR_PROTOCOL_TASK_STATE_IDLE) {
        return MCU_TASK_RUNNING;
    }

    if (IR_COMMAND_QUEUE_is_empty() == 0) {
        return MCU_TASK_RUNNING;
    }
    
    return MCU_TASK_SLEEPING;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.run
 */
static void ir_remote_task_run(void) {

    switch (task_state) {

        case IR_PROTOCOL_TASK_STATE_IDLE:

            if (IR_COMMAND_QUEUE_is_empty()) {
                break;
            }

            IR_COMMAND_QUEUE_deqeue(&ir_command);

            DEBUG_PASS("ir_remote_task_run() - IDLE -> LOAD_PROTOCOL");
            task_state = IR_PROTOCOL_TASK_STATE_LOAD_PROTOCOL;

            // no break;
            // fall trough

        case IR_PROTOCOL_TASK_STATE_LOAD_PROTOCOL:
        
            ir_protocol_task_load_protocol(p_act_protocol, &ir_command);

            if (p_act_protocol == 0) {

                DEBUG_TRACE_byte(
                    ir_command.type,
                    "ir_remote_task_run() - UNKNOWN PROTOCOL - LOAD_PROTOCOL -> IDLE"
                );

                task_state = IR_PROTOCOL_TASK_STATE_IDLE;
                break;
            }

            DEBUG_PASS("ir_remote_task_run() - LOAD_PROTOCOL -> START");
            task_state = IR_PROTOCOL_TASK_STATE_START;
            break;

        case IR_PROTOCOL_TASK_STATE_START:

            ir_remote_task_transmit_start(p_act_protocol, &ir_command);

            DEBUG_PASS("ir_remote_task_run() - START -> TRANSMIT");
            task_state = IR_PROTOCOL_TASK_STATE_TRANSMIT;
            break;

        case IR_PROTOCOL_TASK_STATE_TRANSMIT:

            if (p_act_protocol->transmit_finished() == 0) {
                break;
            }

            DEBUG_PASS("ir_remote_task_run() - TRANSMIT -> FINISH");
            task_state = IR_PROTOCOL_TASK_STATE_FINISH;

            // no break;
            // fall through

        case IR_PROTOCOL_TASK_STATE_FINISH:
        
            ir_protocol_task_finish();

            DEBUG_PASS("ir_remote_task_run() - FINISH -> IDLE");
            task_state = IR_PROTOCOL_TASK_STATE_IDLE;
            break;
    }
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.terminate
 */
static void ir_remote_task_terminate(void) {
    // do nothing
}

// --------------------------------------------------------------------------------
