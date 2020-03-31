
#define TRACER_OFF

#include "config.h"
#include "tracer.h"

#include "common/signal_slot_interface.h"
#include "time_management/time_management.h"

/*!
 *
 */
void signal_slot_init(SIGNAL_SLOT_INTERFACE_SIGNAL_CONTEXT_TYPE* p_signal_context) {

	DEBUG_PASS("signal_slot_init()");

	p_signal_context->p_first_element = 0;
	p_signal_context->send_timeout_ms = 1;
}

/*!
 *
 */
void signal_slot_send(SIGNAL_SLOT_INTERFACE_SIGNAL_CONTEXT_TYPE* p_signal_context, void* p_arg) {

	if (time_mgmnt_istimeup_u16(p_signal_context->send_timeout_ms, SIGNAL_SLOT_INTERFACE_SIGNAL_SEND_TIMEOUT_MS) == 0) {
		DEBUG_PASS("signal_slot_send() - Timeout stil active - A'int sending signal !!! ---");
		return;
	}

	DEBUG_PASS("signal_slot_send()");

	p_signal_context->send_timeout_ms = time_mgmnt_gettime_u16();
	SIGNAL_SLOT_INTERFACE_SLOT_CONTEXT_TYPE* p_act = p_signal_context->p_first_element;

	do {
		if (p_act == 0) {
			break;
		}

		if (p_act->p_event_callback != 0) {
			p_act->p_event_callback(p_arg);
		}

		p_act = p_act->p_next;

	} while (p_act != 0);
}

/*!
 *
 */
void signal_slot_connect(SIGNAL_SLOT_INTERFACE_SIGNAL_CONTEXT_TYPE* p_signal_context, SIGNAL_SLOT_INTERFACE_SLOT_CONTEXT_TYPE* p_slot_context) {

	DEBUG_PASS("signal_slot_connect()");

	if (p_signal_context->p_first_element == 0) {
		p_signal_context->p_first_element = p_slot_context;
		return;
	}

	SIGNAL_SLOT_INTERFACE_SLOT_CONTEXT_TYPE* p_act = p_signal_context->p_first_element;

	while (p_act->p_next != 0) {
		p_act = p_act->p_next;
	} 

	p_act->p_next = p_slot_context;
}
