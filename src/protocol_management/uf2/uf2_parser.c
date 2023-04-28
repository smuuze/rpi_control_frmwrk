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
 * @file    uf2_parser.c
 * @author  Sebastian Lesse
 * @date    2022 / 12 / 31
 * @brief   Short description of this file
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

#include "uf2_interface.h"

// --------------------------------------------------------------------------------

#define UF2_BLOCK_MAGIC_START0                                          0x0A324655
#define UF2_BLOCK_MAGIC_START1                                          0x9E5D5157
#define UF2_BLOCK_MAGIC_END                                             0x0AB16F30

// --------------------------------------------------------------------------------

/**
 * @brief 
 * @see https://github.com/microsoft/uf2
 */
typedef struct UF2_BLOCK {

    u32 magicStart0;
    u32 magicStart1;
    u32 flags;
    u32 target_addr;
    u32 payload_size;
    u32 block_number;
    u32 number_of_blocks;
    u32 file_size;
    u8  data[476];
    u32 magicEnd;

} UF2_BLOCK_TYPE;

// --------------------------------------------------------------------------------

/**
 * @see uf2_interface.h#uf2_block_parse
 */
UF2_ERROR uf2_block_parse(UF2_BLOCK_CONTEXT_TYPE* p_block_ctx, const u8* p_buffer) {

    const UF2_BLOCK_TYPE* p_block = (const UF2_BLOCK_TYPE*)p_buffer;

    if (p_block->magicStart0 != UF2_BLOCK_MAGIC_START0) {
        DEBUG_TRACE_long(p_block->magicStart0, "uf2_block_parse() - INVALID START0");
        return UF2_ERROR_INVALID_START0;
    }

    if (p_block->magicStart1 != UF2_BLOCK_MAGIC_START1) {
        DEBUG_TRACE_long(p_block->magicStart1, "uf2_block_parse() - INVALID START1");
        return UF2_ERROR_INVALID_START0;
    }

    if (p_block->magicEnd != UF2_BLOCK_MAGIC_END) {
        DEBUG_TRACE_long(p_block->magicEnd, "uf2_block_parse() - INVALID END");
        return UF2_ERROR_INVALID_START0;
    }

    if (p_block->block_number >= p_block->number_of_blocks) {
        DEBUG_TRACE_long(p_block->block_number, "uf2_block_parse() - UNEXPECTED BLOCK-NUMBER");
        DEBUG_TRACE_long(p_block->number_of_blocks, "uf2_block_parse() - MAX BLOCK-NUMBER");
        return UF2_ERROR_INVALID_START0;
    }

    p_block_ctx->flags = p_block->flags;
    p_block_ctx->file_size = p_block->file_size;
    p_block_ctx->block_number = p_block->block_number;
    p_block_ctx->number_of_blocks = p_block->number_of_blocks;
    p_block_ctx->payload_size = p_block->payload_size;
    p_block_ctx->target_addr = p_block->target_addr;
    p_block_ctx->p_payload = &p_block->data[0];

    return UF2_ERROR_NONE;
}

/**
 * @see uf2_interface.h#uf2_block_get_flags
 */
u32 uf2_block_get_flags(UF2_BLOCK_CONTEXT_TYPE* p_block_ctx) {
    return p_block_ctx->flags;
}

/**
 * @see uf2_interface.h#uf2_block_get_target_address
 */
u32 uf2_block_get_target_address(UF2_BLOCK_CONTEXT_TYPE* p_block_ctx) {
    return p_block_ctx->target_addr;
}
/**
 * @see uf2_interface.h#uf2_block_get_payload_size
 */
u32 uf2_block_get_payload_size(UF2_BLOCK_CONTEXT_TYPE* p_block_ctx) {
    return p_block_ctx->payload_size;
}

/**
 * @see uf2_interface.h#uf2_block_get_block_number
 */
u32 uf2_block_get_block_number(UF2_BLOCK_CONTEXT_TYPE* p_block_ctx) {
    return p_block_ctx->block_number;
}

/**
 * @see uf2_interface.h#uf2_block_get_number_of_blocks
 */
u32 uf2_block_get_number_of_blocks(UF2_BLOCK_CONTEXT_TYPE* p_block_ctx) {
    return p_block_ctx->number_of_blocks;
}

const u8* uf2_block_get_payload(UF2_BLOCK_CONTEXT_TYPE* p_block_ctx) {
    return p_block_ctx->p_payload;
}

/**
 * @see uf2_interface.h#uf2_block_get_file_size
 */
u32 uf2_block_get_file_size(UF2_BLOCK_CONTEXT_TYPE* p_block_ctx) {
    return p_block_ctx->file_size;
}

/**
 * @see uf2_interface.h#uf2_block_has_flag_set
 */
u8 uf2_block_has_flag_set(UF2_BLOCK_CONTEXT_TYPE* p_block_ctx, u32 flag) {
    return p_block_ctx->number_of_blocks;
}

// --------------------------------------------------------------------------------
