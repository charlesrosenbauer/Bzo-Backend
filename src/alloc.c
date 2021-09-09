#include "alloc.h"

#include "stdlib.h"



Stack      makeStack  (){
	Block* blk = malloc(sizeof(Block));
	*blk       = (Block){malloc(33554432), NULL, NULL, 33554432, 0};
	return       (Stack){blk, blk};
}



void*      alloc      (Stack* stk, size_t size){
	Block* top  =  stk->top;
	int    peak =  top->top + size;
	peak        = (peak & 7)? (peak + 8) & ~7l : peak;
	if(top->size <= peak){
		// Grow and replace top
		if(top->next == NULL){
			top->next                = malloc(sizeof(Block));
			*((Block*)top->next)     = (Block){malloc(33554432), NULL, top, 33554432, 0};
		}else{
			((Block*)top->next)->top = 0;
		}
		top       = top->next;
		stk->top  = top;
		peak      = 0;
	}
	void* ret   = &top->buffer[peak];
	top->top    =  peak;
	return ret;
}



void*      allocAlgn  (Stack* stk, size_t size, size_t align){
	int    mask = align - 1;	// Assume popcount(align) == 1
	Block* top  =  stk->top;
	int    peak =  top->top + size;
	peak        = (peak & mask)? (peak + align) & ~mask : peak;
	if(top->size <= peak){
		// Grow and replace top
		if(top->next == NULL){
			top->next                = malloc(sizeof(Block));
			*((Block*)top->next)     = (Block){malloc(33554432), NULL, top, 33554432, 0};
		}else{
			((Block*)top->next)->top = 0;
		}
		top       = top->next;
		stk->top  = top;
		peak      = 0;
	}
	void* ret   = &top->buffer[peak];
	top->top    =  peak;
	return ret;
}



StackState getPosition(Stack* stk){
	StackState ret = (StackState){stk->top, stk->top->top};
	return ret;
}



void       setPosition(Stack* stk, StackState state){
	stk->top      = state.block;
	stk->top->top = state.top;
}
