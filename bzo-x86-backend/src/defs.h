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

	OP_CNST,	// used for creating constants
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
	int16_t  q, r, a, b;
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
	char*    name;
	int      latency;
	uint16_t pipes[4];	 	// Bitsets for which pipes can be used
	uint64_t a, b, q, r, s; // Bitsets	for which registers can be used for each I/O value, plus status
}OpcodeProperties;


typedef struct{
	Block*  blocks;
	int     size, capacity;

	OpcodeProperties* opProps;
	char**            opNames;
}Program;



char**  makeOpNameTab();

void    printOP      (char**, OP);
void    printBlock   (char**, Block, int);
void    printProgram (Program);

int     findOp       (char**, char*);
void    printOpProps (OpcodeProperties*);

Block   newBlock     (BlockType, int);
void    addBlkOp     (Block*, OP);

Program newProgram   (int);
void    addPrgmBlk   (Program*, Block);


OpcodeProperties* loadOpProps(char**, char*);






#endif