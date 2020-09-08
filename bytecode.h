#ifndef __BYTECODE_HEADER__
#define __BYTECODE_HEADER__

#include "stdint.h"


typedef enum{
	IR_NOP      = 0x00,		// [replace with nop] will be a common way for deleting dead code
	IR_ADD		= 0x01,
	IR_SUB		= 0x02,
	IR_AND		= 0x03,
	IR_OR 		= 0x04,
	IR_XOR		= 0x05,


	IR_CAST_I8  = 0x20,
	IR_CAST_I16 = 0x21,
	IR_CAST_I32 = 0x22,
	IR_CAST_I64 = 0x23,
	IR_CAST_U8  = 0x24,
	IR_CAST_U16 = 0x25,
	IR_CAST_U32 = 0x26,
	IR_CAST_U64 = 0x27,

	IR_ARGS     = 0xA0,
	IR_RETS     = 0xA1,
	IR_CALL     = 0xA2,
	IR_IF       = 0xA3,
	IR_IFE      = 0xA4,

	IR_CONST    = 0xF0
}IR_Opcode;

typedef enum{
	IRP_NIL,
	IRP_I8,
	IRP_I16,
	IRP_I32,
	IRP_I64,
	IRP_U8,
	IRP_U16,
	IRP_U32,
	IRP_U64
}IR_PType;

typedef struct{
	IR_PType ptyp;		// For now, just this. Will add more complexity later.
}IR_Type;

typedef struct{
	uint16_t a, b, c;
	uint16_t q, r;
}IR_Pars;

typedef struct{
	uint64_t x;
	uint16_t ret;
}IR_Imm;

typedef struct{
	IR_Opcode opc;
	IR_Type	  type;
	union{
		IR_Pars pars;
		IR_Imm  imm;
	};
}IR_Instruction;

typedef struct{
	// Table for instructions
	IR_Instruction* ops;
	int opSize, opCap;

	// Type table for local variables
	IR_Type* varTyps;
	int vtSize, vtCap;

	int pars, rets;
}CodeBlock;



CodeBlock makeCodeBlock  (int, int, int, int);
void      resizeCodeBlock(CodeBlock*, int, int);
void      printCodeBlock (CodeBlock);
void      appendOpcode   (CodeBlock*, IR_Instruction);

CodeBlock denopCodeBlock (CodeBlock);
int       getBlockLatency(CodeBlock, int*);			// get minimum bound on latency of code block
int       codeBlock_DCE  (CodeBlock*);				// dead code elimination
void      appendBlock    (CodeBlock*, CodeBlock*);	// 



#endif