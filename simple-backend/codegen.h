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
	NOREG = -1
}X86Register;

typedef enum{
	XO_ADD,
	XO_SUB,
	XO_MUL,
	XO_DIV,
	XO_AND,
	XO_OR,
	XO_XOR,
	XO_CMP
}X86Opcode;


typedef struct{
	X86Opcode   opc;
	X86Register ra, rb;
	uint64_t    imm;
	
	int         a, b, q, r;
}X86Op;


typedef struct{
	X86Op*       ops;
	int          opct, opcap;

	X86Register* varregs;
	int*         vardefs;
	int*         varends;
	int          varct, varcap;
}X86Block;






#endif
