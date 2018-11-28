/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "utils/stdmacros.h"

#include "system_interface.h"
#include "button_watcher.h"

#define noTRACES
#include <traces.H>

#define IO_INPUT_CONTROLLER_TASK_RUN_INTERVAL_MS	30

#define BUTTON_IS_DOWN(p_button_state)			(p_button_state->down == 1 ? 1 : 0)
#define BUTTON_IS_RELEASED(p_button_state, timestamp)	(p_button_state->released == 1 && p_button_state->__release_time + IO_BUTTON_RELEASED_TIMESPAN_MS < timestamp)

/*!
 *
 */
static IO_INPUT_DESCRIPTOR* p_first_button = 0;

/*!
 *
 */
static IO_INPUT_DESCRIPTOR* p_last_button = 0;

/*!
 *
 */
static u16 task_run_interval_reference = 0;


static void _init_state(IO_INPUT_DESCRIPTOR* p_button_state) {

	p_button_state->__down_time = 0;
	p_button_state->__press_time = 0;
	p_button_state->__release_time = 0;

	p_button_state->down = 0;
	p_button_state->pressed = 0;
	p_button_state->released = 0;
}

void io_input_controller_init(void) {
	//system_get_pointer(p_system);
}

u8 io_input_controller_register_input(IO_INPUT_DESCRIPTOR* p_new_button) {

	if (p_first_button == 0) {

		p_first_button = p_new_button;
		p_last_button = p_new_button;
		p_last_button->__next_button = 0;
		p_last_button->id = 1;

		TRACE_byte(p_last_button->id); // button_watcher_register_button() - first button registered

	} else {

		u8 prev_id = p_last_button->id;
		p_last_button->__next_button = p_new_button;
		p_last_button = p_last_button->__next_button;
		p_last_button->id = prev_id + 1;
		p_last_button->__next_button = 0;

		TRACE_byte(p_last_button->id); // button_watcher_register_button() - new button registered
	}

	_init_state(p_last_button);

	return p_last_button->id;
}

void io_input_controller_check_state(IO_INPUT_DESCRIPTOR* p_button_state) {

	//PASS(); // io_input_controller_check_state() ----------------

	u8 pin_state = p_button_state->__pin_state();

	if (pin_state != 0) {

		if (p_button_state->down == 0) {

			p_button_state->down = 1;
			p_button_state->__down_time = i_system.time.now_u32();

			p_button_state->pressed = 1;
			p_button_state->__press_time = i_system.time.now_u16();

			p_button_state->released = 0;
			p_button_state->__release_time = 0;

			TRACE_byte(p_button_state->id); // button_watcher_check_state() - button just has been pressed

			i_system.event.add(SYS_EVT_INPUT_CHANGED);

		}

		if (p_button_state->down != 0 && p_button_state->pressed != 0) {

			if (i_system.time.isup_u16(p_button_state->__press_time, IO_BUTTON_PRESSED_TIMESPAN_MS)) {
				p_button_state->pressed = 0;
				TRACE_byte(p_button_state->id); // button_watcher_check_state() - button in active mode
			}
		}

	} else {

		if (p_button_state->down != 0 && p_button_state->released == 0) {

			p_button_state->released = 1;
			p_button_state->__release_time = i_system.time.now_u16();

			TRACE_byte(p_button_state->id); // button_watcher_check_state() - button just has been released
			TRACE_long(p_button_state->__down_time); // button_watcher_check_state() - Down Time in MS

			i_system.event.add(SYS_EVT_INPUT_CHANGED);

		} else if (p_button_state->__release_time != 0 && i_system.time.isup_u16(p_button_state->__release_time, IO_BUTTON_RELEASED_TIMESPAN_MS)) {

			p_button_state->released = 0;
			p_button_state->__release_time = 0;
			p_button_state->__press_time = 0;

			TRACE_byte(p_button_state->id); // button_watcher_check_state() - button in inactive mode
		}

		p_button_state->down = 0;
		p_button_state->pressed = 0;
		p_button_state->__press_time = 0;
		p_button_state->__down_time = 0;
	}
}

