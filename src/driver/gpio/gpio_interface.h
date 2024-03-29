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
 * @file    gpio_interface.h
 * @author  Sebastian Lesse
 * @date    2022 / 02 / 27
 * @brief   Definition of the GPIO interface to be 
 *          implemented by a gpio-driver of a specific MCU
 * 
 */

//-----------------------------------------------------------------------------

#ifndef _H_gpio_interface_
#define _H_gpio_interface_

//-----------------------------------------------------------------------------

#include "cpu.h"

//-----------------------------------------------------------------------------

/**
 * @brief configuration values of a gpio pin
 * can be combined via '|'
 * Use these values in GPIO_DRIVER_PIN_DESCRIPTOR.pin_cfg
 * 
 * This flag is ignored on initialization time
 */

#define GPIO_INVERTED       0x80U

#define GPIO_DRIVER_PIN_IS_INVERTED(p_pin_descr)    ((p_pin_descr->pin_cfg & GPIO_INVERTED) != 0U ? 1U : 0U)

//-----------------------------------------------------------------------------

#define GPIO_IDLE_HIGH      0x00U
#define GPIO_IDLE_LOW       0x20U
#define GPIO_IDLE_HIGH_Z    0x40U
#define GPIO_IDLE_MASK      0x60U

#define GPIO_DRIVER_IS_IDLE_LOW(p_pin_descr)    ((p_pin_descr->pin_cfg & GPIO_IDLE_MASK) == GPIO_IDLE_LOW)
#define GPIO_DRIVER_IS_IDLE_HIGH(p_pin_descr)   ((p_pin_descr->pin_cfg & GPIO_IDLE_MASK) == GPIO_IDLE_HIGH)

//-----------------------------------------------------------------------------

/**
 * @brief Tell the GPIO driver to do not use this pin
 * This causes that the pin does not have any functionality at all
 */
#define GPIO_DEACTIVATE     0x02U

#define GPIO_DRIVER_PIN_IS_DEACTIVATED(p_descr)     ((p_descr->pin_cfg & GPIO_DEACTIVATE) != 0U)

//-----------------------------------------------------------------------------

/**
 * @brief Tell the GPIO driver to use this pin as output
 */
#define GPIO_OUTPUT         0x01U

/**
 * @brief Tell the GPIO driver to use this pin as input
 */
#define GPIO_INPUT          0x00U

#define GPIO_DRIVER_IS_OUTPUT(p_descr)      ((p_descr->pin_cfg & GPIO_OUTPUT) != 0U)
#define GPIO_DRIVER_SET_OUTPUT(p_descr)     p_descr->pin_cfg |= GPIO_OUTPUT
#define GPIO_DRIVER_SET_INPUT(p_descr)      p_descr->pin_cfg &= ~GPIO_OUTPUT

//-----------------------------------------------------------------------------

/**
 * @brief Function a GPIO can have
 * function depends on MCU and
 * gpio-driver implementation.
 * A gpio can only have one function at a time.
 * 
 * GPIO_FUNCTION_GPIO is optional
 * and is applied if no other function is defined
 */
#define GPIO_FUNCTION_GPIO              0x00U
#define GPIO_FUNCTION_1                 0x04U
#define GPIO_FUNCTION_2                 0x08U
#define GPIO_FUNCTION_3                 0x0CU
#define GPIO_FUNCTION_4                 0x10U
#define GPIO_FUNCTION_5                 0x14U
#define GPIO_FUNCTION_6                 0x18U
#define GPIO_FUNCTION_7                 0x1CU

#define GPIO_FUNCTION_MASK              0x1CU
#define GPIO_GET_FUNCTION(p_pin_descr)  (p_pin_descr->pin_cfg & GPIO_FUNCTION_MASK)

//-----------------------------------------------------------------------------

/**
 * @brief Pin-IDs
 * 
 */

#define GPIO_PIN_0      0x00U
#define GPIO_PIN_1      0x01U
#define GPIO_PIN_2      0x02U
#define GPIO_PIN_3      0x03U
#define GPIO_PIN_4      0x04U
#define GPIO_PIN_5      0x05U
#define GPIO_PIN_6      0x06U
#define GPIO_PIN_7      0x07U

//-----------------------------------------------------------------------------

/**
 * @brief Port-IDs
 */
