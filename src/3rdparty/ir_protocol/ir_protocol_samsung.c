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

// --------------------------------------------------------------------------------

#include "3rdparty/ir_protocol/ir_protocol_samsung.h"

// --------------------------------------------------------------------------------

#ifndef SAMSUNG_IR_PROTOCOL_TRANSMIT_BUFFER_SIZE
#define SAMSUNG_IR_PROTOCOL_TRANSMIT_BUFFER_SIZE				128
#endif

#ifndef SAMSUNG_IR_PROTOCOL_TRANSMIT_BUFFER_BIT_COUNT
#define SAMSUNG_IR_PROTOCOL_TRANSMIT_BUFFER_BIT_COUNT				32
#endif
										// empirical	// correct
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_START_PREAMBLE_US				4500		/* 4500 */
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_START_PAUSE_US				4500		/* 4500 */
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_START_US				560		/* 560 */
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_0_US					560		/* 560 */
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_1_US					1690		/* 1690 */
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_STOP_BIT_START_MS				560		/* 560 */
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_STOP_BIT_PAUSE_MS				560		/* 560 */

#define SAMSUNG_IR_PROTOCOL_MOD_TIME_OFF					0xFFFF

#define SASMUNG_IR_PROTOCOL_CMD_TO_BYTE_ARRAY(p_cmd)				{				\
											p_cmd->address,		\
											p_cmd->address,		\
											p_cmd->control,		\
											p_cmd->control ^ 0xFF	\
										}

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
static u8 data_bit_length;

/*
 *
 */
static u8 data_bit_counter;

/*
 * every byte represents a single bit
 * is used for a faster access
 */
static u8 transmit_buffer[SAMSUNG_IR_PROTOCOL_TRANSMIT_BUFFER_SIZE];

/*
 *
 */
static u8 transmit_guard = 0;

/*
 *
 */
static u8 irq_counter = 0;

// --------------------------------------------------------------------------------

void ir_protocol_samsung_irq_callback(void) {

	if (irq_counter < 8) {

		// Preamle Pulse
		irq_counter += 1;
		IR_MOD_OUT_drive_high(); 

	} else if (irq_counter < 16) {

		// Preamble Pause
		irq_counter += 1;
		IR_MOD_OUT_drive_low(); 

	} else if (data_bit_counter < data_bit_length) {

		// Data Bits

		if (transmit_buffer[data_bit_counter++]) {
			IR_MOD_OUT_drive_high(); 
		} else {
			IR_MOD_OUT_drive_low(); 
		}

	} else if (irq_counter < 17) {

		// Stop-Bit Pulse
		irq_counter += 1;
		IR_MOD_OUT_drive_high(); 

	} else {
			
		IR_MOD_OUT_drive_low();

		p_carrier->stop();
		p_modulator->stop();

		irq_counter = 0;
		transmit_guard = 0;
	}
}

// --------------------------------------------------------------------------------

static void ir_protocol_samsung_prepare_transmit_buffer(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {

	data_bit_length = 0;
	data_bit_counter = 0;

	u8 bit_mask = 0x80;
	u8 byte_index = 0;
	u8 data_buffer[SAMSUNG_IR_PROTOCOL_TRANSMIT_BUFFER_BIT_COUNT] = SASMUNG_IR_PROTOCOL_CMD_TO_BYTE_ARRAY(p_command);

	u8 i = 0;
	for ( ; i < SAMSUNG_IR_PROTOCOL_TRANSMIT_BUFFER_BIT_COUNT; i++ ) {

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

	//DEBUG_TRACE_N(data_bit_length, transmit_buffer, "ir_protocol_samsung_prepare_transmit_buffer() - Transmit-Buffer :");
}

// --------------------------------------------------------------------------------

u8 ir_protocol_samsung_is_busy(void) {
	return transmit_guard != 0;
}

void ir_protocol_samsung_set_timer(TIMER_INTERFACE_TYPE* p_timer_carrier, TIMER_INTERFACE_TYPE* p_timer_modulator) {
	p_carrier = p_timer_carrier;
	p_modulator = p_timer_modulator;
}

void ir_protocol_samsung_transmit(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {

	if (transmit_guard != 0) {

		DEBUG_PASS("ir_protocol_samsung_transmit() - Transmit guard is already set !!! ---");
		return;
	}

	transmit_guard = 1;

	p_carrier->stop();
	p_modulator->stop();

	DEBUG_TRACE_word(p_command->address, "ir_protocol_samsung_transmit() - Device Address:");
	DEBUG_TRACE_word(p_command->control, "ir_protocol_samsung_transmit() - Device Control:");

	ir_protocol_samsung_prepare_transmit_buffer(p_command);

	irq_counter = 0;

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
