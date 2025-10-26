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
 * @file    shared_memory.c
 * @author  Sebastian Lesse
 * @date    2024 / 02 / 21
 * @brief   Implementation of the shared-memory interface defined in
 *          shared_memory.h
 * 
 *          Basic principle:
 * 
 *          - there is only ne large array where all the memory slots all locaeted in.
 *          - the number of memory blocks for each size (64, 128, 256, ... ) is
 *            multplied with the number of bytes of the corresponding block.
 *          - the sum of these blocks form the array that thtat is used to hold the data
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

#include "shared_memory.h"
#include "math_module.h"

// --------------------------------------------------------------------------------

#define SHARED_MEMORY_SIZE_64_BYTE              64
#define SHARED_MEMORY_SIZE_128_BYTE             128
#define SHARED_MEMORY_SIZE_256_BYTE             256
#define SHARED_MEMORY_SIZE_512_BYTE             512
#define SHARED_MEMORY_SIZE_1024_BYTE            1024
#define SHARED_MEMORY_SIZE_2048_BYTE            2048
#define SHARED_MEMORY_SIZE_4096_BYTE            4096

// --------------------------------------------------------------------------------

#ifndef SHARED_MEMORY_COUNT_64_BYTES
#define SHARED_MEMORY_COUNT_64_BYTES          0
#endif // SHARED_MEMORY_COUNT_64_BYTES

#ifndef SHARED_MEMORY_COUNT_128_BYTES
#define SHARED_MEMORY_COUNT_128_BYTES         4
#endif // SHARED_MEMORY_COUNT_128_BYTES

#ifndef SHARED_MEMORY_COUNT_256_BYTES
#define SHARED_MEMORY_COUNT_256_BYTES         0
#endif // SHARED_MEMORY_COUNT_256_BYTES

#ifndef SHARED_MEMORY_COUNT_512_BYTES
#define SHARED_MEMORY_COUNT_512_BYTES         0
#endif // SHARED_MEMORY_COUNT_512_BYTES

#ifndef SHARED_MEMORY_COUNT_1024_BYTES
#define SHARED_MEMORY_COUNT_1024_BYTES        0
#endif // SHARED_MEMORY_COUNT_1024_BYTES

#ifndef SHARED_MEMORY_COUNT_2048_BYTES
#define SHARED_MEMORY_COUNT_2048_BYTES        0
#endif // SHARED_MEMORY_COUNT_2048_BYTES

#ifndef SHARED_MEMORY_COUNT_4096_BYTES
#define SHARED_MEMORY_COUNT_4096_BYTES        0
#endif // SHARED_MEMORY_COUNT_4096_BYTES

#define SHARED_MEMORY_COUNT_SLOTS               SHARED_MEMORY_COUNT_64_BYTES      \
                                                + SHARED_MEMORY_COUNT_128_BYTES   \
                                                + SHARED_MEMORY_COUNT_256_BYTES   \
                                                + SHARED_MEMORY_COUNT_512_BYTES   \
                                                + SHARED_MEMORY_COUNT_1024_BYTES  \
                                                + SHARED_MEMORY_COUNT_2048_BYTES  \
                                                + SHARED_MEMORY_COUNT_4096_BYTES

/**
 * @brief The following definitions are used as an offset to the
 * memory blocks inside of the memory slot array.
 */

#define SHARED_MEMROY_SLOT_OFFSET_64            0
#define SHARED_MEMROY_SLOT_OFFSET_128           (SHARED_MEMORY_COUNT_64_BYTES)
#define SHARED_MEMROY_SLOT_OFFSET_256           (SHARED_MEMROY_SLOT_OFFSET_128  + SHARED_MEMORY_COUNT_128_BYTES)
#define SHARED_MEMROY_SLOT_OFFSET_512           (SHARED_MEMROY_SLOT_OFFSET_256  + SHARED_MEMORY_COUNT_256_BYTES)
#define SHARED_MEMROY_SLOT_OFFSET_1024          (SHARED_MEMROY_SLOT_OFFSET_512  + SHARED_MEMORY_COUNT_512_BYTES)
#define SHARED_MEMROY_SLOT_OFFSET_2048          (SHARED_MEMROY_SLOT_OFFSET_1024 + SHARED_MEMORY_COUNT_1024_BYTES)
#define SHARED_MEMROY_SLOT_OFFSET_4096          (SHARED_MEMROY_SLOT_OFFSET_2048 + SHARED_MEMORY_COUNT_2048_BYTES)

