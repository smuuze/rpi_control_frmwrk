/*! \file *********************************************************************

 *****************************************************************************/

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "tracer.h"

//-----------------------------------------------------------------------------

#include "mcu_task_controller.h"
#include "mcu_task_interface.h"

//-----------------------------------------------------------------------------

#ifndef config_HAS_LED_MATRIX
#define config_HAS_LED_MATRIX 0
#endif

//-----------------------------------------------------------------------------

#if config_HAS_LED_MATRIX == 1
#include "local_led_mcu_task.h"
static MCU_TASK_INTERFACE led_mcu_task = {

	0, 						// u8 identifier,
	0, 						// u16 new_run_timeout,
	0, 						// u16 last_run_time,
	&local_led_mcu_task_init, 			// MCU_TASK_INTERFACE_INIT_CALLBACK		init,
	&local_led_mcu_task_is_runable, 		// MCU_TASK_INTERFACE_GET_STATE_CALLBACK	get_sate,
	&local_led_mcu_task_run, 			// MCU_TASK_INTERFACE_RUN_CALLBACK		run,
	0,						// MCU_TASK_INTERFACE_BG_RUN_CALLBACK		background_run,
	0, 						// MCU_TASK_INTERFACE_SLEEP_CALLBACK		sleep,
	0, 						// MCU_TASK_INTERFACE_WAKEUP_CALLBACK		wakeup,
	&local_led_mcu_task_finish, 			// MCU_TASK_INTERFACE_FINISH_CALLBACK		finish,
	&local_led_mcu_task_terminate, 			// MCU_TASK_INTERFACE_TERMINATE_CALLBACK	terminate,
	0						// next-task
};
#endif

//-----------------------------------------------------------------------------

#ifdef HAS_EXPANSION_BOARD_SENSOR_SHT31_ADS1115
#include "local_ads1115_mcu_task.h"
static MCU_TASK_INTERFACE ads1115_mcu_task = {

	0, 						// u8 identifier,
	0, 						// u16 new_run_timeout,
	0, 						// u16 last_run_time,
	&local_ads1115_mcu_task_init, 			// MCU_TASK_INTERFACE_INIT_CALLBACK			init,
	&local_ads1115_mcu_task_get_schedule_interval, 	// MCU_TASK_INTERFACE_GET_SCHEDULE_INTERVAL_CALLBACK	get_schedule_interval,
	&local_ads1115_mcu_task_get_state, 		// MCU_TASK_INTERFACE_GET_STATE_CALLBACK		get_sate,
	&local_ads1115_mcu_task_run, 			// MCU_TASK_INTERFACE_RUN_CALLBACK			run,
	0,						// MCU_TASK_INTERFACE_BG_RUN_CALLBACK			background_run,
	0, 						// MCU_TASK_INTERFACE_SLEEP_CALLBACK			sleep,
	0, 						// MCU_TASK_INTERFACE_WAKEUP_CALLBACK			wakeup,
	&local_ads1115_mcu_task_finish, 		// MCU_TASK_INTERFACE_FINISH_CALLBACK			finish,
	&local_ads1115_mcu_task_terminate, 		// MCU_TASK_INTERFACE_TERMINATE_CALLBACK		terminate,
	0						// next-task
};

//-----------------------------------------------------------------------------

#include "local_sht31_mcu_task.h"
static MCU_TASK_INTERFACE sht31_mcu_task = {

	0, 						// u8 identifier,
	0, 						// u16 new_run_timeout,
	0, 						// u16 last_run_time,
	&local_sht31_mcu_task_init, 			// MCU_TASK_INTERFACE_INIT_CALLBACK			init,
	&local_sht31_mcu_task_get_schedule_interval,	// MCU_TASK_INTERFACE_INIT_CALLBACK			get_schedule_interval,
	&local_sht31_mcu_task_get_state, 		// MCU_TASK_INTERFACE_GET_STATE_CALLBACK		get_sate,
	&local_sht31_mcu_task_run, 			// MCU_TASK_INTERFACE_RUN_CALLBACK			run,
	0,						// MCU_TASK_INTERFACE_BG_RUN_CALLBACK			background_run,
	0, 						// MCU_TASK_INTERFACE_SLEEP_CALLBACK			sleep,
	0, 						// MCU_TASK_INTERFACE_WAKEUP_CALLBACK			wakeup,
	&local_sht31_mcu_task_finish, 			// MCU_TASK_INTERFACE_FINISH_CALLBACK			finish,
	&local_sht31_mcu_task_terminate, 		// MCU_TASK_INTERFACE_TERMINATE_CALLBACK		terminate,
	0						// next-task
};
#endif

