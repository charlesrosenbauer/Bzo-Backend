#include "x86.h"
#include "stdint.h"



int x86OpRegMasks(X86Opcode opc, uint64_t* a, uint64_t* b, uint64_t* c, uint64_t* q, uint64_t* r, uint8_t* flags){
	
	
	
	const uint64_t GPR = 0x0000ffff;
	const uint64_t XMM = 0xffff0000;
	const uint64_t AXX = 0x00000001;
	const uint64_t CXX = 0x00000002;
	const uint64_t DXX = 0x00000004;
	const uint64_t BXX = 0x00000008;
	const uint64_t SPX = 0x00000010;
	const uint64_t BPX = 0x00000020;
	const uint64_t SIX = 0x00000040;
	const uint64_t DIX = 0x00000080;
	const uint64_t NIL = 0x00000000;
	
	const uint8_t  STS = 0x01;
	const uint8_t  PR1 = 0x02;
	const uint8_t  PR2 = 0x04;
	const uint8_t  PR3 = 0x06;
	const uint8_t  RT1 = 0x08;
	const uint8_t  RT2 = 0x10;
	
	*flags = STS;  // Status register is assumed to be set unless otherwise specified. To be safe, only lea and mov are safe from it for now.
	
	switch(opc){
		// Arithmetic
		case X86_ADD   : {*a = GPR, *b = GPR, *c = NIL, *q = GPR, *r = NIL, *flags |= PR2 | RT1; } break;
		case X86_SUB   : {*a = GPR, *b = GPR, *c = NIL, *q = GPR, *r = NIL, *flags |= PR2 | RT1; } break;
		case X86_MUL   : {*a = AXX, *b = GPR, *c = NIL, *q = AXX, *r = DXX, *flags |= PR2 | RT2; } break;
		case X86_IMUL  : {*a = AXX, *b = GPR, *c = NIL, *q = AXX, *r = DXX, *flags |= PR2 | RT2; } break;
		case X86_DIV   : {*a = AXX, *b = GPR, *c = NIL, *q = AXX, *r = DXX, *flags |= PR2 | RT2; } break;
		case X86_IDIV  : {*a = AXX, *b = GPR, *c = NIL, *q = AXX, *r = DXX, *flags |= PR2 | RT2; } break;
		
		
		// Carry Arithmetic
		
		// Memory
		
		// Bitwise
		case X86_AND   : {*a = GPR, *b = GPR, *c = NIL, *q = GPR, *r = NIL, *flags |= PR2 | RT1; } break;
		case X86_OR    : {*a = GPR, *b = GPR, *c = NIL, *q = GPR, *r = NIL, *flags |= PR2 | RT1; } break;
		case X86_XOR   : {*a = GPR, *b = GPR, *c = NIL, *q = GPR, *r = NIL, *flags |= PR2 | RT1; } break;
		
		// Comparison
		
		// Flow
		
		// Other
		
		default: return 0;
	}
	
	return 1;
}
