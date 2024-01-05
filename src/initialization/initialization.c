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
 * @file   initialization.c
 * @author Sebastian Lesse
 * @date   2018 / 11 / 16
 * @brief  Initialization routine
 * 
 */

#define TRACER_OFF

//-----------------------------------------------------------------------------

#ifdef TRACER_ON
#pragma __WARNING__TRACES_ENABLED__
#endif

//-----------------------------------------------------------------------------

#include "config.h"  // immer als erstes einbinden!

//-----------------------------------------------------------------------------

#include "tracer.h"

//-----------------------------------------------------------------------------

#include "cpu.h"

//-----------------------------------------------------------------------------

#include "system/system_interface.h"

#include "local_context.h"

#include "initialization/system_initialization.h"
#include "initialization/button_initialization.h"
#include "initialization/output_initialization.h"
#include "initialization/protocol_initialization.h"
#include "initialization/command_initialization.h"
#include "initialization/task_initialization.h"
#include "initialization/sensor_initialization.h"

#include "ui/command_line/command_line_interface.h"
#include "modules/cfg_file_parser/cfg_file_parser.h"
#include "ui/log_interface/log_interface.h"

#include "app_tasks/message_executer_task.h"
#include "app_tasks/cli_executer_task.h"
#include "app_tasks/keypad_to_lcd_task.h"
#include "app_tasks/led_blinker_task.h"

#include "power_management/power_management_interface.h"

#include "modules/lcd/lcd_interface.h"
#include "modules/keypad/keypad_interface.h"
#include "modules/ir/ir_protocol_task.h"
#include "modules/movement_detection/movement_detection_controller.h"

#include "copro/copro_interface.h"

#include "expansion/driver_PCA9670.h"

#include "modules/mcu_top/mcu_top.h"

//-----------------------------------------------------------------------------

SYSTEM_T system_context;

//-----------------------------------------------------------------------------

void initialization(void) {

	watchdog_enable();
	watchdog();

	system_initialization();

	task_initialization();

	#ifdef HAS_POWER_MANAGEMENT_MODULE
	{
		power_management_initialization();
	}
	#endif

	sensor_initialization();

	button_initialization();

	output_initialization();

	protocol_initialization();

	command_initialization();

	#ifdef CLI_AVAILABLE
	{
		command_line_interface_init();
	}
	#endif

	#ifdef CFG_FILE_PARSER_AVAILABLE
	{
		cfg_file_parser_init();
	}
	#endif

	#ifdef LOG_INTERFACE_AVAILABLE
	{
		log_interface_init();
	}
	#endif

	#ifdef HAS_APP_TASK_MSG_EXECUTER
	{
		msg_executer_init();
	}
	#endif

	#ifdef HAS_APP_TASK_CLI_EXECUTER
	{
		cli_executer_init();
	}
	#endif

    #ifdef LCD_CONTROLLER_AVAILABLE
    {
        lcd_init();
    }
    #endif

    #ifdef KEYPAD_3x4_CONTROLLER_AVAILABLE
    {
        keypad_init();
    }
    #endif

    #ifdef IR_PROTOCOL_AVAILABLE
    {
        ir_protocol_init();
    }
    #endif

    #ifdef HAS_APP_TASK_LED_BLINKER
    {
        led_blinker_init();
    }
    #endif

    #ifdef HAS_APP_TASK_KEY_2_LCD
    {
        key_2_lcd_init();
    }
    #endif

    #ifdef MOVEMENT_DETECTION_CONTROLLER_AVAILABLE
    {
        movement_detection_controller_init();
    }
    #endif

    #if COPRO_INTERFACE_AVAILABLE
    {
        copro_interface_initialize();
    }
    #endif

    #ifdef HAS_EXPANSION_BOARD_GPIO_PCA9670
    #if defined HAS_DRIVER_I2C0 && HAS_DRIVER_I2C0 == 1
    {
        // UNFINISHED - check if this is working correctly
        // pca9670_init(i_system.driver.i2c0);
    }
    #endif
    #endif

    #ifdef MCU_TOP_AVAILABLE
    {
        mcu_top_init();
    }
    #endif
}

//-----------------------------------------------------------------------------

void deinitialization(void) {
    system_deinitialization();
}

//-----------------------------------------------------------------------------
