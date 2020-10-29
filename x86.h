#ifndef __X86_HEADER__
#define __X86_HEADER__


#include "stdint.h"


typedef enum{
	
	// Arithmetic
	X86_ADD,
	X86_SUB,
	X86_INC,
	X86_DEC,
	X86_NEG,
	X86_MUL,
	X86_DIV,
	X86_IDIV,
	X86_IMUL,
	X86_LEA,
	
	// Carry Arithmetic
	X86_CMC,
	X86_CLC,
	X86_ADC,
	X86_SBB,
	
	// Memory and Registers
	X86_PUSH,
	X86_POP,
	X86_MOV,
	X86_XCHG,
	X86_CMOVCC,
	
	// Bitwise
	X86_OR,
	X86_AND,
	X86_XOR,
	X86_NOT,
	X86_ROL,
	X86_ROR,
	X86_RCL,
	X86_RCR,
	X86_SAL,
	X86_SAR,
	X86_SHL,
	X86_SHR,
	
	// Comparison
	X86_CMP,
	X86_TEST,
	X86_SETCC,
	
	// Flow
	X86_CALL,
	X86_JCC,
	X86_RETN,
	X86_INT,
	X86_JMP,
	
	// Other
	X86_NOP,
	
	// Derived - these instructions don't exist in x86, but are there for codegen convenience
	X86_ZERO_REG,		// xor reg reg
	X86_CMP_JMP,
	X86_CMP_CMOV,
	X86_CMP_SET,
	X86_CONST,
	X86_ACC  			// add rax imm

}X86Opcode;

typedef enum{
	C86_O,
	C86_L,
	C86_LE,
	C86_G,
	C86_GE,
	C86_E,
	C86_NE,
	C86_Z,
	C86_NZ
}X86Cond;



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
	SC_0,
	SC_8,
	SC_16,
	SC_32,
	SC_64,
	
	VC_16x8,
	VC_32x8,
	VC_64x8,
	VC_8x16,
	VC_16x16,
	VC_32x16,
	VC_4x32,
	VC_8x32,
	VC_16x32,
	VC_2x64,
	VC_4x64,
	VC_8x64
}ValSize;

typedef enum{
	AM_RR,	
	AM_RM,
	AM_RS,
	AM_VV,
	AM_VM,
	AM_VS
}X86AddrMode;


typedef struct{
	X86Register s, d;
}X86_RR_Addr;

typedef struct{
	X86Register d, ptr, index;
	int32_t     disp;
}X86_RM_Addr;

typedef struct{
	X86Register d, ptr, index;
	int32_t     disp, scale;
}X86_RS_Addr;

typedef struct{
	int16_t     q, r, a, b, c;
}X86_VV_Addr;

typedef struct{
	int16_t     q, r, a, b, ptr, index;
	int32_t     disp;
}X86_VM_Addr;

typedef struct{
	int16_t     q, r, a, b, ptr, index;
	int32_t     disp, scale;
}X86_VS_Addr;

typedef struct{
	X86Opcode  opc;
	union{
		X86_RR_Addr rr;
		X86_RM_Addr rm;
		X86_RS_Addr rs;
		X86_VV_Addr vv;
		X86_VM_Addr vm;
		X86_VS_Addr vs;
	};
	X86AddrMode addrmode;
	ValSize     bitsize;
	uint64_t    immediate;
	uint8_t     lock;
}X86Op;


typedef struct{
	X86Register reg;
	int         offset;
}X86Value;

typedef struct{
	X86Op*      ops;
	int opct, opcap, nextBlock;
}X86Block;

typedef struct{
	X86Value*      pars;
	X86Value*      rets;
	X86Value*      vars;
	
	int parct, retct, varct, stackSize, blockSize;

	X86Block*    blocks;
	
	int bct, mct;
}X86Function;


typedef struct{
	uint8_t* bytes;
	int *ixs, *symbols;
	int  ixct, bytect, bytecap;
}MachineBlock;



/*
MachineBlock writeMachineBlock(X86Block);

X86Block allocRegs       (X86BCBlock);
int      functionRegAlloc(X86Function*);
int      x86OpRegMasks   (X86Opcode, uint64_t*, uint64_t*, uint64_t*, uint64_t*, uint64_t*, uint8_t*);

int      functionRegAlloc(X86Function*);
*/
#endif
