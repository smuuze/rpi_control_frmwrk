/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "utils/stdmacros.h"

#include "system_interface.h"

#include "mcu_task_interface.h"
#include "mcu_task_controller.h"

#define noTRACES
#include <traces.h>

#include "mcu_idle_task.h"
static MCU_TASK_INTERFACE mcu_idle_task = {

	0, 					// u8 identifier,
	0, 					// const u16 SCHEDULE_INTERVAL,
	5,					// wors-case-execution-time
	0, 					// u16 new_run_timeout,
	0, 					// u16 last_run_time,
	&mcu_idle_task_init, 			// MCU_TASK_INTERFACE_INIT_CALLBACK		init,
	&mcu_idle_task_is_runable, 		// CU_TASK_INTERFACE_IS_RUNABLE_CALLBACK	is_runable,
	&mcu_idle_task_run, 			// MCU_TASK_INTERFACE_RUN_CALLBACK		run,
	&mcu_idle_task_background_run,		// MCU_TASK_INTERFACE_BG_RUN_CALLBACK		background_run,
	&mcu_idle_task_sleep, 			// MCU_TASK_INTERFACE_SLEEP_CALLBACK		sleep,
	&mcu_idle_task_wakeup, 			// MCU_TASK_INTERFACE_WAKEUP_CALLBACK		wakeup,
	&mcu_idle_task_finish, 			// MCU_TASK_INTERFACE_FINISH_CALLBACK		finish,
	&mcu_idle_task_terminate, 		// MCU_TASK_INTERFACE_TERMINATE_CALLBACK	terminate,
	0					// next-task
};

/*!
 *
 */
static MCU_TASK_INTERFACE* _first_task = 0;

/*!
 *
 */
static MCU_TASK_INTERFACE* _last_task = 0;

/*!
 *
 */
static u8 _has_task_interval_passed(MCU_TASK_INTERFACE* p_task);

/*!
 *
 */
static void _update_last_run_time(MCU_TASK_INTERFACE* p_task);

/*!
 *
 */
static u32 _minimum_taks_schedule_interval = 0xFFFFFFFF;


void mcu_task_controller_init(void) {

	_first_task = 0;
	_last_task = 0;
}

void mcu_task_controller_register_task(MCU_TASK_INTERFACE* p_mcu_task) {

	static u8 new_task_id = 1;

	p_mcu_task->next_task = 0;
	p_mcu_task->identifier = new_task_id;
	new_task_id += 1;

	p_mcu_task->init();

	p_mcu_task->last_run_time = i_system.time.now_u16();
	p_mcu_task->new_run_timeout = p_mcu_task->SCHEDULE_INTERVAL;

	if (p_mcu_task->SCHEDULE_INTERVAL < _minimum_taks_schedule_interval) {
		_minimum_taks_schedule_interval = p_mcu_task->SCHEDULE_INTERVAL;
	}

	if (_first_task == 0) {
		_first_task = p_mcu_task;
		_last_task = p_mcu_task;

	} else {
		_last_task->next_task = p_mcu_task;
		_last_task = p_mcu_task;
	}

	TRACE_byte(_last_task->identifier); // mcu_task_controller_register_task() - new task added
}

void mcu_task_controller_schedule(void) {

	MCU_TASK_INTERFACE* act_task = _first_task;

	while (act_task != 0) {

		if (_has_task_interval_passed(act_task) == 0) {
			goto SKIP_TASK;
		}

		_update_last_run_time(act_task);

		if (act_task->is_runable() == 0) {
			TRACE_byte(act_task->identifier); // mcu_task_controller_schedule() - Task is not runnable xxxxxxxxxxxxxxxx
			goto SKIP_TASK;
		}

		TRACE_byte(act_task->identifier); // mcu_task_controller_schedule() - Running Task >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

		//act_task->last_run_time = i_system.time.now_u16();
		act_task->run();

		PASS(); // mcu_task_controller_schedule() - Task complete <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

		SKIP_TASK :
		act_task = act_task->next_task;
	}

	//mcu_idle_task.run();
}

void mcu_task_controller_sleep(void) {

	MCU_TASK_INTERFACE* act_task = _first_task;

	while (act_task != 0) {

		if (act_task->sleep == 0) {
			goto SKIP_TASK;
		}

		act_task->sleep();

		SKIP_TASK :
		act_task = act_task->next_task;
	}
}

void mcu_task_controller_wakeup(void) {

	MCU_TASK_INTERFACE* act_task = _first_task;

	while (act_task != 0) {

		if (act_task->wakeup == 0) {
			goto SKIP_TASK;
		}

		act_task->wakeup();

		SKIP_TASK :
		act_task = act_task->next_task;
	}
}

void mcu_task_controller_background_run(void) {

	MCU_TASK_INTERFACE* act_task = _first_task;

	while (act_task != 0) {

		if (act_task->background_run == 0) {
			goto SKIP_TASK;
		}

		act_task->background_run();

		SKIP_TASK :
		act_task = act_task->next_task;
	}
}


static inline u8 _has_task_interval_passed(MCU_TASK_INTERFACE* p_task) {

	if (p_task->SCHEDULE_INTERVAL == MCU_TASK_SCHEDULE_NO_TIMEOUT) {
		return 1;
	}

	if (i_system.time.isup_u16(p_task->last_run_time, p_task->new_run_timeout) == 0) {
		return 0;
	}

	return 1;
}

static inline void _update_last_run_time(MCU_TASK_INTERFACE* p_task) {

	if (p_task->SCHEDULE_INTERVAL == MCU_TASK_SCHEDULE_NO_TIMEOUT) {
		return;
	}

	PASS(); // _update_last_run_time() ------------------------------------------------------------------------------------------

	u16 actual_time_ms = i_system.time.now_u16();
//	p_task->new_run_timeout = p_task->SCHEDULE_INTERVAL;
//	u16 jitter = actual_time_ms - p_task->last_run_time;
//
//	while (jitter > p_task->SCHEDULE_INTERVAL) {
//		jitter -= p_task->SCHEDULE_INTERVAL;
//	}
//
//	TRACE_word(p_task->last_run_time); //
//	TRACE_word(actual_time_ms); //
//	TRACE_word(p_task->SCHEDULE_INTERVAL); //
//	TRACE_word(jitter); //
//
//	u16 next_scheduled_run	= p_task->last_run_time - jitter;
//	while (next_scheduled_run < actual_time_ms) {
//		next_scheduled_run += p_task->SCHEDULE_INTERVAL;
//	}

	p_task->new_run_timeout = p_task->SCHEDULE_INTERVAL;// - jitter;
	p_task->last_run_time	= actual_time_ms;

//	TRACE_word(p_task->new_run_timeout); //
//	TRACE_word(next_scheduled_run); //
}
