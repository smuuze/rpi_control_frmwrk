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
 * @file    led_blinker_task.c
 * @author  Sebastian Lesse
 * @date    2023 / 03 / 04
 * @brief   Short description of this file
 * 
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#ifdef TRACER_ON
#warning __WARNING__TRACER_ENABLED__WARNING__
#endif

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "cpu.h"

// --------------------------------------------------------------------------------

#include "driver/gpio/gpio_interface.h"
#include "mcu_task_management/mcu_task_interface.h"
#include "time_management/time_management.h"

// --------------------------------------------------------------------------------

INCLUDE_GPIO_ALIAS(LED_ON_BOARD)

// --------------------------------------------------------------------------------

TIME_MGMN_BUILD_STATIC_TIMER_U16(LED_BLINK_TIMER)

// --------------------------------------------------------------------------------

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.start
 */
static void LED_BLINKER_TASK_start(void) {
    LED_BLINK_TIMER_start();
    LED_ON_BOARD_drive_low();
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_schedule_interval
 */
static u16 LED_BLINKER_TASK_get_schedule_interval(void) {
    return 100;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.get_state
 */
static MCU_TASK_INTERFACE_TASK_STATE LED_BLINKER_task_get_state(void) {
    return LED_BLINK_TIMER_is_up(490) ? MCU_TASK_RUNNING : MCU_TASK_SLEEPING;
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.run
 */
static void LED_BLINKER_TASK_execute(void) {
    LED_BLINK_TIMER_start();
    LED_ON_BOARD_toggle_level();
}

/**
 * @see  mcu_task_management/mcu_task_interface.h#MCU_TASK_INTERFACE.terminate
 */
static void LED_BLINKER_TASK_terminate(void) {

}

// --------------------------------------------------------------------------------

TASK_CREATE(
   LED_BLINKER_TASK,
   TASK_PRIORITY_MIDDLE,
   LED_BLINKER_TASK_get_schedule_interval,
   LED_BLINKER_TASK_start,
   LED_BLINKER_TASK_execute,
   LED_BLINKER_task_get_state,
   LED_BLINKER_TASK_terminate
)

// --------------------------------------------------------------------------------

/**
 * @brief Initializes the LED blinker task.
 */
void led_blinker_init(void) {
    LED_BLINKER_TASK_init();
}

// --------------------------------------------------------------------------------
