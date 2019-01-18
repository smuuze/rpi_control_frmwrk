/*! \file *********************************************************************

 *****************************************************************************/

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "local_rtc_driver.h"
#include "local_gpio_driver.h"

void system_initialization(void) {

	gpio_driver_init();
	local_rtc_timer_init();
}

