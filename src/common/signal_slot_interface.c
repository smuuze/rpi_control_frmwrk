
#define TRACER_OFF

// --------------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------------

#include "cpu.h"

#include <stdio.h>

// --------------------------------------------------------------------------------------

#include "common/signal_slot_interface.h"
#include "time_management/time_management.h"

// --------------------------------------------------------------------------------------

#ifdef TRACER_ENABLED

#define SIGNAL_SLOT_COUNTER_RESET()			u8 slot_counter = 0
#define SIGNAL_SLOT_COUNTER_INCREMENT()			slot_counter += 1
#define SIGNAL_SLOT_COUNTER_GET()			slot_counter

#else

#define SIGNAL_SLOT_COUNTER_RESET()			do{}while(0)
#define SIGNAL_SLOT_COUNTER_INCREMENT()			do{}while(0)
#define SIGNAL_SLOT_COUNTER_GET()			0

#endif

// --------------------------------------------------------------------------------------

/*!
 *
 */
void signal_slot_init(SIGNAL_SLOT_INTERFACE_SIGNAL_CONTEXT_TYPE* p_signal_context) {

	DEBUG_PASS("signal_slot_init()");

	//p_signal_context->p_first_element = 0;
	//p_signal_context->send_timeout_ms = 1;
}

/*!
 *
 */
void signal_slot_send(SIGNAL_SLOT_INTERFACE_SIGNAL_CONTEXT_TYPE* p_signal_context, const void* p_arg) {

	if (p_signal_context->timeout_ms != 0 && time_mgmnt_istimeup_u16(p_signal_context->send_timeout_ms, p_signal_context->timeout_ms) == 0) {
		DEBUG_PASS("signal_slot_send() - Timeout stil active - A'int sending signal !!! ---");
		return;
	}

	DEBUG_PASS("signal_slot_send()");

	p_signal_context->send_timeout_ms = time_mgmnt_gettime_u16();
	SIGNAL_SLOT_INTERFACE_SLOT_CONTEXT_TYPE* p_act = p_signal_context->p_first_element;

	SIGNAL_SLOT_COUNTER_RESET();

	while (p_act != 0) {

		if (p_act->p_event_callback != 0) {
			DEBUG_PASS("signal_slot_send() - event_callback()");
			p_act->p_event_callback(p_arg);
			SIGNAL_SLOT_COUNTER_INCREMENT();
		}

		DEBUG_PASS("signal_slot_send() - next slot");
		p_act = p_act->p_next;
	}

	DEBUG_TRACE_byte(SIGNAL_SLOT_COUNTER_GET(), "signal_slot_send() - Number of slots received this signal: ");
}

/*!
 *
 */
void signal_slot_connect(SIGNAL_SLOT_INTERFACE_SIGNAL_CONTEXT_TYPE* p_signal_context, SIGNAL_SLOT_INTERFACE_SLOT_CONTEXT_TYPE* p_slot_context) {

	DEBUG_PASS("signal_slot_connect()");

	if (p_signal_context->p_first_element == 0) {
		DEBUG_PASS("signal_slot_connect() - First slot connected to this signal");
		p_signal_context->p_first_element = p_slot_context;
		return;
	}

	SIGNAL_SLOT_INTERFACE_SLOT_CONTEXT_TYPE* p_act = p_signal_context->p_first_element;

	SIGNAL_SLOT_COUNTER_RESET();

	while (p_act->p_next != 0) {
		p_act = p_act->p_next;
		SIGNAL_SLOT_COUNTER_INCREMENT();
	} 

	p_act->p_next = p_slot_context;
	p_act->p_next->p_next = 0;

	DEBUG_TRACE_byte(SIGNAL_SLOT_COUNTER_GET(), "signal_slot_connect() - Number of slots connected to this signal: ");
}
