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
 * @file    key_pad_driver.h
 * @author  Sebastian Lesse
 * @date    2022 / 12 / 18
 * @brief   Short description of this file
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_key_pad_driver_
#define _H_key_pad_driver_

// --------------------------------------------------------------------------------

/**
 * @brief Structure to get all keys that are currently active.
 * If a key is pressed the field is set to 1, otherwise it is set to 0.
 * 
 */
typedef struct {
    u8 key_0;
    u8 key_1;
    u8 key_2;
    u8 key_3;
    u8 key_4;
    u8 key_5;
    u8 key_6;
    u8 key_7;
    u8 key_8;
    u8 key_9;
    u8 key_star;
    u8 key_raute;
} KEY_PAD_KEYS;

// --------------------------------------------------------------------------------

/**
 * @brief Initializes the key-pad driver.
 * 
 */
void key_pad_driver_init(void);

// --------------------------------------------------------------------------------

/**
 * @brief Checks if any key is pressed.
 * Is used to get an overview if at elast one key
 * is active right now.
 * This fucntion does not perform any de-bouncing.
 * 
 * @return 1 if at least one key is active, otherwise 0.
 */
u8 key_pad_driver_is_key_pressed(void);

// --------------------------------------------------------------------------------

/**
 * @brief Looks for active keys.
 * This fucntion performs debouncing.
 * This function only updates the key-fields of p_keys
 * if the key is active. Otherwise its value is left unchanged.
 * This fucntion blocks until all key values are read.
 * 
 * @param p_keys 
 */
void key_pad_driver_get_keys(KEY_PAD_KEYS* p_keys);

// --------------------------------------------------------------------------------

#endif // _H_key_pad_driver_

// --------------------------------------------------------------------------------
