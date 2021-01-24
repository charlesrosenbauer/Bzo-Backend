#include "stdint.h"

#include "codegen.h"


uint16_t makeRRModrm(X86Op opc){
	if((opc.ra >= XMM0) && (opc.ra <= NOREG)) return 0;
	if((opc.rb >= XMM0) && (opc.ra <= NOREG)) return 0;
	
	uint8_t a = opc.ra;
	uint8_t b = opc.rb;
	
	uint16_t ret = 0;
	ret |= (a & 8) << 1;
	ret |= (b & 8) << 3;
	
	ret |= 0xC0;
	ret |= (a & 0x7);
	ret |= (b & 0x7) << 3;

	return ret;
}


int simpleOpcode(X86Flags flags, X86Size sz, uint32_t opcode, uint16_t modrm, uint8_t* bytes, int head){

	if(flags & XF_LOCK){
		bytes[head] = 0xf0;
		head++;
	}
	
	if(opcode & 0xff000000){
		bytes[head] = (opcode >> 24);
		head++;
		opcode &= 0x00ffffff;
	}

	uint8_t prefix = 0;
	if((modrm & 0xff00) && (sz != SC_64))
		prefix = 0x40 | (modrm >> 8);

	if(sz == SC_64){
		bytes[head  ] = 0x48 | (modrm >> 8);
		head++;
		opcode |= 1;
	}else if(sz == SC_32){
		if(prefix){
			bytes[head] = prefix;
			head++;
		}
	}else if(sz == SC_16){
		bytes[head  ] = 0x66;
		opcode |= 1;
		if(prefix){
			bytes[head+1] = prefix;
			head++;
		}
		head++;
	}else if(sz == SC_8){
		if(prefix){
			bytes[head] = prefix;
			head++;
		}
	}else{
		return 0;
	}
	
	if(opcode == (opcode & 0xff)){
		bytes[head  ] =  opcode & 0xff;
		bytes[head+1] =  modrm  & 0xff;
		return head+2;
	}else if(opcode == (opcode & 0xffff)){
		bytes[head  ] = (opcode >>  8) & 0xff;
		bytes[head+1] =  opcode & 0xff;
		bytes[head+2] =  modrm  & 0xff;
		return head+3;
	}else if(opcode == (opcode & 0xffffff)){
		bytes[head  ] = (opcode >> 16) & 0xff;
		bytes[head  ] = (opcode >>  8) & 0xff;
		bytes[head+2] =  opcode & 0xff;
		bytes[head+3] =  modrm  & 0xff;
		return head+4;
	}
	return 0;
}


int writeX86(X86Op opc, uint8_t* bytes, int head){
	switch(opc.opc){
	
		// Arithmetic and Comparison
		case XO_ADD : {
			return simpleOpcode(opc.flags, opc.size, 0x00, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_SUB : {
			return simpleOpcode(opc.flags, opc.size, 0x28, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_ADC : {
			return simpleOpcode(opc.flags, opc.size, 0x10, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_SBB : {
			return simpleOpcode(opc.flags, opc.size, 0x18, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_IMUL : {
			return simpleOpcode(opc.flags, opc.size, 0x0faf, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_MUL : {
			opc.rb = 4;
			return simpleOpcode(opc.flags, opc.size, 0xf7, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_DIV : {
			opc.rb = 6;
			return simpleOpcode(opc.flags, opc.size, 0xf7, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_IDIV : {
			opc.rb = 7;
			return simpleOpcode(opc.flags, opc.size, 0xf7, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_CMP : {
			return simpleOpcode(opc.flags, opc.size, 0x38, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_INC : {
			opc.rb = RAX;
			return simpleOpcode(opc.flags, SC_8, 0xff, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_DEC : {
			opc.rb = RCX;
			return simpleOpcode(opc.flags, SC_8, 0xff, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_NEG : {
			opc.rb = RDX;
			return simpleOpcode(opc.flags, SC_8, 0xff, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_SETcc : {
			if(opc.cond == CC_NOCODE) return 0;
			opc.rb = RAX;
			return simpleOpcode(opc.flags, SC_8, 0x0f90 + opc.cond, makeRRModrm(opc), bytes, head);
		}break;
		
		
		
		// Bitwise
		case XO_AND : {
			return simpleOpcode(opc.flags, opc.size, 0x20, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_OR  : {
			return simpleOpcode(opc.flags, opc.size, 0x08, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_XOR : {
			return simpleOpcode(opc.flags, opc.size, 0x30, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_NOT : {
			opc.rb = RBX;
			return simpleOpcode(opc.flags, SC_8, 0xff, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_BSF : {
			return simpleOpcode(opc.flags, opc.size, 0x0fbc, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_BSR : {
			return simpleOpcode(opc.flags, opc.size, 0x0fbd, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_POPCNT : {
			return simpleOpcode(opc.flags, opc.size, 0xf3000fb8, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_TZCNT : {
			return simpleOpcode(opc.flags, opc.size, 0xf3000fbc, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_LZCNT : {
			return simpleOpcode(opc.flags, opc.size, 0xf3000fbd, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_ROL : {
			opc.rb = 0;
			return simpleOpcode(opc.flags, opc.size, 0xd3, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_ROR : {
			opc.rb = 1;
			return simpleOpcode(opc.flags, opc.size, 0xd3, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_RCL : {
			opc.rb = 2;
			return simpleOpcode(opc.flags, opc.size, 0xd3, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_RCR : {
			opc.rb = 3;
			return simpleOpcode(opc.flags, opc.size, 0xd3, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_SHL : {
			opc.rb = 4;
			return simpleOpcode(opc.flags, opc.size, 0xd3, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_SHR : {
			opc.rb = 5;
			return simpleOpcode(opc.flags, opc.size, 0xd3, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_SAR : {
			opc.rb = 7;
			return simpleOpcode(opc.flags, opc.size, 0xd3, makeRRModrm(opc), bytes, head);
		}break;
		
		
		
		
		
		
		// Memory, cmov
		case XO_MOV : {
			return simpleOpcode(opc.flags, opc.size, 0x88, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_PUSH : {
			if(opc.ra > RDI){
				bytes[head] = 0x41;
				head++;
			}
			bytes[head] = 0x50 + (opc.ra & 0x7);
			return head + 1;
		}break;
		
		case XO_POP : {
			if(opc.ra > RDI){
				bytes[head] = 0x41;
				head++;
			}
			bytes[head] = 0x58 + (opc.ra & 0x7);
			return head + 1;
		}break;
		
		case XO_LDMOV : {
			return simpleOpcode(opc.flags, opc.size, 0x8b, makeRRModrm(opc) & 0xff3f, bytes, head);
		}break;
		
		case XO_STMOV : {
			return simpleOpcode(opc.flags, opc.size, 0x89, makeRRModrm(opc) & 0xff3f, bytes, head);
		}break;
		
		case XO_CMOVcc : {
			if(opc.cond == CC_NOCODE) return 0;
			return simpleOpcode(opc.flags, opc.size, 0x0f40 + opc.cond, makeRRModrm(opc), bytes, head);
		}break;
		
		
		
		// Control flow
		case XO_RET : {
			bytes[0] = 0xc3; return head + 1;
		}break;
		
		case XO_JMP : {
			opc.rb = RSP;
			return simpleOpcode(opc.flags, SC_8, 0xff, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_INT : {
			bytes[0] = 0xcd;
			bytes[1] = opc.imm & 0xff;
			return head + 2;
		}break;
		
	}
	
	return 0;
}
