#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "program.h"
#include "error.h"
#include "util.h"





typedef enum{
	TL_PAR,
	TL_BRK,
	TL_BRC,
	TL_TKN,
	TL_NIL
}TyListKind;

typedef struct{
	Position   pos;	// We should try to use the position in the token/union, as that would cut struct size by 25%
	union{
		Token  tk;
		void*  here;
	};
	void*      next;
	TyListKind kind;
}TkList;

void freeTkList(TkList* tk){
	if(tk != NULL){
		if(tk->kind != TL_TKN) freeTkList(tk->here);
		freeTkList(tk->next);
		free(tk);
	}
}

void printTkList(TkList* tl, int pad){
	leftpad(pad);
	if((tl == NULL) || (tl->kind == TL_NIL)){
		printf("<> ");
		return;
	}
	switch(tl->kind){
		case TL_PAR : {
			if(tl->here == NULL){
				printf("() ");
			}else{
				printf("( ");
				printTkList(tl->here, pad+1);
				//leftpad(pad);
				printf(") ");
				printTkList(tl->next, 0);
			}
		}break;
		
		case TL_BRK : {
			if(tl->here == NULL){
				printf("[] ");
			}else{
				printf("[ ");
				printTkList(tl->here, pad+1);
				//leftpad(pad);
				printf("] ");
				printTkList(tl->next, 0);
			}
		}break;
		
		case TL_BRC : {
			if(tl->here == NULL){
				printf("{} ");
			}else{
				printf("{ ");
				printTkList(tl->here, pad+1);
				//leftpad(pad);
				printf("} ");
				printTkList(tl->next, 0);
			}
		}break;
		
		case TL_TKN : {
			char buffer[1024];
			printf("<%s> ", printToken(tl->tk, buffer));
			printTkList(tl->next, 0);
		}break;
		
		case TL_NIL : {
			printf("<> ");
			//leftpad(pad);
		}break;
	}
}


int unwrap(LexerState* tks, ErrorList* errs, TkList** list){
	int     ret  = 1;
	Token*  xs  = malloc(sizeof(Token)  * (tks->tkct+1));
	TkList* lst = malloc(sizeof(TkList) * (tks->tkct+1));
	lst[tks->tkct].kind = TL_NIL;
	int     ix  = 0;
	xs[0].type = TKN_VOID;
	for(int i  = 0; i < tks->tkct; i++){
		Token t = tks->tks[i];
		lst[i].next = NULL;
		switch(t.type){
			case TKN_PAR_OPN : {
				if(i > 0) lst[i-1].next = &lst[i];
				lst[i].kind = TL_PAR;
				lst[i].here = &lst[i+1];
				lst[i].pos  = t.pos;
				xs[ix+1]    = t; xs[ix+1].data.i64 = i; ix++; } break;
			case TKN_BRK_OPN : {
				if(i > 0) lst[i-1].next = &lst[i];
				lst[i].kind = TL_BRK;
				lst[i].here = &lst[i+1];
				lst[i].pos  = t.pos;
				xs[ix+1]    = t; xs[ix+1].data.i64 = i; ix++; } break;
			case TKN_BRC_OPN : {
				if(i > 0) lst[i-1].next = &lst[i];
				lst[i].kind = TL_BRC;
				lst[i].here = &lst[i+1];
				lst[i].pos  = t.pos;
				xs[ix+1]    = t; xs[ix+1].data.i64 = i; ix++; } break;
			case TKN_PAR_END : {
				if(xs[ix].type == TKN_PAR_OPN){
					lst[i-1].next = NULL;
					lst[xs[ix].data.i64].next = &lst[i+1];
					lst[i].kind   = TL_NIL;
					ix--;
				}else{
					// Error!
					Position pos = fusePosition(xs[ix].pos, t.pos);
					appendError(errs, (Error){ERR_P_BAD_PAR, pos});
					ret = 0;
				}
			}break;
			case TKN_BRK_END : {
				if(xs[ix].type == TKN_BRK_OPN){
					lst[i-1].next = NULL;
					lst[xs[ix].data.i64].next = &lst[i+1];
					lst[i].kind   = TL_NIL;
					ix--;
				}else{
					// Error!
					Position pos = fusePosition(xs[ix].pos, t.pos);
					appendError(errs, (Error){ERR_P_BAD_BRK, pos});
					ret = 0;
				}
			}break;
			case TKN_BRC_END : {
				if(xs[ix].type == TKN_BRC_OPN){
					lst[i-1].next = NULL;
					lst[xs[ix].data.i64].next = &lst[i+1];
					lst[i].kind   = TL_NIL;
					ix--;
				}else{
					// Error!
					Position pos = fusePosition(xs[ix].pos, t.pos);
					appendError(errs, (Error){ERR_P_BAD_BRC, pos});
					ret = 0;
				}
			}break;
			default:{
				if(i > 0) lst[i-1].next = &lst[i];
				lst[i].next = NULL;
				lst[i].kind = TL_TKN;
				lst[i].pos  = t.pos;
				lst[i].tk   = t;
			}break;
		}
	}
	if(ix != 0){
		ret = 0;
		for(int i = ix; i > 0; i--){
			if       (xs[ix].type == TKN_PAR_OPN){
				appendError(errs, (Error){ERR_P_DNGL_PAR, xs[ix].pos});
			}else if (xs[ix].type == TKN_BRK_OPN){
				appendError(errs, (Error){ERR_P_DNGL_BRK, xs[ix].pos});
			}else if (xs[ix].type == TKN_BRC_OPN){
				appendError(errs, (Error){ERR_P_DNGL_BRC, xs[ix].pos});
			}
		}
	}
	*list = lst;
	free(xs);
	return ret;
}