#define GPIO_PORT_A     0x00U
#define GPIO_PORT_B     0x01U
#define GPIO_PORT_C     0x02U
#define GPIO_PORT_D     0x03U
#define GPIO_PORT_E     0x04U

//-----------------------------------------------------------------------------

/**
 * @brief Inverts the level. The inverted level is stored into the given variable
 */
#define GPIO_DRIVER_INVERT_LEVEL(level)     level = (level == GPIO_LEVEL_HIGH) ? \
                                                    GPIO_LEVEL_LOW : GPIO_LEVEL_HIGH

//-----------------------------------------------------------------------------

/**
 * @brief Structure to realize a gpio-pin context
 */
typedef struct {

    /**
     * @brief The port at which the pin is located
     * 
     */
    const u8 port_id;
    
    /**
     * @brief The pin id of the port of this pin
     * 
     */
    const u8 pin_id;

    /**
     * @brief The configuration of this pin
     * 
     * Bit      Name        Values
     * -------------------------------------
     * 0        DIRECTION   0 - INPUT
     *                      1 - OUTPUT
     * 
     * 1        ACTIVATE    0 - ACTIVATED
     *                      1 - DEACTIVATED
     * 
     * 2:4      FUNCTION    0 - NONE
     *                      1 - Function 1
     *                      2 - Function 2
     *                      3 - Function 3
     *                      4 - Function 4
     *                      5 - Function 5
     *                      6 - Function 6
     *                      7 - Function 7
     * 
     * 5:6      IDLE        0 - IDLE LOW
     *                      1 - IDLE HIGH
     *                      2 - IDLE HIGH-Z
     * 
     * 7        INVERT      0 - NOT INVERTED
     *                      1 - IS INVERTED
     * 
     */
    u8 pin_cfg;

} GPIO_DRIVER_PIN_DESCRIPTOR;

/**
 * @brief levels a gpio-pin can have
 */
typedef enum {

    /**
     * @brief Pin has a low level (e.g. GND)
     */
    GPIO_LEVEL_LOW = 0x00U,

    /**
     * @brief pin has a high level ( e.g. VCC)
     */
    GPIO_LEVEL_HIGH = 0x01U,

    /**
     * @brief pin is high-z
     */
    GPIO_LEVEL_HIGH_Z = 0x02U,

} GPIO_DRIVER_LEVEL;

/**
 * @brief Directions a gpio-pin can have
 */
typedef enum {

    /**
     * @brief pin is a input
     */
    GPIO_DIRECTION_INPUT = 0x00U,

    /**
     * @brief pin is a output
     */
    GPIO_DIRECTION_OUTPUT = 0x01U,

} GPIO_DRIVER_DIRECTION;

//-----------------------------------------------------------------------------

