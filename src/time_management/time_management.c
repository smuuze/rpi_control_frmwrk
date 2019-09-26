/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "system_interface.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_OFF
#include "tracer.h"

//-----------------------------------------------------------------------------

#include "time_management.h"

u8 time_mgmnt_gettime_u8(void) {
	return i_system.time.now_u8();
}

u16 time_mgmnt_gettime_u16(void) {
	return i_system.time.now_u16();
}

u32 time_mgmnt_gettime_u32(void) {
	return i_system.time.now_u32();
}

u8 time_mgmnt_istimeup_u8(u8 time_reference, u8 time_interval) {
	return time_mgmnt_elapsed_u8(time_reference) < time_interval ? 0 : 1;
}

u8 time_mgmnt_istimeup_u16(u16 time_reference, u16 time_interval) {
	return time_mgmnt_elapsed_u16(time_reference) < time_interval ? 0 : 1;
}

u8 time_mgmnt_istimeup_u32(u32 time_reference, u32 time_interval) {
	return time_mgmnt_elapsed_u32(time_reference) < time_interval ? 0 : 1;
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

