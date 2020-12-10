#include "alloc.h"

#include "stdlib.h"



Allocator makeAllocator(int size){
	Allocator ret;
	ret.block = malloc(size);
	ret.size  = size;
	ret.fill  = 0;
	ret.next  = NULL;
	ret.head  = NULL;
	return ret;
}

void* allocf    (Allocator* a, int size){
	Allocator* first = a;
	if(a->head != NULL)	a = a->head;
	
	if((a->fill + size) > a->size){
		
		if(size > a->size) return NULL;		// Can't really allocate this
	
		Allocator* newHead = malloc(sizeof(Allocator));
		*newHead    = makeAllocator(a->size);
		first->head = newHead;
		a->next     = newHead;
		a           = newHead;
	}
	
	int pos = a->fill;
	a->fill += size;
	return &a->block[pos];
}

/*
// I'll handle this case when it's needed
void* allocalign(Allocator*, size_t, size_t){

}*/

void  freeAlloc (Allocator* a){
	while(a != NULL){
		Allocator* here = a;
		a = a->next;
		free(here);
	}
}