/**
 * @brief The following definitions are used as an offset
 * to the first block of the available memory sizes
 */

#define SHARED_MEMORY_ADDR_OFFSET_64            0
#define SHARED_MEMORY_ADDR_OFFSET_128           ((SHARED_MEMORY_COUNT_64_BYTES * SHARED_MEMORY_SIZE_64_BYTE)     \
                                                + SHARED_MEMORY_ADDR_OFFSET_64)
#define SHARED_MEMORY_ADDR_OFFSET_256           ((SHARED_MEMORY_COUNT_128_BYTES * SHARED_MEMORY_SIZE_128_BYTE)  \
                                                + SHARED_MEMORY_ADDR_OFFSET_128)
#define SHARED_MEMORY_ADDR_OFFSET_512           ((SHARED_MEMORY_COUNT_256_BYTES * SHARED_MEMORY_SIZE_256_BYTE)  \
                                                + SHARED_MEMORY_ADDR_OFFSET_256)
#define SHARED_MEMORY_ADDR_OFFSET_1024          ((SHARED_MEMORY_COUNT_512_BYTES * SHARED_MEMORY_SIZE_512_BYTE) \
                                                + SHARED_MEMORY_ADDR_OFFSET_512)
#define SHARED_MEMORY_ADDR_OFFSET_2048          ((SHARED_MEMORY_COUNT_1024_BYTES * SHARED_MEMORY_SIZE_1024_BYTE) \
                                                + SHARED_MEMORY_ADDR_OFFSET_1024)
#define SHARED_MEMORY_ADDR_OFFSET_4096          ((SHARED_MEMORY_COUNT_2048_BYTES * SHARED_MEMORY_SIZE_2048_BYTE) \
                                                + SHARED_MEMORY_ADDR_OFFSET_2048)

// --------------------------------------------------------------------------------

#define SHARED_MEMORY_AVAILABLE_MEMORY          ((SHARED_MEMORY_COUNT_4096_BYTES * SHARED_MEMORY_SIZE_4096_BYTE)     \
                                                + SHARED_MEMORY_ADDR_OFFSET_4096)

// --------------------------------------------------------------------------------

#define SHARED_MEMORY_SLOT_UNUSED               0
#define SHARED_MEMORY_SLOT_OCCUPIED             1

// --------------------------------------------------------------------------------

/**
 * @brief 
 */
typedef struct {

    u8 occupied;

    /**
     * @brief The maximum number of bytes of the current memory-block
     * referenced by p_block. If p_block is set to NULL-POINTER this value
     * is set to 0.
     */
    u16 max_length;
    u16 write_pointer;
    u16 read_pointer;

    /**
     * @brief Number of bytes that are free for storing data.
     */
    u16 bytes_available;
    
    u16 offset;

} SHARED_MEMORY_CONTAINER;

// --------------------------------------------------------------------------------

/**
 * @brief The whole available shared memory
 */
static u8 shared_memory_block[SHARED_MEMORY_AVAILABLE_MEMORY];

/**
 * @brief This array is used to manage the available shared memory slots.
 * A slot is set to SHARED_MEMORY_SLOT_UNUSED if it is not used.
 * It will be set to tha handle of the shared-memory-object
 * that is using it in case of allocation.
 */
static SHARED_MEMORY_CONTAINER shared_memory_container[SHARED_MEMORY_COUNT_SLOTS];

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param p_ctx Pointer to the context where the allocated block is used in
 * @param start_idx first index in shared_memory_slots where to lock for an empty memory slot.
 * @return  SHARED_MEMORY_NO_MORE_MEMORY_AVAILABLE - no more memory available, try again later
 *          SHARED_MEMORY_MEMORY_ALLOCATED         - the memory was allocated successful
 *          SHARED_MEMORY_MEMORY_ALREADY_ALLOCATED - Memory already allocated
 */
