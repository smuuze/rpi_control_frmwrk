/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "time_management/time_management.h"
#include <time.h>

//---------- Implementation of Traces -----------------------------------------

#define TRACER_ON
#include "tracer.h"

//-----------------------------------------------------------------------------

static u32 mstime_get_time(void) {
	struct timespec time_spec;

	if (clock_gettime(CLOCK_MONOTONIC, &time_spec) == 0) {
		return ((u32)(time_spec.tv_sec * 1e3) + (u32)(time_spec.tv_nsec / 1e6)); //time_spec.tv_nsec / 1000 / 1000;
	} else {
		return 0;
	}
}

//-----------------------------------------------------------------------------

u8 time_mgmnt_gettime_u8(void) {
	return (u8)mstime_get_time();
}

u16 time_mgmnt_gettime_u16(void) {
	return (u16)mstime_get_time();
}

u32 time_mgmnt_gettime_u32(void) {
	return mstime_get_time();
}

u8 time_mgmnt_istimeup_u8(u8 time_reference, u8 time_interval) {

	u8 time = time_mgmnt_gettime_u8();

	if (time < time_reference) {

		// there was an overflow 
		time_interval -= (TIME_MGMNT_MAX_TIME_U8 - time_reference);
		time_reference = 0;
	}

	return ((time - time_reference) > time_interval) ? 1 : 0;
}

u8 time_mgmnt_istimeup_u16(u16 time_reference, u16 time_interval) {

/*
	u16 time = time_mgmnt_gettime_u16();

	if (time < time_reference) {

		// there was an overflow 
		time_interval -= (TIME_MGMNT_MAX_TIME_U16 - time_reference);
		time_reference = 0;
	}

	u8 time_is_up = (time - time_reference) > time_interval ? 1 : 0;

	if (time_is_up != 0 && (time_mgmnt_elapsed_u16(time_reference) < time_interval)) {

		ALWAYS_PASS("time_mgmnt_istimeup_u16() - FAILURE !!! ---");
		ALWAYS_TRACE_word(time, "- Atual Time : ");
		ALWAYS_TRACE_word(time_reference, "- Reference : ");
		ALWAYS_TRACE_word(time_interval, "- Interval : ");
		ALWAYS_TRACE_word(time_mgmnt_elapsed_u16(time_reference), "- Elapsed : ");
	}
*/
	return time_mgmnt_elapsed_u16(time_reference) < time_interval ? 0 : 1;
}

u8 time_mgmnt_istimeup_u32(u32 time_reference, u32 time_interval) {

	u32 time = time_mgmnt_gettime_u32();

	if (time < time_reference) {

		// there was an overflow 
		time_interval -= (TIME_MGMNT_MAX_TIME_U32 - time_reference);
		time_reference = 0;
	}

	return ((time - time_reference) > time_interval) ? 1 : 0;
}

u8 time_mgmnt_elapsed_u8(u8 time_reference) {

	u8 time = time_mgmnt_gettime_u8();

	if (time < time_reference) {
		return (TIME_MGMNT_MAX_TIME_U8 - time_reference) + time;
	}

	return time - time_reference;
}

u16 time_mgmnt_elapsed_u16(u16 time_reference) {

	u16 time = time_mgmnt_gettime_u16();

	if (time < time_reference) {

		//ALWAYS_PASS("time_mgmnt_elapsed_u16() - Overflow detected");
		//ALWAYS_TRACE_word(time, "- Atual Time : ");
		//ALWAYS_TRACE_word(time_reference, "- Reference : ");

		return (TIME_MGMNT_MAX_TIME_U16 - time_reference) + time;
	}

	return time - time_reference;
}

u32 time_mgmnt_elapsed_u32(u32 time_reference) {

	u32 time = time_mgmnt_gettime_u32();

	if (time < time_reference) {
		return (TIME_MGMNT_MAX_TIME_U32 - time_reference) + time;
	}

	return time - time_reference;
}

