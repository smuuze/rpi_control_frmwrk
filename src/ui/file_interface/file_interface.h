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
 * @file    file_interface.h
 * @author  Sebastian Lesse
 * @date    2022 / 12 / 31
 * @brief   Short description of this file
 * 
 */

// --------------------------------------------------------------------------------

#ifndef _H_file_interface_
#define _H_file_interface_

// --------------------------------------------------------------------------------

#ifndef FILE_PATH_MAX_STRING_LENGTH
#define FILE_PATH_MAX_STRING_LENGTH                                         256
#endif

// --------------------------------------------------------------------------------

/**
 * @brief file-context to be used with this file-interface.
 * no manual operation needs to be peformed on this context.
 * 
 */
typedef struct {
    char path[FILE_PATH_MAX_STRING_LENGTH];
    FILE* handle;
    u32 last_file_pointer;
    u32 act_file_pointer;
    u32 timestamp_last_modified;
} FILE_INTERFACE;

// --------------------------------------------------------------------------------

/**
 * @brief Creates a new file-object with the given name.
 * 
 * Usage:
 * 
 *  - Generate the file-object:
 * 
 *     FILE_INTERFACE_CREATE_FILE(MY_FILE)
 * 
 * - set file-path
 * 
 *     MY_FILE_set_path("/home/my_name/my_file.txt");
 * 
 * - create file:
 * 
 *     if (MY_FILE_is_existing() == 0) {
 *         MY_FILE_create();
 *     }
 * 
 * - open file and read line:
 * 
 *     if (MY_FILE_open() == 0) {
 *        ... 
 *     }
 * 
 *     if (MY_FILE_is_readable() == 0) {
 *         ...
 *     }
 * 
 *     char my_line[255];
 *     u16 num_characters = MY_FILE_read_next_line(my_line, sizeof(my_line));
 * 
 *     if (num_characters == 0) {
 *         ...
 *     }
 * 
 *     MY_FILE_close();
 * 
 * - append line at the end of the file
 * 
 *     char my_new_line[] = "this is a new line\\0";
 *     if (MY_FILE_append_line(my_new_line) == 0) {
 *         ...
 *     }
 * 
 */
#define FILE_INTERFACE_CREATE_FILE(file_name)                                       \
                                                                                    \
    static FILE_INTERFACE _##file_name##_context = {                                \
        .handle = NULL,                                                             \
        .last_file_pointer = 0,                                                     \
        .act_file_pointer = 0,                                                      \
        .timestamp_last_modified = 0                                                \
    };                                                                              \
                                                                                    \
    u8 file_name##_open(void) {                                                     \
        return file_open(&_##file_name##_context);                                  \
    }                                                                               \
                                                                                    \
    u8 file_name##_is_open(void) {                                                  \
        return file_is_open(&_##file_name##_context);                               \
    }                                                                               \
                                                                                    \
    u8 file_name##_open_path(const char* p_path) {                                  \
        file_set_path(&_##file_name##_context, p_path);                             \
        return file_open(&_##file_name##_context);                                  \
    }                                                                               \
                                                                                    \
    void file_name##_set_path(const char* p_path) {                                 \
        file_set_path(&_##file_name##_context, p_path);                             \
    }                                                                               \
                                                                                    \
    void file_name##_close(void) {                                                  \
        file_close(&_##file_name##_context);                                        \
    }                                                                               \
                                                                                    \
    u8 file_name##_has_changed(void) {                                              \
        return file_has_changed(&_##file_name##_context);                           \
    }                                                                               \
                                                                                    \
    u8 file_name##_is_existing(void) {                                              \
        return file_is_existing(&_##file_name##_context);                           \
    }                                                                               \
                                                                                    \
    u8 file_name##_is_readable(void) {                                              \
        return file_is_readable(&_##file_name##_context);                           \
    }                                                                               \
                                                                                    \
    u32 file_name##_get_size(void) {                                                \
        return file_get_size(&_##file_name##_context);                              \
    }                                                                               \
                                                                                    \
    u8 file_name##_create(void) {                                                   \
        return file_create(&_##file_name##_context);                                \
    }                                                                               \
                                                                                    \
    u8 file_name##_delete(void) {                                                   \
        return file_delete(&_##file_name##_context);                                \
    }                                                                               \
                                                                                    \
    u8 file_name##_rename(const char* new_name) {                                   \
        return file_rename(&_##file_name##_context, new_name);                      \
    }                                                                               \
                                                                                    \
    i16 file_name##_read_next_line(char* p_line_to, u16 max_length) {               \
        return file_read_next_line(&_##file_name##_context, p_line_to, max_length); \
    }                                                                               \
                                                                                    \
    u16 file_name##_read_line(char* p_line_to, u16 line_number, u16 max_length) {   \
        return file_read_specific_line(                                             \
            &_##file_name##_context,                                                \
            line_number,                                                            \
            p_line_to,                                                              \
            max_length);                                                            \
    }                                                                               \
                                                                                    \
    u16 file_name##_read_bytes(u8* p_buffer, u16 offset, u16 max_length) {          \
        return file_read_bytes(                                                     \
            &_##file_name##_context,                                                \
            offset,                                                                 \
            p_buffer,                                                               \
            max_length);                                                            \
    }                                                                               \
                                                                                    \
    u8 file_name##_append_line(const char* new_line) {                              \
        return file_append_line(&_##file_name##_context, new_line);                 \
    }                                                                               \
                                                                                    \
    const char* file_name##_get_path(void) {                                        \
        return file_get_path(&_##file_name##_context);                              \
    }

