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
 * @file    keypad_driver_3x4.c
 * @author  Sebastian Lesse
 * @date    2022 / 12 / 18
 * @brief   This driver contrls a 3x4 keypad with 12 keys.
 *
 *      The following schematic gives an overview how the
 *      this driver handles the keypad.
 * 
 *      COL_1   COL_2   COL_3
 *      |       |       |
 *      1 ----- 2 ----- 3 -------- ROW_1
 *      |       |       |
 *      4 ----- 5 ----- 6 -------- ROW_2
 *      |       |       |
 *      7 ----- 8 ----- 9 -------- ROW_3
 *      |       |       |
 *      # ----- 0 ----- * -------- ROW_4
 * 
 *      The pins COL_1 to COL_3 are used as output and the pins
 *      ROW_1 to ROW_4 are used as inputs.
 * 
 *      The pins ROW_1 to ROW_4 are connected to a pulldown resistor each.
 *      If there is no high level available on the pins COL_1 to COL_3
 *      there is a low level read, even if there is a key is pressed.
 *      If a high level is applied to COL_1 and KEY_1 is pressed there
 *      is a high level detected on ROW_1.
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

#include "driver/keypad/keypad_driver.h"
#include "driver/gpio/gpio_interface.h"

// --------------------------------------------------------------------------------

/**
 * @brief Every key is read for threee times to do a debounce
 * This value is used as threshold to detect a key as pressed
 */
#define KEYPAD_3x4_MIN_DEBOUNCE_VALUE                                          2

// --------------------------------------------------------------------------------

/**
 *
 */

INCLUDE_GPIO(KEY_COL_1)
INCLUDE_GPIO(KEY_COL_2)
INCLUDE_GPIO(KEY_COL_3)

INCLUDE_GPIO(KEY_ROW_1)
INCLUDE_GPIO(KEY_ROW_2)
INCLUDE_GPIO(KEY_ROW_3)
INCLUDE_GPIO(KEY_ROW_4)

// --------------------------------------------------------------------------------

/**
 * @see keypad_driver.h#key_pad_driver_init
 */
void keypad_driver_init(void) {

    DEBUG_PASS("keypad_driver_init()");

    KEY_COL_1_no_drive();
    KEY_COL_2_no_drive();
    KEY_COL_3_no_drive();
    
    KEY_ROW_1_pull_down();
    KEY_ROW_2_pull_down();
    KEY_ROW_3_pull_down();
    KEY_ROW_4_pull_down();
}

// --------------------------------------------------------------------------------

/**
 * @see keypad_driver.h#key_pad_driver_is_key_pressed
 */
u8 keypad_driver_is_key_pressed(void) {

    DEBUG_PASS("keypad_driver_is_key_pressed()");

    /**
     * @brief To check if there is any key pressed
     * we set all COL pins to high level and check
     * the ROW pins for a high level.
     * 
     * We do not activate all COL pins at once to avoid
     * interconnect of them.
     */

    KEY_COL_1_drive_high();
    u8 key_pressed  =
        KEY_ROW_1_is_high_level()
        ||  KEY_ROW_2_is_high_level()
        ||  KEY_ROW_3_is_high_level()
        ||  KEY_ROW_4_is_high_level();
    KEY_COL_1_no_drive();

    if (key_pressed) {
        return 1;
    }

    KEY_COL_2_drive_high();
    key_pressed  =
        KEY_ROW_1_is_high_level()
        ||  KEY_ROW_2_is_high_level()
        ||  KEY_ROW_3_is_high_level()
        ||  KEY_ROW_4_is_high_level();
    KEY_COL_1_no_drive();

    if (key_pressed) {
        return 1;
    }

    KEY_COL_3_drive_high();
    key_pressed  =
        KEY_ROW_1_is_high_level()
        ||  KEY_ROW_2_is_high_level()
        ||  KEY_ROW_3_is_high_level()
        ||  KEY_ROW_4_is_high_level();
    KEY_COL_1_no_drive();

    if (key_pressed) {
        return 1;
    }

    return 0;
}

// --------------------------------------------------------------------------------

/**
 * @see keypad_driver.h#key_pad_driver_get_keys
 */
void keypad_driver_get_keys(KEYPAD_KEYS* p_keys) {

    DEBUG_PASS("keypad_driver_get_keys()");

    memset(p_keys, 0x00, sizeof(KEYPAD_KEYS));
    u8 counter = 0;

    do {

        KEY_COL_1_drive_high();
        
        if (KEY_ROW_1_is_high_level()) { p_keys->key_1 += 1; }
        if (KEY_ROW_2_is_high_level()) { p_keys->key_4 += 1; }
        if (KEY_ROW_3_is_high_level()) { p_keys->key_7 += 1; }
        if (KEY_ROW_4_is_high_level()) { p_keys->key_raute += 1; }

        KEY_COL_1_no_drive();
        KEY_COL_2_drive_high();

        if (KEY_ROW_1_is_high_level()) { p_keys->key_2 += 1; }
        if (KEY_ROW_2_is_high_level()) { p_keys->key_5 += 1; }
        if (KEY_ROW_3_is_high_level()) { p_keys->key_8 += 1; }
        if (KEY_ROW_4_is_high_level()) { p_keys->key_0 += 1; }

        KEY_COL_2_no_drive();
        KEY_COL_3_drive_high();

        if (KEY_ROW_1_is_high_level()) { p_keys->key_3 += 1; }
        if (KEY_ROW_2_is_high_level()) { p_keys->key_6 += 1; }
        if (KEY_ROW_3_is_high_level()) { p_keys->key_9 += 1; }
        if (KEY_ROW_4_is_high_level()) { p_keys->key_star += 1; }

        KEY_COL_3_no_drive();

    } while (++counter < 3);

    p_keys->key_0 = (p_keys->key_0 > KEYPAD_3x4_MIN_DEBOUNCE_VALUE) ? 1 : 0;
    p_keys->key_1 = (p_keys->key_1 > KEYPAD_3x4_MIN_DEBOUNCE_VALUE) ? 1 : 0;
    p_keys->key_2 = (p_keys->key_2 > KEYPAD_3x4_MIN_DEBOUNCE_VALUE) ? 1 : 0;
    p_keys->key_3 = (p_keys->key_3 > KEYPAD_3x4_MIN_DEBOUNCE_VALUE) ? 1 : 0;
    p_keys->key_4 = (p_keys->key_4 > KEYPAD_3x4_MIN_DEBOUNCE_VALUE) ? 1 : 0;
    p_keys->key_5 = (p_keys->key_5 > KEYPAD_3x4_MIN_DEBOUNCE_VALUE) ? 1 : 0;
    p_keys->key_6 = (p_keys->key_6 > KEYPAD_3x4_MIN_DEBOUNCE_VALUE) ? 1 : 0;
    p_keys->key_7 = (p_keys->key_7 > KEYPAD_3x4_MIN_DEBOUNCE_VALUE) ? 1 : 0;
    p_keys->key_8 = (p_keys->key_8 > KEYPAD_3x4_MIN_DEBOUNCE_VALUE) ? 1 : 0;
    p_keys->key_9 = (p_keys->key_9 > KEYPAD_3x4_MIN_DEBOUNCE_VALUE) ? 1 : 0;

    p_keys->key_star = (p_keys->key_star > KEYPAD_3x4_MIN_DEBOUNCE_VALUE) ? 1 : 0;
    p_keys->key_raute = (p_keys->key_raute > KEYPAD_3x4_MIN_DEBOUNCE_VALUE) ? 1 : 0;
}

// --------------------------------------------------------------------------------
