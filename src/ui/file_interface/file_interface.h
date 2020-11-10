/*! 
 * --------------------------------------------------------------------------------
 *
 * \file	ui/file_interface/file_interface.c
 * \brief
 * \author	sebastian lesse
 *
 * --------------------------------------------------------------------------------
 */
 
#ifndef _FILE_INTERFACE_H_
#define _FILE_INTERFACE_H_

// --------------------------------------------------------------------------------------

#include "config.h"

// --------------------------------------------------------------------------------------

#ifndef FILE_PATH_MAX_STRING_LENGTH
#define FILE_PATH_MAX_STRING_LENGTH						256
#endif

// --------------------------------------------------------------------------------------

/*!
 *
 */
typedef struct {
	char path[FILE_PATH_MAX_STRING_LENGTH];
	FILE* handle;
	u32 last_file_pointer;
	u32 act_file_pointer;
	u32 timestamp_last_modified;
} FILE_INTERFACE;

// --------------------------------------------------------------------------------------

#define FILE_INTERFACE_CREATE_FILE(file_name)								\
													\
	static FILE_INTERFACE _##file_name##_context = {						\
		.handle = NULL,										\
		.last_file_pointer = 0,									\
		.act_file_pointer = 0,									\
		.timestamp_last_modified = 0								\
	};												\
													\
	u8 file_name##_open(void) {									\
		return file_open(&_##file_name##_context);						\
	}												\
													\
	u8 file_name##_is_open(void) {									\
		return file_is_open(&_##file_name##_context);						\
	}												\
													\
	u8 file_name##_open_path(const char* p_path) {							\
		file_set_path(&_##file_name##_context, p_path);						\
		return file_open(&_##file_name##_context);						\
	}												\
													\
	void file_name##_set_path(const char* p_path) {							\
		file_set_path(&_##file_name##_context, p_path);						\
	}												\
													\
	void file_name##_close(void) {									\
		file_close(&_##file_name##_context);							\
	}												\
													\
	u8 file_name##_has_changed(void) {								\
		return file_has_changed(&_##file_name##_context);					\
	}												\
													\
	u8 file_name##_is_existing(void) {								\
		return file_has_changed(&_##file_name##_context);					\
	}												\
													\
	u8 file_name##_is_readable(void) {								\
		return file_is_readable(&_##file_name##_context);					\
	}												\
													\
	u32 file_name##_get_size(void) {								\
		return file_get_size(&_##file_name##_context);						\
	}												\
													\
	u8 file_name##_create(void) {									\
		return file_create(&_##file_name##_context);						\
	}												\
													\
	u8 file_name##_delete(void) {									\
		return file_delete(&_##file_name##_context);						\
	}												\
													\
	u8 file_name##_rename(const char* new_name) {							\
		return file_rename(&_##file_name##_context, new_name);					\
	}												\
													\
	i16 file_name##_read_next_line(char* p_line_to, u16 max_length) {				\
		return file_read_next_line(&_##file_name##_context, p_line_to, max_length);		\
	}												\
													\
	u16 file_name##_read_line(char* p_line_to, u16 line_number, u16 max_length) {			\
		return file_read_specific_line(&_##file_name##_context, line_number, p_line_to, max_length);	\
	}												\
													\
	u8 file_name##_append_line(const char* new_line) {						\
		return file_append_line(&_##file_name##_context, new_line);				\
	}		\
													\
	const char* file_name##_get_path(void) {							\
		return file_get_path(&_##file_name##_context);						\
	}												

// --------------------------------------------------------------------------------------

/*!
 *
 */
void file_set_path(FILE_INTERFACE* p_file, const char* path);

/*!
 *
 */
u8 file_has_changed(FILE_INTERFACE* p_file);

/*!
 *
 */
u8 file_is_existing(FILE_INTERFACE* p_file);

/*!
 *
 */
u8 file_is_readable(FILE_INTERFACE* p_file);

/*!
 *
 */
u32 file_get_size(FILE_INTERFACE* p_file);

/*!
 *
 */
u8 file_is_open(FILE_INTERFACE* p_file);

/*!
 *
 */
u8 file_create(FILE_INTERFACE* p_file);

/*!
 *
 */
u8 file_delete(FILE_INTERFACE* p_file);

/*!
 *
 */
const char* file_get_path(FILE_INTERFACE* p_file);

/*!
 *
 */
u8 file_rename(FILE_INTERFACE* p_old_file, const char* new_path);

/*!
 *
 */
u8 file_open(FILE_INTERFACE* p_file);

/*!
 *
 */
void file_close(FILE_INTERFACE* p_file);

/*!
 *
 */
i16 file_read_line(FILE* file_handle, char* p_buffer_to, u16 num_max_bytes);

/*!
 *
 */
i16 file_read_next_line(FILE_INTERFACE* p_file, char* next_line, u16 max_length);

/*!
 *
 */
u16 file_read_specific_line(FILE_INTERFACE* p_file, u16 line_number, char* next_line, u16 max_length);

/*!
 *
 */
u8 file_append_line(FILE_INTERFACE* p_file, const char* new_line);

#endif // _FILE_INTERFACE_H_