#include "codegen.h"
#include "program.h"





void x86AllocRegs(X86Block* blk){
	int         head    = 0;
	X86Register reghead = RAX;
	
	while(head < blk->opct){
		// TODO: Backtracking algo for register allocation
	}
}
