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
 * @file    unittest_keypad_3x4.c
 * @author  Sebastian Lesse
 * @date    2023 / 01 / 03
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

#include "time_management/time_management.h"
#include "initialization/initialization.h"
#include "mcu_task_management/mcu_task_interface.h"

// --------------------------------------------------------------------------------

#include "driver/keypad/keypad_driver.h"

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INITIALIZATION             0
#define TEST_CASE_ID_KEYS_PRESSED_1             1
#define TEST_CASE_ID_KEYS_PRESSED_5             2
#define TEST_CASE_ID_KEYS_PRESSED_9             3
#define TEST_CASE_ID_KEYS_PRESSED_0             4
#define TEST_CASE_ID_NO_KEYS_PRESSED            5
#define TEST_CASE_ID_KEYS_PRESSED_0_5_6_9_star  6

// --------------------------------------------------------------------------------

#define UT_SIGNAL_TIMEOUT_MS    100

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(UNITTEST_TIMER)

// --------------------------------------------------------------------------------

static u8 counter_KEY_COL_1_DRIVE_HIGH = 0;
static u8 counter_KEY_COL_1_NO_DRIVE = 0;

static u8 counter_KEY_COL_2_DRIVE_HIGH = 0;
static u8 counter_KEY_COL_2_NO_DRIVE = 0;

static u8 counter_KEY_COL_3_DRIVE_HIGH = 0;
static u8 counter_KEY_COL_3_NO_DRIVE = 0;

static u8 counter_KEY_ROW_1_pull_down = 0;
static u8 counter_KEY_ROW_1_IS_HIGH_LEVEL = 0;

static u8 counter_KEY_ROW_2_pull_down = 0;
static u8 counter_KEY_ROW_2_IS_HIGH_LEVEL = 0;

static u8 counter_KEY_ROW_3_pull_down = 0;
static u8 counter_KEY_ROW_3_IS_HIGH_LEVEL = 0;

static u8 counter_KEY_ROW_4_pull_down = 0;
static u8 counter_KEY_ROW_4_IS_HIGH_LEVEL = 0;

static u8 counter_INVALID_PIN_DRIVE = 0;

static u8 KEY_COL_1_IS_HIGH = 0;
static u8 KEY_COL_2_IS_HIGH = 0;
static u8 KEY_COL_3_IS_HIGH = 0;

// --------------------------------------------------------------------------------

/**
 * @brief This structure is sued to set some kes pressed by the user.
 * 
 */
static KEYPAD_KEYS ut_keys_pressed;

// --------------------------------------------------------------------------------

static void unittest_reset_counter(void) {

    counter_KEY_COL_1_DRIVE_HIGH = 0;
    counter_KEY_COL_1_NO_DRIVE = 0;

    counter_KEY_COL_2_DRIVE_HIGH = 0;
    counter_KEY_COL_2_NO_DRIVE = 0;

    counter_KEY_COL_3_DRIVE_HIGH = 0;
    counter_KEY_COL_3_NO_DRIVE = 0;

    counter_KEY_ROW_1_pull_down = 0;
    counter_KEY_ROW_1_IS_HIGH_LEVEL = 0;

    counter_KEY_ROW_2_pull_down = 0;
    counter_KEY_ROW_2_IS_HIGH_LEVEL = 0;

    counter_KEY_ROW_3_pull_down = 0;
    counter_KEY_ROW_3_IS_HIGH_LEVEL = 0;

    counter_KEY_ROW_4_pull_down = 0;
    counter_KEY_ROW_4_IS_HIGH_LEVEL = 0;

    counter_INVALID_PIN_DRIVE = 0;
    
    KEY_COL_1_IS_HIGH = 0;
    KEY_COL_2_IS_HIGH = 0;
    KEY_COL_3_IS_HIGH = 0;

    memset(&ut_keys_pressed, 0x00, sizeof(KEYPAD_KEYS));
}

// --------------------------------------------------------------------------------

