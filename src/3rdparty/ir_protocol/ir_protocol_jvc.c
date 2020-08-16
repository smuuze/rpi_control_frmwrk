/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	ir_protocol_jvc.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "mcu_task_management/mcu_task_interface.h"
#include "time_management/time_management.h"

#include "driver/timer/timer_interface.h"

// --------------------------------------------------------------------------------

#include "3rdparty/ir_protocol/ir_protocol_jvc.h"

// --------------------------------------------------------------------------------

#ifndef JVC_IR_PROTOCOL_TRANSMIT_BUFFER_SIZE
#define JVC_IR_PROTOCOL_TRANSMIT_BUFFER_SIZE					128
#endif

#ifndef JVC_IR_PROTOCOL_TRANSMIT_BUFFER_BIT_COUNT
#define JVC_IR_PROTOCOL_TRANSMIT_BUFFER_BIT_COUNT				16
#endif

#ifndef JVC_IR_PROTOCOL_TRANSMIT_BUFFER_BYTE_COUNT
#define JVC_IR_PROTOCOL_TRANSMIT_BUFFER_BYTE_COUNT				2
#endif
										// empirical	// correct
#define JVC_IR_PROTOCOL_MOD_TIME_START_PREAMBLE_US				9000		/* 9000 */
#define JVC_IR_PROTOCOL_MOD_TIME_START_PAUSE_US					4500		/* 4500 */
#define JVC_IR_PROTOCOL_MOD_TIME_DATA_START_US					560		/* 560 */
#define JVC_IR_PROTOCOL_MOD_TIME_DATA_0_US					560		/* 560 */
#define JVC_IR_PROTOCOL_MOD_TIME_DATA_1_US					1690		/* 1690 */
#define JVC_IR_PROTOCOL_MOD_TIME_STOP_BIT_START_MS				560		/* 560 */
#define JVC_IR_PROTOCOL_MOD_TIME_STOP_BIT_PAUSE_MS				560		/* 560 */

#define JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PULSE			(16)
#define JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PAUSE			(16 + JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PULSE)

#define JVC_IR_PROTOCOL_MOD_TIME_OFF						0xFFFF

#define JVC_IR_PROTOCOL_CMD_TO_BYTE_ARRAY(p_cmd)				{								\
											( p_cmd->address << 4) | (u8)(p_cmd->control >> 8),	\
											(u8)(p_cmd->control),					\
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
static u8 transmit_buffer[JVC_IR_PROTOCOL_TRANSMIT_BUFFER_SIZE];

/*
 *
 */
static u8 transmit_guard = 0;

/*
 *
 */
static u8 irq_counter = 0;

// --------------------------------------------------------------------------------

void ir_protocol_jvc_irq_callback(void) {

	if (irq_counter < JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PULSE) {

		// Preamle Pulse
		irq_counter += 1;
		IR_MOD_OUT_drive_high(); 

	} else if (irq_counter < JVC_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PAUSE) {

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

static void ir_protocol_jvc_prepare_transmit_buffer(JVC_IR_PROTOCOL_COMMAND_TYPE* p_command) {

	data_bit_length = 0;
	data_bit_counter = 0;

	u8 bit_mask = 0x80;
	u8 byte_index = 0;
	u8 data_buffer[JVC_IR_PROTOCOL_TRANSMIT_BUFFER_BYTE_COUNT] = JVC_IR_PROTOCOL_CMD_TO_BYTE_ARRAY(p_command);

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

	//DEBUG_TRACE_N(data_bit_length, transmit_buffer, "ir_protocol_jvc_prepare_transmit_buffer() - Transmit-Buffer :");
}

// --------------------------------------------------------------------------------

u8 ir_protocol_jvc_is_busy(void) {
	return transmit_guard != 0;
}

void ir_protocol_jvc_set_timer(TIMER_INTERFACE_TYPE* p_timer_carrier, TIMER_INTERFACE_TYPE* p_timer_modulator) {
	p_carrier = p_timer_carrier;
	p_modulator = p_timer_modulator;
}

void ir_protocol_jvc_transmit(JVC_IR_PROTOCOL_COMMAND_TYPE* p_command) {

	if (transmit_guard != 0) {

		DEBUG_PASS("ir_protocol_jvc_transmit() - Transmit guard is already set !!! ---");
		return;
	}

	transmit_guard = 1;

	p_carrier->stop();
	p_modulator->stop();

	DEBUG_TRACE_word(p_command->address, "ir_protocol_jvc_transmit() - Device Address:");
	DEBUG_TRACE_word(p_command->control, "ir_protocol_jvc_transmit() - Device Control:");

	ir_protocol_jvc_prepare_transmit_buffer(p_command);

	irq_counter = 0;

	IR_MOD_OUT_drive_low();

	TIMER_CONFIGURATION_TYPE timer_config;
	
	timer_config.frequency = TIMER_FREQUENCY_NONE;
	timer_config.irq_callback = &ir_protocol_jvc_irq_callback;
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