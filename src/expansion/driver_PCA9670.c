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
 * @file    driver_PCA9670.c
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

#include "common/local_module_status.h"

#include "driver/cfg_driver_interface.h"
#include "system/system_interface.h"
#include "time_management/time_management.h"

#include "expansion/driver_PCA9670.h"

#include "mcu_task_management/mcu_task_interface.h"

// --------------------------------------------------------------------------------

#ifndef PCA9670_REQUEST_DRIVER_TIMEOUT_MS
#define PCA9670_REQUEST_DRIVER_TIMEOUT_MS                                   100
#endif

#ifndef PCA9670_OPERATION_TIMEOUT_MS
#define PCA9670_OPERATION_TIMEOUT_MS                                        250
#endif

#ifndef PCA9670_DRIVER_REQUEST_TIMEOUT_MS
#define PCA9670_DRIVER_REQUEST_TIMEOUT_MS                                   250
#endif

#ifndef PCA9670_COMMAND_BUFFER_SIZE
#define PCA9670_COMMAND_BUFFER_SIZE                                         10
#endif

// --------------------------------------------------------------------------------

#define PCA9670_WRITE_IO_COMMAND_LENGTH                                     1
#define PCA9670_GEN_COMMAND_WRITE_IO(p_instance, p_buffer)                  p_buffer[0] = (p_instance->direction_mask | p_instance->level_mask)

#define PCA9670_READ_IO_ANSWER_LENGTH                                       1

#define PCA9670_STATUS_PIN_CHANGED                                          (1 << 0)

// --------------------------------------------------------------------------------

/*!
 *
 */
typedef enum {
    PCA9670_TASK_STATE_IDLE,
    PCA9670_TASK_STATE_REQUEST_DRIVER,
    PCA9670_TASK_STATE_CANCEL,
    PCA9670_TASK_STATE_REQUEST_FINISH,
    PCA9670_TASK_STATE_WRITE_IO,
    PCA9670_TASK_STATE_READ_IO,
    PCA9670_TASK_STATE_PROCESS_DATA,
    PCA9670_TASK_STATE_LOAD_NEXT_INSTANCE
} PCA9670_TASK_STATE;

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(task_timer)
TIME_MGMN_BUILD_STATIC_TIMER_U16(operation_timer)

BUILD_MODULE_STATUS_U8(pca9670_status)

// --------------------------------------------------------------------------------

/*!
 *
 */
static TRX_DRIVER_INTERFACE* p_com_driver = 0;    // pointer to call functions from driver through the interface

/*!
 *
 */
static TRX_DRIVER_CONFIGURATION driver_cfg;

/*!
 *
 */
static PCA9670_INSTANCE_TYPE* p_first_instance = 0;

/*!
 *
 */
static PCA9670_INSTANCE_TYPE* p_last_instance = 0;

/*!
 *
 */
static PCA9670_INSTANCE_TYPE* p_act_instance = 0;

/*!
 *
 */
static MCU_TASK_INTERFACE_TASK_STATE task_state = MCU_TASK_TERMINATED;

/*!
 *
 */
static PCA9670_TASK_STATE operation_stage = PCA9670_TASK_STATE_IDLE;

/*!
 *
 */
static u8 com_driver_mutex_id = 0;

// --------------------------------------------------------------------------------

/*!
 *
 */
static u8 _update_mask(u8 act_mask, u8 pin, u8 value) {

    u8 new_mask = act_mask;

    if (value != 0) {
        new_mask |= (1 << pin);
    } else {
        new_mask &= ~(1 << pin);
    }

    return new_mask;
}

/*!
 *
 */
static void _update_instance(PCA9670_INSTANCE_TYPE* p_instance, u8 new_pin_values) {

    DEBUG_TRACE_byte(new_pin_values, "_update_instance() - new_pin_values:"); //
    p_instance->pin_values = new_pin_values;
}

/*!
 *
 */
static u8 _bit_isset(u8 vector, u8 bit) {
    return (vector & (1 << bit)) != 0 ? 1 : 0;
}

// --------------------------------------------------------------------------------

/*!
 *
 */
static void pca9670_task_start(void) {
    
    operation_stage = PCA9670_TASK_STATE_IDLE;
    task_state = MCU_TASK_SLEEPING;
}

