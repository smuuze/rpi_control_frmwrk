/*! 
 * --------------------------------------------------------------------------------
 *
 * 	@file		main_rpi_hat.c
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

u8 time_mgmnt_istimeup_u8(u8 time_reference, u8 time_interval) {
	return local_rtc_timer_istimeup_u8(time_reference, time_interval);
}

u8 time_mgmnt_istimeup_u16(u16 time_reference, u16 time_interval) {
	return local_rtc_timer_istimeup_u16(time_reference, time_interval);
}

u8 time_mgmnt_istimeup_u32(u32 time_reference, u32 time_interval) {
	return local_rtc_timer_istimeup_u32(time_reference, time_interval);
}

u8 time_mgmnt_elapsed_u8(u8 time_reference) {
	return rtc_timer_elapsed_u8(time_reference);
}

u16 time_mgmnt_elapsed_u16(u16 time_reference) {
	return rtc_timer_elapsed_u16(time_reference);
}

u32 time_mgmnt_elapsed_u32(u32 time_reference) {
	return rtc_timer_elapsed_u32(time_reference);
}