void KEY_COL_1_drive_high(void) {
    counter_KEY_COL_1_DRIVE_HIGH += 1;
    KEY_COL_1_IS_HIGH = 1;

    if (KEY_COL_2_IS_HIGH || KEY_COL_3_IS_HIGH) {
        counter_INVALID_PIN_DRIVE += 1;
    }
}

void KEY_COL_1_no_drive(void) {
    counter_KEY_COL_1_NO_DRIVE += 1;
    KEY_COL_1_IS_HIGH = 0;
}

void KEY_COL_2_drive_high(void) {
    counter_KEY_COL_2_DRIVE_HIGH += 1;
    KEY_COL_2_IS_HIGH = 1;

    if (KEY_COL_1_IS_HIGH || KEY_COL_3_IS_HIGH) {
        counter_INVALID_PIN_DRIVE += 1;
    }
}

void KEY_COL_2_no_drive(void) {
    counter_KEY_COL_2_NO_DRIVE += 1;
    KEY_COL_2_IS_HIGH = 0;
}

void KEY_COL_3_drive_high(void) {
    counter_KEY_COL_3_DRIVE_HIGH += 1;
    KEY_COL_3_IS_HIGH = 1;

    if (KEY_COL_2_IS_HIGH || KEY_COL_1_IS_HIGH) {
        counter_INVALID_PIN_DRIVE += 1;
    }
}

void KEY_COL_3_no_drive(void) {
    counter_KEY_COL_3_NO_DRIVE += 1;
    KEY_COL_3_IS_HIGH = 0;
}

void KEY_ROW_1_pull_down(void) {
    counter_KEY_ROW_1_pull_down += 1;
}

u8 KEY_ROW_1_is_high_level(void) {
    counter_KEY_ROW_1_IS_HIGH_LEVEL += 1;
    return ut_keys_pressed.key_1 || ut_keys_pressed.key_2 || ut_keys_pressed.key_3;
}

void KEY_ROW_2_pull_down(void) {
    counter_KEY_ROW_2_pull_down += 1;
}

u8 KEY_ROW_2_is_high_level(void) {
    counter_KEY_ROW_2_IS_HIGH_LEVEL += 1;
    return ut_keys_pressed.key_4 || ut_keys_pressed.key_5 || ut_keys_pressed.key_6;
}

void KEY_ROW_3_pull_down(void) {
    counter_KEY_ROW_3_pull_down += 1;
}

u8 KEY_ROW_3_is_high_level(void) {
    counter_KEY_ROW_3_IS_HIGH_LEVEL += 1;
    return ut_keys_pressed.key_7 || ut_keys_pressed.key_8 || ut_keys_pressed.key_9;
}

void KEY_ROW_4_pull_down(void) {
    counter_KEY_ROW_4_pull_down += 1;
}

u8 KEY_ROW_4_is_high_level(void) {
    counter_KEY_ROW_4_IS_HIGH_LEVEL += 1;
    return ut_keys_pressed.key_raute || ut_keys_pressed.key_0 || ut_keys_pressed.key_star;
}

// --------------------------------------------------------------------------------

// stubs

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

        keypad_driver_init();

        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_DRIVE_HIGH, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_NO_DRIVE, 1);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_DRIVE_HIGH, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_NO_DRIVE, 1);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_DRIVE_HIGH, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_NO_DRIVE, 1);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_pull_down, 1);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_IS_HIGH_LEVEL, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_pull_down, 1);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_IS_HIGH_LEVEL, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_pull_down, 1);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_IS_HIGH_LEVEL, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_pull_down, 1);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_IS_HIGH_LEVEL, 0);

        UT_CHECK_IS_EQUAL(counter_INVALID_PIN_DRIVE, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_1_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_2_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_3_IS_HIGH, 0);

    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

/**
 * @brief Only key 1 is pressed
 * 
 */
