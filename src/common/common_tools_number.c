/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	commn_tools_number.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

u32 common_tools_number_u32_from_array(const u8* p_array) {

	u32 number  = (u32)p_array[3] << 24;
	    number += (u32)p_array[2] << 16;
	    number += (u32)p_array[1] << 8;
	    number += (u32)p_array[0] << 0;

	return number;
}

u16 common_tools_number_u16_from_array(const u8* p_array) {

	u16 number  = (u16)p_array[1] << 8;
	    number += (u16)p_array[0] << 0;

	return number;
}

i32 common_tools_number_i32_from_array(const u8* p_array) {

	i32 number  = (i32)p_array[3] << 24;
	    number += (i32)p_array[2] << 16;
	    number += (i32)p_array[1] << 8;
	    number += (i32)p_array[0] << 0;

	return number;
}

i16 common_tools_number_i16_from_array(const u8* p_array) {

	i16 number  = p_array[1] << 8;
	    number += p_array[0] << 0;

	return number;
}

void common_tools_number_u32_to_array(u8* p_array, u32 number) {
	
	p_array[3] = (u8)(number >> 24);
	p_array[2] = (u8)(number >> 16);
	p_array[1] = (u8)(number >> 8);
	p_array[0] = (u8)(number >> 0);
}

void common_tools_number_u16_to_array(u8* p_array, u16 number) {
	
	p_array[1] = (u8)(number >> 8);
	p_array[0] = (u8)(number >> 0);
}

i32 common_tools_number_from_u8(u8 number, u8 is_signed) {

	if (is_signed) {
		return (i32)(i8)number;
	} else {
		return (i32)number;
	}
}

// --------------------------------------------------------------------------------
