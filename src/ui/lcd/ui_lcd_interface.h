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
 * @file    ui_lcd_interface.h
 * @author  Sebastian Lesse
 * @date    2022 / 06 / 06
 * @brief   Short description of this file
 * 
 */

//-----------------------------------------------------------------------------

#ifndef _H_ui_lcd_interface_
#define _H_ui_lcd_interface_

//-----------------------------------------------------------------------------

#include "config.h"

//-----------------------------------------------------------------------------

#include "common/signal_slot_interface.h"

//-----------------------------------------------------------------------------

#define LCD_ENABLE          1
#define LCD_DISABLE         0

//-----------------------------------------------------------------------------

#define LCD_LINE_ONE        0
#define LCD_LINE_TWO        1

//-----------------------------------------------------------------------------

/**
 * @brief Signals a new Line to write on the connected LCD
 * One line per Signal can be send. The supported length of the text-line
 * depends on the currently used LCD. Characters that does not fit will be discarded.
 * The new line must end with the termination character '\0'
 */
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(SIGNAL_LCD_LINE)

/**
 * @brief The LCD controller will send this signal
 * after the content of the display has been updated
 * and there are no new messages available.
 * This signal does not has any arguments
 */
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(SIGNAL_LCD_UPDATED)

//-----------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
void lcd_controller_init(void);

/**
 * @brief Enables or disables the LCD at all.
 * 
 * @param enabled 1: the lcd is enabled, 0 the lcd is disabled.
 */
void lcd_controller_set_enabled(u8 enabled);

/**
 * @brief Number of lines of the available lcd
 * 
 * @return Number of lines of the available lcd
 */
u8 lcd_controller_get_line_count(void);

/**
 * @brief Number of characters of a single line of the available lcd
 * 
 * @return Number of characters of a single line of the available lcd
 */
u8 lcd_controller_get_character_count(void);

// /**
//  * @brief 
//  * 
//  */
// void lcd_deinit(void);

// /**
//  * @brief Writes the given line on the connected LCD.
//  * The line is always written at the end of the LCD, the last line.
//  * All other lines are shifted up by one line each.
//  * It is only possible to write one line per call.
//  * If the message is too long, the remaining characters are ignored
//  * 
//  * @param message the message to write on the LCD
//  */
// void lcd_write_line(const char* message);

// /**
//  * @brief Get the number of lines of the actual connected LCD
//  * 
//  * @return number of lines of the LCD
//  */
// u8 lcd_line_count(void);

// /**
//  * @brief Get the number of character per line.
//  * 
//  * @return Number of characters per line
//  */
// u8 lcd_character_count(void);

//-----------------------------------------------------------------------------

/**
 * @brief Helper macros to use the LCD
 * without being sure that this module is available
 */

#ifdef LCD_AVAILABLE

#define LCD_INIT()          lcd_controller_init()
#define LCD_ENABLE(en)      lcd_controller_set_enabled(en)
#define LCD_PRINT(p_str)    {                               \
                                SIGNAL_LCD_LINE_send(p_str) \
                            }
#else

#define LCD_INIT(a)         do{}while(0)
#define LCD_PRINT(p_str)    do{}while(0)

#endif

//-----------------------------------------------------------------------------

#endif // _H_ui_lcd_interface_

//-----------------------------------------------------------------------------
