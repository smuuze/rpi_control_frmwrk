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
 * @file    shared_memory.h
 * @author  Sebastian Lesse
 * @date    2024 / 02 / 16
 * @brief   Interface to provide to manage shared memory for temporarily
 *          handling application data.
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_shared_memory_
#define _H_shared_memory_

// --------------------------------------------------------------------------------

#include "cpu.h"

// --------------------------------------------------------------------------------

/**
 * @brief Indicates an invalid handle.
 * The shared memory context must not be used for data operation.
 */
#define SHARED_MEMORY_INVALID_HANDLE                                        0xFFFF

// --------------------------------------------------------------------------------

#define SHARED_MEMORY_NO_MORE_MEMORY_AVAILABLE  0
#define SHARED_MEMORY_MEMORY_ALLOCATED          1
#define SHARED_MEMORY_MEMORY_ALREADY_ALLOCATED  2

// --------------------------------------------------------------------------------

/**
 * @brief Creates a new shared memory object.
 * The created memory object is empty and needs to be allocated before it can be used.
 * 
 * Shared memory objects are supposed to be used only in one sw-module only.
 * Shared-Memory objects are not intended to transfer date from one module to another.
 * 
 * @param name The name of the new shared memory object
 * @param size The size that this objects needs to have after allocation.
 * 
 * Usage:   1. Create Object
 * 
 *              SHARED_MEMORY_CREATE(MY_SAHRED_MEM_OBJ, 1024)
 * 
 *          2. Allocate memory
 * 
 *              if (MY_SAHRED_MEM_OBJ_allocate() != SHARED_MEMORY_NO_MORE_MEMORY_AVAILABLE) {
 *                  ...
 *              }
 * 
 *          3. Use object
 * 
 *              MY_SAHRED_MEM_OBJ_reset();
 *              ...
 *              if (MY_SAHRED_MEM_OBJ_add_u32(my_u32_value)) {
 *                  ...
 *                  u32 my_stored_u32_value = MY_SAHRED_MEM_OBJ_get_u32();
 *                  ...
 *                  MY_SAHRED_MEM_OBJ_rseek(-sizeof(my_stored_u32_value));
 *                  u32 my_restored_u32_value = MY_SAHRED_MEM_OBJ_get_u32();
 *              }
 * 
 *          4. Deallocate object to let other instances use the memory
 * 
 *              MY_SAHRED_MEM_OBJ_free();
 * 
 */
