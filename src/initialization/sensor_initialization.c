/**
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * @file    sensor_initialization.c
 * @author  Sebastian Lesse
 * @date    2019 / 09 / 24
 * @brief   Calls initialization routines of all available senors
 * 
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "cpu.h"

// --------------------------------------------------------------------------------

#include "system_interface.h"

#include "sensor/ads1115/ads1115_driver.h"
#include "sensor/sht31/sht31_driver.h"
#include "sensor/gm5528/gm5528_driver.h"

// --------------------------------------------------------------------------------

/**
 * @see sensor_initialization.h#sensor_initialization
 */
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

// --------------------------------------------------------------------------------
