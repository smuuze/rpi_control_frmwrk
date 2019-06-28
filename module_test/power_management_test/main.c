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

#include "power_management/power_management.h"
#include "time_management/time_management.h"
#include "io_management/io_controller.h"
#include "driver/gpio/gpio_interface.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_ON
#include "tracer.h"

//---------- Type Definitions -------------------------------------------------

#define TEST_POWER_UP_TIME_MS		15

#define TEST_TIMEOUT_POWER_UP_MS	50

//---------- Test-Case Prototype ----------------------------------------------

u8 module_test_power_management_test_two_times_request_and_release(void);

u8 module_test_power_management_test_more_release_than_request(void);

//---------- Static Data ------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(TIMEOUT_TIMER)

IO_CONTROLLER_BUILD_INOUT(EXT_POWER_5V, EXT_POWER_01)

POWER_MGMN_BUILD_UNIT(DUT, TEST_POWER_UP_TIME_MS, EXT_POWER_5V_drive_high, EXT_POWER_5V_drive_low)

//---------- Function ---------------------------------------------------------

int main( void ) {

	DEBUG_PASS("main() - Hello Module Test - Power-Management-Unit\n");
	
	gpio_driver_init();
	
	//void DUT_init(void)
	//u8 DUT_is_on(void)
	//u8 DUT_request(void)
	//void DUT_release(void)
	
	if (EXT_POWER_5V_is_low_level() == 0) {
		DEBUG_PASS("main() - Stage: INIT - EXT_POWER_5V is not on LOW-LEVEL");
		return 1;
	}
	
	DUT_init();
	
	u8 err = 0;
	u8 test_case = 2;
	
	switch (test_case) {
		case 1 : 
			err = module_test_power_management_test_two_times_request_and_release();
			break;
		case 2 : 
			err = module_test_power_management_test_more_release_than_request();
			break;
	}
	
	if (EXT_POWER_5V_is_low_level() == 0) {
		DEBUG_PASS("main() - Stage: RELEASE2 - EXT_POWER_5V is not on LOW-LEVEL");
		return 1;
	}
	
	if (err) {
		DEBUG_PASS("main() - Stage: FINISH - Test has FAILED !!! ---");
	} else {
		DEBUG_PASS("main() - Stage: FINISH - Test successful completed");	
	}
	
	return err;
	
}

//---------- Test-Case Implementation ---------------------------------------------

u8 module_test_power_management_test_two_times_request_and_release(void) {

	TIMEOUT_TIMER_start();	
	
	DUT_request();	
	while (DUT_is_on() == 0) {
					
		usleep(1000);
	
		if (TIMEOUT_TIMER_is_up(TEST_TIMEOUT_POWER_UP_MS) != 0) {
			DEBUG_PASS("main() - Stage: RAMP-UP - Timeout on waiting for power-up\n");
			return 1;
		}
	}
	
	if (TIMEOUT_TIMER_elapsed() < TEST_POWER_UP_TIME_MS) {
		DEBUG_TRACE_byte(TIMEOUT_TIMER_elapsed(), "main() - Stage: POWER-UP - Power-up time to short");
		return 1;
	}
	
	if (EXT_POWER_5V_is_high_level() == 0) {
		DEBUG_PASS("main() - Stage: INIT - EXT_POWER_5V is not on HIGH-LEVEL");
		return 1;
	}
	
	TIMEOUT_TIMER_start();	
	
	DUT_request();	
	while (DUT_is_on() == 0) {
					
		usleep(1000);
	
		if (TIMEOUT_TIMER_is_up(TEST_TIMEOUT_POWER_UP_MS) != 0) {
			DEBUG_PASS("main() - Timeout on waiting for power-up\n");
			return 0;
		}
	}	
	
	if (TIMEOUT_TIMER_elapsed() > 1) {
		DEBUG_TRACE_byte(TIMEOUT_TIMER_elapsed(), "main() - Stage: POWER-ON - Power-up time to long");
		return 1;
	}
	
	if (EXT_POWER_5V_is_high_level() == 0) {
		DEBUG_PASS("main() - Stage: POWER-ON - EXT_POWER_5V is not on HIGH-LEVEL");
		return 1;
	}
	
	DUT_release();
	
	if (EXT_POWER_5V_is_high_level() == 0) {
		DEBUG_PASS("main() - Stage: RELEASE1 - EXT_POWER_5V is not on HIGH-LEVEL");
		return 1;
	}
	
	DUT_release();	
		
	return 0;
}

u8 module_test_power_management_test_more_release_than_request(void) {
	
	TIMEOUT_TIMER_start();	
	
	DUT_request();	
	while (DUT_is_on() == 0) {
					
		usleep(1000);
	
		if (TIMEOUT_TIMER_is_up(TEST_TIMEOUT_POWER_UP_MS) != 0) {
			DEBUG_PASS("main() - Stage: RAMP-UP - Timeout on waiting for power-up\n");
			return 1;
		}
	}
	
	if (TIMEOUT_TIMER_elapsed() < TEST_POWER_UP_TIME_MS) {
		DEBUG_TRACE_byte(TIMEOUT_TIMER_elapsed(), "main() - Stage: POWER-UP - Power-up time to short");
		return 1;
	}
	
	if (EXT_POWER_5V_is_high_level() == 0) {
		DEBUG_PASS("main() - Stage: INIT - EXT_POWER_5V is not on HIGH-LEVEL");
		return 1;
	}
	
	u8 i = 0;
	for ( ; i < 20; i++) {		
		DUT_release();
	}
	
	TIMEOUT_TIMER_start();	
	
	DUT_request();	
	while (DUT_is_on() == 0) {
					
		usleep(1000);
	
		if (TIMEOUT_TIMER_is_up(TEST_TIMEOUT_POWER_UP_MS) != 0) {
			DEBUG_PASS("main() - Stage: RAMP-UP - Timeout on waiting for power-up\n");
			return 1;
		}
	}
	
	if (TIMEOUT_TIMER_elapsed() < TEST_POWER_UP_TIME_MS) {
		DEBUG_TRACE_byte(TIMEOUT_TIMER_elapsed(), "main() - Stage: POWER-UP - Power-up time to short");
		return 1;
	}
	
	if (EXT_POWER_5V_is_high_level() == 0) {
		DEBUG_PASS("main() - Stage: INIT - EXT_POWER_5V is not on HIGH-LEVEL");
		return 1;
	}
	
	DUT_release();
	
	return 0;
}