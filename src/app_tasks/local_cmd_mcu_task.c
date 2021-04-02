/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	local_cmd_mcu_task.c
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

#include "mcu_task_interface.h"
#include "local_cmd_mcu_task.h"
#include "command_controller.h"

// --------------------------------------------------------------------------------

#define LOCAL_CMD_MCU_TASK_SCHEDULE_INTERVAL_MS		5

// --------------------------------------------------------------------------------

void local_cmd_mcu_task_init(void) {
	command_controller_init();
	//local_cmd_protocol_initialization();
}

u16 local_cmd_task_get_schedule_interval(void) {
	return LOCAL_CMD_MCU_TASK_SCHEDULE_INTERVAL_MS;
}

MCU_TASK_INTERFACE_TASK_STATE local_cmd_mcu_task_get_state(void){
	return command_controller_cmd_is_pending() ? MCU_TASK_RUNNING : MCU_TASK_SLEEPING;
}

void local_cmd_mcu_task_run(void) {
	command_controller_handle_command();
}

void local_cmd_mcu_task_background_run(void) {
	command_controller_background_run();
}

void local_cmd_mcu_task_finish(void) {
	
}

void local_cmd_mcu_task_terminate(void) {
	
}
