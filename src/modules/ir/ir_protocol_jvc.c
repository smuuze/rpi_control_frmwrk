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
 * @file    ir_protocol_jvc.c
 * @author  Sebastian Lesse
 * @date    2022 / 12 / 19
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

#include "modules/ir/ir_protocol_interface.h"

// --------------------------------------------------------------------------------

#include "modules/ir/ir_protocol_jvc.h"

// --------------------------------------------------------------------------------

#define JVC_IR_PROTOCOL_WORD_TRANSMIT_COUNT                         3

#define JVC_IR_PROTOCOL_MOD_TIME_START_PREAMBLE_US                  8440
#define JVC_IR_PROTOCOL_MOD_TIME_START_PAUSE_US                     4220
#define JVC_IR_PROTOCOL_MOD_TIME_DATA_START_US                      527
#define JVC_IR_PROTOCOL_MOD_TIME_DATA_0_US                          527
#define JVC_IR_PROTOCOL_MOD_TIME_DATA_1_US                          1583
#define JVC_IR_PROTOCOL_MOD_TIME_STOP_BIT_START_MS                  527
#define JVC_IR_PROTOCOL_MOD_TIME_STOP_BIT_PAUSE_MS                  527

#define JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PULSE          (16)
#define JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PAUSE          (8)
#define JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_STOP_BIT                (1 + JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PAUSE)
#define JVC_IR_PROTOCOLL_MODE_TIME_STEP_WORD_CYCLE                  (88)
#define JVC_IR_PROTOCOLL_MODE_TIME_STEP_WORD_CYCLE_SHORT            (81)

// --------------------------------------------------------------------------------

#define JVC_IR_PROTOCOL_ADDRESS_BIT_COUNT                           8
#define JVC_IR_PROTOCOL_START_BIT_MASK_ADDRESS                      0x80

#define JVC_IR_PROTOCOL_COMMAND_BIT_COUNT                           8
#define JVC_IR_PROTOCOL_START_BIT_MASK_COMMAND                      0x80

// --------------------------------------------------------------------------------

/*
 *
 */
static u8 transmit_active = 0;

/**
 * @brief The frame needs to be repeated for three times.
 * This variable will count the number of repetitions.
 * @see JVC_IR_PROTOCOL_WORD_TRANSMIT_COUNT
 */
static u8 word_transmit_counter = 0;

/**
 * @brief Between two frames a pause cycle needs to take place.
 * This variable will count the number of modualtion time intervals to repsect this pause.
 * @see JVC_IR_PROTOCOLL_MODE_TIME_STEP_WORD_CYCLE_SHORT
 */
static u8 word_cycle_interval_counter = 0;

// --------------------------------------------------------------------------------

/**
 * @brief Appends pulse/pause to the transmit-buffer of the ir-interface depending
 * on the bit-values of bit_vector. For each bit is correct sequence of pulse/pause
 * added to the transmit-buffer using pulse-wide modulation for coding of logical 1/0
 * 
 * @param length number of data-bits to append to the transmit-buffer
 * @param start_bit_mask start-bit mask to skip bits, will be shifted down (>> 1)
 *        to extracts the bits of bit_vector
 * @param bit_vector bits that will be added as pulse/pause to the transmit-buffer
 */
static void ir_protocol_jvc_prepare_transmit_buffer_fill_buffer(
    u8 length,
    u8 start_bit_mask,
    u8 bit_vector
) {

    u8 bit_mask = start_bit_mask;
    u8 i = 0;

    for ( ; i < length; i++) {

        if (bit_vector & bit_mask) {

            // Data-Bit 1:
            ir_protocol_interface_transmit_buffer_append_pulse();
            ir_protocol_interface_transmit_buffer_append_pause();
            ir_protocol_interface_transmit_buffer_append_pause();
            ir_protocol_interface_transmit_buffer_append_pause();

        } else {

            // Data-Bit : 0
            ir_protocol_interface_transmit_buffer_append_pulse();
            ir_protocol_interface_transmit_buffer_append_pause();

        }

        bit_mask = bit_mask >> 1;
    }
}

// --------------------------------------------------------------------------------

static inline void ir_protocol_jvc_prepare_transmit_buffer(
    const IR_COMMON_COMMAND_TYPE* p_command
) {

    if (ir_protocol_interface_transmit_buffer_request() == 0) {
        DEBUG_PASS("ir_protocol_jvc_prepare_transmit_buffer() - Transmit-Buffer is busy!");
        return;
    }

    ir_protocol_interface_transmit_buffer_reset();

    // preamble pusle
    for (u8 i = 0; i < JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PULSE; i++) {
        ir_protocol_interface_transmit_buffer_append_pulse();
    }

    // Preamble Pause
    for (u8 i = 0; i < JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PAUSE; i++) {
        ir_protocol_interface_transmit_buffer_append_pause();
    }

    // Address Bits
    ir_protocol_jvc_prepare_transmit_buffer_fill_buffer(
        JVC_IR_PROTOCOL_ADDRESS_BIT_COUNT,
        JVC_IR_PROTOCOL_START_BIT_MASK_ADDRESS,
        p_command->data_1
    );

    // Command Bits
    ir_protocol_jvc_prepare_transmit_buffer_fill_buffer(
        JVC_IR_PROTOCOL_COMMAND_BIT_COUNT,
        JVC_IR_PROTOCOL_START_BIT_MASK_COMMAND,
        p_command->data_2
    );

    // Stop-Bit Pulse
    ir_protocol_interface_transmit_buffer_append_pulse();

    DEBUG_TRACE_word (
        ir_protocol_interface_transmit_buffer_act_length(),
        "ir_protocol_jvc_prepare_transmit_buffer() - Length of Transmit-Buffer"
    );
}