//-----------------------------------------------------------------------------

#ifdef HAS_MANAGEMENT_MODULE_IO
#include "io_management/io_input_controller.h"
static MCU_TASK_INTERFACE io_input_controller_task = {

	0, 						// u8 identifier,
	0, 						// u16 new_run_timeout,
	0, 						// u16 last_run_time,
	&io_input_controller_task_init, 		// MCU_TASK_INTERFACE_INIT_CALLBACK			init,
	&io_input_controller_task_get_schedule_interval,// MCU_TASK_INTERFACE_INIT_CALLBACK			get_schedule_interval,
	&io_input_controller_task_get_state, 		// MCU_TASK_INTERFACE_GET_STATE_CALLBACK		get_sate,
	&io_input_controller_task_run, 			// MCU_TASK_INTERFACE_RUN_CALLBACK			run,
	&io_input_controller_task_background_run,	// MCU_TASK_INTERFACE_BG_RUN_CALLBACK			background_run,
	0, 						// MCU_TASK_INTERFACE_SLEEP_CALLBACK			sleep,
	0, 						// MCU_TASK_INTERFACE_WAKEUP_CALLBACK			wakeup,
	0, 						// MCU_TASK_INTERFACE_FINISH_CALLBACK			finish,
	0, 						// MCU_TASK_INTERFACE_TERMINATE_CALLBACK		terminate,
	0						// next-task
};

#include "io_management/io_output_controller.h"
static MCU_TASK_INTERFACE io_output_controller_task = {

	0, 						// u8 identifier,
	0, 						// u16 new_run_timeout,
	0, 						// u16 last_run_time,
	&io_output_controller_task_init, 		// MCU_TASK_INTERFACE_INIT_CALLBACK			init,
	&io_output_controller_task_get_schedule_interval,// MCU_TASK_INTERFACE_INIT_CALLBACK			get_schedule_interval,
	&io_output_controller_task_get_state, 		// MCU_TASK_INTERFACE_GET_STATE_CALLBACK		get_sate,
	&io_output_controller_task_run, 		// MCU_TASK_INTERFACE_RUN_CALLBACK			run,
	&io_output_controller_task_background_run,	// MCU_TASK_INTERFACE_BG_RUN_CALLBACK			background_run,
	0, 						// MCU_TASK_INTERFACE_SLEEP_CALLBACK			sleep,
	0, 						// MCU_TASK_INTERFACE_WAKEUP_CALLBACK			wakeup,
	0, 						// MCU_TASK_INTERFACE_FINISH_CALLBACK			finish,
	0, 						// MCU_TASK_INTERFACE_TERMINATE_CALLBACK		terminate,
	0						// next-task
};
#endif

//-----------------------------------------------------------------------------

#ifdef HAS_MANAGEMENT_MODULE_RPI_PROTOCOL
#include "protocol_management/rpi_protocol_handler.h"
static MCU_TASK_INTERFACE rpi_protocol_task = {

	0, 						// u8 identifier,
	0, 						// u16 new_run_timeout,
	0, 						// u16 last_run_time,
	&rpi_protocol_task_init, 			// MCU_TASK_INTERFACE_INIT_CALLBACK			init,
	&rpi_protocol_task_get_schedule_interval,	// MCU_TASK_INTERFACE_INIT_CALLBACK			get_schedule_interval,
	&rpi_protocol_task_get_state, 			// MCU_TASK_INTERFACE_GET_STATE_CALLBACK		get_sate,
	&rpi_protocol_task_run, 			// MCU_TASK_INTERFACE_RUN_CALLBACK			run,
	0,						// MCU_TASK_INTERFACE_BG_RUN_CALLBACK			background_run,
	0, 						// MCU_TASK_INTERFACE_SLEEP_CALLBACK			sleep,
	0, 						// MCU_TASK_INTERFACE_WAKEUP_CALLBACK			wakeup,
	0, 						// MCU_TASK_INTERFACE_FINISH_CALLBACK			finish,
	0, 						// MCU_TASK_INTERFACE_TERMINATE_CALLBACK		terminate,
	0						// next-task
};
#endif

//-----------------------------------------------------------------------------

