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

		u64 u64_time_ms = (u64)time_spec.tv_sec;
		u64_time_ms = u64_time_ms * 1e3;
		u64_time_ms = u64_time_ms & 0xFFFFFFFF;

		u32 u32_time_ms = (u32) u64_time_ms;
		u32 u32_time_ns = (u32) time_spec.tv_nsec;

		DEBUG_TRACE_long(time_spec.tv_sec,  "local_rtc_timer_gettime_u32() - Seconds: ");
		DEBUG_TRACE_long(time_spec.tv_nsec, "local_rtc_timer_gettime_u32() - Nano-Seconds: ");

		DEBUG_TRACE_long(u32_time_ms,  "local_rtc_timer_gettime_u32() - Milli-Seconds (from seconds): ");
		DEBUG_TRACE_long(u32_time_ns, "local_rtc_timer_gettime_u32() - Milli-Seconds (from Nano-Seconds): ");

		return (u32_time_ms + (u32_time_ns / 1e6));

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
