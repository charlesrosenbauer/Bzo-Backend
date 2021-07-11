#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "program.h"
#include "error.h"
#include "util.h"
#include "ast.h"



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
	AL_FNTY,
	AL_UNON,
	AL_ENUM,
	AL_TGUN,
	
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
	
	// List parsing
	AL_TPRS,
	AL_TPAR,
	AL_NPRS,
	AL_NPAR,
	AL_STLN,
	AL_STLS,
	AL_UNLN,
	AL_UNLS,
	AL_TULN,
	AL_TULS,
	
	// Misc
	AL_LOC,
	
	// Program File control
	AL_HEAD,
	AL_DEFS,
	AL_PROG
}ASTListKind;

typedef struct{
	void    *pa, *pb;
	int64_t  ia,  ib;
}Tuple;

typedef struct{
	Position   pos;	// We should try to use the position in the token/union, as that would cut struct size by 25%
	union{
		Token  tk;
		void*  here;
		Tuple  tp;
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
		case AL_FNTY : printf("FNTY "); break;
		case AL_UNON : printf("UNON "); break;
		case AL_ENUM : printf("ENUM "); break;
		case AL_TGUN : printf("TGUN "); break;
	
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
		
		// Pars
		case AL_TPRS : printf("TPRS "); break;
		case AL_TPAR : printf("TPAR "); break;
		case AL_NPRS : printf("NPRS "); break;
		case AL_NPAR : printf("NPAR "); break;
		case AL_STLN : printf("STLN "); break;
		case AL_STLS : printf("STLS "); break;
		case AL_UNLN : printf("UNLN "); break;
		case AL_UNLS : printf("UNLS "); break;
		case AL_TULN : printf("TULN "); break;
		case AL_TULS : printf("TULS "); break;
		
		// Misc
		case AL_LOC  : printf("LOCT "); break;
		
		// Control
		case AL_HEAD : printf("HEAD "); break;
		case AL_DEFS : printf("DEFS "); break;
		case AL_PROG : printf("PROG "); break;
		
		// NIL
		case AL_NIL : {
			printf("<> ");
			//leftpad(pad);
		}break;
	}
}


void formatLocs(LexerState* tks){
	int locHead = -1;
	for(int i = 0; i < tks->tkct; i++){
		if((i > 1) && (tks->tks[i-1].type == TKN_WHERE)){
			if      (tks->tks[i-2].type == TKN_S_ID  ){
				if((tks->tks[i].type == TKN_S_ID) || (tks->tks[i].type == TKN_S_TYID)){
					LocToken lt;
					lt.path = malloc(sizeof(uint64_t) * 2);
					lt.len  = 2;
					lt.path[0] = tks->tks[i-2].data.i64;
					lt.path[1] = tks->tks[i  ].data.i64;
					Position pos  = fusePosition(tks->tks[i-2].pos, tks->tks[i].pos);
					tks->tks[i-2] = (Token){TKN_LOCID, pos, (TokenData)lt};
					tks->tks[i-1].type = TKN_VOID;
					tks->tks[i  ].type = TKN_VOID;
					locHead = i-2;
				}
			}else if(tks->tks[i-2].type == TKN_S_TYID){
				if((tks->tks[i].type == TKN_S_ID) || (tks->tks[i].type == TKN_S_TYID)){
					LocToken lt;
					lt.path = malloc(sizeof(uint64_t) * 2);
					lt.len  = 2;
					lt.path[0] = tks->tks[i-2].data.i64;
					lt.path[1] = tks->tks[i  ].data.i64;
					Position pos  = fusePosition(tks->tks[i-2].pos, tks->tks[i].pos);
					tks->tks[i-2] = (Token){TKN_LOCTY, pos, (TokenData)lt};
					tks->tks[i-1].type = TKN_VOID;
					tks->tks[i  ].type = TKN_VOID;
					locHead = i-2;
				}
			}else if((tks->tks[i-2].type == TKN_VOID) && (locHead != -1)){
				if((tks->tks[i].type == TKN_S_ID) || (tks->tks[i].type == TKN_S_TYID)){
					LocToken lt = tks->tks[locHead].data.loc;
					uint64_t* path = malloc(sizeof(uint64_t) * (lt.len + 1));
					for(int j = 0; j < lt.len; j++) path[j] = lt.path[j];
					path[lt.len] = tks->tks[i].data.i64;
					free(lt.path);
					lt.path = path;
					lt.len++;
					Position pos  = fusePosition(tks->tks[locHead].pos, tks->tks[i].pos); 
					tks->tks[locHead].pos  = pos;
					tks->tks[locHead].data = (TokenData)lt;
					tks->tks[i-1    ].type = TKN_VOID;
					tks->tks[i      ].type = TKN_VOID;
				}else{
					locHead = -1;
				}
			}else{
				locHead = -1;
			}
		}else if(tks->tks[i-1].type != TKN_VOID){
			locHead = -1;
		}
	}
	int ix = 0;
	for(int i = 0; i < tks->tkct; i++){
		if(tks->tks[i].type != TKN_VOID){
			tks->tks[ix] = tks->tks[i];
			ix++;
		}
	}
	tks->tkct = ix;
}


