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
 * @file    ui_lcd_16x2.c
 * @author  Sebastian Lesse
 * @date    2022 / 06 / 05
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

#include "ui/lcd/ui_lcd_interface.h"

// --------------------------------------------------------------------------------

// helping definitions for fast access to pins via function lcd_set_pins()
#define LCD_PIN_RS				(1 << 7)
#define LCD_PIN_D4				(1 << 0)
#define LCD_PIN_D5				(1 << 1)
#define LCD_PIN_D6				(1 << 2)
#define LCD_PIN_D7				(1 << 3)

#define LCD_NUM_LINES				2
#define LCD_NUM_CHARS				16

// --------------------------------------------------------------------------------

static char line_buffer[LCD_NUM_LINES][LCD_NUM_CHARS + 1];

static u8 is_initialized = 0;
static u8 is_enabled = 0;

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param pins 
 */
static void lcd_set_pins(u8 pins) {
	
	if (pins & LCD_PIN_RS) LCD_RS_drive_high();  else  LCD_RS_drive_low();

	if (pins & LCD_PIN_D4) LCD_D4_drive_high();  else  LCD_D4_drive_low();
	if (pins & LCD_PIN_D5) LCD_D5_drive_high();  else  LCD_D5_drive_low();
	if (pins & LCD_PIN_D6) LCD_D6_drive_high();  else  LCD_D6_drive_low();
	if (pins & LCD_PIN_D7) LCD_D7_drive_high();  else  LCD_D7_drive_low();

	usleep(50); // reduce cpu-load

	LCD_EN_drive_high();
	usleep(50); // reduce cpu-load
	LCD_EN_drive_low();
}

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param line_index 
 */
static void lcd_select_line(u8 line_index) {

	switch (line_index) {
		default :
		case 0 : // Line 1
			lcd_set_pins(LCD_PIN_D7 | 0x00);
			lcd_set_pins(0x00);
			break;

		case 1 : // Line 2
			lcd_set_pins(LCD_PIN_D7 | LCD_PIN_D6);
			lcd_set_pins(0);
			break;
	}
}

// --------------------------------------------------------------------------------

void lcd_write_char(char character) {
	lcd_set_pins(LCD_PIN_RS | (u8)(character >> 4));
	lcd_set_pins(LCD_PIN_RS | (u8)(character & 0x0F));
}

// --------------------------------------------------------------------------------

void lcd_init(void) {

	if (is_enabled == 0) {
		DEBUG_PASS("lcd_init() - LCD is not enabled!");
		return;
	}
	
	DEBUG_PASS("lcd_init() - activate");

	LCD_RS_activate();	LCD_RS_drive_low();
	LCD_EN_activate();	LCD_EN_drive_low();

	LCD_D4_activate();	LCD_D4_drive_low();
	LCD_D5_activate();	LCD_D5_drive_low();
	LCD_D6_activate();	LCD_D6_drive_low();
	LCD_D7_activate();	LCD_D7_drive_low();

	DEBUG_PASS("lcd_init() - running init sequence");

	// -----------------------------------------------------

	usleep(15 * 1000); // wait for LCD controller power-up

	lcd_set_pins(LCD_PIN_D4 | LCD_PIN_D5); 	usleep(5 * 1000);
	lcd_set_pins(LCD_PIN_D4 | LCD_PIN_D5); 	usleep(100);
	lcd_set_pins(LCD_PIN_D4 | LCD_PIN_D5); 	usleep(5 * 1000);

	lcd_set_pins(LCD_PIN_D5); 		usleep(5 * 1000); 

	// -----------------------------------------------------	

	lcd_set_pins(LCD_PIN_D5);					// switch to 4-Bit interface
	lcd_set_pins(LCD_PIN_D7);					// 2 Lines / 5x8 Font

	lcd_set_pins(0);						// cmd control
	lcd_set_pins(LCD_PIN_D7 | LCD_PIN_D6);				// enable display	

	lcd_set_pins(0);						
	lcd_set_pins(LCD_PIN_D4);					// display clear

	lcd_set_pins(0);						// Entry mode set
	lcd_set_pins(LCD_PIN_D6 | LCD_PIN_D5);				// cursor increment

	usleep(2 * 1000);

	u8 line_cnt = 0;
	for ( ; line_cnt < LCD_NUM_LINES; line_cnt += 1) {
		memset(line_buffer[line_cnt], ' ', LCD_NUM_CHARS);
		line_buffer[line_cnt][LCD_NUM_CHARS] = '\0';
	}

	is_initialized = 1;
}

// --------------------------------------------------------------------------------

void lcd_deinit(void) {
	is_initialized = 0;
}

// --------------------------------------------------------------------------------

void lcd_set_enabled(u8 enabled) {
	is_enabled = enabled != LCD_DISABLE ? 1 : 0;
}

// --------------------------------------------------------------------------------

void lcd_write_line(char* message) {

	if (is_enabled == 0) {
		DEBUG_PASS("lcd_write_line() - LCD is not enabled!");
		return;
	}

	if (is_initialized == 0) {
		DEBUG_PASS("lcd_write_line() - Need to initialize LCD-Interface");
		lcd_init();
	}

	DEBUG_TRACE_STR(message, "lcd_write_line() - New Line:");

	u8 line_cnt = 0;
	u8 char_cnt = 0;

	for ( ; line_cnt < LCD_NUM_LINES - 1; line_cnt += 1) {
		for ( ; char_cnt < LCD_NUM_CHARS; char_cnt += 1) {
			line_buffer[line_cnt][char_cnt] = line_buffer[line_cnt + 1][char_cnt];
		}
	}

	u8 length = strlen(message);
	if (length > LCD_NUM_CHARS) {
		length = LCD_NUM_CHARS;
	}
		
	for (char_cnt = 0 ; char_cnt < length; char_cnt += 1) {
		line_buffer[LCD_NUM_LINES - 1][char_cnt] = message[char_cnt];
	}
		
	for ( ; char_cnt < LCD_NUM_CHARS; char_cnt += 1) {
		line_buffer[LCD_NUM_LINES - 1][char_cnt] = ' ';
	}

	DEBUG_PASS("lcd_write_line() - LCD-Content:");

	/*
	for (line_cnt = 0 ; line_cnt < LCD_NUM_LINES; line_cnt += 1) {
		printf("\t | %s |\n", line_buffer[line_cnt]);
	}
	*/

	for (line_cnt = 0; line_cnt < LCD_NUM_LINES; line_cnt += 1) {
		
		lcd_select_line(line_cnt);

		for (char_cnt = 0 ; char_cnt < LCD_NUM_CHARS; char_cnt += 1) {
			lcd_write_char(line_buffer[line_cnt][char_cnt]);
		}
	}
}

// --------------------------------------------------------------------------------

u8 lcd_line_count(void) {
    return LCD_NUM_LINES;
}

// --------------------------------------------------------------------------------

u8 lcd_character_count(void) {
    return LCD_NUM_CHARS;
}

// --------------------------------------------------------------------------------
