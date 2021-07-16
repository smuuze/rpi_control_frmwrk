/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	power_management_initialization.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_ON

#ifdef TRACER_ON
#pragma __WARNING__TRACES_ENABLED__
#endif

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!

//-----------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "power_management/power_management_interface.h"
#include "power_management/power_module_5V.h"

//-----------------------------------------------------------------------------

void power_management_initialization(void) {
		
	DEBUG_PASS("power_management_initialization() - START");

	power_mgmnt_init();

	#ifdef HAS_POWER_MANAGEMENT_MODULE_5V
	{	
		DEBUG_PASS("power_management_initialization() - POWER_UNIT_5V_init()");
		POWER_UNIT_5V_init();
	}
	#endif

	DEBUG_PASS("power_management_initialization() - DONW");
}
