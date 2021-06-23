/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	thread_interface.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */
 
#ifndef _THREAD_INTERFACE_H_
#define _THREAD_INTERFACE_H_

// ------------------------------------------------------------------------------

#include "config.h"

// ------------------------------------------------------------------------------

#include "pthread.h"

#ifndef _POSIX_THREADS 
#error THREADS_ARE_NOT_AVAILABLE
#endif

// ------------------------------------------------------------------------------

/*!
 *
 */
typedef enum {
	THREAD_PRIORITY_HIGH = 0x01,
	THREAD_PRIORITY_MIDDLE,
	THREAD_PRIORITY_LOW
} THREAD_INTERFACE_PRITORITY;

/*!
 *
 */
typedef pthread_t THREAD_INTERFACE_ID;

/**
 * @brief This method is used to initialize the thread befor it is started.
 * E.g. connecting to signals
 * 
 */
typedef void (*THREAD_INTERFACE_INIT_CALLBACK)		(void);

/**
 * @brief This method is used to run the thread.
 * 
 */
typedef void* (*THREAD_INTERFACE_RUN_FUNCTION_CALLBACK)		(void* arguments);

/**
 * @brief This method is used to terminate the thread,
 * by updating its internal status.
 * 
 */
typedef void (*THREAD_INTERFACE_TERMINATE_CALLBACK)		(void);

/*!
 *
 */
typedef struct {
	THREAD_INTERFACE_ID id;
	THREAD_INTERFACE_PRITORITY priority;
	THREAD_INTERFACE_INIT_CALLBACK init;
	THREAD_INTERFACE_RUN_FUNCTION_CALLBACK run;
	THREAD_INTERFACE_TERMINATE_CALLBACK terminate;
} THREAD_INTERFACE_TYPE;

// ------------------------------------------------------------------------------

#define THREAD_INTERFACE_BUILD_THREAD(name, prio, p_init, p_run, p_terminate)				\
	static THREAD_INTERFACE_TYPE __##name##_thread_obj = {						\
		.id = 0,										\
		.priority = THREAD_PRIORITY_LOW,							\
		.init = NULL,										\
		.run = NULL,										\
		.terminate = NULL,									\
	};												\
													\
	void name##_init(void) {									\
		thread_interface_build(&__##name##_thread_obj, prio, &p_init, &p_run, &p_terminate);	\
		thread_interface_init(&__##name##_thread_obj);						\
	}												\
													\
	u8 name##_start(void) {										\
		return thread_interface_create_thread(&__##name##_thread_obj);				\
	}												\
													\
	void name##_terminate(void) {									\
		thread_interface_terminate(&__##name##_thread_obj);					\
	}

#define THREAD_INTERFACE_INCLUDE_THREAD(name)								\
	void name##_init(void);										\
	u8 name##_start(void);										\
	void name##_terminate(void);

// ------------------------------------------------------------------------------

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

// ------------------------------------------------------------------------------

#endif // _THREAD_INTERFACE_H_
