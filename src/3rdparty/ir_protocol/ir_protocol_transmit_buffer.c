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
 * @file   ir_protocol_transmit_buffer.c
 * @author Sebastian Lesse
 * @date   2022 / 01 / 04
 * @brief  Implementation of a common buffer that holds the pulse/pause intervals of a ir-command signal
 * 
 *          Concept:
 * 
 *          The Pulse-Pause Intervals of a ir-command is stored into this buffer
 *          On sending the ir-signal always the next interval value (pulse/pause) is read.
 *          This process is continued until no more interval values are available
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

#include "3rdparty/ir_protocol/ir_protocol_interface.h"

// --------------------------------------------------------------------------------

/**
 * @brief common transmit buffer.
 * Is used an all available ir-protocol implementations.
 * 
 */
static u8 transmit_buffer[IR_PROTOCOL_INTERFACE_TRANSMIT_BUFFER_MAX_LENGTH];

/**
 * @brief Index where the next transmit-interval is set
 * 
 */
static u16 write_pointer = 0;

/**
 * @brief Index from which the next interval is read.
 * 
 */
static u16 read_pointer = 0;

/**
 * @brief if 1 the buffer is requested right now, otherwise 0
 * is set via ir_protocol_interface_transmit_buffer_request() and
 * ir_protocol_interface_transmit_buffer_release()
 * 
 */
static u8 is_requested = 0;

// --------------------------------------------------------------------------------

/**
 * @see 3rdparty/ir_protocol/ir_protocol_interface.h#ir_protocol_interface_transmit_buffer_request
 */
u16 ir_protocol_interface_transmit_buffer_request(void) {
    if (is_requested) {
        return 0;
    }

    DEBUG_PASS("ir_protocol_interface_transmit_buffer_request()");

    is_requested = 1;
    return 1;
}

/**
 * @see 3rdparty/ir_protocol/ir_protocol_interface.h#ir_protocol_interface_transmit_buffer_release
 */
void ir_protocol_interface_transmit_buffer_release(void) {
    DEBUG_PASS("ir_protocol_interface_transmit_buffer_release()");
    is_requested = 0;
}

/**
 * @see 3rdparty/ir_protocol/ir_protocol_interface.h#ir_protocol_interface_transmit_buffer_busy
 */
u8 ir_protocol_interface_transmit_buffer_busy(void) {
    return is_requested;
}

/**
 * @see 3rdparty/ir_protocol/ir_protocol_interface.h#ir_protocol_interface_transmit_buffer_reset
 */
void ir_protocol_interface_transmit_buffer_reset(void) {
    DEBUG_PASS("ir_protocol_interface_transmit_buffer_reset()");
    write_pointer = 0;
    read_pointer = 0;
}

/**
 * @see 3rdparty/ir_protocol/ir_protocol_interface.h#ir_protocol_interface_transmit_buffer_restart
 */
void ir_protocol_interface_transmit_buffer_restart(u16 start_position){

    if (start_position > IR_PROTOCOL_INTERFACE_TRANSMIT_BUFFER_MAX_LENGTH) {
        DEBUG_PASS("ir_protocol_interface_transmit_buffer_restart() - ERR - OVERFLOW");

    } else if (start_position > write_pointer) {
        DEBUG_PASS("ir_protocol_interface_transmit_buffer_restart() - ERR - BEYOND");

    } else {
        DEBUG_TRACE_word(start_position, "ir_protocol_interface_transmit_buffer_restart() - START-POS:");
        read_pointer = start_position;
    }
}

/**
 * @see 3rdparty/ir_protocol/ir_protocol_interface.h#ir_protocol_interface_transmit_buffer_end
 */
u8 ir_protocol_interface_transmit_buffer_end(void) {
    if (read_pointer == write_pointer) {
        DEBUG_PASS("ir_protocol_interface_transmit_buffer_end()");
        return 1;
    }

    return 0;
}

/**
 * @see 3rdparty/ir_protocol/ir_protocol_interface.h#ir_protocol_interface_transmit_buffer_append_pulse
 */
void ir_protocol_interface_transmit_buffer_append_pulse(void) {

    if (write_pointer < IR_PROTOCOL_INTERFACE_TRANSMIT_BUFFER_MAX_LENGTH) {

        //DEBUG_PASS("ir_protocol_interface_transmit_buffer_append_pulse()");
        transmit_buffer[write_pointer] = IR_PROTOCOL_INTERFACE_TRANSMIT_INTERVAL_PULSE;
        write_pointer += 1;

    } else {
        DEBUG_PASS("ir_protocol_interface_transmit_buffer_append_pulse() - OVERFLOW !!! ---");
    }
}

/**
 * @see 3rdparty/ir_protocol/ir_protocol_interface.h#ir_protocol_interface_transmit_buffer_append_pause
 */
void ir_protocol_interface_transmit_buffer_append_pause(void) {

    if (write_pointer < IR_PROTOCOL_INTERFACE_TRANSMIT_BUFFER_MAX_LENGTH) {

        //DEBUG_PASS("ir_protocol_interface_transmit_buffer_append_pause()");
        transmit_buffer[write_pointer] = IR_PROTOCOL_INTERFACE_TRANSMIT_INTERVAL_PAUSE;
        write_pointer += 1;

    } else {
        DEBUG_PASS("ir_protocol_interface_transmit_buffer_append_pause() - OVERFLOW !!! ---");
    }
}

/**
 * @see 3rdparty/ir_protocol/ir_protocol_interface.h#ir_protocol_interface_transmit_buffer_get_next
 */
u8 ir_protocol_interface_transmit_buffer_get_next(void) {

    if (read_pointer < write_pointer) {

        u8 next = transmit_buffer[read_pointer];
        read_pointer += 1;

        //DEBUG_TRACE_byte(next, "ir_protocol_interface_transmit_buffer_get_next() - ");
        return next;
    }

    return IR_PROTOCOL_INTERFACE_TRANSMIT_INTERVAL_PAUSE;
}

/**
 * @see 3rdparty/ir_protocol/ir_protocol_interface.h#ir_protocol_interface_transmit_buffer_act_length
 */
u16 ir_protocol_interface_transmit_buffer_act_length(void) {
    return write_pointer;
}

/**
 * @see 3rdparty/ir_protocol/ir_protocol_interface.h#ir_protocol_interface_transmit_buffer_max_length
 */
u16 ir_protocol_interface_transmit_buffer_max_length(void) {
    return IR_PROTOCOL_INTERFACE_TRANSMIT_BUFFER_MAX_LENGTH;
}

// --------------------------------------------------------------------------------
