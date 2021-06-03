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
	AL_TYDF,
	AL_TYPE,
	AL_TYLM,
	AL_STRC,
	AL_STLN,
	AL_FNTY,
	AL_FPRS,
	AL_TPRS,
	AL_UNON,
	AL_ENUM,
	AL_UNLN,
	AL_TGUN,
	AL_TULN,
	
	// Funcdef AST
	AL_FNDF,
	AL_FTPS,
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
		case AL_TYDF : printf("TYDF "); break;
		case AL_TYPE : printf("TYPE "); break;
		case AL_TYLM : printf("ELEM "); break;
		case AL_STRC : printf("STRC "); break;
		case AL_STLN : printf("STLN "); break;
		case AL_FNTY : printf("FNTY "); break;
		case AL_FPRS : printf("FPRS "); break;
		case AL_TPRS : printf("TPRS "); break;
		case AL_UNON : printf("UNON "); break;
		case AL_ENUM : printf("ENUM "); break;
		case AL_UNLN : printf("UNLN "); break;
		case AL_TGUN : printf("TGUN "); break;
		case AL_TULN : printf("TULN "); break;
	
		// Funcdef AST
		case AL_FNDF : printf("FNDF "); break;
		case AL_FTPS : printf("FTPS "); break;
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
			case AL_PAR  : printf("()  "); break;
			case AL_BRC  : printf("{}  "); break;
			case AL_BRK  : printf("[]  "); break;
			case AL_TKN  : {
				switch(ln.lst[i].tk.type){
					case TKN_S_ID      : printf("ID  " ); break;
					case TKN_S_BID     : printf("BI  " ); break;
					case TKN_S_TYID    : printf("TI  " ); break;
					case TKN_S_MID     : printf("MI  " ); break;
					case TKN_INT       : printf("INT " ); break;
					case TKN_FLT       : printf("FLT " ); break;
					case TKN_STR       : printf("STR " ); break;
					case TKN_TAG       : printf("TAG " ); break;
					
					case TKN_DEFINE    : printf("::  " ); break;
					case TKN_COLON     : printf(":   " ); break;
					case TKN_SEMICOLON : printf(";   " ); break;
					case TKN_PERIOD    : printf(".   " ); break;
					case TKN_COMMA     : printf(",   " ); break;
					case TKN_EXP       : printf("^   " ); break;
					case TKN_ADD       : printf("+   " ); break;
					case TKN_SUB       : printf("-   " ); break;
					case TKN_MUL       : printf("*   " ); break;
					case TKN_DIV       : printf("/   " ); break;
					case TKN_MOD       : printf("%%   "); break;
					case TKN_AND       : printf("&   " ); break;
					case TKN_OR        : printf("|   " ); break;
					case TKN_NOT       : printf("!   " ); break;
					case TKN_LS        : printf("<   " ); break;
					case TKN_GT        : printf(">   " ); break;
					case TKN_LSE       : printf("=<  " ); break;
					case TKN_GTE       : printf(">=  " ); break;
					case TKN_EQL       : printf("=   " ); break;
					case TKN_R_ARROW   : printf("->  " ); break;
					case TKN_L_ARROW   : printf("<-  " ); break;
					case TKN_R_DARROW  : printf("=>  " ); break;
					case TKN_L_DARROW  : printf("<=  " ); break;
					
					case TKN_NEWLINE   : printf("NL  " ); break;
					default:             printf("TK  " ); break;
				}
			}break;
			
			case AL_TYDF : printf("TD  "); break;
			case AL_TYPE : printf("TY  "); break;
			case AL_TYLM : printf("LM  "); break;
			case AL_STRC : printf("ST  "); break;
			case AL_STLN : printf("S_  "); break;
			case AL_FNTY : printf("FT  "); break;
			case AL_FPRS : printf("FP  "); break;
			case AL_TPRS : printf("TP  "); break;
			case AL_UNON : printf("UN  "); break;
			case AL_ENUM : printf("EN  "); break;
			case AL_UNLN : printf("U_  "); break;
			case AL_TGUN : printf("TU  "); break;
			case AL_TULN : printf("T_  "); break;
			
			case AL_FNDF : printf("FD  "); break;
			case AL_FTPS : printf("FS  "); break;
			case AL_FUNC : printf("FN  "); break;
			case AL_EXPR : printf("XP  "); break;
			case AL_LTRL : printf("LT  "); break;
			case AL_FNCL : printf("FC  "); break;
			case AL_BNOP : printf("BO  "); break;
			case AL_UNOP : printf("UO  "); break;
			case AL_BLOK : printf("BK  "); break;
			case AL_LMDA : printf("LM  "); break;
			case AL_PARS : printf("PS  "); break;
			
			case AL_NIL  : printf("??  "); break;
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


ASTLine copyLine(ASTLine* ln){
	ASTLine ret;
	ret.lst   = malloc(sizeof(ASTList) * ln->size);
	ret.size  = ln->size;
	for(int i = 0; i < ln->size; i++) ret.lst[i] = ln->lst[i];
	return ret;
}


