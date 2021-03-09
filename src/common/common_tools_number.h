/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	commn_tools_number.h
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#ifndef COMMON_TOOLS_NUMBER_H_
#define COMMON_TOOLS_NUMBER_H_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

/**
 * @brief Converts the byte-array to a unsigned 32 bit value in LSByte first order
 * 
 * @param p_array byte-array 
 * @return number in LSByte first order
 */
u32 common_tools_number_u32_from_array(const u8* p_array);

/**
 * @brief Converts the byte-array to a unsigned 16 bit value in LSByte first order
 * 
 * @param p_array byte-array 
 * @return number in LSByte first order
 */
u16 common_tools_number_u16_from_array(const u8* p_array);

/**
 * @brief Converts the byte-array to a signed 32 bit value in LSByte first order
 * 
 * @param p_array byte-array 
 * @return number in LSByte first order
 */
i32 common_tools_number_i32_from_array(const u8* p_array);

/**
 * @brief Converts the byte-array to a signed 16 bit value in LSByte first order
 * 
 * @param p_array byte-array 
 * @return number in LSByte first order
 */
i16 common_tools_number_i16_from_array(const u8* p_array);

/**
 * @brief Copies a 32 bit number into a byte-array in LSByte first order
 * 
 * @param p_array byte array where the number is copied into
 * @param number 32 bit number
 */
void common_tools_number_u32_to_array(u8* p_array, u32 number);

/**
 * @brief Copies a 16 bit number into a byte-array in LSByte first order
 * 
 * @param p_array byte array where the number is copied into
 * @param number 16 bit number
 */
void common_tools_number_u16_to_array(u8* p_array, u16 number);

/**
 * @brief Converts a u8 value into a i32 value by respecting the signess of the u8 value
 * 
 * @param number u8 number, can be signed or unsigned
 * @param is_signed if 1 the signess of the number is respected, 0 signess is not respected
 * @return signed 32 bit value
 */
i32 common_tools_number_from_u8(u8 number, u8 is_signed);

// --------------------------------------------------------------------------------

#endif //COMMON_TOOLS_NUMBER_H_