// --------------------------------------------------------------------------------

/**
 * @brief Set the path to the file-context.
 * The path must be terminated by '\\0'.
 * The maximum length of the path is FILE_PATH_MAX_STRING_LENGTH
 * 
 * @param p_file context where the path is applied to.
 * @param path The full path to the file to use within this context, \\0-terminated string 
 * @see FILE_PATH_MAX_STRING_LENGTH
 */
void file_set_path(FILE_INTERFACE* p_file, const char* path);

/**
 * @brief Checks if the attributes of the file-context have changed.
 * Attributes that are watched are the last edit date of the file.
 * 
 * @param p_file context of the file to check for modifications
 * @return 1 of the file has changed, otherwise 0
 */
u8 file_has_changed(FILE_INTERFACE* p_file);

/**
 * @brief CHecks if the file of the actual context is existing on the file-system.
 * 
 * @param p_file file-cotnext containing a valid file-path
 * @return 1 if the file is existing, 0 the file is not exisitng of the path is invalid
 */
u8 file_is_existing(FILE_INTERFACE* p_file);

/**
 * @brief Checks if the file of the actual context can be read.
 * 
 * @param p_file context with a valid path to the file too check for read access.
 * @return 1 if the file of the actual context is readable, otherwise 0
 */
u8 file_is_readable(FILE_INTERFACE* p_file);

/**
 * @brief Gets the size in bytes of the actual file-context.
 * If the file isnt exisitng the size is always 0.
 * 
 * @param p_file context with a valid path to the file to get the size from
 * @return file-size in number of bytes, or 0 if the file is not exisitng
 */
u32 file_get_size(FILE_INTERFACE* p_file);

/**
 * @brief Checks if the file of the actual context is opened for read.
 * 
 * @param p_file actual cotnext with a valid file-path
 * @return 1 if the file of the actual cotnext is open for read, otherwise 0.
 */
u8 file_is_open(FILE_INTERFACE* p_file);

/**
 * @brief Creates a file in the full path of the actual file-context.
 * 
 * @param p_file file-cotnext with a valid full-path to the file 
 * @return 1 if the file was created successful, otherwise 0 
 */
u8 file_create(FILE_INTERFACE* p_file);

/**
 * @brief Deletes the file of the actual file-context
 * 
 * @param p_file file-cotnext with a valid path to the file to delete.
 * @return 1 if the file was deleted successful, otheriwse 0.
 */
u8 file_delete(FILE_INTERFACE* p_file);

