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
 * @file    unittest_movement_detection_controller.c
 * @author  Sebastian Lesse
 * @date    2023 / 05 / 07
 * @brief   Short description of this file
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

#include "common/signal_slot_interface.h"
#include "mcu_task_management/mcu_task_interface.h"

#include "ui/file_interface/file_interface.h"
#include "ui/console/ui_console.h"

#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

#include "modules/movement_detection/movement_detection_controller.h"
#include "modules/movement_detection/movement_detect_sensor_interface.h"

// --------------------------------------------------------------------------------

#include "../unittest.h"

UT_ACTIVATE()

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INITIALIZE                 0
#define TEST_CASE_ID_POWER_DOWN                 1
#define TEST_CASE_ID_IS_MOVEMENT                2
#define TEST_CASE_ID_VERIFY_FAILED              3

// --------------------------------------------------------------------------------

// counter

u8 counter_SENSOR_POWER_DOWN = 0;
u8 counter_SENSOR_POWER_UP = 0;
u8 counter_SENSOR_IS_MOVEMENT = 0;
u8 counter_SENSOR_CONFIGURE = 0;
u8 counter_SENSOR_RESET = 0;
u8 counter_SIGNAL_MOVEMENT_DETECT = 0;

// --------------------------------------------------------------------------------

static void unittest_reset_counter(void) {
    counter_SENSOR_POWER_DOWN = 0;
    counter_SENSOR_POWER_UP = 0;
    counter_SENSOR_IS_MOVEMENT = 0;
    counter_SENSOR_CONFIGURE = 0;
    counter_SENSOR_RESET = 0;
    counter_SIGNAL_MOVEMENT_DETECT = 0;
}

// --------------------------------------------------------------------------------

// stubs

static u8 ut_is_movement_return_value = 0;

void movement_detect_sensor_power_up(void) {
    DEBUG_PASS("UT - movement_detect_sensor_power_up()");
    counter_SENSOR_POWER_UP += 1;
}

void movement_detect_sensor_power_down(void) {
    DEBUG_PASS("UT - movement_detect_sensor_power_down()");
    counter_SENSOR_POWER_DOWN += 1;
}

u8 movement_detect_sensor_is_movement(void) {
    DEBUG_PASS("UT - movement_detect_sensor_is_movement()");
    counter_SENSOR_IS_MOVEMENT += 1;
    return ut_is_movement_return_value;
}

void movement_detect_sensor_configure(MOVEMENT_DETECT_SENSOR_CFG* p_config) {
    DEBUG_PASS("UT - movement_detect_sensor_configure()");
    counter_SENSOR_CONFIGURE += 1;
}

void movement_detect_sensor_reset(void) {
    DEBUG_PASS("UT - movement_detect_sensor_reset()");
    counter_SENSOR_RESET += 1;

}

// --------------------------------------------------------------------------------

// slots

