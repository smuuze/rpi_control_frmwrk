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
 * @file    irq_interface.h
 * @author  Sebastian Lesse
 * @date    2022 / 10 / 26
 * @brief   Common interface tp the irq functionality of the current mcu platform.
 *          This interface enables to add one or more callbacks to a specific irq
 *          If a irq raises all callbacks of this irq are executed in the order they
 *          have been added. Every callback can be enabled/disabled individually.
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_irq_interface_
#define _H_irq_interface_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "cpu.h"

// --------------------------------------------------------------------------------

#define IRQ_HANDLER_INVALID_ID              255

#define IRQ_ENABLED                         1
#define IRQ_DISABLED                        0

// --------------------------------------------------------------------------------

/**
 * @brief Creates a new irq-handler usable for irq_add_handler
 * Usage:
 * 
 *      1. implement handler-callback:
 * 
 *          void my_irq_handler_callback(void) {
 *              // do something
 *          }
 * 
 *      2. create handler:
 * 
 *          IRQ_BUILD_HANDLER(MY_IRQ_HANDLER, my_irq_handler_callback, IRQ_DISABLED)
 * 
 *      3. append handler to irq:
 * 
 *          MY_IRQ_HANDLER_init();
 * 
 *      4. enabled irq-handler
 * 
 *          MY_IRQ_HANDLER_set_enabled(IRQ_ENABLED);
 * 
 * 
 */
#define IRQ_BUILD_HANDLER(name, callback, irq_num, en_default)  \
                                                                \
    static IRQ_HANDLER __IRQ_HANDLER_##name = {                 \
        .id = IRQ_HANDLER_INVALID_ID,                           \
        .is_enabled = en_default,                               \
        .p_callback = callback,                                 \
        ._p_next_handler = 0                                    \
    };                                                          \
                                                                \
    static i8 name##_init(void) {                               \
        return irq_add_handler(                                 \
            irq_num,                                            \
            &__IRQ_HANDLER_##name                               \
        );                                                      \
    }                                                           \
                                                                \
    static void name##_set_enabled(u8 en) {                     \
        __IRQ_HANDLER_##name.is_enabled = en;                   \
    }

// --------------------------------------------------------------------------------

/**
 * @brief Return values used by the irq-interface
 * 
 */
typedef enum {
    IRQ_INTERFACE_OK,
    IRQ_INTERFACE_OVERFLOW,
    IRQ_INTERFACE_INVALID,
    IRQ_INTERFACE_OCCUPIED,
    IRQ_INTERFACE_UNKNOWN
} IRQ_INTERFACE_RET_VAL;

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
typedef void (*IRQ_HANDLER_CALLBACK)    (void);

// --------------------------------------------------------------------------------

/**
 * @brief Container of a single irq-handler
 * including a callback that is used in case
 * the irq is raised.
 * 
 */
typedef struct IRQ_HANDLER_STRUCT {

    /**
     * @brief id of this handler in context of the owning irq
     * is set on calling irq_add_handler()
     * 
     */
    u8 id;

    /**
     * @brief 1 this handler will be called if the owning irq is raised.
     * Otherwise this handler will be ignored.
     * 
     */
    u8 is_enabled;

    /**
     * @brief function that is called
     * in case the irq is raised
     * 
     */
    IRQ_HANDLER_CALLBACK p_callback;

    /**
     * @brief next handler to be called
     * after the current handler has finished
     * 
     */
    struct IRQ_HANDLER_STRUCT* _p_next_handler;

} IRQ_HANDLER;

// --------------------------------------------------------------------------------

/**
 * @brief Initializes the irq-interface.
 * This function is called on system-startup.
 * Do not call this function while runtime.
 * 
 */
void irq_interface_init(void);

// --------------------------------------------------------------------------------

/**
 * @brief Adds another irq-handler to the given irq.
 * The handler is added to the current list of handlers of the IRQ.
 * 
 * @param irq_num number of the irq where to add the handler
 * @param p_handler handler to add to the specified irq
 * @return   IRQ_INTERFACE_OK -  the handler was added successfull
 *           IRQ_INTERFACE_INVALID - p_handleris null
 *           IRQ_INTERFACE_UNKNOWN -  the is no irq available for the given irq-number
 *           IRQ_INTERFACE_OVERFLOW - no more handler can be added to the irq
 *           IRQ_INTERFACE_OCCUPIED - the hander was already added to an irq
 */
IRQ_INTERFACE_RET_VAL irq_add_handler(u8 irq_num, IRQ_HANDLER* p_handler);

// --------------------------------------------------------------------------------

/**
 * @brief Enables or disables the IRQ specified by irq_num.
 * 
 * @param irq_num IRQ to dis- or enable
 * @param is_enabled 0: the irq will be disabled, otherwise the irq will be enabled
 * @return   IRQ_INTERFACE_OK -  irq has been enabled/disabled
 *           IRQ_INTERFACE_UNKNOWN -  the is no irq available for the given irq-number
 */
IRQ_INTERFACE_RET_VAL irq_set_enabled(u8 irq_num, u8 is_enabled);

// --------------------------------------------------------------------------------

#endif // _H_irq_interface_

// --------------------------------------------------------------------------------
