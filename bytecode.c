#include "stdlib.h"

#include "bytecode.h"







CodeBlock makeCodeBlock  (int opsize, int varsize){
	CodeBlock ret;
	ret.ops      = malloc(sizeof(IR_Instruction) * opsize);
	ret.opSize   = 0;
	ret.opCap    = opsize;

	ret.varTyps  = malloc(sizeof(IR_Type) * varsize);
	ret.vtSize   = 0;
	ret.vtCap    = varsize;

	return ret;
}


void      resizeCodeBlock(CodeBlock* block, int opsize, int varsize){

}


/*
	Take a block, remove all nops and dead variables.

	Basically cleanup after dead code elimination.
*/
CodeBlock denopCodeBlock (CodeBlock block){
	CodeBlock ret;


	return ret;
}