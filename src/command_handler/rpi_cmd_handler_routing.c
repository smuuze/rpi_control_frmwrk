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
 * @file    rpi_cmd_handler_routing.c
 * @author  Sebastian Lesse
 * @date    2020 / 05 / 26
 * @brief   Command handler to route commands to a co-processing unit
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

#include "command_management/command_handler_interface.h"
#include "command_management/command_buffer_interface.h"
#include "command_management/answer_buffer_interface.h"
#include "command_management/protocol_interface.h"

// --------------------------------------------------------------------------------

#include "copro/copro_interface.h"

#include "common/local_context.h"
#include "common/signal_slot_interface.h"

#include "command_handler/rpi_command_handler.h"

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
typedef void (*COPRO_TABLE_SIGNAL_CALLBACK)        (const void* p_arg);

/**
 * @brief 
 * 
 */
typedef struct {
    const COPRO_INTERFACE_OBJECT* copro;
    COPRO_TABLE_SIGNAL_CALLBACK signal_send;
} COPRO_TABLE_TYPE;

// --------------------------------------------------------------------------------

#ifdef COPRO1_AVAILABLE

static void copro1_slot_RESPONSE_RECEIVED(const void* p_arg) {

    PROTOCOL_INTERFACE* p_protocol = (PROTOCOL_INTERFACE*) p_arg;

    if (p_protocol == 0) {
        DEBUG_PASS("copro1_slot_RESPONSE_RECEIVED() - p_protocol is 0 - FAILEURE !!! ---");
        return;
    }

    DEBUG_PASS("copro1_slot_RESPONSE_RECEIVED()");
    p_protocol->set_finished(CMD_NO_ERR);
}

COPRO_INTERFACE_INCLUDE(COPRO1)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(COPRO1_ROUTING_COMMAND_SIGNAL)
SIGNAL_SLOT_INTERFACE_CREATE_SLOT(
    COPRO1_ROUTING_RESPONSE_SIGNAL,
    COPRO1_ROUTING_RESPONSE_SLOT,
    copro1_slot_RESPONSE_RECEIVED
)

#endif

//-----------------------------------------------------------------------------

#ifdef COPRO2_AVAILABLE

COPRO_INTERFACE_INCLUDE(COPRO2)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(COPRO2_ROUTING_COMMAND_SIGNAL)

#endif

//-----------------------------------------------------------------------------

#ifdef COPRO3_AVAILABLE

COPRO_INTERFACE_INCLUDE(COPRO3)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(COPRO3_ROUTING_COMMAND_SIGNAL)

#endif

//-----------------------------------------------------------------------------

#ifdef COPRO4_AVAILABLE

COPRO_INTERFACE_INCLUDE(COPRO4)
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(COPRO4_ROUTING_COMMAND_SIGNAL)

#endif

COPRO_INTERFACE_INCLUDE(COPRO_BORADCAST)

//-----------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
static COPRO_TABLE_TYPE copro_table[] = {

    #ifdef COPRO1_AVAILABLE
    { &COPRO1, &COPRO1_ROUTING_COMMAND_SIGNAL_send },
    #endif

    #ifdef COPRO2_AVAILABLE
    { &COPRO2, &COPRO2_ROUTING_COMMAND_SIGNAL_send },
    #endif

    #ifdef COPRO3_AVAILABLE
    { &COPRO3, &COPRO3_ROUTING_COMMAND_SIGNAL_send },
    #endif

    #ifdef COPRO4_AVAILABLE
    { &COPRO4,  &COPRO4_ROUTING_COMMAND_SIGNAL_send }
    #endif
};

#define COPRO_TABLE_SIZE    (sizeof(copro_table) / sizeof(COPRO_TABLE_TYPE))

//-----------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
void rpi_cmd_handler_routing_init(void) {

    DEBUG_PASS("rpi_cmd_handler_routing_init()");

    #ifdef COPRO1_AVAILABLE
    {
        DEBUG_PASS("rpi_cmd_handler_routing_init() - COPRO1_ROUTING_RESPONSE_SLOT_connect()");
        COPRO1_ROUTING_RESPONSE_SLOT_connect();
    }
    #endif
}

//-----------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
u8 rpi_cmd_handler_routing(PROTOCOL_INTERFACE* p_protocol) {

    u8 address = p_protocol->cmd_buffer->get_byte();

    DEBUG_TRACE_byte(address, "rpi_cmd_handler_routing() - Address : ");
    DEBUG_TRACE_byte(COPRO_TABLE_SIZE, "rpi_cmd_handler_routing() - Number of Copro's : ");

    u8 i = 0;
    for ( ; i < COPRO_TABLE_SIZE; i++) {
        
        if (address == copro_table[i].copro->get_address()) {
            
            DEBUG_TRACE_byte((i + 1), "rpi_cmd_handler_routing() - COPRO : ");
            copro_table[i].signal_send((void*) p_protocol);

            return CMD_ERR_BUSY;
        }
    }

    p_protocol->set_finished(CMD_ERR_NO_ROUTE_TO_HOST);
    return CMD_ERR_NO_ROUTE_TO_HOST;
}

//-----------------------------------------------------------------------------
