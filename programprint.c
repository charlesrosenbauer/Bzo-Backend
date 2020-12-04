#include "print.h"
#include "util.h"
#include "stdint.h"
#include "stdio.h"



void printFunction(FnDef* fn){
	printf("FUNC %i <%i> (%i) -> (%i)\n", fn->fnid, fn->tpct, fn->prct, fn->rtct);
}


void printType(TyDef* ty){
	printf("TYPE %i <%i> [%i bytes, %i align]\n", ty->tyid, ty->parct, ty->size, ty->align);
}


void printTyClass(TCDef* tc){
	printf("CLSS %i\n", tc->tcid);
}


void printImpl(ImDef* im){
	printf("IMPL %i\n", im->imid);
}
