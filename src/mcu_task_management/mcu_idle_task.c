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
 * @file    idle_task.h
 * @author  Sebastian Lesse
 * @date    2022 / 12 / 12
 * @brief   Implementation of the system idle task
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

#include "system_interface.h"
#include "mcu_task_interface.h"

// --------------------------------------------------------------------------------

#define SLEEP_DRIVER_PRR_ALL_ON     (0)
#define SLEEP_DRIVER_PRR_TWI        (1 << PRTWI)
#define SLEEP_DRIVER_PRR_TIMER2     (1 << PRTIM2)
#define SLEEP_DRIVER_PRR_TIMER0     (1 << PRTIM0)
#define SLEEP_DRIVER_PRR_USART0     (1 << PRUSART0)
#define SLEEP_DRIVER_PRR_TIMER1     (1 << PRTIM1)
#define SLEEP_DRIVER_PRR_SPI        (1 << PRSPI)
#define SLEEP_DRIVER_PRR_USART1     (1 << PRUSART0)
#define SLEEP_DRIVER_PRR_ADC        (1 << PRADC)

// --------------------------------------------------------------------------------

#define SET_MODULE_POWER_SAFE(prr_module_mask)    PRR0 |= (prr_module_mask)

// --------------------------------------------------------------------------------

void mcu_idle_task_init(void) {
    DEBUG_PASS("mcu_idle_task_init()");
}

u16 mcu_idle_task_get_schedule_interval(void) {
    return 0;
}

MCU_TASK_INTERFACE_TASK_STATE mcu_idle_task_get_state(void) {
    return MCU_TASK_SLEEPING;
}

void mcu_idle_task_run(void) {
    DEBUG_PASS("mcu_idle_task_run()");

    #ifndef DISABLE_SLEEP_ON_IDLE
    {
        CPU_PREPARE_SLEEP_MODE();
        CPU_ENTER_SLEEP_MODE();
        CPU_DEACTIVATE_SLEEP_MODE();
    }
    #endif
}

void mcu_idle_task_background_run(void) {

}

void mcu_idle_task_sleep(void) {

}

void mcu_idle_task_wakeup(void) {

}

void mcu_idle_task_terminate(void) {

}

// --------------------------------------------------------------------------------