static u16 pca9670_task_get_schedule_interval(void) {
    return PCA9670_TASK_RUN_INTERVAL_MS;
}

/*!
 *
 */
static MCU_TASK_INTERFACE_TASK_STATE pca9670_task_get_state(void) {

    if (task_state == MCU_TASK_SLEEPING) {

        if (pca9670_status_is_set(PCA9670_STATUS_PIN_CHANGED)) {
            task_state = MCU_TASK_RUNNING;
            
        } else if (task_timer_is_up(PCA9670_TASK_RUN_INTERVAL_MS) /* i_system.time.isup_u16(task_run_interval_reference_actual, SHT31_TASK_RUN_INTERVAL_MS) */ != 0) {
            task_state = MCU_TASK_RUNNING;
        }
    }

    return task_state;
}

/*!
 *
 */
static void pca9670_task_execute(void) {

    #ifdef TRACES_ENABLED
    u16 actual_time = i_system.time.now_u16();
    DEBUG_TRACE_word(actual_time, "pca9670_task_execute() -----------------"); // 
    #endif
        
    u8 command_buffer[PCA9670_COMMAND_BUFFER_SIZE];

    switch (operation_stage) {

        default:
            operation_stage = PCA9670_TASK_STATE_IDLE;
            // no break;

        case PCA9670_TASK_STATE_IDLE :
            
            if (p_first_instance == 0) {
                DEBUG_PASS("pca9670_task_execute() - No instance registered - FINISH !!! ---"); // 
                operation_stage = PCA9670_TASK_STATE_REQUEST_FINISH;
                break;
            }
            
            DEBUG_PASS("pca9670_task_execute() - PCA9670_TASK_STATE_IDLE - Processing IO-States"); // 
            
            p_act_instance = p_first_instance;
            operation_stage = PCA9670_TASK_STATE_REQUEST_DRIVER;
            operation_timer_start();
            // no break;
        
        case PCA9670_TASK_STATE_REQUEST_DRIVER :
        
            if (operation_timer_is_up(PCA9670_REQUEST_DRIVER_TIMEOUT_MS) != 0) {
                DEBUG_PASS("pca9670_task_execute() - PCA9670_TASK_STATE_REQUEST_DRIVER - Waiting for driver request has timed out !!! ---"); // 
                operation_stage = PCA9670_TASK_STATE_CANCEL;
                break;
            }

            if ((com_driver_mutex_id = p_com_driver->mutex_req()) == 0) {
                DEBUG_PASS("pca9670_task_execute() - PCA9670_TASK_STATE_REQUEST_DRIVER - Can't get Mutex of communication-driver"); // 
                break;
            }

            p_com_driver->configure(&driver_cfg);
            operation_timer_start();
            operation_stage = PCA9670_TASK_STATE_WRITE_IO;
            
            if (pca9670_status_is_set(PCA9670_STATUS_PIN_CHANGED) == 0) {
                DEBUG_PASS("pca9670_task_execute() - PCA9670_TASK_STATE_REQUEST_DRIVER - No Pin-Change -> skip writing IO");
                operation_stage = PCA9670_TASK_STATE_READ_IO;
                break;
            }
            
            pca9670_status_unset(PCA9670_STATUS_PIN_CHANGED);
            
            // no break;
            
        case PCA9670_TASK_STATE_WRITE_IO :
        
            if (operation_timer_is_up(PCA9670_DRIVER_REQUEST_TIMEOUT_MS) != 0) {
                DEBUG_PASS("pca9670_task_execute() - PCA9670_TASK_STATE_WRITE_IO - Waiting for driver request has timed out !!! ---"); // 
                operation_stage = PCA9670_TASK_STATE_CANCEL;
                break;
            }

            if (p_com_driver->is_ready_for_tx() == 0) {
                DEBUG_PASS("pca9670_task_execute() - PCA9670_TASK_STATE_WRITE_IO - Waiting for communication-driver"); // 
                break;
            }
                
            DEBUG_TRACE_byte(p_act_instance->address, "pca9670_task_execute() - PCA9670_TASK_STATE_WRITE_IO - Going to write new direction states"); // 
            
            PCA9670_GEN_COMMAND_WRITE_IO(p_act_instance, command_buffer);

            p_com_driver->clear_rx_buffer();
            p_com_driver->clear_tx_buffer();
            
            p_com_driver->set_address(p_act_instance->address);
            p_com_driver->set_N_bytes(PCA9670_WRITE_IO_COMMAND_LENGTH, command_buffer);
            p_com_driver->start_tx();
            
            operation_timer_start();
            operation_stage = PCA9670_TASK_STATE_READ_IO;    
            // no break;
            
        case PCA9670_TASK_STATE_READ_IO :
        
            if (operation_timer_is_up(PCA9670_OPERATION_TIMEOUT_MS) != 0) {
                DEBUG_PASS("pca9670_task_execute() - PCA9670_TASK_STATE_READ_IO - Waiting for driver operation has timed out !!! ---"); // 
                operation_stage = PCA9670_TASK_STATE_CANCEL;
                p_com_driver->stop_tx();
                break;
            }

            if (p_com_driver->is_ready_for_rx() == 0) {
                DEBUG_PASS("pca9670_task_execute() - PCA9670_TASK_STATE_READ_IO - Waiting for communication-driver"); // 
                break;
            }
            
            DEBUG_TRACE_byte(p_act_instance->address, "pca9670_task_execute() - PCA9670_TASK_STATE_WRITE_IO - Going to read actual pin level from instance "); // 

            p_com_driver->set_address(p_act_instance->address);
            p_com_driver->start_rx(PCA9670_READ_IO_ANSWER_LENGTH);
            
            operation_timer_start();
            operation_stage = PCA9670_TASK_STATE_PROCESS_DATA;    
            // no break;
            
        case  PCA9670_TASK_STATE_PROCESS_DATA :
        
            if (operation_timer_is_up(PCA9670_OPERATION_TIMEOUT_MS) != 0) {
                DEBUG_PASS("pca9670_task_execute() - PCA9670_TASK_STATE_READ_IO - Waiting for driver operation has timed out !!! ---"); // 
                operation_stage = PCA9670_TASK_STATE_CANCEL;
                p_com_driver->stop_tx();
                break;
            }

            if (p_com_driver->bytes_available() == 0) {
                //DEBUG_PASS("pca9670_task_execute() - PCA9670_TASK_STATE_READ_IO - Waiting for bytes to be received"); // 
                break;
            }            
            
            DEBUG_TRACE_word(operation_timer_elapsed(), "pca9670_task_execute() - PCA9670_TASK_STATE_READ_IO - Bytes received after [ms]: "); // 
            p_com_driver->get_N_bytes(PCA9670_READ_IO_ANSWER_LENGTH, command_buffer);
            
            _update_instance(p_act_instance, command_buffer[0]);
            
            // no break;
            
        case PCA9670_TASK_STATE_LOAD_NEXT_INSTANCE :
        
            if (p_act_instance->next != 0) {
                
                DEBUG_PASS("pca9670_task_execute() - PCA9670_TASK_STATE_LOAD_NEXT_INSTANCE - Loading next instance"); // 
                
                p_act_instance = p_act_instance->next;
                operation_stage = PCA9670_TASK_STATE_WRITE_IO;                
                operation_timer_start();                
                break;
            }
                
            DEBUG_PASS("pca9670_task_execute() - PCA9670_TASK_STATE_LOAD_NEXT_INSTANCE - No more instance available"); //             
            operation_stage = PCA9670_TASK_STATE_CANCEL;                
            // no break;
            
        case PCA9670_TASK_STATE_CANCEL :
        
            operation_stage = PCA9670_TASK_STATE_REQUEST_FINISH;
            p_com_driver->shut_down();
            p_com_driver->mutex_rel(com_driver_mutex_id);
            // no break;
        
        case PCA9670_TASK_STATE_REQUEST_FINISH :
        
            task_timer_start();
            operation_stage = PCA9670_TASK_STATE_IDLE;
            task_state = MCU_TASK_SLEEPING;
            
            DEBUG_PASS("pca9670_task_execute() - PCA9670_TASK_STATE_REQUEST_FINISH - Task finished -------------------------------"); // 
            
            break;
    }
}

