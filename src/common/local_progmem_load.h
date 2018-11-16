#ifndef _LOCAL_PROGMEM_LOADER_H_
#define _LOCAL_PROGMEM_LOADER_H_

#include "config.h"

u8 local_progmem_get_byte(u8* addr);

u16 local_progmem_get_word(u8* addr);

u32 local_progmem_get_long(u8* addr);

void local_progmem_get_N_bytes(u8* addr, u8* p_buffer_to, u8 num_bytes);

#endif // _LOCAL_PROGMEM_LOADER_H_
