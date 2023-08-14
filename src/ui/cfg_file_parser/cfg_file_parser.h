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
 * @file    cfg_file_parser.h
 * @author  Sebastian Lesse
 * @date    2023 / 07 / 15
 * @brief   Interface to a file with configuration data in it.
 *          Example content:        key_01=value_02
 *                                  key_02=value_02
 */

// --------------------------------------------------------------------------------

#ifndef _H_cfg_file_parser_
#define _H_cfg_file_parser_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "mcu_task_management/mcu_task_interface.h"
#include "common/signal_slot_interface.h"

// --------------------------------------------------------------------------------

#ifndef CFG_FILE_PARSER_MAX_LENGTH_PATH
/**
 * @brief Maximum number of characters that can be used for a file path.
 * You can redefine this value inside your config.h. The default value is
 * 255 characters.
 */
#define CFG_FILE_PARSER_MAX_LENGTH_PATH                 255
#endif

#ifndef CFG_FILE_PARSER_MAX_LENGTH_CFG_KEY
/**
 * @brief Maximum number of characters that can be used for a configuration key.
 * The value includes the termination character.
 * The useable length of key is CFG_FILE_PARSER_MAX_LENGTH_CFG_KEY - 1
 */
#define CFG_FILE_PARSER_MAX_LENGTH_CFG_KEY              255
#endif

#ifndef CFG_FILE_PARSER_MAX_LENGTH_CFG_VALUE
/**
 * @brief Maximum number of characters that can be used for a configuration value.
 * The value includes the termination character.
 * The useable length of key is CFG_FILE_PARSER_MAX_LENGTH_CFG_VALUE - 1
 */
#define CFG_FILE_PARSER_MAX_LENGTH_CFG_VALUE            255
#endif

#ifndef CFG_FILE_PARSER_MAX_LINE_LENGTH
/**
 * @brief Maximum number of characters that are exected in a single line.
 * You can redefine this value inside your config.h.
 * The default value is CFG_FILE_PARSER_MAX_LENGTH_PATH + CFG_FILE_PARSER_MAX_LENGTH_CFG_VALUE + 2.
 * 2: 1 for CFG_FILE_PARSER_KEY_VALUE_SPLITTER / 1 for newline character
 */
#define CFG_FILE_PARSER_MAX_LINE_LENGTH                 (CFG_FILE_PARSER_MAX_LENGTH_PATH + CFG_FILE_PARSER_MAX_LENGTH_CFG_VALUE + 2)
#endif

#ifndef CFG_FILE_PARSER_KEY_VALUE_SPLITTER
/**
 * @brief Key / Value seperator to be used.
 * You can redefine this value inside your config.h.
 * The default value is '='
 */
#define CFG_FILE_PARSER_KEY_VALUE_SPLITTER              '='
#endif

#ifndef CFG_FILE_PARSER_COMMENT_CHARACTER
/**
 * @brief Character that indicates a comment line which is ignored.
 * You can redefine this value inside your config.h.
 * The default value is '#'.
 */
#define CFG_FILE_PARSER_COMMENT_CHARACTER               '#'
#endif

#ifndef CFG_FILE_PARSER_WAIT_FOR_CFG_FILE_TIMEOUT_MS
/**
 * @brief Timeout in milliseconds after which the CFG_PARSER_CFG_COMPLETE_SIGNAL is send
 * after system start in case no cfg-file was given.
 * You can redefine this value inside your config.h.
 * The default value is 150 ms.
 */
#define CFG_FILE_PARSER_WAIT_FOR_CFG_FILE_TIMEOUT_MS    150
#endif

#ifndef CFG_FILE_PARSER_TASK_RUN_INTERVAL_MS
/**
 * @brief Schedule interval in milliseconds at which the cfg-file-parser task is executed.
 * You can redefine this value inside your config.h.
 * The default value is 50 ms.
 * 
 */
#define CFG_FILE_PARSER_TASK_RUN_INTERVAL_MS            50
#endif

// --------------------------------------------------------------------------------

/**
 * @brief A configuration object that is send via the 
 * CFG_PARSER_NEW_CFG_OBJECT_SIGNAL. 
 */
typedef struct {
    char key[CFG_FILE_PARSER_MAX_LENGTH_CFG_KEY];
    char value[CFG_FILE_PARSER_MAX_LENGTH_CFG_VALUE];
} CFG_FILE_PARSER_CFG_OBJECT_TYPE;

// --------------------------------------------------------------------------------

/**
 * @brief The signal CFG_PARSER_CFG_FILE_SIGNAL transfers the path to the
 * configuration file that will be processed.
 * @param argument - path to the configuration file as string.
 */
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CFG_PARSER_CFG_FILE_SIGNAL)

/**
 * @brief The signal is send by the cfg-file-parser. Every configuration pair of
 * the current file is send seperately.
 * @param argument - const reference to an temporary instance of CFG_FILE_PARSER_CFG_OBJECT_TYPE
 */
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CFG_PARSER_NEW_CFG_OBJECT_SIGNAL)

/**
 * @brief Is sent by the cfg-file-parser after the current
 * configuration file was read completely. Is also send after 
 * CFG_FILE_PARSER_WAIT_FOR_CFG_FILE_TIMEOUT_MS in case no cfg-file is available.
 * @param argument - NULL
 */
SIGNAL_SLOT_INTERFACE_INCLUDE_SIGNAL(CFG_PARSER_CFG_COMPLETE_SIGNAL)

// --------------------------------------------------------------------------------

/**
 * @brief Initializes the cfg-file-parser and starts its task.
 */
void cfg_file_parser_init(void);

// --------------------------------------------------------------------------------

#endif // _H_cfg_file_parser_

// --------------------------------------------------------------------------------

