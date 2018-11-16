
#include "config.h"
#include "local_data_storage_array.h"

/*!
 *
 * @param divident
 * @param divisor
 * @return
 */
static u32 division(u32 divident, u32 divisor) {
	u32 result = 0;

	while (divident > divisor) {
		divident -= divisor;
		result += 1;
	}

	return result;
}

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

	mean = division(mean, len);
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

	mean = division(mean, len);
	return (u8)mean;
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

	mean = division(mean, len);
	return (u16)mean;
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

	mean = division(mean, len);
	return mean;
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
