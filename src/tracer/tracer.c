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
 * @file    tracer.c
 * @author  Sebastian Lesse
 * @date    2023 / 04 / 16
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

#include "cpu.h"

// --------------------------------------------------------------------------------

#include <string.h>

// --------------------------------------------------------------------------------

#include "system_interface.h"

#include "trx_driver_interface.h"
#include "cfg_driver_interface.h"

/*-------------------------------------------------------------------------------------------------------------------------------------*/

#if defined TRACER_BAUDRATE_115200 && (TRACER_BAUDRATE_115200) == 1
#define TRACER_BAUDRATE                    BAUDRATE_115200
#elif defined TRACER_BAUDRATE_230400 && (TRACER_BAUDRATE_230400) == 1
#define TRACER_BAUDRATE                    BAUDRATE_230400
#else
#define TRACER_BAUDRATE                    BAUDRATE_9600
#endif

/*-------------------------------------------------------------------------------------------------------------------------------------*/

#if defined TRACER_ENABLED

#if defined TRACER_INTERFACE_USART0 && TRACER_INTERFACE_USART0 == 1

#define TRACER_APPLY_DRIVER(p_com_driver)        p_com_driver = i_system.driver.usart0
#define TRACER_APPLY_DRIVER_CFG(driver_cfg)        memset((u8*)&driver_cfg, 0x00, sizeof(driver_cfg));                \
                            driver_cfg.module.usart.baudrate = TRACER_BAUDRATE

#elif defined TRACER_INTERFACE_USART1 && TRACER_INTERFACE_USART1 == 1

#define TRACER_APPLY_DRIVER(p_com_driver)        p_com_driver = i_system.driver.usart1
#define TRACER_APPLY_DRIVER_CFG(driver_cfg)        memset((u8*)&driver_cfg, 0x00, sizeof(driver_cfg));                \
                            driver_cfg.module.usart.baudrate = TRACER_BAUDRATE

#endif

#endif

/*-------------------------------------------------------------------------------------------------------------------------------------*/

#define TRACER_TRANSFER_ID_PASS                0x01
#define TRACER_TRANSFER_ID_BYTE                0x02
#define TRACER_TRANSFER_ID_WORD                0x03
#define TRACER_TRANSFER_ID_LONG                0x04
#define TRACER_TRANSFER_ID_ARRAY            0x05

#define TRACER_NUM_BYTES_LINE_NUMER            2
#define TRACER_NUM_BYTES_TRACE_TYPE            1
#define TRACER_NUM_BYTES_FRAME_LENGTH            2

#ifndef TRACER_FOOTER_DATA_BYTE
#define TRACER_FOOTER_DATA_BYTE                ((u8)'\n')
#endif

#ifndef TRACER_FOTER_DATA_LENGTH
#define TRACER_FOTER_DATA_LENGTH            1
#endif

#ifndef TRACER_HEADER_PREFIX_DATA_BYTE
#define TRACER_HEADER_PREFIX_DATA_BYTE            0xFA
#endif

#ifndef TRACER_HEADER_PREFIX_DATA_LENGTH
#define TRACER_HEADER_PREFIX_DATA_LENGTH        3
#endif

#define TRACER_HEADER_DATA_LENGTH            (TRACER_HEADER_PREFIX_DATA_LENGTH + TRACER_NUM_BYTES_FRAME_LENGTH + TRACER_NUM_BYTES_TRACE_TYPE)

#define TRACER_INIT_HEADER()                memset(trace_header, TRACER_HEADER_PREFIX_DATA_BYTE, TRACER_HEADER_PREFIX_DATA_LENGTH)
#define TRACER_INIT_FOOTER()                memset(trace_footer, TRACER_FOOTER_DATA_BYTE, TRACER_FOTER_DATA_LENGTH)

#define TRACER_SET_BYTE_COUNT(byte_count)        trace_header[TRACER_HEADER_PREFIX_DATA_LENGTH] = (u8)(byte_count >> 8);                     \
                            trace_header[TRACER_HEADER_PREFIX_DATA_LENGTH + 1] = (u8)(byte_count)
