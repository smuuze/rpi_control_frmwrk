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
 * @file   ir_protocol_sony.c
 * @author Sebastian Lesse
 * @date   2021 / 03 / 20
 * @brief  Implementation of the SONY IR transmission protocol
 * 
 *          Concept:
 * 
 *          The given command is transformed into a byte representation.
 *          The transformed command is stored into a buffer that holds  the number of 600us intervals.
 *          The number of intervals depends on the actual command. Three intervals for a logical one
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

#include "mcu_task_management/mcu_task_interface.h"

// --------------------------------------------------------------------------------

#include "time_management/time_management.h"
#include "driver/timer/timer_interface.h"

// --------------------------------------------------------------------------------

#include "3rdparty/ir_protocol/ir_protocol_interface.h"

// --------------------------------------------------------------------------------

#include "3rdparty/ir_protocol/ir_protocol_sony.h"

// --------------------------------------------------------------------------------

#define SONY_IR_PROTOCOL_COMMAND_BIT_COUNT                      7
#define SONY_IR_PROTOCOL_DEVICE_BIT_COUNT                       5
#define SONY_IR_PROTOCOL_EXTENDED_BIT_COUNT                     8
#define SONY_IR_PROTOCOL_FRAME_MIN_COUNT                        3
#define SONY_IR_PROTOCOL_START_BIT_MASK_COMMAND                 0x80
#define SONY_IR_PROTOCOL_START_BIT_MASK_DEVICE                  0x10
#define SONY_IR_PROTOCOL_START_BIT_MASK_EXTENDED                0x80
#define SONY_IR_PROTOCOL_FRAME_SPACE_COUNTER_VALUE              75
#define SONY_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PULSE     (4)
#define SONY_IR_PROTOCOL_MOD_TIME_OFF                           0xFFFF

// --------------------------------------------------------------------------------

/**
 * @brief interface to the ir-signal carrier
 * Is configured before usage.
 * Will be started to transmit the signal.
 * And is stopped at the end of the transmission
 * 
 */
static TIMER_INTERFACE_TYPE* p_carrier;

/**
 * @brief interface to the ir-signal modaultor
 * Is configured before usage.
 * Will be started to transmit the signal.
 * And is stopped at the end of the transmission
 * 
 */
static TIMER_INTERFACE_TYPE* p_modulator;

/**
 * @brief Flag to tell the caller that
 * this ir-protocol is still running
 * 
 */
static u8 transmit_guard = 0;

/**
 * @brief Counts the number of frames that has already been transmitted
 * There must be at least three frames transmitted.
 * 
 */
static u8 frame_counter = 0;

/**
 * @brief Is called within the isr-callback.
 * Signal generation shall be turned off between to frames
 * 
 */
static u8 turn_off_signal = 0;

/**
 * @brief this counter is used to count the 600us timesteps between two following frames
 */
static u16 frame_space_counter = 0;

// --------------------------------------------------------------------------------

/**
 * @brief Callback of the modulator-irq
 * will set the IRmodulator-gpio
 * depending on the actual bit-value of the transmit-buffer
 * Will repeat the signal for three times, to meet the SONY ir-protocol
 * specification
 * 
 */
