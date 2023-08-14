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
 * @file    uf2_interface.h
 * @author  Sebastian Lesse
 * @date    2022 / 12 / 31
 * @brief   Short description of this file
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_uf2_interface_
#define _H_uf2_interface_

// --------------------------------------------------------------------------------

#include "cpu.h"

// --------------------------------------------------------------------------------

/**
 * @brief Number of bytes that are cotnained within a single uf2 block.
 * 
 */
#define UF2_BLOCK_NUM_BYTES                                             512

// --------------------------------------------------------------------------------

/**
 * @brief not main flash -
 * This block should be skipped when writing the device flash.
 * It can be used to store "comments" in the file, typically
 * embedded source code or debug info that does not fit on the device flash.
 */
#define UF2_BLOCK_FLAG_NOT_MAIN_FLASH                                   0x00000001

/**
 * @brief file container
 * In such a usage the file container flag is set on blocks,
 * the field fileSize holds the file size of the current file,
 * and the field targetAddr holds the offset in current file.
 * The not main flash flag on blocks should be ignored when the
 * file container is set.
 * 
 */
#define UF2_BLOCK_FLAG_FILE_CONTAINER                                   0x00001000


/**
 * @brief familyID present
 * When set, the fileSize/familyID holds a value identifying the
 * board family (usually corresponds to an MCU)
 */
#define UF2_BLOCK_FLAG_FAMILY_ID_PRESENT                                0x00002000

/**
 * @brief MD5 checksum present
 * The flashing program should compute the MD5 sum of the specified region.
 * If the region checksum matches, flashing of the current block can be skipped.
 * Typically, many blocks in sequence will have the same region specified,
 * and can all be skipped, if the matching succeeded. The position of the
 * current block will typically be inside of the region. The position and
 * size of the region should be multiple of page erase size
 * (4k or 64k on typical SPI flash).
 */
#define UF2_BLOCK_FLAG_MD5_PRESENT                                      0x00004000

/**
 * @brief extension tags present
 * When the 0x8000 flag is set, additional information can be appended
 * right after payload data (i.e., it starts at 32 + payloadSize).
 * Every tag starts at 4 byte boundary. The first byte of tag contains its
 * total size in bytes (including the size byte and type designation).
 * The next three bytes designate the type of tag (if you want to define
 * custom tags, pick them at random). The last tag has size of 0 and type of 0.
 * 
 * Standard tag designations follow:
 * 
 *      0x9fc7bc - version of firmware file - UTF8 semver string
 *      0x650d9d - description of device for which the firmware file is
 *                 destined (UTF8)
 *      0x0be9f7 - page size of target device (32 bit unsigned number)
 *      0xb46db0 - SHA-2 checksum of firmware (can be of various size)
 *      0xc8a729 - device type identifier - a refinement of familyID meant to
 *                 identify a kind of device (eg., a toaster with specific pinout
 *                 and heating unit), not only MCU; 32 or 64 bit number;
 *                 can be hash of 0x650d9d
 */
#define UF2_BLOCK_FLAG_EXTENIONS_PRESENT                                0x00008000

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
#define UF2_CREATE_BLOCK(name)                                                  \
                                                                                \
    static UF2_BLOCK_CONTEXT_TYPE __##name##__uf2_context = {                   \
        .flags = 0,                                                             \
        .target_addr = 0,                                                       \
        .payload_size = 0,                                                      \
        .block_number = 0,                                                      \
        .number_of_blocks = 0,                                                  \
        .file_size = 0,                                                         \
        .p_payload = 0                                                          \
    };                                                                          \
                                                                                \
    static UF2_ERROR name##_parse(const u8* p_buffer) {                         \
        return uf2_block_parse(                                                 \
            &__##name##__uf2_context,                                           \
            p_buffer                                                            \
        );                                                                      \
    }                                                                           \
                                                                                \
    static u32 name##_get_flags(void) {                                         \
        return uf2_block_get_flags(                                             \
            &__##name##__uf2_context                                            \
        );                                                                      \
    }                                                                           \
                                                                                \
    static u32 name##_get_target_address(void) {                                \
        return uf2_block_get_target_address(                                    \
            &__##name##__uf2_context                                            \
        );                                                                      \
    }                                                                           \
                                                                                \
    static u32 name##_get_payload_size(void) {                                  \
        return uf2_block_get_payload_size(                                      \
            &__##name##__uf2_context                                            \
        );                                                                      \
    }                                                                           \
                                                                                \
    static u32 name##_get_block_number(void) {                                  \
        return uf2_block_get_block_number(                                      \
            &__##name##__uf2_context                                            \
        );                                                                      \
    }                                                                           \
                                                                                \
    static u32 name##_get_number_of_blocks(void) {                              \
        return uf2_block_get_number_of_blocks(                                  \
            &__##name##__uf2_context                                            \
        );                                                                      \
    }                                                                           \
                                                                                \
    static u8 name##_has_flag_set(u32 flag) {                                   \
        return uf2_block_has_flag_set(                                          \
            &__##name##__uf2_context,                                           \
            flag                                                                \
        );                                                                      \
    }                                                                           \
                                                                                \
    static const u8* name##_get_payload(void) {                                 \
        return uf2_block_get_payload(                                           \
            &__##name##__uf2_context                                            \
        );                                                                      \
    }                                                                           \
                                                                                \
    static u32 name##_get_file_size(void) {                                     \
        return uf2_block_get_file_size(                                         \
            &__##name##__uf2_context                                            \
        );                                                                      \
    }

