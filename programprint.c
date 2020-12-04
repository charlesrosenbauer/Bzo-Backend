#include "print.h"
#include "util.h"
#include "stdint.h"
#include "stdio.h"



void printCode(Lisp* code, int offset){
	for(int i = 0; i < offset; i++) printf("  ");
	if(code->here.typ == OPRTYP){
		switch(code->here.val.UVAL){
			case OP_ADD : printf("<ADD>\n");
			
			default: printf("<malformed %i>\n", code->here.val.UVAL);
		}
	}else{
		printf("<malformed: %i>\n", code->here.typ);
	}
}



void printFunction(FnDef* fn){
	printf("FUNC %i <%i> (%i) -> (%i)\n", fn->fnid, fn->tpct, fn->prct, fn->rtct);
	printCode(fn->codeSource, 1);
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
