/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "utils/stdmacros.h"
#include BOARD_DESCRIPTION_FILE

#include "local_rtc_driver.h"

//---------- Implementation of Traces -----------------------------------------

//#define TRACES
//#include <traces.h>

#define TRACER_ON
#include "tracer.h"

#define CLK_DRIVER_CLK_PRESCALER_1		0x00
#define CLK_DRIVER_CLK_PRESCALER_2		0x01
#define CLK_DRIVER_CLK_PRESCALER_4		0x02
#define CLK_DRIVER_CLK_PRESCALER_8		0x03
#define CLK_DRIVER_CLK_PRESCALER_16		0x04
#define CLK_DRIVER_CLK_PRESCALER_32		0x05
#define CLK_DRIVER_CLK_PRESCALER_64		0x06
#define CLK_DRIVER_CLK_PRESCALER_128		0x07
#define CLK_DRIVER_CLK_PRESCALER_256		0x08


/*
 *	To avoid unintentional changes of clock frequency,
 *	a special write procedure must be followed to change the CLKPS bits:
 *
 *	1.	Write the Clock Prescaler Change Enable (CLKPCE)
 *		bit to one and all other bits in CLKPR to zero.
 *
 *	2.	Within four cycles, write the desired value to CLKPS while
 *		writing a zero to CLKPCE. Interrupts must be disabled when
 *		changing prescaler setting to make sure the write procedure
 *		is not interrupted.
 */
#define CLK_DRIVER_SET_PRESCALER(prescaler)	cli();			\
						CLKPR = (1 << CLKPCE);	\
						CLKPR = prescaler;	\
						sei()


void local_clk_driver_init(void) {

	#ifdef SYS_CRYSTAL_FREQ
	{
		TRACE_byte(SYS_CPU_CLK_PRESCALER); // local_clk_driver_init() - We are using a external crystal oszillator with prescaler

		u8 clk_division_factor = 0;

		switch (SYS_CPU_CLK_PRESCALER) {
			case 1:
				clk_division_factor = CLK_DRIVER_CLK_PRESCALER_1;
				break;
			case 2:
				clk_division_factor = CLK_DRIVER_CLK_PRESCALER_2;
				break;
			case 4:
				clk_division_factor = CLK_DRIVER_CLK_PRESCALER_4;
				break;
			case 8:
				clk_division_factor = CLK_DRIVER_CLK_PRESCALER_8;
				break;
			case 16:
				clk_division_factor = CLK_DRIVER_CLK_PRESCALER_16;
				break;
			case 32:
				clk_division_factor = CLK_DRIVER_CLK_PRESCALER_32;
				break;
			default:
				clk_division_factor = CLK_DRIVER_CLK_PRESCALER_64;
				break;
		}

		CLK_DRIVER_SET_PRESCALER(clk_division_factor);
	}
	#endif
}
