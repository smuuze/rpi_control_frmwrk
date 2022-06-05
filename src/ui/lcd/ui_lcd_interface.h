/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	ui_lcd_interface.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */
 
#ifndef _UI_LCD_INTERFACE_H_
#define _UI_LCD_INTERFACE_H_

//-----------------------------------------------------------------------------

#include "config.h"

//-----------------------------------------------------------------------------

#define LCD_ENABLE          1
#define LCD_DISABLE         0

//-----------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
void lcd_init(void);

/**
 * @brief 
 * 
 */
void lcd_deinit(void);

/**
 * @brief 
 * 
 * @param message 
 */
void lcd_write_line(char* message);

/**
 * @brief 
 * 
 * @param enabled 
 */
void lcd_set_enabled(u8 enabled);

/**
 * @brief Get the number of lines of the actual connected LCD
 * 
 * @return number of lines of the LCD
 */
u8 lcd_line_count(void);

/**
 * @brief Get the number of character per line.
 * 
 * @return Number of characters per line
 */
u8 lcd_character_count(void);

//-----------------------------------------------------------------------------

#ifdef LCD_AVAILABLE
#define LCD_INIT(a)		lcd_set_enabled(a)
#define LCD_PRINT(p_str)	{				\
					lcd_write_line(p_str);	\
				}
#else
#define LCD_INIT(a)		do{}while(0)
#define LCD_PRINT(p_str)	do{}while(0)
#endif

//-----------------------------------------------------------------------------

#endif // _UI_LCD_INTERFACE_H_
