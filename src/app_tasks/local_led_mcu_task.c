/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	local_led_mcu_task.c
 * \brief
 * \author	sebastian lesse
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

#include "io_input_controller.h"
#include "io_output_controller.h"

// --------------------------------------------------------------------------------

#include "local_context.h"
#include "local_led_mcu_task.h"

// --------------------------------------------------------------------------------

#include "system_interface.h"

// --------------------------------------------------------------------------------
/*!
 *  LED Matrix Index
 *
 *  	LED_1	LED_2	LED_3
 *  	LED_4	LED_5	LED_6
 *  	LED_7	LED_8	LED_9
 */

static u8 _led_state = 0;


void local_led_mcu_task_init(void) {

	_led_state = 0;
}


u8 local_led_mcu_task_is_runable(void) {
	return 1;
}


void local_led_mcu_task_run(void) {

	IO_INPUT_STATE onboard_button_01;
	IO_INPUT_STATE onboard_button_02;

	io_input_controller_get_state(GET_SYSTEM(SYS_INPUT).onboard_button_01, &onboard_button_01);
	io_input_controller_get_state(GET_SYSTEM(SYS_INPUT).onboard_button_02, &onboard_button_02);

	u8 state_array[9] = {
		IO_OUTPUT_STATE_OFF, IO_OUTPUT_STATE_OFF, IO_OUTPUT_STATE_OFF,
		IO_OUTPUT_STATE_OFF, IO_OUTPUT_STATE_OFF, IO_OUTPUT_STATE_OFF,
		IO_OUTPUT_STATE_OFF, IO_OUTPUT_STATE_OFF, IO_OUTPUT_STATE_OFF
	};

	u8 state_changed = 0;

	if (onboard_button_01.pressed) {

		state_array[0] = IO_OUTPUT_STATE_ON;	state_array[1] = IO_OUTPUT_STATE_ON; state_array[2] = IO_OUTPUT_STATE_ON;
		state_array[3] = IO_OUTPUT_STATE_ON;	state_array[4] = IO_OUTPUT_STATE_ON; state_array[5] = IO_OUTPUT_STATE_ON;
		state_array[6] = IO_OUTPUT_STATE_ON;	state_array[7] = IO_OUTPUT_STATE_ON; state_array[8] = IO_OUTPUT_STATE_ON;

		state_changed = 1;

	} else if (onboard_button_01.released) {

		state_array[0] = IO_OUTPUT_STATE_OFF;	state_array[1] = IO_OUTPUT_STATE_OFF; state_array[2] = IO_OUTPUT_STATE_OFF;
		state_array[3] = IO_OUTPUT_STATE_OFF;	state_array[4] = IO_OUTPUT_STATE_ON; state_array[5] = IO_OUTPUT_STATE_OFF;
		state_array[6] = IO_OUTPUT_STATE_OFF;	state_array[7] = IO_OUTPUT_STATE_OFF; state_array[8] = IO_OUTPUT_STATE_OFF;

		state_changed = 1;

	} else if (onboard_button_01.down) {

		state_array[0] = IO_OUTPUT_STATE_OFF;	state_array[1] = IO_OUTPUT_STATE_ON; state_array[2] = IO_OUTPUT_STATE_OFF;
		state_array[3] = IO_OUTPUT_STATE_ON;	state_array[4] = IO_OUTPUT_STATE_ON; state_array[5] = IO_OUTPUT_STATE_ON;
		state_array[6] = IO_OUTPUT_STATE_OFF;	state_array[7] = IO_OUTPUT_STATE_ON; state_array[8] = IO_OUTPUT_STATE_OFF;

		state_changed = 1;

	} else if (onboard_button_02.down) {

		state_array[0] = IO_OUTPUT_STATE_ON;

		if (onboard_button_02.down_time > 1000) { state_array[1] = IO_OUTPUT_STATE_ON; }
		if (onboard_button_02.down_time > 2000) { state_array[2] = IO_OUTPUT_STATE_ON; }
		if (onboard_button_02.down_time > 3000) { state_array[3] = IO_OUTPUT_STATE_ON; }
		if (onboard_button_02.down_time > 4000) { state_array[4] = IO_OUTPUT_STATE_ON; }
		if (onboard_button_02.down_time > 5000) { state_array[5] = IO_OUTPUT_STATE_ON; }
		if (onboard_button_02.down_time > 6000) { state_array[6] = IO_OUTPUT_STATE_ON; }
		if (onboard_button_02.down_time > 7000) { state_array[7] = IO_OUTPUT_STATE_ON; }
		if (onboard_button_02.down_time > 8000) { state_array[8] = IO_OUTPUT_STATE_ON; }

		state_changed = 1;
	}

	if (state_changed != 0) {

		io_output_controller_set_output(GET_SYSTEM(SYS_OUTPUT).extern_output_01, state_array[0], 0, 0);
		io_output_controller_set_output(GET_SYSTEM(SYS_OUTPUT).extern_output_02, state_array[1], 0, 0);
		io_output_controller_set_output(GET_SYSTEM(SYS_OUTPUT).extern_output_03, state_array[2], 0, 0);
		io_output_controller_set_output(GET_SYSTEM(SYS_OUTPUT).extern_output_04, state_array[3], 0, 0);


		io_output_controller_set_output(GET_SYSTEM(SYS_SIGNAL).led_01, state_array[0], 0, 0);
		io_output_controller_set_output(GET_SYSTEM(SYS_SIGNAL).led_02, state_array[1], 0, 0);
		io_output_controller_set_output(GET_SYSTEM(SYS_SIGNAL).led_03, state_array[2], 0, 0);
		io_output_controller_set_output(GET_SYSTEM(SYS_SIGNAL).led_04, state_array[3], 0, 0);
		io_output_controller_set_output(GET_SYSTEM(SYS_SIGNAL).led_05, state_array[4], 0, 0);
		io_output_controller_set_output(GET_SYSTEM(SYS_SIGNAL).led_06, state_array[5], 0, 0);
		io_output_controller_set_output(GET_SYSTEM(SYS_SIGNAL).led_07, state_array[6], 0, 0);
		io_output_controller_set_output(GET_SYSTEM(SYS_SIGNAL).led_08, state_array[7], 0, 0);
		io_output_controller_set_output(GET_SYSTEM(SYS_SIGNAL).led_09, state_array[8], 0, 0);
	}
}

void local_led_mcu_task_background_run(void) {

}

void local_led_mcu_task_sleep(void) {

}

void local_led_mcu_task_wakeup(void) {

}

void local_led_mcu_task_finish(void) {

}

void local_led_mcu_task_terminate(void) {

}
