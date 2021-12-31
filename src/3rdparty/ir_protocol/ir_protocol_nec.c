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

#include "mcu_task_management/mcu_task_interface.h"
#include "time_management/time_management.h"

#include "driver/timer/timer_interface.h"

// --------------------------------------------------------------------------------

#include "3rdparty/ir_protocol/ir_protocol_interface.h"

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
#define NEC_IR_PROTOCOL_INTERVAL_PREAMBLE_PAUSE         (NEC_IR_PROTOCOL_INTERVAL_PREAMBLE_PULSE + 8)
#define NEC_IR_PROTOCOL_INTERVAL_STOP_BIT               17

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
 * @brief Pointer to the carrier frequency-generator
 * that is given by ir_protocol_nec_set_timer()
 * 
 */
static TIMER_INTERFACE_TYPE* p_carrier;

/**
 * @brief Pointer to the modulation frequency-generator
 * that is given by ir_protocol_nec_set_timer()
 * 
 */
static TIMER_INTERFACE_TYPE* p_modulator;

/**
 * @brief Number of 560us intervals that are
 * needed to transmit the data of the actual command.
 * Depends on the number of logical ones and zeros
 * 
 */
static u8 data_bit_length;

/**
 * @brief Number of 560us intervals that have
 * already been transmitted
 * 
 */
static u8 data_bit_counter;

/**
 * @brief Every byte represents a single bit.
 * 
 */
static u8 transmit_buffer[NEC_IR_PROTOCOL_TRANSMIT_BUFFER_SIZE];

/**
 * @brief If a transmission is ongoing this variable
 * will be set to 1 to prevent a caller starting a new transmission
 * 
 */
static u8 transmit_guard = 0;

/**
 * @brief Is used by ir_protocol_nec_irq_callback()
 * to generate the single phases of the ir-signal (preamble - pause - data - stop-bit)
 * 
 */
static u8 interval_counter = 0;

// --------------------------------------------------------------------------------

/**
 * @brief Is executed everytime the irq of the modulation-timer rises.
 * This will generate the ir-signal by controlling the modulation gpio-pin
 * depending on the actual ir-command inside of the transmit_buffer.
 * 
 */
static void ir_protocol_nec_irq_callback(void) {

	if (interval_counter < NEC_IR_PROTOCOL_INTERVAL_PREAMBLE_PULSE) {

		// Preamle Pulse
		interval_counter += 1;
		IR_MOD_OUT_drive_high(); 

	} else if (interval_counter < NEC_IR_PROTOCOL_INTERVAL_PREAMBLE_PAUSE) {

		// Preamble Pause
		interval_counter += 1;
		IR_MOD_OUT_drive_low(); 

	} else if (data_bit_counter < data_bit_length) {

		// Data Bits

		if (transmit_buffer[data_bit_counter++]) {
			IR_MOD_OUT_drive_high(); 
		} else {
			IR_MOD_OUT_drive_low(); 
		}

	} else if (interval_counter < NEC_IR_PROTOCOL_INTERVAL_STOP_BIT) {

		// Stop-Bit Pulse
		interval_counter += 1;
		IR_MOD_OUT_drive_high(); 

	} else {
			
		IR_MOD_OUT_drive_low();

		p_carrier->stop();
		p_modulator->stop();

		interval_counter = 0;
		transmit_guard = 0;
	}
}

// --------------------------------------------------------------------------------

/**
 * @brief Prepares the transmit-buffer.
 * The bits from the given command are copied into the transmit_buffer.
 * Every byte of the transmit_buffer represents one bit of the given command.
 * 
 * @param p_command valid ir command that will be transmitted
 */
static void ir_protocol_nec_prepare_transmit_buffer(IR_COMMON_COMMAND_TYPE* p_command) {

	data_bit_length = 0;
	data_bit_counter = 0;

        u8 bit_mask = 0x80;
        u8 byte_index = 0;

        NEC_IR_PROTOCOL_NEW_DATA_BUFFER(data_buffer, p_command);

	u8 i = 0;
	for ( ; i < NEC_IR_PROTOCOL_TRANSMIT_BUFFER_BIT_COUNT; i++ ) {

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

	DEBUG_TRACE_N(data_bit_length, transmit_buffer, "ir_protocol_nec_prepare_transmit_buffer() - Transmit-Buffer :");
}

// --------------------------------------------------------------------------------

/**
 * @brief Sets the frequency and modulation timer that will be used
 * to transmitt commands. Both timers will be automatically configured
 * by this function to the correct values that are needed for the NEC IR-protocol.
 * 
 * @param p_timer_carrier interface to the frequency geneartor
 * @param p_timer_modulator interface to the modulation generator
 */
static void ir_protocol_nec_set_timer(TIMER_INTERFACE_TYPE* p_timer_carrier, TIMER_INTERFACE_TYPE* p_timer_modulator) {
	p_carrier = p_timer_carrier;
	p_modulator = p_timer_modulator;
}

/**
 * @brief Starts transmitting the given command.
 * This function prepares the data-bits and starts the transmitting routine.
 * It will not block until the command has been transmitted.
 * 
 * @param p_command valid IR command to transmit
 */
static void ir_protocol_nec_transmit(IR_COMMON_COMMAND_TYPE* p_command) {

	if (transmit_guard != 0) {

		DEBUG_PASS("ir_protocol_nec_transmit() - Transmit guard is already set !!! ---");
		return;
	}

	transmit_guard = 1;

	p_carrier->stop();
	p_modulator->stop();

	DEBUG_TRACE_word(p_command->data_1, "ir_protocol_nec_transmit() - Device Address:");
	DEBUG_TRACE_word(p_command->data_2, "ir_protocol_nec_transmit() - Device Control:");

	ir_protocol_nec_prepare_transmit_buffer(p_command);

	interval_counter = 0;

	IR_MOD_OUT_drive_low();

	TIMER_CONFIGURATION_TYPE timer_config;
	
	timer_config.frequency = TIMER_FREQUENCY_NONE;
	timer_config.irq_callback = &ir_protocol_nec_irq_callback;
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

/**
 * @brief Check if the module is still transmitting the ircommand.
 * 
 * @return 1 if transmissin is not completed yet, otherwise 0
 */
static u8 ir_protocol_nec_is_busy(void) {
	return transmit_guard != 0;
}

// --------------------------------------------------------------------------------

/**
 * @brief Interface to this ir-protocol implementation.
 * Is used for register this implementation to the ir-handler module.
 * 
 */
static IR_PROTOCOL_GENERATOR_TYPE ir_protocol_nec = {

        .uid = IR_PROTOCOL_TYPE_NEC,
        .set_timer = &ir_protocol_nec_set_timer,
        .transmit = &ir_protocol_nec_transmit,
        .is_busy = &ir_protocol_nec_is_busy,
        ._p_next = 0

};

// --------------------------------------------------------------------------------

void ir_protocol_nec_init(void) {
        DEBUG_PASS("ir_protocol_nec_init()");
        ir_protocol_interface_register_ir_protocol(&ir_protocol_nec);
}
