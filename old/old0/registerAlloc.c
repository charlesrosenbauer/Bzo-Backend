#include "x86.h"
#include "struct.h"

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"



typedef struct{
	Set32 pars, chds, live, defs, refs;
}BlockData;

BlockData newBlockData(){
	BlockData ret;
	ret.pars = initSet32(4);
	ret.chds = initSet32(8);
	ret.live = initSet32(64);
	ret.defs = initSet32(64);
	ret.refs = initSet32(64);
	return ret;
}

void freeBlockData(BlockData* ds, int ct){
	for(int i = 0; i < ct; i++){
		free(ds[i].pars.vals);
		free(ds[i].chds.vals);
		free(ds[i].live.vals);
		free(ds[i].defs.vals);
		free(ds[i].refs.vals);
	}
	free(ds);
}

int allocRegisters(X86Function* fn){
	
	BlockData* bds = malloc(sizeof(BlockData) * fn->bct);
	for(int i = 0; i < fn->bct; i++) bds[i] = newBlockData();
	
	/*
		TODO:
		1. Figure out parents and children of each block
		2. Mark definitions and references
		3. Mark liveness via flow
		4. ???
		5. Allocate registers
	*/
	
	// Parents and Children
	for(int i = 0; i < fn->bct; i++){
		X86Block* blk = &fn->blocks[i];
		
		insertSet32(&bds[i].chds, blk->nextBlock);
		if(blk->nextBlock > 0) insertSet32(&bds[blk->nextBlock].pars, i);
		for(int j = 0; j < blk->opct; j++){
			switch(blk->ops[j].opc){
				case X86_JCC:
				case X86_JMP:
				case X86_CMP_JMP: {
					int64_t imm = *((int64_t*)&blk->ops[j].immediate);
					insertSet32(&bds[i  ].chds, imm);
					insertSet32(&bds[imm].pars, i  );
				} break;
				
				default: break;
			}
		}
	}
	
	for(int i = 0; i < fn->bct; i++){
		printf("Block %i\n", i);
		printf("Parents:\n");
		printSet32(bds[i].pars);
		printf("Children:\n");
		printSet32(bds[i].chds);
	}
	
	return 0;
}