static void TEST_CASE_key_pressed_1(void) {

    UT_START_TEST_CASE("Key Pressed 1")
    {
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_KEYS_PRESSED_1);
        unittest_reset_counter();

        KEYPAD_KEYS keys_pressed;
        memset(&keys_pressed, 0x00, sizeof(KEYPAD_KEYS));

        ut_keys_pressed.key_1 = 1;
        keypad_driver_get_keys(&keys_pressed);

        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_DRIVE_HIGH, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_NO_DRIVE, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_DRIVE_HIGH, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_NO_DRIVE, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_DRIVE_HIGH, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_NO_DRIVE, 3);

        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_IS_HIGH_LEVEL, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_IS_HIGH_LEVEL, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_IS_HIGH_LEVEL, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_IS_HIGH_LEVEL, 9);

        UT_CHECK_IS_EQUAL(counter_INVALID_PIN_DRIVE, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_1_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_2_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_3_IS_HIGH, 0);

        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_1, 1);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_2, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_3, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_4, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_5, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_6, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_7, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_8, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_9, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_0, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_star, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_raute, 0);

    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

/**
 * @brief Only key 5 is pressed
 * 
 */
static void TEST_CASE_key_pressed_5(void) {

    UT_START_TEST_CASE("Key Pressed 5")
    {
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_KEYS_PRESSED_5);
        unittest_reset_counter();

        KEYPAD_KEYS keys_pressed;
        memset(&keys_pressed, 0x00, sizeof(KEYPAD_KEYS));

        ut_keys_pressed.key_5 = 1;
        keypad_driver_get_keys(&keys_pressed);

        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_DRIVE_HIGH, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_NO_DRIVE, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_DRIVE_HIGH, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_NO_DRIVE, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_DRIVE_HIGH, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_NO_DRIVE, 3);

        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_IS_HIGH_LEVEL, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_IS_HIGH_LEVEL, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_IS_HIGH_LEVEL, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_IS_HIGH_LEVEL, 9);

        UT_CHECK_IS_EQUAL(counter_INVALID_PIN_DRIVE, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_1_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_2_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_3_IS_HIGH, 0);

        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_1, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_2, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_3, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_4, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_5, 1);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_6, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_7, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_8, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_9, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_0, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_star, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_raute, 0);

    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

/**
 * @brief Only key 9 is pressed
 * 
 */
static void TEST_CASE_key_pressed_9(void) {

    UT_START_TEST_CASE("Key Pressed 9")
    {
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_KEYS_PRESSED_9);
        unittest_reset_counter();

        KEYPAD_KEYS keys_pressed;
        memset(&keys_pressed, 0x00, sizeof(KEYPAD_KEYS));

        ut_keys_pressed.key_9 = 1;
        keypad_driver_get_keys(&keys_pressed);

        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_DRIVE_HIGH, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_NO_DRIVE, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_DRIVE_HIGH, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_NO_DRIVE, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_DRIVE_HIGH, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_NO_DRIVE, 3);

        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_IS_HIGH_LEVEL, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_IS_HIGH_LEVEL, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_IS_HIGH_LEVEL, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_IS_HIGH_LEVEL, 9);

        UT_CHECK_IS_EQUAL(counter_INVALID_PIN_DRIVE, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_1_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_2_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_3_IS_HIGH, 0);

        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_1, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_2, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_3, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_4, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_5, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_6, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_7, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_8, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_9, 1);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_0, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_star, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_raute, 0);

    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

/**
 * @brief Only key 0 is pressed
 * 
 */
