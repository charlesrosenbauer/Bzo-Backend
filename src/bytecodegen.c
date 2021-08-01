#include "stdint.h"
#include "stdlib.h"

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




