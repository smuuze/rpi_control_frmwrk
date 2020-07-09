/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	common/local_data_storage_array.c
 * \author	sebastian lesse
 * \brief
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "common/local_data_storage_array.h"
#include "common/math_module.h"

// --------------------------------------------------------------------------------

#define div_u32(divident, divisor)		math_div_u32((u32)divident, (u32) divisor)
#define div_u16(divident, divisor)		(u16)math_div_u32((u32)divident, (u32) divisor)
#define div_u8(divident, divisor)		(u8)math_div_u32((u32)divident, (u32) divisor)
#define div_i8(divident, divisor)		(i8)math_div_u32((i32)divident, (i32) divisor)

// -------------------------------------------------------------------
// I8 functions
// -------------------------------------------------------------------

void local_data_storage_array_init_i8(LOCAL_DATA_STORAGE_ARRAY_I8_TYPE* p_context) {

//	u8 i = 0;
//	for ( ; i < p_context->size; i++) {
//		p_context->p_buffer[i] = 0;
//	}

	p_context->write_pointer = 0;
	p_context->array_not_full = 1;
}

void local_data_storage_array_add_value_i8(LOCAL_DATA_STORAGE_ARRAY_I8_TYPE* p_context, i8 value) {

	if (p_context->write_pointer == p_context->size) {
		p_context->write_pointer = 0;
		p_context->array_not_full = 0;
	}

	p_context->p_buffer[p_context->write_pointer++] = value;
}

i8 local_data_storage_array_get_mean_i8(LOCAL_DATA_STORAGE_ARRAY_I8_TYPE* p_context) {

	i16 mean = 0;
	u8 i = 0;
	u8 len =  p_context->array_not_full != 0 ? p_context->write_pointer : p_context->size;

	for ( ; i < len; i++) {
		mean += (i16)p_context->p_buffer[i];
	}

	mean = div_i8(mean, len);
	return (i8)mean;
}

i8 local_data_storage_array_get_max_i8(LOCAL_DATA_STORAGE_ARRAY_I8_TYPE* p_context) {

	i8 max = 0;
	u8 i = 0;
	u8 len =  p_context->array_not_full != 0 ? p_context->write_pointer : p_context->size;

	for ( ; i < len; i++) {
		if (max < p_context->p_buffer[i]) {
			max = p_context->p_buffer[i] ;
		}
	}

	return max;
}

i8 local_data_storage_array_get_min_i8(LOCAL_DATA_STORAGE_ARRAY_I8_TYPE* p_context) {

	i8 min = DATA_STORAGE_ARRAY_MAX_VALUE_I8;
	u8 i = 0;
	u8 len =  p_context->array_not_full != 0 ? p_context->write_pointer : p_context->size;

	for ( ; i < len; i++) {
		if (min > p_context->p_buffer[i]) {
			min = p_context->p_buffer[i];
		}
	}

	return min;
}

// -------------------------------------------------------------------
// U8 functions
// -------------------------------------------------------------------

void local_data_storage_array_init_u8(LOCAL_DATA_STORAGE_ARRAY_U8_TYPE* p_context) {

	p_context->write_pointer = 0;
	p_context->array_not_full = 1;
}

void local_data_storage_array_add_value_u8(LOCAL_DATA_STORAGE_ARRAY_U8_TYPE* p_context, u8 value) {

	if (p_context->write_pointer == p_context->size) {
		p_context->write_pointer = 0;
		p_context->array_not_full = 0;
	}

	p_context->p_buffer[p_context->write_pointer++] = value;
}

u8 local_data_storage_array_get_mean_u8(LOCAL_DATA_STORAGE_ARRAY_U8_TYPE* p_context) {

	u16 mean = 0;
	u8 i = 0;
	u8 len =  p_context->array_not_full != 0 ? p_context->write_pointer : p_context->size;

	for ( ; i < len; i++) {
		mean += (u16)p_context->p_buffer[i];
	}

	return div_u8(mean, len);
}

u8 local_data_storage_array_get_max_u8(LOCAL_DATA_STORAGE_ARRAY_U8_TYPE* p_context) {

	u8 max = DATA_STORAGE_ARRAY_MIN_VALUE_U8;
	u8 i = 0;
	u8 len =  p_context->array_not_full != 0 ? p_context->write_pointer : p_context->size;

	for ( ; i < len; i++) {
		if (max < p_context->p_buffer[i]) {
			max = p_context->p_buffer[i] ;
		}
	}

	return max;
}

