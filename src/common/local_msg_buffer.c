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
 * @file    local_msg_buffer.c
 * @author  Sebastian Lesse
 * @date    2022 / 09 / 24
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

#include "local_msg_buffer.h"

// --------------------------------------------------------------------------------

/**
 * @see common/local_msg_buffer.h#local_msg_buffer_init
 */
void local_msg_buffer_init(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr) {

    p_buffer_descr->status_flags = 0;
    local_msg_buffer_clear_all(p_buffer_descr);
}

// --------------------------------------------------------------------------------

/**
 * @see common/local_msg_buffer.h#local_msg_buffer_start_write
 */
u8 local_msg_buffer_start_write(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr) {

    if ((p_buffer_descr->status_flags & MSG_BUFFER_STATUS_FLAG_WRITE_ACTIVE) != 0) {

        p_buffer_descr->status_flags |= MSG_BUFFER_STATUS_FLAG_WRITE_ACTIVE;
        return 1;

    } else {

        return 0;
    }
}

// --------------------------------------------------------------------------------

/**
 * @see common/local_msg_buffer.h#local_msg_buffer_add_byte
 */
void local_msg_buffer_add_byte(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr, u8 byte) {

    if ((p_buffer_descr->status_flags & MSG_BUFFER_STATUS_FLAG_WRITE_ACTIVE) == 0) {
        //return;
    }

    if (p_buffer_descr->num_elements == p_buffer_descr->size) {
        //PASS(); // local_msg_buffer_add_byte() - Buffer Overflow !!!
        return;
    }

    p_buffer_descr->p_buffer[p_buffer_descr->write_pointer] = byte;
    p_buffer_descr->write_pointer += 1;
    p_buffer_descr->num_elements += 1;

    if (p_buffer_descr->write_pointer == p_buffer_descr->size) {
        //PASS(); // local_msg_buffer_add_byte() - Write-Ring Closed
        p_buffer_descr->write_pointer = 0;
    }
}

// --------------------------------------------------------------------------------

/**
 * @see common/local_msg_buffer.h#local_msg_buffer_add_word
 */
void local_msg_buffer_add_word(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr, u16 word) {

    local_msg_buffer_add_byte(p_buffer_descr, (u8)(word));
    local_msg_buffer_add_byte(p_buffer_descr, (u8)(word >> 8));
}

// --------------------------------------------------------------------------------

/**
 * @see common/local_msg_buffer.h#local_msg_buffer_add_long
 */
void local_msg_buffer_add_long(
    LOCAL_MSG_BUFFER_DESCR* p_buffer_descr, u32 long_integer
) {

    local_msg_buffer_add_byte(p_buffer_descr, (u8)(long_integer));
    local_msg_buffer_add_byte(p_buffer_descr, (u8)(long_integer >> 8));
    local_msg_buffer_add_byte(p_buffer_descr, (u8)(long_integer >> 16));
    local_msg_buffer_add_byte(p_buffer_descr, (u8)(long_integer >> 24));
}

// --------------------------------------------------------------------------------

/**
 * @see common/local_msg_buffer.h#local_msg_buffer_add_N_bytes
 */
u16 local_msg_buffer_add_N_bytes(
    LOCAL_MSG_BUFFER_DESCR* p_buffer_descr, u16 num_bytes, const u8* p_ext_buffer
) {

    u16 i = 0;
    for ( ; i < num_bytes; i++) {
        
        if (p_buffer_descr->num_elements == p_buffer_descr->size) {
            break;
        }

        local_msg_buffer_add_byte(p_buffer_descr, p_ext_buffer[i]);
    }

    return i;
}

// --------------------------------------------------------------------------------

/**
 * @see common/local_msg_buffer.h#local_msg_buffer_stop_write
 */
void local_msg_buffer_stop_write(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr) {
    p_buffer_descr->status_flags &= ~MSG_BUFFER_STATUS_FLAG_WRITE_ACTIVE;
}

// --------------------------------------------------------------------------------

/**
 * @see common/local_msg_buffer.h#local_msg_buffer_start_read
 */
u8 local_msg_buffer_start_read(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr) {
    if ((p_buffer_descr->status_flags & MSG_BUFFER_STATUS_FLAG_READ_ACTIVE) != 0) {
        p_buffer_descr->status_flags |= MSG_BUFFER_STATUS_FLAG_READ_ACTIVE;
        return 1;
    } else {
        return 0;
    }
}

// --------------------------------------------------------------------------------