static u8 shared_memory_allocate_block(
    SHARED_MEMORY_CONTEXT* p_ctx,
    u8 start_idx
) {

    if (p_ctx->handle != SHARED_MEMORY_INVALID_HANDLE) {
        DEBUG_TRACE_word(p_ctx->handle, "shared_memory_allocate_block() - Already allocated - Handle:");
        return SHARED_MEMORY_MEMORY_ALREADY_ALLOCATED;
    }

    for (u8 index = start_idx; index < SHARED_MEMORY_COUNT_SLOTS; index += 1) {
        if (shared_memory_container[index].occupied == SHARED_MEMORY_SLOT_UNUSED) {
            
            p_ctx->handle = index;
            shared_memory_container[index].occupied = SHARED_MEMORY_SLOT_OCCUPIED;

            shared_memory_container[index].write_pointer = 0;
            shared_memory_container[index].read_pointer = 0;
            shared_memory_container[index].bytes_available = shared_memory_container[index].max_length;

            DEBUG_PASS("shared_memory_allocate_block() - Memory Block allocated");
            DEBUG_TRACE_byte(index, "- Slot-Index:");
            DEBUG_TRACE_word(shared_memory_container[index].max_length, "- Size:");
            DEBUG_TRACE_word(shared_memory_container[index].bytes_available, "- Available:");
            DEBUG_TRACE_long(shared_memory_container[index].offset, "- Memory-Offset:");

            return SHARED_MEMORY_MEMORY_ALLOCATED;
        }
    }

    DEBUG_PASS("shared_memory_allocate_block() - No memory block found");

    return SHARED_MEMORY_NO_MORE_MEMORY_AVAILABLE;
}

// --------------------------------------------------------------------------------

/**
 * @brief Checks if the given context can take the requested amount of data bytes
 * 
 * @param p_container shared-memory container to check for
 * @param len number of bytes that shall be added
 * @return  0 not enough data available
 *          1 the number of bytes can be added
 */
static inline u8 shared_memory_check_length(SHARED_MEMORY_CONTAINER* p_container, u16 len) {
    if (p_container->bytes_available < len) {
        DEBUG_PASS("shared_memory_check_length() - Not enough data");
        DEBUG_TRACE_word(len, " - LEN:");
        DEBUG_TRACE_word(p_container->bytes_available, " - AVAILABLE:");
        DEBUG_TRACE_word(p_container->max_length, " - MAX-LEN:");
        return 0;
    }
    return 1;
}

// --------------------------------------------------------------------------------

/**
 * @see shared_memory.h
 */
u8 shared_memory_allocate(SHARED_MEMORY_CONTEXT* p_ctx, u16 size) {

    DEBUG_TRACE_word(size, "shared_memory_allocate() - trying to allocate memory - size:");

    #if SHARED_MEMORY_COUNT_64_BYTES != 0
    {
        if (size <= SHARED_MEMORY_SIZE_64_BYTE) {
            return shared_memory_allocate_block (
                p_ctx,
                SHARED_MEMROY_SLOT_OFFSET_64
            );
        }
    }
    #endif // SHARED_MEMORY_COUNT_64_BYTES

    #if SHARED_MEMORY_COUNT_128_BYTES != 0
    {
        if (size <= SHARED_MEMORY_SIZE_128_BYTE) {
            return shared_memory_allocate_block (
                p_ctx,
                SHARED_MEMROY_SLOT_OFFSET_128
            );
        }
    }
    #endif // SHARED_MEMORY_COUNT_128_BYTES

    #if SHARED_MEMORY_COUNT_256_BYTES != 0
    {
        if (size <= SHARED_MEMORY_SIZE_256_BYTE) {
            return shared_memory_allocate_block (
                p_ctx,
                SHARED_MEMROY_SLOT_OFFSET_256
            );
        }
    }
    #endif // SHARED_MEMORY_COUNT_256_BYTES

    #if SHARED_MEMORY_COUNT_512_BYTES != 0
    {
        if (size <= SHARED_MEMORY_SIZE_512_BYTE) {
            return shared_memory_allocate_block (
                p_ctx,
                SHARED_MEMROY_SLOT_OFFSET_512
            );
        }
    }
    #endif // SHARED_MEMORY_COUNT_512_BYTES

    #if SHARED_MEMORY_COUNT_1024_BYTES != 0
    {
        if (size <= SHARED_MEMORY_SIZE_1024_BYTE) {
            return shared_memory_allocate_block (
                p_ctx,
                SHARED_MEMROY_SLOT_OFFSET_1024
            );
        }
    }
    #endif // SHARED_MEMORY_COUNT_1024_BYTES

    #if SHARED_MEMORY_COUNT_2048_BYTES != 0
    {
        if (size <= SHARED_MEMORY_SIZE_2048_BYTE) {
            return shared_memory_allocate_block (
                p_ctx,
                SHARED_MEMROY_SLOT_OFFSET_2048
            );
        }
    }
    #endif // SHARED_MEMORY_COUNT_2048_BYTES

    #if SHARED_MEMORY_COUNT_4096_BYTES != 0
    {
        if (size <= SHARED_MEMORY_SIZE_4096_BYTE) {
            return shared_memory_allocate_block (
                p_ctx,
                SHARED_MEMROY_SLOT_OFFSET_4096
            );
        }
    }
    #endif // SHARED_MEMORY_COUNT_4096_BYTES
    
    DEBUG_TRACE_word(size, "shared_memory_allocate() - block-size too big");
    return 0;
}

