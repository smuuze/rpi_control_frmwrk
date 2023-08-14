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
 * @file    keypad_interface.h
 * @author  Sebastian Lesse
 * @date    2023 / 01 / 06
 * @brief   Controls a simple keypad with keys for number 0 to 9
 *          and two special keys (e.g. * and #)
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_keypad_interface_
#define _H_keypad_interface_

// --------------------------------------------------------------------------------

#include "signal_slot_interface.h"

// --------------------------------------------------------------------------------

/**
 * @brief These signals are send if one of the keys is detected
 * as pressed for the first time
 */
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_0_PRESSED)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_1_PRESSED)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_2_PRESSED)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_3_PRESSED)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_4_PRESSED)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_5_PRESSED)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_6_PRESSED)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_7_PRESSED)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_8_PRESSED)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_9_PRESSED)

/**
 * @brief These signals are send if a keys was
 * already detected as pressed and is currently still pressed
 */
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_0_DOWN)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_1_DOWN)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_2_DOWN)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_3_DOWN)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_4_DOWN)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_5_DOWN)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_6_DOWN)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_7_DOWN)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_8_DOWN)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_9_DOWN)

/**
 * @brief These signals are send if a key was pressed and is detected as
 * not pressed for the first time.
 */
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_0_RELEASED)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_1_RELEASED)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_2_RELEASED)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_3_RELEASED)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_4_RELEASED)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_5_RELEASED)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_6_RELEASED)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_7_RELEASED)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_8_RELEASED)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_9_RELEASED)

/**
 * @brief e.g. *
 */
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_SPECIAL_1_PRESSED)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_SPECIAL_1_DOWN)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_SPECIAL_1_RELEASED)

/**
 * @brief e.g. #
 */
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_SPECIAL_2_PRESSED)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_SPECIAL_2_DOWN)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(KEY_SPECIAL_2_RELEASED)

// --------------------------------------------------------------------------------

/**
 * @brief Initializes the keypad controller.
 * 
 */
void keypad_init(void);

// --------------------------------------------------------------------------------

#endif // _H_keypad_interface_

// --------------------------------------------------------------------------------
