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
 * @file    local_data_storage_array.h
 * @author  Sebastian Lesse
 * @date    2022 / 09 / 24
 * @brief   Short description of this file
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_local_data_storage_array_
#define _H_local_data_storage_array_

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#define DATA_STORAGE_ARRAY_MIN_VALUE_I8        -100
#define DATA_STORAGE_ARRAY_MAX_VALUE_I8        127

#define DATA_STORAGE_ARRAY_MIN_VALUE_U8        0
#define DATA_STORAGE_ARRAY_MAX_VALUE_U8        255

// --------------------------------------------------------------------------------

/*!
 *
 */
typedef struct LOCAL_DATA_STORAGE_ARRAY_I8 {
    i8* p_buffer;
    u8 size;
    u8 write_pointer;
    u8 array_not_full;
} LOCAL_DATA_STORAGE_ARRAY_I8_TYPE;

/*!
 *
 */
typedef struct LOCAL_DATA_STORAGE_ARRAY_U8 {
    u8* p_buffer;
    u8 size;
    u8 write_pointer;
    u8 array_not_full;
} LOCAL_DATA_STORAGE_ARRAY_U8_TYPE;

/*!
 *
 */
typedef struct LOCAL_DATA_STORAGE_ARRAY_U16 {
    u16* p_buffer;
    u8 size;
    u8 write_pointer;
    u8 array_not_full;
} LOCAL_DATA_STORAGE_ARRAY_U16_TYPE;

/*!
 *
 */
typedef struct LOCAL_DATA_STORAGE_ARRAY_U32 {
    u32* p_buffer;
    u8 size;
    u8 write_pointer;
    u8 array_not_full;
} LOCAL_DATA_STORAGE_ARRAY_U32_TYPE;

// --------------------------------------------------------------------------------

/*!
 *
 * @param p_context
 */
void local_data_storage_array_init_i8(LOCAL_DATA_STORAGE_ARRAY_I8_TYPE* p_context);

/*!
 *
 * @param p_context
 * @param value
 */
void local_data_storage_array_add_value_i8(LOCAL_DATA_STORAGE_ARRAY_I8_TYPE* p_context, i8 value);

/*!
 *
 * @param p_context
 * @return
 */
i8 local_data_storage_array_get_mean_i8(LOCAL_DATA_STORAGE_ARRAY_I8_TYPE* p_context);

/*!
 *
 * @param p_context
 * @return
 */
i8 local_data_storage_array_get_max_i8(LOCAL_DATA_STORAGE_ARRAY_I8_TYPE* p_context);

/*!
 *
 * @param p_context
 * @return
 */
i8 local_data_storage_array_get_min_i8(LOCAL_DATA_STORAGE_ARRAY_I8_TYPE* p_context);

/*!
 *
 * @param p_context
 */
void local_data_storage_array_init_u8(LOCAL_DATA_STORAGE_ARRAY_U8_TYPE* p_context);

/*!
 *
 * @param p_context
 * @param value
 */
void local_data_storage_array_add_value_u8(LOCAL_DATA_STORAGE_ARRAY_U8_TYPE* p_context, u8 value);

/*!
 *
 * @param p_context
 * @return
 */
u8 local_data_storage_array_get_mean_u8(LOCAL_DATA_STORAGE_ARRAY_U8_TYPE* p_context);

/*!
 *
 * @param p_context
 * @return
 */
u8 local_data_storage_array_get_max_u8(LOCAL_DATA_STORAGE_ARRAY_U8_TYPE* p_context);

/*!
 *
 * @param p_context
 * @return
 */
u8 local_data_storage_array_get_min_u8(LOCAL_DATA_STORAGE_ARRAY_U8_TYPE* p_context);

/*!
 *
 * @param p_context
 */
void local_data_storage_array_debug_print_data_u8(LOCAL_DATA_STORAGE_ARRAY_U8_TYPE* p_context);

/*!
 *
 * @param p_context
 */
void local_data_storage_array_init_u16(LOCAL_DATA_STORAGE_ARRAY_U16_TYPE* p_context);

/*!
 *
 * @param p_context
 */
void local_data_storage_array_init_u32(LOCAL_DATA_STORAGE_ARRAY_U32_TYPE* p_context);

/*!
 *
 * @param p_context
 * @param value
 */
void local_data_storage_array_add_value_u16(LOCAL_DATA_STORAGE_ARRAY_U16_TYPE* p_context, u16 value);

/*!
 *
 * @param p_context
 * @param value
 */
void local_data_storage_array_add_value_u32(LOCAL_DATA_STORAGE_ARRAY_U32_TYPE* p_context, u32 value);

/*!
 *
 * @param p_context
 * @return
 */
u16 local_data_storage_array_get_mean_u16(LOCAL_DATA_STORAGE_ARRAY_U16_TYPE* p_context);

/*!
 *
 * @param p_context
 * @return
 */
u32 local_data_storage_array_get_mean_u32(LOCAL_DATA_STORAGE_ARRAY_U32_TYPE* p_context);

/*!
 *
 * @param p_context
 * @return
 */
u16 local_data_storage_array_get_max_u16(LOCAL_DATA_STORAGE_ARRAY_U16_TYPE* p_context);

/*!
 *
 * @param p_context
 * @return
 */
