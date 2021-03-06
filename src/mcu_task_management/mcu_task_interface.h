#ifndef _MCU_TASK_INTERFACE_H_
#define _MCU_TASK_INTERFACE_H_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

/*!
 *
 */
typedef enum {
	MCU_TASK_UNINITIALIZED = 0xFF,
	MCU_TASK_SLEEPING = 0,  //!< MCU_TASK_SLEEPING
	MCU_TASK_IDLE = 1,      //!< MCU_TASK_IDLE
	MCU_TASK_RUNNING = 2,   //!< MCU_TASK_RUNNING
	MCU_TASK_TERMINATED = 3,//!< MCU_TASK_TERMINATED
} MCU_TASK_INTERFACE_TASK_STATE;

// --------------------------------------------------------------------------------

/*!
 *
 */
typedef void (*MCU_TASK_INTERFACE_INIT_CALLBACK)	(void);

/*!
 *
 * @return
 */
typedef MCU_TASK_INTERFACE_TASK_STATE (*MCU_TASK_INTERFACE_GET_STATE_CALLBACK) 	(void);

/*!
 *
 */
typedef u16 (*MCU_TASK_INTERFACE_GET_SCHEDULE_INTERVAL_CALLBACK)			(void);

/*!
 *
 */
typedef void (*MCU_TASK_INTERFACE_RUN_CALLBACK)		(void);

/*!
 *
 */
typedef void (*MCU_TASK_INTERFACE_BG_RUN_CALLBACK)	(void);

/*!
 *
 */
typedef void (*MCU_TASK_INTERFACE_SLEEP_CALLBACK)	(void);

/*!
 *
 */
typedef void (*MCU_TASK_INTERFACE_WAKEUP_CALLBACK)	(void);

/*!
 *
 */
typedef void (*MCU_TASK_INTERFACE_FINISH_CALLBACK)	(void);

/*!
 *
 */
typedef void (*MCU_TASK_INTERFACE_TERMINATE_CALLBACK)	(void);

// --------------------------------------------------------------------------------

/*!
 *
 */
typedef struct MCU_TASK_INTERFACE {

	u8 identifier;

	u16 new_run_timeout;
	u16 last_run_time;

	MCU_TASK_INTERFACE_INIT_CALLBACK			init;
	MCU_TASK_INTERFACE_GET_SCHEDULE_INTERVAL_CALLBACK	get_schedule_interval;
	MCU_TASK_INTERFACE_GET_STATE_CALLBACK			get_sate;
	MCU_TASK_INTERFACE_RUN_CALLBACK				run;
	MCU_TASK_INTERFACE_BG_RUN_CALLBACK			background_run;
	MCU_TASK_INTERFACE_SLEEP_CALLBACK			sleep;
	MCU_TASK_INTERFACE_WAKEUP_CALLBACK			wakeup;
	MCU_TASK_INTERFACE_FINISH_CALLBACK			finish;
	MCU_TASK_INTERFACE_TERMINATE_CALLBACK			terminate;

	struct MCU_TASK_INTERFACE*				next_task;

} MCU_TASK_INTERFACE;

// --------------------------------------------------------------------------------

#define MCU_TASK_SCHEDULE_NO_TIMEOUT		0

// --------------------------------------------------------------------------------

/*!
 *
 */
void mcu_task_controller_register_task(MCU_TASK_INTERFACE* p_mcu_task);

/*!
 *
 */
void mcu_task_controller_schedule(void);

// --------------------------------------------------------------------------------

#endif // _MCU_TASK_INTERFACE_H_
