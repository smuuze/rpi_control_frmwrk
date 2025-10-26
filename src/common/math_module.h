/**
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * @file    math_module.h
 * @author  Sebastian Lesse
 * @date    2021 / 02 / 07
 * @brief   Short description of this file
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_math_module_
#define _H_math_module_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

/**
 * @brief Performs a unsigned devision
 * If the divisor is equal to 0, this function returns 0.
 * 
 * @param dividend 
 * @param divisor 
 * @return u32 
 */
u32 math_div_u32(u32 dividend, u32 divisor);

/**
 * @brief Performs a signed division
 * If the divisor is equal to 0, this function returns 0.
 * 
 * @param dividend 
 * @param divisor 
 * @return i32 
 */
i32 math_div_i32(u32 dividend, u32 divisor);

/**
 * @brief Gets the absolute value.
 * 
 * @param value decimal number, can be positive or negative
 * @return value if value is >= 0, otherwise returns (value * (-1))
 */
u32 math_get_abs(i32 value);

// --------------------------------------------------------------------------------

#define div_u8(divident, divisor)   (u8) math_div_u32((u32)divident, (u32) divisor)
#define div_u16(divident, divisor)  (u16)math_div_u32((u32)divident, (u32) divisor)
#define div_u32(divident, divisor)  (u32)math_div_u32((u32)divident, (u32) divisor)

#define div_i8(divident, divisor)   (i8) math_div_i32((i32)divident, (i32) divisor)
#define div_i16(divident, divisor)  (u16)math_div_u32((i32)divident, (i32) divisor)
#define div_i32(divident, divisor)  (i32)math_div_u32((i32)divident, (i32) divisor)

// --------------------------------------------------------------------------------

#endif // _H_math_module_

// --------------------------------------------------------------------------------