/**
 * @see shared_memory.h
 */
void shared_memory_free(SHARED_MEMORY_CONTEXT* p_ctx) {

    if (p_ctx->handle == SHARED_MEMORY_INVALID_HANDLE) {
        return;
    }

    if (p_ctx->handle >= SHARED_MEMORY_COUNT_SLOTS) {
        return;
    }

    SHARED_MEMORY_CONTAINER* p_container = &shared_memory_container[p_ctx->handle];
    if (p_container->occupied == SHARED_MEMORY_SLOT_OCCUPIED) {
        DEBUG_TRACE_word(p_ctx->handle, "shared_memory_free() - Handle:");
        p_container->occupied = SHARED_MEMORY_SLOT_UNUSED;
        p_ctx->handle = SHARED_MEMORY_INVALID_HANDLE;
    }
}

/**
 * @see shared_memory.h
 */
void shared_memory_reset(SHARED_MEMORY_CONTEXT* p_ctx) {

    if (p_ctx->handle == SHARED_MEMORY_INVALID_HANDLE) {
        return;
    }

    if (p_ctx->handle >= SHARED_MEMORY_COUNT_SLOTS) {
        return;
    }
    
    SHARED_MEMORY_CONTAINER* p_container = &shared_memory_container[p_ctx->handle];
    p_container->write_pointer = 0;
    p_container->read_pointer = 0;
    p_container->bytes_available = p_container->max_length;

    u8* p_data = &shared_memory_block[p_container->offset];
    memset(p_data, 0x00, p_container->max_length);
}

/**
 * @see shared_memory.h
 */
u16 shared_memory_available(SHARED_MEMORY_CONTEXT* p_ctx) {

    if (p_ctx->handle == SHARED_MEMORY_INVALID_HANDLE) {
        return 0;
    }

    if (p_ctx->handle >= SHARED_MEMORY_COUNT_SLOTS) {
        return 0;
    }

    SHARED_MEMORY_CONTAINER* p_container = &shared_memory_container[p_ctx->handle];
    return p_container->bytes_available;
}

/**
 * @see shared_memory.h
 */
u8 shared_memory_add_data(SHARED_MEMORY_CONTEXT* p_ctx, const u8* p_data_src, u16 len) {

    if (p_ctx->handle == SHARED_MEMORY_INVALID_HANDLE) {
        DEBUG_PASS("shared_memory_add_data() - Failed to add data - invalid Handle");
        return 0;
    }

    if (p_ctx->handle >= SHARED_MEMORY_COUNT_SLOTS) {
        return 0;
    }

    SHARED_MEMORY_CONTAINER* p_container = &shared_memory_container[p_ctx->handle];
    if (shared_memory_check_length(p_container, len) == 0) {
        DEBUG_TRACE_word(p_ctx->handle, "shared_memory_add_data() - Failed to add data - Handle:");
        DEBUG_TRACE_word(shared_memory_container[p_ctx->handle].bytes_available, "- Bytes Available");
        return 0;
    }

    // DEBUG_TRACE_word(p_ctx->handle, "shared_memory_add_data() - Offset");
    // DEBUG_TRACE_word(p_container->write_pointer, "shared_memory_add_data() - Pointer");

    /**
     * @brief The shared memory is used as a ring-memory.
     * If we are at the end of the memory we need to continue from the start,
     * if there are enough bytes available.
     * So we perform two mem-copy operations in that case.
     */

    // first mem-copy
    u16 num_bytes = p_container->max_length - p_container->write_pointer;
    if (num_bytes > len) {
        // in this case there are enough
        // bytes available in one block
        num_bytes = len;  
    }

    u8* p_dest = &shared_memory_block[p_container->offset + p_container->write_pointer];
    memcpy(p_dest, p_data_src, num_bytes);
    
    p_container->write_pointer += num_bytes;
    p_container->bytes_available -= num_bytes;

    if (p_container->write_pointer == p_container->max_length) {
        p_container->write_pointer = 0;
    }

    // DEBUG_TRACE_word(num_bytes, "shared_memory_add_data() - first write");
    // DEBUG_TRACE_word(p_container->write_pointer, " - Write-Pointer");

    if (len == num_bytes) {
        // no more data that needs to be copied
        // we are done here
        return 1;
    }

    // second mem-copy

    num_bytes = len - num_bytes;
    p_container->write_pointer = 0;

    p_dest = &shared_memory_block[p_container->offset + p_container->write_pointer];
    const u8* p_src  = &p_data_src[num_bytes];
    memcpy(p_dest, p_src, num_bytes);

    p_container->write_pointer += num_bytes;
    p_container->bytes_available -= num_bytes;

    // DEBUG_TRACE_word(num_bytes, "shared_memory_add_data() - second write");
    // DEBUG_TRACE_word(p_container->write_pointer, " - Write-Pointer");

    return 1;
}