u32 local_data_storage_array_get_max_u32(LOCAL_DATA_STORAGE_ARRAY_U32_TYPE* p_context);

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
#define BUILD_LOCAL_DATA_STORAGE_ARRAY_I8(name, size)                               \
                                                                                    \
    static i8 _##name##_data_storage_buffer[size];                                  \
    static LOCAL_DATA_STORAGE_ARRAY_I8_TYPE _##name##_data_storage_context = {      \
        _##name##_data_storage_buffer,                                              \
        size,                                                                       \
        0,                                                                          \
        0                                                                           \
    };                                                                              \
                                                                                    \
    inline static void name##_data_storage_array_init(void) {                       \
        local_data_storage_array_init_i8(&_##name##_data_storage_context);          \
    }                                                                               \
                                                                                    \
    inline static void name##_data_storage_array_add_value(i8 value) {              \
        local_data_storage_array_add_value_i8(                                      \
            &_##name##_data_storage_context,                                        \
            value                                                                   \
        );                                                                          \
    }                                                                               \
                                                                                    \
    inline static i8 name##_data_storage_array_get_mean(void) {                     \
        return local_data_storage_array_get_mean_i8(                                \
            &_##name##_data_storage_context                                         \
        );                                                                          \
    }                                                                               \
                                                                                    \
    inline static i8 name##_data_storage_array_get_max(void) {                      \
        return local_data_storage_array_get_max_i8(                                 \
            &_##name##_data_storage_context                                         \
        );                                                                          \
    }                                                                               \
                                                                                    \
    inline static i8 name##_data_storage_array_get_min(void) {                      \
        return local_data_storage_array_get_min_i8(                                 \
            &_##name##_data_storage_context                                         \
        );                                                                          \
    }

// --------------------------------------------------------------------------------

#define BUILD_LOCAL_DATA_STORAGE_ARRAY_U8(name, size)                                            \
                                                                    \
            static u8 _##name##_data_storage_buffer[size];                                    \
            static LOCAL_DATA_STORAGE_ARRAY_U8_TYPE _##name##_data_storage_context = {                    \
                _##name##_data_storage_buffer,                                        \
                size,                                                    \
                0,                                                    \
                0                                                    \
            };                                                        \
                                                                    \
            inline static void name##_data_storage_array_init(void) {                            \
                local_data_storage_array_init_u8(&_##name##_data_storage_context);                    \
            }                                                        \
                                                                    \
            inline static void name##_data_storage_array_add_value(u8 value) {                        \
                local_data_storage_array_add_value_u8(&_##name##_data_storage_context, value);                \
            }                                                        \
                                                                    \
            inline static u8 name##_data_storage_array_get_mean(void) {                            \
                return local_data_storage_array_get_mean_u8(&_##name##_data_storage_context);                \
            }                                                        \
                                                                    \
            inline static u8 name##_data_storage_array_get_max(void) {                            \
                return local_data_storage_array_get_max_u8(&_##name##_data_storage_context);                \
            }                                                        \
                                                                    \
            inline static u8 name##_data_storage_array_get_min(void) {                            \
                return local_data_storage_array_get_min_u8(&_##name##_data_storage_context);                \
            }                                                        \
                                                                    \
            inline static void name##_debug_print_data(void) {                                \
                local_data_storage_array_debug_print_data_u8(&_##name##_data_storage_context);                \
            }

// --------------------------------------------------------------------------------

#define BUILD_LOCAL_DATA_STORAGE_ARRAY_U16(name, size)                                            \
                                                                    \
            static u16 _##name##_data_storage_buffer[size];                                    \
            static LOCAL_DATA_STORAGE_ARRAY_U16_TYPE _##name##_data_storage_context = {                    \
                _##name##_data_storage_buffer,                                        \
                size,                                                    \
                0,                                                    \
                0                                                    \
            };                                                        \
                                                                    \
            inline static void name##_data_storage_array_init(void) {                            \
                local_data_storage_array_init_u16(&_##name##_data_storage_context);                    \
            }                                                        \
                                                                    \
            inline static void name##_data_storage_array_add_value(u16 value) {                        \
                local_data_storage_array_add_value_u16(&_##name##_data_storage_context, value);                \
            }                                                        \
                                                                    \
            inline static u16 name##_data_storage_array_get_mean(void) {                            \
                return local_data_storage_array_get_mean_u16(&_##name##_data_storage_context);                \
            }                                                        \
                                                                    \
            inline static u16 name##_data_storage_array_get_max(void) {                            \
                return local_data_storage_array_get_max_u16(&_##name##_data_storage_context);                \
            }

// --------------------------------------------------------------------------------

#define BUILD_LOCAL_DATA_STORAGE_ARRAY_U32(name, size)                                            \
                                                                    \
            static u8 _##name##_data_storage_buffer[size];                                    \
            static LOCAL_DATA_STORAGE_ARRAY_U32_TYPE _##name##_data_storage_context = {                    \
                _##name##_data_storage_buffer,                                        \
                size,                                                    \
                0,                                                    \
                0                                                    \
            };                                                        \
                                                                    \
            inline static void name##_data_storage_array_init(void) {                            \
                local_data_storage_array_init_u32(&_##name##_data_storage_context);                    \
            }                                                        \
                                                                    \
            inline static void name##_data_storage_array_add_value(u32 value) {                        \
                local_data_storage_array_add_value_u32(&_##name##_data_storage_context, value);                \
            }                                                        \
                                                                    \
            inline static u32 name##_data_storage_array_get_mean(void) {                            \
                return local_data_storage_array_get_mean_u32(&_##name##_data_storage_context);                \
            }                                                        \
                                                                    \
            inline static u32 name##_data_storage_array_get_max(void) {                            \
                return local_data_storage_array_get_max_u32(&_##name##_data_storage_context);                \
            }


// --------------------------------------------------------------------------------

#endif // _H_local_data_storage_array_

// --------------------------------------------------------------------------------

