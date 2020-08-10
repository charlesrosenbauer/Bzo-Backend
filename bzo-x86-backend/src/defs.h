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


typedef enum{
	INT0_P  = 0x0001,
	INT1_P  = 0x0002,
	INT2_P  = 0x0004,
	INT3_P  = 0x0008,
	DMA0_P  = 0x0010,
	DMA1_P  = 0x0020,
	DMA2_P  = 0x0040,
	//DMA3_P  = 0x0080,
	FPU0_P  = 0x0100,
	FPU1_P  = 0x0200,
	FPU2_P  = 0x0400,
	FPU3_P  = 0x0800,
	FULL_P  = 0x1000,

	INTS_P  = 0x000F,
	DMAS_P  = 0x0070,
	FPUS_P  = 0x0F00
}Pipes;


typedef enum{
	R_RA    = 0x000000001,
	R_RC    = 0x000000002,
	R_RD    = 0x000000004,
	R_RB    = 0x000000008,
	R_SP    = 0x000000010,
	R_BP    = 0x000000020,
	R_SI    = 0x000000040,
	R_DI    = 0x000000080,
	R_R8    = 0x000000100,
	R_R9    = 0x000000200,
	R_R10   = 0x000000400,
	R_R11   = 0x000000800,
	R_R12   = 0x000001000,
	R_R13   = 0x000002000,
	R_R14   = 0x000004000,
	R_R15   = 0x000008000,
	R_X0    = 0x000010000,
	R_X1    = 0x000020000,
	R_X2    = 0x000040000,
	R_X3    = 0x000080000,
	R_X4    = 0x000100000,
	R_X5    = 0x000200000,
	R_X6    = 0x000400000,
	R_X7    = 0x000800000,
	R_X8    = 0x001000000,
	R_X9    = 0x002000000,
	R_X10   = 0x004000000,
	R_X11   = 0x008000000,
	R_X12   = 0x010000000,
	R_X13   = 0x020000000,
	R_X14   = 0x040000000,
	R_X15   = 0x080000000,
	R_FLGS  = 0x100000000,

	R_GPR   = 0x00000FFFF,
	R_XMM   = 0x0FFFF0000,
	R_GMN   = 0x00000000F,
	R_SPC   = 0x0000000F0,
	R_NIL   = 0x000000000
}Registers;


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
	char*     name;
	int       latency;
	Pipes     pipes[4];	 	 // Bitsets for which pipes can be used
	Registers a, b, q, r, s; // Bitsets	for which registers can be used for each I/O value, plus status
}OpcodeProperties;

typedef struct{
	char      name[16];
	int16_t   latency;
	int16_t   priority;
	int32_t   pipes;
	int32_t   regA, regB, regQ, regR;
	int8_t    setsFlags;
	int8_t    readsFlags;
	int8_t    uOps;
}OpcodeLine;


typedef struct{
	Block*  blocks;
	int     size, capacity;

	OpcodeProperties* opProps;
	char**            opNames;
}Program;


typedef struct{
	OP* table;
	int h, w;
}Tableau;



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

void     printRegisters(char*, Registers);
void     printPipes    (char*, Pipes);



int      getOpcodeLines(char*, char**, OpcodeLine*);
void     printOpLine   (OpcodeLine*);



void     resizeTableau(Tableau*);
Tableau  makeTableau  (int, int);

#endif