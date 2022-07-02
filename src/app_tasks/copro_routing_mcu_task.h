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
 * @file    copro_routing_mcu_task.h
 * @author  Sebastian Lesse
 * @date    2022 / 02 / 20
 * @brief   Sends a given byte-array to the requested copro-unit
 *          and waits for the response of this copro-unit.
 *          There can always only one copor-unit be processed at a time.
 *          Commands for other copro-units are stored temporarily until
 *          this task has finished the actual ongoing processing.
 * 
 * Input signals:
 * 
 * - COPRO1_ROUTING_COMMAND_SIGNAL      command as byte-array that will be transfered to the copro1.
 *                                      This signal is only available if the copor1 is available
 *                                      (#ifdef COPRO1_AVAILABLE)
 * 
 * 
 * Output signals
 * 
 * - COPRO1_ROUTING_RESPONSE_SIGNAL     response as byte-array that was received by copro1.
 *                                      A timeout will also be send via this signal.
 *                                      This signal is only available if the copor1 is available
 *                                      (#ifdef COPRO1_AVAILABLE)
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_copro_routing_mcu_task_
#define _H_copro_routing_mcu_task_

// --------------------------------------------------------------------------------

#include "mcu_task_interface.h"

// --------------------------------------------------------------------------------

/**
 * @brief Interval at which the task is executed
 * 
 */
#ifndef COPRO_ROUTING_TASK_RUN_INTERVAL_MS
#define COPRO_ROUTING_TASK_RUN_INTERVAL_MS              0
#endif

/**
 * @brief 
 * 
 */
#ifndef COPRO_ROUTING_TASK_POWER_UP_TIMEOUT_MS
#define COPRO_ROUTING_TASK_POWER_UP_TIMEOUT_MS          5
#endif

/**
 * @brief Timeout in milliseconds of the copro preapre phase
 * E.g. request copro-mutex
 * 
 */
#ifndef COPRO_ROUTING_TASK_PREPARE_TIMEOUT_MS
#define COPRO_ROUTING_TASK_PREPARE_TIMEOUT_MS           250
#endif

/**
 * @brief Timeout in milliseconds to wait for a response
 * from the copor-interface
 * 
 */
#ifndef COPRO_ROUTING_TASK_PROCESS_TIMEOUT_MS
#define COPRO_ROUTING_TASK_PROCESS_TIMEOUT_MS           250
#endif

/**
 * @brief Timeout in milliseconds of the copro transmit phase
 * E.g. The copro-interface is not ready to transmit the commands
 * 
 */
#ifndef COPRO_ROUTING_TASK_TRANSMIT_TIMEOUT_MS
#define COPRO_ROUTING_TASK_TRANSMIT_TIMEOUT_MS          50
#endif

/**
 * @brief Interval in milliseconds the copro-interface
 * is polled for incomming responses
 * 
 */
#ifndef COPRO_ROUTING_TASK_POLLING_INTERVAL_MS
#define COPRO_ROUTING_TASK_POLLING_INTERVAL_MS          70
#endif

/**
 * @brief Maximum number of bytes of the
 * response of the copro-interfaces
 * 
 */
#ifndef COPRO_ROUTING_TASK_TEMP_DATA_BUFFER_SIZE
#define COPRO_ROUTING_TASK_TEMP_DATA_BUFFER_SIZE        64
#endif

// --------------------------------------------------------------------------------

/*!
 *
 */
void copro_routing_task_init(void);

/*!
 *
 */
u16 copro_routing_task_get_schedule_interval(void);

/*!
 *
 */
MCU_TASK_INTERFACE_TASK_STATE copro_routing_task_get_state(void);

/*!
 *
 */
void copro_routing_task_run(void);

/*!
 *
 */
void copro_routing_task_background_run(void);

/*!
 *
 */
void copro_routing_task_sleep(void);

/*!
 *
 */
void copro_routing_task_wakeup(void);

/*!
 *
 */
void copro_routing_task_finish(void);

/*!
 *
 */
void copro_routing_task_terminate(void);

// --------------------------------------------------------------------------------

#endif // _H_copro_routing_mcu_task_

// --------------------------------------------------------------------------------
