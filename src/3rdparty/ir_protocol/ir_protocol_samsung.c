/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	ir_protocol_samusung.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_ON

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "mcu_task_management/mcu_task_interface.h"
#include "time_management/time_management.h"

#include "driver/timer/timer_interface.h"

// --------------------------------------------------------------------------------

#ifndef SAMSUNG_IR_PROTOCOL_TRANSMIT_BUFFER_SIZE
#define SAMSUNG_IR_PROTOCOL_TRANSMIT_BUFFER_SIZE				4
#endif

#ifndef SAMSUNG_IR_PROTOCOL_TRANSMIT_BUFFER_BIT_COUNT
#define SAMSUNG_IR_PROTOCOL_TRANSMIT_BUFFER_BIT_COUNT				32
#endif

#define SAMSUNG_IR_PROTOCOL_MOD_TIME_START_PREAMBLE_US				4500
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_START_PAUSE_US				4500
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_START_US				560
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_0_US					560
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_1_US					1690

#define SAMSUNG_IR_PROTOCOL_MOD_TIME_OFF					0xFFFF

// --------------------------------------------------------------------------------

/*
 *
 */
typedef enum {
	SAMSUNG_IR_PROTOCOL_IDLE,
	SAMSUNG_IR_PROTOCOL_START_PREAMBLE,
	SAMSUNG_IR_PROTOCOL_START_PAUSE,
	SAMSUNG_IR_PROTOCOL_DATA_START,
	SAMSUNG_IR_PROTOCOL_DATA_0,
	SAMSUNG_IR_PROTOCOL_DATA_1,
	SAMSUNG_IR_PROTOCOL_END
} SASMUNG_IR_PROTOCOL_STATE;

// --------------------------------------------------------------------------------

/*
 *
 */
static void ir_protocol_samsung_calculate_modulation_time(void);

/*
 *
 */
static u16 ir_protocol_samsung_get_modulation_time(void);

/*
 *
 */
static void ir_protocol_samsung_control_modulation(void);

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
static u8 data_bit_counter;

/*
 * every byte represents a single bit
 * is used for a faster access
 */
static u8 transmit_bit_buffer[SAMSUNG_IR_PROTOCOL_TRANSMIT_BUFFER_BIT_COUNT];

/*
 *
 */
static SASMUNG_IR_PROTOCOL_STATE transmit_state;

// --------------------------------------------------------------------------------

void ir_protocol_samsung_irq_callback(void) {

	u16 mod_time = ir_protocol_samsung_get_modulation_time();
	
	//DEBUG_TRACE_word(mod_time, "ir_protocol_samsung_irq_callback() - Modulation time");
	//DEBUG_TRACE_byte(IR_MOD_OUT_is_high_level(), "ir_protocol_samsung_irq_callback() - Pin-Level");

	if (mod_time == SAMSUNG_IR_PROTOCOL_MOD_TIME_OFF) {

		p_carrier->stop();
		p_modulator->stop();

		return;
	}

	p_modulator->start(mod_time);

	ir_protocol_samsung_control_modulation();
	ir_protocol_samsung_calculate_modulation_time();
}

// --------------------------------------------------------------------------------

static u16 ir_protocol_samsung_get_modulation_time(void) {
	
	switch (transmit_state) {

		default: 					return SAMSUNG_IR_PROTOCOL_MOD_TIME_OFF;
		case SAMSUNG_IR_PROTOCOL_START_PREAMBLE : 	return SAMSUNG_IR_PROTOCOL_MOD_TIME_START_PREAMBLE_US;
		case SAMSUNG_IR_PROTOCOL_START_PAUSE : 		return SAMSUNG_IR_PROTOCOL_MOD_TIME_START_PAUSE_US;
		case SAMSUNG_IR_PROTOCOL_DATA_START : 		return SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_START_US;
		case SAMSUNG_IR_PROTOCOL_DATA_0 : 		return SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_0_US;
		case SAMSUNG_IR_PROTOCOL_DATA_1 : 		return SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_1_US;
	}
}