int unwrap(LexerState* tks, ErrorList* errs, ASTList** list){
	formatLocs(tks);
	//printLexerState(*tks);
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
					case TKN_S_TVAR    : printf("TV  " ); break;
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
			case AL_FNTY : printf("FT  "); break;
			case AL_UNON : printf("UN  "); break;
			case AL_ENUM : printf("EN  "); break;
			case AL_TGUN : printf("TU  "); break;
			
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
			
			case AL_TPRS : printf("TPS "); break;
			case AL_TPAR : printf("TP  "); break;
			case AL_NPRS : printf("NPS "); break;
			case AL_NPAR : printf("NP  "); break;
			case AL_STLN : printf("S_  "); break;
			case AL_STLS : printf("S_S "); break;
			case AL_UNLN : printf("U_  "); break;
			case AL_UNLS : printf("U_S "); break;
			case AL_TULN : printf("T_  "); break;
			case AL_TULS : printf("T_S "); break;
			
			case AL_LOC  : printf("LC  "); break;
			
			case AL_DEFS : printf("DF  "); break;
			case AL_HEAD : printf("HD  "); break;
			case AL_PROG : printf("PG  "); break;
			
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


void printASTStack(ASTStack ast){
	ASTLine ln = (ASTLine){ast.stk, ast.head};
	printASTLine(ln);
}


ASTStack makeEmptyStack(int size){
	ASTStack ret;
	ret.size = size;
	ret.stk  = malloc(sizeof(ASTList) * size);
	ret.head = 0;
	return ret;
}


int astStackPop (ASTStack* stk, ASTList* ret){
	if(stk->head > 0){
		stk->head--;
		*ret = stk->stk[stk->head];
		return 1;
	}
	printf("Stack underflow!\n");
	return 0;
}


int astStackPeek(ASTStack* stk, int ix, ASTList* ret){
	if(stk->head-ix > 0){
		*ret = stk->stk[stk->head-(ix+1)];
		return 1;
	}
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
	for(int i = 0; i < ln->size; i++) ret.stk[(ln->size-1) - i] = ln->lst[i];
	return ret;
}



/*
	Actual Parser Rules
*/


// If it is a binop, it returns precedence
int isBinop(TkType t){
	switch(t){
		case TKN_ADD   : return 6;
		case TKN_SUB   : return 6;
		case TKN_MUL   : return 5;
		case TKN_DIV   : return 4;
		case TKN_MOD   : return 2;
		case TKN_EXP   : return 3;
		case TKN_AND   : return 9;
		case TKN_OR    : return 9;
		case TKN_XOR   : return 9;
		case TKN_SHL   : return 7;
		case TKN_SHR   : return 7;
		case TKN_GT    : return 8;
		case TKN_GTE   : return 8;
		case TKN_LS    : return 8;
		case TKN_LSE   : return 8;
		case TKN_EQL   : return 8;
		case TKN_NEQ   : return 8;
		case TKN_PERIOD: return 1;
		// precedence 2 is index
		default :        return 0;
	}
}

int isUnop(TkType t){
	switch(t){
		case TKN_SUB     : return 1;
		case TKN_EXP     : return 1;
		case TKN_L_ARROW : return 1;
		case TKN_NOT     : return 1;
		default          : return 0;
	}
}


// HIGH PRIORITY FIXME: This fails silently in cases where it should create an error!
int parseStep(ASTStack* tks, ASTStack* stk, int printErrs, ASTListKind k, void** ret){
	ASTList tk;
	if((tks->head == 0) && (stk->head == 1) && (stk->stk[0].kind == k)){
		*ret = stk->stk[0].here;
		return 0;
	}else if((tks->head == 0) && (stk->head == 1)){
		if(printErrs) printf("Invalid parsing result.\n");
		return 0;
	}else if(astStackPop(tks, &tk)){
		if(!astStackPush(stk, &tk)){ printf("AST Stack overflow.\n"); exit(-1); }
		return 1;
	}else if(stk->head > 1){
		if(printErrs) printf("Parser could not consume file.\n");
		return 0;
	}else{
		printf("WTF?\n");
		return 1;
	}
	return 0;
}


int exprParser(ASTStack*, ASTStack*, ErrorList*, ASTExpr*);


int parParser(ASTStack* stk, ASTStack* tks, ErrorList* errs, ASTExprPars* ret){
	
	int cont = 1;
	while(cont){
		// PARS  |  EXPR ,
		
		// PARS  |  PARS EXPR ,
		
		// PARS  |  PARS EXPR EOF
	}
	return 1;
}


int lmdaParParser(ASTStack* stk, ASTStack* tks, ErrorList* errs, ASTExpr* ret){


	return 1;
}


int exprParser(ASTStack* stk, ASTStack* tks, ErrorList* errs, ASTExpr* ret){
	
	int cont = 1;
	while(cont){
	
		ASTList x0, x1, x2, x3;
	
		// Int / Flt / Str / Tag
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TKN)){
			switch(x0.tk.type){
				case TKN_INT    : {} continue;
				case TKN_FLT    : {} continue;
				case TKN_STR    : {} continue;
				case TKN_TAG    : {} continue;
				default: break;
			}
		}
		
		
		// [ Expr ]
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_BRK)){
		
		}
		
		
		// Id / MId
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TKN) &&
		  ((x0.tk.type == TKN_S_ID) || (x0.tk.type == TKN_S_MID))){
		
		}
		
		// Loc
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_LOC)){
		
		}
		
		
		// ( Expr )
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_PAR)){
			
		}
		
		
		// [ pars ] { block }
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_BRC) &&
		   astStackPeek(stk, 1, &x1) && (x1.kind == AL_BRK)){
		   
		}
		
		
		// [ pars ] ! { block }
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_BRC) &&
		   astStackPeek(stk, 1, &x1) && (x1.kind == AL_TKN) && (x1.tk.type == TKN_NOT) &&
		   astStackPeek(stk, 2, &x2) && (x2.kind == AL_BRK)){
		   
		}
		
		
		// Switch
		
		
		// Ife
		
		
		// [ Expr : pars ]
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_BRK)){
		
		}
		
		
		// [ TyId : pars ]
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_BRK)){
		
		}
		
		
		// Expr Binop Expr
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_EXPR) &&
		   astStackPeek(stk, 1, &x1) && (x1.kind == AL_TKN)  && isBinop(x1.tk.type) &&
		   astStackPeek(stk, 2, &x2) && (x2.kind == AL_EXPR)){
		   
		}
		
		// Unop Expr
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_EXPR) &&
		   astStackPeek(stk, 1, &x1) && (x1.kind == AL_TKN)  && isUnop(x1.tk.type)){
		   
		}
		
		// No rules, grab another token
		void* xval;
		if(!parseStep(tks, stk, 0, AL_EXPR, &xval)){
			*ret = *(ASTExpr*)xval;
			cont = 0;
		}
	}
	return 1;
}



// This functions as a set of rules that can be inserted into a different loop, not a parse loop in and of itself
int tyElemParser(ASTStack* stk, ASTStack* tks, ErrorList* errs){
	
	ASTList x0, x1, x2, x3;
		
	// TyId / BId / TVar
	if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TKN) &&
	  ((x0.tk.type == TKN_S_TYID) || (x0.tk.type == TKN_S_BID) || (x0.tk.type == TKN_S_TVAR))){
	  
	  	if(!(astStackPeek(tks, 0, &x3) && (x3.kind == AL_TKN) && (x3.tk.type == TKN_COLON))){
			x0.kind       = AL_TYLM;
			ASTTyElem* lm = malloc(sizeof(ASTTyElem));
			x0.here       = lm;
			*lm           = makeASTTyElem(stk->head + 3);
			lm->pos       = x0.tk.pos;
			lm->tyid      = x0.tk.data.i64;
			stk->stk[stk->head-1] = x0;
			return 1;
		}
	}
		
		
	// [] TyElem
	if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TYLM) &&
	   astStackPeek(stk, 1, &x1) && (x1.kind == AL_BRK)  &&
	  (x1.here == NULL)){
	  
	  	ASTTyElem* lm = x0.here;
	  	x0.pos        = fusePosition(x1.pos, x0.pos);
	  	appendASTTyElem(lm, 0);
	  	stk->head--;
		stk->stk[stk->head-1] = x0;
		return 1;
	}
		
	// [Int] TyElem
	if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TYLM) &&
	   astStackPeek(stk, 1, &x1) && (x1.kind == AL_BRK)  &&
	  (x1.here != NULL)){
	  	// TODO: Get size from BRK
		ASTTyElem* lm = x0.here;
		x0.pos        = fusePosition(x1.pos, x0.pos);
	  	appendASTTyElem(lm, 1);
	  	stk->head--;
		stk->stk[stk->head-1] = x0;
	 	return 1;
	}
		
		
	// ^ TyElem
	if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TYLM) &&
	   astStackPeek(stk, 1, &x1) && (x1.kind == AL_TKN)  && (x1.tk.type == TKN_EXP)){
	 	 
		ASTTyElem* lm = x0.here;
		x0.pos        = fusePosition(x1.pos, x0.pos);
	  	appendASTTyElem(lm, -1);
	  	stk->head--;
		stk->stk[stk->head-1] = x0;
		return 1;
	}
		
	
	return 0;
}

