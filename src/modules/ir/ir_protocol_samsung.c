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
 * @file   ir_protocol_samsung.c
 * @author Sebastian Lesse
 * @date   2021 / 12 / 30
 * @brief  Implements the Samsung IR-protocol signal generation
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

#include "mcu_task_management/mcu_task_interface.h"
#include "time_management/time_management.h"

#include "driver/timer/timer_interface.h"

#include "common/common_tools_bit_vector.h"

// --------------------------------------------------------------------------------

#include "modules/ir/ir_protocol_interface.h"

// --------------------------------------------------------------------------------

#include "modules/ir/ir_protocol_samsung.h"

// --------------------------------------------------------------------------------

                                                        // empirical    // correct
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_START_PREAMBLE_US  4500            /* 4500 */
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_START_PAUSE_US     4500            /* 4500 */
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_START_US      560             /* 560 */
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_0_US          560             /* 560 */
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_1_US          1690            /* 1690 */
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_STOP_BIT_START_MS  560             /* 560 */
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_STOP_BIT_PAUSE_MS  560             /* 560 */

#define SAMSUNG_IR_PROTOCOL_MOD_TIME_OFF                0xFFFF

#define IR_PROTOCOL_SAMSUNG_PREAMBLE_PULSE_COUNT        8
#define IR_PROTOCOL_SAMSUNG_PREAMBLE_PAUSE_COUNT        8
#define IR_PROTOCOL_SAMSUNG_STOPBIT_PULSE_COUNT         1

#define IR_PROTOCOL_SAMSUNG_BIT_LEGNTH_ADDRESS_WORD     8
#define IR_PROTOCOL_SAMSUNG_BIT_LENGTH_COMMAND_WORD     8

#define IR_PROTOCOL_SAMSUNG_BIT_MASK_ADRESS_WORD        0x80
#define IR_PROTOCOL_SAMSUNG_BIT_MASK_COMMAND_WORD       0x80

// --------------------------------------------------------------------------------

/*
 *
 */
static TIMER_INTERFACE_TYPE* p_carrier;

/*
 *
 */
static TIMER_INTERFACE_TYPE* p_modulator;

/*
 *
 */
static u8 transmit_guard = 0;

// --------------------------------------------------------------------------------

