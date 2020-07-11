#include "defs.h"

int main(){

	char** optab = makeOpNameTab();

	OP opc = {1, 7, 3, 4, C_LSE, 0, OP_CMOV};

	printOP(optab, opc);
}