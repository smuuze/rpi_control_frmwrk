/**
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * @file    mcu_task_interface.h
 * @author  Sebastian Lesse
 * @date    2022 / 12 / 12
 * @brief   Interface to create task
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _MCU_TASK_INTERFACE_H_
#define _MCU_TASK_INTERFACE_H_

// --------------------------------------------------------------------------------

#include "config.h"
#include "cpu.h"

// --------------------------------------------------------------------------------

#include "iterator_interface.h"

// --------------------------------------------------------------------------------

/**
 * @brief Enumaration to activate / deaativate collecting task statistics on runtime.
 */
typedef enum {

    /**
     * @brief activate collecitng statistics of every task on runtime
     */
    TASK_CTRL_STATISTIC_OFF = 0,

    /**
     * @brief deactivate collecting statistics on runtime.
     */
    TASK_CTRL_STATISTIC_ON = 1
} TASK_CTRL_STATISTIC_EN;

// --------------------------------------------------------------------------------

typedef struct {
    u8 id;
    u32 runtime;
    u8 name_length;
    const char* p_name;
} TASK_CTRL_STATS;

// --------------------------------------------------------------------------------

/**
 * @brief Creates a new task.
 * 
 * Usage:
 * 
 *  1.  declare task callbacks. Implementation is made somewhere else
 * 
 *      static void my_task_start(void);
 *      static u16 my_task_get_schedule_interval(void);
 *      static MCU_TASK_INTERFACE_TASK_STATE my_task_get_state(void);
 *      static void my_task_run(void);
 *      static void my_task_terminate(void);
 * 
 *  2.  create task
 * 
 *      TASK_CREATE (
 *          MY_TASK,
 *          my_task_get_schedule_interval,
 *          my_task_start,
 *          my_task_run,
 *          my_task_get_state,
 *          my_task_terminate
 *      )
 * 
 *  3.  initialize task
 * 
 *      MY_TASK_init();
 * 
 * @param name Name of the new task 
 * @param priority priority at which the task will run a lower number 
 * @param interval interval at which the tas is scheduled
 * @param start task initialization callback, is executed if this task is initialized
 * @param run callback to schedule this task
 * @param state callback to get the current state of this task, see MCU_TASK_INTERFACE_TASK_STATE
 * @param terminate callback to terminate the task
 */
#define TASK_CREATE(name, priority, interval, start, run, state, terminate )        \
                                                                                    \
    static const char __##name##__task_name[] = #name;                              \
                                                                                    \
    static MCU_TASK_INTERFACE __##name##__task_context = {                          \
        0,                                                                          \
        0,                                                                          \
        0,                                                                          \
        0,                                                                          \
        __##name##__task_name,                                                      \
        sizeof(__##name##__task_name),                                              \
        &start,                                                                     \
        &interval,                                                                  \
        &state,                                                                     \
        &run,                                                                       \
        0,                                                                          \
        0,                                                                          \
        0,                                                                          \
        0,                                                                          \
        &terminate,                                                                 \
        0                                                                           \
    };                                                                              \
                                                                                    \
    __UNUSED__ static inline void name##_init(void) {                               \
        mcu_task_controller_register_task(&__##name##__task_context);               \
    }                                                                               \
                                                                                    \
    __UNUSED__ static inline void name##_run(void) {                                \
        run();                                                                      \
    }

// --------------------------------------------------------------------------------

/**
 * @brief Types of priority a task can have.
 */
typedef enum {

    /**
     * @brief The task has a very high priority and is scheduled befor all other
     * task with a lower priority. Shall only be used for system relevant task.
     */
    TASK_PRIORITY_VERY_HIGH,

    /**
     * @brief The task has a high priority.
     * Can be used for task that have a critical timing.
     */
    TASK_PRIORITY_HIGH,

    /**
     * @brief The task has a normal priority.
     * Cann be used for task that does not have a critical timing
     */
    TASK_PRIORITY_MIDDLE,

    /**
     * @brief The task has a low priority.
     * Can be used for tasks that are less important than task
     * with a normal priority.
     */
    TASK_PRIORITY_LOW,

    /**
     * @brief The task has a very low priority.
     * Can be used for tasks that are not time critical and are
     * scheduled in a very long interval.
     * This priority is also apllied to the systems idle task
     */
    TASK_PRIORITY_VERY_LOW
} TASK_INTERFACE_TASK_PRIORITY;

// --------------------------------------------------------------------------------

