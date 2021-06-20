#include "stdint.h"
#include "stdio.h"

#include "program.h"



void leftpad(int pad){
	for(int i = 0; i < pad; i++) printf("  ");
}

void printOperation(Operation op, int pad){
	leftpad(pad);
	switch(op){
		case OPR_ADD : printf(" +  "); return;
		case OPR_SUB : printf(" -  "); return;
		case OPR_MUL : printf(" *  "); return;
		case OPR_DIV : printf(" /  "); return;
		case OPR_MOD : printf(" %%  "); return;
		case OPR_AND : printf(" &  "); return;
		case OPR_OR  : printf(" |  "); return;
		case OPR_XOR : printf(" ^^ "); return;
	}
}
