#include "bytecodegen.h"
#include "program.h"


int getPrecedence(Operation opc){
	switch(opc){
		case OPR_ADD   : return 5;
		case OPR_SUB   : return 5;
		case OPR_MUL   : return 4;
		case OPR_DIV   : return 4;
		case OPR_MOD   : return 4;
		case OPR_EXP   : return 3;
		case OPR_AND   : return 6;
		case OPR_OR    : return 6;
		case OPR_XOR   : return 6;
		case OPR_LS    : return 7;
		case OPR_GT    : return 7;
		case OPR_LSE   : return 7;
		case OPR_GTE   : return 7;
		case OPR_EQ    : return 7;
		case OPR_NEQ   : return 7;
		case OPR_IX    : return 2;
	
		default: return 255;
	}
}
