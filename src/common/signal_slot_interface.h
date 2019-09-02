#ifndef _SIGNAL_SLOT_INTERFACE_H_
#define _SIGNAL_SLOT_INTERFACE_H_

#include "config.h"

typedef void (*SIGNAL_SLOT_INTERFACE_CALLBACK)	(void);

typedef struct SIGNAL_SLOT_CONTEXT {
	SIGNAL_SLOT_INTERFACE_CALLBACK p_event_callback;
	struct SIGNAL_SLOT_CONTEXT* p_next;
} SIGNAL_SLOT_CONTEXT_TYPE;

typedef stuct SIGNAL_SLOT_INTERFACE_SIGNAL_CONTEXT {
	u16 send_timeout_ms;
	SIGNAL_SLOT_CONTEXT_TYPE* p_first_element;
} SIGNAL_SLOT_INTERFACE_SIGNAL_CONTEXT_TYPE;

#define SIGNAL_CREATE(signal_name)								\
												\
	static SIGNAL_SLOT_CONTEXT_TYPE _##signal_name##_context = {				\
		.p_event_callback = 0,								\
		.p_next = 0									\
	};											\
												\
	static void signal_name##_init(void) {							\
		signal_slot_init(&_##signal_name##_context);					\
	}											\
												\
	static void signal_name##_send(void) {							\
		signal_slot_send(&_##signal_name##_context);					\
	}											\
												\
	void signal_name##_connect(SIGNAL_SLOT_CONTEXT_TYPE* p_callback) {			\
		signal_slot_connect(&_##signal_name##_context, p_callback);			\
	}

#define SLOT_CREATE(signal_name, slot_name, p_callback_func)					\
												\
	void signal_name##_connect(SIGNAL_SLOT_CONTEXT_TYPE* p_callback);			\
												\
	static SIGNAL_SLOT_CONTEXT_TYPE _##signal_name##_##slot_name##_context = {		\
		.p_event_callback = 0,								\
		.p_next = 0									\
	};											\
												\
	void slot_name##_connect(void) {							\
		_##signal_name##_##slot_name##_context.p_event_callback = &p_callback_func;	\
		_##signal_name##_##slot_name##_context.p_next = 0;				\
		signal_name##_connect(&_##signal_name##_##slot_name##_context);			\
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
