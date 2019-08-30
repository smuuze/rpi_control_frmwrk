
#define TRACER_ON

#include "config.h"
#include "tracer.h"

/*!
 *
 */
u16 math_div_u16(u16 dividend, u16 divisor) {
	u16 result = 0;

	while (dividend > divisor) {
		result += 1;
		dividend -= divisor;
	}

	return result;
}