int parseType(ASTListKind, ASTList*, ErrorList*, ASTType*);

int unionParser(ASTStack* ast, ASTStack* tks, ErrorList* errs, ASTUnion* ret){

	int cont = 1;
	while(cont){
		/*
			TODO:
			* handle union headers
			* correctly fail with wrong header
		*/
		printf("UN %i %i | ", tks->head, ast->head);
		printASTStack(*ast);
	
		ASTList x0, x1, x2, x3, x4, x5;
		
		// Parse Type Elements
		if(tyElemParser(ast, tks, errs)) continue;
		
		// UL =		Int = TyId : TyElem
		if(astStackPeek(ast, 0, &x0) && (x0.kind == AL_TYLM) &&
		   astStackPeek(ast, 1, &x1) && (x1.kind == AL_TKN ) && (x1.tk.type == TKN_COLON ) &&
		   astStackPeek(ast, 2, &x2) && (x2.kind == AL_TKN ) && (x2.tk.type == TKN_S_TYID) && 
		   astStackPeek(ast, 3, &x3) && (x3.kind == AL_TKN ) && (x3.tk.type == TKN_EQL   ) &&
		   astStackPeek(ast, 4, &x4) && (x4.kind == AL_TKN ) && (x4.tk.type == TKN_INT   )){
		   
		    int neg = (astStackPeek(ast, 5, &x5) && (x5.kind == AL_TKN) && (x5.tk.type == TKN_SUB));
			Position pos = neg? fusePosition(x5.tk.pos, x0.pos) : fusePosition(x4.tk.pos, x0.pos);
			int        id = x2.tk.data.i64;
			ASTTyElem* lm = x0.here;
			x2.tp.ia      = id;
			x2.tp.pa      = lm;
			x2.tp.ib      = x4.tk.data.i64 * (neg? -1 : 1);
			x2.pos        = pos;
			x2.kind       = AL_UNLN;
			ast->head    -= (5 + neg);
			astStackPush(ast, &x2);
			continue;
		}
		
		// UL =		Int = TyId : []		|		Int = TyId : ()
		if(astStackPeek(ast, 0, &x0) && ((x0.kind == AL_BRK ) || (x0.kind    == AL_PAR    )) &&
		   astStackPeek(ast, 1, &x1) &&  (x1.kind == AL_TKN ) && (x1.tk.type == TKN_COLON ) &&
		   astStackPeek(ast, 2, &x2) &&  (x2.kind == AL_TKN ) && (x2.tk.type == TKN_S_TYID) && 
		   astStackPeek(ast, 3, &x3) &&  (x3.kind == AL_TKN ) && (x3.tk.type == TKN_EQL   ) &&
		   astStackPeek(ast, 4, &x4) &&  (x4.kind == AL_TKN ) && (x4.tk.type == TKN_INT   )){
		   	ASTType type;
		   	if(parseType(x0.kind, x0.here, errs, &type)){
		   		int neg = (astStackPeek(ast, 5, &x5) && (x5.kind == AL_TKN) && (x5.tk.type == TKN_SUB));
				Position pos = neg? fusePosition(x5.tk.pos, x0.pos) : fusePosition(x4.tk.pos, x0.pos);
				int       id = x2.tk.data.i64;
				ASTType*  ty = malloc(sizeof(ASTType));
				*ty          = type;
				x2.tp.ia     = id;
				x2.tp.pa     = ty;
				x2.tp.ib     = x4.tk.data.i64 * (neg? -1 : 1);
				x2.pos       = pos;
				x2.kind      = AL_UNLN;
				ast->head   -= (5 + neg);
				astStackPush(ast, &x2);
				continue;
			}
		}
		
		
		// UL =		TyId : TyElem
		if(astStackPeek(ast, 0, &x0) && (x0.kind == AL_TYLM) &&
		   astStackPeek(ast, 1, &x1) && (x1.kind == AL_TKN ) && (x1.tk.type == TKN_COLON ) &&
		   astStackPeek(ast, 2, &x2) && (x2.kind == AL_TKN ) && (x2.tk.type == TKN_S_TYID)){
			Position pos = fusePosition(x2.tk.pos, x0.pos);
			int        id = x2.tk.data.i64;
			ASTTyElem* lm = x0.here;
			x2.tp.ia      = id;
			x2.tp.pa      = lm;
			x2.tp.pb      = 0;		// This should be handled in some other way
			x2.pos        = pos;
			x2.kind       = AL_UNLN;
			ast->head   -= 3;
			astStackPush(ast, &x2);
			continue;
		}
		
		
		// UL =		TyId : []				|		TyId : ()
		if(astStackPeek(ast, 0, &x0) && ((x0.kind == AL_BRK ) || (x0.kind    == AL_PAR    )) &&
		   astStackPeek(ast, 1, &x1) &&  (x1.kind == AL_TKN ) && (x1.tk.type == TKN_COLON ) &&
		   astStackPeek(ast, 2, &x2) &&  (x2.kind == AL_TKN ) && (x2.tk.type == TKN_S_TYID)){
		   	ASTType type;
		   	if(parseType(x0.kind, x0.here, errs, &type)){
				Position pos = fusePosition(x2.tk.pos, x0.pos);
				int       id = x2.tk.data.i64;
				ASTType*  ty = malloc(sizeof(ASTType));
				*ty          = type;
				x2.tp.ia     = id;
				x2.tp.pa     = ty;
				x2.tp.pb     = 0;	// This should be handled in some other way
				x2.pos       = pos;
				x2.kind      = AL_UNLN;
				ast->head   -= 3;
				astStackPush(ast, &x2);
				continue;
			}
		}
		
		
		// ULS =	UL NL UL
		if(astStackPeek(ast, 0, &x0) && (x0.kind == AL_UNLN) &&
		   astStackPeek(ast, 1, &x1) && (x1.kind == AL_TKN ) &&((x1.tk.type == TKN_NEWLINE) || (x1.tk.type == TKN_SEMICOLON)) &&
		   astStackPeek(ast, 2, &x2) && (x2.kind == AL_UNLN)){
			ASTUnion*  unon = malloc(sizeof(ASTUnion));
			*unon           = makeASTUnion(4);
			Position    pos = fusePosition(x2.pos, x0.pos);
			appendASTUnion (unon, *(ASTType*)x0.tp.pa, x0.tp.ia, x0.tp.ib);		free(x0.tp.pa);
			appendASTUnion (unon, *(ASTType*)x2.tp.pa, x2.tp.ia, x0.tp.ib);		free(x2.tp.pa);
			x2.pos          = pos;
			x2.kind         = AL_UNLS;
			x2.here         = unon;
			ast->head   -= 3;
			astStackPush(ast, &x2);
			continue;
		}
		
		
		// ULS =	ULS NL IL
		if(astStackPeek(ast, 0, &x0) && (x0.kind == AL_UNLN) &&
		   astStackPeek(ast, 1, &x1) && (x1.kind == AL_TKN ) &&((x1.tk.type == TKN_NEWLINE) || (x1.tk.type == TKN_SEMICOLON)) &&
		   astStackPeek(ast, 2, &x2) && (x2.kind == AL_UNLS)){
			ASTUnion*  unon = x2.here;
			x2.pos          = fusePosition(x2.pos, x0.pos);
			appendASTUnion (unon, *(ASTType*)x0.tp.pa, x0.tp.ia, x0.tp.ib);		free(x0.tp.pa);
			ast->head   -= 3;
			astStackPush(ast, &x2);
			continue;
		}
		
		
		// NL = NL NL
		if(astStackPeek(ast, 0, &x0) && (x0.kind == AL_TKN) && ((x0.tk.type == TKN_NEWLINE) || (x0.tk.type == TKN_SEMICOLON)) &&
		   astStackPeek(ast, 1, &x1) && (x1.kind == AL_TKN) && ((x1.tk.type == TKN_NEWLINE) || (x1.tk.type == TKN_SEMICOLON))){
			ast->head--;
			continue;
		}
		
		// NL EOF
		if(astStackPeek(ast, 0, &x0) && (x0.kind == AL_TKN) &&
		 ((x0.tk.type == TKN_NEWLINE) || (x0.tk.type == TKN_SEMICOLON)) && (tks->head == 0)){
			ast->head--;
			continue;
		}
		
		// SOF NL
		if(astStackPeek(ast, 0, &x0) && (x0.kind == AL_TKN) &&
		 ((x0.tk.type == TKN_NEWLINE) || (x0.tk.type == TKN_SEMICOLON)) && (ast->head == 1)){
			ast->head--;
			continue;
		}
		
		void* xval;
		if(!parseStep(tks, ast, 0, AL_UNLS, &xval)){
			*ret = *(ASTUnion*)xval;
			cont = 0;
		}
	}
	return 1;
}

