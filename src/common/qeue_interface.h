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
 * @file    qeue_interface.h
 * @author  Sebastian Lesse
 * @date    2021 / 09 / 25
 * @brief   Provides a interface to instanciate a queue with a generic element type
 */

// --------------------------------------------------------------------------------

#ifndef _H_qeue_interface_
#define _H_qeue_interface_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "common/local_mutex.h"

// --------------------------------------------------------------------------------

/**
 * @brief Context of a queue isntance.
 * DO not use this directly.
 * Use QEUE_INTERFACE_BUILD_QEUE instead
 */
typedef struct {

    /**
     * @brief Pointer to the memory to use for the queue elements
     * the max length (in number of bytes) is defnied by max_size * object_size.
     */
    u8* p_memory;

    /**
     * @brief Current write position within p_memory
     */
    volatile u8 write_counter;

    /**
     * @brief Current read position within p_memory
     */
    volatile u8 read_counter;

    /**
     * @brief Number of elemetns stored inside of the queu
     */
    volatile u8 element_counter;

    /**
     * @brief Id of the mutex if this queue is locked
     */
    volatile u8 mutex_id;

    /**
     * @brief Mutex used to lock the queue.
     */
    volatile MUTEX_TYPE mutex;

    /**
     * @brief Number of bytes used for a single queue element.
     */
    const u16 object_size;

    /**
     * @brief Maximum number of elements that can be stored into the queue.
     */
    const u8 max_size;

} QEUE_CONTEXT_TYPE;

// --------------------------------------------------------------------------------

/**
 * @brief Macro to create a queue instance.
 * 
 * Usage:   1. Declare instance (outside of a function)
 * 
 *              // This queue can store up to 128 characters
 *              QEUE_INTERFACE_BUILD_QEUE(MY_QUEUE, char, sizeof(char), 128)
 * 
 *          2. Initialize the queue
 * 
 *              MY_QUEUE_init();
 * 
 *          3. Lock queue before usage
 * 
 *              if (MY_Queue_mutex_get() != 0) {
 *                  // queue can now be used
 *              }
 * 
 *          4. enqueue a new element
 * 
 *              if (MY_QUEUE_is_full() == 0) {
 *                  char character = 'a';
 *                  MY_QUEUE_enqeue(&character);
 *              }
 * 
 *          5. Release the queue
 * 
 *              MY_QUEUE_mutex_release();
 * 
 *          6. Lock mutex and read next element
 * 
 *              if (MY_Queue_mutex_get() != 0) {
 *                  if (MY_QUEUE_is_empty() == 0) {
 *                      char character = '\0';
 *                      MY_QUEUE_deqeue(&character);
 *                  }
 *                  MY_QUEUE_mutex_release();
 *              }
 */
#define QEUE_INTERFACE_BUILD_QEUE(name, object_type, obj_size, max_num_of_elements) \
                                                                                    \
    static u8 __##name##_memory[max_num_of_elements * obj_size];                    \
                                                                                    \
    static QEUE_CONTEXT_TYPE __##name##_qeue_context = {                            \
        .p_memory = &__##name##_memory[0],                                          \
        .write_counter = 0,                                                         \
        .read_counter = 0,                                                          \
        .element_counter = 0,                                                       \
        .mutex_id = MUTEX_INVALID_ID,                                               \
        .object_size = obj_size,                                                    \
        .max_size = max_num_of_elements                                             \
    };                                                                              \
                                                                                    \
    void name##_init(void) {                                                        \
        qeue_interface_init(&__##name##_qeue_context);                              \
    }                                                                               \
                                                                                    \
    u8 name##_enqeue(const object_type* p_object_from) {                            \
        return qeue_interface_enqeue(                                               \
            &__##name##_qeue_context,                                               \
            (const void*) p_object_from                                             \
        );                                                                          \
    }                                                                               \
                                                                                    \
    u8 name##_deqeue(object_type* p_object_to) {                                    \
        return qeue_interface_deqeue(                                               \
            &__##name##_qeue_context,                                               \
            (void*) p_object_to                                                     \
        );                                                                          \
    }                                                                               \
                                                                                    \
    u8 name##_mutex_get(void) {                                                     \
        return qeue_interface_mutex_get(&__##name##_qeue_context);                  \
    }                                                                               \
                                                                                    \
    void name##_mutex_release(void) {                                               \
        qeue_interface_mutex_release(&__##name##_qeue_context);                     \
    }                                                                               \
                                                                                    \
    u8 name##_is_empty(void) {                                                      \
        return qeue_interface_is_empty(&__##name##_qeue_context);                   \
    }                                                                               \
                                                                                    \
    u8 name##_is_full(void) {                                                       \
        return qeue_interface_is_full(&__##name##_qeue_context);                    \
    }

