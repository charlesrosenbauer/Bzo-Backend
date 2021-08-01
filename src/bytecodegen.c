#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "bytecodegen.h"
#include "program.h"
#include "ast.h"



int appendBCBlk(BytecodeFunction* fn, BCBlock blk){
	if(fn->blkct+1 >= fn->blkcap){
		BCBlock* tmp = fn->blks;
		fn->blks     = malloc(sizeof(BCBlock) * fn->blkcap * 2);
		for(int i = 0; i < fn->blkct; i++) fn->blks[i] = tmp[i];
		fn->blkcap  *= 2;
		free(tmp);
	}
	fn->blks[fn->blkct] = blk;
	fn->blkct++;
	return fn->blkct-1;
}

int appendBCVar(BytecodeFunction* fn, BCVariable var){
	if(fn->varct+1 >= fn->varcap){
		BCVariable* tmp = fn->vars;
		fn->vars        = malloc(sizeof(BCVariable) * fn->varcap * 2);
		for(int i = 0; i < fn->varct; i++) fn->vars[i] = tmp[i];
		fn->varcap     *= 2;
		free(tmp);
	}
	fn->vars[fn->varct] = var;
	fn->varct++;
	return fn->varct-1;
}


BytecodeFunction makeBCFn(int vs, int bs){
	BytecodeFunction ret = (BytecodeFunction){NULL, NULL, 0, vs, 0, bs};
	ret.vars = malloc(sizeof(BCVariable) * vs);
	ret.blks = malloc(sizeof(BCBlock   ) * bs);
	return ret;
}


BytecodeFunction buildFunction(SymbolTable* tab, ASTFnDef* fndef){
	BytecodeFunction ret = makeBCFn(8, 4);
	
	
	
	return ret;
}



void fmtOpcode(Opcode op, BytecodeType ty, char* buffer){
	char* ops;
	int   b = 8, v = 1;
	switch(op){
		case BC_ADD  : ops = "ADD  "; break;
		case BC_SUB  : ops = "SUB  "; break;
		case BC_MUL  : ops = "MUL  "; break;
		case BC_DIV  : ops = "DIV  "; break;
		case BC_MOD  : ops = "MOD  "; break;
		default: ops = "???  "; break;
	}
	b = 1 << ((ty & 0x0f) +  3);
	v = 1 << ((ty & 0xf0) >> 4);
	sprintf(buffer, "%s %ix%i ", ops, b, v);
}

void printBCFn(BytecodeFunction fn){
	for(int i = 0; i < fn.blkct; i++){
		printf("BLK #%i:\n", i);
		for(int j = 0; j < fn.blks[i].size; j++){
			Bytecode bc = fn.blks[i].code[j];
			char buffer[32];
			fmtOpcode(bc.opc, bc.size, buffer);
			printf("  %s [%lu] %i %i > %i\n", buffer, bc.imm, bc.a, bc.b, bc.c);
		}
	}
}


