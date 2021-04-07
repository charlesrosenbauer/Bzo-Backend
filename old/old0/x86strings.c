#include "x86.h"

#include "stdlib.h"
#include "string.h"





char* getX86Name(X86Opcode opc){
	switch(opc){
	
		// Arithmetic
		case X86_ADD	 : return "ADD";
		case X86_SUB	 : return "SUB";
		case X86_INC	 : return "INC";
		case X86_DEC	 : return "DEC";
		case X86_NEG	 : return "NEG";
		case X86_MUL	 : return "MUL";
		case X86_IMUL	 : return "IMUL";
		case X86_DIV	 : return "DIV";
		case X86_IDIV	 : return "IDIV";
		case X86_LEA	 : return "LEA";
		
		// Carry Arithmetic
		case X86_CLC	 : return "CLC";
		case X86_CMC	 : return "CMC";
		case X86_ADC	 : return "ADC";
		case X86_SBB	 : return "SBB";
		
		// Memory and Registers
		case X86_PUSH	 : return "PUSH";
		case X86_POP	 : return "POP";
		case X86_MOV	 : return "MOV";
		case X86_XCHG	 : return "XCHG";
		case X86_CMOVCC	 : return "CMOVCC";
		
		// Bitwise
		case X86_OR 	 : return "OR";
		case X86_AND	 : return "AND";
		case X86_XOR	 : return "XOR";
		case X86_NOT	 : return "NOT";
		case X86_ROL	 : return "ROL";
		case X86_ROR	 : return "ROR";
		case X86_RCL	 : return "RCL";
		case X86_RCR	 : return "RCR";
		case X86_SAL	 : return "SAL";
		case X86_SAR	 : return "SAR";
		case X86_SHL	 : return "SHL";
		case X86_SHR	 : return "SHR";
		
		// Comparison
		case X86_CMP	 : return "CMP";
		case X86_TEST	 : return "TEST";
		case X86_SETCC	 : return "SETCC";
		
		// Flow
		case X86_CALL	 : return "CALL";
		case X86_JCC	 : return "JCC";
		case X86_RETN	 : return "RETN";
		case X86_INT	 : return "INT";
		case X86_JMP	 : return "JMP";
		
		// Other
		case X86_NOP	 : return "NOP";
		
		// Variable Management
		case X86_PARVAL	 : return "PARVAL";
		case X86_RETVAL	 : return "RETVAL";
		case X86_DEFVAL	 : return "DEFVAL";
		case X86_CONST	 : return "CONST";
		case X86_ZERO_REG: return "ZERO_REG";
		
		// Compare-Fusion
		case X86_CMP_JMP : return "CMP_JMP";
		case X86_CMP_CMOV: return "CMP_CMOV";
		case X86_CMP_SET : return "CMP_SET";
		
		// Arithmetic and Bitwise Immediates
		case X86_ADDIMM	 : return "ADDIMM";
		case X86_SHLIMM	 : return "SHLIMM";
		case X86_SHRIMM  : return "SHRIMM";
		case X86_XORIMM  : return "XORIMM";
		case X86_ORRIMM  : return "ORRIMM";
		case X86_ANDIMM  : return "ANDIMM";
	}
	return NULL;
}