/**
 * @brief Gets the full path to the file of the actual file-cotnext as string.
 * 
 * @param p_file file-cotnext where the file-path is get from 
 * @return string of to path of the file of the actual file-context.
 */
const char* file_get_path(FILE_INTERFACE* p_file);

/**
 * @brief Renames the file of the actual file-context and updtes the full path.
 * 
 * @param p_old_file file-context with the valid path to the file to rename.
 * @param new_path new full path of the new file name
 * @return 1 if the file was renamed successful, otherwise 0.
 */
u8 file_rename(FILE_INTERFACE* p_old_file, const char* new_path);

/**
 * @brief Opens the file of the actual cotnext for read.
 * This function msut be called before file_read_line() or file_read_specific_line()
 * can be used.
 * 
 * @param p_file file-context with a valid path to the file to open.
 * @return 1 if the file is open for read, otherwise 0.
 */
u8 file_open(FILE_INTERFACE* p_file);

/**
 * @brief Closes a opened file.
 * If the file is not open, nothing happens
 * 
 * @param p_file file-context with an actual opened file.
 */
void file_close(FILE_INTERFACE* p_file);

/**
 * @brief Reads characters from the given file-handle until a new-line character is found or the
 * maximum number of bytes of the destination buffer has reached. The line-fedd character
 * is not copied into the buffer. This function ignores system-characters.
 * 
 * @param file_handle device-handle to use for reading from the file
 * @param p_buffer_to buffer where the read bytes are stored into
 * @param num_max_bytes maximum size of the given buffer p_buffer_to
 * @return number of bytes read or the error-code while access the file.
 * @see file_open()
 */
i16 file_read_line(FILE* file_handle, char* p_buffer_to, u16 num_max_bytes);

/**
 * @brief Reads characters from the given file-handle until a new-line character is found or the
 * maximum number of bytes of the destination buffer has reached. The line-fedd character
 * is not copied into the buffer. This function ignores system-characters.
 * 
 * @param p_file actual file-context with an already opened file.
 * @param next_line buffer where the next line is stored into
 * @param max_length maximum number of characters that can be stored into the given buffer.
 * @return number of bytes read or the error-code while access the file.
 * @see file_open()
 */
i16 file_read_next_line(FILE_INTERFACE* p_file, char* next_line, u16 max_length);

/**
 * @brief Reads the line that is identified by its line number.
 * All other lines of the file are ignored.
 * Reads as much bytes that will fit into the given buffer.
 * The file has to be opened before calling this function.
 * 
 * @param p_file file-cotnext with an already opened file
 * @param line_number the line to read from this file
 * @param next_line buffer where the line is stored into
 * @param max_length maximum number of characters that can stored into the given buffer.
 * @return The number of characters that have been read from the file.
 */
u16 file_read_specific_line(FILE_INTERFACE* p_file, u16 line_number, char* next_line, u16 max_length);

/**
 * @brief Reads at maximum max_length bytes from the currently opened file. The data is read
 * as raw bytes and is stored into p_buffer.
 * If offset is unequal to zero, this amount of bytes will be
 * skipped from the begin of the file.
 * 
 * @param p_file file-cotnext with an already opened file
 * @param offset number of byts to skip
 * @param p_buffer destination where to store the bytes into
 * @param max_length maximum number of bytes that can be stored into p_buffer
 * @return number of bytes that have been read.
 */
u16 file_read_bytes(FILE_INTERFACE* p_file, u16 offset, u8* p_buffer, u16 max_length);

/**
 * @brief Appends a new line at the end of the actual file.
 * The file does not need to be opened before calling this function.
 * The given line does not need to have line endings included.
 * This fucntion will add \\r\\n at the end of the line.
 * 
 * @param p_file file-context with a valid file-name where the line is appended to.
 * @param new_line the new line as a \\0-terminated string
 * @return 1 if the new line was appended successful, otherwise 0
 */
u8 file_append_line(FILE_INTERFACE* p_file, const char* new_line);

// --------------------------------------------------------------------------------

#endif // _H_file_interface_

// --------------------------------------------------------------------------------