/**
 * @see shared_memory.h
 */
u16 shared_memory_get_data(SHARED_MEMORY_CONTEXT* p_ctx, u8* p_data_dest, u16 len) {

    if (p_ctx->handle == SHARED_MEMORY_INVALID_HANDLE) {
        return 0;
    }

    if (p_ctx->handle >= SHARED_MEMORY_COUNT_SLOTS) {
        return 0;
    }

    SHARED_MEMORY_CONTAINER* p_container = &shared_memory_container[p_ctx->handle];

    // check if we need to return the read pointer to the start-position
    // do first memcpy
    // do second memcpy, if needed

    // check if we have enough bytes stored
    u16 num_data_bytes = p_container->max_length - p_container->bytes_available;
    if (num_data_bytes < len) {
        len = num_data_bytes;
    }

    // first mem-copy
    u16 num_bytes = p_container->max_length - p_container->read_pointer;
    if (num_bytes > len) {
        // in this case we can copy all requested
        // bytes within one memcpy operation
        num_bytes = len;  
    }

    // DEBUG_TRACE_word(p_container->read_pointer, "shared_memory_get_data() - Read-Pointer");

    u8* p_src = &shared_memory_block[p_container->offset + p_container->read_pointer];
    u8* p_dest = p_data_dest;
    memcpy(p_dest, p_src, num_bytes);
    
    p_container->read_pointer += num_bytes;
    p_container->bytes_available += num_bytes;

    if (p_container->read_pointer == p_container->max_length) {
        p_container->read_pointer = 0;
    }

    // DEBUG_TRACE_word(num_bytes, "shared_memory_get_data() - first read");

    if (len == num_bytes) {
        // no more data that needs to be copied
        // we are done here
        return len;
    }

    p_src = &shared_memory_block[p_container->offset];
    p_dest = &p_data_dest[num_bytes];

    num_bytes = len - num_bytes;
    p_container->read_pointer = 0;

    memcpy(p_dest, p_src, num_bytes);
    
    p_container->read_pointer += num_bytes;
    p_container->bytes_available += num_bytes;

    // DEBUG_TRACE_word(num_bytes, "shared_memory_get_data() - second read");
    // DEBUG_TRACE_word(p_container->read_pointer, " - Read-Pointer");

    return len;
}

/**
 * @see shared_memory.h
 */
u8 shared_memory_add_u8(SHARED_MEMORY_CONTEXT* p_ctx, const u8 value) {
    return shared_memory_add_data(p_ctx, &value, sizeof(u8));
}

/**
 * @see shared_memory.h
 */
u8 shared_memory_get_u8(SHARED_MEMORY_CONTEXT* p_ctx) {
    u8 value = 0;
    shared_memory_get_data(p_ctx, (u8*)(&value), sizeof(value));
    return value;
}

/**
 * @see shared_memory.h
 */
u8 shared_memory_add_u16(SHARED_MEMORY_CONTEXT* p_ctx, const u16 value) {
    return shared_memory_add_data(p_ctx, (u8*)(&value), sizeof(u16));
}

/**
 * @see shared_memory.h
 */
u16 shared_memory_get_u16(SHARED_MEMORY_CONTEXT* p_ctx) {
    u16 value = 0;
    shared_memory_get_data(p_ctx, (u8*)(&value), sizeof(value));
    return value;
}

