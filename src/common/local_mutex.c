
#include "config.h"
#include "local_mutex.h"


/*!
 *
 * @param p_status
 * @param bit_flag
 */
u8 mutex_request(MUTEX_TYPE* p_mutex) {
	if (p_mutex->is_requested != 0) {
		return MUTEX_INVALID_ID;
	}
	
	if (p_mutex->id == MUTEX_INVALID_ID || p_mutex->id == 0) {
		p_mutex->id = MUTEX_ID_FIRST_VALUE;
	}
	
	p_mutex->is_requested = 1;	
	return p_mutex->id;
}

/*!
 *
 * @param p_status
 * @param bit_flag
 * @return
 */
void mutex_release(MUTEX_TYPE* p_mutex, u8 mutex_id) {
	if (p_mutex->is_requested != 0) {
		return;
	}
	
	if (p_mutex->id != mutex_id) {
		return;
	}
	
	p_mutex->is_requested = 0;
	p_mutex->id += 1;
}

/*!
 *
 * @param p_status
 * @param bit_flag
 */
u8 mutex_is_requested(MUTEX_TYPE* p_mutex) {
	return p_mutex->is_requested != 0 ? 1 : 0;
}