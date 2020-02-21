/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	main_rpi_hat.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

//-----------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "command_controller.h"
#include "command_handler_interface.h"
#include "local_context.h"

//-----------------------------------------------------------------------------

#define COMMAND_CONTROLLER_SCHEDULE_INTERVAL_MS		5

//-----------------------------------------------------------------------------

static COMMAND_HANDLER_INTERFACE* _first_cmd_handler = 0;
static COMMAND_HANDLER_INTERFACE* _act_cmd_handler = 0;
static COMMAND_HANDLER_INTERFACE* _last_cmd_handler = 0;

//-----------------------------------------------------------------------------

void command_controller_init(void) {

}

void command_controller_register_handler(COMMAND_HANDLER_INTERFACE* p_handler) {

	if (_first_cmd_handler == 0) {

		PASS(); // command_controller_register_handler() - First Handler
		_first_cmd_handler = p_handler;

	} else {

		PASS(); // command_controller_register_handler() - New Handler
		_last_cmd_handler->next = p_handler;
	}

	_last_cmd_handler = p_handler;
	_last_cmd_handler->init();
	_last_cmd_handler->next = 0;
}

u8 command_controller_cmd_is_pending(void) {

	_act_cmd_handler = _first_cmd_handler;

	while (_act_cmd_handler != 0) {

		if (_act_cmd_handler->is_requested()) {
			PASS(); // command_controller_cmd_is_pending() - Command-Handler has been requested
			return 1;
		}

		_act_cmd_handler = _act_cmd_handler->next;
	}

	return 0;
}

void command_controller_handle_command(void) {

	PASS(); // command_controller_handle_command()

	if (_act_cmd_handler == 0) {
		PASS(); // command_controller_handle_command() - No command-handler selected !!! ---
		return;
	}

	TRACE_byte(_act_cmd_handler->num_command_handler); // command_controller_handle_command() - Processing Command-Handler Table

	u8 cmd_id = _act_cmd_handler->get_cmd_code();
	u8 cmd_ret_code = 0xFF;

	u8 i = 0;
	while (i < _act_cmd_handler->num_command_handler) {

		if (_act_cmd_handler->command_handler_table[i].command_id == cmd_id || _act_cmd_handler->command_handler_table[i].command_id == 0x00) {

			TRACE_byte(cmd_id); // command_controller_handle_command() - Running Command-Handler
			cmd_ret_code = _act_cmd_handler->command_handler_table[i].handle();
			break;
		}

		i++;
	}

	if (cmd_ret_code == 0xFF) {
		cmd_ret_code = _act_cmd_handler->default_handler();
	}

	_act_cmd_handler = 0;
}

u16 command_controller_task_get_schedule_interval(void) {
	return COMMAND_CONTROLLER_SCHEDULE_INTERVAL_MS;
}

MCU_TASK_INTERFACE_TASK_STATE command_controller_task_get_state(void) {
	if (command_controller_cmd_is_pending() != 0) {
		return MCU_TASK_RUNNING;
	} else {
		return MCU_TASK_SLEEPING;
	}
}


void command_controller_background_run(void) {

}