void ir_protocol_sony_irq_callback(void) {

    if (ir_protocol_interface_transmit_buffer_end() == 0) {

        if (ir_protocol_interface_transmit_buffer_get_next() == IR_PROTOCOL_INTERFACE_TRANSMIT_INTERVAL_PULSE) {

            IR_MOD_OUT_drive_high(); 

        } else {

            IR_MOD_OUT_drive_low(); 
        }

    } else if (frame_counter < SONY_IR_PROTOCOL_FRAME_MIN_COUNT - 1) {

        // we need to repeat the command for at least three times
        // SONY_IR_PROTOCOL_FRAME_MIN_COUNT - 1, because the first frame has already been transmitted

        if (turn_off_signal == 0) {

            // if not already happen
            // we turn off the ir-signal
            // by setting the modulator-gpio to low
            // and stop the carrier to safe power
            // the irq-callback from the modulator will remain active

            DEBUG_PASS("ir_protocol_sony_irq_callback() - TURNOFF");
            turn_off_signal = 1;

            IR_MOD_OUT_drive_low();
            p_carrier->stop();
        }

        frame_space_counter -= 1;

        if (frame_space_counter == 0) {

            p_carrier->start(TIME_CONFIGURATION_RUN_FOREVER);

            ir_protocol_interface_transmit_buffer_restart(0); // start the buffer from the beginning

            frame_space_counter = SONY_IR_PROTOCOL_FRAME_SPACE_COUNTER_VALUE;
            frame_counter += 1;
            turn_off_signal = 0;
            
            DEBUG_TRACE_byte(frame_counter, "ir_protocol_sony_irq_callback() - NEXT FRAME");
        }

    } else if (transmit_guard != 0) {

        IR_MOD_OUT_drive_low();

        p_carrier->stop();
        p_modulator->stop();

        transmit_guard = 0;
        ir_protocol_interface_transmit_buffer_release();

        DEBUG_PASS("ir_protocol_nec_irq_callback() - FINISHED");
    }
}

// --------------------------------------------------------------------------------

/**
 * @brief Appends pulse/pause to the transmit-buffer of the ir-interface depending on the bit-values of bit_vector.
 * For each bit is correct sequence of pulse/pause added to the transmit-buffer
 * sing pulse-wide modulation for coding of logical 1/0
 * 
 * @param length number of data-bits to append to the transmit-buffer
 * @param start_bit_mask start-bit mask to skip bits, will be shifted down (>> 1) to extracts the bits of bit_vector
 * @param bit_vector bits that will be added as pulse/pause to the transmit-buffer
 */
static void ir_protocol_sony_prepare_transmit_buffer_fill_buffer(u8 length, u8 start_bit_mask, u8 bit_vector) {

    u8 bit_mask = start_bit_mask;
    u8 i = 0;

    for ( ; i < length; i++) {

        if (bit_vector & bit_mask) {

            // Data-Bit 1:
            ir_protocol_interface_transmit_buffer_append_pause();
            ir_protocol_interface_transmit_buffer_append_pulse();
            ir_protocol_interface_transmit_buffer_append_pulse();

        } else {

            // Data-Bit : 0
            ir_protocol_interface_transmit_buffer_append_pause();
            ir_protocol_interface_transmit_buffer_append_pulse();

        }

        bit_mask = bit_mask >> 1;
    }
}

/**
 * @brief Prepares the ir-interface transmit-buffer with the given ir-command data.
 * 
 * @param p_command SONY ir-command generated by SONY IR-command module
 */
static void ir_protocol_sony_prepare_transmit_buffer(IR_COMMON_COMMAND_TYPE* p_command) {

    if (ir_protocol_interface_transmit_buffer_request() == 0) {
        DEBUG_PASS("ir_protocol_nec_prepare_transmit_buffer() - Transmit-Buffer is busy!");
        return;
    }

    ir_protocol_interface_transmit_buffer_reset();

    // preamble pusle
    u8 i = 0;
    for ( ; i < SONY_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PULSE; i++) {
        ir_protocol_interface_transmit_buffer_append_pulse();
    }

    // command bits
    DEBUG_PASS("ir_protocol_sony_prepare_transmit_buffer() - COMMAND");
    ir_protocol_sony_prepare_transmit_buffer_fill_buffer(
         SONY_IR_PROTOCOL_COMMAND_BIT_COUNT          // length
        ,SONY_IR_PROTOCOL_START_BIT_MASK_COMMAND     // start_bit_mask
        ,p_command->data_1                           // bit_vector
    );

    // device bits
    DEBUG_PASS("ir_protocol_sony_prepare_transmit_buffer() -  DEVICE");
    ir_protocol_sony_prepare_transmit_buffer_fill_buffer(
         SONY_IR_PROTOCOL_DEVICE_BIT_COUNT         // length
        ,SONY_IR_PROTOCOL_START_BIT_MASK_DEVICE    // start_bit_mask
        ,p_command->data_2                         // bit_vector
    );

    // extended bits
    DEBUG_PASS("ir_protocol_sony_prepare_transmit_buffer() - EXTENDED");
    ir_protocol_sony_prepare_transmit_buffer_fill_buffer(
         SONY_IR_PROTOCOL_EXTENDED_BIT_COUNT         // length
        ,SONY_IR_PROTOCOL_START_BIT_MASK_EXTENDED    // start_bit_mask
        ,p_command->data_3                           // bit_vector
    );
}

