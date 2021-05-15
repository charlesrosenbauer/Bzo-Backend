#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "program.h"
#include "error.h"
#include "util.h"



/*
	Parsing Infrastructure
*/
typedef enum{
	// NIL
	AL_NIL,

	// Tokens and Unwrapping
	AL_PAR,
	AL_BRK,
	AL_BRC,
	AL_TKN,
	
	// Typedef AST
	AL_TYPE,
	AL_STRC,
	AL_STLN,
	AL_UNON,
	AL_UNLN,
	AL_TGUN,
	AL_TULN,
	
	// Funcdef AST
	AL_FUNC,
	AL_EXPR,
	AL_LTRL,
	AL_FNCL,
	AL_BNOP,
	AL_UNOP,
	AL_BLOK,
	AL_LMDA,
	AL_PARS
}ASTListKind;

typedef struct{
	Position   pos;	// We should try to use the position in the token/union, as that would cut struct size by 25%
	union{
		Token  tk;
		void*  here;
	};
	void*      next;
	ASTListKind kind;
}ASTList;

void freeASTList(ASTList* l){
	if(l != NULL){
		if(l->kind != AL_TKN) freeASTList(l->here);
		freeASTList(l->next);
		free(l);
	}
}

void printASTList(ASTList* l, int pad){
	leftpad(pad);
	if((l == NULL) || (l->kind == AL_NIL)){
		printf("<> ");
		return;
	}
	switch(l->kind){
		case AL_PAR : {
			if(l->here == NULL){
				printf("() ");
			}else{
				printf("( ");
				printASTList(l->here, pad+1);
				//leftpad(pad);
				printf(") ");
				printASTList(l->next, 0);
			}
		}break;
		
		case AL_BRK : {
			if(l->here == NULL){
				printf("[] ");
			}else{
				printf("[ ");
				printASTList(l->here, pad+1);
				//leftpad(pad);
				printf("] ");
				printASTList(l->next, 0);
			}
		}break;
		
		case AL_BRC : {
			if(l->here == NULL){
				printf("{} ");
			}else{
				printf("{ ");
				printASTList(l->here, pad+1);
				//leftpad(pad);
				printf("} ");
				printASTList(l->next, 0);
			}
		}break;
		
		case AL_TKN : {
			char buffer[1024];
			printf("<%s> ", printToken(l->tk, buffer));
			printASTList(l->next, 0);
		}break;
		
		
		// Typedef AST
		case AL_TYPE : printf("TYPE "); break;
		case AL_STRC : printf("STRC "); break;
		case AL_STLN : printf("STLN "); break;
		case AL_UNON : printf("UNON "); break;
		case AL_UNLN : printf("UNLN "); break;
		case AL_TGUN : printf("TGUN "); break;
		case AL_TULN : printf("TULN "); break;
	
		// Funcdef AST
		case AL_FUNC : printf("FUNC "); break;
		case AL_EXPR : printf("EXPR "); break;
		case AL_LTRL : printf("LTRL "); break;
		case AL_FNCL : printf("FNCL "); break;
		case AL_BNOP : printf("BNOP "); break;
		case AL_UNOP : printf("UNOP "); break;
		case AL_BLOK : printf("BLOK "); break;
		case AL_LMDA : printf("LMDA "); break;
		case AL_PARS : printf("PARS "); break;
		
		// NIL
		case AL_NIL : {
			printf("<> ");
			//leftpad(pad);
		}break;
	}
}


