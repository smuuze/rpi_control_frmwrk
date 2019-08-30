
#define TRACER_ON

#include "config.h"
#include "tracer.h"

#include "common/signal_slot_interface.h"

/*!
 *
 */
void signal_slot_init(SIGNAL_SLOT_CONTEXT_TYPE* p_context) {

	DEBUG_PASS("signal_slot_init()");

	p_context->p_next = 0;
	p_context->p_event_callback = 0;
}

/*!
 *
 */
void signal_slot_send(SIGNAL_SLOT_CONTEXT_TYPE* p_context) {

	DEBUG_PASS("signal_slot_send()");

	SIGNAL_SLOT_CONTEXT_TYPE* p_act = p_context;	

	while (p_act != 0) {

		if (p_act->p_event_callback != 0) {
			p_act->p_event_callback();
		}

		p_act = p_act->p_next;
	}
}

/*!
 *
 */
void signal_slot_connect(SIGNAL_SLOT_CONTEXT_TYPE* p_context, SIGNAL_SLOT_CONTEXT_TYPE* p_callback) {

	DEBUG_PASS("signal_slot_connect()");

	SIGNAL_SLOT_CONTEXT_TYPE* p_act = p_context;	

	while (p_act != 0) {

		if (p_act->p_next == 0) {
			p_act->p_next = p_callback;
			break;
		}

		p_act = p_act->p_next;
	}
}
