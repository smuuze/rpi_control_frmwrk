
 /*
  * @file	command_management/command_controller.c
  * @author	sebastian lesse
  */

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!

//-----------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "command_controller.h"
#include "command_handler_interface.h"
#include "local_context.h"

#include "mcu_task_management/mcu_task_controller.h"
#include "mcu_task_management/mcu_task_interface.h"

//-----------------------------------------------------------------------------

#define COMMAND_CONTROLLER_SCHEDULE_INTERVAL_MS		5

//-----------------------------------------------------------------------------

/*!
 *
 */
static void command_controller_task_init(void);

/*!
 *
 */
static u16 command_controller_task_get_schedule_interval(void);

/*!
 *
 */
static MCU_TASK_INTERFACE_TASK_STATE command_controller_task_get_state(void);

/*!
 *
 */
static void command_controller_task_run(void);

/*!
 *
 */
static void command_controller_task_background_run(void);

//-----------------------------------------------------------------------------

static MCU_TASK_INTERFACE cmd_mcu_task = {

	0, 						// u8 identifier,
	0, 						// u16 new_run_timeout,
	0, 						// u16 last_run_time,
	&command_controller_task_init, 			// MCU_TASK_INTERFACE_INIT_CALLBACK			init,
	&command_controller_task_get_schedule_interval,	// MCU_TASK_INTERFACE_INIT_CALLBACK			get_schedule_interval,
	&command_controller_task_get_state, 		// MCU_TASK_INTERFACE_GET_STATE_CALLBACK		get_sate,
	&command_controller_task_run, 			// MCU_TASK_INTERFACE_RUN_CALLBACK			run,
	&command_controller_task_background_run,	// MCU_TASK_INTERFACE_BG_RUN_CALLBACK			background_run,
	0, 						// MCU_TASK_INTERFACE_SLEEP_CALLBACK			sleep,
	0, 						// MCU_TASK_INTERFACE_WAKEUP_CALLBACK			wakeup,
	0, 						// MCU_TASK_INTERFACE_FINISH_CALLBACK			finish,
	0, 						// MCU_TASK_INTERFACE_TERMINATE_CALLBACK		terminate,
	0						// next-task
};

//-----------------------------------------------------------------------------

static COMMAND_HANDLER_INTERFACE* _first_cmd_handler = 0;
static COMMAND_HANDLER_INTERFACE* _act_cmd_handler = 0;
static COMMAND_HANDLER_INTERFACE* _last_cmd_handler = 0;

//-----------------------------------------------------------------------------

void command_controller_init(void) {
	DEBUG_PASS("command_controller_init()");
	mcu_task_controller_register_task(&cmd_mcu_task);
}

//-----------------------------------------------------------------------------

static void command_controller_task_init(void) {
	DEBUG_PASS("command_controller_task_init()");
}

void command_controller_register_handler(COMMAND_HANDLER_INTERFACE* p_handler) {

	if (_first_cmd_handler == 0) {

		DEBUG_PASS("command_controller_register_handler() - First Handler");
		_first_cmd_handler = p_handler;

	} else {

		DEBUG_PASS("command_controller_register_handler() - New Handler");
		_last_cmd_handler->next = p_handler;
	}

	_last_cmd_handler = p_handler;
	_last_cmd_handler->next = 0;
}

static u16 command_controller_task_get_schedule_interval(void) {
	return COMMAND_CONTROLLER_SCHEDULE_INTERVAL_MS;
}

static MCU_TASK_INTERFACE_TASK_STATE command_controller_task_get_state(void) {

	_act_cmd_handler = _first_cmd_handler;

	while (_act_cmd_handler != 0) {

		if (_act_cmd_handler->is_requested()) {

			DEBUG_PASS("command_controller_cmd_is_pending() - Command-Handler has been requested");
			return MCU_TASK_RUNNING;
		}

		_act_cmd_handler = _act_cmd_handler->next;
	}
}

static void command_controller_task_run(void) {

	DEBUG_PASS("command_controller_task_run()");

	if (_act_cmd_handler == 0) {
		DEBUG_PASS("command_controller_task_run() - No command-handler selected !!! ---");
		return;
	}

	DEBUG_TRACE_byte(_act_cmd_handler->get_table_size(), "command_controller_task_run() - Processing Command-Handler Table");

	u8 cmd_id = _act_cmd_handler->get_cmd_code();
	u8 cmd_ret_code = 0xFF;

	u8 i = 0;
	while (i < _act_cmd_handler->get_table_size()) {

		if (_act_cmd_handler->command_handler_table[i].command_id == cmd_id || _act_cmd_handler->command_handler_table[i].command_id == 0x00) {

			DEBUG_TRACE_byte(cmd_id, "command_controller_task_run() - Running Command-Handler");
			cmd_ret_code = _act_cmd_handler->command_handler_table[i].handle(_act_cmd_handler->get_protocol());
			break;
		}

		i++;
	}

	if (cmd_ret_code == 0xFF) {
		cmd_ret_code = _act_cmd_handler->default_handler(_act_cmd_handler->get_protocol());
	}

	_act_cmd_handler->unset_reqeust();

	_act_cmd_handler = 0;
}

static void command_controller_task_background_run(void) {

}


