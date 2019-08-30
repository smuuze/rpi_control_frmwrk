#ifndef _SIGNAL_SLOT_INTERFACE_H_
#define _SIGNAL_SLOT_INTERFACE_H_

#include "config.h"

typedef void (*SIGNAL_SLOT_INTERFACE_CALLBACK)	(void);

typedef struct SIGNAL_SLOT_CONTEXT {
	SIGNAL_SLOT_INTERFACE_CALLBACK p_event_callback;
	struct SIGNAL_SLOT_CONTEXT* p_next;
} SIGNAL_SLOT_CONTEXT_TYPE;

#define SIGNAL_CREATE(name)								\
											\
	static SIGNAL_SLOT_CONTEXT_TYPE _##name##_context;				\
											\
	static void name##_init(void) {							\
		signal_slot_init(&_##name##_context);					\
	}										\
											\
	static void name##_send(void) {							\
		signal_slot_send(&_##name##_context);					\
	}										\
											\
	void name##_connect(SIGNAL_SLOT_CONTEXT_TYPE* p_callback) {			\
		signal_slot_connect(&_##name##_context, p_callback);			\
	}

#define SLOT_CREATE(signal_name, slot_name, p_callback)					\
											\
	static SIGNAL_SLOT_CONTEXT_TYPE _##slot_name##p_callback = {			\
		.p_event_callback = &p_callback,					\
		.p_next = 0								\
	};										\
											\
	void slot_name##_connect(void) {						\
		signal_name##_connect(&_##slot_name##p_callback);			\
	}

	

/*!
 *
 */
void signal_slot_init(SIGNAL_SLOT_CONTEXT_TYPE* p_context);

/*!
 *
 */
void signal_slot_send(SIGNAL_SLOT_CONTEXT_TYPE* p_context);

/*!
 *
 */
void signal_slot_connect(SIGNAL_SLOT_CONTEXT_TYPE* p_context, SIGNAL_SLOT_CONTEXT_TYPE* p_callback);

#endif // _SIGNAL_SLOT_INTERFACE_H_