#define SHARED_MEMORY_CREATE(name, size)                                            \
                                                                                    \
    static SHARED_MEMORY_CONTEXT __##name##_shared_mem_ctx = {                      \
        SHARED_MEMORY_INVALID_HANDLE /* handle */                                   \
    };                                                                              \
                                                                                    \
    static inline u8 __UNUSED__ name##_allocate(void) {                             \
        return shared_memory_allocate(&__##name##_shared_mem_ctx, size);            \
    }                                                                               \
                                                                                    \
    static inline void __UNUSED__ name##_free(void) {                               \
        shared_memory_free(&__##name##_shared_mem_ctx);                             \
    }                                                                               \
                                                                                    \
    static inline void __UNUSED__ name##_reset(void) {                              \
        shared_memory_reset(&__##name##_shared_mem_ctx);                            \
    }                                                                               \
                                                                                    \
    static inline u16 __UNUSED__ name##_available(void) {                           \
        return shared_memory_available(&__##name##_shared_mem_ctx);                 \
    }                                                                               \
                                                                                    \
    static inline u8 __UNUSED__ name##_add_data(u8* p_data_src, u16 len) {          \
        return shared_memory_add_data(&__##name##_shared_mem_ctx, p_data_src, len); \
    }                                                                               \
                                                                                    \
    static inline u16 __UNUSED__ name##_get_data(u8* p_dest, u16 len) {             \
        return shared_memory_get_data(&__##name##_shared_mem_ctx, p_dest, len);     \
    }                                                                               \
                                                                                    \
    static inline u8 __UNUSED__ name##_add_u8(u8 value) {                           \
        return shared_memory_add_u8(&__##name##_shared_mem_ctx, value);             \
    }                                                                               \
                                                                                    \
    static inline u8 __UNUSED__ name##_get_u8(void) {                               \
        return shared_memory_get_u8(&__##name##_shared_mem_ctx);                    \
    }                                                                               \
                                                                                    \
    static inline u8 __UNUSED__ name##_add_u16(u16 value) {                         \
        return shared_memory_add_u16(&__##name##_shared_mem_ctx, value);            \
    }                                                                               \
                                                                                    \
    static inline u16 __UNUSED__ name##_get_u16(void) {                             \
        return shared_memory_get_u16(&__##name##_shared_mem_ctx);                   \
    }                                                                               \
                                                                                    \
    static inline u8 __UNUSED__ name##_add_u32(u32 value) {                         \
        return shared_memory_add_u32(&__##name##_shared_mem_ctx, value);            \
    }                                                                               \
                                                                                    \
    static inline u32 __UNUSED__ name##_get_u32(void) {                             \
        return shared_memory_get_u32(&__##name##_shared_mem_ctx);                   \
    }                                                                               \
                                                                                    \
    static inline u32 __UNUSED__ name##_is_allocated(void) {                        \
        return __##name##_shared_mem_ctx.handle != SHARED_MEMORY_INVALID_HANDLE;    \
    }                                                                               \
                                                                                    \
    static inline u32 __UNUSED__ name##_occupied(void) {                            \
        return shared_memory_occupied(&__##name##_shared_mem_ctx);                  \
    }                                                                               \
                                                                                    \
    static inline void __UNUSED__ name##_rseek(i16 offset) {                        \
        shared_memory_rseek(&__##name##_shared_mem_ctx, offset);                    \
    }                                                                               \
                                                                                    \
    static inline u16 __UNUSED__ name##_copy_to(                                    \
        u16 offset,                                                                 \
        u8* p_destination,                                                          \
        u16 num_bytes                                                               \
    ) {                                                                             \
        return shared_memory_copy_to(                                               \
            &__##name##_shared_mem_ctx,                                             \
            offset,                                                                 \
            p_destination,                                                          \
            num_bytes                                                               \
        );                                                                          \
    }

// --------------------------------------------------------------------------------

/**
 * @brief 
 */
typedef struct {

    u16 handle;

} SHARED_MEMORY_CONTEXT;

// --------------------------------------------------------------------------------

/**
 * @brief Tries to allocate the requested amount of data.
 * The data allocated can be higher that requested.
 * It might happen that there is no more memory available.
 * In this case, the caller can retry to allocate memory after a short while.
 * 
 * @param p_ctx The context to use
 * @param size 
 * @return  SHARED_MEMORY_NO_MORE_MEMORY_AVAILABLE - no more memory available, try again later
 *          SHARED_MEMORY_MEMORY_ALLOCATED         - the memory was allocated successful
 *          SHARED_MEMORY_MEMORY_ALREADY_ALLOCATED - Memory already allocated
 */
u8 shared_memory_allocate(SHARED_MEMORY_CONTEXT* p_ctx, u16 size);

/**
 * @brief Frees the occupied memory. After calling this function
 * the shared memory context can not be used until new memory is requested.
 * 
 * @param p_ctx valid and allocated context to be used.
 */
void shared_memory_free(SHARED_MEMORY_CONTEXT* p_ctx);

/**
 * @brief 
 * 
 * @param p_ctx valid and allocated context to be used.
 */
void shared_memory_reset(SHARED_MEMORY_CONTEXT* p_ctx);

/**
 * @brief Checks if the shared memory context is valid.
 * 
 * @param p_ctx valid and allocated context to be used.
 * @return the number of bytes that are still free
 *         0 if there is no more data or the context has not been allocated.
 */
u16 shared_memory_available(SHARED_MEMORY_CONTEXT* p_ctx);

/**
 * @brief 
 * 
 * @param p_ctx valid and allocated context to be used.
 * @param p_data_src 
 * @param len 
 * @return u8 
 */
u8 shared_memory_add_data(SHARED_MEMORY_CONTEXT* p_ctx, const u8* p_data_src, u16 len);

