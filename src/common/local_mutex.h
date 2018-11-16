#ifndef _LOCAL_MUTEX_H_
#define _LOCAL_MUTEX_H_

#include "config.h"

//#include <atomic.h>

#define MUTEX_INVALID_ID 	0xFF
#define MUTEX_ID_FIRST_VALUE	0x01
#define MUTEX_ID_LAST_VALUE	0xFE

/*!
 *
 */
typedef struct MUTEX {
	u8 id;
	u8 is_requested;
} MUTEX_TYPE;

/*!
 *
 * @param p_status
 * @param bit_flag
 */
u8 mutex_request(MUTEX_TYPE* p_mutex);

/*!
 *
 * @param p_status
 * @param bit_flag
 * @return
 */
void mutex_release(MUTEX_TYPE* p_mutex, u8 mutex_id);

/*!
 *
 * @param p_status
 * @param bit_flag
 */
u8 mutex_is_requested(MUTEX_TYPE* p_mutex);



#define BUILD_MUTEX(name)														\
																	\
	static MUTEX_TYPE __##name##_mutex;												\
																	\
	inline static u8 name##_request(void) 		{ return mutex_request(&(__##name##_mutex)); }					\
	inline static void name##_release(u8 m_id) 	{ mutex_release(&(__##name##_mutex), m_id); }					\
	inline static u8 name##_is_requested(void)	{ return mutex_is_requested(&(__##name##_mutex)); }

#endif // _LOCAL_MUTEX_H_