// --------------------------------------------------------------------------------

/**
 * @brief Include of a Queue instance. The declaration need to be made elsewhere
 * 
 */
#define QEUE_INTERFACE_INCLUDE_QEUE(name)                                           \
    void name##_init(void);                                                         \
    u8 name##_enqeue(void* p_object);                                               \
    u8 name##_deqeue(void* p_object);                                               \
    u8 name##_mutex_get(void);                                                      \
    void name##_mutex_release(void);                                                \
    u8 name##_is_empty(void);                                                       \
    u8 name##_is_full(void);

// --------------------------------------------------------------------------------

/**
 * @brief Initializes the given queue instance by reset the read and write pointer.
 * The element coutner is set to 0 and the mutex is to unlocked.
 * 
 * @param p_qeue_context reference to the queue instance
 */
void qeue_interface_init(QEUE_CONTEXT_TYPE* p_qeue_context);

/**
 * @brief Adds a element to the queue. The length of the object is taken from the
 * instance context.
 * 
 * @param p_qeue_context reference to the queue instance
 * @param p_object_from reference to the object that is added to the queue
 * @return 1 if the object was added to the queue, 0 otherwise
 */
u8 qeue_interface_enqeue(QEUE_CONTEXT_TYPE* p_qeue_context, const void* p_object_from);

/**
 * @brief Removes the next element in line from the queue, if available.
 * 
 * @param p_qeue_context reference to the queue instance
 * @param p_object_to reference to the memroy where to store the element
 * @return 1 if the element was dequeued, 0 if the queue is empty
 */
u8 qeue_interface_deqeue(QEUE_CONTEXT_TYPE* p_qeue_context, void* p_object_to);

/**
 * @brief Locks the queue for synchronized operations.
 * The queue shall only be modified if it was locked successful.
 * 
 * @param p_qeue_context reference to the queue instance
 * @return 1 if the queue was locked, otherwise 0
 */
u8 qeue_interface_mutex_get(QEUE_CONTEXT_TYPE* p_qeue_context);

/**
 * @brief Releases the lock of a mutex.
 * The mutex must only be released by the application that has locked it.
 * 
 * @param p_qeue_context reference to the queue instance
 */
void qeue_interface_mutex_release(QEUE_CONTEXT_TYPE* p_qeue_context);

/**
 * @brief Checks if the queue is empty.
 * 
 * @param p_qeue_context reference to the queue instance
 * @return 1 if the queue does not holds any elements, otherwise 0
 */
u8 qeue_interface_is_empty(QEUE_CONTEXT_TYPE* p_qeue_context);

/**
 * @brief Checks if the queue is full.
 * 
 * @param p_qeue_context reference to the queue instance
 * @return 1 if the queue holds its maximum number of elements, otherwise 0
 */
u8 qeue_interface_is_full(QEUE_CONTEXT_TYPE* p_qeue_context);

// --------------------------------------------------------------------------------

#endif // _H_qeue_interface_

// --------------------------------------------------------------------------------
