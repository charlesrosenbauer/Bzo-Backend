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
	OP_CSET,
	OP_CJ,
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

	OP_ARGS,	// used for passing parameters into /out of functions
	OP_ZERO,	// xor r r; zeros register, eliminates dependency, no latency

	OP_CTRL
}OPCODE;


typedef enum{
	B8    = 0,
	B16   = 1,
	B32   = 2,
	B64   = 3,
	LOCK  = 4,
	C_LS  = 8,
	C_LSE = 9,
	C_GT  = 10,
	C_GTE = 11,
	C_EQ  = 12,
	C_NEQ = 13,
	C_O   = 14,
	C_S   = 15,
	C_Z   = 16,
	C_NZ  = 17
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











#endif