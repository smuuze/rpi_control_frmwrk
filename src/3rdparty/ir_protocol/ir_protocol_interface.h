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
 * @file   ir_protocol_interface.h
 * @author Sebastian Lesse
 * @date   2021 / 12 / 30
 * @brief  Interface to realize a ir-protocol generation module.
 * 
 *      Usage:
 * 
 *      - Define callback structure:
 * 
 *              IR_PROTOCOL_GENERATOR ir_protocol_module = {
 *                      .uid = UNIQUE_IR_PROTOCOL_IDENTIFIER,
 *                      .set_timer = p_callback_set_timer,
 *                      .transmit = &p_callback_transmit,
 *                      .is_busy = &p_callback_is_busy,
 *                      ._p_next = 0
 *              };
 * 
 *      - Register module:
 * 
 *              ir_protocol_interface_register_ir_protocol(&ir_protocol_module);
 * 
 *      - Generate ir-command:
 * 
 *              IR_COMMON_COMMAND_TYPE ir_command = {
 *                      .type = UNIQUE_IR_PROTOCOL_IDENTIFIER,
 *                      .data_1 = ir_protocol_data_1,
 *                      .data_2 = ir_protocol_data_2,
 *                      .data_3 = ir_protocol_data_3,
 *                      .data_4 = ir_protocol_data_4,
 *              };
 * 
 *      - Send ir-command via signal-slot-interface
 * 
 *              IR_CMD_RECEIVED_SIGNAL_send(&ir_command);
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_ir_protocol_interface_
#define _H_ir_protocol_interface_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "driver/timer/timer_interface.h"
#include "common/signal_slot_interface.h"

// --------------------------------------------------------------------------------

#define IR_PROTOCOL_TYPE_SAMSUNG        0x01
#define IR_PROTOCOL_TYPE_JVC            0x02
#define IR_PROTOCOL_TYPE_SONY           0x03
#define IR_PROTOCOL_TYPE_NEC            0x04

// --------------------------------------------------------------------------------

/**
 * @brief Common ir-command type taht can hold data of various ir-protocols
 * This type is used to transfer a generated ir-command from the command-handler
 * to the ir-protocol-generator
 * 
 */
typedef struct IR_COMMON_COMMAND {

        /**
         * @brief unique identifier to identify the ir-protocol
         * to use for this command
         * 
         */
        u8 type;

        /**
         * @brief common data area 1
         * 
         */
        u8 data_1;

        /**
         * @brief common data area 2
         * 
         */
        u8 data_2;

        /**
         * @brief common data area 3
         * 
         */
        u8 data_3;

        /**
         * @brief common data area 4
         * 
         */
        u8 data_4;

} IR_COMMON_COMMAND_TYPE;

// --------------------------------------------------------------------------------

/**
 * @brief Callback time to set for the ir-protocol-generator
 * 
 */
typedef void (*IR_PROTOCOL_INTERFACE_SET_TIMER_CALLBACK) (TIMER_INTERFACE_TYPE* p_timer_carrier, TIMER_INTERFACE_TYPE* p_timer_modulator);

/**
 * @brief Callback time to start transmission of a ir-command
 * 
 */
typedef void (*IR_PROTOCOL_INTERFACE_TRANSMIT_CALLBACK) (IR_COMMON_COMMAND_TYPE* p_ir_command);

/**
 * @brief Callback to check if the transmission of a ir-command is still ongoing
 * 
 */
typedef u8 (*IR_PROTOCOL_INTERFACE_SET_TIMERCALLBACK) (void);

/**
 * @brief 
 * 
 */
typedef struct IR_PROTOCOL_GENERATOR {

        /**
         * @brief unique id of each ir-protocol
         * 
         */
	u8 uid;

        /**
         * @brief Callback time to set for the ir-protocol-generator
         * 
         */
	IR_PROTOCOL_INTERFACE_SET_TIMER_CALLBACK set_timer;

        /**
         * @brief Callback time to start transmission of a ir-command
         * 
         */
	IR_PROTOCOL_INTERFACE_TRANSMIT_CALLBACK transmit;

        /**
         * @brief Callback to check if the transmission of a ir-command is still ongoing
         * 
         */
	IR_PROTOCOL_INTERFACE_SET_TIMERCALLBACK is_busy;

        /**
         * @brief Pointer to the next ir-protocol if available.
         * Must be set to 0 if no more ir-protocol is available
         * 
         */
        struct IR_PROTOCOL_GENERATOR* _p_next;

} IR_PROTOCOL_GENERATOR_TYPE;

// --------------------------------------------------------------------------------

SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(IR_CMD_RECEIVED_SIGNAL)

// --------------------------------------------------------------------------------

/**
 * @brief Registers a new IR-protocol.
 * The new protocol is added to the lsit of avilable ir-protocols and is respected on processing a new ir-command.
 * The new protocol is always added to the end of the list.
 * The next-pointer of the new element is always set to 0.
 * 
 * @param p_ir_protocol valid pointer to the new ir-protocol, must not be 0
 */
void ir_protocol_interface_register_ir_protocol(IR_PROTOCOL_GENERATOR_TYPE* p_ir_protocol);

#endif // _H_ir_protocol_interface_

// --------------------------------------------------------------------------------