/**
 * @brief 
 * 
 * @param p_ctx valid and allocated context to be used.
 * @param p_data_dest 
 * @param len 
 * @return The number of bytes that have been copied to p_data_dest
 */
u16 shared_memory_get_data(SHARED_MEMORY_CONTEXT* p_ctx, u8* p_data_dest, u16 len);

/**
 * @brief 
 * 
 * @param p_ctx valid and allocated context to be used.
 * @param value 
 * @return u8 
 */
u8 shared_memory_add_u8(SHARED_MEMORY_CONTEXT* p_ctx, const u8 value);

/**
 * @brief 
 * 
 * @param p_ctx valid and allocated context to be used.
 * @param value 
 * @return u8 
 */
u8 shared_memory_get_u8(SHARED_MEMORY_CONTEXT* p_ctx);

/**
 * @brief 
 * 
 * @param p_ctx valid and allocated context to be used.
 * @param value 
 * @return u8 
 */
u8 shared_memory_add_u16(SHARED_MEMORY_CONTEXT* p_ctx, const u16 value);

/**
 * @brief 
 * 
 * @param p_ctx valid and allocated context to be used.
 * @param value 
 * @return u16
 */
u16 shared_memory_get_u16(SHARED_MEMORY_CONTEXT* p_ctx);

/**
 * @brief 
 * 
 * @param p_ctx valid and allocated context to be used.
 * @param value 
 * @return u8
 */
u8 shared_memory_add_u32(SHARED_MEMORY_CONTEXT* p_ctx, const u32 value);

/**
 * @brief 
 * 
 * @param p_ctx valid and allocated context to be used.
 * @param value 
 * @return u32
 */
u32 shared_memory_get_u32(SHARED_MEMORY_CONTEXT* p_ctx);

/**
 * @brief Get a reference to the first byte of the allocated memory
 * of the given shared memory object.
 * 
 * @param p_ctx valid and allocated context to be used.
 * @return  Pointer to the first byte of the allocated memory,
 *          or NULL if there is no memory allocated in the given object
 */
const u8* shared_memory_get(SHARED_MEMORY_CONTEXT* p_ctx);

/**
 * @brief Gives the number of bytes that have been used.
 * 
 * @param p_ctx valid and allocated context to be used.
 * @return number of bytes that are used to store data
 */
u16 shared_memory_occupied(SHARED_MEMORY_CONTEXT* p_ctx);

/**
 * @brief Moves the current read pointer by the given amount of bytes.
 * The read pointer can be moved forward (offset is positive)
 * and backwards (offset is negative). The number of available bytes is updated.
 * Moving is only possible to the position of the write pointer.
 * If the read pointer was moved forwards to the current position of the write pointer
 * the number of avaialble bytes is zero. If the read pointer was moved back
 * to the position write-pointer+1 the number of available bytes is at its maximum.
 * In this case it is not possible to add more bytes.
 * 
 * @param p_ctx valid and allocated context to be used.
 * @param offset number of bytes to move the read-pointer.
 */
void shared_memory_rseek(SHARED_MEMORY_CONTEXT* p_ctx, i16 offset);

/**
 * @brief Copies the content of thegiven shared memory object ot the given memory destination.
 * The data is always copied from the given offset. THis function tries always to copy the requested number of bytes.
 * Even if the number of occupied bytes is lesser than num_bytes. If the size of the given shared memory object
 * is less than the requested number of bytes only the available bytes are copied.
 * 
 * @param p_ctx valid and allocated shared memory context
 * @param offset positve number of bytes from where to start copy the data, range: 0 <= offset <= memory-size 
 * @param p_destination destination address where to copy the data, shall be big enough to hold the requested amount of bytes.
 * @param num_bytes positive number of bytes to copy from the given shared memory object.
 */
u16 shared_memory_copy_to(SHARED_MEMORY_CONTEXT* p_ctx, u16 offset, u8* p_destination, u16 num_bytes);

// --------------------------------------------------------------------------------

/**
 * @brief 
 */
void shared_memory_initialize(void);

// --------------------------------------------------------------------------------

#endif // _H_shared_memory_

// --------------------------------------------------------------------------------