/*!
 *
 */
static void pca9670_task_terminate(void) {

}

TASK_CREATE(
    PCA9670_TASK,
    TASK_PRIORITY_MIDDLE,
    pca9670_task_get_schedule_interval,
    pca9670_task_start,
    pca9670_task_execute,
    pca9670_task_get_state,
    pca9670_task_terminate
)

// --------------------------------------------------------------------------------

void pca9670_init(TRX_DRIVER_INTERFACE* p_driver) {

    DEBUG_PASS("pca9670_init()"); // 

    driver_cfg.module.i2c.answer_general_call = 0;
    driver_cfg.module.i2c.bit_rate = DRIVER_I2C_BITRATE_1KHZ;
    driver_cfg.module.i2c.enable_ack = 0;
    driver_cfg.module.i2c.interrupt_enable = 1;
    driver_cfg.module.i2c.is_master = 1;
    driver_cfg.module.i2c.slave_addr = 0;

    p_com_driver = p_driver;
    
    pca9670_status_clear_all();
    pca9670_status_set(PCA9670_STATUS_PIN_CHANGED);

    PCA9670_TASK_init();
}

void pca9670_register_module(PCA9670_INSTANCE_TYPE* p_instance) {

    DEBUG_PASS("pca9670_register_module()"); // 
    
    if (p_first_instance == 0) {
    
        DEBUG_PASS("pca9670_register_module() - This is the first module"); // 
        p_first_instance = p_instance;
        p_last_instance  = p_instance;
        
    } else {
    
        p_last_instance->next = p_instance;
        p_last_instance = p_last_instance->next;
    }

    p_last_instance->next = 0;
}

