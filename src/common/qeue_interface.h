/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	qeue_interface.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */
 
#ifndef _QEUE_INTERFACE_H_
#define _QEUE_INTERFACE_H_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "common/local_mutex.h"

// --------------------------------------------------------------------------------

/*!
 *
 */
typedef struct {
	u8* p_memory;
	volatile u8 write_counter;
	volatile u8 read_counter;
	volatile u8 element_counter;
	volatile u8 mutex_id;
	volatile MUTEX_TYPE mutex;
	const u8 object_size;
	const u8 max_size;
} QEUE_CONTEXT_TYPE;

// --------------------------------------------------------------------------------

#define QEUE_INTERFACE_BUILD_QEUE(name, object_type, obj_size, max_num_of_elements)				\
														\
	static u8 __##name##_memory[max_num_of_elements * obj_size];						\
														\
	static QEUE_CONTEXT_TYPE __##name##_qeue_context = {							\
		.p_memory = &__##name##_memory[0],								\
		.write_counter = 0,										\
		.read_counter = 0,										\
		.element_counter = 0,										\
		.mutex_id = MUTEX_INVALID_ID,									\
		.object_size = obj_size,									\
		.max_size = max_num_of_elements									\
	};													\
														\
	void name##_init(void) {										\
		qeue_interface_init(&__##name##_qeue_context);							\
	}													\
														\
	u8 name##_enqeue(const object_type* p_object_from) {							\
		return qeue_interface_enqeue(&__##name##_qeue_context, (const void*) p_object_from);		\
	}													\
														\
	u8 name##_deqeue(object_type* p_object_to) {								\
		return qeue_interface_deqeue(&__##name##_qeue_context, (void*) p_object_to);			\
	}													\
														\
	u8 name##_mutex_get(void) {										\
		return qeue_interface_mutex_get(&__##name##_qeue_context);					\
	}													\
														\
	void name##_mutex_release(void) {									\
		qeue_interface_mutex_release(&__##name##_qeue_context);						\
	}													\
														\
	u8 name##_is_empty(void) {										\
		return qeue_interface_is_empty(&__##name##_qeue_context);					\
	}													\
														\
	u8 name##_is_full(void) {										\
		return qeue_interface_is_full(&__##name##_qeue_context);					\
	}


#define QEUE_INTERFACE_INCLUDE_QEUE(name)									\
	void name##_init(void);											\
	u8 name##_enqeue(void* p_object);									\
	u8 name##_deqeue(void* p_object);									\
	u8 name##_mutex_get(void);										\
	void name##_mutex_release(void);									\
	u8 name##_is_empty(void);										\
	u8 name##_is_full(void);

// --------------------------------------------------------------------------------

/*
 *
 */
void qeue_interface_init(QEUE_CONTEXT_TYPE* p_qeue_context);

/*
 *
 */
u8 qeue_interface_enqeue(QEUE_CONTEXT_TYPE* p_qeue_context, const void* p_object_from);

/*
 *
 */
u8 qeue_interface_deqeue(QEUE_CONTEXT_TYPE* p_qeue_context, void* p_object_to);

/*
 *
 */
u8 qeue_interface_mutex_get(QEUE_CONTEXT_TYPE* p_qeue_context);

/*
 *
 */
void qeue_interface_mutex_release(QEUE_CONTEXT_TYPE* p_qeue_context);

/*
 *
 */
u8 qeue_interface_is_empty(QEUE_CONTEXT_TYPE* p_qeue_context);

/*
 *
 */
u8 qeue_interface_is_full(QEUE_CONTEXT_TYPE* p_qeue_context);

// --------------------------------------------------------------------------------

#endif // _QEUE_INTERFACE_H_