int enumParser(ASTStack* ast, ASTStack* tks, ErrorList* errs, ASTEnum* ret){

	int cont = 0;
	while(cont){
		ASTList x0, x1, x2, x3, x4;
		
		// Fill me out
	}
	return 1;
}

int structParser(ASTStack* ast, ASTStack* tks, ErrorList* errs, ASTStruct* ret){

	int cont = 1;
	while(cont){
		printf("ST %i %i | ", tks->head, ast->head);
		printASTStack(*ast);
	
		ASTList x0, x1, x2, x3;
		
		// Parse Type Elements
		if(tyElemParser(ast, tks, errs)) continue;
		
		// SL =		Id : TyElem
		if(astStackPeek(ast, 0, &x0) && (x0.kind == AL_TYLM) &&
		   astStackPeek(ast, 1, &x1) && (x1.kind == AL_TKN ) && (x1.tk.type == TKN_COLON) &&
		   astStackPeek(ast, 2, &x2) && (x2.kind == AL_TKN ) && (x2.tk.type == TKN_S_ID)){
			Position pos = fusePosition(x2.tk.pos, x0.pos);
			int        id = x2.tk.data.i64;
			ASTTyElem* lm = x0.here;
			x2.tp.ia      = id;
			x2.tp.pa      = lm;
			x2.pos        = pos;
			x2.kind       = AL_STLN;
			ast->head   -= 3;
			astStackPush(ast, &x2);
			continue;
		}
		
		// SL =		Id : []		|		Id : ()
		if(astStackPeek(ast, 0, &x0) && ((x0.kind == AL_BRK ) || (x0.kind == AL_PAR)) &&
		   astStackPeek(ast, 1, &x1) && (x1.kind == AL_TKN ) && (x1.tk.type == TKN_COLON) &&
		   astStackPeek(ast, 2, &x2) && (x2.kind == AL_TKN ) && (x2.tk.type == TKN_S_ID)){
		   	ASTType type;
		   	if(parseType(x0.kind, x0.here, errs, &type)){
				Position pos = fusePosition(x2.tk.pos, x0.pos);
				int       id = x2.tk.data.i64;
				ASTType*  ty = malloc(sizeof(ASTType));
				*ty          = type;
				x2.tp.ia     = id;
				x2.tp.pa     = ty;
				x2.pos       = pos;
				x2.kind      = AL_STLN;
				ast->head   -= 3;
				astStackPush(ast, &x2);
				continue;
			}
		}
		
		
		// SLS =	SL NL SL
		if(astStackPeek(ast, 0, &x0) && (x0.kind == AL_STLN) &&
		   astStackPeek(ast, 1, &x1) && (x1.kind == AL_TKN ) &&((x1.tk.type == TKN_NEWLINE) || (x1.tk.type == TKN_SEMICOLON)) &&
		   astStackPeek(ast, 2, &x2) && (x2.kind == AL_STLN)){
			ASTStruct* strc = malloc(sizeof(ASTStruct));
			*strc           = makeASTStruct(4);
			Position    pos = fusePosition(x2.pos, x0.pos);
			appendASTStruct(strc, *(ASTType*)x0.tp.pa, x0.tp.ia);		free(x0.tp.pa);
			appendASTStruct(strc, *(ASTType*)x2.tp.pa, x2.tp.ia);		free(x2.tp.pa);
			x2.pos          = pos;
			x2.kind         = AL_STLS;
			x2.here         = strc;
			ast->head   -= 3;
			astStackPush(ast, &x2);
			continue;
		}
		
		
		// SLS =	SLS NL SL
		if(astStackPeek(ast, 0, &x0) && (x0.kind == AL_STLN) &&
		   astStackPeek(ast, 1, &x1) && (x1.kind == AL_TKN ) &&((x1.tk.type == TKN_NEWLINE) || (x1.tk.type == TKN_SEMICOLON)) &&
		   astStackPeek(ast, 2, &x2) && (x2.kind == AL_STLS)){
			ASTStruct* strc = x2.here;
			x2.pos          = fusePosition(x2.pos, x0.pos);
			appendASTStruct(strc, *(ASTType*)x0.tp.pa, x0.tp.ia);		free(x0.tp.pa);
			ast->head   -= 3;
			astStackPush(ast, &x2);
			continue;
		}
		
		// NL = NL NL
		if(astStackPeek(ast, 0, &x0) && (x0.kind == AL_TKN) && ((x0.tk.type == TKN_NEWLINE) || (x0.tk.type == TKN_SEMICOLON)) &&
		   astStackPeek(ast, 1, &x1) && (x1.kind == AL_TKN) && ((x1.tk.type == TKN_NEWLINE) || (x1.tk.type == TKN_SEMICOLON))){
			ast->head--;
			continue;
		}
		
		// NL EOF
		if(astStackPeek(ast, 0, &x0) && (x0.kind == AL_TKN) &&
		 ((x0.tk.type == TKN_NEWLINE) || (x0.tk.type == TKN_SEMICOLON)) && (tks->head == 0)){
			ast->head--;
			continue;
		}
		
		// SOF NL
		if(astStackPeek(ast, 0, &x0) && (x0.kind == AL_TKN) &&
		 ((x0.tk.type == TKN_NEWLINE) || (x0.tk.type == TKN_SEMICOLON)) && (ast->head == 1)){
			ast->head--;
			continue;
		}
		
		void* xval;
		if(!parseStep(tks, ast, 0, AL_STLS, &xval)){
			*ret = *(ASTStruct*)xval;
			cont = 0;
		}
	}
	return 1;
}


