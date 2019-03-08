/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "mcu_task_controller.h"
#include "mcu_task_interface.h"


//---------- Implementation of Traces -----------------------------------------

//#define TRACES
//#include <traces.h>

#define TRACER_OFF
#include "tracer.h"

#ifndef config_HAS_LED_MATRIX
#define config_HAS_LED_MATRIX 0
#endif

#if config_HAS_LED_MATRIX == 1
#include "local_led_mcu_task.h"
static MCU_TASK_INTERFACE led_mcu_task = {

	0, 					// u8 identifier,
	100, 					// const u16 SCHEDULE_INTERVAL,
	15,					// const ux16 WORST_CASE_EXECUTION_TIME;
	0, 					// u16 new_run_timeout,
	0, 					// u16 last_run_time,
	&local_led_mcu_task_init, 		// MCU_TASK_INTERFACE_INIT_CALLBACK		init,
	&local_led_mcu_task_is_runable, 	// CU_TASK_INTERFACE_IS_RUNABLE_CALLBACK	is_runable,
	&local_led_mcu_task_run, 		// MCU_TASK_INTERFACE_RUN_CALLBACK		run,
	0,					// MCU_TASK_INTERFACE_BG_RUN_CALLBACK		background_run,
	0, 					// MCU_TASK_INTERFACE_SLEEP_CALLBACK		sleep,
	0, 					// MCU_TASK_INTERFACE_WAKEUP_CALLBACK		wakeup,
	&local_led_mcu_task_finish, 		// MCU_TASK_INTERFACE_FINISH_CALLBACK		finish,
	&local_led_mcu_task_terminate, 		// MCU_TASK_INTERFACE_TERMINATE_CALLBACK	terminate,
	0					// next-task
};
#endif



#include "local_ads1115_mcu_task.h"
static MCU_TASK_INTERFACE ads1115_mcu_task = {

	0, 					// u8 identifier,
	50, 					// const u16 SCHEDULE_INTERVAL,
	15,					// const ux16 WORST_CASE_EXECUTION_TIME;
	0, 					// u16 new_run_timeout,
	0, 					// u16 last_run_time,
	&local_ads1115_mcu_task_init, 		// MCU_TASK_INTERFACE_INIT_CALLBACK		init,
	&local_ads1115_mcu_task_get_state, 	// CU_TASK_INTERFACE_IS_RUNABLE_CALLBACK	is_runable,
	&local_ads1115_mcu_task_run, 		// MCU_TASK_INTERFACE_RUN_CALLBACK		run,
	0,					// MCU_TASK_INTERFACE_BG_RUN_CALLBACK		background_run,
	0, 					// MCU_TASK_INTERFACE_SLEEP_CALLBACK		sleep,
	0, 					// MCU_TASK_INTERFACE_WAKEUP_CALLBACK		wakeup,
	&local_ads1115_mcu_task_finish, 	// MCU_TASK_INTERFACE_FINISH_CALLBACK		finish,
	&local_ads1115_mcu_task_terminate, 	// MCU_TASK_INTERFACE_TERMINATE_CALLBACK	terminate,
	0					// next-task
};



#include "local_sht31_mcu_task.h"
static MCU_TASK_INTERFACE sht31_mcu_task = {

	0, 					// u8 identifier,
	50, 					// const u16 SCHEDULE_INTERVAL,
	15,					// const ux16 WORST_CASE_EXECUTION_TIME;
	0, 					// u16 new_run_timeout,
	0, 					// u16 last_run_time,
	&local_sht31_mcu_task_init, 		// MCU_TASK_INTERFACE_INIT_CALLBACK		init,
	&local_sht31_mcu_task_get_state, 	// CU_TASK_INTERFACE_IS_RUNABLE_CALLBACK	is_runable,
	&local_sht31_mcu_task_run, 		// MCU_TASK_INTERFACE_RUN_CALLBACK		run,
	0,					// MCU_TASK_INTERFACE_BG_RUN_CALLBACK		background_run,
	0, 					// MCU_TASK_INTERFACE_SLEEP_CALLBACK		sleep,
	0, 					// MCU_TASK_INTERFACE_WAKEUP_CALLBACK		wakeup,
	&local_sht31_mcu_task_finish, 		// MCU_TASK_INTERFACE_FINISH_CALLBACK		finish,
	&local_sht31_mcu_task_terminate, 	// MCU_TASK_INTERFACE_TERMINATE_CALLBACK	terminate,
	0					// next-task
};


