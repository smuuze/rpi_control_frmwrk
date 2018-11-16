
#include "config.h"
#include "local_progmem_load.h"

u8 local_progmem_get_byte(u8* addr) {
	(void)addr;
	return 0;
}

u16 local_progmem_get_word(u8* addr) {
	(void)addr;
	return 0;
}

u32 local_progmem_get_long(u8* addr) {
	(void)addr;
	return 0;
}

void local_progmem_get_N_bytes(u8* addr, u8* p_buffer_to, u8 num_bytes) {
	(void)addr;
	(void)p_buffer_to;
	(void)num_bytes;

}
