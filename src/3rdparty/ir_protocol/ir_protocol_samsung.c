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

#include "3rdparty/ir_protocol/ir_protocol_samsung.h"

// --------------------------------------------------------------------------------

#ifndef SAMSUNG_IR_PROTOCOL_TRANSMIT_BUFFER_SIZE
#define SAMSUNG_IR_PROTOCOL_TRANSMIT_BUFFER_SIZE				4
#endif

#ifndef SAMSUNG_IR_PROTOCOL_TRANSMIT_BUFFER_BIT_COUNT
#define SAMSUNG_IR_PROTOCOL_TRANSMIT_BUFFER_BIT_COUNT				32
#endif
										// empirical	// correct
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_START_PREAMBLE_US				4560		/* 4500 */
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_START_PAUSE_US				4560		/* 4500 */
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_START_US				525		/* 560 */
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_0_US					530		/* 560 */
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_1_US					1690		/* 1690 */
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_STOP_BIT_START_MS				540		/* 560 */
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_STOP_BIT_PAUSE_MS				540		/* 560 */

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
typedef enum {
	SAMSUNG_IR_PROTOCOL_IDLE,
	SAMSUNG_IR_PROTOCOL_START_PREAMBLE,
	SAMSUNG_IR_PROTOCOL_START_PAUSE,
	SAMSUNG_IR_PROTOCOL_DATA_START,
	SAMSUNG_IR_PROTOCOL_DATA_0,
	SAMSUNG_IR_PROTOCOL_DATA_1,
	SASMUNG_IR_PROTOCOL_STOP_BIT_START,
	SASMUNG_IR_PROTOCOL_STOP_BIT_PAUSE,
	SAMSUNG_IR_PROTOCOL_END
} SASMUNG_IR_PROTOCOL_STATE;

// --------------------------------------------------------------------------------

/*
 *
 */
static void ir_protocol_samsung_control_modulation(void);

/*
 *
 */
static u16 ir_protocol_samsung_calculate_modulation_time(void);

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

/*
 *
 */
static u8 transmit_guard = 0;

// --------------------------------------------------------------------------------

void ir_protocol_samsung_irq_callback(void) {

	u16 mod_time = ir_protocol_samsung_calculate_modulation_time();

	if (mod_time == SAMSUNG_IR_PROTOCOL_MOD_TIME_OFF) {

		p_carrier->stop();
		p_modulator->stop();
	
		ir_protocol_samsung_control_modulation();

		transmit_guard = 0;

		DEBUG_PASS("ir_protocol_samsung_irq_callback() - FINISHED");

		return;
	}

	p_modulator->start(mod_time);

	ir_protocol_samsung_control_modulation();
}

// --------------------------------------------------------------------------------

static u16 ir_protocol_samsung_calculate_modulation_time(void) {
	
	switch (transmit_state) {

		default: 					return SAMSUNG_IR_PROTOCOL_MOD_TIME_OFF;
		case SAMSUNG_IR_PROTOCOL_START_PREAMBLE : 	return SAMSUNG_IR_PROTOCOL_MOD_TIME_START_PREAMBLE_US;
		case SAMSUNG_IR_PROTOCOL_START_PAUSE : 		return SAMSUNG_IR_PROTOCOL_MOD_TIME_START_PAUSE_US;
		case SAMSUNG_IR_PROTOCOL_DATA_START : 		return SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_START_US;
		case SAMSUNG_IR_PROTOCOL_DATA_0 : 		return SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_0_US;
		case SAMSUNG_IR_PROTOCOL_DATA_1 : 		return SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_1_US;
		case SASMUNG_IR_PROTOCOL_STOP_BIT_START : 	return SAMSUNG_IR_PROTOCOL_MOD_TIME_STOP_BIT_START_MS;
		case SASMUNG_IR_PROTOCOL_STOP_BIT_PAUSE : 	return SAMSUNG_IR_PROTOCOL_MOD_TIME_STOP_BIT_PAUSE_MS;
	}
}

