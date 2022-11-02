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
 * @file    local_module_status.h
 * @author  Sebastian Lesse
 * @date    2022 / 10 / 09
 * @brief   Short description of this file
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_local_module_status_
#define _H_local_module_status_

// --------------------------------------------------------------------------------

#include "config.h"
#include <string.h>

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
typedef struct MODULE_STATUS_U8 {
   u8 data;
} MODULE_STATUS_U8_TYPE;

/**
 * @brief 
 * 
 */
typedef struct MODULE_STATUS_U16 {
   u16 data;
} MODULE_STATUS_U16_TYPE;

/**
 * @brief 
 * 
 */
typedef struct MODULE_STATUS_U32 {
   u32 data;
} MODULE_STATUS_U32_TYPE;

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
#define BUILD_MODULE_STATUS_U8(name)                                            \
                                                                                \
   static MODULE_STATUS_U8_TYPE __##name##_module_status;                       \
                                                                                \
   __UNUSED__ inline static void name##_set(u8 bit_flag) {                      \
       module_status_set_u8(&(__##name##_module_status), bit_flag);             \
    }                                                                           \
                                                                                \
   __UNUSED__ inline static u8 name##_is_set(u8 bit_flag) {                     \
       return module_status_isset_u8(&(__##name##_module_status), bit_flag);    \
    }                                                                           \
                                                                                \
   __UNUSED__ inline static void name##_unset(u8 bit_flag) {                    \
       module_status_unset_u8(&(__##name##_module_status), bit_flag);           \
    }                                                                           \
                                                                                \
   __UNUSED__ inline static void name##_clear_all(void) {                       \
       module_status_clear_all_u8(&(__##name##_module_status));                 \
    }

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
#define BUILD_MODULE_STATUS_U16(name)                                           \
                                                                                \
   static MODULE_STATUS_U16_TYPE __##name##_module_status;                      \
                                                                                \
   __UNUSED__ inline static void name##_set(u16 bit_flag) {                     \
       module_status_set_u16(&(__##name##_module_status), bit_flag);            \
    }                                                                           \
                                                                                \
   __UNUSED__ inline static u16 name##_is_set(u16 bit_flag) {                   \
       return module_status_isset_u16(&(__##name##_module_status), bit_flag);   \
    }                                                                           \
                                                                                \
   __UNUSED__ inline static void name##_unset(u16 bit_flag) {                   \
       module_status_unset_u16(&(__##name##_module_status), bit_flag);          \
    }                                                                           \
                                                                                \
   __UNUSED__ inline static void name##_clear_all(void) {                       \
       module_status_clear_all_u16(&(__##name##_module_status));                \
    }

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
#define BUILD_MODULE_STATUS_U32(name)                                       \
                                                   \
   static MODULE_STATUS_U32_TYPE __##name##_module_status;                              \
                                                   \
   __UNUSED__ inline static void name##_set(u32 bit_flag)    { module_status_set_u32(&(__##name##_module_status), bit_flag); }      \
   __UNUSED__ inline static u32 name##_is_set(u32 bit_flag){ return module_status_isset_u32(&(__##name##_module_status), bit_flag); }   \
   __UNUSED__ inline static void name##_unset(u32 bit_flag){ module_status_unset_u32(&(__##name##_module_status), bit_flag); }      \
   __UNUSED__ inline static void name##_clear_all(void)    { module_status_clear_all_u32(&(__##name##_module_status)); }

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
#define BUILD_MODULE_STATUS_FAST(name, size)                                    \
                                                   \
   static u8 __##name##_module_status[size];                                 \
                                                   \
   __UNUSED__ inline static void name##_set(u8 index)   { __##name##_module_status[index] = 1; }               \
   __UNUSED__ inline static u8 name##_is_set(u8 index)   { return __##name##_module_status[index] != 0 ? 1 : 0; }         \
   __UNUSED__ inline static void name##_unset(u8 index)   { __##name##_module_status[index] = 0; }               \
   __UNUSED__ inline static void name##_clear_all(void)   { memset(__##name##_module_status, 0x00, size); }

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
#define BUILD_MODULE_STATUS_FAST_VOLATILE(name, size)                                 \
                                                   \
   static volatile u8 __##name##_module_status[size];                                 \
                                                   \
   static inline void name##_set(u8 index)      { __##name##_module_status[index] = 1; }               \
   static inline u8 name##_is_set(u8 index)      { return __##name##_module_status[index] != 0 ? 1 : 0; }         \
   static inline void name##_unset(u8 index)      { __##name##_module_status[index] = 0; }               \
   static inline void name##_clear_all(void)      { memset((u8*)&__##name##_module_status[0], 0x00, size); }

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
#define INCLUDE_MODULE_STATUS_FAST_VOLATILE(name)                               \
   void name##_set(u8 index);                                                   \
   u8 name##_is_set(u8 index);                                                  \
   void name##_unset(u8 index);                                                 \
   void name##_clear_all(void);                                                 \

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param p_status 
 * @param bit_flag 
 */
void module_status_set_u8(MODULE_STATUS_U8_TYPE* p_status, u8 bit_flag);

/**
 * @brief 
 * 
 * @param p_status 
 * @param bit_flag 
 * @return u8 
 */
u8 module_status_isset_u8(MODULE_STATUS_U8_TYPE* p_status, u8 bit_flag);

/**
 * @brief 
 * 
 * @param p_status 
 * @param bit_flag 
 */
void module_status_unset_u8(MODULE_STATUS_U8_TYPE* p_status, u8 bit_flag);

/**
 * @brief 
 * 
 * @param p_status 
 */
void module_status_clear_all_u8(MODULE_STATUS_U8_TYPE* p_status);

/**
 * @brief 
 * 
 * @param p_status 
 * @param bit_flag 
 */
void module_status_set_u16(MODULE_STATUS_U16_TYPE* p_status, u16 bit_flag);

/**
 * @brief 
 * 
 * @param p_status 
 * @param bit_flag 
 * @return u16 
 */
u16 module_status_isset_u16(MODULE_STATUS_U16_TYPE* p_status, u16 bit_flag);

/**
 * @brief 
 * 
 * @param p_status 
 * @param bit_flag 
 */
void module_status_unset_u16(MODULE_STATUS_U16_TYPE* p_status, u16 bit_flag);

/**
 * @brief 
 * 
 * @param p_status 
 */
void module_status_clear_all_u16(MODULE_STATUS_U16_TYPE* p_status);

/**
 * @brief 
 * 
 * @param p_status 
 * @param bit_flag 
 */
void module_status_set_u32(MODULE_STATUS_U32_TYPE* p_status, u32 bit_flag);

/**
 * @brief 
 * 
 * @param p_status 
 * @param bit_flag 
 * @return u32 
 */
u32 module_status_isset_u32(MODULE_STATUS_U32_TYPE* p_status, u32 bit_flag);

/**
 * @brief 
 * 
 * @param p_status 
 * @param bit_flag 
 */
void module_status_unset_u32(MODULE_STATUS_U32_TYPE* p_status, u32 bit_flag);

/**
 * @brief 
 * 
 * @param p_status 
 */
void module_status_clear_all_u32(MODULE_STATUS_U32_TYPE* p_status);

// --------------------------------------------------------------------------------

#endif // _H_local_module_status_

// --------------------------------------------------------------------------------