static void unittest_movement_detect_signal_callback(const void* p_argument) {
    (void) p_argument;
    counter_SIGNAL_MOVEMENT_DETECT += 1;
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(
    MOVEMENT_DETECT_SIGNAL,
    UT_MOVEMENT_DETECT_SLOT,
    unittest_movement_detect_signal_callback
)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(UNITTEST_TIMER)

// --------------------------------------------------------------------------------

static void UNITTEST_movement_detect_ctrl_init(void) {
    
    UT_START_TEST_CASE("Movement-Detect-Ctrl - Initialize")
    {    
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);

        unittest_reset_counter();

        UNITTEST_TIMER_start();

        movement_detection_controller_init();

        while (UNITTEST_TIMER_is_up(500) == 0) {
            mcu_task_controller_schedule();
        }

        UT_MOVEMENT_DETECT_SLOT_connect();

        UNITTEST_TIMER_stop();

        UT_CHECK_IS_EQUAL(counter_SENSOR_POWER_DOWN, 0);
        UT_CHECK_IS_EQUAL(counter_SENSOR_POWER_UP, 0);
        UT_CHECK_IS_GREATER(counter_SENSOR_IS_MOVEMENT, 0);
        UT_CHECK_IS_EQUAL(counter_SENSOR_CONFIGURE, 1);
        UT_CHECK_IS_EQUAL(counter_SENSOR_RESET, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_MOVEMENT_DETECT, 0);
    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

static void UNITTEST_movement_detect_ctrl_power_down(void) {
    
    UT_START_TEST_CASE("Movement-Detect-Ctrl - POWER DOWN")
    {    
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_POWER_DOWN);

        unittest_reset_counter();

        UNITTEST_TIMER_start();

        MOVEMENT_DETECT_POWER_DOWN_SIGNAL_send(NULL);

        while (UNITTEST_TIMER_is_up(500) == 0) {
            mcu_task_controller_schedule();

            if (counter_SENSOR_POWER_DOWN) {
                break;
            }
        }

        MOVEMENT_DETECT_POWER_UP_SIGNAL_send(NULL);

        while (UNITTEST_TIMER_is_up(500) == 0) {
            mcu_task_controller_schedule();

            if (counter_SENSOR_POWER_UP) {
                break;
            }
        }

        UNITTEST_TIMER_stop();

        UT_CHECK_IS_EQUAL(counter_SENSOR_POWER_DOWN, 1);
        UT_CHECK_IS_EQUAL(counter_SENSOR_POWER_UP, 1);
        UT_CHECK_IS_EQUAL(counter_SENSOR_IS_MOVEMENT, 0);
        UT_CHECK_IS_EQUAL(counter_SENSOR_CONFIGURE, 0);
        UT_CHECK_IS_EQUAL(counter_SENSOR_RESET, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_MOVEMENT_DETECT, 0);
    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

static void UNITTEST_movement_detect_is_movement(void) {
    
    UT_START_TEST_CASE("Movement-Detect-Ctrl - Is Movement")
    {    
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_IS_MOVEMENT);

        unittest_reset_counter();

        ut_is_movement_return_value = 1;

        UNITTEST_TIMER_start();
        while (UNITTEST_TIMER_is_up(500) == 0) {
            mcu_task_controller_schedule();

            if (counter_SENSOR_IS_MOVEMENT) {
                break;
            }
        }

        /**
         * @brief Movement was detected for the first time
         */

        UNITTEST_TIMER_start();
        while (UNITTEST_TIMER_is_up(500) == 0) {
            mcu_task_controller_schedule();

            if (counter_SENSOR_IS_MOVEMENT == 2) {
                break;
            }
        }

        /**
         * @brief Movement was detected for the second time
         * It has now verified.
         */

        ut_is_movement_return_value = 0;

        UNITTEST_TIMER_start();
        while (UNITTEST_TIMER_is_up(5000) == 0) {
            mcu_task_controller_schedule();

            if (counter_SIGNAL_MOVEMENT_DETECT) {
                break;
            }
        }

        /**
         * @brief The state machine now enters the pause state
         * we check the time until the first movement check is
         * performed.
         */

        UNITTEST_TIMER_start();
        while (UNITTEST_TIMER_is_up(4000) == 0) {
            mcu_task_controller_schedule();

            if (counter_SENSOR_IS_MOVEMENT == 3) {
                break;
            }
        }

        UT_CHECK_IS_GREATER(UNITTEST_TIMER_elapsed(), MOVEMENT_DETECTION_CONTROLLER_PAUSE_TIME_MS);
        UT_CHECK_IS_EQUAL(counter_SENSOR_POWER_DOWN, 0);
        UT_CHECK_IS_EQUAL(counter_SENSOR_POWER_UP, 0);
        UT_CHECK_IS_GREATER(counter_SENSOR_IS_MOVEMENT, 2);
        UT_CHECK_IS_EQUAL(counter_SENSOR_CONFIGURE, 0);
        UT_CHECK_IS_EQUAL(counter_SENSOR_RESET, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_MOVEMENT_DETECT, 1);
        
        UNITTEST_TIMER_stop();
    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

static void UNITTEST_movement_detect_verify_failed(void) {
    
    UT_START_TEST_CASE("Movement-Detect-Ctrl - Verify failed")
    {    
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_VERIFY_FAILED);

        unittest_reset_counter();

        ut_is_movement_return_value = 1;

        UNITTEST_TIMER_start();
        while (UNITTEST_TIMER_is_up(500) == 0) {
            mcu_task_controller_schedule();

            if (counter_SENSOR_IS_MOVEMENT == 1) {
                break;
            }
        }

        /**
         * @brief Movement was detected for the first time.
         * The controller now enters the wait-for-verify state
         * Then it will check for movement a second time
         */
        
        ut_is_movement_return_value = 0;

        UNITTEST_TIMER_start();
        while (UNITTEST_TIMER_is_up(500) == 0) {
            mcu_task_controller_schedule();

            if (counter_SENSOR_IS_MOVEMENT == 2) {
                break;
            }
        }

        /**
         * @brief Now the state machine enters the 
         * wait-for-movement state.
         */

        UNITTEST_TIMER_start();
        while (UNITTEST_TIMER_is_up(MOVEMENT_DETECTION_CONTROLLER_VERIFY_TIMEOUT_MS) == 0) {
            mcu_task_controller_schedule();
        }

        UT_MOVEMENT_DETECT_SLOT_connect();

        UNITTEST_TIMER_stop();

        UT_CHECK_IS_EQUAL(counter_SENSOR_POWER_DOWN, 0);
        UT_CHECK_IS_EQUAL(counter_SENSOR_POWER_UP, 0);
        UT_CHECK_IS_GREATER(counter_SENSOR_IS_MOVEMENT, 3);
        UT_CHECK_IS_EQUAL(counter_SENSOR_CONFIGURE, 0);
        UT_CHECK_IS_EQUAL(counter_SENSOR_RESET, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_MOVEMENT_DETECT, 0);
    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

    //TRACER_DISABLE();

    UT_START_TESTBENCH("Welcome the the UNITTEST for log-interface v1.0")
    {
        UNITTEST_movement_detect_ctrl_init();
        UNITTEST_movement_detect_ctrl_power_down();
        UNITTEST_movement_detect_is_movement();
        UNITTEST_movement_detect_verify_failed();
    }
    UT_END_TESTBENCH()

    return UT_TEST_RESULT();
}

// --------------------------------------------------------------------------------
