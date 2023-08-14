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
 * @file    thread_read_trace_object.c
 * @author  Sebastian Lesse
 * @date    2022 / 07 / 16
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

#include <stdio.h>

// --------------------------------------------------------------------------------

#include "common/signal_slot_interface.h"
#include "common/common_types.h"
#include "common/common_tools_number.h"
#include "common/qeue_interface.h"
#include "common/local_module_status.h"

#include "mcu_task_management/mcu_task_controller.h"
#include "mcu_task_management/thread_interface.h"

#include "tracer/trace_object.h"

#include "driver/trx_driver_interface.h"
#include "driver/cfg_driver_interface.h"

#include "system/system_interface.h"

// ------------------------------------------------------------------------------

#ifndef TRACER_READ_RAW_OBJECT_TIMEOUT_MS
#define TRACER_READ_RAW_OBJECT_TIMEOUT_MS           100
#endif

// ------------------------------------------------------------------------------

QEUE_INTERFACE_INCLUDE_QEUE(RAW_TRACE_OBJECT_QEUE)

// --------------------------------------------------------------------------------

#define READ_TRACE_OBJECT_STATUS_TERMINATED         (1 << 3)

BUILD_MODULE_STATUS_U8(READ_TRACE_OBJECT_STATUS)

// --------------------------------------------------------------------------------

static TRX_DRIVER_INTERFACE* p_com_driver = NULL;

// --------------------------------------------------------------------------------

static u8 main_read_trace_object_raw(TRACE_OBJECT_RAW* p_raw_obj) {

    //usart_read_bytes(p_com_driver, TRACE_PARSER_NUM_BYTES_HEADER, p_raw_obj->data, DEFAULT_USART_TIMEOUT_MS);
    if (p_com_driver->bytes_available() == 0) {
        //THREAD_DEBUG_MSG("main_read_trace_object_raw() - usart_read_bytes(HEADER) has FAILED !!!\n");
        return 0;
    }

    p_raw_obj->length = 0;

    // ----- Read Trace-Header -----

    u8 header[] = {TRACE_PARSER_HEADER_BYTE_ARRAY};
    u8 byte_counter = 0;

    //p_raw_obj->length = length;

    while (byte_counter < TRACE_PARSER_NUM_BYTES_HEADER) {

        if (p_com_driver->bytes_available() == 0) {
            p_com_driver->wait_for_rx(1, TRACER_READ_RAW_OBJECT_TIMEOUT_MS);
        }

        if (p_com_driver->bytes_available() == 0) {
            return 0;
        }

        p_com_driver->get_N_bytes(1, p_raw_obj->data + byte_counter);

        if (p_raw_obj->data[byte_counter] != header[byte_counter]) {
            return 0;
        }

        byte_counter += 1;
    }
    
    //DEBUG_PASS("main_read_trace_object_raw() - HEADER found - reading data");
    p_raw_obj->length += TRACE_PARSER_NUM_BYTES_HEADER;

    // ----- Read Trace-Number-Of-Bytes -----

    if (p_com_driver->bytes_available() < TRACE_PARSER_NUM_BYTES_BYTE_COUNT) {

        p_com_driver->wait_for_rx(TRACE_PARSER_NUM_BYTES_BYTE_COUNT, TRACER_READ_RAW_OBJECT_TIMEOUT_MS);

        if (p_com_driver->bytes_available() < TRACE_PARSER_NUM_BYTES_BYTE_COUNT) {
            DEBUG_PASS("main_read_trace_object_raw() - Waiting for number of bytes has FAILED !!!");
            p_raw_obj->length = 0;
            return 0;
        }
    }

    p_com_driver->get_N_bytes(TRACE_PARSER_NUM_BYTES_BYTE_COUNT, p_raw_obj->data + TRACE_PARSER_NUM_BYTES_HEADER);

    p_raw_obj->length += TRACE_PARSER_NUM_BYTES_BYTE_COUNT;
    u16 length = common_tools_number_readU16_MSB(p_raw_obj->data + TRACE_PARSER_NUM_BYTES_HEADER);

    //DEBUG_TRACE_word(length, "main_read_trace_object_raw() - Number of bytes to read");

    // CHECK FOR BUFFER-OVERFLOW !!! ---

    if (length > TRACE_OBJECT_RAW_DATA_LENGTH - (TRACE_PARSER_NUM_BYTES_BYTE_COUNT + TRACE_PARSER_NUM_BYTES_HEADER)) {
        DEBUG_TRACE_word(length, "main_read_trace_object_raw() - Too much bytes");
        length = TRACE_OBJECT_RAW_DATA_LENGTH - (TRACE_PARSER_NUM_BYTES_BYTE_COUNT + TRACE_PARSER_NUM_BYTES_HEADER);
    }

    // ----- Read Trace-Data -----

    if (p_com_driver->bytes_available() < length) {

        p_com_driver->wait_for_rx(length, TRACER_READ_RAW_OBJECT_TIMEOUT_MS);

        if (p_com_driver->bytes_available() < length) {
            DEBUG_PASS("main_read_trace_object_raw() - Waiting for trace-data has FAILED !!!");
            p_raw_obj->length = 0;
            return 0;
        }
    }

    p_com_driver->get_N_bytes(length, p_raw_obj->data + TRACE_PARSER_NUM_BYTES_HEADER + TRACE_PARSER_NUM_BYTES_BYTE_COUNT);
    if (length == 0) {
        DEBUG_PASS("main_read_trace_object_raw() - usart_read_bytes(PAYLOAD) has FAILED !!!");
        p_raw_obj->length = 0;
        return 0;
    }

    p_raw_obj->length += length;
    return 1;
}

