/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	common_tools_bit_vector.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef COMMON_TOOLS_BIT_VECTOR_H_
#define COMMON_TOOLS_BIT_VECTOR_H_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

/**
 * @brief Creates a bit-vector with a length of 8 bits.
 * If this macro is used on a target variable with a higher bit-length.
 * the higher bits will be set to zero.
 * 
 * Usage:
 * 
 * - create the bit-vector:
 * 
 * 	u8 bit_vector_0x01 = BUILD_BIT_VECTOR_U8(0,0,0,0,0,0,0,1);
 * 	u8 bit_vector_0x80 = BUILD_BIT_VECTOR_U8(1,0,0,0,0,0,0,0);
 * 	u8 bit_vector_0xFF = BUILD_BIT_VECTOR_U8(1,1,1,1,1,1,1,1);
 * 
 */
#define BUILD_BIT_VECTOR_U8(a,b,c,d,e,f,g,h)		(u8)			\
							(			\
								  (a << 7)	\
								| (b << 6)	\
								| (c << 5)	\
								| (d << 4)	\
								| (e << 3)	\
								| (f << 2)	\
								| (g << 1)	\
								| (h << 0)	\
							)

// --------------------------------------------------------------------------------

#endif // COMMON_TOOLS_BIT_VECTOR_H_