// --------------------------------------------------------------------------------

/**
 * @see ir_protocol_interface.h#IR_PROTOCOL_INTERFACE_GET_FREQUENCY_CALLBACK
 */
static TIMER_CONFIGURAITON_FREQUENCY ir_protocol_jvc_get_frequency(void) {
    DEBUG_PASS("ir_protocol_jvc_get_frequency() - TIMER_FREQUENCY_37_9kHz");
    return TIMER_FREQUENCY_37_9kHz;
}

/**
 * @see ir_protocol_interface.h#IR_PROTOCOL_INTERFACE_GET_MODUALTION_INTERVAL_CALLBACK
 */
static TIMER_CONFIGURATION_TIME_INTERVAL ir_protocol_jvc_get_mod_interval(void) {
    DEBUG_PASS("ir_protocol_jvc_get_mod_interval() - TIMER_TIME_INTERVAL_527us");
    return TIMER_TIME_INTERVAL_527us;
}

/**
 * @see ir_protocol_interface.h#IR_PROTOCOL_INTERFACE_TRANSMIT_PREPARE_CALLBACK
 */
static void ir_protocol_jvc_transmit_prepare(const IR_COMMON_COMMAND_TYPE* p_command) {

    if (transmit_active != 0) {

        DEBUG_PASS("ir_protocol_jvc_transmit() - Transmit guard is already set !!! ---");
        return;
    }

    transmit_active = 1;

    word_transmit_counter = 0;

    DEBUG_TRACE_word(p_command->data_1, "ir_protocol_jvc_transmit() - Device Address:");
    DEBUG_TRACE_word(p_command->data_2, "ir_protocol_jvc_transmit() - Device Control:");

    ir_protocol_jvc_prepare_transmit_buffer(p_command);

    word_cycle_interval_counter = JVC_IR_PROTOCOLL_MODE_TIME_STEP_WORD_CYCLE
                                + JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PULSE
                                + JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PAUSE;
}

/**
 * @see ir_protocol_interface.h#IR_PROTOCOL_INTERFACE_TRANSMIT_START_CALLBACK
 */
static void ir_protocol_jvc_transmit_start(void) {
    DEBUG_PASS("ir_protocol_jvc_transmit_start()");
    transmit_active = 1;
    IR_MOD_OUT_drive_low();
}

/**
 * @see ir_protocol_interface.h#IR_PROTOCOL_INTERFACE_TRANSMIT_IRQ_CALLBACK
 */
static void ir_protocol_jvc_transmit_irq(void) {

    if (ir_protocol_interface_transmit_buffer_end() == 0) {

        word_cycle_interval_counter -= 1;

        if (IR_PROTOCOL_IS_PULSE(ir_protocol_interface_transmit_buffer_get_next())) {

            IR_MOD_OUT_drive_high(); 

        } else {

            IR_MOD_OUT_drive_low();
        }

    } else if (word_transmit_counter < JVC_IR_PROTOCOL_WORD_TRANSMIT_COUNT ) {

        // The IR command is repeated for three times
        // The first frame was already sent
        // With a pause between the frames

        // Word-Cycle Pause
        word_cycle_interval_counter -= 1;
        IR_MOD_OUT_drive_low();

        if (word_cycle_interval_counter == 0) {

            DEBUG_TRACE_byte(
                word_transmit_counter,
                "ir_protocol_jvc_irq_callback() - Repeat Frame"
            );

            // start the transmit-buffer after the preamble pulse and pause
            ir_protocol_interface_transmit_buffer_restart(
                JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PULSE
                + JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PAUSE
            );

            word_transmit_counter += 1;
            word_cycle_interval_counter =
                JVC_IR_PROTOCOLL_MODE_TIME_STEP_WORD_CYCLE_SHORT;
        }

    } else if (transmit_active != 0) {

        IR_MOD_OUT_drive_low();

        transmit_active = 0;
        ir_protocol_interface_transmit_buffer_release();

        DEBUG_PASS("ir_protocol_jvc_irq_callback() - IR Command finished!");
    }
}

/**
 * @see ir_protocol_interface.h#IR_PROTOCOL_INTERFACE_TRANSMIT_FINISHED_CALLBACK
 */
static u8 ir_protocol_jvc_transmit_finished(void) {
    return transmit_active == 0;
}

// --------------------------------------------------------------------------------

/**
 * @brief Interface to this ir-protocol implementation.
 * Is used for register this implementation to the ir-handler module.
 */
IR_PROTOCOL_CREATE (
    IR_PROTO_JVC,
    IR_PROTOCOL_TYPE_JVC,
    ir_protocol_jvc_get_frequency,
    ir_protocol_jvc_get_mod_interval,
    ir_protocol_jvc_transmit_prepare,
    ir_protocol_jvc_transmit_start,
    ir_protocol_jvc_transmit_irq,
    ir_protocol_jvc_transmit_finished
)

// --------------------------------------------------------------------------------

/**
 * @see 3rdparty/ir_protocol/ir_protocol_sony.h#ir_protocol_jvc_init
 */
void ir_protocol_jvc_init(void) {
        DEBUG_PASS("ir_protocol_jvc_init()");
        IR_PROTO_JVC_init();
}

// --------------------------------------------------------------------------------