#define BUILD_GPIO(pin_name, port_id, pin_id, pin_cfg)                          \
                                                                                \
    GPIO_DRIVER_PIN_DESCRIPTOR pin_name = {                                     \
        port_id,                                                                \
        pin_id,                                                                 \
        (pin_cfg)                                                               \
    };                                                                          \
                                                                                \
    GPIO_DRIVER_PIN_DESCRIPTOR* const p_pin_##port_id##_##pin_id = &pin_name;   \
                                                                                \
    void pin_name##_drive_high(void) {                                          \
        /* OUTPUT_HIGH_LEVEL  */                                                \
        gpio_driver_set_direction(&pin_name, GPIO_DIRECTION_OUTPUT);            \
        gpio_driver_set_level(&pin_name, GPIO_LEVEL_HIGH);                      \
    }                                                                           \
                                                                                \
    void pin_name##_drive_low(void) {                                           \
        /* OUTPUT_LOW_LEVEL  */                                                 \
        gpio_driver_set_direction(&pin_name, GPIO_DIRECTION_OUTPUT);            \
        gpio_driver_set_level(&pin_name, GPIO_LEVEL_LOW);                       \
                                                                                \
    }                                                                           \
                                                                                \
    void pin_name##_no_drive(void) {                                            \
        /* INPUT_HIGH_Z  */                                                     \
        gpio_driver_set_direction(&pin_name, GPIO_DIRECTION_INPUT);             \
        gpio_driver_set_level(&pin_name, GPIO_LEVEL_HIGH_Z);                    \
                                                                                \
    }                                                                           \
                                                                                \
    void pin_name##_toggle_level(void) {                                        \
        gpio_driver_toggle_level(&pin_name);                                    \
    }                                                                           \
                                                                                \
    void pin_name##_pull_up(void) {                                             \
        gpio_driver_set_direction(&pin_name, GPIO_DIRECTION_INPUT);             \
        gpio_driver_set_level(&pin_name, GPIO_LEVEL_HIGH);                      \
                                                                                \
    }                                                                           \
                                                                                \
    void pin_name##_pull_down(void) {                                           \
        gpio_driver_set_direction(&pin_name, GPIO_DIRECTION_INPUT);             \
        gpio_driver_set_level(&pin_name, GPIO_LEVEL_LOW);                       \
                                                                                \
    }                                                                           \
                                                                                \
    void pin_name##_no_pull(void) {                                             \
        /* INPUT_HIGH_Z  */                                                     \
        gpio_driver_set_direction(&pin_name, GPIO_DIRECTION_INPUT);             \
        gpio_driver_set_level(&pin_name, GPIO_LEVEL_HIGH_Z);                    \
    }                                                                           \
                                                                                \
    u8 pin_name##_is_high_level(void) {                                         \
        return gpio_driver_get_level(&pin_name) == GPIO_LEVEL_HIGH ? 1 : 0;     \
    }                                                                           \
                                                                                \
    u8 pin_name##_is_low_level(void) {                                          \
        return gpio_driver_get_level(&pin_name) == GPIO_LEVEL_LOW ? 1 : 0;      \
    }                                                                           \
                                                                                \
    void pin_name##_print_state(void) {                                         \
        gpio_driver_print_pin_state(&pin_name);                                 \
    }                                                                           \
                                                                                \
    void pin_name##_activate(void) {                                            \
        gpio_driver_activate(&pin_name);                                        \
    }                                                                           \
                                                                                \
    void pin_name##_deactivate(void) {                                          \
        gpio_driver_deactivate(&pin_name);                                      \
    }

//-----------------------------------------------------------------------------

#define INCLUDE_GPIO(pin_name)                      \
    extern GPIO_DRIVER_PIN_DESCRIPTOR pin_name;     \
    void pin_name##_drive_high(void);               \
    void pin_name##_drive_low(void);                \
    void pin_name##_no_drive(void);                 \
    void pin_name##_toggle_level(void);             \
    void pin_name##_pull_up(void);                  \
    void pin_name##_pull_down(void);                \
    void pin_name##_no_pull(void);                  \
    u8 pin_name##_is_high_level(void);              \
    u8 pin_name##_is_low_level(void);               \
    void pin_name##_print_state(void);              \
    void pin_name##_activate(void);                 \
    void pin_name##_deactivate(void);

//-----------------------------------------------------------------------------

#define INCLUDE_GPIO_ALIAS(pin_name)                \
    void pin_name##_drive_high(void);               \
    void pin_name##_drive_low(void);                \
    void pin_name##_no_drive(void);                 \
    void pin_name##_toggle_level(void);             \
    void pin_name##_pull_up(void);                  \
    void pin_name##_pull_down(void);                \
    void pin_name##_no_pull(void);                  \
    u8 pin_name##_is_high_level(void);              \
    u8 pin_name##_is_low_level(void);               \
    void pin_name##_print_state(void);              \
    void pin_name##_activate(void);                 \
    void pin_name##_deactivate(void);

//-----------------------------------------------------------------------------

