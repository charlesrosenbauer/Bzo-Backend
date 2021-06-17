#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "program.h"
#include "error.h"
#include "util.h"


ASTProgram makeASTProgram(int size){
	ASTProgram ret;
	ret.hds   = malloc(sizeof(ASTHeader) * size);
	ret.fns   = malloc(sizeof(ASTFnDef ) * size);
	ret.tys   = malloc(sizeof(ASTTyDef ) * size);
	ret.fnct  = 0;
	ret.tyct  = 0;
	ret.hdct  = 0;
	ret.fncap = size;
	ret.tycap = size;
	ret.hdcap = size;
	return ret;
}

void appendFnDef(ASTProgram* prog, ASTFnDef fn){
	if(prog->fnct+1 > prog->fncap){
		ASTFnDef* tmp = prog->fns;
		prog->fns     = malloc(sizeof(ASTFnDef) * prog->fncap * 2);
		for(int i = 0; i < prog->fnct; i++) prog->fns[i] = tmp[i];
		prog->fncap  *= 2;
		free(tmp);
	}
	prog->fns[prog->fnct] = fn;
	prog->fnct++;
}


void appendTyDef(ASTProgram* prog, ASTTyDef ty){
	if(prog->tyct+1 > prog->fncap){
		ASTTyDef* tmp = prog->tys;
		prog->tys     = malloc(sizeof(ASTTyDef) * prog->tycap * 2);
		for(int i = 0; i < prog->tyct; i++) prog->tys[i] = tmp[i];
		prog->tycap  *= 2;
		free(tmp);
	}
	prog->tys[prog->tyct] = ty;
	prog->tyct++;
}


void appendHeader(ASTProgram* prog, ASTHeader hd){
	if(prog->hdct+1 > prog->hdcap){
		ASTHeader* tmp = prog->hds;
		prog->hds     = malloc(sizeof(ASTHeader) * prog->hdcap * 2);
		for(int i = 0; i < prog->hdct; i++) prog->hds[i] = tmp[i];
		prog->hdcap  *= 2;
		free(tmp);
	}
	prog->hds[prog->hdct] = hd;
	prog->hdct++;
}




/*
AllocatorAST makeAlloc(int size){
	AllocatorAST ret;
	ret.size   = size;
	ret.fill   = 0;
	ret.buffer = malloc(sizeof(uint8_t) * size);
	ret.next   = NULL;
	return ret;
}

void* allocate(AllocatorAST* a, int size, int align){
	AllocatorAST*  here = a;
	AllocatorAST** last = NULL;
	while(here != NULL){
		int off =  here->fill & (align-1);
		int ix  =  here->fill;
		if(off){
			ix +=   align;
			ix &= ~(align-1);
		}
		if((ix + size) < here->size){
			here->fill = ix+size;
			return &here->buffer[ix];
		}else{
			last = (AllocatorAST**)&here->next;
			here =                  here->next;
		}
	}
	*last  = malloc(sizeof(AllocatorAST));
	**last = makeAlloc(16384);
	return allocate(*last, size, align);
}

void freeAllocator(AllocatorAST* a){
	if(a == NULL) return;
	free(a->buffer);
	freeAllocator(a->next);
}

ASTProgram makeASTProgram(int defct){
	ASTProgram ret;
	ret.fns   = malloc(sizeof(ASTFnDef) * defct);
	ret.tys   = malloc(sizeof(ASTTyDef) * defct);	
	ret.fncap = defct;
	ret.tycap = defct;
	ret.fnct  = 0;
	ret.tyct  = 0;
	ret.alloc = makeAlloc(16384);
	return ret;
}*/
