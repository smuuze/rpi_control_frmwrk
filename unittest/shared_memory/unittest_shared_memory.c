/*! 
 * --------------------------------------------------------------------------------
 *
 * \file    unittest_shared_memory.c
 * \brief
 * \author    sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */

#define TRACER_OFF

// --------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------

#include "tracer.h"

// --------------------------------------------------------------------------------

#include "../unittest.h"

UT_ACTIVATE()

// --------------------------------------------------------------------------------

#include "shared_memory.h"
#include "math_module.h"

// --------------------------------------------------------------------------------

#define UNITTEST_OUTPUT_PREFIX                  "UT - "

// --------------------------------------------------------------------------------

#define UNITTEST_CFG_FILE_PATH                  "shared_memory_output.txt"

// --------------------------------------------------------------------------------

#define TEST_CASE_ID_INIT                       1

// --------------------------------------------------------------------------------

static void unittest_reset_counter(void) {

}

// --------------------------------------------------------------------------------

// slots

// --------------------------------------------------------------------------------

// Signals

// --------------------------------------------------------------------------------

// stubs

void cfg_file_parser_init(void) {

}

// --------------------------------------------------------------------------------

SHARED_MEMORY_CREATE (UT_SHARED_MEM_44, 44)
SHARED_MEMORY_CREATE (UT_SHARED_MEM_64, 64)
SHARED_MEMORY_CREATE (UT_SHARED_MEM_128, 128)
SHARED_MEMORY_CREATE (UT_SHARED_MEM_4096, 4096)
SHARED_MEMORY_CREATE (UT_SHARED_MEM_12900, 12900)

SHARED_MEMORY_CREATE (UT_SHARED_MEM_4090, 4090)

// --------------------------------------------------------------------------------

static void TEST_CASE_init(void) {

    UT_START_TEST_CASE("TEST_CASE_initt")
    {
        shared_memory_initialize();
    }
    UT_END_TEST_CASE()
}

/**
 * @brief Test allocation of different shared memory blocks
 * @details - Test successful allocation
 *          - Test failed allocation - memory too big
 *          - Test failed allocation - no more memory available
 */
static void TEST_CASE_allocate(void) {

    UT_START_TEST_CASE("TEST_CASE_allocate")
    {
        // successful
        u8 ret_val = UT_SHARED_MEM_44_allocate();
        UT_CHECK_IS_EQUAL( ret_val, SHARED_MEMORY_MEMORY_ALLOCATED);
        UT_CHECK_IS_GREATER(UT_SHARED_MEM_44_available(), 44);

        // successful
        ret_val = UT_SHARED_MEM_64_allocate();
        UT_CHECK_IS_EQUAL( ret_val, SHARED_MEMORY_MEMORY_ALLOCATED);
        UT_CHECK_IS_EQUAL(UT_SHARED_MEM_64_available(), 64);

        // successfull
        ret_val = UT_SHARED_MEM_128_allocate();
        UT_CHECK_IS_EQUAL( ret_val, SHARED_MEMORY_MEMORY_ALLOCATED);
        UT_CHECK_IS_EQUAL(UT_SHARED_MEM_128_available(), 128);

        // successfull
        ret_val = UT_SHARED_MEM_4096_allocate();
        UT_CHECK_IS_EQUAL( ret_val, SHARED_MEMORY_MEMORY_ALLOCATED);
        UT_CHECK_IS_EQUAL(UT_SHARED_MEM_4096_available(), 4096);

        // too big
        ret_val = UT_SHARED_MEM_12900_allocate();
        UT_CHECK_IS_EQUAL( ret_val, SHARED_MEMORY_NO_MORE_MEMORY_AVAILABLE);
        UT_CHECK_IS_EQUAL(UT_SHARED_MEM_12900_available(), 0);

        // no more memory
        ret_val = UT_SHARED_MEM_4090_allocate();
        UT_CHECK_IS_EQUAL( ret_val, SHARED_MEMORY_NO_MORE_MEMORY_AVAILABLE);
        UT_CHECK_IS_EQUAL(UT_SHARED_MEM_4090_available(), 0);
        
    }
    UT_END_TEST_CASE()
}

/**
 * @brief Test to add a single byte to a shared memory object.
 * The byte is read back immediately.
 * @details - checks for return value on adding data
 *          - checks for number of byytes available
 *          - checks for correct data that is read back
 */
