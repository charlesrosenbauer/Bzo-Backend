#include "stdint.h"
#include "stdio.h"

#include "program.h"
#include "util.h"




void printOperation(Operation op, int pad){
	leftpad(pad);
	switch(op){
		case OPR_ADD : printf(" +  " ); return;
		case OPR_SUB : printf(" -  " ); return;
		case OPR_MUL : printf(" *  " ); return;
		case OPR_DIV : printf(" /  " ); return;
		case OPR_MOD : printf(" %%  "); return;
		case OPR_EXP : printf(" ^  " ); return;
		case OPR_AND : printf(" &  " ); return;
		case OPR_OR  : printf(" |  " ); return;
		case OPR_XOR : printf(" ^^ " ); return;
		case OPR_NOT : printf(" !  " ); return;
		case OPR_EQ  : printf(" =  " ); return;
		case OPR_NEQ : printf(" != " ); return;
		case OPR_GT  : printf(" >  " ); return;
		case OPR_GTE : printf(" >= " ); return;
		case OPR_LS  : printf(" <  " ); return;
		case OPR_LSE : printf(" =< " ); return;
		case OPR_IX  : printf(" [] " ); return;
		case OPR_NEG : printf(" -  " ); return;
		case OPR_DREF: printf(" <- " ); return;
		case OPR_PTR : printf(" ^  " ); return;
		default      : printf(" ?? " ); return;
	}
}