u8 local_data_storage_array_get_min_u8(LOCAL_DATA_STORAGE_ARRAY_U8_TYPE* p_context) {

	u8 min = DATA_STORAGE_ARRAY_MAX_VALUE_U8;
	u8 i = 0;
	u8 len =  p_context->array_not_full != 0 ? p_context->write_pointer : p_context->size;

	for ( ; i < len; i++) {
		if (min > p_context->p_buffer[i]) {
			min = p_context->p_buffer[i];
		}
	}

	return min;
}

void local_data_storage_array_debug_print_data_u8(LOCAL_DATA_STORAGE_ARRAY_U8_TYPE* p_context) {
	DEBUG_TRACE_N(p_context->write_pointer, p_context->p_buffer, "local_data_storage_array_debug_print_data_u8");
}

// -------------------------------------------------------------------
// U16 functions
// -------------------------------------------------------------------

void local_data_storage_array_init_u16(LOCAL_DATA_STORAGE_ARRAY_U16_TYPE* p_context) {

	p_context->write_pointer = 0;
	p_context->array_not_full = 1;
}

void local_data_storage_array_add_value_u16(LOCAL_DATA_STORAGE_ARRAY_U16_TYPE* p_context, u16 value) {

	if (p_context->write_pointer == p_context->size) {
		p_context->write_pointer = 0;
		p_context->array_not_full = 0;
	}

	p_context->p_buffer[p_context->write_pointer++] = value;
}

u16 local_data_storage_array_get_mean_u16(LOCAL_DATA_STORAGE_ARRAY_U16_TYPE* p_context) {

	u32 mean = 0;
	u8 i = 0;
	u8 len =  p_context->array_not_full != 0 ? p_context->write_pointer : p_context->size;

	for ( ; i < len; i++) {
		mean += (u32)p_context->p_buffer[i];
	}

	return div_u16(mean, len);
}

u16 local_data_storage_array_get_max_u16(LOCAL_DATA_STORAGE_ARRAY_U16_TYPE* p_context) {

	u16 max = 0;
	u8 i = 0;
	u8 len =  p_context->array_not_full != 0 ? p_context->write_pointer : p_context->size;

	for ( ; i < len; i++) {
		if (max < p_context->p_buffer[i]) {
			max = p_context->p_buffer[i] ;
		}
	}

	return max;
}

void local_data_storage_array_init_u32(LOCAL_DATA_STORAGE_ARRAY_U32_TYPE* p_context) {

//	u8 i = 0;
//	for ( ; i < p_context->size; i++) {
//		p_context->p_buffer[i] = 0;
//	}

	p_context->write_pointer = 0;
	p_context->array_not_full = 1;
}

// -------------------------------------------------------------------
// U32 functions
// -------------------------------------------------------------------

void local_data_storage_array_add_value_u32(LOCAL_DATA_STORAGE_ARRAY_U32_TYPE* p_context, u32 value) {

	if (p_context->write_pointer == p_context->size) {
		p_context->write_pointer = 0;
		p_context->array_not_full = 0;
	}

	p_context->p_buffer[p_context->write_pointer++] = value;
}

u32 local_data_storage_array_get_mean_u32(LOCAL_DATA_STORAGE_ARRAY_U32_TYPE* p_context) {

	u32 mean = 0;
	u8 i = 0;
	u8 len =  p_context->array_not_full != 0 ? p_context->write_pointer : p_context->size;

	for ( ; i < len; i++) {
		mean += p_context->p_buffer[i];
	}


	return div_u32(mean, len);
}

u32 local_data_storage_array_get_max_u32(LOCAL_DATA_STORAGE_ARRAY_U32_TYPE* p_context) {

	u32 max = 0;
	u8 i = 0;
	u8 len =  p_context->array_not_full != 0 ? p_context->write_pointer : p_context->size;

	for ( ; i < len; i++) {
		if (max < p_context->p_buffer[i]) {
			max = p_context->p_buffer[i] ;
		}
	}

	return max;
}
