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
 * @file    unittest_task_controller
 * @author  Sebastian Lesse
 * @date    2023 / 10 / 15
 * @brief   Short description of this file
 * 
 */

#define TRACER_ON

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

#include "mcu_task_management/mcu_task_interface.h"
#include "time_management/time_management.h"
#include "common/iterator_interface.h"

// --------------------------------------------------------------------------------

#include "../unittest.h"

UT_ACTIVATE()

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INITIALIZE                 0
#define TEST_CASE_ID_ITERATE_01                 1

// --------------------------------------------------------------------------------

// counter

u8 counter_SENSOR_POWER_DOWN = 0;

// --------------------------------------------------------------------------------

static void unittest_reset_counter(void) {

    counter_SENSOR_POWER_DOWN = 0;
}

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(UT_DELAY_TIMER)

static void unittest_daly_ms(u16 milli) {
    UT_DELAY_TIMER_start();
    while(UT_DELAY_TIMER_is_up(milli) == 0) {

    }
    UT_DELAY_TIMER_stop;
}

// --------------------------------------------------------------------------------

// stubs

static void ut_task_01_start(void) { }
u16 ut_task_01_get_interval(void) { return 5; }
MCU_TASK_INTERFACE_TASK_STATE ut_task_01_get_state(void) { return MCU_TASK_RUNNING; }
void ut_task_01_execute(void) { unittest_daly_ms(5); }
void ut_task_01_get_termiante(void) { }

TASK_CREATE(
    UT_TASK_01,
    TASK_PRIORITY_MIDDLE,
    ut_task_01_get_interval,
    ut_task_01_start,
    ut_task_01_execute,
    ut_task_01_get_state,
    ut_task_01_get_termiante
)

static void ut_task_02_start(void) { }
u16 ut_task_02_get_interval(void) { return 50; }
MCU_TASK_INTERFACE_TASK_STATE ut_task_02_get_state(void) { return MCU_TASK_RUNNING; }
void ut_task_02_execute(void) { unittest_daly_ms(5); }
void ut_task_02_get_termiante(void) { }

TASK_CREATE(
    UT_TASK_02,
    TASK_PRIORITY_MIDDLE,
    ut_task_02_get_interval,
    ut_task_02_start,
    ut_task_02_execute,
    ut_task_02_get_state,
    ut_task_02_get_termiante
)

// --------------------------------------------------------------------------------

// slots

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(UNITTEST_TIMER)

// --------------------------------------------------------------------------------

static void UNITTEST_task_control_init(void) {

    UT_START_TEST_CASE("Task-Controll - Initialize")
    {    
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZE);

        unittest_reset_counter();

        mcu_task_controller_init();
        mcu_task_controller_enable_statistics(TASK_CTRL_STATISTIC_ON);
        UT_TASK_01_init();
        UT_TASK_02_init();

    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

static void UNITTEST_task_control_iterate_01(void) {

    UT_START_TEST_CASE("Task-Controll - Iterate_01")
    {    
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_ITERATE_01);

        unittest_reset_counter();

        UNITTEST_TIMER_start();
        while (UNITTEST_TIMER_is_up(500) == 0) {
            mcu_task_controller_schedule();
        }
        UNITTEST_TIMER_stop();

        u8 task_counter = 0;

        const char task_names[2][11] = {
            "UT_TASK_01\0",
            "UT_TASK_02\0"
        };

        ITERATE_FOR(
            TASK_CTRL_STATS,
            task_stats,
            MCU_TASK_ITERATOR_INTERFACE_iter(),

            UT_CHECK_IS_GREATER (task_stats.last_runtime, 0);

            if (task_counter < 2) {
                UT_COMPARE_STRING(task_names[task_counter], task_stats.p_name);
            }

            task_counter += 1;
        )

        UT_CHECK_IS_EQUAL (task_counter, 2);

    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

    //TRACER_DISABLE();

    UT_START_TESTBENCH("Welcome the the UNITTEST for task-controller 1.0")
    {
        UNITTEST_task_control_init();
        UNITTEST_task_control_iterate_01();
    }
    UT_END_TESTBENCH()

    return UT_TEST_RESULT();
}

// --------------------------------------------------------------------------------
