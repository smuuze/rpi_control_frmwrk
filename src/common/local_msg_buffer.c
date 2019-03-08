

#include "config.h"  // immer als erstes einbinden!
#include "specific.h"
#include "hmsrc/config_f.h"   // Default-Configuration nach config.h einbinden

#include "utils/stdmacros.h"

#include "local_msg_buffer.h"

//---------- Implementation of Traces -----------------------------------------

//#define TRACES
//#include <traces.h>

#define TRACER_OFF
#include "tracer.h"


void local_msg_buffer_init(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr) {

	p_buffer_descr->status_flags = 0;
	local_msg_buffer_clear_all(p_buffer_descr);
}


u8 local_msg_buffer_start_write(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr) {

	if ((p_buffer_descr->status_flags & MSG_BUFFER_STATUS_FLAG_WRITE_ACTIVE) != 0) {

		p_buffer_descr->status_flags |= MSG_BUFFER_STATUS_FLAG_WRITE_ACTIVE;
		return 1;

	} else {

		return 0;
	}
}


void local_msg_buffer_add_byte(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr, u8 byte) {

	if ((p_buffer_descr->status_flags & MSG_BUFFER_STATUS_FLAG_WRITE_ACTIVE) == 0) {
		//return;
	}

	if (p_buffer_descr->num_elements == p_buffer_descr->size) {
		PASS(); // local_msg_buffer_add_byte() - Buffer Overflow !!!
		return;
	}

	//ATOMIC_BLOCK(ATOMIC_FORCEON) {
		p_buffer_descr->p_buffer[p_buffer_descr->write_pointer] = byte;
		p_buffer_descr->write_pointer += 1;
		p_buffer_descr->num_elements += 1;

		if (p_buffer_descr->write_pointer == p_buffer_descr->size) {
			PASS(); // local_msg_buffer_add_byte() - Write-Ring Closed
			p_buffer_descr->write_pointer = 0;
		}
	//}
}


void local_msg_buffer_add_word(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr, u16 word) {

	local_msg_buffer_add_byte(p_buffer_descr, (u8)(word));
	local_msg_buffer_add_byte(p_buffer_descr, (u8)(word >> 8));
}


void local_msg_buffer_add_long(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr, u32 long_integer) {

	local_msg_buffer_add_byte(p_buffer_descr, (u8)(long_integer));
	local_msg_buffer_add_byte(p_buffer_descr, (u8)(long_integer >> 8));
	local_msg_buffer_add_byte(p_buffer_descr, (u8)(long_integer >> 16));
	local_msg_buffer_add_byte(p_buffer_descr, (u8)(long_integer >> 24));
}


void local_msg_buffer_add_N_bytes(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr, u16 num_bytes, const u8* p_ext_buffer) {

	u16 i = 0;
	for ( ; i < num_bytes; i++) {
		local_msg_buffer_add_byte(p_buffer_descr, p_ext_buffer[i]);
	}
}


void local_msg_buffer_stop_write(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr) {
	p_buffer_descr->status_flags &= ~MSG_BUFFER_STATUS_FLAG_WRITE_ACTIVE;
}


u8 local_msg_buffer_start_read(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr) {
	if ((p_buffer_descr->status_flags & MSG_BUFFER_STATUS_FLAG_READ_ACTIVE) != 0) {
		p_buffer_descr->status_flags |= MSG_BUFFER_STATUS_FLAG_READ_ACTIVE;
		return 1;
	} else {
		return 0;
	}
}


u16 local_msg_buffer_bytes_available(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr) {

	//TRACE_word(p_buffer_descr->num_elements); // local_msg_buffer_bytes_available()
	return p_buffer_descr->num_elements;
}


u16 local_msg_buffer_bytes_free(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr) {
	return (p_buffer_descr->size - local_msg_buffer_bytes_available(p_buffer_descr));
}


u8 local_msg_buffer_get_byte(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr) {

	if ((p_buffer_descr->status_flags & MSG_BUFFER_STATUS_FLAG_READ_ACTIVE) == 0) {
		//return LOCAL_MSG_BUFFER_INVALID_CHARACTER;
	}

	if (p_buffer_descr->num_elements == 0) {
		PASS(); // local_msg_buffer_get_byte() - Buffer Underrun !!!
		return LOCAL_MSG_BUFFER_INVALID_CHARACTER;
	}

	u8 element = LOCAL_MSG_BUFFER_INVALID_CHARACTER ;

	//ATOMIC_BLOCK(ATOMIC_FORCEON) {
		element = p_buffer_descr->p_buffer[p_buffer_descr->read_pointer];
		p_buffer_descr->read_pointer += 1;
		p_buffer_descr->num_elements -= 1;

		if (p_buffer_descr->read_pointer == p_buffer_descr->size) {
			p_buffer_descr->read_pointer = 0;
			PASS(); // local_msg_buffer_get_byte() - Buffer Read-Ring Closed !!!
		}
	//}

	return element;
}


u16 local_msg_buffer_get_word(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr) {

	u16 word  = (u16)local_msg_buffer_get_byte(p_buffer_descr);
	    word |= ((u16)(local_msg_buffer_get_byte(p_buffer_descr)) << 8);

	return word;
}


u32 local_msg_buffer_get_long(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr) {

	u32 long_integer = (u32)local_msg_buffer_get_byte(p_buffer_descr);
	long_integer    |= ((u32)(local_msg_buffer_get_byte(p_buffer_descr)) << 8);
	long_integer    |= ((u32)(local_msg_buffer_get_byte(p_buffer_descr)) << 16);
	long_integer    |= ((u32)(local_msg_buffer_get_byte(p_buffer_descr)) << 24);

	return long_integer;
}

u16 local_msg_buffer_get_N_bytes(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr, u16 num_bytes, u8* p_buffer_to) {

	u16 i = 0;
	for ( ; i < num_bytes; i++) {

		if (p_buffer_descr->num_elements == 0) {
			break;
		}

		p_buffer_to[i] = local_msg_buffer_get_byte(p_buffer_descr);
	}

	return i;
}


void local_msg_buffer_stop_read(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr) {
	p_buffer_descr->status_flags &= ~MSG_BUFFER_STATUS_FLAG_READ_ACTIVE;
}


void local_msg_buffer_clear_all(LOCAL_MSG_BUFFER_DESCR* p_buffer_descr) {

	p_buffer_descr->read_pointer = 0;
	p_buffer_descr->write_pointer = 0;
	p_buffer_descr->num_elements = 0;
}
