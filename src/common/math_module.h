#ifndef _MATH_MODULE_H_
#define _MATH_MODULE_H_

/**
 * @file math_module.h
 * @author sebastian lesse (sebastian lesse)
 * @brief 
 * @version 1.0
 * @date 2021-02-07
 * 
 * @copyright Copyright (c) 2021
 * 
 */

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

/**
 * @brief 	Performs a unsigned devision
 * 		If the divisor is equal to 0, this function returns 0.
 * 
 * @param dividend 
 * @param divisor 
 * @return u32 
 */
u32 math_div_u32(u32 dividend, u32 divisor);

/**
 * @brief 	Performs a signed division
 * 		If the divisor is equal to 0, this function returns 0.
 * 
 * @param dividend 
 * @param divisor 
 * @return i32 
 */
i32 math_div_i32(u32 dividend, u32 divisor);

// --------------------------------------------------------------------------------

#define div_u8(divident, divisor)		(u8) math_div_u32((u32)divident, (u32) divisor)
#define div_u16(divident, divisor)		(u16)math_div_u32((u32)divident, (u32) divisor)
#define div_u32(divident, divisor)		(u32)math_div_u32((u32)divident, (u32) divisor)

#define div_i8(divident, divisor)		(i8) math_div_i32((i32)divident, (i32) divisor)
#define div_i16(divident, divisor)		(u16)math_div_u32((i32)divident, (i32) divisor)
#define div_i32(divident, divisor)		(i32)math_div_u32((i32)divident, (i32) divisor)

// --------------------------------------------------------------------------------

#endif // _MATH_MODULE_H_