static inline void ir_protocol_samsung_control_modulation(void) {
	
	switch (transmit_state) {

		default: 					IR_MOD_OUT_drive_low(); break;
		case SAMSUNG_IR_PROTOCOL_START_PREAMBLE : 	IR_MOD_OUT_drive_high(); break;
		case SAMSUNG_IR_PROTOCOL_START_PAUSE : 		IR_MOD_OUT_drive_low(); break;
		case SAMSUNG_IR_PROTOCOL_DATA_START : 		IR_MOD_OUT_drive_high(); break;
		case SAMSUNG_IR_PROTOCOL_DATA_0 : 		IR_MOD_OUT_drive_low(); break;
		case SAMSUNG_IR_PROTOCOL_DATA_1 : 		IR_MOD_OUT_drive_low(); break;
	}
}

static void ir_protocol_samsung_calculate_modulation_time(void) {

	if (transmit_state == SAMSUNG_IR_PROTOCOL_START_PREAMBLE) {
		transmit_state = SAMSUNG_IR_PROTOCOL_START_PAUSE;
		return;
	}

	if (transmit_state == SAMSUNG_IR_PROTOCOL_START_PAUSE) {
		transmit_state = SAMSUNG_IR_PROTOCOL_DATA_START;
		return;
	}

	if (data_bit_counter == SAMSUNG_IR_PROTOCOL_TRANSMIT_BUFFER_BIT_COUNT) {
		transmit_state = SAMSUNG_IR_PROTOCOL_END;
		return;
	}

	if (transmit_state == SAMSUNG_IR_PROTOCOL_END) {
		transmit_state = SAMSUNG_IR_PROTOCOL_IDLE;
		return;
	}

	if (transmit_state == SAMSUNG_IR_PROTOCOL_DATA_0 || transmit_state == SAMSUNG_IR_PROTOCOL_DATA_1) {
		transmit_state = SAMSUNG_IR_PROTOCOL_DATA_START;
		return;
	}

	transmit_state = transmit_bit_buffer[data_bit_counter] != 0 ? SAMSUNG_IR_PROTOCOL_DATA_1 : SAMSUNG_IR_PROTOCOL_DATA_0;
	data_bit_counter += 1;
}

// --------------------------------------------------------------------------------

void ir_protocol_samsung_set_timer(TIMER_INTERFACE_TYPE* p_timer_carrier, TIMER_INTERFACE_TYPE* p_timer_modulator) {
	p_carrier = p_timer_carrier;
	p_modulator = p_timer_modulator;
}

void ir_protocol_samsung_transmit(u8* p_data) {

	DEBUG_PASS("ir_protocol_samsung_start()");

	IR_MOD_OUT_drive_low();

	// pre calculate bit-buffer

	u8 i = 0;
	for ( ; i < SAMSUNG_IR_PROTOCOL_TRANSMIT_BUFFER_BIT_COUNT; i++ ) {

		u8 byte_index = (i) / 8;
		u8 bit_index = i - (byte_index * 8);
		u8 bit_mask = (0x80 >> bit_index); // start transmit from the highest bit

		transmit_bit_buffer[i] = p_data[byte_index] & bit_mask;		
	}

	transmit_state = SAMSUNG_IR_PROTOCOL_START_PREAMBLE;
	data_bit_counter = 0;

	TIMER_CONFIGURATION_TYPE timer_config;
	
	timer_config.frequency = TIMER_FREQUENCY_NONE;
	timer_config.irq_callback = &ir_protocol_samsung_irq_callback;
	timer_config.mode = TIMER_MODE_TIMER;
	timer_config.time_interval = TIMER_TIME_INTERVAL_80us;

	p_modulator->configure(&timer_config);
	
	timer_config.frequency = TIMER_FREQUENCY_37_9kHz;
	timer_config.irq_callback = 0;
	timer_config.mode = TIMER_MODE_FREQUENCY;

	p_carrier->configure(&timer_config);

	p_carrier->start(TIME_CONFIGURATION_RUN_FOREVER);
	p_modulator->start(ir_protocol_samsung_get_modulation_time());

	ir_protocol_samsung_control_modulation();
	ir_protocol_samsung_calculate_modulation_time();
}