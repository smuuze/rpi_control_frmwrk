#ifndef RPI_COMMAND_HANDLER_H_
#define RPI_COMMAND_HANDLER_H_

/**
 * @file 	rpi_command_handler.h
 * @author 	sebastian lesse (sebastian lesse)
 * @brief 
 * @version 	1.0
 * @date 	2021-02-05
 * 
 * @copyright 	Copyright (c) 2021
 * 
 */

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "command_management/command_handler_interface.h"

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
void rpi_cmd_handler_init(void);

/**
 * @brief 
 * 
 * @param p_protocol_handler 
 */
void rpi_cmd_handler_set_request(PROTOCOL_INTERFACE* p_protocol_handler);

/**
 * @brief 
 * 
 * @return PROTOCOL_INTERFACE* 
 */
PROTOCOL_INTERFACE* rpi_cmd_handler_get_protocol(void);

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
u8 rpi_cmd_default_handler(PROTOCOL_INTERFACE* p_protocol);

/**
 * @brief 
 * 
 * @param p_protocol 
 * @return u8 
 */
u8 rpi_cmd_get_version(PROTOCOL_INTERFACE* p_protocol);

/**
 * @brief 
 * 
 * @param p_protocol 
 * @return u8 
 */
u8 rpi_cmd_get_input_list(PROTOCOL_INTERFACE* p_protocol);

/**
 * @brief 
 * 
 * @param p_protocol 
 * @return u8 
 */
u8 rpi_cmd_get_output_list(PROTOCOL_INTERFACE* p_protocol);

/**
 * @brief 
 * 
 * @param p_protocol 
 * @return u8 
 */
u8 rpi_cmd_set_output(PROTOCOL_INTERFACE* p_protocol);

/**
 * @brief 
 * 
 * @param p_protocol 
 * @return u8 
 */
u8 rpi_cmd_get_input_state(PROTOCOL_INTERFACE* p_protocol);

/**
 * @brief 
 * 
 * @param p_protocol 
 * @return u8 
 */
u8 rpi_cmd_get_output_state(PROTOCOL_INTERFACE* p_protocol);

/**
 * @brief 
 * 
 * @param p_protocol 
 * @return u8 
 */
u8 rpi_cmd_get_temperature(PROTOCOL_INTERFACE* p_protocol);

/**
 * @brief 
 * 
 * @param p_protocol 
 * @return u8 
 */
u8 rpi_cmd_get_humidity(PROTOCOL_INTERFACE* p_protocol);

/**
 * @brief 
 * 
 * @param p_protocol 
 * @return u8 
 */
u8 rpi_cmd_get_adc(PROTOCOL_INTERFACE* p_protocol);

/**
 * @brief 
 * 
 * @param p_protocol 
 * @return u8 
 */
u8 rpi_cmd_get_light(PROTOCOL_INTERFACE* p_protocol);

// --------------------------------------------------------------------------------

#endif //RPI_COMMAND_HANDLER_H_
