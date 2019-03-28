/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "system_interface.h"
#include "io_virtual_controller.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_OFF
#include "tracer.h"

//-----------------------------------------------------------------------------

/*!
 *
 */
static IO_OUTPUT_DESCRIPTOR* _output_list_first = 0;

/*!
 *
 */
static IO_OUTPUT_DESCRIPTOR* _output_list_last = 0;

/*!
 *
 */
static volatile u8 output_is_active = 0;

void io_output_controller_init(void) {
	output_is_active = 0;
}

u8 io_output_controller_register_output(IO_OUTPUT_DESCRIPTOR* p_new_output) {

	if (_output_list_first == 0) {

		_output_list_first = p_new_output;
		_output_list_last = _output_list_first;
		_output_list_last->id = 1;

	} else {

		u8 prev_id = _output_list_last->id;
		_output_list_last->_next = p_new_output;
		_output_list_last = p_new_output;
		_output_list_last->id = prev_id + 1;
	}

	_output_list_last->actual_pin_state = 0xFF;
	_output_list_last->next_pin_state = IO_OUTPUT_STATE_OFF;
	_output_list_last->reference_time = 0;
	_output_list_last->duration = 0;
	_output_list_last->toggle_period = 0;
	_output_list_last->_next = 0;

	output_is_active = 1;

	i_system.io.init_pin(p_new_output->pin_descriptor);

	return _output_list_last->id;
}

u8 io_output_controller_set_output(u8 id, u8 state, u32 duration, u32 toggle_peridod) {

	IO_OUTPUT_DESCRIPTOR* act_output = _output_list_first;
	while (act_output != 0) {

		if (act_output->id == id) {

			TRACE_byte(id); // io_output_controller_set_output()
			TRACE_byte(state); //
			TRACE_byte(duration); //

			act_output->next_pin_state = state != 0 ? IO_OUTPUT_STATE_ON : IO_OUTPUT_STATE_OFF;
			act_output->duration = duration;
			act_output->toggle_period = toggle_peridod;

			output_is_active = 1;
			return 1;
		}

		act_output = act_output->_next;
	}

	return 0;
}


void io_output_controller_get_output(u8 id, IO_OUTPUT_STATE* p_state) {

	IO_OUTPUT_DESCRIPTOR* act_output = _output_list_first;

	while (act_output != 0) {

		if (act_output->id == id) {

			p_state->pin_state = act_output->actual_pin_state;
			p_state->toggle_period = act_output->toggle_period;
			p_state->duration = act_output->duration;

			if (act_output->actual_pin_state == IO_OUTPUT_STATE_ON) {
				p_state->on_time = i_system.time.now_u32() - act_output->reference_time;
			} else {
				p_state->on_time = 0;
			}

			return;
		}

		act_output = act_output->_next;
	}

	p_state->pin_state = IO_OUTPUT_STATE_OFF;
	p_state->toggle_period = 0;
	p_state->duration = 0;
	p_state->on_time = 0;
}

void io_output_controller_task_init(void) {

}

MCU_TASK_INTERFACE_TASK_STATE io_output_controller_task_get_state(void) {

	if (output_is_active != 0) {
		return MCU_TASK_RUNNING;
	} else {
		return MCU_TASK_SLEEPING;
	}
}

