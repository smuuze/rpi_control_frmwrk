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
 * @file    thread_interface.h
 * @author  Sebastian Lesse
 * @date    2022 / 07 / 16
 * @brief   Create thread objects using the posix thread extension
 * 
 *          Usage:
 * 
 *          1. CREATE CALLBACK FUNCTIONS
 * 
 *              THREAD_INTERFACE_EXIT_STATUS thread_print_trace_object_run(void) {
 *                  ...
 *                  return exit_status;
 *              }
 * 
 *              void thread_print_trace_object_init(void) {
 *                  ...
 *              }
 * 
 *              void MY_THREAD_OBJECT_terminate(void) {
 *                  ...
 *              }
 * 
 *          2. CREATE THREAD OBJECT
 *
 *              // somewhere outside of a function
 *              THREAD_INTERFACE_BUILD_THREAD(
 *                  MY_THREAD_OBJECT,
 *                  THREAD_PRIORITY_MIDDLE,
 *                  MY_THREAD_OBJECT_init,
 *                  MY_THREAD_OBJECT_run,
 *                  MY_THREAD_OBJECT_terminate
 *              )
 * 
 *          3. INITIALIZE THREAD OBJECT
 * 
 *              MY_THREAD_OBJECT_init();
 * 
 *          4. START THREAD
 * 
 *              if (MY_THREAD_OBJECT_start()) {
 *                  // everything is fine
 *              }
 * 
 *          5. TERMINATE THREAD
 * 
 *              MY_THREAD_OBJECT_terminate();
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_thread_interface_
#define _H_thread_interface_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "pthread.h"

#ifndef _POSIX_THREADS 
#error THREADS_ARE_NOT_AVAILABLE
#endif

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
typedef enum {
    THREAD_PRIORITY_HIGH = 0x01,
    THREAD_PRIORITY_MIDDLE,
    THREAD_PRIORITY_LOW
} THREAD_INTERFACE_PRITORITY;

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
typedef pthread_t THREAD_INTERFACE_ID;

/**
 * @brief Typedefinition for exit-status of a thread
 * created by the thread-interface
 * 
 */
typedef uint8_t THREAD_INTERFACE_EXIT_STATUS;

/**
 * @brief This method is used to initialize the thread befor it is started.
 * E.g. connecting to signals
 * 
 */
typedef void (*THREAD_INTERFACE_INIT_CALLBACK) (void);

/**
 * @brief This method is used to run the thread.
 * 
 */
typedef void* (*THREAD_INTERFACE_RUN_FUNCTION_CALLBACK) (void* p_thread_id);

/**
 * @brief This method is used to terminate the thread,
 * by updating its internal status.
 * 
 */
typedef void (*THREAD_INTERFACE_TERMINATE_CALLBACK) (void);

/**
 * @brief 
 * 
 */
typedef struct {

    /**
     * @brief Identification of this thread
     * Is set by the posix thread extension on calling start()
     * 
     */
    THREAD_INTERFACE_ID id;

    /**
     * @brief Is set by the thread in the moment the thread is terminated
     * 
     */
    THREAD_INTERFACE_EXIT_STATUS exit_status;

    /**
     * @brief Priority at which this thread will run
     * 
     */
    THREAD_INTERFACE_PRITORITY priority;

    /**
     * @brief Callback that is executed on initializing the thread
     * Should not be NULL.
     * 
     */
    THREAD_INTERFACE_INIT_CALLBACK init;

    /**
     * @brief Callback to the runtime function of this thread
     * Must not be NULL
     * 
     */
    THREAD_INTERFACE_RUN_FUNCTION_CALLBACK run;

    /**
     * @brief Callback that is executed on terminating the thead object
     * Can be NULL.
     * 
     */
    THREAD_INTERFACE_TERMINATE_CALLBACK terminate;

} THREAD_INTERFACE_TYPE;

// --------------------------------------------------------------------------------

#define THREAD_INTERFACE_BUILD_THREAD(name, prio, p_init, p_run, p_terminate)                   \
    static THREAD_INTERFACE_TYPE __##name##_thread_obj = {                                      \
        .id = 0,                                                                                \
        .exit_status = 0,                                                                       \
        .priority = THREAD_PRIORITY_LOW,                                                        \
        .init = NULL,                                                                           \
        .run = NULL,                                                                            \
        .terminate = NULL,                                                                      \
    };                                                                                          \
                                                                                                \
    static void* __##name##_run(void* p_thread_id) {                                            \
        (void) p_thread_id;                                                                     \
        __##name##_thread_obj.exit_status = p_run();                                            \
        pthread_exit(NULL);                                                                     \
        return NULL;                                                                            \
    }                                                                                           \
                                                                                                \
    void name##_init(void) {                                                                    \
        thread_interface_build(                                                                 \
            &__##name##_thread_obj,                                                             \
            prio,                                                                               \
            &p_init,                                                                            \
            &__##name##_run,                                                                    \
            &p_terminate                                                                        \
        );                                                                                      \
        thread_interface_init(&__##name##_thread_obj);                                          \
    }                                                                                           \
                                                                                                \
    u8 name##_start(void) {                                                                     \
        return thread_interface_create_thread(&__##name##_thread_obj);                          \
    }                                                                                           \
                                                                                                \
    void name##_terminate(void) {                                                               \
        thread_interface_terminate(&__##name##_thread_obj);                                     \
    }

#define THREAD_INTERFACE_INCLUDE_THREAD(name)       \
    void name##_init(void);                         \
    u8 name##_start(void);                          \
    void name##_terminate(void);

// --------------------------------------------------------------------------------

/**
 * @brief builds the thread. The given methods are set to the method pointer of the thread-object
 * 
 * @param p_thread 
 * @param priority 
 * @param init 
 * @param run 
 * @param terminate 
 */
void thread_interface_build(THREAD_INTERFACE_TYPE* p_thread,
                THREAD_INTERFACE_PRITORITY priority,
                THREAD_INTERFACE_INIT_CALLBACK init,
                THREAD_INTERFACE_RUN_FUNCTION_CALLBACK run,
                THREAD_INTERFACE_TERMINATE_CALLBACK terminate);

/**
 * @brief initializes the thread by calling the init() method.
 * This will not start the thread.
 * 
 * @param p_thread valid context of the thread object
 */
void thread_interface_init(THREAD_INTERFACE_TYPE* p_thread);

/**
 * @brief creates the thread and starts it by calling the run() method.
 * 
 * @param p_thread valid context of the thread object
 * @return 1 if the thread was successful created ands start, otherwise 0
 */
u8 thread_interface_create_thread(THREAD_INTERFACE_TYPE* p_thread);

/**
 * @brief terminates the thread by calling the terminate() method
 * 
 * @param p_thread valid context of the thread object
 */
void thread_interface_terminate(THREAD_INTERFACE_TYPE* p_thread);

// --------------------------------------------------------------------------------

#endif // _H_thread_interface_

// --------------------------------------------------------------------------------
