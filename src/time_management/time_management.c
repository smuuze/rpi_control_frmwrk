/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "utils/stdmacros.h"

#include "system_interface.h"

#define noTRACES
#include <traces.h>

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
	return ((time_mgmnt_gettime_u8() - time_reference) > time_interval) ? 1 : 0;
}

u8 time_mgmnt_istimeup_u16(u16 time_reference, u16 time_interval) {
	return time_mgmnt_gettime_u16() - time_reference > time_interval ? 1 : 0;
}

u8 time_mgmnt_istimeup_u32(u32 time_reference, u32 time_interval) {
	return time_mgmnt_gettime_u32() - time_reference > time_interval ? 1 : 0;
}

u8 time_mgmnt_elapsed_u8(u8 time_reference) {
	return time_mgmnt_gettime_u8() - time_reference;
}

u8 time_mgmnt_elapsed_u16(u16 time_reference) {
	return time_mgmnt_gettime_u16() - time_reference;
}

u8 time_mgmnt_elapsed_u32(u32 time_reference) {
	return time_mgmnt_gettime_u32() - time_reference;
}

