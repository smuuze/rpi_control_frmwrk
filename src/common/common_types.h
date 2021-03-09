/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	common_types.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef _COMMON_TYPES_H_
#define _COMMON_TYPES_H_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#ifndef COMMON_TYPES_GENERIC_BUFFER_SIZE
#define COMMON_TYPES_GENERIC_BUFFER_SIZE	128
#endif

// --------------------------------------------------------------------------------

typedef struct {
	u16 length;
	u8 data[COMMON_TYPES_GENERIC_BUFFER_SIZE];
} COMMON_GENERIC_BUFFER_TYPE;

// --------------------------------------------------------------------------------

#endif // _COMMON_TYPES_H_
