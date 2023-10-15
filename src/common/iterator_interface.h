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
 * @file    iterator_interface.h
 * @author  Sebastian Lesse
 * @date    2023 / 10 / 02
 * @brief   Short description of this file
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_iterator_interface_
#define _H_iterator_interface_

// --------------------------------------------------------------------------------

/**
 * @brief Common interface to iterate through a list of objects.
 * The object type depends on the implementation of this interface.
 * 
 * Example:
 * 
 *  - Function to start a new iteration. Sets the given Iterator to the first element.
 *  
 *      u8 iter_start(ITERATOR_INTERFACE* p_iterator);
 * 
 *  - Function to get the next element, if available. If no more elements available false is returned.
 * 
 *      u8 iter_next(ITERATOR_INTERFACE* p_iterator);
 */
typedef struct ITERATOR_INTERFACE {
    void* __element;
    u8 is_first;
    u8 is_last;
    u8 is_valid;
} ITERATOR_INTERFACE;

// --------------------------------------------------------------------------------

/**
 * @brief Callback to start the iteration of an itrerator instance.
 * 
 * @param p_iterator reference to the iterator instance to use
 * @param p_data pointer to a valid memory region to store the iteration element.
 * 
 * @return 1 if there is an element avaialble, otherwise 0
 */
typedef u8 (*ITERATOR_INTERFACE_CALLBACK_START) (ITERATOR_INTERFACE* p_iterator, void* p_data);

/**
 * @brief Callback to get the next element of the current itrerator instance.
 * 
 * @param p_iterator reference to the iterator instance to use
 * @param p_data pointer to a valid memory region to store the iteration element.
 * 
 * @return 1 if there is an element avaialble, otherwise 0
 */
typedef u8 (*ITERATOR_INTERFACE_CALLBACK_NEXT) (ITERATOR_INTERFACE* p_iterator, void* p_data);

// --------------------------------------------------------------------------------

typedef struct ITERATOR_INTERFACE_CALLBACKS {
    ITERATOR_INTERFACE_CALLBACK_START start;
    ITERATOR_INTERFACE_CALLBACK_NEXT next;
} ITERATOR_INTERFACE_CALLBACKS;

// --------------------------------------------------------------------------------

/**
 * @brief Add this definition to your header file with
 * the name of your iterator interface instance.
 * 
 * Example:
 * 
 *      ITERATOR_INTERFACE_INCLUDE(MY_ITERATOR)
 */
#define ITERATOR_INTERFACE_INCLUDE(name)                                        \
    const ITERATOR_INTERFACE_CALLBACKS* name##_get(void);

// --------------------------------------------------------------------------------

/**
 * @brief Add this definition to your source-file. With the name that
 * was given in your header file. You also need to define the function
 * callbacks for start() and next().
 * 
 * Example:
 * 
 *      static void* my_object_iter_start(ITERATOR_INTERFACE* p_iterator, u8* p_data) {
 *          ...
 *      }
 * 
 *      static void* my_object_iter_next(ITERATOR_INTERFACE* p_iterator, u8* p_data) {
 *          ...
 *      }
 * 
 *      ITERATOR_INTERFACE_CREATE(
 *          MY_ITERATOR,
 *          my_object_iter_get,
 *          my_object_iter_next
 *      )
 * 
 */
#define ITERATOR_INTERFACE_CREATE(name, callback_start, callback_next)          \
                                                                                \
    static ITERATOR_INTERFACE_CALLBACKS __##name##_callback_interface = {       \
        &callback_start,                                                        \
        &callback_next                                                          \
    };                                                                          \
                                                                                \
    const ITERATOR_INTERFACE_CALLBACKS* name##_iter(void) {                     \
        return &__##name##_callback_interface;                                  \
    }

// --------------------------------------------------------------------------------

/**
 * @brief Convenience macro to iterate over a set of objects
 * To use this macro you need to know the following:
 * 
 *  - iterator-interface to access the list of objects,
 *    in general it is included via a header-file
 *  - the type of a single object,
 *    in general it is a pointer-type
 * 
 * Example:
 * 
 *      - Include the reference to the itertator-interface to use
 * 
 *          ITERATOR_INTERFACE_INCLUDE(MY_ITERATOR)
 * 
 *      - Apply the ITERATOR_FOR macro
 * 
 *          ITERATE_FOR(
 *              MY_OBJ_TYPE*,
 *              my_iteration_var,
 *              MY_ITERATOR_iter(),
 * 
 *              if (my_iteration_var->is_valid()) {
 *                  my_iteration_var->do_something()
 *              }
 *          )
 */
#define ITERATE_FOR(type, var_name, i_iterator, code_block)                     \
{                                                                               \
    ITERATOR_INTERFACE __##var_name##_iterator = {                              \
        0,                                                                      \
        0,                                                                      \
        0,                                                                      \
        0                                                                       \
    };                                                                          \
                                                                                \
    type var_name;                                                              \
    u8 ret_val = i_iterator->start(&__##var_name##_iterator, &var_name);        \
                                                                                \
    while (ret_val != 0) {                                                      \
        code_block                                                              \
        ret_val = i_iterator->next(&__##var_name##_iterator, &var_name);  \
    }                                                                           \
}

// --------------------------------------------------------------------------------

#endif // _H_iterator_interface_

// --------------------------------------------------------------------------------