#include "command_controller.h"
static MCU_TASK_INTERFACE cmd_mcu_task = {

	0, 					// u8 identifier,
	5, 					// const u16 SCHEDULE_INTERVAL,
	15,					// const ux16 WORST_CASE_EXECUTION_TIME;
	0, 					// u16 new_run_timeout,
	0, 					// u16 last_run_time,
	&command_controller_init, 		// MCU_TASK_INTERFACE_INIT_CALLBACK		init,
	&command_controller_task_get_state, 	// CU_TASK_INTERFACE_IS_RUNABLE_CALLBACK	is_runable,
	&command_controller_handle_command, 	// MCU_TASK_INTERFACE_RUN_CALLBACK		run,
	&command_controller_background_run,	// MCU_TASK_INTERFACE_BG_RUN_CALLBACK		background_run,
	0, 					// MCU_TASK_INTERFACE_SLEEP_CALLBACK		sleep,
	0, 					// MCU_TASK_INTERFACE_WAKEUP_CALLBACK		wakeup,
	0, 					// MCU_TASK_INTERFACE_FINISH_CALLBACK		finish,
	0, 					// MCU_TASK_INTERFACE_TERMINATE_CALLBACK	terminate,
	0					// next-task
};

#include "io_input_controller.h"
static MCU_TASK_INTERFACE io_input_controller_task = {

	0, 					// u8 identifier,
	25, 					// const u16 SCHEDULE_INTERVAL,
	15,					// const ux16 WORST_CASE_EXECUTION_TIME;
	0, 					// u16 new_run_timeout,
	0, 					// u16 last_run_time,
	&io_input_controller_task_init, 		// MCU_TASK_INTERFACE_INIT_CALLBACK	init,
	&io_input_controller_task_get_state, 	// CU_TASK_INTERFACE_IS_RUNABLE_CALLBACK	is_runable,
	&io_input_controller_task_run, 		// MCU_TASK_INTERFACE_RUN_CALLBACK		run,
	&io_input_controller_task_background_run,// MCU_TASK_INTERFACE_BG_RUN_CALLBACK		background_run,
	0, 					// MCU_TASK_INTERFACE_SLEEP_CALLBACK		sleep,
	0, 					// MCU_TASK_INTERFACE_WAKEUP_CALLBACK		wakeup,
	0, 					// MCU_TASK_INTERFACE_FINISH_CALLBACK		finish,
	0, 					// MCU_TASK_INTERFACE_TERMINATE_CALLBACK	terminate,
	0					// next-task
};

#include "io_output_controller.h"
static MCU_TASK_INTERFACE io_output_controller_task = {

	0, 						// u8 identifier,
	5, 						// const u16 SCHEDULE_INTERVAL,
	15,						// const ux16 WORST_CASE_EXECUTION_TIME;
	0, 						// u16 new_run_timeout,
	0, 						// u16 last_run_time,
	&io_output_controller_task_init, 		// MCU_TASK_INTERFACE_INIT_CALLBACK		init,
	&io_output_controller_task_get_state, 		// CU_TASK_INTERFACE_IS_RUNABLE_CALLBACK	is_runable,
	&io_output_controller_task_run, 		// MCU_TASK_INTERFACE_RUN_CALLBACK		run,
	&io_output_controller_task_background_run,	// MCU_TASK_INTERFACE_BG_RUN_CALLBACK		background_run,
	0, 						// MCU_TASK_INTERFACE_SLEEP_CALLBACK		sleep,
	0, 						// MCU_TASK_INTERFACE_WAKEUP_CALLBACK		wakeup,
	0, 						// MCU_TASK_INTERFACE_FINISH_CALLBACK		finish,
	0, 						// MCU_TASK_INTERFACE_TERMINATE_CALLBACK	terminate,
	0						// next-task
};

#include "rpi_protocol_handler.h"
static MCU_TASK_INTERFACE rpi_protocol_task = {

	0, 						// u8 identifier,
	5, 						// const u16 SCHEDULE_INTERVAL,
	15,						// const ux16 WORST_CASE_EXECUTION_TIME;
	0, 						// u16 new_run_timeout,
	0, 						// u16 last_run_time,
	&rpi_protocol_task_init, 			// MCU_TASK_INTERFACE_INIT_CALLBACK		init,
	&rpi_protocol_task_get_state, 			// CU_TASK_INTERFACE_IS_RUNABLE_CALLBACK	is_runable,
	&rpi_protocol_task_run, 			// MCU_TASK_INTERFACE_RUN_CALLBACK		run,
	0,						// MCU_TASK_INTERFACE_BG_RUN_CALLBACK		background_run,
	0, 						// MCU_TASK_INTERFACE_SLEEP_CALLBACK		sleep,
	0, 						// MCU_TASK_INTERFACE_WAKEUP_CALLBACK		wakeup,
	0, 						// MCU_TASK_INTERFACE_FINISH_CALLBACK		finish,
	0, 						// MCU_TASK_INTERFACE_TERMINATE_CALLBACK	terminate,
	0						// next-task
};

