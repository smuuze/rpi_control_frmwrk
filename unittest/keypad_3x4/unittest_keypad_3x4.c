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
#include "signal_slot_interface.h"

// --------------------------------------------------------------------------------

#include "modules/keypad/keypad_interface.h"
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

static u8 counter_SIGNAL_KEY_1_PRESSED = 0;
static u8 counter_SIGNAL_KEY_1_DOWN = 0;
static u8 counter_SIGNAL_KEY_1_RELEASED = 0;

static u8 counter_SIGNAL_KEY_5_PRESSED = 0;
static u8 counter_SIGNAL_KEY_5_DOWN = 0;
static u8 counter_SIGNAL_KEY_5_RELEASED = 0;

static u8 counter_SIGNAL_KEY_9_PRESSED = 0;
static u8 counter_SIGNAL_KEY_9_DOWN = 0;
static u8 counter_SIGNAL_KEY_9_RELEASED = 0;

static u8 counter_SIGNAL_KEY_0_PRESSED = 0;
static u8 counter_SIGNAL_KEY_0_DOWN = 0;
static u8 counter_SIGNAL_KEY_0_RELEASED = 0;

// --------------------------------------------------------------------------------

static void ut_slot_key_0_pressed(const void* p_arg) {
    DEBUG_PASS("ut_slot_key_0_pressed()");
    counter_SIGNAL_KEY_0_PRESSED += 1;
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_0_PRESSED, UT_KEY_0_PRESSED_SLOT, ut_slot_key_0_pressed)

static void ut_slot_key_0_down(const void* p_arg) {
    DEBUG_PASS("ut_slot_key_0_down()");
    counter_SIGNAL_KEY_0_DOWN += 1;
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_0_DOWN, UT_KEY_0_DOWN_SLOT, ut_slot_key_0_down)

static void ut_slot_key_0_released(const void* p_arg) {
    DEBUG_PASS("ut_slot_key_0_released()");
    counter_SIGNAL_KEY_0_RELEASED += 1;
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_0_RELEASED, UT_KEY_0_RELEASED_SLOT, ut_slot_key_0_released)

// --------------------------------------------------------------------------------

static void ut_slot_key_1_pressed(const void* p_arg) {
    DEBUG_PASS("ut_slot_key_1_pressed()");
    counter_SIGNAL_KEY_1_PRESSED += 1;
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_1_PRESSED, UT_KEY_1_PRESSED_SLOT, ut_slot_key_1_pressed)

static void ut_slot_key_1_down(const void* p_arg) {
    DEBUG_PASS("ut_slot_key_1_down()");
    counter_SIGNAL_KEY_1_DOWN += 1;
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_1_DOWN, UT_KEY_1_DOWN_SLOT, ut_slot_key_1_down)

static void ut_slot_key_1_released(const void* p_arg) {
    DEBUG_PASS("ut_slot_key_1_released()");
    counter_SIGNAL_KEY_1_RELEASED += 1;
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_1_RELEASED, UT_KEY_1_RELEASED_SLOT, ut_slot_key_1_released)

// --------------------------------------------------------------------------------

static void ut_slot_key_5_pressed(const void* p_arg) {
    DEBUG_PASS("ut_slot_key_5_pressed()");
    counter_SIGNAL_KEY_5_PRESSED += 1;
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_5_PRESSED, UT_KEY_5_PRESSED_SLOT, ut_slot_key_5_pressed)

static void ut_slot_key_5_down(const void* p_arg) {
    DEBUG_PASS("ut_slot_key_5_down()");
    counter_SIGNAL_KEY_5_DOWN += 1;
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_5_DOWN, UT_KEY_5_DOWN_SLOT, ut_slot_key_5_down)

static void ut_slot_key_5_released(const void* p_arg) {
    DEBUG_PASS("ut_slot_key_5_released()");
    counter_SIGNAL_KEY_5_RELEASED += 1;
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_5_RELEASED, UT_KEY_5_RELEASED_SLOT, ut_slot_key_5_released)

// --------------------------------------------------------------------------------

static void ut_slot_key_9_pressed(const void* p_arg) {
    DEBUG_PASS("ut_slot_key_9_pressed()");
    counter_SIGNAL_KEY_9_PRESSED += 1;
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_9_PRESSED, UT_KEY_9_PRESSED_SLOT, ut_slot_key_9_pressed)

static void ut_slot_key_9_down(const void* p_arg) {
    DEBUG_PASS("ut_slot_key_9_down()");
    counter_SIGNAL_KEY_9_DOWN += 1;
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_9_DOWN, UT_KEY_9_DOWN_SLOT, ut_slot_key_9_down)

static void ut_slot_key_9_released(const void* p_arg) {
    DEBUG_PASS("ut_slot_key_9_released()");
    counter_SIGNAL_KEY_9_RELEASED += 1;
}

