/*! 
 * --------------------------------------------------------------------------------
 *
 * 	@file		time_management.c
 * 	@author		sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "driver/rtc/rtc_driver_interface.h"
#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

u8 time_mgmnt_gettime_u8(void) {
	return local_rtc_timer_gettime_u8();
}

u16 time_mgmnt_gettime_u16(void) {
	return local_rtc_timer_gettime_u16();
}

u32 time_mgmnt_gettime_u32(void) {
	return local_rtc_timer_gettime_u32();
}

// --------------------------------------------------------------------------------

void time_mgmnt_start_u8(TIME_MANAGEMENT_U8_TYPE* p_context) {
	p_context->time_reference = local_rtc_timer_gettime_u8();
	p_context->status = TIME_MGMNT_STATUS_ACTIVE;
}

void time_mgmnt_start_u16(TIME_MANAGEMENT_U16_TYPE* p_context) {
	p_context->time_reference = local_rtc_timer_gettime_u16();
	p_context->status = TIME_MGMNT_STATUS_ACTIVE;
}

void time_mgmnt_start_u32(TIME_MANAGEMENT_U32_TYPE* p_context) {
	p_context->time_reference = local_rtc_timer_gettime_u32();
	p_context->status = TIME_MGMNT_STATUS_ACTIVE;
}

// --------------------------------------------------------------------------------

void time_mgmnt_stop_u8(TIME_MANAGEMENT_U8_TYPE* p_context) {
	p_context->time_reference = 0;
	p_context->status = 0;
}

void time_mgmnt_stop_u16(TIME_MANAGEMENT_U16_TYPE* p_context) {
	p_context->time_reference = 0;
	p_context->status = 0;
}

void time_mgmnt_stop_u32(TIME_MANAGEMENT_U32_TYPE* p_context) {
	p_context->time_reference = 0;
	p_context->status = 0;
}

// --------------------------------------------------------------------------------

u8 time_mgmnt_is_active_u8(TIME_MANAGEMENT_U8_TYPE* p_context) {
	return p_context->status == TIME_MGMNT_STATUS_ACTIVE ? 1 : 0;
}

u8 time_mgmnt_is_active_u16(TIME_MANAGEMENT_U16_TYPE* p_context) {
	return p_context->status == TIME_MGMNT_STATUS_ACTIVE ? 1 : 0;
}

u8 time_mgmnt_is_active_u32(TIME_MANAGEMENT_U32_TYPE* p_context) {
	return p_context->status == TIME_MGMNT_STATUS_ACTIVE ? 1 : 0;
}

// --------------------------------------------------------------------------------

u8 time_mgmnt_istimeup_u8(TIME_MANAGEMENT_U8_TYPE* p_context, u8 time_interval) {
	return (time_mgmnt_elapsed_u8(p_context) >= time_interval) ? 1 : 0;
}

u8 time_mgmnt_istimeup_u16(TIME_MANAGEMENT_U16_TYPE* p_context, u16 time_interval) {
	return time_mgmnt_elapsed_u16(p_context) >= time_interval ? 1 : 0;
}

u8 time_mgmnt_istimeup_u32(TIME_MANAGEMENT_U32_TYPE* p_context, u32 time_interval) {
	return time_mgmnt_elapsed_u32(p_context) >= time_interval ? 1 : 0;
}

// --------------------------------------------------------------------------------

u8 time_mgmnt_istimeup_raw_u8(u8 time_reference, u8 time_interval) {

	TIME_MANAGEMENT_U8_TYPE context = {
		.status = TIME_MGMNT_STATUS_ACTIVE,
		.time_reference = time_reference
	};

	return time_mgmnt_istimeup_u8(&context, time_interval);
}

u8 time_mgmnt_istimeup_raw_u16(u16 time_reference, u16 time_interval) {
	
	TIME_MANAGEMENT_U16_TYPE context = {
		.status = TIME_MGMNT_STATUS_ACTIVE,
		.time_reference = time_reference
	};

	return time_mgmnt_istimeup_u16(&context, time_interval);
}

u8 time_mgmnt_istimeup_raw_u32(u32 time_reference, u32 time_interval) {
	
	TIME_MANAGEMENT_U32_TYPE context = {
		.status = TIME_MGMNT_STATUS_ACTIVE,
		.time_reference = time_reference
	};

	return time_mgmnt_istimeup_u32(&context, time_interval);
}

// --------------------------------------------------------------------------------

u8 time_mgmnt_elapsed_u8(TIME_MANAGEMENT_U8_TYPE* p_context) {

	if (p_context->status != TIME_MGMNT_STATUS_ACTIVE) {
		return 0;
	}

	return local_rtc_timer_gettime_u8() - p_context->time_reference;
}

u16 time_mgmnt_elapsed_u16(TIME_MANAGEMENT_U16_TYPE* p_context) {

	if (p_context->status != TIME_MGMNT_STATUS_ACTIVE) {
		return 0;
	}

	return local_rtc_timer_gettime_u16() - p_context->time_reference;
}

u32 time_mgmnt_elapsed_u32(TIME_MANAGEMENT_U32_TYPE* p_context) {

	if (p_context->status != TIME_MGMNT_STATUS_ACTIVE) {
		return 0;
	}

	return local_rtc_timer_gettime_u32() - p_context->time_reference;
}

// --------------------------------------------------------------------------------

u8 time_mgmnt_start_time_u8(TIME_MANAGEMENT_U8_TYPE* p_context) {
	return p_context->time_reference;
}

u16 time_mgmnt_start_time_u16(TIME_MANAGEMENT_U16_TYPE* p_context) {
	return p_context->time_reference;
}

u32 time_mgmnt_start_time_u32(TIME_MANAGEMENT_U32_TYPE* p_context) {
	return p_context->time_reference;
}
