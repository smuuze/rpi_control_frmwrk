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


#include "driver/cfg_driver_interface.h"
#include "driver/trx_driver_interface.h"

#include "driver/gpio/gpio_interface.h"

#include "time_management/time_management.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_ON
#include "tracer.h"

//---------- Type Definitions -------------------------------------------------


//---------- Test-Case Prototype ----------------------------------------------


//---------- Static Data ------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(run_interval_timer)


//---------- Function ---------------------------------------------------------

int main( void ) {

	DEBUG_PASS("main() - Hello Module Test - RPi-Protocol-Handler");
	
	gpio_driver_init();

	return 0;
}

//---------- Dummy Implementation ---------------------------------------------
