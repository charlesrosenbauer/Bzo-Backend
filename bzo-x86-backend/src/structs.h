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

void     printHeap (Heap*);
int      testHeap  ();



typedef struct{
	int64_t*   data;
	int        size, capacity;
}DynArr;


DynArr newDynArr   (int);
void   growDynArr  (DynArr*, int);
int    appendDynArr(DynArr*, int64_t);
void   resizeDynArr(DynArr*, int);
void   insertDynArr(DynArr*, int64_t, int);


#endif