#include "util.h"
#include "ir.h"
#include "bytecode.h"

#include "stdlib.h"
#include "stdio.h"


int countCodeVars(Lisp* code){
	int count = 0;
	while(code != NULL){
		int here = 0;
		if     (code->here.typ == LSPTYP) here = countCodeVars(code->here.val.PVAL);
		else if(code->here.typ == VARTYP) here = code->here.val.UVAL+1;
		count = (here > count)? here : count;
		code  = code->next;
	}
	return count;
}


int buildFunction(Program* prog, FnDef* fn, BCProgram* bc){
	int varct = countCodeVars(fn->codeSource);
	CodeBlock fnheader = makeCodeBlock(BKT_FUNCTION_HEAD, varct*2, varct, fn->prct, fn->rtct);
	
	printf("CODEBLOCK : %i %i %i\n", fn->prct, fn->rtct, varct);
	
	return 0;
}



int buildBytecode(Program* prog, BCProgram* bc){
	
	for(int i = 0; i < prog->fnct; i++){
		
		// For each function:
		// 1. Map variables within scope
		// 2. 
		if(prog->funcs[i].fnid == i){
			buildFunction(prog, &prog->funcs[i], bc);
		}
	}
	
	return 0;
}
