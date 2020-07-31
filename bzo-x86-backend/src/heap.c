#include "stdint.h"
#include "stdlib.h"
#include "structs.h"





void     heapGrow  (Heap* h){
	h->capacity *= 2;
	uint64_t* heap = malloc(sizeof(uint64_t) * h->capacity);
	for(int i = 0; i < h->size; i++) heap[i] = h->heap[i];
	free(h->heap);
	h->heap = heap;
}

int      heapParent(int i){
	return (i-1) / 2;
}

int      heapLChild(int i){
	return (i*2) + 1;
}

int      heapRChild(int i){
	return (i*2) + 2;
}