/**
 * @file local_mutex.h
 * @author sebastian lesse (sebastian lesse)
 * @brief 
 * @version 1.0
 * @date 2021-09-25
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef _LOCAL_MUTEX_H_
#define _LOCAL_MUTEX_H_

//-----------------------------------------------------------------------------

#include "config.h"

//-----------------------------------------------------------------------------

/**
 * @brief Value to mark a mutex-id to invalid
 * 
 */
#define MUTEX_INVALID_ID 	0x00

/**
 * @brief if a mutex is requestd for the first time
 * this value will be returned.
 * 
 */
#define MUTEX_ID_FIRST_VALUE	0x01

/**
 * @brief This value is the last value that will be
 * returned. If the mutex is released and requested
 * once more the MUTEX_ID_FIRST_VALUE is returned.
 * 
 */
#define MUTEX_ID_LAST_VALUE	0xFE

//-----------------------------------------------------------------------------

/**
 * @brief Creats a mutex to use for critical program-sections
 * Usage:
 * 
 * - declaration:
 * 
 * 	BUILD_MUTEX(MY_MUTEX)
 * 
 * - check if mutes is requested:
 * 
 * 	if (MY_MUTEX_is_requested()) {
 * 		return;
 * 	}
 * 
 * - request mutex:
 * 
 * 	u8 mutex_id = MY_MUTEX_request();
 * 	if (mutex_id == MUTEX_INVALID_ID) {
 * 		return;
 * 	}
 * 
 * - release mutex:
 * 
 * 	MY_MUTEX_release(mutex_id);
 * 
 */
#define BUILD_MUTEX(name)											\
														\
	static MUTEX_TYPE __##name##_mutex;									\
														\
	inline static u8 name##_request(void) 		{ return mutex_request(&(__##name##_mutex)); }		\
	inline static void name##_release(u8 m_id) 	{ mutex_release(&(__##name##_mutex), m_id); }		\
	inline static u8 name##_is_requested(void)	{ return mutex_is_requested(&(__##name##_mutex)); }

//-----------------------------------------------------------------------------

/**
 * @brief realizes a mutex-context. This context holds the
 * actual used mutex-id and a flag if it is requested.
 * 
 */
typedef struct MUTEX {
	u8 id;
	u8 is_requested;
} MUTEX_TYPE;

//-----------------------------------------------------------------------------

/**
 * @brief Request a mutex to enter a critical program-section.
 * Returns the mutex-id that is used to release the lock.
 * A valid mutex-id is in the range MUTEX_ID_FIRST_VALUE to
 * MUTEX_ID_LAST_VALUE.
 * There is always only one valid mutex-id at a time.
 * 
 * @see MUTEX_ID_FIRST_VALUE
 * @see MUTEX_ID_LAST_VALUE
 * @see MUTEX_INVALID_ID
 * 
 * @param p_mutex mutex-context to get the lock
 * @return the mutex-id that is used to release the lock,
 * or MUTEX_INVALID_ID if the mutex is already requested.
 */
u8 mutex_request(MUTEX_TYPE* p_mutex);

/**
 * @brief Release the lock on leaving a critical program-section.
 * the mutex can only be released with the id that was returned
 * by the lock request.
 * 
 * @param p_mutex mutex-context to release the lock
 * @param mutex_id the mutex-id that was returned on lock request
 */
void mutex_release(MUTEX_TYPE* p_mutex, u8 mutex_id);

/**
 * @brief Checks if a mutex is requested right now.
 * This function shall be used before requesting a mutex.
 * 
 * @param p_mutex the mutex-context to check
 * @return 1 if the mutex is already requested, otherwise 0
 */
u8 mutex_is_requested(MUTEX_TYPE* p_mutex);

//-----------------------------------------------------------------------------

#endif // _LOCAL_MUTEX_H_

//-----------------------------------------------------------------------------
