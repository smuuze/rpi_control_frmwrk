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
 * @file    math_module.c
 * @author  Sebastian Lesse
 * @date    2021 / 02 / 07
 * @brief   Short description of this file
 * 
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "cpu.h"

// --------------------------------------------------------------------------------

/**
 * @see math_module.h
 */
u32 math_div_u32(u32 dividend, u32 divisor) {

    if (divisor == 0) {
        return 0;
    }

    u32 result = 0;

    while (dividend >= divisor) {
        result += 1;
        dividend -= divisor;
    }

    return result;
}

// --------------------------------------------------------------------------------

/**
 * @see math_module.h
 */
i32 math_div_i32(i32 dividend, i32 divisor) {

    u8 dividend_is_negative = (dividend < 0) ? 1 : 0;
    u8 divisor_is_negative = (divisor < 0) ? 1 : 0;

    if (dividend_is_negative && divisor_is_negative) {

        // both values are negative, so result is also positive
        return (i32)math_div_u32((u32)(dividend * (-1)), (u32)(divisor * (-1)));

    } if (dividend_is_negative) {

        // only dividend is negative, so result is negative
        return (i32)math_div_u32((u32)(dividend * (-1)), (u32)(divisor)) * (-1);

    } if (divisor_is_negative) {

        // only divisor is negative, so result is negative
        return (i32)math_div_u32((u32)(dividend), (u32)(divisor * (-1))) * (-1);

    }  else {

        // both values are positive, so result is also positive
        return (i32)math_div_u32((u32)dividend, (u32)divisor);
    }
}

// --------------------------------------------------------------------------------

/**
 * @see math_module.h
 */
u32 math_get_abs(i32 value) {
    if (value < 0) {
        return -value;
    } else {
        return value;
    }
}

// --------------------------------------------------------------------------------
