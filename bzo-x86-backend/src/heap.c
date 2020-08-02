#include "stdint.h"
#include "stdlib.h"
#include "structs.h"




Heap 	 newHeap(int capacity){
	Heap ret;
	ret.heap     = malloc(sizeof(uint64_t) * capacity);
	ret.capacity = capacity;
	ret.size     = 0;
	return ret;
}

int      heapSwap  (Heap* h, int a, int b){
	if((a < h->size) && (b < h->size)){
		uint64_t ax = h->heap[a];
		uint64_t bx = h->heap[b];
		if(a > b){
			int i = a;
			b = a;
			a = i;
		}
		if(ax > bx){
			h->heap[a] = bx;
			h->heap[b] = ax;
			return 1;
		}
	}
	return 0;
}

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






