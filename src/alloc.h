#ifndef __ALLOC_HEADER__
#define __ALLOC_HEADER__


#include "stdint.h"
#include "stdlib.h"


typedef struct{
	uint8_t* buffer;
	void    *next, *last;
	int      size,  top;
}Block;


typedef struct{
	Block   *base, *top;
}Stack;


typedef struct{
	Block*  block;
	int     top;
}StackState;


Stack      makeStack  ();
void*      alloc      (Stack*, size_t);
void*      allocAlgn  (Stack*, size_t, size_t);
StackState getPosition(Stack*);
void       setPosition(Stack*, StackState);



#endif
