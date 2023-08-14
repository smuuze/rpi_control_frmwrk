/**
 * @file signal_slot_interface.h
 * @author sebastian lesse (sebastian lesse)
 * @brief 
 * @version 1.0
 * @date 2021-09-24
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef _SIGNAL_SLOT_INTERFACE_H_
#define _SIGNAL_SLOT_INTERFACE_H_

// --------------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------------

#ifndef SIGNAL_SLOT_INTERFACE_SIGNAL_SEND_TIMEOUT_MS
#define SIGNAL_SLOT_INTERFACE_SIGNAL_SEND_TIMEOUT_MS            50
#endif

// --------------------------------------------------------------------------------------

/**
 * @brief Type definition for the function callback of a slot
 * 
 */
typedef void (*SIGNAL_SLOT_INTERFACE_CALLBACK)    (const void* p_arg);

/**
 * @brief Context to declare a new slot.
 * This context is automatically generated
 * by the SIGNAL_SLOT_INTERFACE_CREATE_SLOT
 * macro definition
 * 
 */
typedef struct SIGNAL_SLOT_INTERFACE_SLOT_CONTEXT {
    SIGNAL_SLOT_INTERFACE_CALLBACK p_event_callback;
    struct SIGNAL_SLOT_INTERFACE_SLOT_CONTEXT* p_next;
} SIGNAL_SLOT_INTERFACE_SLOT_CONTEXT_TYPE;

/**
 * @brief Context to declare a new signal.
 * This context is automatically generated
 * by the SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL
 * macro definition
 * 
 */
typedef struct SIGNAL_SLOT_INTERFACE_SIGNAL_CONTEXT {
    u16 time_reference_ms;
    u16 time_interval_ms;
    SIGNAL_SLOT_INTERFACE_SLOT_CONTEXT_TYPE* p_first_element;
} SIGNAL_SLOT_INTERFACE_SIGNAL_CONTEXT_TYPE;

// --------------------------------------------------------------------------------------

/**
 * @brief Creats a signal that can be used to
 * publish information to a list of slots
 * Slots can connect to this signal to consume
 * it. Befor a signal can be used it has to
 * be initialized
 * 
 * Usage:
 * 
 * - definition.
 * 
 *     SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(MY_SIGNAL);
 * 
 * - initialization:
 * 
 *     MY_SIGNAL_init();
 * 
 * - Send the signal to all connected slots:
 * 
 *     MY_SIGNAL_send(p_argument);
 * 
 */
#define SIGNAL_SLOT_INTERFACE_CREATE_SIGNAL(signal_name)                                    \
                                                                                            \
    static SIGNAL_SLOT_INTERFACE_SIGNAL_CONTEXT_TYPE _##signal_name##_context = {           \
        .time_reference_ms = 0,                                                             \
        .time_interval_ms = 0,                                                              \
        .p_first_element = 0                                                                \
    };                                                                                      \
                                                                                            \
    void signal_name##_init(void) {                                                         \
        signal_slot_init(&_##signal_name##_context);                                        \
    }                                                                                       \
                                                                                            \
    void signal_name##_send(const void* p_arg) {                                            \
        signal_slot_send(&_##signal_name##_context, p_arg);                                 \
    }                                                                                       \
                                                                                            \
    void signal_name##_set_timeout(u16 timeout_ms) {                                        \
        _##signal_name##_context.time_interval_ms = timeout_ms;                             \
    }                                                                                       \
                                                                                            \
    void signal_name##_connect(SIGNAL_SLOT_INTERFACE_SLOT_CONTEXT_TYPE* p_slot_context) {   \
        signal_slot_connect(&_##signal_name##_context, p_slot_context);                     \
    }

/**
 * @brief Includes a signal that was defined extern.
 * This enables the usage of this signal by an other module.
 * Only sending a signal is possible in this way. The signal
 * has to be initialized first, at the place where it is declared.
 * 
 */
#define SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(signal_name)                        \
    void signal_name##_send(const void* p_arg);                                

/**
 * @brief Creates a slot that will listen to a signal.
 * Before the slot can consume the signal it must be connected.
 * If the signal is send the given callback function will be called
 * with the argument of the signal. If the signal is send before
 * this slot is not connected, the signal cannot be received.
 * 
 * Usage:
 * 
 * - declaration:
 * 
 *     SIGNAL_SLOT_INTERFACE_CREATE_SLOT(MY_SIGNAL, MY_SLOT, my_slot_callback);
 * 
 * - connect to signal
 * 
 *     MY_SLOT_connect();
 * 
 */
#define SIGNAL_SLOT_INTERFACE_CREATE_SLOT(signal_name, slot_name, callback_func)            \
                                                                                            \
    void signal_name##_connect(SIGNAL_SLOT_INTERFACE_SLOT_CONTEXT_TYPE* p_slot_context);    \
                                                                                            \
    static SIGNAL_SLOT_INTERFACE_SLOT_CONTEXT_TYPE _##slot_name##_context;                  \
                                                                                            \
    void slot_name##_connect(void) {                                                        \
        _##slot_name##_context.p_event_callback = &callback_func;                           \
        signal_name##_connect(&_##slot_name##_context);                                     \
    }

// --------------------------------------------------------------------------------------


/**
 * @brief Initializes a signal-context.
 * This function has to be called before a signal can be used.
 * 
 * @param p_signal_context The context to intialize
 */
void signal_slot_init(SIGNAL_SLOT_INTERFACE_SIGNAL_CONTEXT_TYPE* p_context);

/**
 * @brief Sends a signal to all connected slots.
 * The signal is only send if the timeout of the
 * last time the signal was send is already over.
 * Every slot gets access to the given argument.
 * After the signal was send the timeout is started.
 * 
 * @param p_signal_context Context containing a list of all slots the signal is send to
 * @param p_arg pointer to a argument that is given to every slot.
 */
void signal_slot_send(SIGNAL_SLOT_INTERFACE_SIGNAL_CONTEXT_TYPE* p_context, const void* p_arg);

/**
 * @brief Connects an other slot to a signal.
 * The new slot is added to the actual list of slots of the given signal.
 * 
 * @param p_signal_context Signal where the new slot is connected to
 * @param p_slot_context slot that will be connected to the given signal
 */
void signal_slot_connect(
    SIGNAL_SLOT_INTERFACE_SIGNAL_CONTEXT_TYPE* p_signal_context,
    SIGNAL_SLOT_INTERFACE_SLOT_CONTEXT_TYPE* p_slot_context
);

// --------------------------------------------------------------------------------------

#endif // _SIGNAL_SLOT_INTERFACE_H_
