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

#define TRACER_OFF

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

// --------------------------------------------------------------------------------
#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

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