int parseCode(LexerState* tks, SymbolTable* tab, ASTProgram* prog, ErrorList* errs){
	printf("\n=================\n");
	TkList* lst;
	if(!unwrap(tks, errs, &lst)) return -1;
	printTkList(lst, 0);
	printf("\n=================\n");
	
	//TkLines   lines = splitLines(lst);
	//printTkLines(&lines);

	return 0;
}




void printASTType(ASTType ty, int pad){
	leftpad(pad);
	if(ty.kind == TT_ELEM){
		for(int i = 0; i < ty.type.elem.arct; i++){
			if(ty.type.elem.arrs[i] < 0){
				printf("^");
			}else if(ty.type.elem.arrs[i] == 0){
				printf("[]");
			}else{
				printf("[%i]", ty.type.elem.arrs[i]);
			}
		}
		printf("T%i", ty.type.elem.tyid);
	}else if(ty.kind == TT_STRC){
		printf("[%i:\n", ty.type.strc.valct);
		ASTType* ts = ty.type.strc.vals;
		for(int i = 0; i < ty.type.strc.valct; i++){ printASTType(ts[i], pad+1); printf(" -> %i\n", ty.type.unon.labels[i]); }
		leftpad(pad);
		printf("]");
	}else if(ty.kind == TT_UNON){
		printf("(%i:\n", ty.type.strc.valct);
		ASTType* ts = ty.type.unon.vals;
		for(int i = 0; i < ty.type.unon.valct; i++){ printASTType(ts[i], pad+1); printf(" -> %i\n", ty.type.unon.labels[i]); }
		leftpad(pad);
		printf(")");
	}else{
		printf("BID%i", ty.type.bity.bid);
	}
}



void printASTProgram(ASTProgram prog){
	printf("Functions:[%i]\n", prog.fnct);
	for(int i = 0; i < prog.fnct; i++){
		Position p = prog.fns[i].pos;
		printf("  FN%i %i@(%i:%i - %i:%i)\n", i, p.fileId, p.lineStart, p.colStart, p.lineStart, p.lineEnd);
	}
	
	printf("Types:[%i]\n", prog.tyct);
	for(int i = 0; i < prog.tyct; i++){
		Position p = prog.tys[i].pos;
		printf("  TY%i %i@(%i:%i - %i:%i)\n", i, p.fileId, p.lineStart, p.colStart, p.lineStart, p.lineEnd);
		printASTType(prog.tys[i].type, 2);
		printf("\n");
	}
}
