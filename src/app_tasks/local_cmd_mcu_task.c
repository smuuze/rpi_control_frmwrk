/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "local_cmd_mcu_task.h"
#include "command_controller.h"

void local_cmd_mcu_task_init(void) {
	command_controller_init();
	local_cmd_protocol_initialization();
}

u8 local_cmd_mcu_task_is_runable(void) {
	return command_controller_cmd_is_pending();
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