int unwrap(LexerState* tks, ErrorList* errs, ASTList** list){
	int      ret = 1;
	Token*    xs = malloc(sizeof(Token)   * (tks->tkct+1));
	ASTList* lst = malloc(sizeof(ASTList) * (tks->tkct+1));
	lst[tks->tkct].kind = AL_NIL;
	int     ix  = 0;
	xs[0].type = TKN_VOID;
	for(int i  = 0; i < tks->tkct; i++){
		Token t = tks->tks[i];
		lst[i].next = NULL;
		switch(t.type){
			case TKN_PAR_OPN : {
				if(i > 0) lst[i-1].next = &lst[i];
				lst[i].kind = AL_PAR;
				lst[i].here = &lst[i+1];
				lst[i].pos  = t.pos;
				xs[ix+1]    = t; xs[ix+1].data.i64 = i; ix++; } break;
			case TKN_BRK_OPN : {
				if(i > 0) lst[i-1].next = &lst[i];
				lst[i].kind = AL_BRK;
				lst[i].here = &lst[i+1];
				lst[i].pos  = t.pos;
				xs[ix+1]    = t; xs[ix+1].data.i64 = i; ix++; } break;
			case TKN_BRC_OPN : {
				if(i > 0) lst[i-1].next = &lst[i];
				lst[i].kind = AL_BRC;
				lst[i].here = &lst[i+1];
				lst[i].pos  = t.pos;
				xs[ix+1]    = t; xs[ix+1].data.i64 = i; ix++; } break;
			case TKN_PAR_END : {
				if(xs[ix].type == TKN_PAR_OPN){
					lst[i-1].next = NULL;
					lst[xs[ix].data.i64].next = &lst[i+1];
					lst[i].kind   = AL_NIL;
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
					lst[i].kind   = AL_NIL;
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
					lst[i].kind   = AL_NIL;
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
				lst[i].kind = AL_TKN;
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


typedef struct{
	ASTList* lst;
	int      size;
}ASTLine;


void printASTLine(ASTLine ln){
	for(int i = 0; i < ln.size; i++){
		switch(ln.lst[i].kind){
			case AL_PAR  : printf("() "); break;
			case AL_BRC  : printf("{} "); break;
			case AL_BRK  : printf("[] "); break;
			case AL_TKN  : printf("TK "); break;
			
			case AL_TYPE : printf("TY "); break;
			case AL_STRC : printf("ST "); break;
			case AL_STLN : printf("S_ "); break;
			case AL_UNON : printf("UN "); break;
			case AL_UNLN : printf("U_ "); break;
			case AL_TGUN : printf("TU "); break;
			case AL_TULN : printf("T_ "); break;
			
			case AL_FUNC : printf("FN "); break;
			case AL_EXPR : printf("XP "); break;
			case AL_LTRL : printf("LT "); break;
			case AL_FNCL : printf("FC "); break;
			case AL_BNOP : printf("BO "); break;
			case AL_UNOP : printf("UO "); break;
			case AL_BLOK : printf("BK "); break;
			case AL_LMDA : printf("LM "); break;
			case AL_PARS : printf("PS "); break;
			
			case AL_NIL  : printf("?? "); break;
		}
	}
	printf("\n");
}


int astLen(ASTList* lst){
	int ct = 0;
	while(lst != NULL){
		ct++;
		lst = lst->next;
	}
	return ct;
}


ASTLine makeASTLine(int sz){
	ASTLine ret;
	ret.lst  = malloc(sizeof(ASTList) * sz);
	ret.size = sz;
	return ret;
}


ASTLine toLine(ASTList* lst){
	ASTLine ret;
	ret.size = astLen(lst);
	ret.lst  = malloc(sizeof(ASTList) * ret.size);
	for(int i = 0; i < ret.size; i++){
		ret.lst[i] = *lst;
		lst = lst->next;
	}
	return ret;
}


int splitOn(ASTLine* x, ASTLine* a, ASTLine* b, ASTListKind k){	
	for(int i = 0; i < x->size; i++){
		if (a->lst[i].kind == k){
			*a = makeASTLine(i);
			*b = makeASTLine(x->size - (i+1));
			for(int j =   0; j < i;       j++) a->lst[j  ] = x->lst[j];
			for(int j = i+1; j < x->size; j++) b->lst[j-i] = x->lst[j];
			return i;
		}
	}
	
	return 0;
}


int splitOnToken(ASTLine* x, ASTLine* a, ASTLine* b, TkType t){
	for(int i = 0; i < x->size; i++){
		if((a->lst[i].kind == AL_TKN) && (a->lst[i].tk.type == t)){
			*a = makeASTLine(i);
			*b = makeASTLine(x->size - (i+1));
			for(int j =   0; j < i;       j++) a->lst[j  ] = x->lst[j];
			for(int j = i+1; j < x->size; j++) b->lst[j-i] = x->lst[j];
			return i;
		}
	}
	
	return 0;
}


int viewSplitOn(ASTLine* x, ASTLine* a, ASTLine* b, ASTListKind k){	
	for(int i = 0; i < x->size; i++){
		if (a->lst[i].kind == k){
			a->size =  i;
			b->size =  x->size - (i+1);
			a->lst  =  x->lst;
			b->lst  = &x->lst[i+1];
			return i;
		}
	}
	
	return 0;
}


int viewSplitOnToken(ASTLine* x, ASTLine* a, ASTLine* b, TkType t){
	for(int i = 0; i < x->size; i++){
		if((a->lst[i].kind == AL_TKN) && (a->lst[i].tk.type == t)){
			a->size =  i;
			b->size =  x->size - (i+1);
			a->lst  =  x->lst;
			b->lst  = &x->lst[i+1];
			return i+1;
		}
	}
	
	return 0;
}


int match(ASTLine* ln, ASTListKind* ks){
	for(int i = 0; i < ln->size; i++)
		if(ln->lst[i].kind != ks[i]) return 0;
	return 1;
}


int tokenMatch(ASTLine* ln, TkType* ts){
	for(int i = 0; i < ln->size; i++)
		if((ts[i] != TKN_VOID) && ((ln->lst[i].kind == AL_TKN) || (ln->lst[i].tk.type != ts[i]))) return 0;
	return 1;
}





/*
	Actual Parser Rules
*/










int parseCode(LexerState* tks, SymbolTable* tab, ASTProgram* prog, ErrorList* errs){
	printf("\n=================\n");
	ASTList* lst;
	if(!unwrap(tks, errs, &lst)) return -1;
	printASTList(lst, 0);
	printf("\n=================\n");
	
	printf("Len=%i\n", astLen(lst));
	
	ASTLine ln = toLine(lst);
	int cont = 1;
	ASTLine a = ln, b;
	while(cont){
		ASTLine x = a;
		cont = viewSplitOnToken(&x, &a, &b, TKN_NEWLINE);
		printASTLine(a);
		a = b;
	}
	

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
