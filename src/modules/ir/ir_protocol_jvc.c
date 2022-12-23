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

#include "mcu_task_management/mcu_task_interface.h"
#include "time_management/time_management.h"

#include "driver/timer/timer_interface.h"

// --------------------------------------------------------------------------------

#include "modules/ir/ir_protocol_interface.h"

// --------------------------------------------------------------------------------

#include "modules/ir/ir_protocol_jvc.h"

// --------------------------------------------------------------------------------

#ifndef JVC_IR_PROTOCOL_TRANSMIT_BUFFER_SIZE
#define JVC_IR_PROTOCOL_TRANSMIT_BUFFER_SIZE                        128
#endif

#ifndef JVC_IR_PROTOCOL_TRANSMIT_BUFFER_BIT_COUNT
#define JVC_IR_PROTOCOL_TRANSMIT_BUFFER_BIT_COUNT                   16
#endif

#ifndef JVC_IR_PROTOCOL_TRANSMIT_BUFFER_BYTE_COUNT
#define JVC_IR_PROTOCOL_TRANSMIT_BUFFER_BYTE_COUNT                  2
#endif

#ifndef JVC_IR_PROTOCOL_WORD_TRANSMIT_COUNT
#define JVC_IR_PROTOCOL_WORD_TRANSMIT_COUNT                         3
#endif

#define JVC_IR_PROTOCOL_MOD_TIME_START_PREAMBLE_US                  8440
#define JVC_IR_PROTOCOL_MOD_TIME_START_PAUSE_US                     4220
#define JVC_IR_PROTOCOL_MOD_TIME_DATA_START_US                      527
#define JVC_IR_PROTOCOL_MOD_TIME_DATA_0_US                          527
#define JVC_IR_PROTOCOL_MOD_TIME_DATA_1_US                          1583
#define JVC_IR_PROTOCOL_MOD_TIME_STOP_BIT_START_MS                  527
#define JVC_IR_PROTOCOL_MOD_TIME_STOP_BIT_PAUSE_MS                  527

#define JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PULSE          (16)
#define JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PAUSE          (8 + JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PULSE)
#define JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_STOP_BIT                (1 + JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PAUSE)
#define JVC_IR_PROTOCOLL_MODE_TIME_STEP_WORD_CYCLE                  (88)
#define JVC_IR_PROTOCOLL_MODE_TIME_STEP_WORD_CYCLE_SHORT            (81)

#define JVC_IR_PROTOCOL_MOD_TIME_OFF                                0xFFFF

#define JVC_IR_PROTOCOL_CMD_TO_BYTE_ARRAY(p_cmd) {              \
                                            p_cmd->data_1,     \
                                            p_cmd->data_2      \
                                        }

// --------------------------------------------------------------------------------

/**
 * @brief interface to the ir-signal carrier
 * Is configured before usage.
 * Will be started to transmit the signal.
 * And is stopped at the end of the transmission
 */
static TIMER_INTERFACE_TYPE* p_carrier;

/**
 * @brief interface to the ir-signal modaultor
 * Is configured before usage.
 * Will be started to transmit the signal.
 * And is stopped at the end of the transmission
 */
static TIMER_INTERFACE_TYPE* p_modulator;

/*
 *
 */
static u8 data_bit_length;

/*
 *
 */
static u8 data_bit_counter;

/**
 * @brief every byte represents a single bit
 * is used for a faster access
 * 
 */
static u8 transmit_buffer[JVC_IR_PROTOCOL_TRANSMIT_BUFFER_SIZE];

/*
 *
 */
static u8 transmit_guard = 0;

/*
 *
 */
static u8 irq_counter = 0;

/**
 * @brief counts the number of repeats of the data-word
 * 
 */
static u8 word_transmit_counter = 0;

/*
 *
 */
static u8 word_cycle_interval_counter = 0;

// --------------------------------------------------------------------------------

void ir_protocol_jvc_irq_callback(void) {

    if (irq_counter < JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PULSE) {

        // Preamble Pulse
        irq_counter += 1;
        IR_MOD_OUT_drive_high(); 

    } else if (irq_counter < JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PAUSE) {

        // Preamble Pause
        irq_counter += 1;
        IR_MOD_OUT_drive_low(); 

    } else if (data_bit_counter < data_bit_length) {

        word_cycle_interval_counter -= 1;

        // Data Bits

        if (transmit_buffer[data_bit_counter++]) {
            IR_MOD_OUT_drive_high(); 
        } else {
            IR_MOD_OUT_drive_low(); 
        }

    } else if (irq_counter < JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_STOP_BIT) {

        // Stop-Bit Pulse
        irq_counter += 1;
        word_cycle_interval_counter -= 1;

        IR_MOD_OUT_drive_high(); 

    } else if (word_transmit_counter < JVC_IR_PROTOCOL_WORD_TRANSMIT_COUNT) {

        // The IR command is repeated for three times

        // Word-Cycle Puase
        word_cycle_interval_counter -= 1;
        IR_MOD_OUT_drive_low();

        if (word_cycle_interval_counter == 0) {

            word_transmit_counter += 1;
            word_cycle_interval_counter =
                JVC_IR_PROTOCOLL_MODE_TIME_STEP_WORD_CYCLE_SHORT;

            data_bit_counter = 0;
            irq_counter = JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PAUSE;
        }    

    } else if (transmit_guard != 0) {

        IR_MOD_OUT_drive_low();

        p_carrier->stop();
        p_modulator->stop();

        irq_counter = 255;
        transmit_guard = 0;

        DEBUG_PASS("ir_protocol_jvc_irq_callback() - IR Command finished!");
    }
}

