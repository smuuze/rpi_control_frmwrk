/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_ON
#include "tracer.h"

//-----------------------------------------------------------------------------

#include "system/system_interface.h"

#include "common/local_context.h"
#include "common/local_data_storage_array.h"
#include "common/math_module.h"

#include "time_management/time_management.h"

//-----------------------------------------------------------------------------

#define GM5528_MAXMIN_INTERVAL_MS			900000	/* every 15 minutes*/
#define GM5528_DATA_ARRAY_NUMBER_OF_ELEMENTS		96 /* last 24 hours */

//-----------------------------------------------------------------------------

BUILD_LOCAL_DATA_STORAGE_ARRAY_U8(GM5528_LIGHT_RESISTOR, GM5528_DATA_ARRAY_NUMBER_OF_ELEMENTS)
TIME_MGMN_BUILD_STATIC_TIMER_U32(GM5528_MAXMIN_TIMER)

//-----------------------------------------------------------------------------

void light_resistor_gm5528_init(void) {

	GET_SYSTEM(data).light.maximal = 0;
	GET_SYSTEM(data).light.minimal = 100;
}

void light_resistor_gm5528_callback(void) {

	DEBUG_PASS("light_resistor_gm5528_callback()");

	if (GM5528_MAXMIN_TIMER_is_active() == 0) {
		GM5528_MAXMIN_TIMER_start();
	}

	GET_SYSTEM(data).light.actual = (u8) math_div_u32((u32)(26403 - GET_SYSTEM(data).adc.channel_3), (u32)262);

	/*
	 * 	0x7FFF		:	INPUT_SIGNAL >= FS * (2^15 - 1) / 2^15
	 * 	0x0001 - 0x7FFE	:	INPUT_SIGNAL  = FS / 2^15
	 * 	0x0000		:	INPUT_SIGNAL  = 0
	 * 
	 * 	FS = 4096
	 * 
	 * 	Resistor-Range	:	50   Ohm	-> very bright
	 * 				1.3 kOhm	-> directly under office light
	 * 				30-40  kOhm	-> no lights in office
	 * 				> 10 MOhm	-> almost perfect dark
	 */ 

	DEBUG_TRACE_word(GET_SYSTEM(data).adc.channel_3, "light_resistor_gm5528_callback() - ADC-Value");
	DEBUG_TRACE_byte(GET_SYSTEM(data).light.actual, "light_resistor_gm5528_callback() - Actual value");

	if (GET_SYSTEM(data).light.minimal > GET_SYSTEM(data).light.actual) {

		GET_SYSTEM(data).light.minimal = GET_SYSTEM(data).light.actual;
		DEBUG_TRACE_byte(GET_SYSTEM(data).light.minimal, "light_resistor_gm5528_callback() - Minimal value");
	}

	if (GET_SYSTEM(data).light.maximal < GET_SYSTEM(data).light.actual) {

		GET_SYSTEM(data).light.maximal = GET_SYSTEM(data).light.actual;
		DEBUG_TRACE_byte(GET_SYSTEM(data).light.maximal, "light_resistor_gm5528_callback() - Maximal value");
	}

	if (GM5528_MAXMIN_TIMER_is_up(GM5528_MAXMIN_INTERVAL_MS)) {
	
		GM5528_MAXMIN_TIMER_start();

		GM5528_LIGHT_RESISTOR_data_storage_array_add_value(GET_SYSTEM(data).light.actual);

		GET_SYSTEM(data).light.maximal = GM5528_LIGHT_RESISTOR_data_storage_array_get_max();
		GET_SYSTEM(data).light.minimal = GM5528_LIGHT_RESISTOR_data_storage_array_get_min();
		GET_SYSTEM(data).light.mean = GM5528_LIGHT_RESISTOR_data_storage_array_get_mean();

		TRACE_byte(GET_SYSTEM(data).light.maximal); // local_sht31_mcu_task_run() - maximum Light in percent
		TRACE_byte(GET_SYSTEM(data).light.minimal); // local_sht31_mcu_task_run() - minimum Light in percent
		TRACE_byte(GET_SYSTEM(data).light.mean); // local_sht31_mcu_task_run() - mean Light in percent
	}
}