#define GPIO_ALIAS(new_name, exisitng_name)         \
                                                    \
    void exisitng_name##_drive_high(void);          \
    void exisitng_name##_drive_low(void);           \
    void exisitng_name##_no_drive(void);            \
    void exisitng_name##_toggle_level(void);        \
    void exisitng_name##_pull_up(void);             \
    void exisitng_name##_pull_down(void);           \
    void exisitng_name##_no_pull(void);             \
    u8 exisitng_name##_is_high_level(void);         \
    u8 exisitng_name##_is_low_level(void);          \
    void exisitng_name##_print_state(void);         \
    void exisitng_name##_activate(void);            \
    void exisitng_name##_deactivate(void);          \
                                                    \
    void new_name##_drive_high(void) {              \
        exisitng_name##_drive_high();               \
    }                                               \
                                                    \
    void new_name##_drive_low(void) {               \
        exisitng_name##_drive_low();                \
    }                                               \
                                                    \
    void new_name##_no_drive(void) {                \
        exisitng_name##_no_drive();                 \
    }                                               \
                                                    \
    void new_name##_toggle_level(void) {            \
        exisitng_name##_toggle_level();             \
    }                                               \
                                                    \
    void new_name##_pull_up(void) {                 \
        exisitng_name##_pull_up();                  \
    }                                               \
                                                    \
    void new_name##_pull_down(void) {               \
        exisitng_name##_pull_down();                \
    }                                               \
                                                    \
    void new_name##_no_pull(void) {                 \
        exisitng_name##_no_pull();                  \
    }                                               \
                                                    \
    u8 new_name##_is_high_level(void) {             \
        return exisitng_name##_is_high_level();     \
    }                                               \
                                                    \
    u8 new_name##_is_low_level(void) {              \
        return exisitng_name##_is_low_level();      \
    }                                               \
                                                    \
    void new_name##_print_state(void) {             \
        exisitng_name##_print_state();              \
    }                                               \
                                                    \
    void new_name##_activate(void) {                \
        exisitng_name##_activate();                 \
    }                                               \
                                                    \
    void new_name##_deactivate(void) {              \
        exisitng_name##_deactivate();               \
    }

//-----------------------------------------------------------------------------

#define FAKE_GPIO(pin_name, port_id, pin_id, pin_cfg)                               \
                                                                                    \
    static GPIO_DRIVER_DIRECTION _##pin_name##_direction = GPIO_DIRECTION_INPUT;    \
    static GPIO_DRIVER_LEVEL _##pin_name##_level = GPIO_LEVEL_HIGH_Z;               \
    GPIO_DRIVER_PIN_DESCRIPTOR pin_name = {                                         \
        port_id,                                                                    \
        pin_id,                                                                     \
        (pin_cfg)                                                                   \
    };                                                                              \
    GPIO_DRIVER_PIN_DESCRIPTOR* const p_pin_##port_id##_##pin_id = &pin_name;       \
                                                                                    \
    void pin_name##_drive_high(void) {                                              \
        _##pin_name##_direction = GPIO_DIRECTION_OUTPUT;                            \
        _##pin_name##_level = GPIO_LEVEL_HIGH;                                      \
    }                                                                               \
                                                                                    \
    void pin_name##_drive_low(void) {                                               \
        _##pin_name##_direction = GPIO_DIRECTION_OUTPUT;                            \
        _##pin_name##_level = GPIO_LEVEL_LOW;                                       \
    }                                                                               \
                                                                                    \
    void pin_name##_no_drive(void) {                                                \
        _##pin_name##_direction = GPIO_DIRECTION_INPUT;                             \
        _##pin_name##_level = GPIO_LEVEL_HIGH_Z;                                    \
    }                                                                               \
                                                                                    \
    void pin_name##_toggle_level(void) {                                            \
    }                                                                               \
                                                                                    \
    void pin_name##_pull_up(void) {                                                 \
        _##pin_name##_direction = GPIO_DIRECTION_INPUT;                             \
        _##pin_name##_level = GPIO_LEVEL_HIGH;                                      \
    }                                                                               \
                                                                                    \
    void pin_name##_pull_down(void) {                                               \
        _##pin_name##_direction = GPIO_DIRECTION_INPUT;                             \
        _##pin_name##_level = GPIO_LEVEL_LOW;                                       \
                                                                                    \
    }                                                                               \
                                                                                    \
    void pin_name##_no_pull(void) {                                                 \
        _##pin_name##_direction = GPIO_DIRECTION_INPUT;                             \
        _##pin_name##_level = GPIO_LEVEL_HIGH_Z;                                    \
    }                                                                               \
                                                                                    \
    u8 pin_name##_is_high_level(void) {                                             \
        return _##pin_name##_level == GPIO_LEVEL_HIGH ? 1U : 0U;                    \
    }                                                                               \
                                                                                    \
    u8 pin_name##_is_low_level(void) {                                              \
        return _##pin_name##_level == GPIO_LEVEL_LOW ? 1U : 0U;                     \
    }                                                                               \
                                                                                    \
    void pin_name##_print_state(void) {                                             \
    }                                                                               \
                                                                                    \
    void pin_name##_activate(void) {                                                \
    }                                                                               \
                                                                                    \
    void pin_name##_deactivate(void) {                                              \
    }

