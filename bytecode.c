#include "stdlib.h"
#include "stdio.h"

#include "bytecode.h"




char* getOpcodeName(IR_Opcode op){
	switch(op){
		case IR_NOP		 : return "NOP";
		case IR_ADD		 : return "ADD";
		case IR_SUB		 : return "SUB";
		case IR_AND		 : return "AND";
		case IR_OR 		 : return "OR";
		case IR_XOR		 : return "XOR";

		case IR_CAST_I8  : return "CAST-I8";
		case IR_CAST_I16 : return "CAST-I16";
		case IR_CAST_I32 : return "CAST-I32";
		case IR_CAST_I64 : return "CAST-I64";
		case IR_CAST_U8  : return "CAST-U8";
		case IR_CAST_U16 : return "CAST-U16";
		case IR_CAST_U32 : return "CAST-U32";
		case IR_CAST_U64 : return "CAST-U64";

		case IR_ARGS     : return "ARGS";
		case IR_RETS     : return "RETS";
		case IR_CALL     : return "CALL";
		case IR_IF       : return "IF";
		case IR_IFE      : return "IFE";

		case IR_CONST    : return "CONST";
	}

	return "???";
}


char* getTypeName(IR_PType ty){
	switch(ty){
		case IRP_NIL    : return "NIL";

		case IRP_I8     : return "I8" ;
		case IRP_I16	: return "I16";
		case IRP_I32	: return "I32";
		case IRP_I64	: return "I64";

		case IRP_U8     : return "U8" ;
		case IRP_U16	: return "U16";
		case IRP_U32	: return "U32";
		case IRP_U64	: return "U64";
	}

	return "???";
}


void getTypeSignature(IR_Instruction opc, IR_Type* a, IR_Type* b, IR_Type* c, IR_Type* q, IR_Type* r){

	IR_Type nil = (IR_Type){IRP_NIL};
	IR_Type i8  = (IR_Type){IRP_I8 };
	IR_Type i16 = (IR_Type){IRP_I16};
	IR_Type i32 = (IR_Type){IRP_I32};
	IR_Type i64 = (IR_Type){IRP_I64};
	IR_Type u8  = (IR_Type){IRP_U8 };
	IR_Type u16 = (IR_Type){IRP_U16};
	IR_Type u32 = (IR_Type){IRP_U32};
	IR_Type u64 = (IR_Type){IRP_U64};

	IR_Type typ = opc.type;

	switch(opc.opc){
		case IR_NOP		: {*a = nil; *b = nil; *c = nil; *q = nil; *r = nil; return;}

		case IR_ADD     : {*a = typ; *b = typ; *c = nil; *q = typ; *r = nil; return;}
		case IR_SUB     : {*a = typ; *b = typ; *c = nil; *q = typ; *r = nil; return;}
		case IR_AND     : {*a = typ; *b = typ; *c = nil; *q = typ; *r = nil; return;}
		case IR_OR      : {*a = typ; *b = typ; *c = nil; *q = typ; *r = nil; return;}
		case IR_XOR     : {*a = typ; *b = typ; *c = nil; *q = typ; *r = nil; return;}

		case IR_CAST_I8 : {*a = typ; *b = nil; *c = nil; *q = i8 ; *r = nil; return;}
		case IR_CAST_I16: {*a = typ; *b = nil; *c = nil; *q = i16; *r = nil; return;}
		case IR_CAST_I32: {*a = typ; *b = nil; *c = nil; *q = i32; *r = nil; return;}
		case IR_CAST_I64: {*a = typ; *b = nil; *c = nil; *q = i64; *r = nil; return;}
		case IR_CAST_U8 : {*a = typ; *b = nil; *c = nil; *q = u8 ; *r = nil; return;}
		case IR_CAST_U16: {*a = typ; *b = nil; *c = nil; *q = u16; *r = nil; return;}
		case IR_CAST_U32: {*a = typ; *b = nil; *c = nil; *q = u32; *r = nil; return;}
		case IR_CAST_U64: {*a = typ; *b = nil; *c = nil; *q = u64; *r = nil; return;}
	}

	// Default: all nil
	*a = nil; *b = nil; *c = nil; *q = nil; *r = nil;
}




CodeBlock makeCodeBlock  (BlockType t, int opsize, int varsize, int pars, int rets){
	CodeBlock ret;
	ret.ops      = malloc(sizeof(IR_Instruction) * opsize);
	ret.opSize   = 0;
	ret.opCap    = opsize;
	for(int i = 0; i < opsize;  i++) ret.ops    [i] = (IR_Instruction){IR_NOP, IRP_NIL, (IR_Pars){0, 0, 0, 0, 0}};

	ret.varTyps  = malloc(sizeof(IR_Type) * varsize);
	ret.vtSize   = 0;
	ret.vtCap    = varsize;
	for(int i = 0; i < varsize; i++) ret.varTyps[i] = (IR_Type       ){IRP_NIL};

	ret.pars     = pars;
	ret.rets     = rets;
	ret.btype    = t;

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
	char* blockType = "";
	switch(block.btype){
		case BKT_FUNCTION_HEAD: blockType = "FUNCTION HEAD"; break;
		case BKT_LOOP_BODY    : blockType = "LOOP BODY";     break;
		case BKT_FORK         : blockType = "FORK";			 break;
		case BKT_LAMBDA		  : blockType = "LAMBDA";		 break;
	}

	printf("%s. inputs: 0-%i, outputs: %i-%i\n", blockType, block.pars, block.pars+1, block.rets+block.pars);
	for(int i = 0; i < block.opSize; i++){
		IR_Instruction op = block.ops[i];
		if(op.opc != IR_CONST){
			printf("  (%i, %i) := <%s> %s (%i, %i, %i)\n",
				op.pars.q, op.pars.r, getTypeName(op.type.ptyp), getOpcodeName(op.opc), op.pars.a, op.pars.b, op.pars.c);
		}else{
			printf("  %i := <%s> CONST (%lu)\n",
				op.imm.ret, getTypeName(op.type.ptyp), op.imm.x);
		}
	}
}


void appendOpcode(CodeBlock* block, IR_Instruction opc){
	int codeSize = 0;
	if(block->opSize+1 >= block->opCap)	codeSize = block->opSize * 2;
	
	uint16_t maxvar = 0;
	if(opc.opc != IR_CONST){
		maxvar = (maxvar > opc.pars.a)? maxvar : opc.pars.a;
		maxvar = (maxvar > opc.pars.b)? maxvar : opc.pars.b;
		maxvar = (maxvar > opc.pars.c)? maxvar : opc.pars.c;
		maxvar = (maxvar > opc.pars.q)? maxvar : opc.pars.q;
		maxvar = (maxvar > opc.pars.r)? maxvar : opc.pars.r;
	}

	resizeCodeBlock(block, codeSize, maxvar);

	block->ops[block->opSize] = opc;
	block->opSize++;
}


/*
	Take a block, remove all nops and dead variables.

	Basically cleanup after dead code elimination.
*/
CodeBlock denopCodeBlock (CodeBlock block){
	CodeBlock ret;


	return ret;
}