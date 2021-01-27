#ifndef __CODEGEN_HEADER__
#define __CODEGEN_HEADER__


#include "stdint.h"

#include "program.h"


typedef enum{
	RAX   =  0,
	RCX   =  1,
	RDX   =  2,
	RBX   =  3,
	RSP   =  4,
	RBP   =  5,
	RSI   =  6,
	RDI   =  7,
	R8    =  8,
	R9    =  9,
	R10   = 10,
	R11   = 11,
	R12   = 12,
	R13   = 13,
	R14   = 14,
	R15   = 15,
	XMM0  = 16,
	XMM1  = 17,
	XMM2  = 18,
	XMM3  = 19,
	XMM4  = 20,
	XMM5  = 21,
	XMM6  = 22,
	XMM7  = 23,
	XMM8  = 24,
	XMM9  = 25,
	XMM10 = 26,
	XMM11 = 27,
	XMM12 = 28,
	XMM13 = 29,
	XMM14 = 30,
	XMM15 = 31,
	STSRG = 32,
	NOREG = -1
}X86Register;

typedef enum{
	// Arithmetic and ordering
	XO_ADD,
	XO_SUB,
	XO_ADC,
	XO_SBB,
	XO_MUL,
	XO_DIV,
	XO_IMUL,
	XO_IDIV,
	XO_CMP,
	XO_NEG,
	XO_INC,
	XO_DEC,
	XO_SETcc,
	
	// Bitwise
	XO_AND,
	XO_OR,
	XO_XOR,
	XO_NOT,
	XO_POPCNT,
	XO_BSF,
	XO_BSR,
	XO_LZCNT,
	XO_TZCNT,
	XO_SAR,
	XO_SHL,
	XO_SHR,
	XO_ROL,
	XO_ROR,
	XO_RCL,
	XO_RCR,
	XO_TEST,
	
	// Memory, cmov
	XO_MOV,
	XO_PUSH,
	XO_POP,
	XO_LDMOV,
	XO_STMOV,
	XO_CMOVcc,
	
	// Control flow
	XO_RET,
	XO_JMP,
	XO_Jcc,
	XO_CALL,
	XO_INT
}X86Opcode;


typedef enum{
	SC_8,
	SC_16,
	SC_32,
	SC_64
}X86Size;

typedef enum{
	CC_O      = 0x0,
	CC_NO     = 0x1,
	CC_B      = 0x2,
	CC_AE     = 0x3,
	CC_E      = 0x4,
	CC_NE     = 0x5,
	CC_BE     = 0x6,
	CC_A      = 0x7,
	CC_S      = 0x8,
	CC_NS     = 0x9,
	CC_PE     = 0xA,
	CC_PO     = 0xB,
	CC_L      = 0xC,
	CC_GE     = 0xD,
	CC_LE     = 0xE,
	CC_G      = 0xF,
	CC_NOCODE = -1,
}X86Cond;

typedef enum{
	XF_NOFLAGS = 0,
	XF_LOCK    = 1
}X86Flags;


typedef struct{
	X86Opcode   opc;
	X86Size     size;
	X86Register ra, rb;
	uint64_t    imm;
	
	int         a, b, q, r;
	
	X86Cond     cond;
	X86Flags    flags;
}X86Op;


typedef struct{
	X86Op*       ops;
	int          opct, opcap;
	
	// If >= 0, it maps variables to registers
	// If   -1, register is available
	// If   -2, register is preserved
	// If   -3, register is unavailable
	int          invars[32];
	int          exvars[32];
}X86Block;


typedef struct{
	X86Block*    blocks;
	int          bct, bcap;

	X86Register* varregs;
	int*         vardefs;
	int*         varends;
	int          varct, varcap;
}X86Func;



X86Func  makeX86Func   (int, int);
void     printX86Func  (X86Func*);
void     printX86Op    (X86Op);

int      appendX86Var  (X86Func*);
int      appendX86Block(X86Func*);
int      appendX86Op   (X86Block*);
X86Block makeX86Block  (int);

void     x86AllocRegs  (X86Block*);

int      compileBlock  (X86Block*, uint8_t*);

#endif
