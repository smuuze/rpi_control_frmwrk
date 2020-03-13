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

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

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

#define SAMSUNG_IR_PROTOCOL_MOD_TIME_START_PREAMBLE_US				4500
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_START_PAUSE_US				4500
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_START_US				560
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_0_US					560
#define SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_1_US					1690

// --------------------------------------------------------------------------------

/*
 *
 */
typedef enum {
	SAMSUNG_IR_PROTOCOL_START_PREAMBLE,
	SAMSUNG_IR_PROTOCOL_START_PAUSE,
	SAMSUNG_IR_PROTOCOL_DATA_START,
	SAMSUNG_IR_PROTOCOL_DATA_0,
	SAMSUNG_IR_PROTOCOL_DATA_1
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
static u8 transmit_buffer[SAMSUNG_IR_PROTOCOL_TRANSMIT_BUFFER_SIZE];

/*
 *
 */
static SASMUNG_IR_PROTOCOL_STATE transmit_state;

/*
 *
 */
static u8 data_bit_counter;

// --------------------------------------------------------------------------------

void ir_protocol_samsung_irq_callback(void) {

	u16 mod_time = ir_protocol_samsung_get_modulation_time();

	if (mod_time != 0xFFFF) {
		
		timer1_driver_start(mod_time); // 560 us
		ir_protocol_samsung_calculate_modulation_time();
	}
}

// --------------------------------------------------------------------------------

static u16 ir_protocol_samsung_get_modulation_time(void) {
	
	switch (transmit_state) {

		case SAMSUNG_IR_PROTOCOL_START_PREAMBLE :
			
			IR_MOD_OUT_high_level();
			return SAMSUNG_IR_PROTOCOL_MOD_TIME_START_PREAMBLE_US;

		case SAMSUNG_IR_PROTOCOL_START :
			
			IR_MOD_OUT_low_level();
			return SAMSUNG_IR_PROTOCOL_MOD_TIME_START_PAUSE_US;

		case SAMSUNG_IR_PROTOCOL_DATA_START :
			
			IR_MOD_OUT_high_level();
			return SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_START_US;

		case SAMSUNG_IR_PROTOCOL_DATA_0 :
			
			IR_MOD_OUT_low_level();
			return SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_0_US;

		case SAMSUNG_IR_PROTOCOL_DATA_1 :
			
			IR_MOD_OUT_low_level();
			return SAMSUNG_IR_PROTOCOL_MOD_TIME_DATA_1_US;
	}
}

static void ir_protocol_samsung_calculate_modulation_time(void) {

}

// --------------------------------------------------------------------------------

void ir_protocol_samsung_set_timer(TIMER_INTERFACE_TYPE* p_timer_carrier, TIMER_INTERFACE_TYPE* p_timer_modulator) {
	p_carrier = p_timer_carrier;
	p_modulator = p_timer_modulator;
}

void ir_protocol_samsung_transmit(u8* p_data) {

	DEBUG_PASS("ir_protocol_samsung_start()");

	IR_MOD_OUT_drive_low();

	memcpy(transmit_buffer, p_data, SAMSUNG_IR_PROTOCOL_TRANSMIT_BUFFER_SIZE);

	transmit_state = SAMSUNG_IR_PROTOCOL_START;
	data_bit_counter = 0;

	TIMER_CONFIGURATION_TYPE timer_config;
	
	timer_config.frequency = TIMER_FREQUENCY_NONE;
	timer_config.irq_callback = &ir_protocol_samsung_irq_callback;
	timer_config.mode = TIMER_MODE_TIMER;
	timer_config.time_interval = TIMER_TIME_INTERVAL_560us;

	p_modulator->configure(&timer_config);
	p_modulator->start(10000); // 0.01 sec = 10 ms = 10000 us
	
	timer_config.frequency = TIMER_FREQUENCY_37_9kHz;
	timer_config.irq_callback = 0;
	timer_config.mode = TIMER_MODE_FREQUENCY;

	p_carrier->configure(&timer_config);
	p_carrier->start(SAMSUNG_IR_PROTOCOL_MOD_TIME_START_US);
}