/**
 * @see common/local_msg_buffer.h#local_msg_buffer_bytes_available
 */
u16 local_msg_buffer_bytes_available(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr) {

    //TRACE_word(p_buffer_descr->num_elements); // local_msg_buffer_bytes_available()
    return p_buffer_descr->num_elements;
}

// --------------------------------------------------------------------------------

/**
 * @see common/local_msg_buffer.h#local_msg_buffer_bytes_free
 */
u16 local_msg_buffer_bytes_free(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr) {
    return (p_buffer_descr->size - local_msg_buffer_bytes_available(p_buffer_descr));
}

// --------------------------------------------------------------------------------

/**
 * @see common/local_msg_buffer.h#local_msg_buffer_get_byte
 */
u8 local_msg_buffer_get_byte(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr) {

    if ((p_buffer_descr->status_flags & MSG_BUFFER_STATUS_FLAG_READ_ACTIVE) == 0) {
        //return LOCAL_MSG_BUFFER_INVALID_CHARACTER;
    }

    if (p_buffer_descr->num_elements == 0) {
        //PASS(); // local_msg_buffer_get_byte() - Buffer Underrun !!!
        return LOCAL_MSG_BUFFER_INVALID_CHARACTER;
    }

    u8 element = LOCAL_MSG_BUFFER_INVALID_CHARACTER ;

    element = p_buffer_descr->p_buffer[p_buffer_descr->read_pointer];
    p_buffer_descr->read_pointer += 1;
    p_buffer_descr->num_elements -= 1;

    if (p_buffer_descr->read_pointer == p_buffer_descr->size) {
        p_buffer_descr->read_pointer = 0;
        //PASS(); // local_msg_buffer_get_byte() - Buffer Read-Ring Closed !!!
    }

    return element;
}

// --------------------------------------------------------------------------------

/**
 * @see common/local_msg_buffer.h#local_msg_buffer_get_word
 */
u16 local_msg_buffer_get_word(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr) {

    u16 word  = (u16)local_msg_buffer_get_byte(p_buffer_descr);
        word |= ((u16)(local_msg_buffer_get_byte(p_buffer_descr)) << 8);

    return word;
}

// --------------------------------------------------------------------------------

/**
 * @see common/local_msg_buffer.h#local_msg_buffer_get_long
 */
u32 local_msg_buffer_get_long(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr) {

    u32 long_integer = (u32)local_msg_buffer_get_byte(p_buffer_descr);
    long_integer    |= ((u32)(local_msg_buffer_get_byte(p_buffer_descr)) << 8);
    long_integer    |= ((u32)(local_msg_buffer_get_byte(p_buffer_descr)) << 16);
    long_integer    |= ((u32)(local_msg_buffer_get_byte(p_buffer_descr)) << 24);

    return long_integer;
}

// --------------------------------------------------------------------------------

/**
 * @see common/local_msg_buffer.h#local_msg_buffer_get_N_bytes
 */
u16 local_msg_buffer_get_N_bytes(
    LOCAL_MSG_BUFFER_DESCR* p_buffer_descr, u16 num_bytes, u8* p_buffer_to
) {

    u16 i = 0;
    for ( ; i < num_bytes; i++) {

        if (p_buffer_descr->num_elements == 0) {
            break;
        }

        p_buffer_to[i] = local_msg_buffer_get_byte(p_buffer_descr);
    }

    return i;
}

// --------------------------------------------------------------------------------

/**
 * @see common/local_msg_buffer.h#local_msg_buffer_stop_read
 */
void local_msg_buffer_stop_read(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr) {
    p_buffer_descr->status_flags &= ~MSG_BUFFER_STATUS_FLAG_READ_ACTIVE;
}

// --------------------------------------------------------------------------------

/**
 * @see common/local_msg_buffer.h#local_msg_buffer_clear_all
 */
void local_msg_buffer_clear_all(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr) {

    p_buffer_descr->read_pointer = 0;
    p_buffer_descr->write_pointer = 0;
    p_buffer_descr->num_elements = 0;
}

// --------------------------------------------------------------------------------

/**
 * @see common/local_msg_buffer.h#local_msg_buffer_debug_print
 */
void local_msg_buffer_debug_print(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr) {
    (void) p_buffer_descr;

    DEBUG_TRACE_N(
        p_buffer_descr->num_elements,
        p_buffer_descr->p_buffer,
        "local_msg_buffer_debug_print()"
    );
}

// --------------------------------------------------------------------------------
