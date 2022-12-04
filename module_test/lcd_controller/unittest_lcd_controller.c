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
 * @file    unittest_lcd_controller.c
 * @author  Sebastian Lesse
 * @date    2022 / 11 / 28
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

#include "../unittest.h"

// --------------------------------------------------------------------------------

UT_ACTIVATE()

// --------------------------------------------------------------------------------

#include "initialization/initialization.h"

// --------------------------------------------------------------------------------

#include "common/common_tools_bit_vector.h"

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INITIALIZATION             0
#define TEST_CASE_ID_SET_OUTPUT_LOW             1
#define TEST_CASE_ID_CHANGE_STATES              2
#define TEST_CASE_ID_TOGGLE_LEVEL               3

// --------------------------------------------------------------------------------

#define UT_SIGNAL_TIMEOUT_MS    100

// --------------------------------------------------------------------------------

static void unittest_reset_counter(void) {

}

// --------------------------------------------------------------------------------

// stubs

// --------------------------------------------------------------------------------

BUILD_GPIO ( LCD_RS,       GPIO_PORT_A,    GPIO_PIN_0,     GPIO_INPUT | GPIO_IDLE_HIGH ) //
BUILD_GPIO ( LCD_EN,       GPIO_PORT_A,    GPIO_PIN_1,     GPIO_INPUT | GPIO_IDLE_HIGH ) //
BUILD_GPIO ( LCD_D4,       GPIO_PORT_A,    GPIO_PIN_2,     GPIO_INPUT | GPIO_IDLE_HIGH ) //
BUILD_GPIO ( LCD_D5,       GPIO_PORT_A,    GPIO_PIN_3,     GPIO_INPUT | GPIO_IDLE_HIGH ) //
BUILD_GPIO ( LCD_D6,       GPIO_PORT_A,    GPIO_PIN_4,     GPIO_INPUT | GPIO_IDLE_HIGH ) // 
BUILD_GPIO ( LCD_D7,       GPIO_PORT_A,    GPIO_PIN_5,     GPIO_INPUT | GPIO_IDLE_HIGH ) //

// --------------------------------------------------------------------------------

// Signals / Slots

// --------------------------------------------------------------------------------

/**
 * @brief Initialize all GPIOS
 * Check if the values taht have been written to the cpu-regsiters match the expected values.
 * 
 */
static void TEST_CASE_initialization(void) {

    UT_START_TEST_CASE("Initialization")
    {
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZATION);
        unittest_reset_counter();
    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

    UT_START_TESTBENCH("Welcome the the UNITTEST for LCD-Controller")
    {
        TEST_CASE_initialization();
    }
    UT_END_TESTBENCH()

    return UT_TEST_RESULT();
}

// --------------------------------------------------------------------------------