// --------------------------------------------------------------------------------

/**
 * @brief 
 */
typedef struct UF2_BLOCK_CONTEXT {

    /**
     * @brief
     */
    u32 flags;

    /**
     * @brief 
     * 
     */
    u32 target_addr;

    /**
     * @brief The number of data bytes is configurable and depends on the
     * size of the flash page (that is the smallest size that can be erased)
     * on the microcontroller.
     */
    u32 payload_size;

    /**
     * @brief Number of the current block.
     */
    u32 block_number;

    /**
     * @brief Number of all available blocks of thie current file.
     */
    u32 number_of_blocks;

    /**
     * @brief This field is optional, and should be set only when the
     * corresponding flag is set. It is recommended that new bootloaders
     * require the field to be set appropriately, and refuse to flash UF2
     * files without it. If you're developing your own bootloader, and
     * your board family isn't listed here, pick a new family ID at random.
     * It's good to also send a PR here, so your family can be listed.
     */
    u32 file_size;

    /**
     * @brief 
     * 
     */
    const u8* p_payload;

} UF2_BLOCK_CONTEXT_TYPE;

// --------------------------------------------------------------------------------

/**
 * @brief 
 */
typedef enum {

    /**
     * @brief Indication of no error
     */
    UF2_ERROR_NONE = 0x00,

    /**
     * @brief The magic-number START0 of the current block is invalid.
     */
    UF2_ERROR_INVALID_START0,

    /**
     * @brief The magic-number START1 of the current block is invalid.
     */
    UF2_ERROR_INVALID_START1,

    /**
     * @brief The magic-number END of the current block is invalid.
     */
    UF2_ERROR_INVALID_END
} UF2_ERROR;

// --------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param p_block_ctx 
 * @param p_buffer 
 * @return UF2_ERROR_NONE if block was parsed successful, otherwise @see UF2_ERROR
 */
UF2_ERROR uf2_block_parse(UF2_BLOCK_CONTEXT_TYPE* p_block_ctx, const u8* p_buffer);

/**
 * @brief 
 * 
 * @param p_block_ctx 
 * @return u32 
 */
u32 uf2_block_get_flags(UF2_BLOCK_CONTEXT_TYPE* p_block_ctx);

/**
 * @brief 
 * 
 * @param p_block_ctx 
 * @return u32 
 */
u32 uf2_block_get_target_address(UF2_BLOCK_CONTEXT_TYPE* p_block_ctx);

/**
 * @brief 
 * 
 * @param p_block_ctx 
 * @return u32 
 */
u32 uf2_block_get_payload_size(UF2_BLOCK_CONTEXT_TYPE* p_block_ctx);

/**
 * @brief 
 * 
 * @param p_block_ctx 
 * @return u32 
 */
u32 uf2_block_get_block_number(UF2_BLOCK_CONTEXT_TYPE* p_block_ctx);

/**
 * @brief 
 * 
 * @param p_block_ctx 
 * @return u32 
 */
u32 uf2_block_get_number_of_blocks(UF2_BLOCK_CONTEXT_TYPE* p_block_ctx);

/**
 * @brief 
 * 
 * @param p_block_ctx 
 * @return u32 
 */
u32 uf2_block_get_file_size(UF2_BLOCK_CONTEXT_TYPE* p_block_ctx);

/**
 * @brief 
 * 
 * @param p_block_ctx 
 * @return const u8* 
 */
const u8* uf2_block_get_payload(UF2_BLOCK_CONTEXT_TYPE* p_block_ctx);

/**
 * @brief 
 * 
 * @param p_block_ctx 
 * @param flag 
 * @return 1 if the given flag is set within the current block, otherwise 0.
 */
u8 uf2_block_has_flag_set(UF2_BLOCK_CONTEXT_TYPE* p_block_ctx, u32 flag);

/**
 * @brief 
 * 
 * @param p_block_ctx 
 * @return const u8* 
 */
u32 uf2_block_get_file_size(UF2_BLOCK_CONTEXT_TYPE* p_block_ctx);

// --------------------------------------------------------------------------------

#endif // _H_uf2_interface_

// --------------------------------------------------------------------------------