ASTLine copyNoComms(ASTLine* ln){
	ASTLine ret;
	ret.lst   = malloc(sizeof(ASTList) * ln->size);
	ret.size  = 0;
	for(int i = 0; i < ln->size; i++){
		if((ln->lst[i].kind != AL_TKN) || ((ln->lst[i].tk.type != TKN_COMMENT) && (ln->lst[i].tk.type != TKN_COMMS))){
			ret.lst[ret.size] = ln->lst[i];
			ret.size++;
		}
	}
	return ret;
}


int viewAt(ASTLine* x, ASTLine* n, int ix){
	if(ix >= x->size){
		n->lst  = NULL;
		n->size = 0;
		return 0;
	}
	n->lst  = &x->lst[ix];
	n->size =  x->size - ix;
	return n->size;
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


int cleanLines(ASTLine* x){
	int ix   = 0;
	int last = 0;
	int size = x->size;
	for(int i = 0; i < x->size; i++){
		int here = (x->lst[i].kind == AL_TKN) && ((x->lst[i].tk.type == TKN_NEWLINE) || (x->lst[i].tk.type == TKN_SEMICOLON));
		if(!(((ix == 0) && here) || ((i > 0) && here && last))){
			x->lst[ix] = x->lst[i];
			if((x->lst[i].kind == AL_TKN) && (x->lst[i].tk.type == TKN_SEMICOLON)) x->lst[i].tk.type = TKN_NEWLINE;
			ix++;
		}
	}
	x->size = ix;
	return ix < size;
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


int match(ASTLine* ln, ASTListKind* ks, int ct){
	if(ln->size < ct) return 0;
	for(int i = 0; i < ln->size; i++)
		if((i < ct) && (ln->lst[i].kind != ks[i])) return 0;
	return 1;
}


int tokenMatch(ASTLine* ln, TkType* ts, int ct){
	if(ln->size < ct) return 0;
	for(int i = 0; i < ln->size; i++)
		if((i < ct) && (ts[i] != TKN_VOID) && ((ln->lst[i].kind != AL_TKN) || (ln->lst[i].tk.type != ts[i]))) return 0;
	return 1;
}


int filter(ASTLine* ln, ASTLine* ret, ASTListKind k){
	*ret = makeASTLine(ln->size);
	int ix = 0;
	for(int i = 0; i < ln->size; i++){
		if(ln->lst[i].kind != k){
			ret->lst[ix] = ln->lst[i];
			ix++;
		}
	}
	ret->size = ix;
	return ret->size;
}


int filterToken(ASTLine* ln, ASTLine* ret, TkType t){
	*ret = makeASTLine(ln->size);
	int ix = 0;
	for(int i = 0; i < ln->size; i++){
		if((ln->lst[i].kind != AL_TKN) || (ln->lst[i].tk.type != t)){
			ret->lst[ix] = ln->lst[i];
			ix++;
		}
	}
	ret->size = ix;
	return ret->size;
}


int filterInline(ASTLine* ln, ASTListKind k){
	int ix = 0;
	for(int i = 0; i < ln->size; i++){
		if(ln->lst[i].kind != k){
			ln->lst[ix] = ln->lst[i];
			ix++;
		}
	}
	ln->size = ix;
	return ln->size;
}


int filterTokenInline(ASTLine* ln, TkType t){
	int ix = 0;
	for(int i = 0; i < ln->size; i++){
		if((ln->lst[i].kind != AL_TKN) || (ln->lst[i].tk.type != t)){
			ln->lst[ix] = ln->lst[i];
			ix++;
		}
	}
	ln->size = ix;
	return ln->size;
}


typedef struct{
	ASTList* stk;
	int      size, head;
}ASTStack;

int astStackPop (ASTStack* stk, ASTList* ret){
	if(stk->head > 0){
		stk->head--;
		*ret = stk->stk[stk->head];
		return 1;
	}
	printf("Stack underflow!\n");
	return 0;
}

int astStackPush(ASTStack* stk, ASTList* val){
	if(stk->head < stk->size){
		stk->stk[stk->head] = *val;
		stk->head++;
		return 1;
	}
	printf("Stack overflow!\n");
	return 0;
}


ASTStack lineToStack(ASTLine* ln){
	ASTStack ret;
	ret.size = ln->size * 2;
	ret.stk  = malloc(sizeof(ASTList) * ln->size * 2);
	ret.head = ln->size;
	for(int i = 0; i < ln->size; i++) ret.stk[i] = ln->lst[i];
	return ret;
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
	filterTokenInline(&ln, TKN_COMMENT);
	filterTokenInline(&ln, TKN_COMMS);
	int cont = 1;
	ASTLine a = ln, b;
	while(cont){
		/*
		ASTLine x = a;
		cont = viewSplitOnToken(&x, &a, &b, TKN_NEWLINE);
		ASTTyDef tydf;
		if(parseTyDef(&a, errs, &tydf)){a = b; continue;}
		ASTFnDef fndf;
		if(parseFnDef(&a, errs, &fndf)){a = b; continue;}
		printf("WTF @ %i\n", a.lst[0].pos.lineStart);
		a = b;*/
	}
	

	return 0;
}




