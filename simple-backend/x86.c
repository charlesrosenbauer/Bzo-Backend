#include "stdint.h"

#include "codegen.h"


uint8_t modrm(X86Op opc){
	return 0;
}


int simpleOpcode(X86Size sz, uint8_t opcode, uint8_t* bytes, int head){
	if(sz == SC_64){
		bytes[head  ] = 0x48;
		bytes[head+1] = opcode | 1;
		//bytes[head+2] = modrm(opc);
		return 3;
	}else if(sz == SC_32){
		bytes[head  ] = opcode | 1;
		//bytes[head+1] = modrm(opc);
		return 2;
	}else if(sz == SC_16){
		bytes[head  ] = 0x66;
		bytes[head+1] = opcode | 1;
		//bytes[head+2] = modrm(opc);
		return 3;
	}else if(sz == SC_8){
		bytes[head  ] = opcode;
		//bytes[head+1] = modrm(opc);
		return 2;
	}
	return 0;
}


int writeX86(X86Op opc, uint8_t* bytes, int head){
	switch(opc.opc){
		case XO_ADD : {
			return simpleOpcode(opc.size,  0, bytes, head);
		}break;
		
		case XO_SUB : {
			return simpleOpcode(opc.size, 28, bytes, head);
		}break;
		
		case XO_ADC : {
			return simpleOpcode(opc.size, 10, bytes, head);
		}break;
		
		case XO_SBB : {
			return simpleOpcode(opc.size, 18, bytes, head);
		}break;
		
		
		
		case XO_AND : {
			return simpleOpcode(opc.size, 20, bytes, head);
		}break;
		
		case XO_OR  : {
			return simpleOpcode(opc.size,  8, bytes, head);
		}break;
		
		case XO_XOR : {
			return simpleOpcode(opc.size, 30, bytes, head);
		}break;
		
		case XO_CMP : {
			return simpleOpcode(opc.size, 38, bytes, head);
		}break;
		
		case XO_MOV : {
			return simpleOpcode(opc.size, 88, bytes, head);
		}break;
		
		case XO_RET : {
			bytes[0] = 0xc3; return 1;
		}break;
		
	}
	
	return 0;
}