static void ir_protocol_samsung_control_modulation(void) {
	
	switch (transmit_state) {					

		default :
		case SAMSUNG_IR_PROTOCOL_START_PREAMBLE : 	
			
			IR_MOD_OUT_drive_high(); 
			transmit_state = SAMSUNG_IR_PROTOCOL_START_PAUSE; 
			return;

		case SAMSUNG_IR_PROTOCOL_START_PAUSE : 		
			
			IR_MOD_OUT_drive_low(); 
			transmit_state = SAMSUNG_IR_PROTOCOL_DATA_START; 
			return;

		case SASMUNG_IR_PROTOCOL_STOP_BIT_START :	
		
			IR_MOD_OUT_drive_high(); 
			transmit_state = SASMUNG_IR_PROTOCOL_STOP_BIT_PAUSE; 
			return;

		case SASMUNG_IR_PROTOCOL_STOP_BIT_PAUSE :	
			
			IR_MOD_OUT_drive_low(); 
			transmit_state = SAMSUNG_IR_PROTOCOL_END;
			return;

		case SAMSUNG_IR_PROTOCOL_END :
		
			IR_MOD_OUT_drive_low();
			transmit_state = SAMSUNG_IR_PROTOCOL_IDLE;
			return;
				
		case SAMSUNG_IR_PROTOCOL_DATA_START : 		
			
			IR_MOD_OUT_drive_high();
			transmit_state = transmit_bit_buffer[data_bit_counter] != 0 ? SAMSUNG_IR_PROTOCOL_DATA_1 : SAMSUNG_IR_PROTOCOL_DATA_0;
			data_bit_counter += 1;

			break;

		case SAMSUNG_IR_PROTOCOL_DATA_0 :		// no break;
		case SAMSUNG_IR_PROTOCOL_DATA_1 :

			IR_MOD_OUT_drive_low();	

			if (data_bit_counter == SAMSUNG_IR_PROTOCOL_TRANSMIT_BUFFER_BIT_COUNT) {
				transmit_state = SASMUNG_IR_PROTOCOL_STOP_BIT_START;
			} else {
				transmit_state = SAMSUNG_IR_PROTOCOL_DATA_START;
			}

			break;
	}
}

static void ir_protocol_samsung_prepare_transmit_buffer(SAMSUNG_IR_PROTOCOL_COMMAND_TYPE* p_command) {

	u8 bit_mask = 0x80;
	u8 byte_index = 0;
	u8 data_buffer[SAMSUNG_IR_PROTOCOL_TRANSMIT_BUFFER_BIT_COUNT] = SASMUNG_IR_PROTOCOL_CMD_TO_BYTE_ARRAY(p_command);

	u8 i = 0;
	for ( ; i < SAMSUNG_IR_PROTOCOL_TRANSMIT_BUFFER_BIT_COUNT; i++ ) {

		if (bit_mask == 0) {
			bit_mask = 0x80;
			byte_index += 1;
		}

		transmit_bit_buffer[i] = data_buffer[byte_index] & bit_mask;
		bit_mask = bit_mask >> 1;
	}
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

	DEBUG_TRACE_word(p_command->address, "ir_protocol_samsung_transmit() - Device Address:");
	DEBUG_TRACE_word(p_command->control, "ir_protocol_samsung_transmit() - Device Control:");

	transmit_guard = 1;

	IR_MOD_OUT_drive_low();

	TIMER_CONFIGURATION_TYPE timer_config;
	
	timer_config.frequency = TIMER_FREQUENCY_NONE;
	timer_config.irq_callback = &ir_protocol_samsung_irq_callback;
	timer_config.mode = TIMER_MODE_TIMER;
	timer_config.time_interval = TIMER_TIME_INTERVAL_40us;

	p_modulator->configure(&timer_config);
	
	timer_config.frequency = TIMER_FREQUENCY_37_9kHz;
	timer_config.irq_callback = 0;
	timer_config.mode = TIMER_MODE_FREQUENCY;

	p_carrier->configure(&timer_config);

	ir_protocol_samsung_prepare_transmit_buffer(p_command);

	transmit_state = SAMSUNG_IR_PROTOCOL_START_PREAMBLE;
	data_bit_counter = 0;
	
	p_carrier->start(TIME_CONFIGURATION_RUN_FOREVER);
	p_modulator->start(ir_protocol_samsung_calculate_modulation_time());

	ir_protocol_samsung_control_modulation();
}