// --------------------------------------------------------------------------------

void thread_read_trace_object_init(void) {
    DEBUG_PASS("thread_read_trace_object_init()");
}

THREAD_INTERFACE_EXIT_STATUS thread_read_trace_object_run(void) {

    DEBUG_PASS("thread_read_trace_object_run() - Thread started");

    TRACE_OBJECT_RAW raw_obj;

    while (1) {

        usleep(900); // reduce cpu-load

        if (READ_TRACE_OBJECT_STATUS_is_set(READ_TRACE_OBJECT_STATUS_TERMINATED)) {
            DEBUG_PASS("thread_read_trace_object_run() - TERMINATE SIGNAL RECEIVED");
            break;
        }

        if (p_com_driver == NULL) {
            continue;
        }

        if (main_read_trace_object_raw(&raw_obj) == 0) {
            continue;
        }

        while (RAW_TRACE_OBJECT_QEUE_mutex_get() == 0) {
            DEBUG_PASS("thread_read_trace_object_run() - QEUE is busy");
            usleep(500); // reduce cpu-load
        }
        
        if (RAW_TRACE_OBJECT_QEUE_is_full()) {
            DEBUG_PASS("thread_read_trace_object_run() - QEUE is full");
        } else if (RAW_TRACE_OBJECT_QEUE_enqeue(&raw_obj)) {
            //THREAD_DEBUG_MSG("thread_read_trace_object_run() - Object enqeued <<<\n");
        } else {
            DEBUG_PASS("thread_read_trace_object_run() - Object enqeued has FAILED !!!");
        }

        RAW_TRACE_OBJECT_QEUE_mutex_release();
    }
    
    DEBUG_PASS("thread_read_trace_object_run() - THREAD FINISHED");
    return 0;
}

void thread_read_trace_object_terminate(void) {
    DEBUG_PASS("thread_read_trace_object_terminate()");
    READ_TRACE_OBJECT_STATUS_set(READ_TRACE_OBJECT_STATUS_TERMINATED);
}

void thread_read_trace_object_set_com_driver(TRX_DRIVER_INTERFACE* p_driver) {
    
    p_com_driver = p_driver; //(const TRX_DRIVER_INTERFACE*) i_system.driver.usart0;
    //p_com_driver->initialize();
    //p_com_driver->configure(&usart0_config);
}

// --------------------------------------------------------------------------------

THREAD_INTERFACE_BUILD_THREAD(
    READ_TRACE_OBJECT_THREAD,
    THREAD_PRIORITY_MIDDLE,
    thread_read_trace_object_init,
    thread_read_trace_object_run,
    thread_read_trace_object_terminate
)

// --------------------------------------------------------------------------------
