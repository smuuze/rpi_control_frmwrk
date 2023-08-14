/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "time_management/time_management.h"
#include <time.h>

//---------- Implementation of Traces -----------------------------------------

#define TRACER_OFF

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

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
	return (time_mgmnt_elapsed_u8(time_reference) > time_interval) ? 1 : 0;
}

u8 time_mgmnt_istimeup_u16(u16 time_reference, u16 time_interval) {
	return time_mgmnt_elapsed_u16(time_reference) < time_interval ? 0 : 1;
}

u8 time_mgmnt_istimeup_u32(u32 time_reference, u32 time_interval) {
	return (time_mgmnt_elapsed_u32(time_reference) > time_interval) ? 1 : 0;
}

u8 time_mgmnt_elapsed_u8(u8 time_reference) {
	return (time_mgmnt_gettime_u8() - time_reference);
}

u16 time_mgmnt_elapsed_u16(u16 time_reference) {
	return (time_mgmnt_gettime_u16() - time_reference);
}

u32 time_mgmnt_elapsed_u32(u32 time_reference) {
	return (time_mgmnt_gettime_u32() - time_reference);
}
