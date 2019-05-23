/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"

#include "system_interface.h"

//---------- Implementation of Traces -----------------------------------------

#define TRACER_OFF
#include "tracer.h"

//-----------------------------------------------------------------------------

#include "power_management/power_management.h"

u8 power_mgmnt_request(POWER_MANAGEMENT_UNIT_TYPE* p_unit) {
	PASS(); // power_mgmnt_release()
	
	if (p_unit->request_counter == 0) {
		PASS(); // power_mgmnt_release() - Going to switch on unit
		
	}
	
	p_unit->request_counter += 1;
}

/*!
 *
 */
void power_mgmnt_release(POWER_MANAGEMENT_UNIT_TYPE* p_unit) {
	
	PASS(); // power_mgmnt_release()
	
	if (p_unit->request_counter == 0) {
		PASS(); // power_mgmnt_release() - Unit was not requested
		
	} else if (p_unit->request_counter == 1) {
		PASS(); // power_mgmnt_release() - The is the last release
	}
	
	p_unit->request_counter -= 1;
}

/*!
 *
 */
u8 power_mgmnt_is_on(POWER_MANAGEMENT_UNIT_TYPE* p_unit) {
	return (p_unit->status.is_on != 0) ? 1 : 0;
}

