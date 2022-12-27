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
 * @file   ir_protocol_nec.c
 * @author Sebastian Lesse
 * @date   2021 / 12 / 28
 * @brief  Implementation of the NEC ir-protocol
 * 
 *          Frequency:          36 kHz
 * 
 *          Preamble:           9000us Pulse
 *                              4500us Pause
 * 
 *          Logical-One:        560us Pulse
 *                              1690 s Pause
 * 
 *          Logical Zero:       560us Pulse
 *                              560us Pause
 * 
 *          Stop-Bit:           560us Pulse
 * 
 *          Concept:
 * 
 *          The given command is transformed into a byte representation.
 *          The transformed command is stored into a buffer that holds  the number of 560us intervals.
 *          The number of intervals depends on the actual command. Four intervals for a logical one
 *          and two intervals for a logical zero. Every interval is represented by a single byte.
 * 
 *          The correct frequency of the carrier and the modulator is automatically selected.
 * 
 *          For every interval that is set to one the modulation pin is activated,
 *          for every 0 interval the pin is deactivated
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

#include "modules/ir/ir_protocol_interface.h"

// --------------------------------------------------------------------------------

#include "modules/ir/ir_protocol_nec.h"

// --------------------------------------------------------------------------------

/**
 * @brief Number of bits that will be added to the transfer-buffer
 * Here we have one byte for every bit.
 * 
 */
#define NEC_IR_PROTOCOL_TRANSMIT_BUFFER_SIZE            128

/**
 * @brief Number of bits that are used for the NEC ir-protocol
 * 
 */
#define NEC_IR_PROTOCOL_TRANSMIT_BUFFER_BIT_COUNT       32

#define NEC_IR_PROTOCOL_TRNASMIT_BUFFER_BYTE_COUNT      4

#define NEC_IR_PROTOCOL_INTERVAL_PREAMBLE_PULSE         16
#define NEC_IR_PROTOCOL_INTERVAL_PREAMBLE_PAUSE         (8)
#define NEC_IR_PROTOCOL_INTERVAL_STOP_BIT               (1)

#define NEC_IR_PROTOCOL_MOD_TIME_OFF                    0xFFFF

/**
 * @brief Declares a new data-buffer with the given name
 * and applies the command into the correct byte-positions
 * 
 */
#define NEC_IR_PROTOCOL_NEW_DATA_BUFFER(name, p_ir_command)                     \
            u8 name[NEC_IR_PROTOCOL_TRNASMIT_BUFFER_BYTE_COUNT];                \
            name[0] = p_ir_command->data_1;                                     \
            name[1] = p_ir_command->data_1 ^ 0xFF;                              \
            name[2] = p_ir_command->data_2;                                     \
            name[3] = p_ir_command->data_2 ^ 0xFF

// --------------------------------------------------------------------------------

/**
 * @brief If a transmission is ongoing this variable
 * will be set to 1 to prevent a caller starting a new transmission
 * 
 */
static u8 transmit_active = 0;

// --------------------------------------------------------------------------------

/**
 * @brief Prepares the transmit-buffer.
 * The bits from the given command are copied into the transmit_buffer.
 * Every byte of the transmit_buffer represents one bit of the given command.
 * 
 * @param p_command valid ir command that will be transmitted
 * @return 1 if the transmit-buffer is ready, otherwise 0
 */
static u8 ir_protocol_nec_prepare_transmit_buffer(IR_COMMON_COMMAND_TYPE* p_command) {

    if (ir_protocol_interface_transmit_buffer_request() == 0) {
        DEBUG_PASS("ir_protocol_nec_prepare_transmit_buffer() - Transmit-Buffer is busy!");
        return 0;
    }

    DEBUG_TRACE_byte(p_command->data_1, "ir_protocol_nec_prepare_transmit_buffer() - Device Address:");
    DEBUG_TRACE_byte(p_command->data_2, "ir_protocol_nec_prepare_transmit_buffer() - Device Control:");

    ir_protocol_interface_transmit_buffer_reset();

    // Append the preamble pulse - 9000us = 16 x 560us

    u8 i = 0;
    for ( ; i < NEC_IR_PROTOCOL_INTERVAL_PREAMBLE_PULSE; i++) {
        ir_protocol_interface_transmit_buffer_append_pulse();
    }

    // append the preamble pause - 4500us = 8 x 560us

    i = 0;
    for ( ; i < NEC_IR_PROTOCOL_INTERVAL_PREAMBLE_PAUSE; i++) {
        ir_protocol_interface_transmit_buffer_append_pause();
    }

    // append the data-bits

    u8 bit_mask = 0x80;
    u8 byte_index = 0;

    NEC_IR_PROTOCOL_NEW_DATA_BUFFER(data_buffer, p_command);

    i = 0;
    for ( ; i < NEC_IR_PROTOCOL_TRANSMIT_BUFFER_BIT_COUNT; i++ ) {

        if (bit_mask == 0) {
            bit_mask = 0x80;
            byte_index += 1;
        }

        if (data_buffer[byte_index] & bit_mask) {

            // Data-Bit 1:
            ir_protocol_interface_transmit_buffer_append_pulse();
            ir_protocol_interface_transmit_buffer_append_pause();
            ir_protocol_interface_transmit_buffer_append_pause();
            ir_protocol_interface_transmit_buffer_append_pause();

            //DEBUG_PASS("ir_protocol_nec_prepare_transmit_buffer() - DATA-BIT 1");

        } else {

            // Data-Bit : 0
            ir_protocol_interface_transmit_buffer_append_pulse();
            ir_protocol_interface_transmit_buffer_append_pause();

            //DEBUG_PASS("ir_protocol_nec_prepare_transmit_buffer() - DATA-BIT 0");
        }

        bit_mask = bit_mask >> 1;
    }

    i = 0;
    for ( ; i < NEC_IR_PROTOCOL_INTERVAL_STOP_BIT; i++) {
        ir_protocol_interface_transmit_buffer_append_pulse();
        ir_protocol_interface_transmit_buffer_append_pause();
    }

    return 1;
}

