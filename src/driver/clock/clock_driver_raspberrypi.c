/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	src/driver/clock/clock_driver_raspberrypi.c
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include BOARD_DESCRIPTION_FILE

// --------------------------------------------------------------------------------

#include "driver/rtc/rtc_driver_interface.h"
#include "driver/clock/clock_driver_interface.h"

// --------------------------------------------------------------------------------

void clock_driver_init(void) {
	DEBUG_PASS("clock_driver_init()");
}