void io_input_controller_get_state(u8 button_id, IO_INPUT_STATE* p_button_state) {

	IO_INPUT_DESCRIPTOR* p_act_button = p_first_button;

	while (p_act_button != 0) {

		if (p_act_button->id == button_id) {

			p_button_state->down = p_act_button->down;
			p_button_state->pressed = p_act_button->pressed;
			p_button_state->released = p_act_button->released;

			if (p_button_state->down) {
				p_button_state->down_time = i_system.time.now_u32() - p_act_button->__down_time;
			} else {
				p_button_state->down_time = 0;
			}

			// states have been read, so these events are out-dated
			p_act_button->released = 0;
			p_act_button->pressed = 0;

			return;
		}

		p_act_button = p_act_button->__next_button;
	}

	p_button_state->down = 0;
	p_button_state->pressed = 0;
	p_button_state->released = 0;
	p_button_state->down_time = 0;
}


void io_input_controller_task_init(void) {
	task_run_interval_reference = i_system.time.now_u16();
}

MCU_TASK_INTERFACE_TASK_STATE io_input_controller_task_get_state(void) {

	if (i_system.time.isup_u16(task_run_interval_reference, IO_INPUT_CONTROLLER_TASK_RUN_INTERVAL_MS) != 0) {
		return MCU_TASK_RUNNING;
	} else {
		return MCU_TASK_SLEEPING;
	}
}

void io_input_controller_task_run(void) {

	#ifdef TRACES_ENABLED
	u32 actual_time = i_system.time.now_u32();
	TRACE_long(actual_time); // rpi_protocol_task_run() ------------------
	#endif

	IO_INPUT_DESCRIPTOR* p_act_button = p_first_button;

	while (p_act_button != 0) {
		io_input_controller_check_state(p_act_button);
		p_act_button = p_act_button->__next_button;
	}

	task_run_interval_reference = i_system.time.now_u16();
}

void io_input_controller_task_background_run(void) {

}

void io_input_controller_task_sleep(void) {

}

void io_input_controller_task_wakeup(void) {

}

void io_input_controller_task_finish(void) {

}

void io_input_controller_task_terminate(void) {

}


void io_input_controller_get_iterator(ITERATOR_INTERFACE* p_iterator) {

	if (p_first_button == 0) {
		p_iterator->is_valid = 0;
		p_iterator->is_last  = 0;
		p_iterator->is_first = 0;
		return;
	}

	p_iterator->__element = (void*) p_first_button;
	p_iterator->is_first = 1;
	p_iterator->is_valid = 1;

	if (p_first_button->__next_button != 0) {
		p_iterator->is_last = 0;
	} else {
		p_iterator->is_last = 1;
	}
}

void io_input_controller_iterator_get_element(ITERATOR_INTERFACE* p_iterator, IO_INPUT_STATE* p_button_state) {

	if (p_iterator->is_valid == 0) {
		return;
	}

	IO_INPUT_DESCRIPTOR* p_button = (IO_INPUT_DESCRIPTOR*)p_iterator->__element;

	p_button_state->id = p_button->id;
	p_button_state->down = p_button->down;
	p_button_state->pressed = p_button->pressed;
	p_button_state->released = p_button->released;

	if (p_button_state->down) {
		p_button_state->down_time = i_system.time.now_u32() - p_button->__down_time;
	} else {
		p_button_state->down_time = 0;
	}
}

void io_input_controller_iterator_get_next(ITERATOR_INTERFACE* p_iterator) {

	IO_INPUT_DESCRIPTOR* p_button = (IO_INPUT_DESCRIPTOR*)p_iterator->__element;

	if (p_button == 0) {
		p_iterator->is_valid = 0;
		p_iterator->is_last  = 0;
		p_iterator->is_first = 0;
		return;
	}

	if (p_button->__next_button == 0) {
		p_iterator->is_valid = 0;
		p_iterator->is_last  = 0;
		p_iterator->is_first = 0;
		return;
	}

	p_button = p_button->__next_button;

	p_iterator->__element = (void*) p_button;
	p_iterator->is_first = 0;
	p_iterator->is_valid = 1;

	if (p_button->__next_button != 0) {
		p_iterator->is_last = 0;
	} else {
		p_iterator->is_last = 1;
	}
}