int parseBlock(ASTList* blk, ErrorList* errs, ASTBlock* ret){
	ASTLine  ln  = toLine(blk);
	ASTStack tks = lineToStack(&ln);
	ASTStack ast = makeEmptyStack(ln.size);
	
	ret->stmct = 0;
	
	int cont = 0;
	while(cont){
		printf("BK %i %i | ", tks.head, ast.head);
		printASTStack(ast);
		
		ASTList x0, x1, x2;
		
		
		
		// Comment Removal
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_COMMENT  )){ast.head--; continue; }
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_COMMS    )){ast.head--; continue; }
		
		
		void* xval;
		if(!parseStep(&tks, &ast, 0, AL_BLOK, &xval)){
			*ret = *(ASTBlock*)xval;
			cont = 0;
		}
	}
	
	free(ln.lst);
	free(ast.stk);
	free(tks.stk);
	return 1;
}


// Type  Pars |  [ TyElem , TyElem ]
int parseTPars(ASTList* tps, ErrorList* errs, ASTPars* ret){
	ASTLine  ln  = toLine(tps);
	ASTStack tks = lineToStack(&ln);
	ASTStack ast = makeEmptyStack(ln.size);
	
	ret->prct = 0;
	ret->lbls = NULL;
	ret->pars = NULL;
	
	int cont = 1;
	while(cont){
		printf("TP %i %i | ", tks.head, ast.head);
		printASTStack(ast);
		
		ASTList x0, x1, x2;
		
		// Parse Type Elements
		if(tyElemParser(&ast, &tks, errs)) continue;
		
		// FIXME: position calculations below have issues. x.tk.pos is not always valid.
		
		// TPRS = TyElem , TyElem
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TYLM) &&
		   astStackPeek(&ast, 1, &x1) && (x1.kind == AL_TKN ) && (x1.tk.type == TKN_COMMA) &&
		   astStackPeek(&ast, 2, &x2) && (x2.kind == AL_TYLM)){
			
			ASTPars  new = makeASTPars(tks.size / 2);
		 	new.pos      = fusePosition(x0.tk.pos, x2.tk.pos);
		 	ASTTyElem  a = *(ASTTyElem*)x2.here;
		 	ASTTyElem  b = *(ASTTyElem*)x0.here;
		 	appendASTPars(&new, a, -1);
		 	appendASTPars(&new, b, -1);
		 	ast.head    -= 3;
		 	
		 	ASTList tp;
			tp.here = malloc(sizeof(ASTPars));
			tp.kind = AL_TPRS;
			tp.here = &new;
			astStackPush(&ast, &tp);
			continue;
		}
		
		// TPRS = TPRS   , TyElem
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TYLM) &&
		   astStackPeek(&ast, 1, &x1) && (x1.kind == AL_TKN ) && (x1.tk.type == TKN_COMMA) &&
		   astStackPeek(&ast, 2, &x2) && (x2.kind == AL_TPRS)){
			
			ASTPars* old = x2.here;
		 	old->pos     = fusePosition(old->pos, x2.tk.pos);
		 	ASTTyElem lm = *(ASTTyElem*)x0.here;
		 	appendASTPars(old, lm, -1);
		 	ast.head    -= 3;
		 	
		 	ASTList tp;
			tp.here = malloc(sizeof(ASTPars));
			tp.kind = AL_TPRS;
			tp.here = old;
			astStackPush(&ast, &tp);
			continue;
		}
		
		
		// TPRS = TyElem  EOF
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TYLM) && (tks.head == 0)){
			ASTPars  new = makeASTPars(tks.size / 2);
		 	new.pos      = x0.pos;
		 	ASTTyElem lm = *(ASTTyElem*)x0.here;
		 	appendASTPars(&new, lm, -1);
		 	ast.head    --;
		 	
		 	ASTList np;
			np.here = malloc(sizeof(ASTPars));
			np.kind = AL_TPRS;
			np.here = &new;
			astStackPush(&ast, &np);
			continue;
		}
		
		
		
		// Comment and Newline Removal
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_COMMENT  )){ast.head--; continue; }
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_COMMS    )){ast.head--; continue; }
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_NEWLINE  )){ast.head--; continue; }
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_SEMICOLON)){ast.head--; continue; }
		
		
		void* xval;
		if(!parseStep(&tks, &ast, 0, AL_TPRS, &xval)){
			*ret = *(ASTPars*)xval;
			cont = 0;
		}
	}
	
	if(ret->prct < 1){
		if(ret->lbls != NULL){ free(ret->lbls); ret->lbls = NULL; }
		if(ret->pars != NULL){ free(ret->pars); ret->pars = NULL; }
	}
	
	free(ln.lst);
	free(ast.stk);
	free(tks.stk);
	return 1;
}


