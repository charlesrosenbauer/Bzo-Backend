#include "x86.h"
#include "stdlib.h"


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
