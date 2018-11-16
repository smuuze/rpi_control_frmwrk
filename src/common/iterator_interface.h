#ifndef _INTERATOR_INTERFACE_H_
#define _INTERATOR_INTERFACE_H_

/*!
 *
 */
typedef struct ITERATOR_INTERFACE {
	void* __element;
	u8 is_first;
	u8 is_last;
	u8 is_valid;
} ITERATOR_INTERFACE;

#endif // _INTERATOR_INTERFACE_H_