// Named Pars | [ a : TyElem , b : TyElem ]
int parseNPars(ASTList* nps, ErrorList* errs, ASTPars* ret, int isVars){
	ASTLine  ln  = toLine(nps);
	ASTStack tks = lineToStack(&ln);
	ASTStack ast = makeEmptyStack(ln.size);
	
	int cont = 1;
	while(cont){
		printf("NP %i %i | ", tks.head, ast.head);
		printASTStack(ast);
		
		ASTList x0, x1, x2;
		
		// Parse Type Elements
		if(tyElemParser(&ast, &tks, errs)) continue;
		
		// FIXME: position calculations below have issues. x.tk.pos is not always valid.
		
		// NPAR = Id : TyElem
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TYLM) &&
		   astStackPeek(&ast, 1, &x1) && (x1.kind == AL_TKN ) && (x1.tk.type == TKN_COLON) &&
		   astStackPeek(&ast, 2, &x2) && (x2.kind == AL_TKN ) &&
		   
		   ((isVars && ((x2.tk.type == TKN_S_ID)  || (x2.tk.type == TKN_S_MID))) ||
		   (!isVars &&  (x2.tk.type == TKN_S_TVAR)))){
		 	
		 	ASTParam npar;
		 	npar.pos   = fusePosition(x2.tk.pos, x0.tk.pos);
		 	npar.elem  = *(ASTTyElem*)x0.here;
		 	npar.label = x2.tk.data.i64;
		 	ast.head -= 3;
		 	
		 	ASTList np;
			np.here = malloc(sizeof(ASTParam));
			np.kind = AL_NPAR;
			*(ASTParam*)np.here = npar;
			astStackPush(&ast, &np);
		 	continue;  
		}
		
		
		// NPRS = NPAR , NPAR
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_NPAR) &&
		   astStackPeek(&ast, 1, &x1) && (x1.kind == AL_TKN ) && (x1.tk.type == TKN_COMMA) &&
		   astStackPeek(&ast, 2, &x2) && (x2.kind == AL_NPAR)){
		   
		    ASTPars  new = makeASTPars(tks.size / 2);
		 	new.pos      = fusePosition(x0.tk.pos, x2.tk.pos);
		 	ASTParam   a = *(ASTParam*)x2.here;
		 	ASTParam   b = *(ASTParam*)x0.here;
		 	appendASTPars(&new, a.elem, a.label);
		 	appendASTPars(&new, b.elem, b.label);
		 	ast.head    -= 3;
		 	
		 	ASTList np;
			np.here = malloc(sizeof(ASTPars));
			np.kind = AL_NPRS;
			np.here = &new;
			astStackPush(&ast, &np);
			continue;
		}
		
		// NPRS = NPRS   , NPAR
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_NPAR) &&
		   astStackPeek(&ast, 1, &x1) && (x1.kind == AL_TKN ) && (x1.tk.type == TKN_COMMA) &&
		   astStackPeek(&ast, 2, &x2) && (x2.kind == AL_NPRS)){
			
			ASTPars* old = x2.here;
		 	old->pos     = fusePosition(old->pos, x2.tk.pos);
		 	ASTParam par = *(ASTParam*)x0.here;
		 	appendASTPars(old, par.elem, par.label);
		 	ast.head    -= 3;
		 	
		 	ASTList np;
			np.here = malloc(sizeof(ASTPars));
			np.kind = AL_NPRS;
			np.here = old;
			astStackPush(&ast, &np);
			continue;
		}
		
		// NPRS = NPAR  EOF
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_NPAR) && (tks.head == 0)){
			ASTPars  new = makeASTPars(tks.size / 2);
		 	new.pos      = x0.pos;
		 	ASTParam   a = *(ASTParam*)x0.here;
		 	appendASTPars(&new, a.elem, a.label);
		 	ast.head    --;
		 	
		 	ASTList np;
			np.here = malloc(sizeof(ASTPars));
			np.kind = AL_NPRS;
			np.here = &new;
			astStackPush(&ast, &np);
			continue;
		}
		
		
		// Comment and Newline Removal
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_COMMENT  )){ast.head--; continue; }
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_COMMS    )){ast.head--; continue; }
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_NEWLINE  )){ast.head--; continue; }
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_SEMICOLON)){ast.head--; continue; }
		
		
		void* xval;
		if(!parseStep(&tks, &ast, 0, AL_NPRS, &xval)){
			*ret = *(ASTPars*)xval;
			cont = 0;
		}
	}
	
	if(ret->prct < 1){
		if(ret->lbls != NULL){ free(ret->lbls); ret->lbls = NULL; }
		if(ret->pars != NULL){ free(ret->pars); ret->pars = NULL; }
	}
	
	free(ln.lst);
	free(ast.stk);
	free(tks.stk);
	return 1;
}

int parseType (ASTListKind k, ASTList* typ, ErrorList* errs, ASTType* ret){
	ASTLine  ln  = toLine(typ);
	ASTStack tks = lineToStack(&ln);
	ASTStack ast = makeEmptyStack(ln.size);
	
	/*
		try parseStruct
		try parseUnion
		try parseEnum
		try parseTagUnion
		try parseTyElem
	*/
	int pass = 0;
	if(k == AL_BRK){
		if      (structParser(&ast, &tks, errs, &ret->strc)){
			ret->type = TT_STRC;
			pass      = 1;
		}else if(tyElemParser(&ast, &tks, errs)){
			ret->elem = *(ASTTyElem*)ast.stk[ast.head-1].here;
			ret->type = TT_ELEM;
			pass      = 1;
		}
	}else if(k == AL_PAR){
		// TODO: try tagUnionParser
		if      (unionParser (&ast, &tks, errs, &ret->unon)){
			ret->type = TT_UNON;
			pass      = 1;
		}else if(enumParser  (&ast, &tks, errs, &ret->enmt)){
			ret->type = TT_ENUM;
			pass      = 1;
		}
	}

	free(ln.lst);
	free(ast.stk);
	free(tks.stk);
	return pass;
}