/**
 * @see shared_memory.h
 */
u8 shared_memory_add_u32(SHARED_MEMORY_CONTEXT* p_ctx, const u32 value) {
    return shared_memory_add_data(p_ctx, (u8*)(&value), sizeof(u32));
}

/**
 * @see shared_memory.h
 */
u32 shared_memory_get_u32(SHARED_MEMORY_CONTEXT* p_ctx) {
    u32 value = 0;
    shared_memory_get_data(p_ctx, (u8*)(&value), sizeof(value));
    return value;
}

/**
 * @see shared_memory.h
 */
const u8* shared_memory_get(SHARED_MEMORY_CONTEXT* p_ctx) {

    if (p_ctx->handle == SHARED_MEMORY_INVALID_HANDLE) {
        return 0;
    }

    if (p_ctx->handle >= SHARED_MEMORY_COUNT_SLOTS) {
        return 0;
    }

    SHARED_MEMORY_CONTAINER* p_container = &shared_memory_container[p_ctx->handle];
    return &shared_memory_block[p_container->offset];
}

/**
 * @see shared_memory.h
 */
u16 shared_memory_occupied(SHARED_MEMORY_CONTEXT* p_ctx) {

    if (p_ctx->handle == SHARED_MEMORY_INVALID_HANDLE) {
        return 0;
    }

    if (p_ctx->handle >= SHARED_MEMORY_COUNT_SLOTS) {
        return 0;
    }

    SHARED_MEMORY_CONTAINER* p_container = &shared_memory_container[p_ctx->handle];
    return (p_container->max_length - p_container->bytes_available);
}

/**
 * @see shared_memory.h
 */
void shared_memory_rseek(SHARED_MEMORY_CONTEXT* p_ctx, i16 offset) {

    if (p_ctx->handle == SHARED_MEMORY_INVALID_HANDLE) {
        return;
    }

    if (p_ctx->handle >= SHARED_MEMORY_COUNT_SLOTS) {
        return;
    }

    SHARED_MEMORY_CONTAINER* p_container = &shared_memory_container[p_ctx->handle];

    if (offset > p_container->max_length) {
        // cannot move beyond memory limit
        return;
    }

    DEBUG_CODE_BLOCK (
        if (offset < 0) {
            u16 offset_tmp = (u16)(-offset);
            DEBUG_TRACE_word(offset_tmp, "shared_memory_rseek() - offset (neg):");
        } else {
            DEBUG_TRACE_word(offset, "shared_memory_rseek() - offset (pos):");
        }
    )

    DEBUG_TRACE_word(p_container->write_pointer, "shared_memory_rseek() - write-pointer:");

    DEBUG_TRACE_word(p_container->bytes_available, "shared_memory_rseek() - available before:");
    DEBUG_TRACE_word(p_container->read_pointer, "shared_memory_rseek() - read-pointer before:");

    p_container->read_pointer = (p_container->read_pointer + (u16)(offset)) % p_container->max_length;
    p_container->bytes_available -= math_get_abs(offset);

    DEBUG_TRACE_word(p_container->bytes_available, "shared_memory_rseek() - available after:");
    DEBUG_TRACE_word(p_container->read_pointer, "shared_memory_rseek() - read-pointer after:");

    if (offset < 0) {

        // moving backwards

    } else {

        // moving forward

    }
}

/**
 * @see shared_memory.h
 */
u16 shared_memory_copy_to(SHARED_MEMORY_CONTEXT* p_ctx, u16 offset, u8* p_destination, u16 num_bytes) {

    if (p_ctx->handle == SHARED_MEMORY_INVALID_HANDLE) {
        return 0;
    }

    if (p_ctx->handle >= SHARED_MEMORY_COUNT_SLOTS) {
        return 0;
    }

    SHARED_MEMORY_CONTAINER* p_container = &shared_memory_container[p_ctx->handle];

    if (offset > p_container->max_length) {
        return 0;
    }

    u16 length = num_bytes > p_container->max_length ? p_container->max_length : num_bytes;

    u8* p_src = &shared_memory_block[p_container->offset + offset];
    memcpy(p_destination, p_src, length);

    return length;
}

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param block_count Number of blocks to initialize with the given parameters
 * @param block_size number of bytes the blocks have
 * @param offset memory offset to start from for the current blocks
 */