static void TEST_CASE_add_u8(void) {

    UT_START_TEST_CASE("TEST_CASE_add_u8")
    {
        u8 data_ref[256];

        // fill the reference array with dummy data
        for (u16 i = 0; i < sizeof(data_ref); i += 1) {
            data_ref[i] = (u8)(i + 1);
        }

        UT_SHARED_MEM_44_reset();
        u16 bytes_available = UT_SHARED_MEM_44_available();

        for (u8 repeat = 0; repeat < 3; repeat += 1) {

            u16 bytes_written = 0;

            while (UT_SHARED_MEM_44_available()) {

                u8 ret_val = UT_SHARED_MEM_44_add_u8(data_ref[bytes_written]);

                if (ret_val != 1) {
                    UT_CHECK_IS_EQUAL( ret_val, 1);
                    break;
                }

                bytes_written += 1;

                if (bytes_written >= sizeof(data_ref)) {
                    break;
                }

            }

            UT_CHECK_IS_GREATER(bytes_written, 44);

            u8 data_read[sizeof(data_ref)];

            for (u16 i = 0; i < bytes_written; i += 1) {
                data_read[i] = UT_SHARED_MEM_44_get_u8();
            }
            
            u16 bytes_avail = UT_SHARED_MEM_44_available();
            UT_CHECK_IS_EQUAL( bytes_avail, bytes_available);
            UT_COMPARE_ARRAY(data_read, data_ref, bytes_written);
        }
    }
    UT_END_TEST_CASE()
}

/**
 * @brief The goal is to check if a u16 can be added and read out.
 * The number of available bytes shall be decrease/increase with adding/removing
 * the value.
 */
static void TEST_CASE_add_u16(void) {

    UT_START_TEST_CASE("TEST_CASE_add_u16")
    {
        u16 data_ref[256];

        // fill the reference array with dummy data
        for (u16 i = 0; i < sizeof(data_ref); i += 1) {
            data_ref[i] = (i + 1);
        }

        UT_SHARED_MEM_44_reset();

        for (u8 repeat = 0; repeat < 3; repeat += 1) {

            u16 words_written = 0;
            u16 bytes_available_before = UT_SHARED_MEM_44_available();

            while (UT_SHARED_MEM_44_available()) {

                u8 ret_val = UT_SHARED_MEM_44_add_u16(data_ref[words_written]);

                if (ret_val != 1) {
                    UT_CHECK_IS_EQUAL( ret_val, 1);
                    break;
                }

                words_written += 1;

                if (words_written >= sizeof(data_ref)) {
                    break;
                }
            }

            UT_CHECK_IS_GREATER(words_written, ((44 / 2) - 1)); // one word has two bytes

            u8 data_read[sizeof(data_ref)];

            for (u16 i = 0; i < words_written; i += 1) {
                data_read[i] = UT_SHARED_MEM_44_get_u16();
            }
            
            u16 bytes_available_after = UT_SHARED_MEM_44_available();
            UT_CHECK_IS_EQUAL( bytes_available_after, bytes_available_before);
            UT_COMPARE_ARRAY(data_read, data_ref, words_written);
        }
    }
    UT_END_TEST_CASE()
}

/**
 * @brief The goal is to check if a u16 can be added and read out.
 * The number of available bytes shall be decrease/increase with adding/removing
 * the value.
 */
static void TEST_CASE_add_u32(void) {

    UT_START_TEST_CASE("TEST_CASE_add_u32")
    {
        u32 data_ref[256];

        // fill the reference array with dummy data
        for (u16 i = 0; i < sizeof(data_ref); i += 1) {
            data_ref[i] = (u32)(i + 1);
        }

        for (u8 repeat = 0; repeat < 3; repeat += 1) {

            u32 u32_written = 0;
            u16 bytes_available_before = UT_SHARED_MEM_44_available();

            while (UT_SHARED_MEM_44_available()) {

                u8 ret_val = UT_SHARED_MEM_44_add_u32(data_ref[u32_written]);

                if (ret_val != 1) {
                    UT_CHECK_IS_EQUAL( ret_val, 1);
                    break;
                }

                u32_written += 1;

                if (u32_written >= 256) {
                    break;
                }

            }

            UT_CHECK_IS_GREATER(u32_written, ((44 / 4) - 1)); // one int has four bytes

            u8 data_read[sizeof(data_ref)];

            for (u16 i = 0; i < u32_written; i += 1) {
                data_read[i] = UT_SHARED_MEM_44_get_u32();
            }
            
            u16 bytes_available_after = UT_SHARED_MEM_44_available();
            UT_CHECK_IS_EQUAL( bytes_available_after, bytes_available_before);
            UT_COMPARE_ARRAY(data_read, data_ref, u32_written);
        }
    }
    UT_END_TEST_CASE()
}

/**
 * @brief The goal is to check if writing data will be continued at the begin
 * of the memory-block in case the end of the block is reached.
 */