// --------------------------------------------------------------------------------

/**
 * @see ir_protocol_interface.h#IR_PROTOCOL_INTERFACE_GET_FREQUENCY_CALLBACK
 */
static TIMER_CONFIGURAITON_FREQUENCY ir_protocol_nec_get_frequency(void) {
    DEBUG_PASS("ir_protocol_nec_get_frequency() - TIMER_FREQUENCY_37_9kHz");
    return TIMER_FREQUENCY_37_9kHz;
}

/**
 * @see ir_protocol_interface.h#IR_PROTOCOL_INTERFACE_GET_MODUALTION_INTERVAL_CALLBACK
 */
static TIMER_CONFIGURATION_TIME_INTERVAL ir_protocol_nec_get_mod_interval(void) {
    DEBUG_PASS("ir_protocol_nec_get_mod_interval() - TIMER_TIME_INTERVAL_527us");
    return TIMER_TIME_INTERVAL_560us;
}

/**
 * @see ir_protocol_interface.h#IR_PROTOCOL_GENERATOR_TYPE.transmit
 */
static void ir_protocol_nec_transmit_prepare(IR_COMMON_COMMAND_TYPE* p_command) {

    if (transmit_active != 0) {

        DEBUG_PASS("ir_protocol_nec_transmit_prepare() - Transmit guard is already set !!! ---");
        return;
    }

    transmit_active = 1;

    DEBUG_PASS("ir_protocol_nec_transmit_prepare()");

    if (ir_protocol_nec_prepare_transmit_buffer(p_command) == 0) {
        transmit_active = 0;
        return;
    }
}

/**
 * @see ir_protocol_interface.h#IR_PROTOCOL_INTERFACE_TRANSMIT_START_CALLBACK
 */
static void ir_protocol_nec_transmit_start(void) {
    DEBUG_PASS("ir_protocol_nec_transmit_start()");
    transmit_active = 1;
    IR_MOD_OUT_drive_low();
}

/**
 * @see ir_protocol_interface.h#IR_PROTOCOL_INTERFACE_TRANSMIT_IRQ_CALLBACK
 */
static void ir_protocol_nec_transmit_irq(void) {

    if (ir_protocol_interface_transmit_buffer_end() == 0) {

        if (IR_PROTOCOL_IS_PULSE(ir_protocol_interface_transmit_buffer_get_next())) {

            IR_MOD_OUT_drive_high(); 

        } else {

            IR_MOD_OUT_drive_low(); 
        }

    } else if (transmit_active != 0) {

        transmit_active = 0;
        ir_protocol_interface_transmit_buffer_release();

        DEBUG_PASS("ir_protocol_nec_irq_callback() - FINISHED");
    }
}

/**
 * @see ir_protocol_interface.h#IR_PROTOCOL_INTERFACE_TRANSMIT_FINISHED_CALLBACK
 */
static u8 ir_protocol_nec_transmit_finished(void) {
    return transmit_active == 0;
}

// --------------------------------------------------------------------------------

/**
 * @brief Interface to this ir-protocol implementation.
 * Is used for register this implementation to the ir-handler module.
 */
IR_PROTOCOL_CREATE (
    IR_PROTO_NEC,
    IR_PROTOCOL_TYPE_NEC,
    ir_protocol_nec_get_frequency,
    ir_protocol_nec_get_mod_interval,
    ir_protocol_nec_transmit_prepare,
    ir_protocol_nec_transmit_start,
    ir_protocol_nec_transmit_irq,
    ir_protocol_nec_transmit_finished
)

// --------------------------------------------------------------------------------

void ir_protocol_nec_init(void) {
        DEBUG_PASS("ir_protocol_nec_init()");
        IR_PROTO_NEC_init();
}

// --------------------------------------------------------------------------------
