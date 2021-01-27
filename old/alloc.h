#ifndef __ALLOC_HEADER__
#define __ALLOC_HEADER__


#include "stdint.h"


/*
	This is an allocator for temporary data structures.
	
	Allocating using this code should probably be faster than malloc,
	and allows for worry-free allocation of complex data structures
	in cases where their use is only temporary.
*/

typedef struct{
	uint8_t* block;
	int size, fill;
	void*    next;
	void*    head;
}Allocator;


Allocator makeAllocator(int);
void*     allocf       (Allocator*, int);
//void*     allocalign   (Allocator*, int, int);
void      freeAlloc    (Allocator*);











#endif
