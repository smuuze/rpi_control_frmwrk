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
 * @file    lcd_driver_interface.h
 * @author  Sebastian Lesse
 * @date    2022 / 12 / 17
 * @brief   Interface of a character based LCD
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_lcd_driver_interface_
#define _H_lcd_driver_interface_

// --------------------------------------------------------------------------------

/**
 * @brief Used as option for lcd_driver_update_screen.
 * This causes the driver to update only one character of the last line per call.
 * This enables an smooth effect of updating the lcd content.
 */
#define LCD_DRIVER_UPDATE_MODE_LAST_LINE_SMOOTH         (1 << 0)

//-----------------------------------------------------------------------------

#define LCD_LINE_ONE        0
#define LCD_LINE_TWO        1

// --------------------------------------------------------------------------------

/**
 * @brief Executes the initialization routine of the currently used LCD.
 * Depending on the LCD this routine will need some time, e.g. 15+ milliseconds
 * for a HD44780. This fucntion block until initialization is done.
 */
void lcd_driver_init(void);

// --------------------------------------------------------------------------------

/**
 * @brief 
 */
void lcd_driver_deinit(void);

// --------------------------------------------------------------------------------

/**
 * @brief Updates the screen buffer.
 * The content will be shifted up by one line.
 * The new line is set at the end.
 * This function does not update the LCD.
 * if the message is too long, the characters that does not fit
 * will be ignored and discarded.
 * 
 * @param message string to set as last line
 * @param length number of characters of message.
 */
void lcd_driver_set_line(const char* message, u8 length);

// --------------------------------------------------------------------------------

/**
 * @brief Updates the content of the LCD.
 * This function will write the current line buffer to the LCD.
 * 
 * @param mode 
 * @return 1 if the content was update, otherwise 0
 */
u8 lcd_driver_update_screen(u8 mode);

// --------------------------------------------------------------------------------

/**
 * @brief Get the number of lines of the current LCD.
 * 
 * @return number of lines of the current LCD.
 */
u8 lcd_driver_line_count(void);

// --------------------------------------------------------------------------------

/**
 * @brief Number of characters of a single line.
 * 
 * @return Number of characters of a single line.
 */
u8 lcd_driver_character_count(void);

// --------------------------------------------------------------------------------

#endif // _H_lcd_driver_interface_

// --------------------------------------------------------------------------------