static void TEST_CASE_check_ring_buffer_mechanism(void) {

    UT_START_TEST_CASE("TEST_CASE_check_ring_buffer_mechanism")
    {
        u8 reference[40];
        memset(reference, 0xAA, sizeof(reference));

        for (u8 i = 0; i < 3; i++) {
            u16 avail_before = UT_SHARED_MEM_44_available();
            u16 avail_after = avail_before - sizeof(reference);

            u8 ret_val = UT_SHARED_MEM_44_add_data(reference, sizeof(reference));

            UT_CHECK_IS_EQUAL( ret_val, 1);
            UT_CHECK_IS_EQUAL(UT_SHARED_MEM_44_available(), avail_after);

            u8 read_out[sizeof(reference)];
            memset(read_out, 0x00, sizeof(read_out));

            u16 bytes_read = UT_SHARED_MEM_44_get_data(read_out, sizeof(read_out));

            UT_CHECK_IS_EQUAL( bytes_read, sizeof(read_out));
            UT_COMPARE_ARRAY(read_out, reference, sizeof(reference));
            UT_CHECK_IS_EQUAL(UT_SHARED_MEM_44_available(), avail_before);
        }
    }
    UT_END_TEST_CASE()
}

/**
 * @brief Chekcs if the shared memory object is freed successfull
 * And the memory can be allocated by another object.
 */
static void TEST_CASE_free(void) {

    UT_START_TEST_CASE("TEST_CASE_free")
    {
        // no more memory
        u8 ret_val = UT_SHARED_MEM_4090_allocate();
        UT_CHECK_IS_EQUAL( ret_val, SHARED_MEMORY_NO_MORE_MEMORY_AVAILABLE);
        UT_CHECK_IS_EQUAL(UT_SHARED_MEM_4090_available(), 0);
        
        UT_CHECK_IS_GREATER(UT_SHARED_MEM_4096_available(), 0);
        UT_SHARED_MEM_4096_free();
        UT_CHECK_IS_EQUAL(UT_SHARED_MEM_4096_available(), 0);

        ret_val = UT_SHARED_MEM_4090_allocate();
        UT_CHECK_IS_EQUAL( ret_val, SHARED_MEMORY_MEMORY_ALLOCATED);
        UT_CHECK_IS_GREATER(UT_SHARED_MEM_4090_available(), 4090);

        UT_CHECK_IS_GREATER(UT_SHARED_MEM_44_available(), 0);
        UT_SHARED_MEM_44_free();
        UT_CHECK_IS_EQUAL(UT_SHARED_MEM_44_available(), 0);
        
        UT_CHECK_IS_GREATER(UT_SHARED_MEM_64_available(), 0);
        UT_SHARED_MEM_64_free();
        UT_CHECK_IS_EQUAL(UT_SHARED_MEM_64_available(), 0);
    }
    UT_END_TEST_CASE()
}

/**
 * @brief Test to check if the rseek function is working properly
 * @details - checks for moving forward
 *          - checks for moving backwards
 *          - checks for number of available bytes
 */
static void TEST_CASE_read_seek(void) {

    UT_START_TEST_CASE("TEST_CASE_read_seek")
    {
        u8 data_ref[256];

        // fill the reference array with dummy data
        for (u16 i = 0; i < sizeof(data_ref); i += 1) {
            data_ref[i] = (u8)(i + 1);
        }

        UT_SHARED_MEM_44_reset();

        u16 bytes_available_before = UT_SHARED_MEM_44_available();
        UT_CHECK_IS_GREATER( bytes_available_before, 44);
            
        u16 bytes_occupied = UT_SHARED_MEM_44_occupied();
        UT_CHECK_IS_EQUAL( bytes_occupied, 0);

        u8 ret_val = UT_SHARED_MEM_44_add_data(data_ref, bytes_available_before);
        u16 bytes_written = bytes_available_before;
        UT_CHECK_IS_EQUAL( ret_val, 1);

        u16 bytes_available_after = UT_SHARED_MEM_44_available();
        UT_CHECK_IS_EQUAL( bytes_available_after, 0);

        bytes_occupied = UT_SHARED_MEM_44_occupied();
        UT_CHECK_IS_EQUAL( bytes_occupied, bytes_available_before);

        u8 data_read[sizeof(data_ref)];

        u16 bytes_read = UT_SHARED_MEM_44_get_data(data_read, bytes_occupied);
        UT_CHECK_IS_EQUAL( bytes_read, bytes_occupied);

        bytes_occupied = UT_SHARED_MEM_44_occupied();
        UT_CHECK_IS_EQUAL( bytes_occupied, 0);

        bytes_available_before = UT_SHARED_MEM_44_available();
        UT_CHECK_IS_GREATER( bytes_available_before, 44);

        UT_COMPARE_ARRAY(data_read, data_ref, bytes_read);

        // seek backwards by 10

        i16 offset = -10;
        u16 offset_abs = (u16)math_get_abs((i32)offset);

        UT_SHARED_MEM_44_rseek(offset);

        bytes_occupied = UT_SHARED_MEM_44_occupied();
        UT_CHECK_IS_EQUAL( bytes_occupied, offset_abs);

        bytes_available_after = UT_SHARED_MEM_44_available();
        u16 reference = (bytes_available_before + offset);
        UT_CHECK_IS_EQUAL( bytes_available_after, reference);

        bytes_read = UT_SHARED_MEM_44_get_data(data_read, bytes_occupied);
        UT_CHECK_IS_EQUAL( bytes_read, bytes_occupied);

        u8* p_data_ref = &data_ref[bytes_written + offset];

        for (u16 i = 0; i < offset_abs; i += 1) {
            UT_CHECK_IS_EQUAL(data_read[i], p_data_ref[i]);
        }

    }
    UT_END_TEST_CASE()
}

