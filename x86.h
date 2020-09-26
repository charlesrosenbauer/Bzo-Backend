#ifndef __X86_HEADER__
#define __X86_HEADER__


#include "stdint.h"


typedef enum{
	X86_ADD,
	X86_SUB,
	X86_XOR,
	X86_AND,
	X86_OR,
	X86_NOT,
	X86_SHL,
	X86_SHR,
	X86_CMP,
	X86_JL,
	X86_JG,
	X86_JE,
	X86_JNE,
	X86_JMP
}X86Opcode;

typedef enum{
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

typedef struct{
	X86Opcode 	opc;
	ValSize	  	bitsize;
	int         a, b, c;
	uint64_t    imm; 
	uint8_t		lock;
}X86Op;


typedef struct{
	X86Op*		ops;
	int opct, opcap;
}X86Block;

typedef struct{
	uint8_t*	bytes;
	uint64_t	offset;
	int			codesize, codecap;
	
	uint64_t*	addresses;
	int*		addroffsets;
	
	int			addrsize, addrcap;
}MachineBlock;



MachineBlock writeMachineBlock(X86Block);


#endif
