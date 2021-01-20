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
	XO_ADD,
	XO_SUB,
	XO_ADC,
	XO_SBB,
	XO_MUL,
	XO_DIV,
	XO_IMUL,
	XO_IDIV,
	XO_AND,
	XO_OR,
	XO_XOR,
	XO_CMP,
	XO_MOV,
	XO_RET,
	XO_PUSH,
	XO_POP
}X86Opcode;


typedef enum{
	SC_8,
	SC_16,
	SC_32,
	SC_64
}X86Size;

typedef enum{
	CC_NOCODE,
	CC_L,
	CC_LE,
	CC_G,
	CC_GE,
	CC_B,
	CC_BE,
	CC_A,
	CC_AE,
	CC_E,
	CC_NE,
	CC_Z,
	CC_NZ,
	CC_O
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



X86Func makeX86Func (int, int);
void    printX86Func(X86Func*);

int     appendX86Var  (X86Func*);
int     appendX86Block(X86Func*);
int     appendX86Op   (X86Block*);



#endif