static void shared_memory_init_container(
    u8 block_count,
    u16 block_size,
    u16 offset
) {

    static u16 index = 0;

    if (block_count == 0) {
        return;
    }

    for (u8 i = 0; i < block_count; i += 1) {

        if (index >= SHARED_MEMORY_COUNT_SLOTS) {
            DEBUG_TRACE_word(SHARED_MEMORY_COUNT_SLOTS, "shared_memory_init_container() - Maximum number of containers reached - NUM:");
            break;
        }

        shared_memory_container[index].bytes_available = block_size;
        shared_memory_container[index].max_length = block_size;
        shared_memory_container[index].occupied = SHARED_MEMORY_SLOT_UNUSED;
        shared_memory_container[index].read_pointer = 0;
        shared_memory_container[index].write_pointer = 0;
        shared_memory_container[index].offset = offset;

        DEBUG_TRACE_word(index, "shared_memory_init_container() - Index:");
        DEBUG_TRACE_word(block_size, " - Size:");
        DEBUG_TRACE_word(offset, " - Offset:");

        offset += block_size;
        index += 1;
    }
}

// --------------------------------------------------------------------------------

/**
 * @see shared_memory.h
 */
void shared_memory_initialize(void) {

    /**
     * @brief Is set to 1 after XXX was executed.
     * Used as a guard to avoid initializing this module again.
     */
    static u8 is_initialized = 0;

    if (is_initialized == 0) {
        is_initialized = 1;
    } else {
        return;
    }

    DEBUG_TRACE_word(
        sizeof(shared_memory_block),
        "shared_memory_initialize() - Clear memory block - Size:"
    );

    DEBUG_TRACE_byte(
        SHARED_MEMORY_COUNT_64_BYTES,
        "- #64-byte blocks: "
    );

    DEBUG_TRACE_byte(
        SHARED_MEMORY_COUNT_128_BYTES,
        "- #128-byte blocks: "
    );

    DEBUG_TRACE_byte(
        SHARED_MEMORY_COUNT_256_BYTES,
        "- #256-byte blocks: "
    );

    DEBUG_TRACE_byte(
        SHARED_MEMORY_COUNT_512_BYTES,
        "- #512-byte blocks: "
    );

    DEBUG_TRACE_byte(
        SHARED_MEMORY_COUNT_1024_BYTES,
        "- #1024-byte blocks: "
    );

    DEBUG_TRACE_byte(
        SHARED_MEMORY_COUNT_2048_BYTES,
        "- #2048-byte blocks: "
    );

    DEBUG_TRACE_byte(
        SHARED_MEMORY_COUNT_4096_BYTES,
        "- #4096-byte blocks: "
    );

    shared_memory_init_container(
        SHARED_MEMORY_COUNT_64_BYTES,
        SHARED_MEMORY_SIZE_64_BYTE,
        SHARED_MEMORY_ADDR_OFFSET_64
    );

    shared_memory_init_container(
        SHARED_MEMORY_COUNT_128_BYTES,
        SHARED_MEMORY_SIZE_128_BYTE,
        SHARED_MEMORY_ADDR_OFFSET_128
    );

    shared_memory_init_container(
        SHARED_MEMORY_COUNT_256_BYTES,
        SHARED_MEMORY_SIZE_256_BYTE,
        SHARED_MEMORY_ADDR_OFFSET_256
    );

    shared_memory_init_container(
        SHARED_MEMORY_COUNT_512_BYTES,
        SHARED_MEMORY_SIZE_512_BYTE,
        SHARED_MEMORY_ADDR_OFFSET_512
    );

    shared_memory_init_container(
        SHARED_MEMORY_COUNT_1024_BYTES,
        SHARED_MEMORY_SIZE_1024_BYTE,
        SHARED_MEMORY_ADDR_OFFSET_1024
    );

    shared_memory_init_container(
        SHARED_MEMORY_COUNT_2048_BYTES,
        SHARED_MEMORY_SIZE_2048_BYTE,
        SHARED_MEMORY_ADDR_OFFSET_2048
    );

    shared_memory_init_container(
        SHARED_MEMORY_COUNT_4096_BYTES,
        SHARED_MEMORY_SIZE_4096_BYTE,
        SHARED_MEMORY_ADDR_OFFSET_4096
    );
    
    memset(shared_memory_block, 0x00, sizeof(shared_memory_block));
}

// --------------------------------------------------------------------------------
