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
 * @file    qeue_interface.c
 * @author  Sebastian Lesse
 * @date    2021 / 09 / 25
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

#include "string.h"

// --------------------------------------------------------------------------------

#include "common/local_mutex.h"
#include "common/qeue_interface.h"

// --------------------------------------------------------------------------------

/**
 * @see queue_interface.h#qeue_interface_init
 */
void qeue_interface_init(QEUE_CONTEXT_TYPE* p_qeue_context) {
    p_qeue_context->write_counter = 0;
    p_qeue_context->read_counter = 0;
    p_qeue_context->element_counter = 0;
    p_qeue_context->mutex.id = MUTEX_INVALID_ID;
    p_qeue_context->mutex.is_requested = 0;
}

/**
 * @see queue_interface.h#qeue_interface_enqeue
 */
u8 qeue_interface_enqeue(QEUE_CONTEXT_TYPE* p_qeue_context, const void* p_object_from) {

    if (p_qeue_context->element_counter == p_qeue_context->max_size) {
        DEBUG_PASS("qeue_interface_enqeue() - QEUE IS FULL !!! ---");
        return 0;
    }

    u16 offset = p_qeue_context->write_counter * p_qeue_context->object_size;
    memcpy(
        p_qeue_context->p_memory + offset,
        (const u8*)p_object_from,
        p_qeue_context->object_size
    );

    p_qeue_context->write_counter += 1;
    p_qeue_context->element_counter += 1;

    if (p_qeue_context->write_counter == p_qeue_context->max_size) {
        p_qeue_context->write_counter = 0;
    }

    DEBUG_TRACE_byte(
        p_qeue_context->element_counter,
        "qeue_interface_enqeue() - Elements in qeue: "
    );

    DEBUG_TRACE_byte(
        p_qeue_context->write_counter,
        "qeue_interface_enqeue() - write-counter: "
    );

    DEBUG_TRACE_N(
        (p_qeue_context->max_size * p_qeue_context->object_size),
        p_qeue_context->p_memory,
        "qeue_interface_enqeue() - Qeue-Memory:"
    );

    return 1;
}

/**
 * @see queue_interface.h#qeue_interface_deqeue
 */
u8 qeue_interface_deqeue(QEUE_CONTEXT_TYPE* p_qeue_context, void* p_object_to) {

    if (p_qeue_context->element_counter == 0) {
        DEBUG_PASS("qeue_interface_deqeue() - QEUE IS EMPTY !!! ---");
        return 0;
    }

    u16 offset = p_qeue_context->read_counter * p_qeue_context->object_size;

    memcpy(
        (u8*)p_object_to,
        p_qeue_context->p_memory + offset,
        p_qeue_context->object_size
    );

    p_qeue_context->read_counter += 1;
    p_qeue_context->element_counter -= 1;

    if (p_qeue_context->read_counter == p_qeue_context->max_size) {
        p_qeue_context->read_counter = 0;
    }

    DEBUG_TRACE_byte(
        p_qeue_context->element_counter,
        "qeue_interface_deqeue() - Elements in qeue: "
    );

    DEBUG_TRACE_byte(
        p_qeue_context->read_counter,
        "qeue_interface_deqeue() - read-counter: "
    );

    return 1;
}

/**
 * @see queue_interface.h#qeue_interface_is_empty
 */
u8 qeue_interface_is_empty(QEUE_CONTEXT_TYPE* p_qeue_context) {
    return p_qeue_context->element_counter == 0 ? 1 : 0;
}

/**
 * @see queue_interface.h#qeue_interface_is_full
 */
u8 qeue_interface_is_full(QEUE_CONTEXT_TYPE* p_qeue_context) {
    return p_qeue_context->element_counter == p_qeue_context->max_size ? 1 : 0;
}

/**
 * @see queue_interface.h#qeue_interface_mutex_get
 */
u8 qeue_interface_mutex_get(QEUE_CONTEXT_TYPE* p_qeue_context) {

    p_qeue_context->mutex_id = mutex_request((MUTEX_TYPE*)&p_qeue_context->mutex);

    if (p_qeue_context->mutex_id != MUTEX_INVALID_ID) {
        return 1;
    } else {
        return 0;
    }
}

/**
 * @see queue_interface.h#qeue_interface_mutex_release
 */
void qeue_interface_mutex_release(QEUE_CONTEXT_TYPE* p_qeue_context) {
    mutex_release((MUTEX_TYPE*)&p_qeue_context->mutex, p_qeue_context->mutex_id);
}

// --------------------------------------------------------------------------------
