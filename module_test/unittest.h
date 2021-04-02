#ifndef _UNITTEST_H_
#define _UNITTEST_H_

// --------------------------------------------------------------------------------

#include <stdio.h>

// --------------------------------------------------------------------------------

#define UT_GET_VAR_NAME(var)				#var

// --------------------------------------------------------------------------------

#define UT_ACTIVATE()					static u8  __ut_test_case_id = 0;										\
							static u16 __ut_number_of_test_passed = 0;									\
							static u16 __ut_number_of_test_failed = 0;

#define UT_SET_TEST_CASE_ID(id)				__ut_test_case_id = id
#define UT_GET_TEST_CASE_ID()				__ut_test_case_id

// --------------------------------------------------------------------------------

#define UT_PRINT_HEX_DUMP(array, length)		{														\
								u16 i = 0;												\
								u16 buffer_index = 0;											\
								u16 character_index = 0;										\
								u16 address = 0;											\
								u8 line_size = 16;											\
								u8 end_of_line = 0;											\
								u16 array_length = length;										\
																					\
								printf("\n\t%04X  |  ", address);									\
																					\
								do {													\
																					\
									if (array_length != 0) {									\
										printf("%02x ", array[buffer_index++]);							\
										array_length--;										\
									} else {											\
										printf("__ ");										\
									}												\
																					\
									if ( (i != 0) && ((i + 1) % 8) == 0) {								\
										printf(" ");										\
									}												\
																					\
									if ( !(++i % line_size) ) {									\
																					\
										printf(" |  ");										\
																					\
										while (character_index < buffer_index) {						\
																					\
											unsigned char c = (unsigned char) array[character_index++];			\
											printf("%c", (c < 33 || c == 255) ? 0x2E : c);					\
										}											\
																					\
										printf("\n");										\
																					\
										if (array_length != 0) {								\
											address += line_size;								\
											printf("\t%04X  |  ", address);							\
										} else {										\
											end_of_line = 1;								\
										}											\
									}												\
																					\
								} while (end_of_line == 0);										\
							}

// --------------------------------------------------------------------------------

#define UT_EQUATION_ERROR(value, reference)		printf(" - !! FAILED !! : %s = %ld (expected: %ld) in %s:%d\n", UT_GET_VAR_NAME(value), (long)value, (long)reference, __FILE__, __LINE__);
#define UT_EQUATION_OK(value)				printf(" - OK : %s = %ld\n", UT_GET_VAR_NAME(value), (long)value);

#define UT_GREATER_OK(value, reference)			printf(" - OK : %s = %d > %d\n", UT_GET_VAR_NAME(value), value, reference);
#define UT_GREATER_ERROR(value, reference)		printf(" - !! FAILED !! : %s <= %d (expected: %d) in %s:%d\n", UT_GET_VAR_NAME(value), value, reference, __FILE__, __LINE__);

#define UT_STRING_ERROR(value, reference)		printf(" - !! FAILED !! : \n%s = %s \nexpected: \n%s = %s\n in %s:%d\n", UT_GET_VAR_NAME(value), value, UT_GET_VAR_NAME(value), reference, __FILE__, __LINE__);
#define UT_STRING_OK(value)				printf(" - OK : %s = %s\n", UT_GET_VAR_NAME(value), value);

#define UT_ARRAY_LENGTH_ERROR(value, len, ref)		printf(" - !! ARRAY-LENGTH ERROR !! : sizeof(%s) = %d < %d  in %s:%d\n", UT_GET_VAR_NAME(value), (int)len, (int)ref, __FILE__, __LINE__);
#define UT_ARRAY_ERROR(value, reference, length)	printf(" - !! FAILED !! : %s  in %s:%d\n", UT_GET_VAR_NAME(value), __FILE__, __LINE__);				\
							printf(" - - content  :");											\
							UT_PRINT_HEX_DUMP((value), length)										\
							printf("\n - - expected :");											\
							UT_PRINT_HEX_DUMP((reference), length)										\
							printf("\n");													\
							do{}while(0)							
