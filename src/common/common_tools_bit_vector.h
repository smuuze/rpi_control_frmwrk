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
 * @file    common_tools_bit_vector.h
 * @author  Sebastian Lesse
 * @date    2022 / 09 / 24
 * @brief   bit-vector tools
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_common_tools_bit_vector_
#define _H_common_tools_bit_vector_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#define _BIT_00     1u
#define _BIT_01     2u
#define _BIT_02     4u
#define _BIT_03     8u
#define _BIT_04     16u
#define _BIT_05     32u
#define _BIT_06     64u
#define _BIT_07     128u
#define _BIT_08     256u
#define _BIT_09     512u
#define _BIT_10     1024u
#define _BIT_11     2048u
#define _BIT_12     4096u
#define _BIT_13     8192u
#define _BIT_14     16384u
#define _BIT_15     32768u
#define _BIT_16     65536u
#define _BIT_17     131072u
#define _BIT_18     262144u
#define _BIT_19     524288u
#define _BIT_20     1048576u
#define _BIT_21     2097152u
#define _BIT_22     4194304u
#define _BIT_23     8388608u
#define _BIT_24     16777216u
#define _BIT_25     33554432u
#define _BIT_26     67108864u
#define _BIT_27     134217728u
#define _BIT_28     268435456u
#define _BIT_29     536870912u
#define _BIT_30     1073741824u
#define _BIT_31     2147483648u

// --------------------------------------------------------------------------------

#define _BIT_RETURN(en, bit) (en ? bit : 0u)

// --------------------------------------------------------------------------------

/**
 * @brief Creates a bit-vector with a length of 8 bits.
 * If this macro is used on a target variable with a higher bit-length
 * the higher bits will be set to zero.
 * 
 * Usage:
 * 
 * - create the bit-vector:
 * 
 *     u8 bit_vector_0x01 = BUILD_BIT_VECTOR_U8(0,0,0,0,0,0,0,1);
 *     u8 bit_vector_0x80 = BUILD_BIT_VECTOR_U8(1,0,0,0,0,0,0,0);
 *     u8 bit_vector_0xFF = BUILD_BIT_VECTOR_U8(1,1,1,1,1,1,1,1);
 * 
 */
#define BUILD_BIT_VECTOR_U8(b07,b06,b05,b04,b03,b02,b01,b00)    \
          (u8)_BIT_RETURN(b00, _BIT_00)                         \
        + (u8)_BIT_RETURN(b01, _BIT_01)                         \
        + (u8)_BIT_RETURN(b02, _BIT_02)                         \
        + (u8)_BIT_RETURN(b03, _BIT_03)                         \
        + (u8)_BIT_RETURN(b04, _BIT_04)                         \
        + (u8)_BIT_RETURN(b05, _BIT_05)                         \
        + (u8)_BIT_RETURN(b06, _BIT_06)                         \
        + (u8)_BIT_RETURN(b07, _BIT_07)

/**
 * @brief Creates a bit-vector with a length of 32 bits.
 * If this macro is used on a target variable with a higher bit-length
 * the higher bits will be set to zero.
 * 
 * Usage:
 * 
 * - create the bit-vector:
 * 
 *     u8 bit_vector_0x00000001 = BUILD_BIT_VECTOR_U32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,1);
 *     u8 bit_vector_0x00000080 = BUILD_BIT_VECTOR_U32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 1,0,0,0,0,0,0,0);
 *     u8 bit_vector_0x000000FF = BUILD_BIT_VECTOR_U32(0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1);
 * 
 */
#define BUILD_BIT_VECTOR_U32(b31,b30,b29,b28,b27,b26,b25,b24, b23,b22,b21,b20,b19,b18,b17,b16, b15,b14,b13,b12,b11,b10,b09,b08, b07,b06,b05,b04,b03,b02,b01,b00)    \
          (u32)_BIT_RETURN(b00, _BIT_00)                        \
        + (u32)_BIT_RETURN(b01, _BIT_01)                        \
        + (u32)_BIT_RETURN(b02, _BIT_02)                        \
        + (u32)_BIT_RETURN(b03, _BIT_03)                        \
        + (u32)_BIT_RETURN(b04, _BIT_04)                        \
        + (u32)_BIT_RETURN(b05, _BIT_05)                        \
        + (u32)_BIT_RETURN(b06, _BIT_06)                        \
        + (u32)_BIT_RETURN(b07, _BIT_07)                        \
        + (u32)_BIT_RETURN(b08, _BIT_08)                        \
        + (u32)_BIT_RETURN(b09, _BIT_09)                        \
        + (u32)_BIT_RETURN(b10, _BIT_10)                        \
        + (u32)_BIT_RETURN(b11, _BIT_11)                        \
        + (u32)_BIT_RETURN(b12, _BIT_12)                        \
        + (u32)_BIT_RETURN(b13, _BIT_13)                        \
        + (u32)_BIT_RETURN(b14, _BIT_14)                        \
        + (u32)_BIT_RETURN(b15, _BIT_15)                        \
        + (u32)_BIT_RETURN(b16, _BIT_16)                        \
        + (u32)_BIT_RETURN(b17, _BIT_17)                        \
        + (u32)_BIT_RETURN(b18, _BIT_18)                        \
        + (u32)_BIT_RETURN(b19, _BIT_19)                        \
        + (u32)_BIT_RETURN(b20, _BIT_20)                        \
        + (u32)_BIT_RETURN(b21, _BIT_21)                        \
        + (u32)_BIT_RETURN(b22, _BIT_22)                        \
        + (u32)_BIT_RETURN(b23, _BIT_23)                        \
        + (u32)_BIT_RETURN(b24, _BIT_24)                        \
        + (u32)_BIT_RETURN(b25, _BIT_25)                        \
        + (u32)_BIT_RETURN(b26, _BIT_26)                        \
        + (u32)_BIT_RETURN(b27, _BIT_27)                        \
        + (u32)_BIT_RETURN(b28, _BIT_28)                        \
        + (u32)_BIT_RETURN(b29, _BIT_29)                        \
        + (u32)_BIT_RETURN(b30, _BIT_30)                        \
        + (u32)_BIT_RETURN(b31, _BIT_31)

// --------------------------------------------------------------------------------

#endif // _H_common_tools_bit_vector_

// --------------------------------------------------------------------------------

