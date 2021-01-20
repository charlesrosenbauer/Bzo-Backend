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


int simpleOpcode(X86Flags flags, X86Size sz, uint8_t opcode, uint16_t modrm, uint8_t* bytes, int head){

	if(flags & XF_LOCK){
		bytes[head] = 0xf0;
		head++;
	}

	uint8_t prefix = 0;
	if((modrm & 0xff00) && (sz != SC_64))
		prefix = 0x40 | (modrm >> 8);

	if(sz == SC_64){
		bytes[head  ] = 0x48 | (modrm >> 8);
		bytes[head+1] = opcode | 1;
		bytes[head+2] = modrm & 0xff;
		return 3;
	}else if(sz == SC_32){
		if(prefix){
			bytes[head] = prefix;
			head++;
		}
		bytes[head  ] = opcode | 1;
		bytes[head+1] = modrm & 0xff;
		return 2;
	}else if(sz == SC_16){
		bytes[head  ] = 0x66;
		if(prefix){
			bytes[head+1] = prefix;
			head++;
		}
		bytes[head+1] = opcode | 1;
		bytes[head+2] = modrm & 0xff;
		return 3;
	}else if(sz == SC_8){
		if(prefix){
			bytes[head] = prefix;
			head++;
		}
		bytes[head  ] = opcode;
		bytes[head+1] = modrm & 0xff;
		return 2;
	}
	return 0;
}


int writeX86(X86Op opc, uint8_t* bytes, int head){
	switch(opc.opc){
		case XO_ADD : {
			return simpleOpcode(opc.flags, opc.size,  0, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_SUB : {
			return simpleOpcode(opc.flags, opc.size, 28, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_ADC : {
			return simpleOpcode(opc.flags, opc.size, 10, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_SBB : {
			return simpleOpcode(opc.flags, opc.size, 18, makeRRModrm(opc), bytes, head);
		}break;
		
		
		
		case XO_AND : {
			return simpleOpcode(opc.flags, opc.size, 20, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_OR  : {
			return simpleOpcode(opc.flags, opc.size,  8, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_XOR : {
			return simpleOpcode(opc.flags, opc.size, 30, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_CMP : {
			return simpleOpcode(opc.flags, opc.size, 38, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_MOV : {
			return simpleOpcode(opc.flags, opc.size, 88, makeRRModrm(opc), bytes, head);
		}break;
		
		case XO_RET : {
			bytes[0] = 0xc3; return 1;
		}break;
		
		
		case XO_PUSH : {
			if(opc.ra > RDI){
				bytes[head] = 0x41;
				head++;
			}
			bytes[head] = 0x50 + (opc.ra & 0x7);
			return 2;
		}break;
		
		case XO_POP : {
			if(opc.ra > RDI){
				bytes[head] = 0x41;
				head++;
			}
			bytes[head] = 0x58 + (opc.ra & 0x7);
			return 2;
		}break;
		
	}
	
	return 0;
}