void pca9670_set_direction(u8 instance_address, u8 instance_pin_number, u8 new_direction) {

    DEBUG_TRACE_byte(instance_address, "pca9670_set_direction() - instance address"); // 
    DEBUG_TRACE_byte(instance_pin_number, "pca9670_set_direction() - pin number"); // 

    PCA9670_INSTANCE_TYPE* p_act = p_first_instance;

    while (p_act != 0) {

        if (p_act->address != instance_address) {
            p_act = p_act->next;
            continue;
        }
        
        p_act->direction_mask  = _update_mask(p_act->direction_mask, instance_pin_number, new_direction);
        DEBUG_TRACE_byte(p_act->direction_mask, "pca9670_set_direction() - New State"); //
        
        pca9670_status_set(PCA9670_STATUS_PIN_CHANGED);

        return;
    }
}

u8 pca9670_get_direction(u8 instance_address, u8 instance_pin_number) {

    PCA9670_INSTANCE_TYPE* p_act = p_first_instance;

    while (p_act != 0) {

        if (p_act->address != instance_address) {
            p_act = p_act->next;
            continue;
        }
    
        if (_bit_isset(p_act->direction_mask, instance_pin_number) != 0) {
            return PCA9670_DIRECTION_OUTPUT;
        } else {
            return PCA9670_DIRECTION_INPUT;
        }
    }
    
    return PCA9670_DIRECTION_UNDEFINED;
}

void pca9670_set_state(u8 instance_address, u8 instance_pin_number, u8 new_state) {
        
    DEBUG_TRACE_byte(instance_address, "pca9670_set_state() - instance address"); // 
    DEBUG_TRACE_byte(instance_pin_number, "pca9670_set_state() - pin number"); // 

    PCA9670_INSTANCE_TYPE* p_act = p_first_instance;

    while (p_act != 0) {

        if (p_act->address != instance_address) {
            p_act = p_act->next;
            continue;
        }

        p_act->level_mask  = _update_mask(p_act->level_mask, instance_pin_number, new_state);
        DEBUG_TRACE_byte(p_act->level_mask, "pca9670_set_state() - New State"); // 
        
        pca9670_status_set(PCA9670_STATUS_PIN_CHANGED);

        break;
    }
}

u8 pca9670_get_state(u8 instance_address, u8 instance_pin_number) {

    PCA9670_INSTANCE_TYPE* p_act = p_first_instance;

    while (p_act != 0) {

        if (p_act->address != instance_address) {
            p_act = p_act->next;
            continue;
        }
    
        if (_bit_isset(p_act->pin_values, instance_pin_number) != 0) {
            return PCA9670_STATE_HIGH;
        } else {
            return PCA9670_STATE_LOW;
        }
    }
    
    return PCA9670_STATE_UNKNOWN;
}

// --------------------------------------------------------------------------------
