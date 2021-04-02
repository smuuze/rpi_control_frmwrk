/**
 * --------------------------------------------------------------------------------
 *
 * @file 	ir_protocol_sony.c
 * @author 	sebastian lesse (sebastian lesse)
 * @brief 
 * @version 	1.0
 * @date 	2021-03-21
 * 
 * @copyright 	Copyright (c) 2021
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

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

#include "3rdparty/ir_protocol/ir_protocol_sony.h"

// --------------------------------------------------------------------------------

#ifndef SONY_IR_PROTOCOL_TRANSMIT_BUFFER_SIZE
#define SONY_IR_PROTOCOL_TRANSMIT_BUFFER_SIZE					128
#endif

#define SONY_IR_PROTOCOL_COMMAND_BIT_COUNT					7
#define SONY_IR_PROTOCOL_DEVICE_BIT_COUNT					5
#define SONY_IR_PROTOCOL_EXTENDED_BIT_COUNT					8

#define SONY_IR_PROTOCOL_FRAME_MIN_COUNT					3
#define SON_IR_PROTOCOL_FRAME_INTERVAL_MS					46

#define SONY_IR_PROTOCOL_START_BIT_MASK_COMMAND					0x80
#define SONY_IR_PROTOCOL_START_BIT_MASK_DEVICE					0x10
#define SONY_IR_PROTOCOL_START_BIT_MASK_EXTENDED				0x80

#define SONY_IR_PROTOCOL_TRANSMIT_BUFFER_BIT_COUNT				20

#ifndef SONY_IR_PROTOCOL_TRANSMIT_BUFFER_BYTE_COUNT
#define SONY_IR_PROTOCOL_TRANSMIT_BUFFER_BYTE_COUNT				3
#endif

#define SONY_IR_PROTOCOL_MOD_TIME_START_PREAMBLE_US				8440
#define SONY_IR_PROTOCOL_MOD_TIME_START_PAUSE_US				4220
#define SONY_IR_PROTOCOL_MOD_TIME_DATA_START_US					527
#define SONY_IR_PROTOCOL_MOD_TIME_DATA_0_US					527
#define SONY_IR_PROTOCOL_MOD_TIME_DATA_1_US					1583
#define SONY_IR_PROTOCOL_MOD_TIME_STOP_BIT_START_MS				527
#define SONY_IR_PROTOCOL_MOD_TIME_STOP_BIT_PAUSE_MS				527

#define SONY_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PULSE			(4)
#define SONY_IR_PROTOCOL_MOD_TIME_STEP_COUNT_STOP_BIT				(1 + SONY_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PAUSE)
#define SONY_IR_PROTOCOLL_MODE_TIME_STEP_WORD_CYCLE				(88)

#define SONY_IR_PROTOCOL_MOD_TIME_OFF						0xFFFF

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U8(IR_SONY_FRAME_SPACE_TIMER)

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
static u8 transmit_buffer[SONY_IR_PROTOCOL_TRANSMIT_BUFFER_SIZE];

/*
 *
 */
static u8 transmit_guard = 0;

/*
 *
 */
static u8 irq_counter = 0;

/*
 *
 */
static u8 frame_counter = 0;

/**
 * @brief 	Is called within the isr-callback.
 * 		Signal generation shall be turned off between to frames
 * 
 */
static u8 turn_off_signal = 0;

// --------------------------------------------------------------------------------

void ir_protocol_sony_irq_callback(void) {

	if (irq_counter < SONY_IR_PROTOCOL_MOD_TIME_STEP_COUNT_PREAMBLE_PULSE) {

		// Preamle Pulse

		if (irq_counter == 0) {
			DEBUG_PASS("ir_protocol_sony_irq_callback() - PREAMBLE");
		}

		irq_counter += 1;
		IR_MOD_OUT_drive_high(); 

	} else if (data_bit_counter < data_bit_length) {

		// Data Bits

		if (data_bit_counter == 0) {
			DEBUG_PASS("ir_protocol_sony_irq_callback() - DATA");
		}

		if (transmit_buffer[data_bit_counter]) {
			IR_MOD_OUT_drive_high(); 
		} else {
			IR_MOD_OUT_drive_low();
		}

		data_bit_counter += 1;

	} else if (frame_counter < SONY_IR_PROTOCOL_FRAME_MIN_COUNT - 1) {
			
		// SONY_IR_PROTOCOL_FRAME_MIN_COUNT - 1, because the first frame has already been transmitted

		if (turn_off_signal == 0) {

			DEBUG_PASS("ir_protocol_sony_irq_callback() - TURNOFF");
			turn_off_signal = 1;

			IR_MOD_OUT_drive_low();
			p_carrier->stop();
		}

		if (IR_SONY_FRAME_SPACE_TIMER_is_up(SON_IR_PROTOCOL_FRAME_INTERVAL_MS)) {

			IR_SONY_FRAME_SPACE_TIMER_start();

			p_carrier->start(TIME_CONFIGURATION_RUN_FOREVER);
			IR_MOD_OUT_drive_high(); 

			frame_counter += 1;
			irq_counter = 1;
			data_bit_counter = 0;
			turn_off_signal = 0;

			if (data_bit_counter == 0) {
				DEBUG_TRACE_byte(frame_counter, "ir_protocol_sony_irq_callback() - NEXT FRAME");
			}
		}

	} else {
			
		IR_MOD_OUT_drive_low();

		p_carrier->stop();
		p_modulator->stop();

		transmit_guard = 0;

		DEBUG_PASS("ir_protocol_sony_irq_callback() - FINISHED");
	}
}