SIGNAL_SLOT_INTERFACE_CREATE_SLOT(KEY_9_RELEASED, UT_KEY_9_RELEASED_SLOT, ut_slot_key_9_released)

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

    counter_SIGNAL_KEY_1_PRESSED = 0;
    counter_SIGNAL_KEY_1_DOWN = 0;
    counter_SIGNAL_KEY_1_RELEASED = 0;

    counter_SIGNAL_KEY_5_PRESSED = 0;
    counter_SIGNAL_KEY_5_DOWN = 0;
    counter_SIGNAL_KEY_5_RELEASED = 0;

    counter_SIGNAL_KEY_9_PRESSED = 0;
    counter_SIGNAL_KEY_9_DOWN = 0;
    counter_SIGNAL_KEY_9_RELEASED = 0;

    counter_SIGNAL_KEY_0_PRESSED = 0;
    counter_SIGNAL_KEY_0_DOWN = 0;
    counter_SIGNAL_KEY_0_RELEASED = 0;
    
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

void KEY_COL_1_activate(void) {
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

void KEY_COL_2_activate(void) {
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

void KEY_COL_3_activate(void) {
}

void KEY_ROW_1_pull_down(void) {
    counter_KEY_ROW_1_pull_down += 1;
}

u8 KEY_ROW_1_is_high_level(void) {
    counter_KEY_ROW_1_IS_HIGH_LEVEL += 1;

    return  ( ut_keys_pressed.key_1 && KEY_COL_1_IS_HIGH)
         || ( ut_keys_pressed.key_2 && KEY_COL_2_IS_HIGH)
         || ( ut_keys_pressed.key_3 && KEY_COL_3_IS_HIGH);
}

void KEY_ROW_1_activate(void) {
}

void KEY_ROW_2_pull_down(void) {
    counter_KEY_ROW_2_pull_down += 1;
}

u8 KEY_ROW_2_is_high_level(void) {
    counter_KEY_ROW_2_IS_HIGH_LEVEL += 1;
    
    return  ( ut_keys_pressed.key_4 && KEY_COL_1_IS_HIGH)
         || ( ut_keys_pressed.key_5 && KEY_COL_2_IS_HIGH)
         || ( ut_keys_pressed.key_6 && KEY_COL_3_IS_HIGH);
}

void KEY_ROW_2_activate(void) {
}

void KEY_ROW_3_pull_down(void) {
    counter_KEY_ROW_3_pull_down += 1;
}

u8 KEY_ROW_3_is_high_level(void) {
    counter_KEY_ROW_3_IS_HIGH_LEVEL += 1;
    
    return  ( ut_keys_pressed.key_7 && KEY_COL_1_IS_HIGH)
         || ( ut_keys_pressed.key_8 && KEY_COL_2_IS_HIGH)
         || ( ut_keys_pressed.key_9 && KEY_COL_3_IS_HIGH);
}

void KEY_ROW_3_activate(void) {
}

void KEY_ROW_4_pull_down(void) {
    counter_KEY_ROW_4_pull_down += 1;
}

u8 KEY_ROW_4_is_high_level(void) {
    counter_KEY_ROW_4_IS_HIGH_LEVEL += 1;
    
    return  ( ut_keys_pressed.key_raute && KEY_COL_1_IS_HIGH)
         || ( ut_keys_pressed.key_0 && KEY_COL_2_IS_HIGH)
         || ( ut_keys_pressed.key_star && KEY_COL_3_IS_HIGH);
}

void KEY_ROW_4_activate(void) {
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

        keypad_init();

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

        UNITTEST_TIMER_start();

        while (UNITTEST_TIMER_is_up(1000) == 0) {
            mcu_task_controller_schedule();

            if (ut_keys_pressed.key_1) {
                if (counter_SIGNAL_KEY_1_DOWN) {
                    ut_keys_pressed.key_1 = 0;
                }
            } else {
                if (counter_SIGNAL_KEY_1_RELEASED) {
                    break;
                }
            }
        }

        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_0_PRESSED, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_0_DOWN, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_0_RELEASED, 0);

        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_1_PRESSED, 1);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_1_DOWN, 1);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_1_RELEASED, 1);

        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_5_PRESSED, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_5_DOWN, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_5_RELEASED, 0);

        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_9_PRESSED, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_9_DOWN, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_9_RELEASED, 0);

        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_DRIVE_HIGH, 10);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_NO_DRIVE, 10);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_DRIVE_HIGH, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_NO_DRIVE, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_DRIVE_HIGH, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_NO_DRIVE, 9);

        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_IS_HIGH_LEVEL, 28);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_IS_HIGH_LEVEL, 27);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_IS_HIGH_LEVEL, 27);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_IS_HIGH_LEVEL, 27);

        UT_CHECK_IS_EQUAL(counter_INVALID_PIN_DRIVE, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_1_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_2_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_3_IS_HIGH, 0);

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

        UNITTEST_TIMER_start();
        while (UNITTEST_TIMER_is_up(1000) == 0) {
            mcu_task_controller_schedule();

            if (ut_keys_pressed.key_5) {
                if (counter_SIGNAL_KEY_5_DOWN) {
                    ut_keys_pressed.key_5 = 0;
                }
            } else {
                if (counter_SIGNAL_KEY_5_RELEASED) {
                    break;
                }
            }
        }

        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_0_PRESSED, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_0_DOWN, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_0_RELEASED, 0);

        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_1_PRESSED, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_1_DOWN, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_1_RELEASED, 0);

        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_5_PRESSED, 1);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_5_DOWN, 1);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_5_RELEASED, 1);

        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_9_PRESSED, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_9_DOWN, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_9_RELEASED, 0);

        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_DRIVE_HIGH, 10);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_NO_DRIVE, 10);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_DRIVE_HIGH, 10);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_NO_DRIVE, 10);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_DRIVE_HIGH, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_NO_DRIVE, 9);

        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_IS_HIGH_LEVEL, 29);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_IS_HIGH_LEVEL, 29);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_IS_HIGH_LEVEL, 28);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_IS_HIGH_LEVEL, 28);

        UT_CHECK_IS_EQUAL(counter_INVALID_PIN_DRIVE, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_1_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_2_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_3_IS_HIGH, 0);

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

        UNITTEST_TIMER_start();
        while (UNITTEST_TIMER_is_up(1000) == 0) {
            mcu_task_controller_schedule();

            if (ut_keys_pressed.key_9) {
                if (counter_SIGNAL_KEY_9_DOWN) {
                    ut_keys_pressed.key_9 = 0;
                }
            } else {
                if (counter_SIGNAL_KEY_9_RELEASED) {
                    break;
                }
            }
        }

        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_0_PRESSED, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_0_DOWN, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_0_RELEASED, 0);

        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_1_PRESSED, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_1_DOWN, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_1_RELEASED, 0);

        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_5_PRESSED, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_5_DOWN, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_5_RELEASED, 0);

        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_9_PRESSED, 1);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_9_DOWN, 1);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_9_RELEASED, 1);

        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_DRIVE_HIGH, 10);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_NO_DRIVE, 10);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_DRIVE_HIGH, 10);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_NO_DRIVE, 10);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_DRIVE_HIGH, 10);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_NO_DRIVE, 10);

        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_IS_HIGH_LEVEL, 30);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_IS_HIGH_LEVEL, 30);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_IS_HIGH_LEVEL, 30);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_IS_HIGH_LEVEL, 29);

        UT_CHECK_IS_EQUAL(counter_INVALID_PIN_DRIVE, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_1_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_2_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_3_IS_HIGH, 0);

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

        UNITTEST_TIMER_start();
        while (UNITTEST_TIMER_is_up(1000) == 0) {
            mcu_task_controller_schedule();

            if (ut_keys_pressed.key_0) {
                if (counter_SIGNAL_KEY_0_DOWN) {
                    ut_keys_pressed.key_0 = 0;
                }
            } else {
                if (counter_SIGNAL_KEY_0_RELEASED) {
                    break;
                }
            }
        }

        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_0_PRESSED, 1);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_0_DOWN, 1);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_0_RELEASED, 1);

        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_1_PRESSED, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_1_DOWN, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_1_RELEASED, 0);

        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_5_PRESSED, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_5_DOWN, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_5_RELEASED, 0);

        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_9_PRESSED, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_9_DOWN, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_9_RELEASED, 0);

        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_DRIVE_HIGH, 10);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_NO_DRIVE, 10);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_DRIVE_HIGH, 10);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_NO_DRIVE, 10);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_DRIVE_HIGH, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_NO_DRIVE, 9);

        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_IS_HIGH_LEVEL, 29);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_IS_HIGH_LEVEL, 29);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_IS_HIGH_LEVEL, 29);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_IS_HIGH_LEVEL, 29);

        UT_CHECK_IS_EQUAL(counter_INVALID_PIN_DRIVE, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_1_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_2_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_3_IS_HIGH, 0);

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

        UNITTEST_TIMER_start();
        while (UNITTEST_TIMER_is_up(1000) == 0) {
            mcu_task_controller_schedule();
        }

        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_0_PRESSED, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_0_DOWN, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_0_RELEASED, 0);

        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_1_PRESSED, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_1_DOWN, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_1_RELEASED, 0);

        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_5_PRESSED, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_5_DOWN, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_5_RELEASED, 0);

        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_9_PRESSED, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_9_DOWN, 0);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_9_RELEASED, 0);

        UT_CHECK_IS_GREATER(counter_KEY_COL_1_DRIVE_HIGH, 10);
        UT_CHECK_IS_GREATER(counter_KEY_COL_1_NO_DRIVE, 10);
        UT_CHECK_IS_GREATER(counter_KEY_COL_2_DRIVE_HIGH, 10);
        UT_CHECK_IS_GREATER(counter_KEY_COL_2_NO_DRIVE, 10);
        UT_CHECK_IS_GREATER(counter_KEY_COL_3_DRIVE_HIGH, 10);
        UT_CHECK_IS_GREATER(counter_KEY_COL_3_NO_DRIVE, 10);

        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_pull_down, 0);
        UT_CHECK_IS_GREATER(counter_KEY_ROW_1_IS_HIGH_LEVEL, 30);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_pull_down, 0);
        UT_CHECK_IS_GREATER(counter_KEY_ROW_2_IS_HIGH_LEVEL, 30);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_pull_down, 0);
        UT_CHECK_IS_GREATER(counter_KEY_ROW_3_IS_HIGH_LEVEL, 30);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_pull_down, 0);
        UT_CHECK_IS_GREATER(counter_KEY_ROW_4_IS_HIGH_LEVEL, 30);

        UT_CHECK_IS_EQUAL(counter_INVALID_PIN_DRIVE, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_1_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_2_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_3_IS_HIGH, 0);

    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

