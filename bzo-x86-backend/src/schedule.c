#include "defs.h"
#include "stdlib.h"
#include "stdio.h"





char** makeOpNameTab(){
	// Just going to make the table bigger than necessary for now
	char** ret = malloc(sizeof(char*) * 256);

	ret[OP_ADD  ] = "ADD";
	ret[OP_SUB  ] = "SUB";
	ret[OP_ADC  ] = "ADC";
	ret[OP_SBB  ] = "SBB";
	ret[OP_MUL  ] = "MUL";
	ret[OP_IMUL ] = "IMUL";
	ret[OP_DIV  ] = "DIV";
	ret[OP_IDIV ] = "IDIV";
	ret[OP_XOR  ] = "XOR";
	ret[OP_AND  ] = "AND";
	ret[OP_OR   ] = "OR";
	ret[OP_SHL  ] = "SHL";
	ret[OP_SHR  ] = "SHR";
	ret[OP_ROL  ] = "ROL";
	ret[OP_ROR  ] = "ROR";
	ret[OP_SAL  ] = "SAL";
	ret[OP_SAR  ] = "SAR";
	ret[OP_XCHG ] = "XCHG";
	ret[OP_CMOV ] = "CMOV";
	ret[OP_SET  ] = "SET";
	ret[OP_J    ] = "J";
	ret[OP_JMP  ] = "JMP";
	ret[OP_CALL ] = "CALL";
	ret[OP_RET  ] = "RET";
	ret[OP_MOV  ] = "MOV";
	ret[OP_TEST ] = "TEST";
	ret[OP_PCT  ] = "PCT";
	ret[OP_CTZ  ] = "CTZ";
	ret[OP_CLZ  ] = "CLZ";
	ret[OP_LD   ] = "LD";
	ret[OP_STR  ] = "STR";
	ret[OP_LEA  ] = "LEA";
	ret[OP_NOT  ] = "NOT";
	ret[OP_NEG  ] = "NEG";
	ret[OP_INC  ] = "INC";
	ret[OP_DEC  ] = "DEC";
	ret[OP_INT  ] = "INT";
	ret[OP_NOP  ] = "NOP";
	ret[OP_PUSH ] = "PUSH";
	ret[OP_POP  ] = "POP";
	ret[OP_PUSHF] = "PUSHF";
	ret[OP_POPF ] = "POPF";
	ret[OP_STC  ] = "STC";
	ret[OP_STD  ] = "STD";
	ret[OP_CMP  ] = "CMP";
	ret[OP_ARGS ] = "ARGS";
	ret[OP_ZERO ] = "ZERO";
	ret[OP_CTRL ] = "CTRL";

	return ret;
}


char* getCCText(Settings cc){
	if(cc & COND){
		switch(cc & COND){
			case C_LS  : return "L";
			case C_LSE : return "LE";
			case C_GT  : return "G";
			case C_GTE : return "GE";
			case C_EQ  : return "E";
			case C_NEQ : return "NE";
			case C_O   : return "O";
			case C_S   : return "S";
			case C_Z   : return "Z";
			case C_NZ  : return "NZ";
		}
	}
	return "";
}


Block newBlock(BlockType ty, int capacity){
	Block ret;
	ret.code     = malloc(sizeof(OP) * capacity);
	ret.capacity = capacity;
	ret.size     = 0;
	ret.type     = ty;
	return ret;
}



void printOP(char** opnames, OP opcode){
	printf("#%i #%i = %s%s #%i #%i <%#010x>\n", opcode.q, opcode.r, opnames[opcode.opc], getCCText(opcode.settings), opcode.a, opcode.b, opcode.imm);
}


void printBlock(char** opnames, Block blk){
	for(int i = 0; i < blk.size; i++){

	}
}