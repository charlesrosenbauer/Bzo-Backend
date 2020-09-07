#include "stdlib.h"
#include "stdio.h"

#include "bytecode.h"




char* getOpcodeName(IR_Opcode op){
	switch(op){
		case IR_NOP		: return "NOP";
		case IR_ADD		: return "ADD";
		case IR_SUB		: return "SUB";
		case IR_AND		: return "AND";
		case IR_OR 		: return "OR";
		case IR_XOR		: return "XOR";

		case IR_ARGS    : return "ARGS";
		case IR_RETS    : return "RETS";
		case IR_CALL    : return "CALL";
		case IR_IF      : return "IF";
		case IR_IFE     : return "IFE";

		case IR_CONST   : return "CONST";
	}

	return "???";
}




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
	if(opsize > block->opCap){
		IR_Instruction* tmp = block->ops;
		block->ops          = malloc(sizeof(IR_Instruction) * opsize);
		block->opCap        = opsize;
		for(int i = 0; i < block->opSize; i++) block->ops[i] = tmp[i];
		free(tmp);
	}
	if(varsize > block->vtCap){
		IR_Type* tmp        = block->varTyps;
		block->varTyps      = malloc(sizeof(IR_Type) * varsize);
		block->vtCap        = varsize;
		for(int i = 0; i < block->vtSize; i++) block->varTyps[i] = tmp[i];
		free(tmp);
	}
}


void printCodeBlock(CodeBlock block){
	printf("BLOCK. inputs: 0-%i, outputs: %i-%i\n", block.pars, block.retix, block.rets + block.retix);
	for(int i = 0; i < block.opSize; i++){
		IR_Instruction op = block.ops[i];
		if(op.opc != IR_CONST){
			printf("  %i %i := %s (%i, %i, %i)\n", op.pars.q, op.pars.r, getOpcodeName(op.opc), op.pars.a, op.pars.b, op.pars.c);
		}else{
			printf("  %i := CONST (%lu)\n", op.imm.ret, op.imm.x);
		}
	}
}


/*
	Take a block, remove all nops and dead variables.

	Basically cleanup after dead code elimination.
*/
CodeBlock denopCodeBlock (CodeBlock block){
	CodeBlock ret;


	return ret;
}