#include "print.h"
#include "util.h"
#include "stdint.h"
#include "stdio.h"



void leftpad(int offset){
	for(int i = 0; i < offset; i++) printf("  ");
}


void printStruct(Lisp* type, int offset){
	leftpad(offset);
	if(type == NULL){
		printf("<>\n");
		return;
	}
	if(type->here.typ == LSPTYP){
		printf("[\n");
		printStruct(type, offset+1);
		for(int i = 0; i < offset; i++) printf("  ");
		printf("]\n");
	}else if(type->here.typ == OPRTYP){
		printf("<opr: %lu>\n", type->here.val.UVAL);
	}else{
		printf("<malformed: %d>\n", type->here.typ);
	}
}


void printPars(Lisp* code){
	printf("( ");
	Lisp* head = code;
	while(head != NULL){
		switch(head->here.typ){
			case VARTYP: printf("<VAR %lu> ", head->here.val.UVAL); break;
			default    : printf("<malformed parameter> ");
		}
		head = head->next;
	}
	printf(")");
}


void printCode(Lisp* code, int offset){
	leftpad(offset);
	if(code == NULL){
		printf("<>\n");
		return;
	}
	if(code->here.typ == LSPTYP){
		printf("{\n");
		printCode(code->here.val.PVAL, offset+1);
		leftpad(offset);
		printf("}\n");
	}else if(code->here.typ == VARTYP){
		printf("<VAR %lu>\n", code->here.val.UVAL);
	}else if(code->here.typ == OPRTYP){
		switch(code->here.val.UVAL){
			case OP_ADD : printf("<ADD>\n"); break;
			case OP_SUB : printf("<SUB>\n"); break;
			case OP_MUL : printf("<MUL>\n"); break;
			case OP_DIV : printf("<DIV>\n"); break;
			
			case OP_FOLD: printf("<FOLD>\n"); break;
			
			case LET    :{
				if(lispSize(code) != 2){
					printf("<malformed let: expected 1 parameter, found %i\n", lispSize(code)-1);
				}else{printf("(LET\n");;
				      printCode(lispIx(code, 1).val.PVAL, offset+1); leftpad(offset); printf(")\n");} }break;
				      
			case LAMBDA :{
				if(lispSize(code) != 3){
					printf("<malformed lambda: expected 2 parameters, found %i\n", lispSize(code)-1);
				}else{printf("(LAMBDA "); printPars(lispIx(code, 1).val.PVAL);        printf("\n");
				      printCode(lispIx(code, 2).val.PVAL, offset+1); leftpad(offset); printf(")\n");} }break;
			
			case LSP    :{
				if(lispSize(code) != 3){
					printf("<malformed prefix expression: expected 2 parameters, found %i\n", lispSize(code)-1);
				}else{printf("(LSP (\n"); printCode(lispIx(code, 1).val.PVAL, offset+1); leftpad(offset+1); printf(")\n");
				      printCode(lispIx(code, 2).val.PVAL, offset+1);                     leftpad(offset  ); printf(")\n");} }break;
			
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