// --------------------------------------------------------------------------------

static u8 ir_protocol_sony_prepare_transmit_buffer_fill_buffer(u8 length, u8 start_bit_mask, u8 bit_vector, u8 bit_counter) {

	if (bit_counter >= SONY_IR_PROTOCOL_TRANSMIT_BUFFER_SIZE) {
		DEBUG_PASS("ir_protocol_sony_prepare_transmit_buffer_fill_buffer() - BUFFER OVERFLOW !!! ---");
		return bit_counter;
	}

	u8 bit_mask = start_bit_mask;
	u8 i = 0;

	for ( ; i < length; i++) {

		if (bit_vector & bit_mask) {

			// Data-Bit 1:
			transmit_buffer[bit_counter++] = 0;
			transmit_buffer[bit_counter++] = 1;
			transmit_buffer[bit_counter++] = 1;

		} else {

			// Data-Bit : 0
			transmit_buffer[bit_counter++] = 0;
			transmit_buffer[bit_counter++] = 1;

		}

		bit_mask = bit_mask >> 1;
	}

	return bit_counter;
}

static void ir_protocol_sony_prepare_transmit_buffer(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command) {

	data_bit_length = 0;
	data_bit_counter = 0;

	// command bits
	data_bit_length = ir_protocol_sony_prepare_transmit_buffer_fill_buffer(SONY_IR_PROTOCOL_COMMAND_BIT_COUNT, SONY_IR_PROTOCOL_START_BIT_MASK_COMMAND, p_command->command, data_bit_length);
	DEBUG_TRACE_N(data_bit_length, transmit_buffer, "ir_protocol_sony_prepare_transmit_buffer() - Transmit-Buffer (COMMAND)");

	// device bits
	data_bit_length = ir_protocol_sony_prepare_transmit_buffer_fill_buffer(SONY_IR_PROTOCOL_DEVICE_BIT_COUNT, SONY_IR_PROTOCOL_START_BIT_MASK_DEVICE, p_command->device, data_bit_length);
	DEBUG_TRACE_N(data_bit_length, transmit_buffer, "ir_protocol_sony_prepare_transmit_buffer() - Transmit-Buffer (COMMMAND + DEVICE)");

	// extended bits
	data_bit_length = ir_protocol_sony_prepare_transmit_buffer_fill_buffer(SONY_IR_PROTOCOL_EXTENDED_BIT_COUNT, SONY_IR_PROTOCOL_START_BIT_MASK_EXTENDED, p_command->extended, data_bit_length);
	DEBUG_TRACE_N(data_bit_length, transmit_buffer, "ir_protocol_sony_prepare_transmit_buffer() - Transmit-Buffer (COMMAND + DEVICE + EXTENDED)");
}

// --------------------------------------------------------------------------------

u8 ir_protocol_sony_is_busy(void) {
	return transmit_guard != 0;
}

void ir_protocol_sony_set_timer(TIMER_INTERFACE_TYPE* p_timer_carrier, TIMER_INTERFACE_TYPE* p_timer_modulator) {
	p_carrier = p_timer_carrier;
	p_modulator = p_timer_modulator;
}

void ir_protocol_sony_transmit(SONY_IR_PROTOCOL_COMMAND_TYPE* p_command) {

	if (transmit_guard != 0) {

		DEBUG_PASS("ir_protocol_sony_transmit() - Transmit guard is already set !!! ---");
		return;
	}

	transmit_guard = 1;

	p_carrier->stop();
	p_modulator->stop();

	DEBUG_TRACE_word(p_command->device, "ir_protocol_sony_transmit() - Device Address:");
	DEBUG_TRACE_word(p_command->command, "ir_protocol_sony_transmit() - Device Command:");
	DEBUG_TRACE_word(p_command->extended, "ir_protocol_sony_transmit() - Extended:");

	ir_protocol_sony_prepare_transmit_buffer(p_command);

	irq_counter = 0;
	frame_counter = 0;
	turn_off_signal = 0;

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

	IR_SONY_FRAME_SPACE_TIMER_start();
}
