
#include "config.h"
#include "local_mutex.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_OFF

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

#include "tracer.h"

//-----------------------------------------------------------------------------

u8 mutex_request(MUTEX_TYPE* p_mutex) {

	if (p_mutex->is_requested != 0) {
		return MUTEX_INVALID_ID;
	}

	if (p_mutex->id == MUTEX_INVALID_ID || p_mutex->id == 0) {
		p_mutex->id = MUTEX_ID_FIRST_VALUE;
	}

	TRACE_byte(p_mutex->id); // mutex_request() -------------------------

	p_mutex->is_requested = 1;
	return p_mutex->id;
}

void mutex_release(MUTEX_TYPE* p_mutex, u8 mutex_id) {

	if (p_mutex->is_requested == 0) {
		PASS(); // mutex_release() - mutex was not requested
		return;
	}

	if (p_mutex->id != mutex_id) {
		PASS(); // mutex_release() - incorrect mutex id
		TRACE_byte(mutex_id); //
		TRACE_byte(p_mutex->id); //
		return;
	}

	TRACE_byte(p_mutex->id); // mutex_release() -------------------------

	if (p_mutex->id == MUTEX_ID_LAST_VALUE) {
		p_mutex->id = MUTEX_ID_FIRST_VALUE;
	} else {
		p_mutex->id += 1;
	}

	p_mutex->is_requested = 0;
}

u8 mutex_is_requested(MUTEX_TYPE* p_mutex) {

	return p_mutex->is_requested != 0 ? 1 : 0;
}