/**
 * @brief The key's 0 5 6 9 and star are pressed
 * 
 */
static void TEST_CASE_key_pressed_0_5_6_9_star(void) {

    UT_START_TEST_CASE("Key Pressed 0 5 6 9 star")
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

        UNITTEST_TIMER_start();
        while (UNITTEST_TIMER_is_up(1000) == 0) {
            mcu_task_controller_schedule();

            if (ut_keys_pressed.key_0) {
                if (counter_SIGNAL_KEY_0_DOWN) {
                    ut_keys_pressed.key_0 = 0;
                }
            }

            if (ut_keys_pressed.key_5) {
                if (counter_SIGNAL_KEY_5_DOWN) {
                    ut_keys_pressed.key_5 = 0;
                }
            }

            if (ut_keys_pressed.key_9) {
                if (counter_SIGNAL_KEY_9_DOWN) {
                    ut_keys_pressed.key_9 = 0;
                }
            }

            if (ut_keys_pressed.key_0 == 0) {
                if (ut_keys_pressed.key_5 == 0) {
                    if (ut_keys_pressed.key_9 == 0) {

                        if (counter_SIGNAL_KEY_0_RELEASED) {
                            if (counter_SIGNAL_KEY_5_RELEASED) {
                                if (counter_SIGNAL_KEY_9_RELEASED) {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_0_PRESSED, 1);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_0_DOWN, 1);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_0_RELEASED, 1);

        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_5_PRESSED, 1);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_5_DOWN, 1);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_5_RELEASED, 1);

        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_9_PRESSED, 1);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_9_DOWN, 1);
        UT_CHECK_IS_EQUAL(counter_SIGNAL_KEY_9_RELEASED, 1);

        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_DRIVE_HIGH, 10);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_1_NO_DRIVE, 10);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_DRIVE_HIGH, 10);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_2_NO_DRIVE, 10);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_DRIVE_HIGH, 9);
        UT_CHECK_IS_EQUAL(counter_KEY_COL_3_NO_DRIVE, 9);

        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_1_IS_HIGH_LEVEL, 29);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_2_IS_HIGH_LEVEL, 29);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_3_IS_HIGH_LEVEL, 28);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_pull_down, 0);
        UT_CHECK_IS_EQUAL(counter_KEY_ROW_4_IS_HIGH_LEVEL, 28);

        UT_CHECK_IS_EQUAL(counter_INVALID_PIN_DRIVE, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_1_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_2_IS_HIGH, 0);
        UT_CHECK_IS_EQUAL(KEY_COL_3_IS_HIGH, 0);

    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

    UT_KEY_0_PRESSED_SLOT_connect();
    UT_KEY_0_DOWN_SLOT_connect();
    UT_KEY_0_RELEASED_SLOT_connect();

    UT_KEY_1_PRESSED_SLOT_connect();
    UT_KEY_1_DOWN_SLOT_connect();
    UT_KEY_1_RELEASED_SLOT_connect();

    UT_KEY_5_PRESSED_SLOT_connect();
    UT_KEY_5_DOWN_SLOT_connect();
    UT_KEY_5_RELEASED_SLOT_connect();

    UT_KEY_9_PRESSED_SLOT_connect();
    UT_KEY_9_DOWN_SLOT_connect();
    UT_KEY_9_RELEASED_SLOT_connect();

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
