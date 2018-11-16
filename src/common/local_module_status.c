
#include "config.h"
#include "local_module_status.h"

void module_status_set_u8(MODULE_STATUS_U8_TYPE* p_status, u8 bit_flag) {
	p_status->data |= bit_flag; 	
}

u8 module_status_isset_u8(MODULE_STATUS_U8_TYPE* p_status, u8 bit_flag) {
	return (p_status->data & bit_flag) != 0 ? 1 : 0;
}

void module_status_unset_u8(MODULE_STATUS_U8_TYPE* p_status, u8 bit_flag) {
	p_status->data &= ~(bit_flag);
}

void module_status_clear_all_u8(MODULE_STATUS_U8_TYPE* p_status) {
	p_status->data = 0;
}



void module_status_set_u16(MODULE_STATUS_U16_TYPE* p_status, u16 bit_flag) {
	p_status->data |= bit_flag; 	
}

u16 module_status_isset_u16(MODULE_STATUS_U16_TYPE* p_status, u16 bit_flag) {
	return (p_status->data & bit_flag) != 0 ? 1 : 0;
}

void module_status_unset_u16(MODULE_STATUS_U16_TYPE* p_status, u16 bit_flag) {
	p_status->data &= ~(bit_flag);
}

void module_status_clear_all_u16(MODULE_STATUS_U16_TYPE* p_status) {
	p_status->data = 0;
}



void module_status_set_u32(MODULE_STATUS_U32_TYPE* p_status, u32 bit_flag) {
	p_status->data |= bit_flag; 	
}

u32 module_status_isset_u32(MODULE_STATUS_U32_TYPE* p_status, u32 bit_flag) {
	return (p_status->data & bit_flag) != 0 ? 1 : 0;
}

void module_status_unset_u32(MODULE_STATUS_U32_TYPE* p_status, u32 bit_flag) {
	p_status->data &= ~(bit_flag);
}

void module_status_clear_all_u32(MODULE_STATUS_U32_TYPE* p_status) { 
	p_status->data = 0;
}