#define TRACER_SET_TYPE(type)                trace_header[TRACER_HEADER_PREFIX_DATA_LENGTH + TRACER_NUM_BYTES_FRAME_LENGTH] = type

#define TRACER_GET_HEADER()                trace_header
#define TRACER_GET_FOOTER()                trace_footer

/*-------------------------------------------------------------------------------------------------------------------------------------*/

/*!
 *
 */
static TRX_DRIVER_INTERFACE* p_com_driver = 0;

/*!
 *
 */
static u8 is_enabled = 0;

/*!
 *
 */
__UNUSED__ static TRX_DRIVER_CONFIGURATION driver_cfg;

/*!
 *
 */
static u8 trace_header[TRACER_HEADER_DATA_LENGTH];

/*!
 *
 */
static u8 trace_footer[TRACER_FOTER_DATA_LENGTH + 1]; // +1 to avoid compiler warnings on zero data length

/*!
 *
 */
__UNUSED__ static u8 driver_mutex_id = 0xFF;

/*!
 *
 */
static u8 _is_initialized = 0;

/*-------------------------------------------------------------------------------------------------------------------------------------*/

/*!
 *
 * @param string
 * @return
 */
static u16 _get_string_length(const char* string) {
    char * pointer = (char*)string;

    u16 length = 0;
    while (*pointer != '\0') {
        length += 1;
        pointer += 1;
    }

    return length;
}

void tracer_init(void) {

    #if defined TRACER_ENABLED
    {
        TRACER_APPLY_DRIVER(p_com_driver);
        TRACER_APPLY_DRIVER_CFG(driver_cfg);

        driver_mutex_id = p_com_driver->mutex_req();
        p_com_driver->configure(&driver_cfg);

        TRACER_INIT_HEADER();
        TRACER_INIT_FOOTER();

        _is_initialized = 1;
        is_enabled = 1;
    }
    #endif
}

void tracer_pass(const char* str, const char* file_name, u16 line_id) {

    (void) str;

    if (_is_initialized == 0) {
        return;
    }

    if (is_enabled == 0) {
        return;
    }

    u16 file_name_length    = _get_string_length(file_name);
    u16 byte_count        = TRACER_NUM_BYTES_TRACE_TYPE + TRACER_NUM_BYTES_LINE_NUMER + file_name_length + TRACER_FOTER_DATA_LENGTH;

    TRACER_SET_BYTE_COUNT(byte_count);
    TRACER_SET_TYPE(TRACER_TRANSFER_ID_PASS);

    p_com_driver->set_N_bytes(TRACER_HEADER_DATA_LENGTH, TRACER_GET_HEADER());
    p_com_driver->set_N_bytes(TRACER_NUM_BYTES_LINE_NUMER, (u8*)&line_id);
    p_com_driver->set_N_bytes(file_name_length, (u8*)file_name);
    p_com_driver->set_N_bytes(TRACER_FOTER_DATA_LENGTH, TRACER_GET_FOOTER());
    p_com_driver->start_tx();
}

void tracer_trace_byte(const char* str, const char* file_name, u16 line_id, u8 byte) {

    (void) str;

    if (_is_initialized == 0) {
        return;
    }

    if (is_enabled == 0) {
        return;
    }

    u16 file_name_length    = _get_string_length(file_name);
    u16 byte_count          = TRACER_NUM_BYTES_TRACE_TYPE + 1 + TRACER_NUM_BYTES_LINE_NUMER + file_name_length + TRACER_FOTER_DATA_LENGTH;

    TRACER_SET_BYTE_COUNT(byte_count);
    TRACER_SET_TYPE(TRACER_TRANSFER_ID_BYTE);

    p_com_driver->set_N_bytes(TRACER_HEADER_DATA_LENGTH, TRACER_GET_HEADER());
    p_com_driver->set_N_bytes(1, &byte);
    p_com_driver->set_N_bytes(TRACER_NUM_BYTES_LINE_NUMER, (u8*)&line_id);
    p_com_driver->set_N_bytes(file_name_length, (u8*)file_name);
    p_com_driver->set_N_bytes(TRACER_FOTER_DATA_LENGTH, TRACER_GET_FOOTER());
    p_com_driver->start_tx();
}