// --------------------------------------------------------------------------------

static void ir_protocol_jvc_prepare_transmit_buffer(
    IR_COMMON_COMMAND_TYPE* p_command
) {

    data_bit_length = 0;
    data_bit_counter = 0;

    u8 bit_mask = 0x80;
    u8 byte_index = 0;
    u8 data_buffer[JVC_IR_PROTOCOL_TRANSMIT_BUFFER_BYTE_COUNT] =
                JVC_IR_PROTOCOL_CMD_TO_BYTE_ARRAY(p_command);

    u8 i = 0;
    for ( ; i < JVC_IR_PROTOCOL_TRANSMIT_BUFFER_BIT_COUNT; i++ ) {

        if (bit_mask == 0) {
            bit_mask = 0x80;
            byte_index += 1;
        }

        if (data_buffer[byte_index] & bit_mask) {

            // Data-Bit 1:
            transmit_buffer[data_bit_length++] = 1;
            transmit_buffer[data_bit_length++] = 0;
            transmit_buffer[data_bit_length++] = 0;
            transmit_buffer[data_bit_length++] = 0;

        } else {

            // Data-Bit : 0
            transmit_buffer[data_bit_length++] = 1;
            transmit_buffer[data_bit_length++] = 0;

        }

        bit_mask = bit_mask >> 1;
    }

    // DEBUG_TRACE_N(
    //     data_bit_length,
    //     transmit_buffer,
    //     "ir_protocol_jvc_prepare_transmit_buffer() - Transmit-Buffer :"
    // );
}

// --------------------------------------------------------------------------------

u8 ir_protocol_jvc_is_busy(void) {
    return transmit_guard != 0;
}

// --------------------------------------------------------------------------------

void ir_protocol_jvc_set_timer(
    TIMER_INTERFACE_TYPE* p_timer_carrier,
    TIMER_INTERFACE_TYPE* p_timer_modulator
) {
    p_carrier = p_timer_carrier;
    p_modulator = p_timer_modulator;
}

// --------------------------------------------------------------------------------

void ir_protocol_jvc_transmit(IR_COMMON_COMMAND_TYPE* p_command) {

    if (transmit_guard != 0) {

        DEBUG_PASS("ir_protocol_jvc_transmit() - Transmit guard is already set !!! ---");
        return;
    }

    transmit_guard = 1;

    word_transmit_counter = 0;

    p_carrier->stop();
    p_modulator->stop();

    DEBUG_TRACE_word(p_command->data_1, "ir_protocol_jvc_transmit() - Device Address:");
    DEBUG_TRACE_word(p_command->data_2, "ir_protocol_jvc_transmit() - Device Control:");

    ir_protocol_jvc_prepare_transmit_buffer(p_command);

    irq_counter = 0;
    word_cycle_interval_counter = JVC_IR_PROTOCOLL_MODE_TIME_STEP_WORD_CYCLE;

    IR_MOD_OUT_drive_low();

    TIMER_CONFIGURATION_TYPE timer_config;
    
    timer_config.frequency = TIMER_FREQUENCY_NONE;
    timer_config.irq_callback = &ir_protocol_jvc_irq_callback;
    timer_config.mode = TIMER_MODE_TIMER;
    timer_config.time_interval = TIMER_TIME_INTERVAL_527us;

    p_modulator->configure(&timer_config);
    
    timer_config.frequency = TIMER_FREQUENCY_37_9kHz;
    timer_config.irq_callback = 0;
    timer_config.mode = TIMER_MODE_FREQUENCY;

    p_carrier->configure(&timer_config);
    
    p_carrier->start(TIME_CONFIGURATION_RUN_FOREVER);
    p_modulator->start(TIME_CONFIGURATION_RUN_FOREVER);
}

// --------------------------------------------------------------------------------

/**
 * @brief Interface to this ir-protocol implementation.
 * Is used for register this implementation to the ir-handler module.
 * 
 */
static IR_PROTOCOL_GENERATOR_TYPE ir_protocol_sony = {
        .uid = IR_PROTOCOL_TYPE_JVC,
        .set_timer = &ir_protocol_jvc_set_timer,
        .transmit = &ir_protocol_jvc_transmit,
        .is_busy = &ir_protocol_jvc_is_busy,
        ._p_next = 0
};

// --------------------------------------------------------------------------------

/**
 * @see 3rdparty/ir_protocol/ir_protocol_sony.h#ir_protocol_jvc_init
 * 
 */
void ir_protocol_jvc_init(void) {
        DEBUG_PASS("ir_protocol_jvc_init()");
        ir_protocol_interface_register_ir_protocol(&ir_protocol_sony);
}

// --------------------------------------------------------------------------------
