#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "postprocess.h"
#include "program.h"
#include "ast.h"



int postprocess(SymbolTable* tab, ASTProgram* prog){
	TypeTable ttab;

	// Assemble namespaces
	
	
	
	// Perform sizing on types
	sizeTypes(&ttab, prog);
	printTypeTable(&ttab);
	
	// Build Function Bytecodes
	
	
	
	return 1;
}
