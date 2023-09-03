/*! \file *********************************************************************

 *****************************************************************************/

#define TRACER_OFF

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "tracer.h"

//-----------------------------------------------------------------------------

#include "system_interface.h"

#include "sensor/ads1115/ads1115_driver.h"
#include "sensor/sht31/sht31_driver.h"
#include "sensor/gm5528/gm5528_driver.h"

//-----------------------------------------------------------------------------

void sensor_initialization(void) {

    #ifdef HAS_SENSOR_ADS1115
    {
        DEBUG_PASS("sensor_initialization() - ads1115_driver_init()");
        ads1115_driver_init(i_system.driver.i2c0);
    }
    #endif
        
    #ifdef HAS_SENSOR_SHT31
    {
        DEBUG_PASS("sensor_initialization() - sht31_driver_init()");
        sht31_driver_init(i_system.driver.i2c0);
    }
    #endif    

    #if HAS_LIGHT_SENSOR_GM5528
    {
        DEBUG_PASS("sensor_initialization() - gm5528_driver_init()");
        gm5528_driver_init();
    }
    #endif

}