/**
 * @brief The goal is to check if reseting data will not affect
 * memory blocks before and behind the block that is reset.
 */
static void TEST_CASE_reset_data(void) {

    UT_START_TEST_CASE("TEST_CASE_add_not_allocated")
    {
        for (u8 repeat = 0; repeat < 3; repeat += 1) {

            u32 u32_written = 0x11111111;
            
            u8 ret_val = UT_SHARED_MEM_44_add_u32(u32_written);
            UT_CHECK_IS_EQUAL( ret_val, 0);
            
            u32_written += 0x11111111;
        }

        u16 bytes_available = UT_SHARED_MEM_44_available();
        UT_CHECK_IS_EQUAL( bytes_available, 0);
    }
    UT_END_TEST_CASE()
}

/**
 * @brief The goal is to check if adding data fails, because
 * the shared memory has not allocated.
 */
static void TEST_CASE_add_not_allocated(void) {

    UT_START_TEST_CASE("TEST_CASE_add_not_allocated")
    {
        for (u8 repeat = 0; repeat < 3; repeat += 1) {

            u32 u32_written = 0x11111111;

            u8 ret_val = UT_SHARED_MEM_44_add_u32(u32_written);
            UT_CHECK_IS_EQUAL( ret_val, 0);
            
            u32_written += 0x11111111;
        }

        u16 bytes_available = UT_SHARED_MEM_44_available();
        UT_CHECK_IS_EQUAL( bytes_available, 0);
    }
    UT_END_TEST_CASE()
}

/**
 * @brief The goal is to check if data is nto overwritten
 * in case of reset and adding data to a neigbor memory block.
 */
static void TEST_CASE_data_is_not_overwritten(void) {

    UT_START_TEST_CASE("TEST_CASE_data_is_not_overwritten")
    {
        u8 data_ref[128];

        // fill the reference array with dummy data
        for (u16 i = 0; i < sizeof(data_ref); i += 1) {
            data_ref[i] = (i + 1);
        }

        u16 bytes_available = UT_SHARED_MEM_64_available();
        u8 ret_val = UT_SHARED_MEM_64_add_data(data_ref, bytes_available);
        UT_CHECK_IS_EQUAL( ret_val, 1);
        u16 bytes_written = bytes_available - UT_SHARED_MEM_64_available();

        bytes_available = UT_SHARED_MEM_44_available();
        ret_val = UT_SHARED_MEM_44_add_data(data_ref, bytes_available);
        UT_CHECK_IS_EQUAL( ret_val, 1);
        UT_SHARED_MEM_44_reset();

        bytes_available = UT_SHARED_MEM_128_available();
        ret_val = UT_SHARED_MEM_128_add_data(data_ref, bytes_available);
        UT_CHECK_IS_EQUAL( ret_val, 1);
        UT_SHARED_MEM_128_reset();

        u8 data_read[sizeof(data_ref)];

        for (u16 i = 0; i < bytes_written; i += 1) {
            data_read[i] = UT_SHARED_MEM_64_get_u8();
        }

        UT_COMPARE_ARRAY(data_read, data_ref, bytes_written);
    }
    UT_END_TEST_CASE()
}

// --------------------------------------------------------------------------------

int main(void) {

    UT_START_TESTBENCH("Welcome to the UNITTEST for shared-memory v1.0")
    {
        TEST_CASE_add_not_allocated();
        TEST_CASE_init();
        TEST_CASE_allocate();
        TEST_CASE_add_u8();
        TEST_CASE_add_u16();
        TEST_CASE_add_u32();

        TEST_CASE_check_ring_buffer_mechanism();
        TEST_CASE_data_is_not_overwritten();

        TEST_CASE_read_seek();

        TEST_CASE_free();
        TEST_CASE_add_not_allocated();
    }
    UT_END_TESTBENCH()

    return UT_TEST_RESULT();
}

// --------------------------------------------------------------------------------