void ir_protocol_samsung_irq_callback(void) {

    if (ir_protocol_interface_transmit_buffer_end() == 0) {

        if (IR_PROTOCOL_IS_PULSE(ir_protocol_interface_transmit_buffer_get_next())) {

            IR_MOD_OUT_drive_high(); 

        } else {

            IR_MOD_OUT_drive_low();
        }

    } else if (transmit_guard != 0) {

        IR_MOD_OUT_drive_low();

        p_carrier->stop();
        p_modulator->stop();

        transmit_guard = 0;
        ir_protocol_interface_transmit_buffer_release();

        DEBUG_PASS("ir_protocol_samsung_irq_callback() - IR Command finished!");
    }
}

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
static void ir_protocol_samsung_prepare_transmit_buffer_fill_buffer(
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

/**
 * @brief 
 * 
 * @param p_command 
 */
static void ir_protocol_samsung_prepare_transmit_buffer(
    IR_COMMON_COMMAND_TYPE* p_command
) {

    if (ir_protocol_interface_transmit_buffer_request() == 0) {
        DEBUG_PASS(
            "ir_protocol_samsung_prepare_transmit_buffer()"
            " - Transmit-Buffer is busy!"
        );
        return;
    }

    ir_protocol_interface_transmit_buffer_reset();

    for(u8 i = 0; i < IR_PROTOCOL_SAMSUNG_PREAMBLE_PULSE_COUNT; ++i) {
        ir_protocol_interface_transmit_buffer_append_pulse();
    }

    for(u8 i = 0; i < IR_PROTOCOL_SAMSUNG_PREAMBLE_PAUSE_COUNT; ++i) {
        ir_protocol_interface_transmit_buffer_append_pause();
    }

    // adress
    ir_protocol_samsung_prepare_transmit_buffer_fill_buffer(
        IR_PROTOCOL_SAMSUNG_BIT_LEGNTH_ADDRESS_WORD,
        IR_PROTOCOL_SAMSUNG_BIT_MASK_ADRESS_WORD,
        p_command->data_1
    );

    // adress (repeat)
    ir_protocol_samsung_prepare_transmit_buffer_fill_buffer(
        IR_PROTOCOL_SAMSUNG_BIT_LEGNTH_ADDRESS_WORD,
        IR_PROTOCOL_SAMSUNG_BIT_MASK_ADRESS_WORD,
        p_command->data_1
    );

    // command
    ir_protocol_samsung_prepare_transmit_buffer_fill_buffer(
        IR_PROTOCOL_SAMSUNG_BIT_LENGTH_COMMAND_WORD,
        IR_PROTOCOL_SAMSUNG_BIT_MASK_COMMAND_WORD,
        p_command->data_2
    );

    // command (inverted)
    ir_protocol_samsung_prepare_transmit_buffer_fill_buffer(
        IR_PROTOCOL_SAMSUNG_BIT_LENGTH_COMMAND_WORD,
        IR_PROTOCOL_SAMSUNG_BIT_MASK_COMMAND_WORD,
        BIT_VECTOR_INVERT_U8(p_command->data_2)
    );

    for(u8 i = 0; i < IR_PROTOCOL_SAMSUNG_STOPBIT_PULSE_COUNT; ++i) {
        ir_protocol_interface_transmit_buffer_append_pulse();
    }
}

// --------------------------------------------------------------------------------

/**
 * @see ir_protocol_interface.h#IR_PROTOCOL_GENERATOR_TYPE.is_busy
 */
static u8 ir_protocol_samsung_is_busy(void) {
    return transmit_guard != 0;
}

/**
 * @see ir_protocol_interface.h#IR_PROTOCOL_GENERATOR_TYPE.set_timer
 */
static void ir_protocol_samsung_set_timer(
    TIMER_INTERFACE_TYPE* p_timer_carrier,
    TIMER_INTERFACE_TYPE* p_timer_modulator
) {
    DEBUG_PASS("ir_protocol_samsung_set_timer()");
    p_carrier = p_timer_carrier;
    p_modulator = p_timer_modulator;
}

/**
 * @see ir_protocol_interface.h#IR_PROTOCOL_GENERATOR_TYPE.transmit
 */
static void ir_protocol_samsung_transmit(IR_COMMON_COMMAND_TYPE* p_command) {

    if (transmit_guard != 0) {

        DEBUG_PASS(
            "ir_protocol_samsung_transmit()"
            " - Transmit guard is already set !!! ---"
        );
        return;
    }

    transmit_guard = 1;

    p_carrier->stop();
    p_modulator->stop();

    DEBUG_TRACE_word
        (p_command->data_1,
        "ir_protocol_samsung_transmit() - Device Address:"
    );

    DEBUG_TRACE_word(
        p_command->data_2,
        "ir_protocol_samsung_transmit() - Device Control:"
    );

    ir_protocol_samsung_prepare_transmit_buffer(p_command);

    IR_MOD_OUT_drive_low();

    TIMER_CONFIGURATION_TYPE timer_config;
    
    timer_config.frequency = TIMER_FREQUENCY_NONE;
    timer_config.irq_callback = &ir_protocol_samsung_irq_callback;
    timer_config.mode = TIMER_MODE_TIMER;
    timer_config.time_interval = TIMER_TIME_INTERVAL_560us;

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
 */
static IR_PROTOCOL_GENERATOR_TYPE ir_protocol_samsung = {
        .uid = IR_PROTOCOL_TYPE_SAMSUNG,
        .set_timer = &ir_protocol_samsung_set_timer,
        .transmit = &ir_protocol_samsung_transmit,
        .is_busy = &ir_protocol_samsung_is_busy,
        ._p_next = 0
};

// --------------------------------------------------------------------------------

/**
 * @see ir_protocol_samsung.h#ir_protocol_samsung
 */
void ir_protocol_samsung_init(void) {
        DEBUG_PASS("ir_protocol_samsung_init()");
        ir_protocol_interface_register_ir_protocol(&ir_protocol_samsung);
}

// --------------------------------------------------------------------------------
