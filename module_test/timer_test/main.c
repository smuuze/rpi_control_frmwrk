/*This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief
 *
 * \author               sebastian lesse
 *
 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "time_management/time_management.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_ON
#include "tracer.h"

//---------- Type Definitions -------------------------------------------------

#define TEST_POWER_UP_TIME_MS		15

#define TEST_TIMEOUT_POWER_UP_MS	50

//---------- Test-Case Prototype ----------------------------------------------



//---------- Static Data ------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(TEST_TIMER)
TIME_MGMN_BUILD_STATIC_TIMER_U8(OUTPUT_TIMER)

//---------- Static Function --------------------------------------------------

static u32 mstime_get_time(void) {
	struct timespec time_spec;

	if (clock_gettime(CLOCK_MONOTONIC, &time_spec) == 0) {
		return ((u32)(time_spec.tv_sec * 1e3) + (u32)(time_spec.tv_nsec / 1e6)); //time_spec.tv_nsec / 1000 / 1000;
	} else {
		return 0;
	}
}

//---------- MAIN --------------------------------------------------------------

int main( void ) {

	DEBUG_PASS("--------------------------------------------------");
	DEBUG_PASS("main() - Hello Module Test - Time-Management");
	DEBUG_PASS("--------------------------------------------------\n");

	TEST_TIMER_start();
	OUTPUT_TIMER_start();

	u8 counter = 0;
	u32 timer_start = mstime_get_time();

	while (TEST_TIMER_is_up(2000) == 0) {
		if (OUTPUT_TIMER_is_up(200)) {
			OUTPUT_TIMER_start();
			counter += 1;
		}
	}

	TEST_TIMER_stop();
	u32 timer_elapsed = mstime_get_time() - timer_start;
	u32 elapsed_timer_time = TEST_TIMER_elapsed();

	DEBUG_TRACE_long(timer_elapsed, "main() - Test finished - Elapsed Time: ");

	
	DEBUG_TRACE_word(TEST_TIMER_start_time(), "main() - Start Time of Timer: ");
	DEBUG_TRACE_word(2000, "main() - Timeout of Timer: ");
	DEBUG_TRACE_word(elapsed_timer_time, "main() - Elapsed Time of Timer: ");

	if (counter != 20) {

		DEBUG_TRACE_byte(counter, "main() - Invalid value for counter - TEST HAS FAILED !!! ---");
	}
	
	return 0;
	
}

//---------- Test-Case Implementation ---------------------------------------------


//---------------------------------------------------------------------------------