#ifdef HAS_APP_TASK_EVENT
#include "local_event_task.h"
static MCU_TASK_INTERFACE event_task = {

	0, 						// u8 identifier,
	0, 						// u16 new_run_timeout,
	0, 						// u16 last_run_time,
	&local_event_mcu_task_init, 			// MCU_TASK_INTERFACE_INIT_CALLBACK			init,
	&local_event_mcu_task_get_schedule_interval,	// MCU_TASK_INTERFACE_INIT_CALLBACK			get_schedule_interval,
	&local_event_mcu_task_get_state, 		// MCU_TASK_INTERFACE_GET_STATE_CALLBACK		get_sate,
	&local_event_mcu_task_run, 			// MCU_TASK_INTERFACE_RUN_CALLBACK			run,
	&local_event_mcu_task_background_run,		// MCU_TASK_INTERFACE_BG_RUN_CALLBACK			background_run,
	&local_event_mcu_task_sleep, 			// MCU_TASK_INTERFACE_SLEEP_CALLBACK			sleep,
	&local_event_mcu_task_wakeup, 			// MCU_TASK_INTERFACE_WAKEUP_CALLBACK			wakeup,
	&local_event_mcu_task_finish, 			// MCU_TASK_INTERFACE_FINISH_CALLBACK			finish,
	&local_event_mcu_task_terminate, 		// MCU_TASK_INTERFACE_TERMINATE_CALLBACK		terminate,
	0						// next-task
};
#endif

//-----------------------------------------------------------------------------

#if defined (HAS_APP_TASK_TEST_TRACER) && (HAS_APP_TASK_TEST_TRACER) == 1
#include "test_tracer_mcu_task.h"
static MCU_TASK_INTERFACE test_tracer_task = {

	0, 						// u8 identifier,
	0, 						// u16 new_run_timeout,
	0, 						// u16 last_run_time,
	&test_tracer_task_init, 			// MCU_TASK_INTERFACE_INIT_CALLBACK			init,
	&test_tracer_task_get_schedule_interval,	// MCU_TASK_INTERFACE_INIT_CALLBACK			get_schedule_interval,
	&test_tracer_task_get_state, 			// MCU_TASK_INTERFACE_GET_STATE_CALLBACK		get_sate,
	&test_tracer_task_run, 				// MCU_TASK_INTERFACE_RUN_CALLBACK			run,
	&test_tracer_task_background_run,		// MCU_TASK_INTERFACE_BG_RUN_CALLBACK			background_run,
	0, 						// MCU_TASK_INTERFACE_SLEEP_CALLBACK			sleep,
	0, 						// MCU_TASK_INTERFACE_WAKEUP_CALLBACK			wakeup,
	0, 						// MCU_TASK_INTERFACE_FINISH_CALLBACK			finish,
	0, 						// MCU_TASK_INTERFACE_TERMINATE_CALLBACK		terminate,
	0						// next-task
};
#endif

//-----------------------------------------------------------------------------

#ifdef HAS_EXPANSION_BOARD_GPIO_PCA9670
#include "driver_PCA9670.h"
__UNUSED__ static MCU_TASK_INTERFACE pca9670_task = {

	0, 						// u8 identifier,
	0, 						// u16 new_run_timeout,
	0, 						// u16 last_run_time,
	&pca9670_task_init, 				// MCU_TASK_INTERFACE_INIT_CALLBACK			init,
	&pca9670_task_get_schedule_interval,		// MCU_TASK_INTERFACE_INIT_CALLBACK			get_schedule_interval,
	&pca9670_task_get_state, 			// MCU_TASK_INTERFACE_GET_STATE_CALLBACK		get_sate,
	&pca9670_task_run, 				// MCU_TASK_INTERFACE_RUN_CALLBACK			run,
	&pca9670_task_background_run,			// MCU_TASK_INTERFACE_BG_RUN_CALLBACK			background_run,
	0, 						// MCU_TASK_INTERFACE_SLEEP_CALLBACK			sleep,
	0, 						// MCU_TASK_INTERFACE_WAKEUP_CALLBACK			wakeup,
	0, 						// MCU_TASK_INTERFACE_FINISH_CALLBACK			finish,
	0, 						// MCU_TASK_INTERFACE_TERMINATE_CALLBACK		terminate,
	0						// next-task
};
#endif

//-----------------------------------------------------------------------------

