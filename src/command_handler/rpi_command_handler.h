/*! \file *********************************************************************

 *****************************************************************************/

#ifndef RPI_COMMAND_HANDLER_H_
#define RPI_COMMAND_HANDLER_H_

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"


#include "command_management/command_handler_interface.h"


#define RPI_COMMAND_GET_VERSION			0x01
#define RPI_COMMAND_GET_INPUT_LIST		0x02
#define RPI_COMMAND_GET_OUTPUT_LIST		0x03
#define RPI_COMMAND_SET_OUTPUT			0x04
#define RPI_COMMAND_GET_OUTPUT_STATE		0x05
#define RPI_COMMAND_GET_INPUT_STATE		0x06
#define RPI_COMMAND_GET_TEMPERATURE		0x07
#define RPI_COMMAND_GET_HUMIDTY			0x08
#define RPI_COMMAND_GET_ADC			0x09
#define RPI_COMMAND_GET_LIGHT			0x0A


/*!
 *
 */
void rpi_cmd_handler_init(void);

/*!
 *
 * @param cmd_code
 */
void rpi_cmd_handler_set_request(PROTOCOL_INTERFACE* p_protocol_handler);

/*!
 *
 */
void rpi_cmd_handler_set_unrequested(void);

/*!
 *
 * @return
 */
u8 rpi_cmd_handler_get_command_code(void);

/*!
 *
 * @return
 */
u8 rpi_cmd_handler_is_requested(void);

/*!
 *
 * @return
 */
u8 rpi_cmd_default_handler(void);

/*!
 *
 * @param p_cmd_buffer
 * @param p_answ_buffer
 * @return
 */
u8 rpi_cmd_get_version(void);

/*!
 *
 * @return
 */
u8 rpi_cmd_get_input_list(void);

/*!
 *
 * @return
 */
u8 rpi_cmd_get_output_list(void);

/*!
 *
 * @return
 */
u8 rpi_cmd_set_output(void);

/*!
 *
 * @return
 */
u8 rpi_cmd_get_input_state(void);

/*!
 *
 * @return
 */
u8 rpi_cmd_get_output_state(void);

/*!
 *
 * @return
 */
u8 rpi_cmd_get_temperature(void);

/*!
 *
 * @return
 */
u8 rpi_cmd_get_humidity(void);

/*!
 *
 * @return
 */
u8 rpi_cmd_get_adc(void);

/*!
 *
 * @return
 */
u8 rpi_cmd_get_light(void);

#endif //RPI_COMMAND_HANDLER_H_
