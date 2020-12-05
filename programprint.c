#include "print.h"
#include "util.h"
#include "stdint.h"
#include "stdio.h"




void printStruct(Lisp* type, int offset){
	for(int i = 0; i < offset; i++) printf("  ");
	if(type == NULL){
		printf("<>\n");
		return;
	}
	if(type->here.typ == LSPTYP){
		printf("[\n");
		printStruct(type, offset+1);
		for(int i = 0; i < offset; i++) printf("  ");
		printf("]\n");
	}else{
		printf("<malformed: %d>\n", type->here.typ);
	}
}


void printCode(Lisp* code, int offset){
	for(int i = 0; i < offset; i++) printf("  ");
	if(code == NULL){
		printf("<>\n");
		return;
	}
	if(code->here.typ == LSPTYP){
		printf("{\n");
		printCode(code->here.val.PVAL, offset+1);
		for(int i = 0; i < offset; i++) printf("  ");
		printf("}\n");
	}else if(code->here.typ == VARTYP){
		printf("<VAR %lu>\n", code->here.val.UVAL);
	}else if(code->here.typ == OPRTYP){
		switch(code->here.val.UVAL){
			case OP_ADD : printf("<ADD>\n");
			case OP_SUB : printf("<SUB>\n");
			case OP_MUL : printf("<MUL>\n");
			case OP_DIV : printf("<DIV>\n");
			
			default: printf("<malformed %lu>\n", code->here.val.UVAL);
		}
	}else{
		printf("<malformed: %d>\n", code->here.typ);
	}
}



void printFunction(FnDef* fn){
	printf("FUNC %i <%i> (%i) -> (%i)\n", fn->fnid, fn->tpct, fn->prct, fn->rtct);
	printCode(fn->codeSource, 1);
}


void printType(TyDef* ty){
	printf("TYPE %i <%i> [%i bytes, %i align]\n", ty->tyid, ty->parct, ty->size, ty->align);
	printStruct(ty->parSource, 1);
	printStruct(ty->defSource, 1);
}


void printTyClass(TCDef* tc){
	printf("CLSS %i\n", tc->tcid);
}


void printImpl(ImDef* im){
	printf("IMPL %i\n", im->imid);
}