#if defined (HAS_APP_TASK_IR_REMOTE) && (HAS_APP_TASK_IR_REMOTE) == 1
#include "app_tasks/ir_remote_mcu_task.h"
static MCU_TASK_INTERFACE ir_remote_task = {

	0, 						// u8 identifier,
	0, 						// u16 new_run_timeout,
	0, 						// u16 last_run_time,
	&ir_remote_task_init, 				// MCU_TASK_INTERFACE_INIT_CALLBACK			init,
	&ir_remote_task_get_schedule_interval,		// MCU_TASK_INTERFACE_INIT_CALLBACK			get_schedule_interval,
	&ir_remote_task_get_state, 			// MCU_TASK_INTERFACE_GET_STATE_CALLBACK		get_sate,
	&ir_remote_task_run, 				// MCU_TASK_INTERFACE_RUN_CALLBACK			run,
	&ir_remote_task_background_run,			// MCU_TASK_INTERFACE_BG_RUN_CALLBACK			background_run,
	0, 						// MCU_TASK_INTERFACE_SLEEP_CALLBACK			sleep,
	0, 						// MCU_TASK_INTERFACE_WAKEUP_CALLBACK			wakeup,
	0, 						// MCU_TASK_INTERFACE_FINISH_CALLBACK			finish,
	0, 						// MCU_TASK_INTERFACE_TERMINATE_CALLBACK		terminate,
	0						// next-task
};
#endif

//-----------------------------------------------------------------------------

#if defined (HAS_APP_TASK_COPRO_ROUTING) && (HAS_APP_TASK_COPRO_ROUTING) == 1
#include "app_tasks/copro_routing_mcu_task.h"
static MCU_TASK_INTERFACE copro_routing_task = {

	0, 						// u8 identifier,
	0, 						// u16 new_run_timeout,
	0, 						// u16 last_run_time,
	&copro_routing_task_init, 			// MCU_TASK_INTERFACE_INIT_CALLBACK			init,
	&copro_routing_task_get_schedule_interval,	// MCU_TASK_INTERFACE_INIT_CALLBACK			get_schedule_interval,
	&copro_routing_task_get_state, 			// MCU_TASK_INTERFACE_GET_STATE_CALLBACK		get_sate,
	&copro_routing_task_run, 			// MCU_TASK_INTERFACE_RUN_CALLBACK			run,
	&copro_routing_task_background_run,		// MCU_TASK_INTERFACE_BG_RUN_CALLBACK			background_run,
	0, 						// MCU_TASK_INTERFACE_SLEEP_CALLBACK			sleep,
	0, 						// MCU_TASK_INTERFACE_WAKEUP_CALLBACK			wakeup,
	0, 						// MCU_TASK_INTERFACE_FINISH_CALLBACK			finish,
	0, 						// MCU_TASK_INTERFACE_TERMINATE_CALLBACK		terminate,
	0						// next-task
};
#endif

//-----------------------------------------------------------------------------

void task_initialization(void) {

	DEBUG_PASS("task_initialization()");

	mcu_task_controller_init();

	#ifdef HAS_MANAGEMENT_MODULE_IO
	DEBUG_PASS("task_initialization() - IO input-controller task");
	mcu_task_controller_register_task(&io_input_controller_task);
	#endif

	#ifdef HAS_MANAGEMENT_MODULE_RPI_PROTOCOL
	DEBUG_PASS("task_initialization() - rpi-protocol task");
	mcu_task_controller_register_task(&rpi_protocol_task);
	#endif

	#ifdef HAS_EXPANSION_BOARD_SENSOR_SHT31_ADS1115

	DEBUG_PASS("task_initialization() - SHT31 (Temperature / Humidity) task");
	mcu_task_controller_register_task(&sht31_mcu_task);

	DEBUG_PASS("task_initialization() - ADS1115 (ADC) task");
	mcu_task_controller_register_task(&ads1115_mcu_task);
	#endif

	#if defined (HAS_APP_TASK_TEST_TRACER) && (HAS_APP_TASK_TEST_TRACER) == 1
	mcu_task_controller_register_task(&test_tracer_task);
	#endif

	#if defined (HAS_APP_TASK_IR_REMOTE) && (HAS_APP_TASK_IR_REMOTE) == 1
	mcu_task_controller_register_task(&ir_remote_task);
	#endif

	#if config_HAS_LED_MATRIX == 1
	mcu_task_controller_register_task(&led_mcu_task);
	#endif

	#ifdef HAS_EXPANSION_BOARD_GPIO_PCA9670
	//-- task is currupted mcu_task_controller_register_task(&pca9670_task);
	#endif

	#ifdef HAS_APP_TASK_EVENT
	mcu_task_controller_register_task(&event_task);
	#endif

	#ifdef HAS_MANAGEMENT_MODULE_IO
	DEBUG_PASS("task_initialization() - IO output-controller task");
	mcu_task_controller_register_task(&io_output_controller_task);
	#endif

	#if defined (HAS_APP_TASK_COPRO_ROUTING) && (HAS_APP_TASK_COPRO_ROUTING) == 1
	DEBUG_PASS("task_initialization() - copro-routing task");
	mcu_task_controller_register_task(&copro_routing_task);
	#endif
}


