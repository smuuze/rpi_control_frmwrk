#ifndef _MCU_TASK_INTERFACE_H_
#define _MCU_TASK_INTERFACE_H_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
typedef enum {
	MCU_TASK_UNINITIALIZED = 0xFF,
	MCU_TASK_SLEEPING = 0,  //!< MCU_TASK_SLEEPING
	MCU_TASK_IDLE = 1,      //!< MCU_TASK_IDLE
	MCU_TASK_RUNNING = 2,   //!< MCU_TASK_RUNNING
	MCU_TASK_TERMINATED = 3//!< MCU_TASK_TERMINATED
} MCU_TASK_INTERFACE_TASK_STATE;

// --------------------------------------------------------------------------------

typedef void (*MCU_TASK_INTERFACE_INIT_CALLBACK) (void);
typedef MCU_TASK_INTERFACE_TASK_STATE (*MCU_TASK_INTERFACE_GET_STATE_CALLBACK) (void);
typedef u16 (*MCU_TASK_INTERFACE_GET_SCHEDULE_INTERVAL_CALLBACK) (void);
typedef void (*MCU_TASK_INTERFACE_RUN_CALLBACK) (void);
typedef void (*MCU_TASK_INTERFACE_BG_RUN_CALLBACK) (void);
typedef void (*MCU_TASK_INTERFACE_SLEEP_CALLBACK) (void);
typedef void (*MCU_TASK_INTERFACE_WAKEUP_CALLBACK) (void);
typedef void (*MCU_TASK_INTERFACE_FINISH_CALLBACK) (void);
typedef void (*MCU_TASK_INTERFACE_TERMINATE_CALLBACK) (void);

// --------------------------------------------------------------------------------

/**
 * @brief structure to create a task that can be registered 
 * in the mcu-task-controller.
 * 
 */
typedef struct MCU_TASK_INTERFACE {

	/**
	 * @brief 
	 * 
	 */
	u8 identifier;

	/**
	 * @brief This variable is used by the task-controller
	 * to determine the next execution time of this task
	 * 
	 */
	u16 new_run_timeout;

	/**
	 * @brief This variable is used by the task-controller
	 * to determine the next execution time of this task
	 * 
	 */
	u16 last_run_time;
		
	/**
	 * @brief Callback to initialize a task.
	 * This function is called immediately after
	 * a task was registered to the task-controller.
	 * 
	 */
	MCU_TASK_INTERFACE_INIT_CALLBACK init;

	/**
	 * @brief Returns the interval in milliseconds at which the task needs to be executed.
	 * 
	 * @return number of milliseconds between two following task-executions.
	 * 
	 */
	MCU_TASK_INTERFACE_GET_SCHEDULE_INTERVAL_CALLBACK get_schedule_interval;

	/**
	 * @brief Returns the actual task-state. Via this function the task-controller
	 * decides if a task needs to be executed or not. If the actual state is
	 * MCU_TASK_SLEEPING the task will not be executed. This function is also used to
	 * determine if the system is in idle state, this means no task is running, and can
	 * enter a low-power state
	 * 
	 * @return
	 * 	MCU_TASK_SLEEPING 	The task is in sleep mode and will not be executed
	 * 				If this state is returned the system can enter the low-power state
	 * 	MCU_TASK_IDLE 		The task is in sleep mode and will not be executed
	 * 				If this state is returned the system can enter the low-power state
	 * 	MCU_TASK_RUNNING	The task is ready to be executed. 
	 * 				If this state is returned after the task was executed
	 * 				the system then cannot enter the idle-state.
	 * 	MCU_TASK_TERMINATED	The task is terminated and cannot executed anymore
	 * 				If this state is returned the system can enter the low-power state
	 * 
	 */
	MCU_TASK_INTERFACE_GET_STATE_CALLBACK get_sate;

	/**
	 * @brief Executes the task. This function is blocking.
	 * The task decides when it will give back cpu-time.
	 * A running task cannot be stopped. It can only be interrupted by an HW-IRQ
	 * 
	 */
	MCU_TASK_INTERFACE_RUN_CALLBACK run;

	/**
	 * @brief This function is optional for every task.
	 * A task can be run for a short time in the background.
	 * This function can be used to update internal status-flags and timers.
	 * A task is not allowed to execute complex opperations in this function
	 * 
	 */
	MCU_TASK_INTERFACE_BG_RUN_CALLBACK background_run;

	/**
	 * @brief This function is optional for every task
	 * If the system goes into low-power mode, this functions is called.
	 * A task may perform special opperations to enter its low-power mode
	 * E.g. a task needs to set the GPIOs into a low-power state.
	 * 
	 */
	MCU_TASK_INTERFACE_SLEEP_CALLBACK sleep;

	/**
	 * @brief This function is optional for every task.
	 * If the system is un low-power mode and returns to normal operation
	 * this function is called.
	 * 
	 */
	MCU_TASK_INTERFACE_WAKEUP_CALLBACK wakeup;

	/**
	 * @brief 
	 * 
	 */
	MCU_TASK_INTERFACE_FINISH_CALLBACK finish;

	/**
	 * @brief This function is optional for every task.
	 * If this function is called a task must clean
	 * up itself and give back all allocated resources.
	 * It then has to enter the MCU_TASK_TERMINATED state.
	 * The task is not allowed to leave this state.
	 * 
	 */
	MCU_TASK_INTERFACE_TERMINATE_CALLBACK terminate;

	/**
	 * @brief pointer to the next task in the list.
	 * 
	 */
	struct MCU_TASK_INTERFACE* next_task;

} MCU_TASK_INTERFACE;

// --------------------------------------------------------------------------------

#define MCU_TASK_SCHEDULE_NO_TIMEOUT		0

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param p_mcu_task 
 */
void mcu_task_controller_register_task(MCU_TASK_INTERFACE* p_mcu_task);

/**
 * @brief 
 * 
 */
void mcu_task_controller_schedule(void);

// --------------------------------------------------------------------------------

#endif // _MCU_TASK_INTERFACE_H_