#define UT_ARRAY_OK(array, length)			printf(" - OK : %s = { ", UT_GET_VAR_NAME(array));								\
							UT_PRINT_HEX_DUMP((array), length);										\
							printf(" } \n");

// --------------------------------------------------------------------------------

#define UT_START_TESTBENCH(name)			printf("-------------------------------------------------\n");							\
							printf("%s \n", name);												\
							printf("-------------------------------------------------\n");
#define UT_END_TESTBENCH()				printf("\n-------------------------------------------------\n");						\
							printf("END OF TESTBENCH - Test passed: %d / %d \n", __ut_number_of_test_passed, __ut_number_of_test_failed + __ut_number_of_test_passed);	\
							printf("-------------------------------------------------\n");

#define UT_TEST_RESULT()				(__ut_number_of_test_failed != 0 ? 1 : 0)

// --------------------------------------------------------------------------------

#define UT_START_TEST_CASE(name)			{														\
								u32 counter_TEST_PASSED = 0;										\
								u32 counter_TEST_FAILED = 0;										\
								const char name_TEST_CASE[] = name;									\
								printf("\n---------- Start of Test-Case: %s\n", name_TEST_CASE);		

#define UT_END_TEST_CASE()					printf("---------- End of Test-Case: %s - PASSED: %d / %d\n", name_TEST_CASE, counter_TEST_PASSED, counter_TEST_FAILED + counter_TEST_PASSED);		\
							}

// --------------------------------------------------------------------------------

#define UT_CHECK_IS_GREATER(value, reference)		if (value > reference ) {											\
								counter_TEST_PASSED += 1;										\
								__ut_number_of_test_passed += 1;									\
								UT_GREATER_OK(value, reference)										\
							} else {													\
								counter_TEST_FAILED += 1;										\
								__ut_number_of_test_failed += 1;									\
								UT_GREATER_ERROR(value, reference)									\
							}

#define UT_CHECK_IS_EQUAL(value, reference)		if (reference != value) {											\
								counter_TEST_FAILED += 1;										\
								__ut_number_of_test_failed += 1;									\
								UT_EQUATION_ERROR(value, reference)									\
							} else {													\
								counter_TEST_PASSED += 1;										\
								__ut_number_of_test_passed += 1;									\
								UT_EQUATION_OK(value)											\
							}

#define UT_COMPARE_STRING(value, reference)		if (strlen(reference) != strlen(value) || memcmp(value, reference, strlen(reference)) != 0) {			\
								counter_TEST_FAILED += 1;										\
								__ut_number_of_test_failed += 1;									\
								UT_STRING_ERROR(value, reference)									\
							} else {													\
								counter_TEST_PASSED += 1;										\
								__ut_number_of_test_passed += 1;									\
								UT_STRING_OK(value)											\
							}

#define UT_COMPARE_ARRAY(array1, array2, length)	{														\
								if (sizeof(array1) < length) {										\
									UT_ARRAY_LENGTH_ERROR(array1, sizeof(array1), length)						\
									UT_ARRAY_ERROR(array1, array2, length);								\
									__ut_number_of_test_failed += 1;								\
									counter_TEST_FAILED += 1;									\
																					\
								} else if (sizeof(array2) < length) {									\
									UT_ARRAY_LENGTH_ERROR(array2, sizeof(array2), length)						\
									UT_ARRAY_ERROR(array1, array2, length);								\
									__ut_number_of_test_failed += 1;								\
									counter_TEST_FAILED += 1;									\
																					\
								} else {												\
									u8 i = 0;											\
									u8 is_equal = 1;										\
									for ( ; i < length; i++) {									\
										if ((array1)[i] != (array2)[i]) {							\
											UT_ARRAY_ERROR(array1, array2, length);						\
											__ut_number_of_test_failed += 1;						\
											counter_TEST_FAILED += 1;							\
											is_equal = 0;									\
											break;										\
										}											\
									}												\
																					\
									if (is_equal) {											\
										UT_ARRAY_OK(array1, length)								\
										__ut_number_of_test_passed += 1;							\
										counter_TEST_PASSED += 1;								\
									}												\
								}													\
							}

// --------------------------------------------------------------------------------

#endif