void tracer_trace_word(const char* str, const char* file_name, u16 line_id, u16 word) {

    (void) str;

    if (_is_initialized == 0) {
        return;
    }

    u16 file_name_length    = _get_string_length(file_name);
    u16 byte_count          = TRACER_NUM_BYTES_TRACE_TYPE + 2 + TRACER_NUM_BYTES_LINE_NUMER + file_name_length + TRACER_FOTER_DATA_LENGTH;

    TRACER_SET_BYTE_COUNT(byte_count);
    TRACER_SET_TYPE(TRACER_TRANSFER_ID_WORD);

    p_com_driver->set_N_bytes(TRACER_HEADER_DATA_LENGTH, TRACER_GET_HEADER());
    p_com_driver->set_N_bytes(2, (u8*)&word);
    p_com_driver->set_N_bytes(TRACER_NUM_BYTES_LINE_NUMER, (u8*)&line_id);
    p_com_driver->set_N_bytes(file_name_length, (u8*)file_name);
    p_com_driver->set_N_bytes(TRACER_FOTER_DATA_LENGTH, TRACER_GET_FOOTER());
    p_com_driver->start_tx();
}

void tracer_trace_long(const char* str, const char* file_name, u16 line_id, u32 integer) {

    (void) str;

    if (_is_initialized == 0) {
        return;
    }

    if (is_enabled == 0) {
        return;
    }

    u16 file_name_length    = _get_string_length(file_name);
    u16 byte_count          = TRACER_NUM_BYTES_TRACE_TYPE + 4 + TRACER_NUM_BYTES_LINE_NUMER + file_name_length + TRACER_FOTER_DATA_LENGTH;

    TRACER_SET_BYTE_COUNT(byte_count);
    TRACER_SET_TYPE(TRACER_TRANSFER_ID_LONG);

    p_com_driver->set_N_bytes(TRACER_HEADER_DATA_LENGTH, TRACER_GET_HEADER());
    p_com_driver->set_N_bytes(4, (u8*)&integer);
    p_com_driver->set_N_bytes(TRACER_NUM_BYTES_LINE_NUMER, (u8*)&line_id);
    p_com_driver->set_N_bytes(file_name_length, (u8*)file_name);
    p_com_driver->set_N_bytes(TRACER_FOTER_DATA_LENGTH, TRACER_GET_FOOTER());
    p_com_driver->start_tx();
}

void tracer_trace_n(const char* str, const char* file_name, u16 line_id, u16 length, const u8* p_buffer) {

    (void) str;

    if (_is_initialized == 0) {
        return;
    }

    if (is_enabled == 0) {
        return;
    }

    u16 file_name_length    = _get_string_length(file_name);
    u16 byte_count          = TRACER_NUM_BYTES_TRACE_TYPE + 1 + length + TRACER_NUM_BYTES_LINE_NUMER + file_name_length + TRACER_FOTER_DATA_LENGTH;

    TRACER_SET_BYTE_COUNT(byte_count);
    TRACER_SET_TYPE(TRACER_TRANSFER_ID_ARRAY);

    u8 u8_length = (u8)length;
    if (length > 255) {
        u8_length = 255;
    }

    p_com_driver->set_N_bytes(TRACER_HEADER_DATA_LENGTH, TRACER_GET_HEADER());
    p_com_driver->set_N_bytes(1, &u8_length);
    p_com_driver->set_N_bytes(length, p_buffer);
    p_com_driver->set_N_bytes(TRACER_NUM_BYTES_LINE_NUMER, (u8*)&line_id);
    p_com_driver->set_N_bytes(file_name_length, (u8*)file_name);
    p_com_driver->set_N_bytes(TRACER_FOTER_DATA_LENGTH, TRACER_GET_FOOTER());
    p_com_driver->start_tx();
}

void tracer_trace_string(
    const char* str,
    const char* file_name,
    u16 line_id,
    const char* p_string
) {
    tracer_trace_n(str, file_name, line_id, _get_string_length(p_string), p_string);
}

void tracer_enable(u8 enable) {

    if (_is_initialized == 0) {
        return;
    }
    
    is_enabled = enable;
}
