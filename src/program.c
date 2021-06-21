#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "program.h"
#include "error.h"
#include "util.h"
#include "ast.h"


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



void printASTProgram(ASTProgram prog){
	printf("HEADS=\n");
	for(int i = 0; i < prog.hdct; i++){ printf("  HD%i | %i %i %s\n", i, prog.hds[i].bid, prog.hds[i].str.len, prog.hds[i].str.text); }
	printf("FUNCS=\n");
	for(int i = 0; i < prog.fnct; i++)	printf("  FN%i | %i\n", i, prog.fns[i].fnid);
	printf("TYPES=\n");
	for(int i = 0; i < prog.tyct; i++)	printf("  TY%i | %i\n", i, prog.tys[i].tyid);
}