int headerParser(ASTStack* stk, ASTStack* tks, ErrorList* errs, ASTProgram* ret){
	
	int cont = 1;
	while(cont){
		printf("HD %i %i | ", tks->head, stk->head);
		printASTStack(*stk);
	
		ASTList x0, x1, x2, x3, x4, x5, x6, x7, x8;
		
		// id :: [npars] => [npars] -> [tpars] {block} \n
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_NEWLINE ) &&
		   astStackPeek(stk, 1, &x1) && (x1.kind == AL_BRC)                                 &&
		   astStackPeek(stk, 2, &x2) && (x2.kind == AL_BRK)                                 &&
		   astStackPeek(stk, 3, &x3) && (x3.kind == AL_TKN) && (x3.tk.type == TKN_R_ARROW ) &&
		   astStackPeek(stk, 4, &x4) && (x4.kind == AL_BRK)                                 &&
		   astStackPeek(stk, 5, &x5) && (x5.kind == AL_TKN) && (x5.tk.type == TKN_R_DARROW) &&
		   astStackPeek(stk, 6, &x6) && (x6.kind == AL_BRK)                                 &&
		   astStackPeek(stk, 7, &x7) && (x7.kind == AL_TKN) && (x7.tk.type == TKN_DEFINE  ) &&
		   astStackPeek(stk, 8, &x8) && (x8.kind == AL_TKN) && (x8.tk.type == TKN_S_ID    )){
		 	// If:
		 	//   x1 is a valid block
		 	//   x2 is a valid tpars
		 	//   x4 is a valid npars
		 	//   x6 is a valid npars
		 	// then build func
		 	ASTBlock blk;
		 	ASTPars  tps, fps, rts;
		 	if(parseBlock(x1.here, errs, &blk) &&
		 	   parseTPars(x2.here, errs, &rts) &&
		 	   parseNPars(x4.here, errs, &fps, 1) &&
		 	   parseNPars(x6.here, errs, &tps, 0)){
		 		ASTFnDef fndef;
		 		fndef.pos  = x8.pos;
		 		fndef.fnid = x8.tk.data.i64;	 		
		 		fndef.tvrs = tps;
		 		fndef.pars = fps;
		 		fndef.rets = rts;
		 		fndef.def  = blk;
		 		
		 		
		 		printf("B\n");
		 		stk->head -= 9;
		 	
		 		ASTList fn;
				fn.pos  = fndef.pos;
				fn.here = malloc(sizeof(ASTFnDef));
				fn.kind = AL_FNDF;
				*(ASTFnDef*)fn.here = fndef;
				astStackPush(stk, &fn);
		 		continue;
		 	}
		 	
		 	// If not, report error  
		}
		
		// id :: [npars] -> [tpars] {block} \n
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_NEWLINE ) &&
		   astStackPeek(stk, 1, &x1) && (x1.kind == AL_BRC)                                 &&
		   astStackPeek(stk, 2, &x2) && (x2.kind == AL_BRK)                                 &&
		   astStackPeek(stk, 3, &x3) && (x3.kind == AL_TKN) && (x3.tk.type == TKN_R_ARROW ) &&
		   astStackPeek(stk, 4, &x4) && (x4.kind == AL_BRK)                                 &&
		   astStackPeek(stk, 5, &x5) && (x5.kind == AL_TKN) && (x5.tk.type == TKN_DEFINE  ) &&
		   astStackPeek(stk, 6, &x6) && (x6.kind == AL_TKN) && (x6.tk.type == TKN_S_ID    )){
		 	// If:
		 	//   x1 is a valid block
		 	//   x2 is a valid tpars
		 	//   x4 is a valid npars
		 	// then build func
		 	ASTBlock blk;
		 	ASTPars  fps, rts;
		 	if(parseBlock(x1.here, errs, &blk) &&
		 	   parseTPars(x2.here, errs, &rts) &&
		 	   parseNPars(x4.here, errs, &fps, 1)){
		 		ASTFnDef fndef;
		 		fndef.pos  = x6.pos;
		 		fndef.fnid = x6.tk.data.i64;
		 		fndef.tvrs = (ASTPars){fndef.pos, 0, 0};
		 		fndef.pars = fps;
		 		fndef.rets = rts;
		 		fndef.def  = blk;
		 		
		 		printf("C\n");
		 		stk->head -= 7;
		 	
		 		ASTList fn;
				fn.pos  = fndef.pos;
				fn.here = malloc(sizeof(ASTFnDef));
				fn.kind = AL_FNDF;
				*(ASTFnDef*)fn.here = fndef;
				astStackPush(stk, &fn);
		 		continue;
		 	}
		 	// If not, report error  
		}
		
		
		// TI :: [npars] => [] \n
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_NEWLINE ) &&
		   astStackPeek(stk, 1, &x1) && (x1.kind == AL_BRK)                                 &&
		   astStackPeek(stk, 2, &x2) && (x2.kind == AL_TKN) && (x1.tk.type == TKN_R_DARROW) &&
		   astStackPeek(stk, 3, &x3) && (x3.kind == AL_BRK)                                 &&
		   astStackPeek(stk, 4, &x4) && (x4.kind == AL_TKN) && (x2.tk.type == TKN_DEFINE  ) &&
		   astStackPeek(stk, 5, &x5) && (x5.kind == AL_TKN) && (x3.tk.type == TKN_S_TYID  )){
			// If:
			//   x1 is a valid type
			//   x3 is a valid npars
			// then build type
			ASTType  typ;
		 	ASTPars  tps;
		 	if(parseType (AL_BRK, x1.here, errs, &typ) &&
		 	   parseNPars(x3.here, errs, &tps, 0)){
				ASTTyDef tydef;
				tydef.pos  = x5.pos;
		 		tydef.tyid = x5.tk.data.i64;
				printf("D\n");
				stk->head -= 6;
			
				ASTList ty;
				ty.pos  = tydef.pos;
				ty.here = malloc(sizeof(ASTTyDef));
				ty.kind = AL_TYDF;
				*(ASTTyDef*)ty.here = tydef;
				astStackPush(stk, &ty);
				continue;
			}
			
			// If not, report an error
		}
		
		// TI :: [npars] => () \n
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_NEWLINE ) &&
		   astStackPeek(stk, 1, &x1) && (x1.kind == AL_PAR)                                 &&
		   astStackPeek(stk, 2, &x2) && (x2.kind == AL_TKN) && (x2.tk.type == TKN_R_DARROW) &&
		   astStackPeek(stk, 3, &x3) && (x3.kind == AL_BRK)                                 &&
		   astStackPeek(stk, 4, &x4) && (x4.kind == AL_TKN) && (x4.tk.type == TKN_DEFINE  ) &&
		   astStackPeek(stk, 5, &x5) && (x5.kind == AL_TKN) && (x5.tk.type == TKN_S_TYID  )){
			// If:
			//   x1 is a valid type
			//   x3 is a valid npars
			// then build type
			ASTType  typ;
		 	ASTPars  tps;
		 	if(parseType (AL_PAR, x1.here, errs, &typ) &&
		 	   parseNPars(x3.here, errs, &tps, 0)){
				ASTTyDef tydef;
				tydef.pos  = x5.pos;
		 		tydef.tyid = x5.tk.data.i64;
				printf("E\n");
				stk->head -= 6;
			
				ASTList ty;
				ty.pos  = tydef.pos;
				ty.here = malloc(sizeof(ASTTyDef));
				ty.kind = AL_TYDF;
				*(ASTTyDef*)ty.here = tydef;
				astStackPush(stk, &ty);
				continue;
			}
			// If not, report an error
		}
		
		
		// TI :: [] \n
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_NEWLINE ) &&
		   astStackPeek(stk, 1, &x1) && (x1.kind == AL_BRK)                                 &&
		   astStackPeek(stk, 2, &x2) && (x2.kind == AL_TKN) && (x2.tk.type == TKN_DEFINE  ) &&
		   astStackPeek(stk, 3, &x3) && (x3.kind == AL_TKN) && (x3.tk.type == TKN_S_TYID  )){
			// If:
			//   x1 is a valid type
			// then build type
			ASTType  typ;
		 	if(parseType (AL_BRK, x1.here, errs, &typ)){
				ASTTyDef tydef;
				tydef.pos  = x3.pos;
			 	tydef.tyid = x3.tk.data.i64;
			 	printf("F\n");
				stk->head -= 4;
			
				ASTList ty;
				ty.pos  = tydef.pos;
				ty.here = malloc(sizeof(ASTTyDef));
				ty.kind = AL_TYDF;
				*(ASTTyDef*)ty.here = tydef;
				astStackPush(stk, &ty);
				continue;
			}
			// If not, report an error
		}
		
		// TI :: () \n
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_NEWLINE ) &&
		   astStackPeek(stk, 1, &x1) && (x1.kind == AL_PAR)                                 &&
		   astStackPeek(stk, 2, &x2) && (x2.kind == AL_TKN) && (x2.tk.type == TKN_DEFINE  ) &&
		   astStackPeek(stk, 3, &x3) && (x3.kind == AL_TKN) && (x3.tk.type == TKN_S_TYID  )){
			// If:
			//   x1 is a valid type
			// then build type
			ASTType  typ;
		 	if(parseType (AL_PAR, x1.here, errs, &typ)){
				ASTTyDef tydef;
				tydef.pos  = x3.pos;
			 	tydef.tyid = x3.tk.data.i64;
			 	printf("G\n");
				stk->head -= 4;
			
				ASTList ty;
				ty.pos  = tydef.pos;
				ty.here = malloc(sizeof(ASTTyDef));
				ty.kind = AL_TYDF;
				*(ASTTyDef*)ty.here = tydef;
				astStackPush(stk, &ty);
				continue;
			}	
			// If not, report an error
		}
		
		
		// [ id : string ] \n
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_NEWLINE ) &&
		   astStackPeek(stk, 1, &x1) && (x1.kind == AL_BRK)                               ){
		 	// If:
		 	//   x1 inside has form bid : string
		 	// then build header
		 	
		 	printf("A\n");
		 	ASTList* here = x1.here;
		 	if(here != NULL){
		 		ASTLine ln = toLine(here);
		 		if((ln.size == 3) &&
		 		   (ln.lst[0].kind == AL_TKN) && (ln.lst[0].tk.type == TKN_S_BID) &&
		 		   (ln.lst[1].kind == AL_TKN) && (ln.lst[1].tk.type == TKN_COLON) &&
		 		   (ln.lst[2].kind == AL_TKN) && (ln.lst[2].tk.type == TKN_STR  )){
		 			ASTHeader head;
		 			head.pos   = x1.pos;
		 			head.bid   = ln.lst[0].tk.data.i64;
		 			head.str   = ln.lst[2].tk.data.str;
		 			stk->head -= 2;
		 	
		 			ASTList hd;
					hd.pos  = head.pos;
					hd.here = malloc(sizeof(ASTHeader));
					hd.kind = AL_HEAD;
					*(ASTHeader*)hd.here = head;
					astStackPush(stk, &hd);
		 			continue;
		 		}
		 		return 0;
		 	}else{
		 		return 0;
		 	}
		 	
		 	// If not, report error  
		}
		
		
		// HEADER combination
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_HEAD) && (stk->head == 1)          ) {
		   
		   // Merge
		   printf("H\n");
		   
		   x0.kind = AL_PROG;
		   ASTHeader hd = *(ASTHeader*)x0.here;
		   free(x0.here);
		   x0.here = malloc(sizeof(ASTProgram));
		   ASTProgram* prog = x0.here;
		   *prog = makeASTProgram(stk->size);
		   appendHeader(prog, hd);
		   stk->stk[stk->head-1] = x0;
		   continue;
		}
		
		
		// HEADER combination
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_HEAD)                                &&
		   astStackPeek(stk, 1, &x1) && (x1.kind == AL_PROG)                              ){
		   
		   // Merge
		   printf("I\n");
		   stk->head -= 1;
		   ASTHeader hd     = *(ASTHeader*)x0.here;
		   free(x0.here);
		   ASTProgram* prog = x1.here;
		   appendHeader(prog, hd);
		   stk->stk[stk->head-1] = x1;
		   continue;
		}
		
		
		// DEF combination
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_FNDF)                                &&
		   astStackPeek(stk, 1, &x1) && (x1.kind == AL_PROG)                              ){
		   
		   // Merge
		   printf("J\n");
		   stk->head -= 1;
		   ASTFnDef fn      = *(ASTFnDef*)x0.here;
		   free(x0.here);
		   ASTProgram* prog = x1.here;
		   appendFnDef(prog, fn);
		   stk->stk[stk->head-1] = x1;
		   continue;
		}
		
		
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TYDF)                                &&
		   astStackPeek(stk, 1, &x1) && (x1.kind == AL_PROG)                              ){
		   
		   // Merge
		   printf("K\n");
		   stk->head -= 1;
		   ASTTyDef ty      = *(ASTTyDef*)x0.here;
		   free(x0.here);
		   ASTProgram* prog = x1.here;
		   appendTyDef(prog, ty);
		   stk->stk[stk->head-1] = x1;
		   continue;
		}
		
		
		// Comment Removal
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_COMMENT )){stk->head--; printf("X\n"); continue; }
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_COMMS   )){stk->head--; printf("Y\n"); continue; }
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_NEWLINE )){stk->head--; printf("Z\n"); continue; }
		
		
		
		
		// Eventually we need rules for typesets, etc. as well
		
		
		// No rules applied. Let's grab another token
		void* xval;
		if(!parseStep(tks, stk, 1, AL_PROG, &xval)){
			*ret = *(ASTProgram*)xval;
			cont = 0;
		}
	}
	return 1;
}






int parseCode(LexerState* tks, SymbolTable* tab, ASTProgram* prog, ErrorList* errs){
	//printf("\n=================\n");
	ASTList* lst;
	if(!unwrap(tks, errs, &lst)) return -1;
	//printASTList(lst, 0);
	//printf("\n=================\n");
	
	printf("Len=%i\n", astLen(lst));
	
	ASTLine  ln  = toLine(lst);
	ASTStack ast = lineToStack(&ln);
	ASTStack stk = makeEmptyStack(ln.size);
	if(headerParser(&stk, &ast, errs, prog)){
		printf("Successful parsing\n");
		return 0;
	}

	return -1;
}