void io_output_controller_task_run(void) {

	#ifdef TRACES_ENABLED
	u32 actual_time = i_system.time.now_u32();
	TRACE_long(actual_time); // io_output_controller_task_run() ------------------
	#endif

	IO_OUTPUT_DESCRIPTOR* act_output = _output_list_first;

	output_is_active = 0;

	while (act_output != 0) {

		if (act_output->actual_pin_state == IO_OUTPUT_STATE_ON) {
			if (act_output->duration != 0) {

				if (i_system.time.isup_u32(act_output->reference_time, act_output->duration)) {

					PASS(); // io_output_controller_task_run() - set pin back to off-state
					act_output->next_pin_state = IO_OUTPUT_STATE_OFF;

				} else {
					output_is_active = 1;
				}
			}
		}

		if (act_output->actual_pin_state != act_output->next_pin_state) {

			TRACE_byte(act_output->id); // io_output_controller_task_run() changing pin-state

			act_output->actual_pin_state = act_output->next_pin_state;
			SYSTEM_INTERFACE_GPIO_LEVEL level;

			if (act_output->actual_pin_state == IO_OUTPUT_STATE_ON) {
				level = GPIO_LEVEL_HIGH;
			} else {
				level = GPIO_LEVEL_LOW;
			}

			i_system.io.set_level(act_output->pin_descriptor, level); //act_output->set_pin(act_output->next_pin_state);

			if (act_output->actual_pin_state == IO_OUTPUT_STATE_ON) {

				act_output->reference_time = i_system.time.now_u32();
				TRACE_long(act_output->reference_time); // io_output_controller_task_run() - Reference Time

			} else {

				#ifdef TRACES_ENABLED
				act_output->reference_time = i_system.time.now_u32() - act_output->reference_time;
				TRACE_long(act_output->reference_time); // io_output_controller_task_run() - output was on for this time of milliseconds
				#endif

				act_output->reference_time = 0;
				act_output->toggle_period = 0;
				act_output->duration = 0;

				if (act_output->actual_pin_state == IO_OUTPUT_STATE_DISABLED) {

				}
			}

			if (act_output->duration != 0) {
				output_is_active = 1;
			}
		}

		if (act_output->toggle_period != 0) {

			if (act_output->duration != 0) {

			}

		}

		act_output = act_output->_next;
	}
}

void io_output_controller_task_background_run(void) {

}


void io_output_controller_get_iterator(ITERATOR_INTERFACE* p_iterator, IO_TYPE type_filter) {

	if (_output_list_first == 0) {
		p_iterator->is_valid = 0;
		p_iterator->is_last  = 0;
		p_iterator->is_first = 0;
		return;
	}

	IO_OUTPUT_DESCRIPTOR* act_output = _output_list_first;
	while (type_filter != IO_TYPE_UNDEFINED && act_output->type != type_filter) {

		if (act_output->_next == 0) {
			p_iterator->is_valid = 0;
			p_iterator->is_last  = 0;
			p_iterator->is_first = 0;
			return;
		}
		act_output = act_output->_next;
	}

	p_iterator->__element = (void*) act_output;
	p_iterator->is_first = 1;
	p_iterator->is_valid = 1;

	if (_output_list_first->_next != 0) {
		p_iterator->is_last = 0;
	} else {
		p_iterator->is_last = 1;
	}
}

void io_output_controller_iterator_get_element(ITERATOR_INTERFACE* p_iterator, IO_OUTPUT_STATE* p_output_state) {

	if (p_iterator->is_valid == 0) {
		return;
	}

	IO_OUTPUT_DESCRIPTOR* p_output = (IO_OUTPUT_DESCRIPTOR*)p_iterator->__element;

	p_output_state->id = p_output->id;
	p_output_state->pin_state = p_output->actual_pin_state;
	p_output_state->toggle_period = p_output->toggle_period;
	p_output_state->duration = p_output->duration;

	if (p_output->reference_time != 0) {
		p_output_state->on_time = i_system.time.now_u32() - p_output->reference_time;
	} else {
		p_output_state->on_time = 0;
	}
}

void io_output_controller_iterator_get_next(ITERATOR_INTERFACE* p_iterator, IO_TYPE type_filter) {

	IO_OUTPUT_DESCRIPTOR* p_output = (IO_OUTPUT_DESCRIPTOR*)p_iterator->__element;

	if (p_output == 0) {
		p_iterator->is_valid = 0;
		p_iterator->is_last  = 0;
		p_iterator->is_first = 0;
		return;
	}

	if (p_output->_next == 0) {
		p_iterator->is_valid = 0;
		p_iterator->is_last  = 0;
		p_iterator->is_first = 0;
		return;
	}

	IO_OUTPUT_DESCRIPTOR* act_output = p_output->_next;
	while (type_filter != IO_TYPE_UNDEFINED && act_output->type != type_filter) {

		if (act_output->_next == 0) {
			p_iterator->is_valid = 0;
			p_iterator->is_last  = 0;
			p_iterator->is_first = 0;
			return;
		}
		act_output = act_output->_next;
	}

	p_output = act_output;

	p_iterator->__element = (void*) p_output;
	p_iterator->is_first = 0;
	p_iterator->is_valid = 1;

	if (p_output->_next != 0) {
		p_iterator->is_last = 0;
	} else {
		p_iterator->is_last = 1;
	}
}

