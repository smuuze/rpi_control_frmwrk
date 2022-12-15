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
 * @file    rtc_driver_raspberrypi.c
 * @author  Sebastian Lesse
 * @date    2022 / 12 / 15
 * @brief   Implementation of the rtc_driver_interface
 *          to be used on a Linux based Raspberry Pi
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

/**
 * @see driver/rtc/rtc_interface.h#rtc_driver_init
 */
void rtc_driver_init(void) {
    DEBUG_PASS("rtc_driver_init()");
}

/**
 * @see driver/rtc/rtc_interface.h#rtc_driver_deinit
 */
void rtc_driver_deinit(void) {
    DEBUG_PASS("rtc_driver_deinit()");
}

// --------------------------------------------------------------------------------

/**
 * @see driver/rtc/rtc_interface.h#local_rtc_timer_gettime_u8
 */
u8 local_rtc_timer_gettime_u8(void) {
    return (u8)local_rtc_timer_gettime_u32();
}

/**
 * @see driver/rtc/rtc_interface.h#local_rtc_timer_gettime_u16
 */
u16 local_rtc_timer_gettime_u16(void) {
    return (u16)local_rtc_timer_gettime_u32();
}

/**
 * @see driver/rtc/rtc_interface.h#local_rtc_timer_gettime_u32
 */
u32 local_rtc_timer_gettime_u32(void) {
    struct timespec time_spec;

    if (clock_gettime(CLOCK_MONOTONIC, &time_spec) == 0) {

        u64 u64_time_ms = (u64)time_spec.tv_sec;
        u64_time_ms = u64_time_ms * 1e3;
        u64_time_ms = u64_time_ms & 0xFFFFFFFF;

        u32 u32_time_ms = (u32) u64_time_ms;
        u32 u32_time_ns = (u32) time_spec.tv_nsec;

        DEBUG_TRACE_long(
            time_spec.tv_sec, 
            "local_rtc_timer_gettime_u32() - Seconds: "
        );

        DEBUG_TRACE_long(
            time_spec.tv_nsec,
            "local_rtc_timer_gettime_u32() - Nano-Seconds: "
        );

        DEBUG_TRACE_long(
            u32_time_ms, 
            "local_rtc_timer_gettime_u32() - Milli-Seconds (from seconds): "
        );

        DEBUG_TRACE_long(
            u32_time_ns,
            "local_rtc_timer_gettime_u32() - Milli-Seconds (from Nano-Seconds): "
        );

        return (u32_time_ms + (u32_time_ns / 1e6));

    } else {
        return 0;
    }
}

// --------------------------------------------------------------------------------

/**
 * @see driver/rtc/rtc_interface.h#local_rtc_timer_istimeup_u8
 */
u8 local_rtc_timer_istimeup_u8(u8 time_reference, u8 time_interval) {
    return (rtc_timer_elapsed_u8(time_reference) > time_interval) ? 1 : 0;
}

/**
 * @see driver/rtc/rtc_interface.h#local_rtc_timer_istimeup_u16
 */
u8 local_rtc_timer_istimeup_u16(u16 time_reference, u16 time_interval) {
    return rtc_timer_elapsed_u16(time_reference) > time_interval ? 1 : 0;
}

/**
 * @see driver/rtc/rtc_interface.h#local_rtc_timer_istimeup_u32
 */
u8 local_rtc_timer_istimeup_u32(u32 time_reference, u32 time_interval) {
    return rtc_timer_elapsed_u32(time_reference) > time_interval ? 1 : 0;
}

// --------------------------------------------------------------------------------

/**
 * @see driver/rtc/rtc_interface.h#rtc_timer_elapsed_u8
 */
u8 rtc_timer_elapsed_u8(u8 time_reference) {
    return local_rtc_timer_gettime_u8() - time_reference;
}

/**
 * @see driver/rtc/rtc_interface.h#rtc_timer_elapsed_u16
 */
u16 rtc_timer_elapsed_u16(u16 time_reference) {
    return local_rtc_timer_gettime_u16() - time_reference;
}

/**
 * @see driver/rtc/rtc_interface.h#rtc_timer_elapsed_u32
 */
u32 rtc_timer_elapsed_u32(u32 time_reference) {
    return local_rtc_timer_gettime_u32() - time_reference;
}

// --------------------------------------------------------------------------------

/**
 * @see driver/rtc/rtc_interface.h#rtc_timer_usleep
 */
void rtc_timer_usleep(u32 delay_us) {
    usleep(delay_us);
}

// --------------------------------------------------------------------------------
