#ifndef __DEFS_HEADER__
#define __DEFS_HEADER__


#include "stdint.h"


typedef enum{
	OP_ADD,
	OP_SUB,
	OP_ADC,
	OP_SBB,
	OP_MUL,
	OP_IMUL,
	OP_DIV,
	OP_IDIV,
	OP_XOR,
	OP_AND,
	OP_OR,
	OP_SHL,
	OP_SHR,
	OP_ROL,
	OP_ROR,
	OP_SAL,
	OP_SAR,
	OP_XCHG,
	OP_CMOV,
	OP_SET,
	OP_J,
	OP_JMP,
	OP_CALL,
	OP_RET,
	OP_MOV,
	OP_TEST,
	OP_PCT,
	OP_CTZ,
	OP_CLZ,
	OP_LD,
	OP_STR,
	OP_LEA,
	OP_NOT,
	OP_NEG,
	OP_INC,
	OP_DEC,
	OP_INT,
	OP_NOP,
	OP_PUSH,
	OP_POP,
	OP_PUSHF,
	OP_POPF,
	OP_STC,
	OP_STD,
	OP_STI,
	OP_CMP,

	OP_ARGS,	// used for passing parameters into /out of functions
	OP_ZERO,	// xor r r; zeros register, eliminates dependency, no latency

	OP_CTRL
}OPCODE;


typedef enum{
	B8    = 0,
	B16   = 1,
	B32   = 2,
	B64   = 3,

	BITS  = 3,

	LOCK  = 4,

	C_LS  = 8,
	C_LSE = 16,
	C_GT  = 24,
	C_GTE = 32,
	C_EQ  = 40,
	C_NEQ = 48,
	C_O   = 56,
	C_S   = 64,
	C_Z   = 72,
	C_NZ  = 80,
	
	COND  = 120,

	SCL_2 = 128,
	SCL_4 = 256,
	SCL_8 = 384,

	SCALE = 384
}Settings;


typedef struct{
	int16_t  a, b, q, r;
	Settings settings;
	int32_t  imm;
	OPCODE   opc;
}OP;

typedef enum{
	INNERLOOP,
	FUNCHEAD,
	BRANCH
}BlockType;

typedef struct{
	OP*        code;
	int        size,   capacity;
	BlockType  type;
}Block;

typedef struct{
	Block*  blocks;
	int     size, capacity;
}Program;



char** makeOpNameTab();

void   printOP      (char**, OP);
void   printBlock   (char**, Block);
void   printFunction(char**, Program, Block);

Block  newBlock(BlockType, int);
void   addBlkOp(Block*, OP);






#endif