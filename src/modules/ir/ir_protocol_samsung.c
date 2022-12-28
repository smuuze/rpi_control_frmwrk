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

/**
 * @brief If is set to 1 the transmission of the current ir-command
 * is still ongoing. Is set to 0 in ir_protocol_samsung_transmit_irq
 * after the whole command has finished.
 * 
 */
static u8 transmit_active = 0;

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
static inline void ir_protocol_samsung_prepare_transmit_buffer_fill_buffer(
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
static inline void ir_protocol_samsung_prepare_transmit_buffer(
    const IR_COMMON_COMMAND_TYPE* p_command
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
 * @see ir_protocol_interface.h#IR_PROTOCOL_INTERFACE_GET_FREQUENCY_CALLBACK
 */
static TIMER_CONFIGURAITON_FREQUENCY ir_protocol_samsung_get_frequency(void) {
    DEBUG_PASS("ir_protocol_samsung_get_frequency() - TIMER_FREQUENCY_37_9kHz");
    return TIMER_FREQUENCY_37_9kHz;
}

/**
 * @see ir_protocol_interface.h#IR_PROTOCOL_INTERFACE_GET_MODUALTION_INTERVAL_CALLBACK
 */
static TIMER_CONFIGURATION_TIME_INTERVAL ir_protocol_samsung_get_mod_interval(void) {
    DEBUG_PASS("ir_protocol_samsung_get_mod_interval() - TIMER_TIME_INTERVAL_560us");
    return TIMER_TIME_INTERVAL_560us;
}

/**
 * @see ir_protocol_interface.h#IR_PROTOCOL_GENERATOR_TYPE.transmit
 */
static void ir_protocol_samsung_transmit_prepare(const IR_COMMON_COMMAND_TYPE* p_command) {

    if (transmit_active != 0) {

        DEBUG_PASS(
            "ir_protocol_samsung_transmit()"
            " - Transmit guard is already set !!! ---"
        );
        return;
    }

    DEBUG_PASS("ir_protocol_samsung_transmit_prepare()");

    transmit_active = 1;

    ir_protocol_samsung_prepare_transmit_buffer(p_command);
}

/**
 * @see ir_protocol_interface.h#IR_PROTOCOL_INTERFACE_TRANSMIT_START_CALLBACK
 */
static void ir_protocol_samsung_transmit_start(void) {
    DEBUG_PASS("ir_protocol_samsung_transmit_start()");
    transmit_active = 1;
    IR_MOD_OUT_drive_low();
}

/**
 * @see ir_protocol_interface.h#IR_PROTOCOL_INTERFACE_TRANSMIT_IRQ_CALLBACK
 */
void ir_protocol_samsung_transmit_irq(void) {

    if (ir_protocol_interface_transmit_buffer_end() == 0) {

        if (IR_PROTOCOL_IS_PULSE(ir_protocol_interface_transmit_buffer_get_next())) {

            IR_MOD_OUT_drive_high(); 

        } else {

            IR_MOD_OUT_drive_low();
        }

    } else if (transmit_active != 0) {

        IR_MOD_OUT_drive_low();

        transmit_active = 0;
        ir_protocol_interface_transmit_buffer_release();

        DEBUG_PASS("ir_protocol_samsung_irq_callback() - IR Command finished!");
    }
}

/**
 * @see ir_protocol_interface.h#IR_PROTOCOL_INTERFACE_TRANSMIT_FINISHED_CALLBACK
 */
static u8 ir_protocol_samsung_transmit_finished(void) {
    return transmit_active == 0;
}

// --------------------------------------------------------------------------------

/**
 * @brief Interface to this ir-protocol implementation.
 * Is used for register this implementation to the ir-handler module.
 */
IR_PROTOCOL_CREATE (
    IR_PROTO_SAMSUNG,
    IR_PROTOCOL_TYPE_SAMSUNG,
    ir_protocol_samsung_get_frequency,
    ir_protocol_samsung_get_mod_interval,
    ir_protocol_samsung_transmit_prepare,
    ir_protocol_samsung_transmit_start,
    ir_protocol_samsung_transmit_irq,
    ir_protocol_samsung_transmit_finished
)

// --------------------------------------------------------------------------------

/**
 * @see ir_protocol_samsung.h#ir_protocol_samsung
 */
void ir_protocol_samsung_init(void) {
        DEBUG_PASS("ir_protocol_samsung_init()");
        IR_PROTO_SAMSUNG_init();
}

// --------------------------------------------------------------------------------
