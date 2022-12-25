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

#define IR_REMOTE_TASK_STATUS_TX_ACTIVE             (1 << 0)
#define IR_REMOTE_TASK_STATUS_CMD_PENDING           (1 << 1)
#define IR_REMOTE_TASK_STATUS_CMD_RECEIVED          (1 << 2)
#define IR_REMOTE_TASK_STATUS_SAMSUNG_CMD_RECEIVED  (1 << 3)
#define IR_REMOTE_TASK_STATUS_JVC_CMD_RECEIVED      (1 << 4)
#define IR_REMOTE_TASK_STATUS_SONY_CMD_RECEIVED     (1 << 5)

BUILD_MODULE_STATUS_U8(IR_REMOTE_TASK_STATUS)

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

// --------------------------------------------------------------------------------

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.init
 * 
 */
static void ir_remote_task_init(void);

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_schedule_interval
 * 
 */
static u16 ir_remote_task_get_schedule_interval(void);

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_state
 * 
 */
static MCU_TASK_INTERFACE_TASK_STATE ir_remote_task_get_state(void);

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_state
 * 
 */
static void ir_remote_task_run(void);

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.terminate
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
 * @brief temporarily sotre the new ir-command.
 * The command is processed within the task-schedule
 * 
 */
static IR_COMMON_COMMAND_TYPE ir_command;

// --------------------------------------------------------------------------------

/**
 * @brief Pointer to the actual active protocol
 * 
 */
static IR_PROTOCOL_GENERATOR_TYPE* p_act_protocol = 0;

// --------------------------------------------------------------------------------

/**
 * @brief Slot to receive a new ir-command
 * The ir-command is stored temporarily and will be processed within task-schedule.
 * 
 * @param p_arg pointer to the new ir-command of type IR_COMMON_COMMAND_TYPE
 */
static void ir_remote_task_slot_IR_CMD_RECEIVED(const void* p_arg) {

    const IR_COMMON_COMMAND_TYPE* p_command = (const IR_COMMON_COMMAND_TYPE*) p_arg;

    if (IR_REMOTE_TASK_STATUS_is_set(IR_REMOTE_TASK_STATUS_CMD_PENDING)) {
        DEBUG_TRACE_byte(p_command->type, "ir_remote_task_slot_IR_CMD_RECEIVED() - Cannot proecess IR-Command");
        DEBUG_TRACE_byte(ir_command.type, "ir_remote_task_slot_IR_CMD_RECEIVED() - IR-Command still pending");
        return;
    }

    ir_command.type = p_command->type;
    ir_command.data_1 = p_command->data_1;
    ir_command.data_2 = p_command->data_2;
    ir_command.data_3 = p_command->data_3;
    ir_command.data_4 = p_command->data_4;

    IR_REMOTE_TASK_STATUS_set(IR_REMOTE_TASK_STATUS_CMD_PENDING | IR_REMOTE_TASK_STATUS_CMD_RECEIVED);

    DEBUG_TRACE_byte(ir_command.type, "ir_remote_task_slot_IR_CMD_RECEIVED() - ir_command.type");
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
    IR_CMD_RECEIVED_SLOT_connect();
    
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
}

// --------------------------------------------------------------------------------

/**
 * @see  app_task/ir_remote_mcu_task.h#ir_protocol_interface_register_ir_protocol
 * 
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

    p_ir_protocol_last->set_timer(&timer_carrier, &timer_modulator);

    DEBUG_TRACE_byte(p_ir_protocol->uid, "ir_protocol_interface_register_ir_protocol() - new ir-protocol added");
}

// --------------------------------------------------------------------------------

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.init
 * 
 */
static void ir_remote_task_init(void) {

    DEBUG_PASS("ir_remote_task_init()");
    IR_REMOTE_TASK_STATUS_clear_all();
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_schedule_interval
 * 
 */
static u16 ir_remote_task_get_schedule_interval(void) {

    if (IR_REMOTE_TASK_STATUS_is_set(IR_REMOTE_TASK_STATUS_TX_ACTIVE | IR_REMOTE_TASK_STATUS_CMD_PENDING)) {
        return 0;
    } else {
        return IR_REMOTE_TASK_RUN_INTERVAL_MS;
    }
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_state
 * 
 */
static MCU_TASK_INTERFACE_TASK_STATE ir_remote_task_get_state(void) {

    if (IR_REMOTE_TASK_STATUS_is_set(IR_REMOTE_TASK_STATUS_TX_ACTIVE | IR_REMOTE_TASK_STATUS_CMD_PENDING)) {
        return MCU_TASK_RUNNING;
    }
    
    return MCU_TASK_SLEEPING;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.run
 * 
 */
static void ir_remote_task_run(void) {

    u8 is_active = 0;

    if (IR_REMOTE_TASK_STATUS_is_set(IR_REMOTE_TASK_STATUS_CMD_RECEIVED)) {

        if (IR_REMOTE_TASK_STATUS_is_set(IR_REMOTE_TASK_STATUS_TX_ACTIVE) == 0) {

            DEBUG_TRACE_byte(ir_command.type, "ir_remote_task_run() - Start IR-Command");

            IR_REMOTE_TASK_STATUS_set(IR_REMOTE_TASK_STATUS_TX_ACTIVE);

            p_act_protocol = p_ir_protocol_first;

            while (p_act_protocol != 0) {

                if (p_act_protocol->uid == ir_command.type) {

                    p_act_protocol->transmit(&ir_command);
                    is_active = 1;
                    break;
                }

                p_act_protocol = p_act_protocol->_p_next;
            }
                
            if (is_active == 0) {
                DEBUG_TRACE_byte(ir_command.type, "ir_remote_task_run() - unknown IR-Protocol");
                IR_REMOTE_TASK_STATUS_unset(IR_REMOTE_TASK_STATUS_CMD_RECEIVED);
            }

        } else  if (p_act_protocol != 0 && p_act_protocol->is_busy()) {
            is_active = 1;

        } else {

            DEBUG_PASS("ir_remote_task_run() - IR-Command finished");
            IR_REMOTE_TASK_STATUS_unset(IR_REMOTE_TASK_STATUS_CMD_RECEIVED | IR_REMOTE_TASK_STATUS_CMD_PENDING);
            p_act_protocol = 0;
        }
    }

    if (is_active == 0) {

        IR_CARRIER_IN_no_pull();
        IR_CARRIER_OUT_drive_low();
        IR_MOD_OUT_drive_low();

        //DEBUG_PASS("ir_remote_task_run() - All operations finished");
        IR_REMOTE_TASK_STATUS_unset(IR_REMOTE_TASK_STATUS_TX_ACTIVE);
    }
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.terminate
 * 
 */
static void ir_remote_task_terminate(void) {
    // do nothing
}

// --------------------------------------------------------------------------------
