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

#include "../unittest.h"

// --------------------------------------------------------------------------------

UT_ACTIVATE()

// --------------------------------------------------------------------------------

#include "initialization/initialization.h"
#include "time_management/time_management.h"
#include "mcu_task_management/mcu_task_interface.h"

// --------------------------------------------------------------------------------

#include "modules/lcd/lcd_interface.h"

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INITIALIZATION             0
#define TEST_CASE_ID_WRITE_FIRST_LINE           1
#define TEST_CASE_ID_WRITE_SECOND_LINE          2

// --------------------------------------------------------------------------------

#define UT_SIGNAL_TIMEOUT_MS    100

// --------------------------------------------------------------------------------

static void unittest_reset_counter(void) {

}

// --------------------------------------------------------------------------------

// stubs

u16 common_tools_string_length(const char* p_string) {
    return strlen(p_string);
}

void rtc_timer_delay(u16 delay_us) {
    (void) delay_us;
}

// --------------------------------------------------------------------------------

BUILD_GPIO ( LCD_RS,       GPIO_PORT_A,    GPIO_PIN_0,     GPIO_INPUT | GPIO_IDLE_HIGH ) //
BUILD_GPIO ( LCD_EN,       GPIO_PORT_A,    GPIO_PIN_1,     GPIO_INPUT | GPIO_IDLE_HIGH ) //
BUILD_GPIO ( LCD_D4,       GPIO_PORT_A,    GPIO_PIN_2,     GPIO_INPUT | GPIO_IDLE_HIGH ) //
BUILD_GPIO ( LCD_D5,       GPIO_PORT_A,    GPIO_PIN_3,     GPIO_INPUT | GPIO_IDLE_HIGH ) //
BUILD_GPIO ( LCD_D6,       GPIO_PORT_A,    GPIO_PIN_4,     GPIO_INPUT | GPIO_IDLE_HIGH ) // 
BUILD_GPIO ( LCD_D7,       GPIO_PORT_A,    GPIO_PIN_5,     GPIO_INPUT | GPIO_IDLE_HIGH ) //

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(UNITTEST_TIMER)

// --------------------------------------------------------------------------------

// Signals / Slots

// --------------------------------------------------------------------------------

/**
 * @brief Initializes the lcd controller.
 * The lcd then will be initialized by set GPIO-Pins
 * 
 */
static void TEST_CASE_initialization(void) {

    UT_START_TEST_CASE("Initialization")
    {
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_INITIALIZATION);
        unittest_reset_counter();

        lcd_init();
        lcd_set_enabled(LCD_ENABLE);

        UNITTEST_TIMER_start();

        while (UNITTEST_TIMER_is_up(250) == 0) {
            mcu_task_controller_schedule();
        }

    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

/**
 * @brief Writing the first line on the LCD.
 * 
 */
static void TEST_CASE_write_first_line(void) {

    UT_START_TEST_CASE("Write First Line")
    {
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_WRITE_FIRST_LINE);
        unittest_reset_counter();

        SIGNAL_LCD_LINE_send("UT LCD LINE ONE");

        UNITTEST_TIMER_start();

        while (UNITTEST_TIMER_is_up(2500) == 0) {
            mcu_task_controller_schedule();
        }

    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

/**
 * @brief Writing the first line on the LCD.
 * 
 */
static void TEST_CASE_write_second_line(void) {

    UT_START_TEST_CASE("Write Second Line")
    {
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_WRITE_SECOND_LINE);
        unittest_reset_counter();

        SIGNAL_LCD_LINE_send("UT LCD LINE TWO");

        UNITTEST_TIMER_start();

        while (UNITTEST_TIMER_is_up(2500) == 0) {
            mcu_task_controller_schedule();
        }

    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

    UT_START_TESTBENCH("Welcome the the UNITTEST for LCD-Controller")
    {
        TEST_CASE_initialization();
        TEST_CASE_write_first_line();
        TEST_CASE_write_second_line();
    }
    UT_END_TESTBENCH()

    return UT_TEST_RESULT();
}

// --------------------------------------------------------------------------------