/**
 * @brief
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
     * @brief Unique id of this task.
     * Every task has its own unique id.
     */ 
    u8 identifier;

    /**
     * @brief This variable is used by the task-controller
     * to determine the next execution time of this task.
     */
    u16 new_run_timeout;

    /**
     * @brief Contains the last time this task was scheduled.
     */
    u16 last_run_time;

    /**
     * @brief Number of usec this task was active at the last schedule.
     */
    u32 last_active_time;

    /**
     * @brief Reference to the name of this task
     */
    const char* p_task_name;

    /**
     * @brief Length of the tasks name.
     */
    u8 name_length;
        
    /**
     * @brief Callback to initialize a task.
     * This function is called immediately after
     * a task was registered to the task-controller.
     */
    MCU_TASK_INTERFACE_INIT_CALLBACK init;

    /**
     * @brief Returns the interval in milliseconds at which the task needs to be executed.
     * 
     * @return number of milliseconds between two following task-executions.
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
     *     MCU_TASK_SLEEPING    The task is in sleep mode and will not be executed
     *                          If this state is returned the system can enter the low-power state
     *     MCU_TASK_IDLE        The task is in sleep mode and will not be executed
     *                          If this state is returned the system can enter the low-power state
     *     MCU_TASK_RUNNING     The task is ready to be executed. 
     *                          If this state is returned after the task was executed
     *                          the system then cannot enter the idle-state.
     *     MCU_TASK_TERMINATED  The task is terminated and cannot executed anymore
     *                          If this state is returned the system can enter the low-power state
     * 
     */
    MCU_TASK_INTERFACE_GET_STATE_CALLBACK get_sate;

    /**
     * @brief Executes the task. This function is blocking.
     * The task decides when it will give back cpu-time.
     * A running task cannot be stopped. It can only be interrupted by an HW-IRQ.
     */
    MCU_TASK_INTERFACE_RUN_CALLBACK run;

    /**
     * @brief This function is optional for every task.
     * A task can be run for a short time in the background.
     * This function can be used to update internal status-flags and timers.
     * A task is not allowed to execute complex opperations in this function
     */
    MCU_TASK_INTERFACE_BG_RUN_CALLBACK background_run;

    /**
     * @brief This function is optional for every task
     * If the system goes into low-power mode, this functions is called.
     * A task may perform special opperations to enter its low-power mode
     * E.g. a task needs to set the GPIOs into a low-power state.
     */
    MCU_TASK_INTERFACE_SLEEP_CALLBACK sleep;

    /**
     * @brief This function is optional for every task.
     * If the system is un low-power mode and returns to normal operation
     * this function is called.
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
     */
    MCU_TASK_INTERFACE_TERMINATE_CALLBACK terminate;

    /**
     * @brief pointer to the next task in the list.
     * 
     */
    struct MCU_TASK_INTERFACE* next_task;

} MCU_TASK_INTERFACE;

// --------------------------------------------------------------------------------

#define MCU_TASK_SCHEDULE_NO_TIMEOUT        0

// --------------------------------------------------------------------------------

ITERATOR_INTERFACE_INCLUDE(MCU_TASK_ITERATOR_INTERFACE)

// --------------------------------------------------------------------------------

/**
 * @brief Initializes the task controller module.
 */
void mcu_task_controller_init(void);

/**
 * @brief Registers a new task to the system.
 * The task is added to tha current list of tasks in order of its priority
 * 
 * @param p_mcu_task the task to be added 
 */
void mcu_task_controller_register_task(MCU_TASK_INTERFACE* p_mcu_task);

/**
 * @brief Performs a schedule of all currently known tasks.
 * The task are scheduled in order of their priority
 * 
 */
void mcu_task_controller_schedule(void);

/**
 * @brief Gets the number of tasks that are currently registered to the mcu-task controller.
 * 
 * @return The number of tasks that are currently registered to the mcu-task controller.
 */
u8 mcu_task_controller_task_count(void);

// --------------------------------------------------------------------------------

/**
 * @brief Enables or disables the task statistics.
 * E.g. measuremnt of the runtime per task
 * 
 * @param enable TASK_CTRL_STATISTIC_ON  - task statistics are enabled
 *               TASK_CTRL_STATISTIC_OFF - task statistics are disabled
 */
void mcu_task_controller_enable_statistics(TASK_CTRL_STATISTIC_EN enable);

/**
 * @brief Reset the statistics of all availabel task.
 * Statistics must be enabled to reset them. Otherwise nothing happens.
 */
void mcu_task_controller_reset_statistics(void);

// --------------------------------------------------------------------------------

#endif // _MCU_TASK_INTERFACE_H_