// --------------------------------------------------------------------------------

/**
 * @brief Cheks if the actual transmission is done, or not
 * 
 * @return 1 if transimission is still going on, otherwise 0.
 */
static u8 ir_protocol_sony_is_busy(void) {
    return transmit_guard != 0;
}

/**
 * @brief Sets the Access-Pointer to the timer control-units.
 * 
 * @param p_timer_carrier Pointer to the Carrier-Timer
 * @param p_timer_modulator Pointer to the Modulator-Timer
 */
static void ir_protocol_sony_set_timer(TIMER_INTERFACE_TYPE* p_timer_carrier, TIMER_INTERFACE_TYPE* p_timer_modulator) {
    p_carrier = p_timer_carrier;
    p_modulator = p_timer_modulator;
}

/**
 * @brief Transmits the actual content of command. This is a non-blocking function.
 * @param p_command valid command that is transmitted.
 * @see u8 ir_protocol_sony_is_busy(void) to check acutal transmit status
 */
static void ir_protocol_sony_transmit(IR_COMMON_COMMAND_TYPE* p_command) {

    if (transmit_guard != 0) {

        DEBUG_PASS("ir_protocol_sony_transmit() - Transmit guard is already set !!! ---");
        return;
    }

    transmit_guard = 1;

    p_carrier->stop();
    p_modulator->stop();

    DEBUG_TRACE_byte(p_command->data_1, "ir_protocol_sony_transmit() - Device Address:");
    DEBUG_TRACE_byte(p_command->data_2, "ir_protocol_sony_transmit() - Device Command:");
    DEBUG_TRACE_byte(p_command->data_3, "ir_protocol_sony_transmit() - Extended:");

    ir_protocol_sony_prepare_transmit_buffer(p_command);

    frame_counter = 0;
    turn_off_signal = 0;
    frame_space_counter = SONY_IR_PROTOCOL_FRAME_SPACE_COUNTER_VALUE;

    IR_MOD_OUT_drive_low();

    TIMER_CONFIGURATION_TYPE timer_config;
    
    timer_config.frequency = TIMER_FREQUENCY_NONE;
    timer_config.irq_callback = &ir_protocol_sony_irq_callback;
    timer_config.mode = TIMER_MODE_TIMER;
    timer_config.time_interval = TIMER_TIME_INTERVAL_600us;

    p_modulator->configure(&timer_config);
    
    timer_config.frequency = TIMER_FREQUENCY_40kHz;
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
        .uid = IR_PROTOCOL_TYPE_SONY,
        .set_timer = &ir_protocol_sony_set_timer,
        .transmit = &ir_protocol_sony_transmit,
        .is_busy = &ir_protocol_sony_is_busy,
        ._p_next = 0
};

// --------------------------------------------------------------------------------

/**
 * @see 3rdparty/ir_protocol/ir_protocol_sony.h#ir_protocol_sony_init
 * 
 */
void ir_protocol_sony_init(void) {
        DEBUG_PASS("ir_protocol_sony_init()");
        ir_protocol_interface_register_ir_protocol(&ir_protocol_sony);
}

// --------------------------------------------------------------------------------
