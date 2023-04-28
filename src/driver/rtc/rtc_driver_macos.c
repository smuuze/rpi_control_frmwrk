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
 * @file    rtc_driver_universal.c
 * @author  Sebastian Lesse
 * @date    2022 / 12 / 31
 * @brief   Short description of this file
 * 
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "cpu.h"

// --------------------------------------------------------------------------------

#include <time.h>

// --------------------------------------------------------------------------------

#include "driver/rtc/rtc_driver_interface.h"

// --------------------------------------------------------------------------------

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
