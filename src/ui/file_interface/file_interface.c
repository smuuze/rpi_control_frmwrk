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
 * @file    file_interface.c
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

#include "ui/file_interface/file_interface.h"

// --------------------------------------------------------------------------------

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

// --------------------------------------------------------------------------------

/**
 * @brief file_interface.h#file_set_path
 */
void file_set_path(FILE_INTERFACE* p_file, const char* path) {

    memset(p_file->path, '\0', FILE_PATH_MAX_STRING_LENGTH);
    memcpy(p_file->path, path, strlen(path));
}

/**
 * @brief file_interface.h#file_has_changed
 */
u8 file_has_changed(FILE_INTERFACE* p_file) {

    struct stat file_attribute;
    if (stat(p_file->path, &file_attribute) == 0) {
        return 0;
        
    } else if ((u32)file_attribute.st_mtime > (p_file->timestamp_last_modified) {
        return 1;
    }
    
    return 0;
}

/**
 * @brief file_interface.h#file_get_path
 */
const char* file_get_path(FILE_INTERFACE* p_file) {
    return (const char*)p_file->path;
}

/**
 * @brief file_interface.h#file_is_open
 */
u8 file_is_open(FILE_INTERFACE* p_file) {
    return p_file->handle != NULL ? 1 : 0;
}

/**
 * @brief file_interface.h#file_is_existing
 */
u8 file_is_existing(FILE_INTERFACE* p_file) {

    struct stat file_attribute;
    if (stat(p_file->path, &file_attribute) < 0) {
        return 0;        
    }
    
    return 1;
}

/**
 * @brief file_interface.h#file_is_readable
 */
u8 file_is_readable(FILE_INTERFACE* p_file) {
    (void) p_file;
    return 1;
}

/**
 * @brief file_interface.h#file_get_size
 */
u32 file_get_size(FILE_INTERFACE* p_file) {

    struct stat file_attribute;
    if (stat(p_file->path, &file_attribute) < 0) {
        return 0;        
    }
    
    return file_attribute.st_size;
}

/**
 * @brief file_interface.h#file_delete
 */
u8 file_delete(FILE_INTERFACE* p_file) {
    return (u8)remove(p_file->path) ? 1 : 0;
}

/**
 * @brief file_interface.h#file_rename
 */
u8 file_rename(FILE_INTERFACE* p_old_file, const char* new_path) {
    if (rename(p_old_file->path, new_path)) {
        file_set_path(p_old_file, new_path); 
        return 1;
    } else {
        return 0;
    }
}

/**
 * @brief file_interface.h#file_create
 */
u8 file_create(FILE_INTERFACE* p_file) {
    p_file->handle = fopen((const char*)p_file->path, "w");

    if (p_file->handle != NULL) {
        file_close(p_file);
        return 1;
    } 

    return 0;
}

/**
 * @brief file_interface.h#file_open
 */
u8 file_open(FILE_INTERFACE* p_file) {

    file_close(p_file);

    p_file->handle = fopen((const char*)p_file->path, "r");
    if (p_file->handle == NULL) {
        DEBUG_PASS("file_open() - Open file has FAILED !!! --- (FILE: %s / ERROR: %d)");//, p_file->path,  EXIT_FAILURE);
        return 0;
    }

    //DEBUG_PASS("file_open() - FILE: %s\n", p_file->path);
    return 1;
}

/**
 * @brief file_interface.h#file_close
 */
void file_close(FILE_INTERFACE* p_file) {

    if (p_file->handle == NULL) {
        return;
    }

    //DEBUG_PASS("file_close() - FILE: %s\n", p_file->path);
    fclose(p_file->handle);
    p_file->handle = NULL;
}

/**
 * @brief file_interface.h#file_read_line
 */
i16 file_read_line(FILE* file_handle, char* p_buffer_to, u16 num_max_bytes) {
    
    if (num_max_bytes == 0) {
        return 0;
    }
    

    if ( feof(file_handle) ) {
        return -1;
    }

    char character;
    u16 num_bytes_read = 0;
    
    //DEBUG_PASS("read_line() - Line: ");
    
    while ((unsigned char)(character = getc(file_handle)) != 255) {
        
        if (num_bytes_read == num_max_bytes - 1) {
            break;
        }
        
        if (character == '\n') {
            //DEBUG_PASS("----> End of line reached (LF)\n");
            break;
        }    
        
        if ((character  < 32 || (unsigned char)character > 254)) {        
            //DEBUG_PASS("----> Character is not supported (%d)\n", character);
            continue;
        }
        
        //DEBUG_PASS("%d ", character);        
        p_buffer_to[num_bytes_read++] = character;
    }
    
    //DEBUG_PASS("\n");
    
    p_buffer_to[num_bytes_read] = '\0';
    return num_bytes_read;
}

/**
 * @brief file_interface.h#file_read_next_line
 */
i16 file_read_next_line(FILE_INTERFACE* p_file, char* next_line, u16 max_length) {
    
    if (p_file->handle == NULL) {
        DEBUG_PASS("file_read_next_line() - File is not open !!! --- (FILE: %s)");//, p_file->path);
        return 0;
    }
    
    u16 num_bytes = file_read_line(p_file->handle, next_line, max_length);

    //FILE_DEBUG_MSG("file_read_next_line() - FILE: %s - LINE: %s\n", p_file->path, next_line);
    return num_bytes;
}

/**
 * @brief file_interface.h#file_read_specific_line
 */
u16 file_read_specific_line(FILE_INTERFACE* p_file, u16 line_number, char* next_line, u16 max_length) {
    
    if (line_number == 0) {
        DEBUG_PASS("file_read_specific_line() - Invalid line-number given !!!");
        return 0;
    }

    if (p_file->handle == NULL) {
        DEBUG_PASS("file_read_next_line() - File is not open !!! --- (FILE: %s)");//, p_file->path);
        return 0;
    }
    
    u16 num_bytes_line = 0;

    do {
        num_bytes_line = file_read_line(p_file->handle, next_line, max_length);
    } while (--line_number != 0);

    return num_bytes_line;
}

/**
 * @brief file_interface.h#file_read_bytes
 */
u16 file_read_bytes(FILE_INTERFACE* p_file, u16 offset, u8* p_buffer, u16 max_length) {
    
    if (max_length == 0) {
        return 0;
    }
    

    if ( feof(p_file->handle) ) {
        return 0;
    }

    DEBUG_TRACE_word(offset, "file_read_bytes() - OFFSET:");
    DEBUG_TRACE_word(max_length, "file_read_bytes() - MAX_LENGTH:");

    fseek(p_file->handle, offset, SEEK_SET);

    u16 num_bytes_read = 0;
    int return_value = 0;
    
    //DEBUG_PASS("read_line() - Line: ");
    
    while ( (return_value = getc(p_file->handle)) != -1 ) {

        if (num_bytes_read >= max_length) {
            break;
        }

        p_buffer[num_bytes_read++] = (u8)(return_value);
    }
    
    return num_bytes_read;
}

/**
 * @brief file_interface.h#file_append_line
 */
u8 file_append_line(FILE_INTERFACE* p_file, const char* new_line) {

    file_close(p_file);
    
    p_file->handle = fopen((const char*)p_file->path, "a");
    if (p_file->handle == NULL) {
        DEBUG_PASS("file_append_line() - Open file has FAILED !!! --- (FILE: %s)");//, p_file->path);
        return 0;
    }

    int err_code = fprintf(p_file->handle, "%s\r\n", new_line);
    fclose(p_file->handle);
    p_file->handle = NULL;

    if (err_code < 0) {
        DEBUG_PASS("file_append_line() - Writing File has FAILED !!! --- (FILE: %s)");//, p_file->path);
        return 0;
    } else {
        return 1;
    }

}

// --------------------------------------------------------------------------------
