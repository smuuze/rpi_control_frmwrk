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
 * @file    system_interface.h
 * @author  Sebastian Lesse
 * @date    2022 / 07 / 26
 * @brief   Short description of this file
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_system_interface_
#define _H_system_interface_

// --------------------------------------------------------------------------------

#include BOARD_DESCRIPTION_FILE

// --------------------------------------------------------------------------------

#include "driver/gpio/gpio_interface.h"

// --------------------------------------------------------------------------------

/*
 */
typedef enum {

    SYS_EVT_NO_EVENT = 0x00,
    SYS_EVT_INPUT_CHANGED = 0x01

} SYSTEM_EVENT;

// --------------------------------------------------------------------------------

/*!
 *
 * @return
 */
typedef u8  (*SYSTEM_INTERFACE_GET_TIME_U8_CALLBACK)        (void);
typedef u16 (*SYSTEM_INTERFACE_GET_TIME_U16_CALLBACK)        (void);
typedef u32 (*SYSTEM_INTERFACE_GET_TIME_U32_CALLBACK)        (void);

/*!
 *
 */
typedef u8 (*SYSTEM_INTERFACE_IS_TIME_UP_U8_CALLBACK)        (u8 ref_time, u8 time_interval);
typedef u8 (*SYSTEM_INTERFACE_IS_TIME_UP_u16_CALLBACK)        (u16 ref_time, u16 time_interval);
typedef u8 (*SYSTEM_INTERFACE_IS_TIME_UP_U32_CALLBACK)        (u32 ref_time, u32 time_interval);

/*!
 *
 */
typedef struct {
    SYSTEM_INTERFACE_GET_TIME_U8_CALLBACK         now_u8;
    SYSTEM_INTERFACE_GET_TIME_U16_CALLBACK         now_u16;
    SYSTEM_INTERFACE_GET_TIME_U32_CALLBACK         now_u32;

    SYSTEM_INTERFACE_IS_TIME_UP_U8_CALLBACK        isup_u8;
    SYSTEM_INTERFACE_IS_TIME_UP_u16_CALLBACK     isup_u16;
    SYSTEM_INTERFACE_IS_TIME_UP_U32_CALLBACK     isup_u32;
} SYSTEM_INTERFACE_TIME;

// --------------------------------------------------------------------------------

/*!
 *
 */
typedef void (*SYSTEM_INTERFACE_ADD_EVENT_CALLBACK)        (SYSTEM_EVENT evt);
typedef SYSTEM_EVENT (*SYSTEM_INTERFACE_GET_EVENT_CALLBACK)    (void);

/*!
 *
 */
typedef struct {
    SYSTEM_INTERFACE_ADD_EVENT_CALLBACK     add;
    SYSTEM_INTERFACE_GET_EVENT_CALLBACK     get_next;
} SYSTEM_INTERFACE_EVENT;

// --------------------------------------------------------------------------------

/*!
 *
 */
typedef void (*SYSTEM_INTERFACE_IO_INIT_PIN_CALLBACK)                    (const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);
typedef void (*SYSTEM_INTERFACE_IO_SET_PIN_DIR_CALLBACK)                (const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, GPIO_DRIVER_DIRECTION direction);
typedef void (*SYSTEM_INTERFACE_IO_SET_PIN_LEVEL_CALLBACK)                  (const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr, GPIO_DRIVER_LEVEL level);
typedef void (*SYSTEM_INTERFACE_IO_TOGGLE_PIN_LEVEL_CALLBACK)               (const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);
typedef GPIO_DRIVER_LEVEL (*SYSTEM_INTERFACE_IO_GET_PIN_LEVEL_CALLBACK)     (const GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);

/*!
 *
 */
typedef struct {
    SYSTEM_INTERFACE_IO_INIT_PIN_CALLBACK           init_pin;
    SYSTEM_INTERFACE_IO_SET_PIN_DIR_CALLBACK        set_direction;
    SYSTEM_INTERFACE_IO_SET_PIN_LEVEL_CALLBACK      set_level;
    SYSTEM_INTERFACE_IO_TOGGLE_PIN_LEVEL_CALLBACK   toggle_level;
    SYSTEM_INTERFACE_IO_GET_PIN_LEVEL_CALLBACK      get_level;
} SYSTEM_INTERFACE_IO;

// --------------------------------------------------------------------------------

typedef u8   (*SYSTEM_INTERFACE_PROGMEM_GET_BYTE_CALLBACK)      (u8* addr);
typedef u16  (*SYSTEM_INTERFACE_PROGMEM_GET_WORD_CALLBACK)      (u8* addr);
typedef u32  (*SYSTEM_INTERFACE_PROGMEM_GET_LONG_CALLBACK)      (u8* addr);
typedef void (*SYSTEM_INTERFACE_PROGMEM_GET_N_BYTES_CALLBACK)   (u8* addr, u8* p_buffer_to, u8 num_bytes);


/*!
 *
 */
typedef struct {
    SYSTEM_INTERFACE_PROGMEM_GET_BYTE_CALLBACK      get_byte;
    SYSTEM_INTERFACE_PROGMEM_GET_WORD_CALLBACK      get_word;
    SYSTEM_INTERFACE_PROGMEM_GET_LONG_CALLBACK      get_long;
    SYSTEM_INTERFACE_PROGMEM_GET_N_BYTES_CALLBACK   get_N_bytes;
} SYSTEM_INTERFACE_MEMORY_PROGMEM;

/*!
 *
 */
typedef struct {
    SYSTEM_INTERFACE_MEMORY_PROGMEM    progmem;
} SYSTEM_INTERFACE_MEMORY;

// --------------------------------------------------------------------------------

#include "driver/trx_driver_interface.h"

/*!
 *
 */
typedef struct {
    TRX_DRIVER_INTERFACE*    spi0;
    TRX_DRIVER_INTERFACE*    usart0;
    TRX_DRIVER_INTERFACE*    usart1;
    TRX_DRIVER_INTERFACE*    i2c0;
} SYSTEM_INTERFACE_DRIVER;

// --------------------------------------------------------------------------------

/*!
 *
 */
typedef struct {
    SYSTEM_INTERFACE_TIME     time;
    SYSTEM_INTERFACE_EVENT    event;
    SYSTEM_INTERFACE_MEMORY    memory;
    SYSTEM_INTERFACE_IO    io;
    SYSTEM_INTERFACE_DRIVER driver;
} SYSTEM_INTERFACE;

/*!
 *
 */
extern const SYSTEM_INTERFACE i_system;

// --------------------------------------------------------------------------------

#endif // _H_system_interface_

// --------------------------------------------------------------------------------

