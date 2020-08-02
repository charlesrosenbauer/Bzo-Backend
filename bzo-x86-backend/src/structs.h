#ifndef __STRUCTS_HEADER__
#define __STRUCTS_HEADER__


#include "stdint.h"


typedef struct{
	uint64_t*  heap;
	int        size, capacity;
}Heap;


Heap     newHeap   (int);
void     heapInsert(Heap*, uint64_t);
uint64_t heapRemove(Heap*);
void     heapSwap  (Heap*, int, int);
void     heapGrow  (Heap*);
int      heapParent(int);
int      heapLChild(int);
int      heapRChild(int);





#endif