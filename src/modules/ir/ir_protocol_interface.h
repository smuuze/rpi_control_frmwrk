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

#define IR_PROTOCOL_TYPE_SAMSUNG                                                0x01
#define IR_PROTOCOL_TYPE_JVC                                                    0x02
#define IR_PROTOCOL_TYPE_SONY                                                   0x03
#define IR_PROTOCOL_TYPE_NEC                                                    0x04

#define IR_PROTOCOL_TYPE_INVALID                                                0xFF

// --------------------------------------------------------------------------------

#ifndef IR_PROTOCOL_INTERFACE_TRANSMIT_BUFFER_MAX_LENGTH
#define IR_PROTOCOL_INTERFACE_TRANSMIT_BUFFER_MAX_LENGTH                        512
#endif // IR_PROTOCOL_INTERFACE_TRANSMIT_BUFFER_MAX_LENGTH

#define IR_PROTOCOL_INTERFACE_TRANSMIT_INTERVAL_PULSE                           0x01
#define IR_PROTOCOL_INTERFACE_TRANSMIT_INTERVAL_PAUSE                           0x00

// --------------------------------------------------------------------------------

#define IR_COMMON_COMMAND_DATA_INVALID                                          0XFF

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

// --------------------------------------------------------------------------------

/**
 * @brief Requests the access to the transmit buffer.
 * If the access is granted the maximum length of the buffer is returned.
 * If the buffer is busy right now, 0 is returned.
 * 
 * @return length of the transmit buffer in number of bytes, or 0 if buffer is busy right now
 */
u16 ir_protocol_interface_transmit_buffer_request(void);

/**
 * @brief Releases the transmit buffer.
 * The buffer then can be used by other ir-protocol implementations.
 * 
 */
void ir_protocol_interface_transmit_buffer_release(void);

/**
 * @brief Checks if the common transmit buffer is busy right now.
 * 
 * @return 0 the buffer is not busy and can be used, otherwise 0
 */
u8 ir_protocol_interface_transmit_buffer_busy(void);

/**
 * @brief Resets the transmit buffer.
 * New data is appended at the beginning.
 * Existing data is lost
 * 
 */
void ir_protocol_interface_transmit_buffer_reset(void);

/**
 * @brief Restarts the transmit buffer.
 * Existing data can be read once again.
 * Data is not lost.
 * If start_position is above the maximum buffer size or the actual
 * length of the buffer reset will be ignored and the buffer remains at its actual position.
 * 
 * @param start_position position where to restart from
 * 
 */
void ir_protocol_interface_transmit_buffer_restart(u16 start_position);

/**
 * @brief Checks if the transmit-buffer has data left or not.
 * 
 * @return 1 data is still available, 0 no more data available
 */
u8 ir_protocol_interface_transmit_buffer_end(void);

/**
 * @brief Appends a pulse interval to the transmit-buffer
 * 
 */
void ir_protocol_interface_transmit_buffer_append_pulse(void);

/**
 * @brief Appends a pause-interval to the transmit buffer
 * 
 */
void ir_protocol_interface_transmit_buffer_append_pause(void);

/**
 * @brief After the buffer was restarted and all data has appended
 * The next interval, starting from the beginning, is returned.
 * If the end of the actual buffer-content is reached,
 * a pause-interval is always returned.
 * 
 * @return The next interval (pulse / pause) is returned
 */
u8 ir_protocol_interface_transmit_buffer_get_next(void);

/**
 * @brief The number of actual appended intervals
 * 
 * @return The number of actual appended intervals
 */
u16 ir_protocol_interface_transmit_buffer_act_length(void);

/**
 * @brief The maximum length of the transmit-buffer in number of bytes
 * 
 * @return The maximum length of the transmit-buffer in number of bytes
 */
u16 ir_protocol_interface_transmit_buffer_max_length(void);

// --------------------------------------------------------------------------------

#endif // _H_ir_protocol_interface_

// --------------------------------------------------------------------------------
