#ifndef __X86_READER_HEADER__
#define __X86_READER_HEADER__


#include "stdint.h"
#include "util.h"
#include "hashtable.h"


typedef enum{
	RAX = 0x00001,
	RBX = 0x00002,
	RCX = 0x00004,
	RDX = 0x00008,
	RBP = 0x00010,
	RSP = 0x00020,
	RSI = 0x00040,
	RDI = 0x00080,
	R8  = 0x00100,
	R9  = 0x00200,
	R10 = 0x00400,
	R11 = 0x00800,
	R12 = 0x01000,
	R13 = 0x02000,
	R14 = 0x04000,
	R15 = 0x08000,
	STS = 0x10000,
	RIP = 0x20000
}X86REG;




typedef struct{
	STRING   opname;
	uint32_t opcode;
	uint8_t  opcodesize;
	uint8_t  requiredprefix;
	uint8_t  rmm;
	uint8_t  inct, exct;
	uint64_t inmasks[4];
	uint64_t exmasks[4];
	uint64_t pipes;
	uint16_t minlatency;
	uint16_t maxlatency;
}X86_OPCODE;




HASHTABLE loadOpcodeTable(char*);




#endif