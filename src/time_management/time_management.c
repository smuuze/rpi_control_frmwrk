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
 * @file    time_management.c
 * @author  Sebastian Lesse
 * @date    2022 / 11 / 13
 * @brief   Common implementation of the time-management interface
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

#include "driver/rtc/rtc_driver_interface.h"
#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

/**
 * @see time_management/time_management.h#time_mgmnt_gettime_u8
 */
u8 time_mgmnt_gettime_u8(void) {
    return rtc_timer_gettime_u8();
}

/**
 * @see time_management/time_management.h#time_mgmnt_gettime_u16
 */
u16 time_mgmnt_gettime_u16(void) {
    return rtc_timer_gettime_u16();
}

/**
 * @see time_management/time_management.h#time_mgmnt_gettime_u32
 */
u32 time_mgmnt_gettime_u32(void) {
    return rtc_timer_gettime_u32();
}

// --------------------------------------------------------------------------------

/**
 * @see time_management/time_management.h#time_mgmnt_start_u8
 */
void time_mgmnt_start_u8(TIME_MANAGEMENT_U8_TYPE* p_context) {
    p_context->time_reference = rtc_timer_gettime_u8();
    p_context->status = TIME_MGMNT_STATUS_ACTIVE;
}

/**
 * @see time_management/time_management.h#time_mgmnt_start_u16
 */
void time_mgmnt_start_u16(TIME_MANAGEMENT_U16_TYPE* p_context) {
    p_context->time_reference = rtc_timer_gettime_u16();
    p_context->status = TIME_MGMNT_STATUS_ACTIVE;
}

/**
 * @see time_management/time_management.h#time_mgmnt_start_u32
 */
void time_mgmnt_start_u32(TIME_MANAGEMENT_U32_TYPE* p_context) {
    p_context->time_reference = rtc_timer_gettime_u32();
    p_context->status = TIME_MGMNT_STATUS_ACTIVE;
}

// --------------------------------------------------------------------------------

/**
 * @see time_management/time_management.h#time_mgmnt_stop_u8
 */
void time_mgmnt_stop_u8(TIME_MANAGEMENT_U8_TYPE* p_context) {
    p_context->time_reference = 0;
    p_context->status = 0;
}

/**
 * @see time_management/time_management.h#time_mgmnt_stop_u16
 */
void time_mgmnt_stop_u16(TIME_MANAGEMENT_U16_TYPE* p_context) {
    p_context->time_reference = 0;
    p_context->status = 0;
}

/**
 * @see time_management/time_management.h#time_mgmnt_stop_u32
 */
void time_mgmnt_stop_u32(TIME_MANAGEMENT_U32_TYPE* p_context) {
    p_context->time_reference = 0;
    p_context->status = 0;
}

// --------------------------------------------------------------------------------

/**
 * @see time_management/time_management.h#time_mgmnt_is_active_u8
 */
u8 time_mgmnt_is_active_u8(TIME_MANAGEMENT_U8_TYPE* p_context) {
    return p_context->status == TIME_MGMNT_STATUS_ACTIVE ? 1 : 0;
}

/**
 * @see time_management/time_management.h#time_mgmnt_is_active_u16
 */
u8 time_mgmnt_is_active_u16(TIME_MANAGEMENT_U16_TYPE* p_context) {
    return p_context->status == TIME_MGMNT_STATUS_ACTIVE ? 1 : 0;
}

/**
 * @see time_management/time_management.h#time_mgmnt_is_active_u32
 */
u8 time_mgmnt_is_active_u32(TIME_MANAGEMENT_U32_TYPE* p_context) {
    return p_context->status == TIME_MGMNT_STATUS_ACTIVE ? 1 : 0;
}

// --------------------------------------------------------------------------------

/**
 * @see time_management/time_management.h#time_mgmnt_istimeup_u8
 */
u8 time_mgmnt_istimeup_u8(TIME_MANAGEMENT_U8_TYPE* p_context, u8 time_interval) {
    return (time_mgmnt_elapsed_u8(p_context) >= time_interval) ? 1 : 0;
}

