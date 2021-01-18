#include "codegen.h"
#include "program.h"




int implicitRegisters(X86Op opc, X86Register* varregs, int* regvars){
	switch(opc.opc){
		case XO_MUL: {
			varregs[opc.r] = RDX;
			regvars[RDX  ] = opc.r;
		}break;
		
		case XO_DIV: {
			varregs[opc.r] = RDX;
			regvars[RDX  ] = opc.r;
		}break;
		
		default: return 0;
	}
	
	return 1;
}



void x86AllocRegs(X86Block* blk){
	

	int         head    = 0;
	X86Register reghead = RAX;
	
	while(head < blk->opct){
		// TODO: Backtracking algo for register allocation
	}
}