static void TEST_CASE_key_pressed_0(void) {

    UT_START_TEST_CASE("Key Pressed 0")
    {
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_KEYS_PRESSED_0);
        unittest_reset_counter();

        KEYPAD_KEYS keys_pressed;
        memset(&keys_pressed, 0x00, sizeof(KEYPAD_KEYS));

        ut_keys_pressed.key_0 = 1;
        keypad_driver_get_keys(&keys_pressed);

        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_DRIVE_HIGH, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_NO_DRIVE, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_DRIVE_HIGH, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_NO_DRIVE, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_DRIVE_HIGH, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_NO_DRIVE, 3);

        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_IS_HIGH_LEVEL, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_IS_HIGH_LEVEL, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_IS_HIGH_LEVEL, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_IS_HIGH_LEVEL, 9);

        UT_CHECK_IS_EQUAL(counter_INVALID_PIN_DRIVE, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_1_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_2_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_3_IS_HIGH, 0);

        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_1, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_2, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_3, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_4, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_5, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_6, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_7, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_8, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_9, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_0, 1);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_star, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_raute, 0);

    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

/**
 * @brief No key is pressed
 * 
 */
static void TEST_CASE_no_key_pressed(void) {

    UT_START_TEST_CASE("No Key Pressed")
    {
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_NO_KEYS_PRESSED);
        unittest_reset_counter();

        KEYPAD_KEYS keys_pressed;
        memset(&keys_pressed, 0x00, sizeof(KEYPAD_KEYS));

        keypad_driver_get_keys(&keys_pressed);

        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_DRIVE_HIGH, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_NO_DRIVE, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_DRIVE_HIGH, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_NO_DRIVE, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_DRIVE_HIGH, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_NO_DRIVE, 3);

        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_IS_HIGH_LEVEL, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_IS_HIGH_LEVEL, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_IS_HIGH_LEVEL, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_IS_HIGH_LEVEL, 9);

        UT_CHECK_IS_EQUAL(counter_INVALID_PIN_DRIVE, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_1_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_2_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_3_IS_HIGH, 0);

        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_1, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_2, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_3, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_4, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_5, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_6, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_7, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_8, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_9, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_0, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_star, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_raute, 0);

    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

/**
 * @brief The key's 0 5 6 9 and star are pressed
 * 
 */
static void TEST_CASE_key_pressed_0_5_6_9_star(void) {

    UT_START_TEST_CASE("Key Pressed 9")
    {
        UT_SET_TEST_CASE_ID(TEST_CASE_ID_KEYS_PRESSED_0_5_6_9_star);
        unittest_reset_counter();

        KEYPAD_KEYS keys_pressed;
        memset(&keys_pressed, 0x00, sizeof(KEYPAD_KEYS));

        ut_keys_pressed.key_0 = 1;
        ut_keys_pressed.key_5 = 1;
        ut_keys_pressed.key_6 = 1;
        ut_keys_pressed.key_9 = 1;
        ut_keys_pressed.key_star = 1;
        keypad_driver_get_keys(&keys_pressed);

        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_DRIVE_HIGH, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_NO_DRIVE, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_DRIVE_HIGH, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_NO_DRIVE, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_DRIVE_HIGH, 3);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_NO_DRIVE, 3);

        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_IS_HIGH_LEVEL, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_IS_HIGH_LEVEL, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_IS_HIGH_LEVEL, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_IS_HIGH_LEVEL, 9);

        UT_CHECK_IS_EQUAL(counter_INVALID_PIN_DRIVE, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_1_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_2_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_3_IS_HIGH, 0);

        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_1, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_2, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_3, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_4, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_5, 1);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_6, 1);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_7, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_8, 0);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_9, 1);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_0, 1);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_star, 1);
        UT_CHECK_IS_EQUAL(ut_keys_pressed.key_raute, 0);

    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

    UT_START_TESTBENCH("Welcome the the UNITTEST for Keypad 3x4")
    {
        TEST_CASE_initialization();
        TEST_CASE_key_pressed_1();
        TEST_CASE_key_pressed_5();
        TEST_CASE_key_pressed_9();
        TEST_CASE_key_pressed_0();
        TEST_CASE_no_key_pressed();
        TEST_CASE_key_pressed_0_5_6_9_star();
    }
    UT_END_TESTBENCH()

    return UT_TEST_RESULT();
}

// --------------------------------------------------------------------------------
