/*! \file *********************************************************************

 *****************************************************************************/

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

//-----------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "driver/timer/timer0_driver.h"

//-----------------------------------------------------------------------------

#define TIMER0_TCCR0A_COM0A1						(1 << 7)
#define TIMER0_TCCR0A_COM0A0						(1 << 6)

#define TIMER0_TCCR0B_CS02						(1 << 2)
#define TIMER0_TCCR0B_CS01						(1 << 1)
#define TIMER0_TCCR0B_CS00						(1 << 0)

#define TIMER0_OC0A_ON_COMPARE_MATCH_SET				(TIMER0_TCCR0A_COM0A1 | TIMER0_TCCR0A_COM0A0)
#define TIMER0_OC0A_ON_COMPARE_MATCH_CLEAR				TIMER0_TCCR0A_COM0A1
#define TIMER0_OC0A_ON_COMPARE_MATCH_TOGGLE				TIMER0_TCCR0A_COM0A0

#define TIMER0_CLOCK_SOURCE_NONE					(0)
#define TIMER0_CLOCK_SOURCE_CLK_IO					TIMER0_TCCR0B_CS00
#define TIMER0_CLOCK_SOURCE_CLK_IO_BY_8					TIMER0_TCCR0B_CS01
#define TIMER0_CLOCK_SOURCE_CLK_IO_BY_64				(TIMER0_TCCR0B_CS00 | TIMER0_TCCR0B_CS01)
#define TIMER0_CLOCK_SOURCE_CLK_IO_BY_256				TIMER0_TCCR0B_CS02
#define TIMER0_CLOCK_SOURCE_CLK_IO_BY_1024				(TIMER0_TCCR0B_CS02 | TIMER0_TCCR0B_CS00)
#define TIMER0_CLOCK_SOURCE_CLK_T0_FALLING_EDGE				(TIMER0_TCCR0B_CS02 | TIMER0_TCCR0B_CS01)
#define TIMER0_CLOCK_SOURCE_CLK_T0_RISING_EDGE				(TIMER0_TCCR0B_CS02 | TIMER0_TCCR0B_CS01 | TIMER0_TCCR0B_CS00)

//-----------------------------------------------------------------------------

static u8 TCCR0A_backup = 0;
static u8 TCCR0B_backup = 0;
static u8 OCR0A_backup = 0;

void timer0_driver_init(void) {
	TCCR0A_backup = TIMER0_OC0A_ON_COMPARE_MATCH_TOGGLE;
	TCCR0B_backup = TIMER0_CLOCK_SOURCE_CLK_IO_BY_64;
	OCR0A_backup = 100;
}

void timer0_driver_deinit(void) {

}

void timer0_driver_configure(TIMER0_DRIVER_FREQUENCY_TYPE frequency) {

}

void timer0_driver_start(void) {
	TCCR0A = TCCR0A_backup;
	TCCR0B = TCCR0B_backup;
	OCR0A  = OCR0A_backup;
}

void timer0_driver_stop(void) {
	TCCR0A = 0;
	TCCR0B = 0;
	OCR0A  = 0;
}