/*! \file *********************************************************************

 *****************************************************************************/

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!

//-----------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include <time.h>

//-----------------------------------------------------------------------------

#include "driver/rtc/rtc_driver_interface.h"

//-----------------------------------------------------------------------------

void rtc_driver_init(void) {
	DEBUG_PASS("rtc_driver_init()");
}

void rtc_driver_deinit(void) {
	DEBUG_PASS("rtc_driver_deinit()");
}

u8 local_rtc_timer_gettime_u8(void) {
	return (u8)local_rtc_timer_gettime_u32();
}

u16 local_rtc_timer_gettime_u16(void) {
	return (u16)local_rtc_timer_gettime_u32();
}

u32 local_rtc_timer_gettime_u32(void) {
	struct timespec time_spec;

	if (clock_gettime(CLOCK_MONOTONIC, &time_spec) == 0) {
		return ((u32)(time_spec.tv_sec * 1e3) + (u32)(time_spec.tv_nsec / 1e6)); //time_spec.tv_nsec / 1000 / 1000;
	} else {
		return 0;
	}
}

u8 local_rtc_timer_istimeup_u8(u8 time_reference, u8 time_interval) {
	return (rtc_timer_elapsed_u8(time_reference) > time_interval) ? 1 : 0;
}

u8 local_rtc_timer_istimeup_u16(u16 time_reference, u16 time_interval) {
	return rtc_timer_elapsed_u16(time_reference) > time_interval ? 1 : 0;
}

u8 local_rtc_timer_istimeup_u32(u32 time_reference, u32 time_interval) {
	return rtc_timer_elapsed_u32(time_reference) > time_interval ? 1 : 0;
}

u8 rtc_timer_elapsed_u8(u8 time_reference) {
	return local_rtc_timer_gettime_u8() - time_reference;
}

u16 rtc_timer_elapsed_u16(u16 time_reference) {
	return local_rtc_timer_gettime_u16() - time_reference;
}

u32 rtc_timer_elapsed_u32(u32 time_reference) {
	return local_rtc_timer_gettime_u32() - time_reference;
}

// --------------------------------------------------------------------------------

/**
 * @see driver/rtc/rtc_interface.h#rtc_timer_get_usec
 */
u64 rtc_timer_get_usec(void) {
    struct timespec time_spec;

    if (clock_gettime(CLOCK_MONOTONIC, &time_spec) == 0) {

        u64 time_us = (u64)time_spec.tv_sec * 1000000;
        time_us += time_spec.tv_nsec / 1000;
        return time_us;

    } else {
        return 0;
    }
}
