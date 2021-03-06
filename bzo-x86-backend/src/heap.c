#include "stdint.h"
#include "stdlib.h"
#include "structs.h"
#include "stdio.h"



Heap 	 newHeap(int capacity){
	Heap ret;
	ret.heap     = malloc(sizeof(uint64_t) * capacity);
	ret.capacity = capacity;
	ret.size     = 0;
	return ret;
}


void     heapSwap  (Heap* h, int a, int b){
	if((a < h->size) && (b < h->size)){
		uint64_t tmp = h->heap[a];
		h->heap[a] = h->heap[b];
		h->heap[b] = tmp;
	}
}

int      heapBublUp(Heap* h, int a, int b){
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
	//printf("HEAP GROW : %i -> %i\n", h->capacity, h->capacity * 2);
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



void     heapInsert(Heap* h, uint64_t x){
	if((h->size+1) >= h->capacity) heapGrow(h);

	h->heap[h->size] = x;
	h->size++;

	int ix   = h->size-1;
	int cont = 1;
	while(cont){
		int p = heapParent(ix);
		cont  = heapBublUp(h, p, ix) & (p != 0);
		ix    = p;
	}
}

uint64_t heapRemove(Heap* h){
	if(h->size <= 0) return 0;

	uint64_t ret = h->heap[0];
	h->heap[0] = h->heap[h->size-1];
	h->size--;

	int ix   = 0;
	int cont = 1;
	while(cont){
		int l = heapLChild(ix);
		int r = heapRChild(ix);
		uint64_t min;
		int      mix;
		if(l < h->size){
			min = h->heap[l];
			mix = l;
			if((r < h->size) && (h->heap[r] < min)){
				min = h->heap[r];
				mix = r;
			}
			if(h->heap[ix] < min){
				cont = 0;
			}else{
				heapSwap(h, mix, ix);
				ix = mix;
			}
		}else{
			cont = 0;
		}
	}
	return ret;
}


void printHeap(Heap* h){
	for(int i = 0; i < h->size; i++)
		printf("%lu\n", h->heap[i]);
}


int testHeap(){
	const int TESTSIZE = 65536;

	Heap h = newHeap(128);
	for(int i = 0; i < TESTSIZE; i++){
		// Was doing stuff with coprime multiplication, but there were bugs when TESTSIZE > 32768
		int x = (i + 13513) % TESTSIZE;
		heapInsert(&h, x);
	}
	int ret = 1;
	//printf("====\n");
	for(int i = 0; i < (TESTSIZE+1); i++){
		int x = heapRemove(&h);
		if(i < TESTSIZE){
			if(x != i){
				ret = 0;
				i   = (TESTSIZE+1);
				break;
			}
		}else{
			if(x != 0){
				ret = 0;
				i   = (TESTSIZE+1);
				break;
			}
		}
		//printf("%i : %lu\n", i, x);
	}
	free(h.heap);
	return ret;
}