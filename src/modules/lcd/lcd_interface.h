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
 * @file    lcd_interface.h
 * @author  Sebastian Lesse
 * @date    2022 / 06 / 06
 * @brief   Interface to a character based lcd controller
 * 
 */

//-----------------------------------------------------------------------------

#ifndef _H_lcd_interface_
#define _H_lcd_interface_

//-----------------------------------------------------------------------------

#include "config.h"

//-----------------------------------------------------------------------------

#include "common/signal_slot_interface.h"

//-----------------------------------------------------------------------------

#define LCD_ENABLE          1
#define LCD_DISABLE         0

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
 */
typedef enum {
    LCD_REFRESH_MODE_SMOOTH,
    LCD_REFRESH_MODE_DIRECT
} LCD_REFRESH_MODE;

//-----------------------------------------------------------------------------

/**
 * @brief 
 */
typedef struct {
    LCD_REFRESH_MODE refresh_mode;
} LCD_CONFIGUREATION;

//-----------------------------------------------------------------------------

/**
 * @brief Initializes the LCD controller.
 * This function must be called before using the LCD.
 * It shall only be called once.
 */
void lcd_init(void);

/**
 * @brief Enables or disables the LCD at all.
 * 
 * @param enabled 1: the lcd is enabled, 0 the lcd is disabled.
 */
void lcd_set_enabled(u8 enabled);

/**
 * @brief Number of lines of the available lcd
 * 
 * @return Number of lines of the available lcd
 */
u8 lcd_get_line_count(void);

/**
 * @brief Number of characters of a single line of the available lcd
 * 
 * @return Number of characters of a single line of the available lcd
 */
u8 lcd_get_character_count(void);

/**
 * @brief 
 * 
 * @param p_lcd_cfg 
 */
void lcd_configure(LCD_CONFIGUREATION* p_lcd_cfg);

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

#endif // _H_lcd_interface_

//-----------------------------------------------------------------------------
