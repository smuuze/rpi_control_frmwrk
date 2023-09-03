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
 * @file    copro.c
 * @author  Sebastian Lesse
 * @date    2020 / 05 / 26
 * @brief   Implementation of the copro-interface definition
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

#include "system/system_interface.h"
#include "copro/copro_interface.h"
#include "common/local_mutex.h"
#include "command_handler/rpi_cmd_handler_routing.h"
#include "power_management/power_module_5V.h"

#include "driver/cfg_driver_interface.h"

// --------------------------------------------------------------------------------

#ifndef COPRO1_DEFAULT_ADDRESS
#define COPRO1_DEFAULT_ADDRESS                                              0x11
#endif

// --------------------------------------------------------------------------------

#ifdef COPRO1_AVAILABLE

void copro1_POWER_ON_CALLBACK(void) {

    if (POWER_UNIT_5V_is_on() == 0) {
            DEBUG_PASS("copro1_POWER_ON_CALLBACK() - Requesting 5V power-unit");
            POWER_UNIT_5V_request();
    }
}

void copro1_POWER_OFF_CALLBACK(void) {
    //POWER_UNIT_5V_release();
}

u8 copro1_POWER_IS_ON_CALLBACK(void) {
    return POWER_UNIT_5V_is_on();
}

COPRO_INTERFACE_CRATE(
    COPRO1,
    copro1_POWER_ON_CALLBACK,
    copro1_POWER_IS_ON_CALLBACK,
    copro1_POWER_OFF_CALLBACK
)
 
static TRX_DRIVER_CONFIGURATION copro1_driver_cfg = { COPRO1_DRIVER_CFG };

#endif

// --------------------------------------------------------------------------------

#ifdef COPRO2_AVAILABLE

void copro2_POWER_ON_CALLBACK(void) { }
void copro2_POWER_OFF_CALLBACK(void) { }
COPRO_INTERFACE_CRATE(COPRO2, copro2_POWER_ON_CALLBACK, copro2_POWER_OFF_CALLBACK))

#endif

// --------------------------------------------------------------------------------

#ifdef COPRO3_AVAILABLE

void copro3_POWER_ON_CALLBACK(void) { }
void copro3_POWER_OFF_CALLBACK(void) { }
COPRO_INTERFACE_CRATE(COPRO3, copro3_POWER_ON_CALLBACK, copro3_POWER_OFF_CALLBACK))

#endif

// --------------------------------------------------------------------------------

#ifdef COPRO4_AVAILABLE

void copro4_POWER_ON_CALLBACK(void) { }
void copro4_POWER_OFF_CALLBACK(void) { }
COPRO_INTERFACE_CRATE(COPRO4, copro4_POWER_ON_CALLBACK, copro4_POWER_OFF_CALLBACK))

#endif

// --------------------------------------------------------------------------------

void copro_interface_initialize(void) {

    DEBUG_PASS("copro_interface_initialize()");

    #ifdef COPRO1_AVAILABLE
    {
        DEBUG_PASS("copro_interface_initialize() - COPRO1");

        #ifdef COPRO1_I2C0
        {
            DEBUG_PASS("copro_interface_initialize() - COPRO1 over I2C0");
            COPRO1_initialize(i_system.driver.i2c0, &copro1_driver_cfg);
        }
        #endif

        COPRO1_set_address(COPRO1_DEFAULT_ADDRESS);
    }
    #endif
}

// --------------------------------------------------------------------------------

void copro_initialize(COPRO_INTERFACE* p_copro, TRX_DRIVER_INTERFACE* p_com_driver_interface, TRX_DRIVER_CONFIGURATION* p_com_driver_cfg) {
    p_copro->p_com_driver = p_com_driver_interface;
    p_copro->p_com_driver_cfg = p_com_driver_cfg;
}

void copro_power_on(COPRO_INTERFACE* p_copro) {
    p_copro->p_com_driver->configure(p_copro->p_com_driver_cfg);
}

void copro_power_off(COPRO_INTERFACE* p_copro) {

}

u8 copro_bytes_available(COPRO_INTERFACE* p_copro) {
    return p_copro->p_com_driver->bytes_available();
}

u8 copro_get_N_bytes(COPRO_INTERFACE* p_copro, u8 num_bytes, u8* p_buffer_to) {
    return p_copro->p_com_driver->get_N_bytes(num_bytes, p_buffer_to);
}

u8 copro_set_N_bytes(COPRO_INTERFACE* p_copro, u8 num_bytes, const u8* p_buffer_from) {
    return p_copro->p_com_driver->set_N_bytes(num_bytes, p_buffer_from);
}

u8 copro_is_ready_for_rx(COPRO_INTERFACE* p_copro) {
    return p_copro->p_com_driver->is_ready_for_rx();
}

void copro_start_rx(COPRO_INTERFACE* p_copro, u16 num_of_rx_bytes) {
    p_copro->p_com_driver->set_address(p_copro->address);
    p_copro->p_com_driver->start_rx(num_of_rx_bytes);
}

void copro_wait_for_rx(COPRO_INTERFACE* p_copro, u8 num_bytes, u16 timeout_ms) {
    p_copro->p_com_driver->wait_for_rx(num_bytes, timeout_ms);
}

void copro_stop_rx(COPRO_INTERFACE* p_copro) {
    p_copro->p_com_driver->stop_rx();
}

u8 copro_is_ready_for_tx(COPRO_INTERFACE* p_copro) {
    return p_copro->p_com_driver->is_ready_for_tx();
}

void copro_start_tx(COPRO_INTERFACE* p_copro) {
    p_copro->p_com_driver->set_address(p_copro->address);
    p_copro->p_com_driver->start_tx();
}

void copro_wait_for_tx(COPRO_INTERFACE* p_copro, u8 num_bytes, u16 timeout_ms) {
    p_copro->p_com_driver->wait_for_tx(num_bytes, timeout_ms);
}

void copro_stop_tx(COPRO_INTERFACE* p_copro) {
    p_copro->p_com_driver->stop_tx();
}

void copro_clear_rx_buffer(COPRO_INTERFACE* p_copro) {
    p_copro->p_com_driver->clear_rx_buffer();
}

void copro_clear_tx_buffer(COPRO_INTERFACE* p_copro) {
    p_copro->p_com_driver->clear_tx_buffer();
}

void copro_set_address(COPRO_INTERFACE* p_copro, u8 addr) {
    p_copro->address = addr;
    DEBUG_TRACE_byte(p_copro->address, "copro_set_address()");
}

u8 copro_get_address(COPRO_INTERFACE* p_copro) {
    DEBUG_TRACE_byte(p_copro->address, "copro_get_address()");
    return p_copro->address;
}

u8 copro_mutex_request(COPRO_INTERFACE* p_copro) {

    if (p_copro->mutex_id != MUTEX_INVALID_ID) {
        return 0;
    }

    p_copro->mutex_id = p_copro->p_com_driver->mutex_req();
    return 1;
}

void copro_mutex_release(COPRO_INTERFACE* p_copro) {
    p_copro->p_com_driver->mutex_rel(p_copro->mutex_id);
    p_copro->mutex_id = MUTEX_INVALID_ID;
}

// --------------------------------------------------------------------------------
