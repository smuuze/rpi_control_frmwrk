
#define TRACER_ON

#include "config.h"
#include "tracer.h"

/*!
 *
 */
u32 math_div_u32(u32 dividend, u32 divisor) {
	u32 result = 0;

	while (dividend >= divisor) {
		result += 1;
		dividend -= divisor;
	}

	return result;
}
