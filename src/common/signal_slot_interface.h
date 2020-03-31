#ifndef _SIGNAL_SLOT_INTERFACE_H_
#define _SIGNAL_SLOT_INTERFACE_H_


#include "config.h"


#ifndef SIGNAL_SLOT_INTERFACE_SIGNAL_SEND_TIMEOUT_MS
#define SIGNAL_SLOT_INTERFACE_SIGNAL_SEND_TIMEOUT_MS			50
#endif

/*!
 *
 */
typedef void (*SIGNAL_SLOT_INTERFACE_CALLBACK)	(void* p_arg);

/*!
 *
 */
typedef struct SIGNAL_SLOT_INTERFACE_SLOT_CONTEXT {
	SIGNAL_SLOT_INTERFACE_CALLBACK p_event_callback;
	struct SIGNAL_SLOT_INTERFACE_SLOT_CONTEXT* p_next;
} SIGNAL_SLOT_INTERFACE_SLOT_CONTEXT_TYPE;

/*!
 *
 */
typedef struct SIGNAL_SLOT_INTERFACE_SIGNAL_CONTEXT {
	u16 send_timeout_ms;
	SIGNAL_SLOT_INTERFACE_SLOT_CONTEXT_TYPE* p_first_element;
} SIGNAL_SLOT_INTERFACE_SIGNAL_CONTEXT_TYPE;


#define SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(signal_name)						\
													\
	static SIGNAL_SLOT_INTERFACE_SIGNAL_CONTEXT_TYPE _##signal_name##_context = {			\
		.send_timeout_ms = 0,									\
		.p_first_element = 0									\
	};												\
													\
	static void signal_name##_init(void) {								\
		signal_slot_init(&_##signal_name##_context);						\
	}												\
													\
	static void signal_name##_send(void* p_arg) {							\
		signal_slot_send(&_##signal_name##_context, p_arg);					\
	}												\
													\
	void signal_name##_connect(SIGNAL_SLOT_INTERFACE_SLOT_CONTEXT_TYPE* p_slot_context) {		\
		signal_slot_connect(&_##signal_name##_context, p_slot_context);				\
	}

#define SIGNAL_SLOT_INTERFACE_CREATE_SLOT(signal_name, slot_name, callback_func)			\
													\
	void signal_name##_connect(SIGNAL_SLOT_INTERFACE_SLOT_CONTEXT_TYPE* p_slot_context);		\
													\
	static SIGNAL_SLOT_INTERFACE_SLOT_CONTEXT_TYPE _##slot_name##_context = {			\
		.p_event_callback = 0,									\
		.p_next = 0										\
	};												\
													\
	void slot_name##_connect(void) {								\
		_##slot_name##_context.p_event_callback = &callback_func;				\
		_##slot_name##_context.p_next = 0;							\
		signal_name##_connect(&_##slot_name##_context);						\
	}

	
/*!
 *
 */
void signal_slot_init(SIGNAL_SLOT_INTERFACE_SIGNAL_CONTEXT_TYPE* p_context);

/*!
 *
 */
void signal_slot_send(SIGNAL_SLOT_INTERFACE_SIGNAL_CONTEXT_TYPE* p_context, void* p_arg);

/*!
 *
 */
void signal_slot_connect(SIGNAL_SLOT_INTERFACE_SIGNAL_CONTEXT_TYPE* p_signal_context, SIGNAL_SLOT_INTERFACE_SLOT_CONTEXT_TYPE* p_slot_context);

#endif // _SIGNAL_SLOT_INTERFACE_H_