#include "local_event_task.h"
static MCU_TASK_INTERFACE event_task = {

	0, 						// u8 identifier,
	25, 						// const u16 SCHEDULE_INTERVAL,
	15,						// const ux16 WORST_CASE_EXECUTION_TIME;
	0, 						// u16 new_run_timeout,
	0, 						// u16 last_run_time,
	&local_event_mcu_task_init, 			// MCU_TASK_INTERFACE_INIT_CALLBACK		init,
	&local_event_mcu_task_get_state, 		// CU_TASK_INTERFACE_IS_RUNABLE_CALLBACK	is_runable,
	&local_event_mcu_task_run, 			// MCU_TASK_INTERFACE_RUN_CALLBACK		run,
	&local_event_mcu_task_background_run,		// MCU_TASK_INTERFACE_BG_RUN_CALLBACK		background_run,
	&local_event_mcu_task_sleep, 			// MCU_TASK_INTERFACE_SLEEP_CALLBACK		sleep,
	&local_event_mcu_task_wakeup, 			// MCU_TASK_INTERFACE_WAKEUP_CALLBACK		wakeup,
	&local_event_mcu_task_finish, 			// MCU_TASK_INTERFACE_FINISH_CALLBACK		finish,
	&local_event_mcu_task_terminate, 		// MCU_TASK_INTERFACE_TERMINATE_CALLBACK	terminate,
	0						// next-task
};

#include "local_debus_mcu_task.h"
static MCU_TASK_INTERFACE debus_task = {

	0, 						// u8 identifier,
	25, 						// const u16 SCHEDULE_INTERVAL,
	15,						// const ux16 WORST_CASE_EXECUTION_TIME;
	0, 						// u16 new_run_timeout,
	0, 						// u16 last_run_time,
	&debus_task_init, 				// MCU_TASK_INTERFACE_INIT_CALLBACK		init,
	&debus_task_get_state, 				// CU_TASK_INTERFACE_IS_RUNABLE_CALLBACK	is_runable,
	&debus_task_run, 				// MCU_TASK_INTERFACE_RUN_CALLBACK		run,
	&debus_task_background_run,			// MCU_TASK_INTERFACE_BG_RUN_CALLBACK		background_run,
	0, 						// MCU_TASK_INTERFACE_SLEEP_CALLBACK		sleep,
	0, 						// MCU_TASK_INTERFACE_WAKEUP_CALLBACK		wakeup,
	0, 						// MCU_TASK_INTERFACE_FINISH_CALLBACK		finish,
	0, 						// MCU_TASK_INTERFACE_TERMINATE_CALLBACK	terminate,
	0						// next-task
};

#if defined (HAS_APP_TASK_TEST_TRACER) && (HAS_APP_TASK_TEST_TRACER) == 1

#include "test_tracer_mcu_task.h"
static MCU_TASK_INTERFACE test_tracer_task = {

	0, 						// u8 identifier,
	1000, 						// const u16 SCHEDULE_INTERVAL,
	15,						// const ux16 WORST_CASE_EXECUTION_TIME;
	0, 						// u16 new_run_timeout,
	0, 						// u16 last_run_time,
	&test_tracer_task_init, 				// MCU_TASK_INTERFACE_INIT_CALLBACK		init,
	&test_tracer_task_get_state, 				// CU_TASK_INTERFACE_IS_RUNABLE_CALLBACK	is_runable,
	&test_tracer_task_run, 				// MCU_TASK_INTERFACE_RUN_CALLBACK		run,
	&test_tracer_task_background_run,			// MCU_TASK_INTERFACE_BG_RUN_CALLBACK		background_run,
	0, 						// MCU_TASK_INTERFACE_SLEEP_CALLBACK		sleep,
	0, 						// MCU_TASK_INTERFACE_WAKEUP_CALLBACK		wakeup,
	0, 						// MCU_TASK_INTERFACE_FINISH_CALLBACK		finish,
	0, 						// MCU_TASK_INTERFACE_TERMINATE_CALLBACK	terminate,
	0						// next-task
};
#endif

void task_initialization(void) {

	PASS(); // task_initialization()

	mcu_task_controller_init();

	mcu_task_controller_register_task(&io_input_controller_task);
	mcu_task_controller_register_task(&rpi_protocol_task);
	mcu_task_controller_register_task(&sht31_mcu_task);
	mcu_task_controller_register_task(&ads1115_mcu_task);
	mcu_task_controller_register_task(&cmd_mcu_task);
	//mcu_task_controller_register_task(&debus_task);


	#if defined (HAS_APP_TASK_TEST_TRACER) && (HAS_APP_TASK_TEST_TRACER) == 1
	mcu_task_controller_register_task(&test_tracer_task);
	#endif

	#if config_HAS_LED_MATRIX == 1
	mcu_task_controller_register_task(&led_mcu_task);
	#endif


	mcu_task_controller_register_task(&event_task);
	mcu_task_controller_register_task(&io_output_controller_task);
}