/**
 * @see time_management/time_management.h#time_mgmnt_istimeup_u16
 */
u8 time_mgmnt_istimeup_u16(TIME_MANAGEMENT_U16_TYPE* p_context, u16 time_interval) {
    return time_mgmnt_elapsed_u16(p_context) >= time_interval ? 1 : 0;
}

/**
 * @see time_management/time_management.h#time_mgmnt_istimeup_u32
 */
u8 time_mgmnt_istimeup_u32(TIME_MANAGEMENT_U32_TYPE* p_context, u32 time_interval) {
    return time_mgmnt_elapsed_u32(p_context) >= time_interval ? 1 : 0;
}

// --------------------------------------------------------------------------------

/**
 * @see time_management/time_management.h#time_mgmnt_istimeup_raw_u8
 */
u8 time_mgmnt_istimeup_raw_u8(u8 time_reference, u8 time_interval) {

    TIME_MANAGEMENT_U8_TYPE context = {
        .status = TIME_MGMNT_STATUS_ACTIVE,
        .time_reference = time_reference
    };

    return time_mgmnt_istimeup_u8(&context, time_interval);
}

/**
 * @see time_management/time_management.h#time_mgmnt_istimeup_raw_u16
 */
u8 time_mgmnt_istimeup_raw_u16(u16 time_reference, u16 time_interval) {
    
    TIME_MANAGEMENT_U16_TYPE context = {
        .status = TIME_MGMNT_STATUS_ACTIVE,
        .time_reference = time_reference
    };

    return time_mgmnt_istimeup_u16(&context, time_interval);
}

/**
 * @see time_management/time_management.h#time_mgmnt_istimeup_raw_u32
 */
u8 time_mgmnt_istimeup_raw_u32(u32 time_reference, u32 time_interval) {
    
    TIME_MANAGEMENT_U32_TYPE context = {
        .status = TIME_MGMNT_STATUS_ACTIVE,
        .time_reference = time_reference
    };

    return time_mgmnt_istimeup_u32(&context, time_interval);
}

// --------------------------------------------------------------------------------

/**
 * @see time_management/time_management.h#time_mgmnt_elapsed_u8
 */
u8 time_mgmnt_elapsed_u8(TIME_MANAGEMENT_U8_TYPE* p_context) {

    if (p_context->status != TIME_MGMNT_STATUS_ACTIVE) {
        return 0;
    }

    return rtc_timer_gettime_u8() - p_context->time_reference;
}

/**
 * @see time_management/time_management.h#time_mgmnt_elapsed_u16
 */
u16 time_mgmnt_elapsed_u16(TIME_MANAGEMENT_U16_TYPE* p_context) {

    if (p_context->status != TIME_MGMNT_STATUS_ACTIVE) {
        return 0;
    }

    return rtc_timer_gettime_u16() - p_context->time_reference;
}

/**
 * @see time_management/time_management.h#time_mgmnt_elapsed_u32
 */
u32 time_mgmnt_elapsed_u32(TIME_MANAGEMENT_U32_TYPE* p_context) {

    if (p_context->status != TIME_MGMNT_STATUS_ACTIVE) {
        return 0;
    }

    return rtc_timer_gettime_u32() - p_context->time_reference;
}

// --------------------------------------------------------------------------------

/**
 * @see time_management/time_management.h#time_mgmnt_start_time_u8
 */
u8 time_mgmnt_start_time_u8(TIME_MANAGEMENT_U8_TYPE* p_context) {
    return p_context->time_reference;
}

/**
 * @see time_management/time_management.h#time_mgmnt_start_time_u16
 */
u16 time_mgmnt_start_time_u16(TIME_MANAGEMENT_U16_TYPE* p_context) {
    return p_context->time_reference;
}

/**
 * @see time_management/time_management.h#time_mgmnt_start_time_u32
 */
u32 time_mgmnt_start_time_u32(TIME_MANAGEMENT_U32_TYPE* p_context) {
    return p_context->time_reference;
}

// --------------------------------------------------------------------------------
