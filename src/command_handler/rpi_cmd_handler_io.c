/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	rpi_cmd_handler_io.c
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

#include "command_management/command_handler_interface.h"
#include "command_management/command_buffer_interface.h"
#include "command_management/answer_buffer_interface.h"
#include "command_management/protocol_interface.h"

// --------------------------------------------------------------------------------

#include "common/local_context.h"
#include "common/signal_slot_interface.h"

// --------------------------------------------------------------------------------

#include "io_management/io_input_controller.h"
#include "io_management/io_output_controller.h"

#include "command_handler/rpi_command_handler.h"

//-----------------------------------------------------------------------------

u8 rpi_cmd_get_input_list(PROTOCOL_INTERFACE* p_protocol) {

	PASS(); // rpi_cmd_get_input()

	// one byte per input
	// Bit		Meaning
	//---------------------
	// 0		down
	// 1		pressed
	// 2		released
	// 3		RFU
	// 4		RFU
	// 5		RFU
	// 6		RFU
	// 7		RFU

	p_protocol->answ_buffer->start_write();

	u8 button_bitmask = 0;
	IO_INPUT_STATE button;

	ITERATOR_INTERFACE iterator;
	io_input_controller_get_iterator(&iterator);

	while (iterator.is_valid) {

		io_input_controller_iterator_get_element(&iterator, &button);

		button_bitmask  =  button.down     ? (1 << 0) : 0;
		button_bitmask |=  button.pressed  ? (1 << 1) : 0;
		button_bitmask |=  button.released ? (1 << 2) : 0;

		TRACE_byte(button_bitmask); // rpi_cmd_get_input()

		p_protocol->answ_buffer->add_byte(button.id);
		p_protocol->answ_buffer->add_byte(button_bitmask);

		io_input_controller_iterator_get_next(&iterator);
	}

	p_protocol->answ_buffer->stop_write();
	p_protocol->set_finished(CMD_NO_ERR);

	return CMD_NO_ERR;
}

u8 rpi_cmd_get_input_state(PROTOCOL_INTERFACE* p_protocol) {

	if (p_protocol->cmd_buffer->bytes_available() == 0) {

		PASS(); // rpi_cmd_get_input_state() - No Argument given !!! ---

		p_protocol->set_finished(CMD_ERR_INVALID_COMMAND);
		return CMD_ERR_INVALID_COMMAND;
	}

	p_protocol->cmd_buffer->start_read();
	u8 button_index = p_protocol->cmd_buffer->get_byte();
	p_protocol->cmd_buffer->stop_read();

	IO_INPUT_STATE button;
	io_input_controller_get_state(button_index, &button);

	u8 button_bitmask  =  button.down     ? (1 << 0) : 0;
	button_bitmask    |=  button.pressed  ? (1 << 1) : 0;
	button_bitmask    |=  button.released ? (1 << 2) : 0;

	p_protocol->answ_buffer->start_write();
	p_protocol->answ_buffer->add_byte(button_index);
	p_protocol->answ_buffer->add_byte(button_bitmask);
	p_protocol->answ_buffer->add_long(button.down_time);
	p_protocol->answ_buffer->stop_write();

	TRACE_byte(button_bitmask); // rpi_cmd_get_input() - onboard_button_01

	p_protocol->set_finished(CMD_NO_ERR);
	return CMD_NO_ERR;
}

u8 rpi_cmd_get_output_list(PROTOCOL_INTERFACE* p_protocol) {

	PASS(); // rpi_cmd_get_output_list()

	p_protocol->answ_buffer->start_write();
	IO_OUTPUT_STATE output;

	ITERATOR_INTERFACE iterator;
	io_output_controller_get_iterator(&iterator, IO_TYPE_USER);

	while (iterator.is_valid) {

		io_output_controller_iterator_get_element(&iterator, &output);

		p_protocol->answ_buffer->add_byte(output.id);
		p_protocol->answ_buffer->add_byte(output.pin_state);
		TRACE_byte(output.id); // rpi_cmd_get_output()

		io_output_controller_iterator_get_next(&iterator, IO_TYPE_USER);
	}

	p_protocol->answ_buffer->stop_write();
	p_protocol->set_finished(CMD_NO_ERR);

	return CMD_NO_ERR;
}

u8 rpi_cmd_get_output_state(PROTOCOL_INTERFACE* p_protocol) {

	PASS(); // rpi_cmd_get_output_state()

	if (p_protocol->cmd_buffer->bytes_available() == 0) {

		PASS(); // rpi_cmd_get_output_state() - No Argument given !!! ---

		p_protocol->set_finished(CMD_ERR_INVALID_COMMAND);
		return CMD_ERR_INVALID_COMMAND;
	}

	p_protocol->cmd_buffer->start_read();
	u8 output_index = p_protocol->cmd_buffer->get_byte();
	p_protocol->cmd_buffer->stop_read();

	TRACE_byte(output_index); // rpi_cmd_get_output_state()

	IO_OUTPUT_STATE output;
	io_output_controller_get_output(output_index, &output);

	p_protocol->answ_buffer->start_write();
	p_protocol->answ_buffer->add_byte(output_index);
	p_protocol->answ_buffer->add_byte(output.pin_state);
	p_protocol->answ_buffer->add_long(output.on_time);
	p_protocol->answ_buffer->add_long(output.toggle_period);
	p_protocol->answ_buffer->add_long(output.duration);
	p_protocol->answ_buffer->stop_write();

	p_protocol->set_finished(CMD_NO_ERR);
	return CMD_NO_ERR;
}

u8 rpi_cmd_set_output(PROTOCOL_INTERFACE* p_protocol) {

	PASS(); // rpi_cmd_set_output()

	if (p_protocol->cmd_buffer->bytes_available() < 2) {

		PASS(); // rpi_cmd_set_output() - No argument was given
		p_protocol->set_finished(CMD_ERR_INVARG);
		return CMD_ERR_INVARG;
	}
	
	if (p_protocol->cmd_buffer->bytes_available() < 10) {
	
		PASS(); // rpi_cmd_set_output() - Not enough bytes to execute command !!! ---
		p_protocol->set_finished(CMD_ERR_INVARG);
		return CMD_ERR_INVARG;
	}

	u8 err_code = CMD_NO_ERR;

	while (p_protocol->cmd_buffer->bytes_available() >= 10) {

		u8 id = p_protocol->cmd_buffer->get_byte();
		u8 state = p_protocol->cmd_buffer->get_byte();

		TRACE_byte(id); // rpi_cmd_set_output()
		TRACE_byte(state); // rpi_cmd_set_output()

		u32 duration = p_protocol->cmd_buffer->get_long();
		TRACE_long(duration); // rpi_cmd_set_output() - Duration

		u32 toggle_period = p_protocol->cmd_buffer->get_long();
		TRACE_long(toggle_period); // rpi_cmd_set_output() - toggle-Period

		if (io_output_controller_set_output(id, state, duration, toggle_period) == 0) {
			PASS(); // rpi_cmd_set_output() - Output not set - wrong ID !!!
			err_code = CMD_ERR_INVARG;
			break;
		}
	}

	p_protocol->set_finished(err_code);
	return err_code;
}