//-----------------------------------------------------------------------------

#define INCLUDE_GPIO_REFRENCE(port_id, pin_id) \
	extern GPIO_DRIVER_PIN_DESCRIPTOR* const p_pin_##port_id##_##pin_id;

#define GET_GPIO_REFERENCE(port_id, pin_id) \
	p_pin_##port_id##_##pin_id

//-----------------------------------------------------------------------------

#define HAS_GPIO(name)

//-----------------------------------------------------------------------------

/**
 * @brief Initializes the gpio-driver.
 * This fucntion must be called before any
 * gpio-pin operation is performed
 */
void gpio_driver_init(void);

/**
 * @brief Deinitializes the gpio-driver.
 * The driver then cannot be used anymore until 
 * gpio_driver_init() is called once again.
 */
void gpio_driver_deinit(void);

/**
 * @brief Initializes the pin defined by p_pin_descr.
 * If the pin is deactivated this function does nothing.
 * 
 * @param p_pin_descr dcescriptor of the pin to configure, must not be NULL
 * @see GPIO_DRIVER_PIN_DESCRIPTOR
 */
void gpio_driver_init_pin(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);

/**
 * @brief Changes the direction of the gpio-pin defined by p_pin_descr.
 * 
 * @param p_pin_descr descriptor of the pin where to change the direction
 * must not be NULL.
 * @param direction new direction to set on the gpio-pin
 * @see GPIO_DRIVER_PIN_DESCRIPTOR
 * @see GPIO_DRIVER_DIRECTION
 */
void gpio_driver_set_direction(
    GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr,
    GPIO_DRIVER_DIRECTION direction
);

/**
 * @brief Changes the current level of the gpio-pin defined by p_pin_descr.
 * 
 * @param p_pin_descr descriptor of the pin where to change the level
 * must not be NULL.
 * @param level new level to set on the pin
 * @see GPIO_DRIVER_PIN_DESCRIPTOR
 * @see GPIO_DRIVER_LEVEL
 */
void gpio_driver_set_level(
    GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr,
    GPIO_DRIVER_LEVEL level
);

/**
 * @brief Inverts the current level of the gpio-pin defined by p_pin_descr.
 * If the gpio-pin is set to output, this function does nothing.
 * 
 * @param p_pin_descr 
 * @see GPIO_DRIVER_PIN_DESCRIPTOR
 */
void gpio_driver_toggle_level(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);

/**
 * @brief Get the actual level of the requested gpio-pin.
 * The returned value reflects the actual physical of the
 * requested gpio-pin. If the given gpio-pin is disabled,
 * GPIO_LEVEL_HIGH_Z is returned. If the gpio-pin is
 * configured to be inverted, the level is inverted
 * before returning it.
 * 
 * @param p_pin_descr pin descriptor of the gpio-pin
 * where to read the level from, must not be NULL,
 * @return actual (inverted) level of the requested gpio-pin,
 * or GPIO_LEVEL_HIGH_Z if the given pin is disabled
 * @see GPIO_DRIVER_PIN_DESCRIPTOR
 */
GPIO_DRIVER_LEVEL gpio_driver_get_level(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);

/**
 * @brief 
 * 
 * @param p_pin_descr 
 * @see GPIO_DRIVER_PIN_DESCRIPTOR
 */
void gpio_driver_print_pin_state(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);

/**
 * @brief Deactivates the GPIO pin given by p_pin_descr. The configuration of this pin
 * is set to deactivated. The pin then cannot be used at all.
 * 
 * @param p_pin_descr descriptor of the pin to deactivate.
 * @see GPIO_DRIVER_PIN_DESCRIPTOR
 */
void gpio_driver_deactivate(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);

/**
 * @brief Activates the GPIO pin given by p_pin_descr. The configuration of this pin
 * is set to activated. The pin then can be used.
 * 
 * @param p_pin_descr descriptor of the pin to be activate
 * @see GPIO_DRIVER_PIN_DESCRIPTOR
 */
void gpio_driver_activate(GPIO_DRIVER_PIN_DESCRIPTOR* p_pin_descr);

// --------------------------------------------------------------------------------

#endif // _H_gpio_interface_

// --------------------------------------------------------------------------------