X86Opcode getX86Op(char* opc){
	
	if(opc[0] == 'A'){
		if(!strcmp(opc, "ADD"     )) return X86_ADD; 
		if(!strcmp(opc, "ADC"     )) return X86_ADC; 
		if(!strcmp(opc, "AND"     )) return X86_AND; 
		if(!strcmp(opc, "ADDIMM"  )) return X86_ADDIMM;
		if(!strcmp(opc, "ANDIMM"  )) return X86_ANDIMM;  
	}else if(opc[0] == 'C'){
		if(!strcmp(opc, "CLC"     )) return X86_CLC;
		if(!strcmp(opc, "CMC"     )) return X86_CMC;
		if(!strcmp(opc, "CMOVCC"  )) return X86_CMOVCC;
		if(!strcmp(opc, "CMP"     )) return X86_CMP;
		if(!strcmp(opc, "CALL"    )) return X86_CALL;
		if(!strcmp(opc, "CONST"   )) return X86_CONST;
		if(!strcmp(opc, "CMP_JMP" )) return X86_CMP_JMP;
		if(!strcmp(opc, "CMP_CMOV")) return X86_CMP_CMOV;
		if(!strcmp(opc, "CMP_SET" )) return X86_CMP_SET;
	}else if(opc[0] == 'D'){
		if(!strcmp(opc, "DEC"     )) return X86_DEC;
		if(!strcmp(opc, "DIV"     )) return X86_DIV;
		if(!strcmp(opc, "DEFVAL"  )) return X86_DEFVAL;
	}else if(opc[0] == 'I'){
		if(!strcmp(opc, "INC"     )) return X86_INC;
		if(!strcmp(opc, "IDIV"    )) return X86_IDIV;
		if(!strcmp(opc, "INT"     )) return X86_INT;
	}else if(opc[0] == 'J'){
		if(!strcmp(opc, "JCC"     )) return X86_JCC;
		if(!strcmp(opc, "JMP"     )) return X86_JMP;
	}else if(opc[0] == 'L'){
		if(!strcmp(opc, "LEA"     )) return X86_LEA;
	}else if(opc[0] == 'M'){
		if(!strcmp(opc, "MOV"     )) return X86_MOV;
	}else if(opc[0] == 'N'){
		if(!strcmp(opc, "NEG"     )) return X86_NEG;
		if(!strcmp(opc, "NOT"     )) return X86_NOT;
		if(!strcmp(opc, "NOP"     )) return X86_NOP;
	}else if(opc[0] == 'O'){
		if(!strcmp(opc, "OR"      )) return X86_OR;
		if(!strcmp(opc, "ORRIMM"  )) return X86_ORRIMM;
	}else if(opc[0] == 'P'){
		if(!strcmp(opc, "PUSH"    )) return X86_PUSH;
		if(!strcmp(opc, "POP"     )) return X86_POP;
		if(!strcmp(opc, "PARVAL"  )) return X86_PARVAL;
	}else if(opc[0] == 'R'){
		if(!strcmp(opc, "ROL"     )) return X86_ROL;
		if(!strcmp(opc, "ROR"     )) return X86_ROR;
		if(!strcmp(opc, "RCL"     )) return X86_RCL;
		if(!strcmp(opc, "RCR"     )) return X86_RCR;
		if(!strcmp(opc, "RETN"    )) return X86_RETN;
		if(!strcmp(opc, "RETVAL"  )) return X86_RETVAL;
	}else if(opc[0] == 'S'){
		if(!strcmp(opc, "SUB"     )) return X86_SUB;
		if(!strcmp(opc, "SBB"     )) return X86_SBB;
		if(!strcmp(opc, "SAL"     )) return X86_SAL;
		if(!strcmp(opc, "SAR"     )) return X86_SAR;
		if(!strcmp(opc, "SHL"     )) return X86_SHL;
		if(!strcmp(opc, "SHR"     )) return X86_SHR;
		if(!strcmp(opc, "SETCC"   )) return X86_SETCC;
		if(!strcmp(opc, "SHLIMM"  )) return X86_SHLIMM;
		if(!strcmp(opc, "SHRIMM"  )) return X86_SHRIMM;
	}else if(opc[0] == 'T'){
		if(!strcmp(opc, "TEST"    )) return X86_TEST;
	}else if(opc[0] == 'X'){
		if(!strcmp(opc, "XCHG"    )) return X86_XCHG;
		if(!strcmp(opc, "XOR"     )) return X86_XOR;
		if(!strcmp(opc, "XORIMM"  )) return X86_XORIMM;
	}else{
		if(!strcmp(opc, "ZERO_REG")) return X86_ZERO_REG;
	}
	
	return X86_NOP;
}
