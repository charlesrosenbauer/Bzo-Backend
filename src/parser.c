#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "common.h"
#include "error.h"
#include "util.h"
#include "ast.h"


//#define PARSER_DEBUG


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
	AL_AGEN,
	AL_ASIZ,
	AL_XNIL,
	AL_NOXP,
	AL_TKN,
	
	// Typedef AST
	AL_TYDF,
	AL_TYHD,
	AL_TATM,
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
	AL_ASGN,
	AL_CNST,
	AL_STMT,
	AL_STMS,
	
	// List parsing
	AL_TPRS,
	AL_TPAR,
	AL_NPRS,
	AL_NPAR,
	AL_STLN,
	AL_STLS,
	AL_UNLN,
	AL_UNLS,
	AL_ENLN,
	AL_ENLS,
	AL_CALL,
	AL_TAG,
	AL_SEPR,
	
	// Misc
	AL_LOC,
	
	// Program File control
	AL_HEAD,
	AL_DEFS,
	AL_PROG
}ASTListKind;


typedef struct{Token   		tk;											 } AS_TK;
typedef struct{Token   		tk;		void* here;							 } AS_WRAP;
typedef struct{ASTProgram 	prog;										 } AS_PROG;
typedef struct{ASTHeader	hd;											 } AS_HD;
typedef struct{ASTFnDef		fn;											 } AS_FN;
typedef struct{ASTTyDef		ty;											 } AS_TY;
typedef struct{ASTTyElem	lm;											 } AS_TYLM;
typedef struct{ASTType      ty;											 } AS_TYPE;
typedef struct{int64_t	    tyid;	ASTPars tprs;						 } AS_TYHD; 
typedef struct{ASTTyAtom	tatm;										 } AS_TATM;
typedef struct{int64_t		fld;	ASTType type;   					 } AS_SF;
typedef struct{int64_t		fld;	ASTType type;		int64_t		tag; } AS_UF;
typedef struct{int64_t		fld;						int64_t		tag; } AS_EF;
typedef struct{ASTStruct    strc;										 } AS_STRC;
typedef struct{ASTUnion		unon;										 } AS_UNON;
typedef struct{ASTEnum		enmt;										 } AS_ENUM;
typedef struct{int64_t		tyid;	int64_t tgid;						 } AS_TAG;
typedef struct{ASTFnTy		fnty;										 } AS_FNTY;
typedef struct{ASTCnst		cnst;										 } AS_CNST;
// Add more as needed

typedef struct{
	Position   pos;	// We should try to use the position in the token/union, as that would cut struct size by 25%
	union{
		AS_TK		tk;
		AS_WRAP		wrap;
		AS_PROG		prog;
		AS_HD		hd;
		AS_FN		fn;
		AS_TY		ty;
		AS_TYLM		tylm;
		AS_TYPE		type;
		AS_TYHD		tyhd;
		AS_TATM		tatm;
		AS_SF		sf;
		AS_UF		uf;
		AS_EF		ef;
		AS_TAG		tag;
		AS_FNTY		ft;
		AS_STRC		strc;
		AS_UNON		unon;
		AS_ENUM		enmt;
		AS_CNST		cnst;
	};
	void*		next;
	ASTListKind kind;
}ASTList;

void freeASTList(ASTList* l){
	if(l != NULL){
		switch(l->kind){
			case AL_PAR:
			case AL_BRK:
			case AL_BRC: {free(l->wrap.here);} break;
			default: break;		// Not all cases need to be freed
		}
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
			if(l->wrap.here == NULL){
				printf("() ");
			}else{
				printf("( ");
				printASTList(l->wrap.here, pad+1);
				//leftpad(pad);
				printf(") ");
				printASTList(l->next, 0);
			}
		}break;
		
		case AL_BRK : {
			if(l->wrap.here == NULL){
				printf("[] ");
			}else{
				printf("[ ");
				printASTList(l->wrap.here, pad+1);
				//leftpad(pad);
				printf("] ");
				printASTList(l->next, 0);
			}
		}break;
		
		case AL_BRC : {
			if(l->wrap.here == NULL){
				printf("{} ");
			}else{
				printf("{ ");
				printASTList(l->wrap.here, pad+1);
				//leftpad(pad);
				printf("} ");
				printASTList(l->next, 0);
			}
		}break;
		
		case AL_AGEN : printf("[0] "); break;
		case AL_ASIZ : printf("[N] "); break;
		case AL_XNIL : printf("(0) "); break;
		case AL_NOXP : printf("{0} "); break;
		
		case AL_TKN : {
			char buffer[1024];
			printf("<%s> ", printToken(l->tk.tk, buffer));
			printASTList(l->next, 0);
		}break;
		
		
		// Typedef AST
		case AL_TYDF : printf("TYDF "); break;
		case AL_TYHD : printf("TYHD "); break;
		case AL_TATM : printf("TATM "); break;
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
		case AL_ASGN : printf("ASGN "); break;
		case AL_CNST : printf("CNST "); break;
		case AL_STMT : printf("STMT "); break;
		case AL_STMS : printf("STMS "); break;
		
		// Pars
		case AL_TPRS : printf("TPRS "); break;
		case AL_TPAR : printf("TPAR "); break;
		case AL_NPRS : printf("NPRS "); break;
		case AL_NPAR : printf("NPAR "); break;
		case AL_STLN : printf("STLN "); break;
		case AL_STLS : printf("STLS "); break;
		case AL_UNLN : printf("UNLN "); break;
		case AL_UNLS : printf("UNLS "); break;
		case AL_ENLN : printf("ENLN "); break;
		case AL_ENLS : printf("ENLS "); break;
		case AL_CALL : printf("CALL "); break;
		case AL_TAG  : printf("TAG  "); break;
		case AL_SEPR : printf("SEPR "); break;
		
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
				lst[i].kind      = AL_PAR;
				lst[i].wrap.here = &lst[i+1];
				lst[i].pos       = t.pos;
				xs[ix+1]         = t; xs[ix+1].data.i64 = i; ix++; } break;
			case TKN_BRK_OPN : {
				if(i > 0) lst[i-1].next = &lst[i];
				lst[i].kind      = AL_BRK;
				lst[i].wrap.here = &lst[i+1];
				lst[i].pos       = t.pos;
				xs[ix+1]         = t; xs[ix+1].data.i64 = i; ix++; } break;
			case TKN_BRC_OPN : {
				if(i > 0) lst[i-1].next = &lst[i];
				lst[i].kind      = AL_BRC;
				lst[i].wrap.here = &lst[i+1];
				lst[i].pos       = t.pos;
				xs[ix+1]         = t; xs[ix+1].data.i64 = i; ix++; } break;
			case TKN_PAR_END : {
				if(xs[ix].type == TKN_PAR_OPN){
					lst[i-1].next             = NULL;
					lst[xs[ix].data.i64].next = &lst[i+1];
					lst[i].kind               = AL_NIL;
					ix--;
				}else{
					// Error!
					Position pos = fusePosition(xs[ix].pos, t.pos);
					appendList(&errs->errs, &(Error){ERR_P_BAD_PAR, pos});
					ret = 0;
				}
			}break;
			case TKN_BRK_END : {
				if(xs[ix].type == TKN_BRK_OPN){
					lst[i-1].next             = NULL;
					lst[xs[ix].data.i64].next = &lst[i+1];
					lst[i].kind               = AL_NIL;
					ix--;
				}else{
					// Error!
					Position pos = fusePosition(xs[ix].pos, t.pos);
					appendList(&errs->errs, &(Error){ERR_P_BAD_BRK, pos});
					ret = 0;
				}
			}break;
			case TKN_BRC_END : {
				if(xs[ix].type == TKN_BRC_OPN){
					lst[i-1].next             = NULL;
					lst[xs[ix].data.i64].next = &lst[i+1];
					lst[i].kind               = AL_NIL;
					ix--;
				}else{
					// Error!
					Position pos = fusePosition(xs[ix].pos, t.pos);
					appendList(&errs->errs, &(Error){ERR_P_BAD_BRC, pos});
					ret = 0;
				}
			}break;
			default:{
				if(i > 0) lst[i-1].next = &lst[i];
				lst[i].next  = NULL;
				lst[i].kind  = AL_TKN;
				lst[i].pos   = t.pos;
				lst[i].tk.tk = t;
			}break;
		}
	}
	for(int i = 0; i < tks->tkct; i++){
		if(lst[i].kind == AL_BRK){
			ASTList* here = lst[i].wrap.here;
			if((here == NULL) || (here->kind == AL_NIL)){
				lst[i].kind = AL_AGEN;
			}else if((here->kind == AL_TKN) && (here->tk.tk.type == TKN_INT) && (here->next == NULL)){
				lst[i].kind = AL_ASIZ;
				lst[i].tk   = here->tk;
			}
		}
		if(lst[i].kind == AL_PAR){
			ASTList* here = lst[i].wrap.here;
			if((here == NULL) || (here->kind == AL_NIL)){
				lst[i].kind = AL_XNIL;
			}
		}
		if(lst[i].kind == AL_BRC){
			ASTList* here = lst[i].wrap.here;
			if((here == NULL) || (here->kind == AL_NIL)){
				lst[i].kind = AL_NOXP;
			}
		}
	}
	if(ix != 0){
		ret = 0;
		for(int i = ix; i > 0; i--){
			if       (xs[ix].type == TKN_PAR_OPN){
				appendList(&errs->errs, &(Error){ERR_P_DNGL_PAR, xs[ix].pos});
			}else if (xs[ix].type == TKN_BRK_OPN){
				appendList(&errs->errs, &(Error){ERR_P_DNGL_BRK, xs[ix].pos});
			}else if (xs[ix].type == TKN_BRC_OPN){
				appendList(&errs->errs, &(Error){ERR_P_DNGL_BRC, xs[ix].pos});
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
			case AL_AGEN : printf("[0] "); break;
			case AL_ASIZ : printf("[N] "); break;
			case AL_XNIL : printf("(0) "); break;
			case AL_NOXP : printf("{0} "); break;
			case AL_TKN  : {
				switch(ln.lst[i].tk.tk.type){
					case TKN_S_ID      : printf("ID  " ); break;
					case TKN_S_BID     : printf("BI  " ); break;
					case TKN_S_TYID    : printf("TI  " ); break;
					case TKN_S_TVAR    : printf("TV  " ); break;
					case TKN_S_MID     : printf("MI  " ); break;
					case TKN_INT       : printf("INT " ); break;
					case TKN_FLT       : printf("FLT " ); break;
					case TKN_STR       : printf("STR " ); break;
					case TKN_TAG       : printf("TAG " ); break;
					case TKN_FNTY      : printf("|>  " ); break;
					case TKN_DEFINE    : printf("::  " ); break;
					case TKN_COLON     : printf(":   " ); break;
					case TKN_SEMICOLON : printf(";   " ); break;
					case TKN_PERIOD    : printf(".   " ); break;
					case TKN_COMMA     : printf(",   " ); break;
					case TKN_CONSTRAIN : printf("|:  " ); break;
					case TKN_PIPE      : printf("\\   "); break;
					case TKN_WILD      : printf("_   " ); break;
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
			case AL_TYHD : printf("TH  "); break;
			case AL_TATM : printf("TA  "); break;
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
			case AL_ASGN : printf("ASN "); break;
			case AL_CNST : printf("CSN "); break;
			case AL_STMT : printf("SM  "); break;
			case AL_STMS : printf("SMS "); break;
			
			case AL_TPRS : printf("TPS "); break;
			case AL_TPAR : printf("TP  "); break;
			case AL_NPRS : printf("NPS "); break;
			case AL_NPAR : printf("NP  "); break;
			case AL_STLN : printf("S_  "); break;
			case AL_STLS : printf("S_S "); break;
			case AL_UNLN : printf("U_  "); break;
			case AL_UNLS : printf("U_S "); break;
			case AL_ENLN : printf("E_  "); break;
			case AL_ENLS : printf("E_S "); break;
			case AL_CALL : printf("CL  "); break;
			case AL_TAG  : printf("TG  "); break;
			case AL_SEPR : printf("SP  "); break;
			
			case AL_LOC  : printf("LC  "); break;
			
			case AL_DEFS : printf("DF  "); break;
			case AL_HEAD : printf("HD  "); break;
			case AL_PROG : printf("PG  "); break;
			
			case AL_NIL  : printf("??  "); break;
			
			default      : printf("_?_ "); break;
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
		if((ln->lst[i].kind != AL_TKN) || ((ln->lst[i].tk.tk.type != TKN_COMMENT) && (ln->lst[i].tk.tk.type != TKN_COMMS))){
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
		if((a->lst[i].kind == AL_TKN) && (a->lst[i].tk.tk.type == t)){
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
		int here = (x->lst[i].kind == AL_TKN) && ((x->lst[i].tk.tk.type == TKN_NEWLINE) || (x->lst[i].tk.tk.type == TKN_SEMICOLON));
		if(!(((ix == 0) && here) || ((i > 0) && here && last))){
			x->lst[ix] = x->lst[i];
			if((x->lst[i].kind == AL_TKN) && (x->lst[i].tk.tk.type == TKN_SEMICOLON)) x->lst[i].tk.tk.type = TKN_NEWLINE;
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
		if((a->lst[i].kind == AL_TKN) && (a->lst[i].tk.tk.type == t)){
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
		if((i < ct) && (ts[i] != TKN_VOID) && ((ln->lst[i].kind != AL_TKN) || (ln->lst[i].tk.tk.type != ts[i]))) return 0;
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
		if((ln->lst[i].kind != AL_TKN) || (ln->lst[i].tk.tk.type != t)){
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
		if((ln->lst[i].kind != AL_TKN) || (ln->lst[i].tk.tk.type != t)){
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
	//printf("Stack underflow!\n");
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


void makeStacks(ASTList* lst, ASTStack* stk, ASTStack* tks){
	ASTLine ln = toLine(lst);
	if(tks->stk != NULL) free(tks->stk);
	if(stk->stk != NULL) free(stk->stk);
	*tks       = lineToStack(&ln);
	*stk       = makeEmptyStack(ln.size);
	free(ln.lst);
}




int isBinop(TkType t){
	switch(t){
		case TKN_ADD   : return 1;
		case TKN_SUB   : return 1;
		case TKN_MUL   : return 1;
		case TKN_DIV   : return 1;
		case TKN_MOD   : return 1;
		case TKN_EXP   : return 1;
		case TKN_AND   : return 1;
		case TKN_OR    : return 1;
		case TKN_XOR   : return 1;
		case TKN_SHL   : return 1;
		case TKN_SHR   : return 1;
		case TKN_GT    : return 1;
		case TKN_GTE   : return 1;
		case TKN_LS    : return 1;
		case TKN_LSE   : return 1;
		case TKN_EQL   : return 1;
		case TKN_NEQ   : return 1;
		case TKN_PIPE  : return 1;
		case TKN_PERIOD: return 1;
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



int parseStep(ASTStack* tks, ASTStack* stk, int printErrs, ASTListKind k, ASTList* ret){
	ASTList tk;
	if((tks->head == 0) && (stk->head == 1) && (stk->stk[0].kind == k)){
		*ret = stk->stk[0];
		return  0;
	}else if((tks->head == 0) && (stk->head == 1)){
		if(printErrs) printf("Invalid parsing result.\n");
		return -1;
	}else if(astStackPop(tks, &tk)){
		if(!astStackPush(stk, &tk)){ printf("AST Stack overflow.\n"); exit(-1); }
		return  1;
	}else if(stk->head > 1){
		if(printErrs) printf("Parser could not consume file.\n");
		return -1;
	}else{
		printf("WTF?\n");
		return -1;
	}
	return  0;
}

/*
	Actual Parser Rules
*/
/*
int exprParser(ASTStack*, ASTStack*, ErrorList*);


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


int parseExprCall(ASTList* line, ErrorList* errs, ASTCall* ret){
	ASTStack tks, ast;
	tks.stk = NULL; ast.stk = NULL;
	makeStacks(line, &ast, &tks);
	
	int pass = 1;
	int cont = 1;
	while(cont){
		#ifdef PARSER_DEBUG
			printf("XC %i %i | ", tks.head, ast.head);
			printASTStack(ast);
		#endif
		
		ASTList x0, x1, x2, x3, x4;
	
		// Comment Removal
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_COMMENT  )){ast.head--; continue; }
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_COMMS    )){ast.head--; continue; }
		
		// CL =		ID :	TYID :		LOCID :		LOCTY :			MID :
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN) &&  (x0.tk.type == TKN_COLON) &&
		   astStackPeek(&ast, 1, &x1) && (x1.kind == AL_TKN) && ((x1.tk.type == TKN_S_ID) || (x1.tk.type == TKN_S_MID) || (x1.tk.type == TKN_S_TYID) || (x1.tk.type == TKN_LOCTY))){
			ASTCall* call = malloc(sizeof(ASTCall));
			*call         = makeASTCall(4);
			call->pos     = fusePosition(x1.pos, x0.pos);
			call->name    = x1.tk;
			x2.pos        = call->pos;
			x2.kind       = AL_CALL;
			x2.here       = call;
			ast.head     -= 2;
			astStackPush(&ast, &x2);
			continue;
		}
		
		
		// CL =		CL EXPR ,
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN)  && (x0.tk.type == TKN_COMMA) &&
		   astStackPeek(&ast, 1, &x1) && (x1.kind == AL_EXPR) &&
		   astStackPeek(&ast, 2, &x2) && (x2.kind == AL_CALL)){
			ASTCall* call = x2.here;
			call->pos     = fusePosition(call->pos, x0.pos);
			ASTExpr* expr = x1.here;
			appendASTCall(call, *expr);
			free(expr);
			x3.pos        = call->pos;
			x3.kind       = AL_CALL;
			x3.here       = call;
			ast.head     -= 3;
			astStackPush(&ast, &x3);
			continue;
		}
		
		// CL =		CL _    ,
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.type == TKN_COMMA) &&
		   astStackPeek(&ast, 1, &x1) && (x1.kind == AL_TKN ) && (x1.tk.type == TKN_WILD ) &&
		   astStackPeek(&ast, 2, &x2) && (x2.kind == AL_CALL)){
			ASTCall* call = x2.here;
			call->pos     = fusePosition(x2.pos, x0.pos);
			ASTExpr  expr = (ASTExpr){.pos=x1.tk.pos, .a=NULL, .b=NULL, .tk=x1.tk, .type=XT_WILD};
			appendASTCall(call, expr);
			x3.pos        = call->pos;
			x3.kind       = AL_CALL;
			x3.here       = call;
			ast.head     -= 3;
			astStackPush(&ast, &x3);
			continue;
		}
		
		// CL =		CL EXPR EOF
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_EXPR) &&
		   astStackPeek(&ast, 1, &x1) && (x1.kind == AL_CALL) &&
		   (tks.head == 0)){
		    ASTCall* call = x1.here;
			call->pos     = fusePosition(call->pos, x0.pos);
			ASTExpr* expr = x0.here;
			appendASTCall(call, *expr);
			free(expr);
			x2.pos        = call->pos;
			x2.kind       = AL_CALL;
			x2.here       = call;
			ast.head     -= 2;
			astStackPush(&ast, &x2);
			continue;
		}
		
		
		// CL =		CL _	EOF
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.type == TKN_WILD) &&
		   astStackPeek(&ast, 1, &x1) && (x1.kind == AL_CALL) &&
		   (tks.head == 0)){
			ASTCall* call = x1.here;
			call->pos     = fusePosition(call->pos, x0.pos);
			ASTExpr  expr = (ASTExpr){.pos=x0.tk.pos, .a=NULL, .b=NULL, .tk=x0.tk, .type=XT_WILD};
			appendASTCall(call, expr);
			x2.pos        = call->pos;
			x2.kind       = AL_CALL;
			x2.here       = call;
			ast.head     -= 2;
			astStackPush(&ast, &x2);
			continue;
		}
		
		
		// EXPR
		if(exprParser(&ast, &tks, errs)) continue;
		
		// SOF NL		|	 NL EOF
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN) &&
		 ((x0.tk.type == TKN_NEWLINE) || (x0.tk.type == TKN_SEMICOLON)) && ((ast.head == 1) || (tks.head == 0))){
			ast.head--;
			continue;
		}
		
		void* xval;
		int step = parseStep(&tks, &ast, 0, AL_CALL, &xval);
		if(!step){
			*ret = *(ASTCall*)xval;
			cont = 0;
		}else if(step < 0){
			pass = 0;
			cont = 0;
		}
	}
	
	free(ast.stk);
	free(tks.stk);
	return pass;
}


int parseExprLine(ASTList* line, ErrorList* errs, ASTExpr* ret){
	ASTStack tks, ast;
	tks.stk = NULL; ast.stk = NULL;
	makeStacks(line, &ast, &tks);
	
	int pass = 1;
	int cont = 1;
	while(cont){
		#ifdef PARSER_DEBUG
			printf("XL %i %i | ", tks.head, ast.head);
			printASTStack(ast);
		#endif
	
		ASTList x0;
	
		// Comment Removal
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_COMMENT  )){ast.head--; continue; }
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_COMMS    )){ast.head--; continue; }
		
		// EXPR
		if(exprParser(&ast, &tks, errs)) continue;
		
		// SOF NL		|	 NL EOF
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN) &&
		 ((x0.tk.type == TKN_NEWLINE) || (x0.tk.type == TKN_SEMICOLON)) && ((ast.head == 1) || (tks.head == 0))){
			ast.head--;
			continue;
		}
		
		void* xval;
		int step = parseStep(&tks, &ast, 0, AL_EXPR, &xval);
		if(!step){
			*ret = *(ASTExpr*)xval;
			cont = 0;
		}else if(step < 0){
			pass = 0;
			cont = 0;
		}
	}
	
	free(ast.stk);
	free(tks.stk);
	return pass;
}


int parseBlock  (ASTList*, ErrorList*, ASTBlock*);
int tyElemParser(ASTStack*, ASTStack*, ErrorList*);


int constraintParser(ASTStack* ast, ASTStack* tks, ErrorList* errs){
	#ifdef PARSER_DEBUG
		printf("CS %i %i | ", tks->head, ast->head);
		printASTStack(*ast);
	#endif
	
	ASTList x0, x1, x2, x3, x4;

	// CNST =   ID  |:  TYLM  NL
	if(astStackPeek(ast, 0, &x0) && (x0.kind == AL_TKN ) && ((x0.tk.type == TKN_NEWLINE) || (x0.tk.type == TKN_SEMICOLON)) &&
	   astStackPeek(ast, 1, &x1) && (x1.kind == AL_TYLM) &&
	   astStackPeek(ast, 2, &x2) && (x2.kind == AL_TKN ) &&  (x2.tk.type == TKN_CONSTRAIN) &&
	   astStackPeek(ast, 3, &x3) && (x3.kind == AL_TKN ) &&
	  ((x3.tk.type == TKN_S_ID) || (x3.tk.type == TKN_S_MID) || (x3.tk.type == TKN_LOCID) || (x3.tk.type == TKN_TVAR))){
	  
	  	ASTCnst* cnst = malloc(sizeof(ASTCnst));
	  	cnst->pos     = fusePosition(x3.pos, x0.pos);
	  	cnst->name    = x3.tk;
	  	cnst->type    = *(ASTTyElem*)x1.here;
	    free(x1.here);
	    
	    x4.pos        = cnst->pos;
	    x4.here       = cnst;
	    x4.kind       = AL_CNST;
	    ast->head    -= 4;
	    astStackPush(ast, &x4);
	    return 1;
	}
		
		
	// CNST =	|:  EXPR  NL
	if(astStackPeek(ast, 0, &x0) && (x0.kind == AL_TKN ) && ((x0.tk.type == TKN_NEWLINE) || (x0.tk.type == TKN_SEMICOLON)) &&
	   astStackPeek(ast, 1, &x1) && (x1.kind == AL_EXPR) &&
	   astStackPeek(ast, 2, &x2) && (x2.kind == AL_TKN ) &&  (x2.tk.type == TKN_CONSTRAIN)){
		    
	   ASTCnst* cnst = malloc(sizeof(ASTCnst));
	  	cnst->pos     = fusePosition(x3.pos, x0.pos);
	  	cnst->name    = (Token){.type=TKN_VOID, .pos=cnst->pos};
	  	cnst->expr    = *(ASTExpr*)x1.here;
	    free(x1.here);
	    
	    x4.pos        = cnst->pos;
	    x4.here       = cnst;
	    x4.kind       = AL_CNST;
	    ast->head    -= 3;
	    astStackPush(ast, &x4);
	    return 1;
	    return 1;
	}
	
	return 0;
}


int exprParser(ASTStack* ast, ASTStack* tks, ErrorList* errs){
	#ifdef PARSER_DEBUG
		printf("XP %i %i | ", tks->head, ast->head);
		printASTStack(*ast);
	#endif
	
	ASTList x0, x1, x2, x3;
	
	// Int / Flt / Str / Tag			no : or |: afterward
	if(astStackPeek(ast, 0, &x0) && (x0.kind == AL_TKN)){
		if(!(astStackPeek(tks, 0, &x1) && (x1.kind == AL_TKN) && ((x1.tk.type == TKN_COLON) || (x1.tk.type == TKN_CONSTRAIN)))){
			ExprType ty = XT_VOID;
			switch(x0.tk.type){
				case TKN_INT   : ty = XT_INT;  break;
				case TKN_FLT   : ty = XT_FLT;  break;
				case TKN_STR   : ty = XT_STR;  break;
				case TKN_S_ID  : ty = XT_ID;   break;
				case TKN_S_MID : ty = XT_MID;  break;
				case TKN_LOCID : ty = XT_LOCI; break;
				default: break;
			}
		
			if(ty != XT_VOID){
				ASTExpr* expr = malloc(sizeof(ASTExpr));
				*expr = (ASTExpr){.pos = x0.tk.pos, .a=NULL, .b=NULL, .tk=x0.tk, .type=ty};
				x2.pos        = x0.tk.pos;
				x2.here       = expr;
				x2.kind       = AL_EXPR;
				ast->head    -= 1;
				astStackPush(ast, &x2);
				return 1;
			}
		}
	}
		
		
	// Expr [ Expr ]
	if(astStackPeek(ast, 0, &x0) && (x0.kind == AL_BRK ) &&
	   astStackPeek(ast, 1, &x1) && (x1.kind == AL_EXPR)){
	    ASTExpr xp;
		if(parseExprLine(x0.here, errs, &xp)){
			ASTExpr* expr = malloc(sizeof(ASTExpr));
			ASTExpr* xval = malloc(sizeof(ASTExpr));
			*expr = (ASTExpr){.pos = fusePosition(x1.pos, x0.pos), .a=NULL, .b=NULL, .tk=x1.tk, .type=XT_IX};
			*xval         = xp;
			expr->a       = (ASTExpr*)x1.here;
			expr->b       = xval;
			x2.pos        = expr->pos;
			x2.here       = expr;
			x2.kind       = AL_EXPR;
			ast->head    -= 2;
			astStackPush(ast, &x2);
			return 1;
		}
	}
		
		
	// ( Expr )
	if(astStackPeek(ast, 0, &x0) && (x0.kind == AL_PAR)){
		ASTExpr xp;
		if(parseExprLine(x0.here, errs, &xp)){
			ASTExpr* xval = malloc(sizeof(ASTExpr));
			ASTExpr* parn = malloc(sizeof(ASTExpr));
			*xval         = xp;
			parn->pos     = x0.pos;
			parn->a       = NULL;
			parn->b       = NULL;
			parn->xp      = xval;
			parn->type    = XT_PAR;
			x1.pos        = x0.pos;
			x1.here       = parn;
			x1.kind       = AL_EXPR;
			ast->head    -= 1;
			astStackPush(ast, &x1);
			return 1;
		}
	}
		
		
	// [ pars ] { block }		| [ pars ] ! { block }
	x2.kind = AL_NIL;
	if((astStackPeek(ast, 0, &x0) && (x0.kind == AL_BRC)   &&
	    astStackPeek(ast, 1, &x1) && (x1.kind == AL_BRK))  ||
	   
	   (astStackPeek(ast, 0, &x0) && (x0.kind == AL_BRC) &&
	    astStackPeek(ast, 1, &x2) && (x2.kind == AL_TKN) && (x2.tk.type == TKN_NOT) &&
	    astStackPeek(ast, 2, &x1) && (x1.kind == AL_BRK))){
	    ASTBlock blk;
	   	if(parseBlock(x0.here, errs, &blk)){
	   		// For now we're going to ignore the pars
	   		ASTLambda* lmda = malloc(sizeof(ASTLambda));
	   		lmda->pos       = fusePosition(x1.pos, x0.pos);
	   		//lmda->pars      = prs;
	   		lmda->block     = blk;
	   		lmda->isProc    = (x2.kind == AL_TKN);
	   		
	   		ASTExpr*   expr = malloc(sizeof(ASTExpr  ));
	   		expr->pos       = fusePosition(x1.pos, x0.pos);
	   		*expr           = (ASTExpr){.pos=lmda->pos, .a=NULL, .b=NULL, .xp=lmda, .type=XT_LMDA};
	   		
	   		x3.pos          = expr->pos;
	   		x3.here         = expr;
	   		x3.kind         = AL_EXPR;
	   		ast->head      -= (2 + (x2.kind != AL_NIL));
	   		astStackPush(ast, &x3);
	   		return 1;
	   	}
		return 0;
	}
		
		
	// Switch
		
		
	// Ife
		
		
	// [ Call : pars ]
	if(astStackPeek(ast, 0, &x0) && (x0.kind == AL_BRK)){
		ASTCall call;
		if(parseExprCall(x0.here, errs, &call)){
			ExprType type;
			switch(call.name.type){
				case TKN_S_ID   : type = XT_FNCL; break;
				case TKN_S_MID  : type = XT_FNCL; break;
				case TKN_LOCID  : type = XT_FNCL; break;
				case TKN_S_TYID : type = XT_MK;   break;
				case TKN_LOCTY  : type = XT_MK;   break;
				default: break;
			}
			ASTCall* clxp = malloc(sizeof(ASTCall));
			*clxp         = call;
			ASTExpr* expr = malloc(sizeof(ASTExpr));
			*expr = (ASTExpr){.pos = x0.pos, .a=NULL, .b=NULL, .xp=clxp, .type=type};
			x1.pos        = x0.pos;
			x1.here       = expr;
			x1.kind       = AL_EXPR;
			ast->head    -= 1;
			astStackPush(ast, &x1);
			return 1;
		}
	}
	
		
		
	// Expr Binop Expr
	if(astStackPeek(ast, 0, &x0) && (x0.kind == AL_EXPR) &&
	   astStackPeek(ast, 1, &x1) && (x1.kind == AL_TKN ) && isBinop(x1.tk.type) &&
	   astStackPeek(ast, 2, &x2) && (x2.kind == AL_EXPR)){
		ASTExpr* expr = malloc(sizeof(ASTExpr));
		*expr = (ASTExpr){.pos = fusePosition(x2.pos, x0.pos), .a=NULL, .b=NULL, .tk=x1.tk, .type=XT_BOP};
		expr->a       = (ASTExpr*)x2.here;
		expr->b       = (ASTExpr*)x0.here;
		x3.pos        = expr->pos;
		x3.here       = expr;
		x3.kind       = AL_EXPR;
		ast->head    -= 3;
		astStackPush(ast, &x3);
		return 1;
	}
	
	
	// Unop Expr
	if(astStackPeek(ast, 0, &x0) && (x0.kind == AL_EXPR) &&
	   astStackPeek(ast, 1, &x1) && (x1.kind == AL_TKN)  && isUnop(x1.tk.type)){
		ASTExpr* expr = malloc(sizeof(ASTExpr));
		*expr = (ASTExpr){.pos = fusePosition(x1.pos, x0.pos), .a=NULL, .b=NULL, .tk=x1.tk, .type=XT_UOP};
		expr->a       = (ASTExpr*)x0.here;
		x2.pos        = expr->pos;
		x2.here       = expr;
		x2.kind       = AL_EXPR;
		ast->head    -= 2;
		astStackPush(ast, &x2);
		return 1;
	}
		
	return 0;
}


int parseBlock(ASTList* blk, ErrorList* errs, ASTBlock* ret){
	ASTStack tks, ast;
	tks.stk = NULL; ast.stk = NULL;
	makeStacks(blk, &ast, &tks);
	
	ret->stmct = 0;
	
	int pass = 1;
	int cont = 1;
	while(cont){
		#ifdef PARSER_DEBUG
			printf("BK %i %i | ", tks.head, ast.head);
			printASTStack(ast);
		#endif
		
		ASTList x0, x1, x2, x3, x4, x5;
		
		
		if(exprParser(&ast, &tks, errs)) continue;
		
		if(tyElemParser(&ast, &tks, errs)) continue;
		
		if(constraintParser(&ast, &tks, errs)) continue;
		
		
		// ASGN =	_ :=
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.type == TKN_ASSIGN) &&
		   astStackPeek(&ast, 1, &x1) && (x1.kind == AL_TKN ) && (x1.tk.type == TKN_WILD)){
			ASTStmt* stmt = malloc(sizeof(ASTStmt));
			*stmt         = makeASTStmt(3, 2);
			ASTExpr  expr = (ASTExpr){.pos=x1.tk.pos, .a=NULL, .b=NULL, .xp=NULL, .type=XT_WILD};
			appendASTStmtRet(stmt, expr);
			x2.pos        = fusePosition(x1.tk.pos, x0.tk.pos);
			x2.here       = stmt;
			x2.kind       = AL_ASGN;
			ast.head     -= 2;
			astStackPush(&ast, &x2);
			continue;
		}
		
		// ASGN =	EXPR :=
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.type == TKN_ASSIGN) &&
		   astStackPeek(&ast, 1, &x1) && (x1.kind == AL_EXPR)){
			ASTStmt* stmt = malloc(sizeof(ASTStmt));
			*stmt         = makeASTStmt(3, 2);
			ASTExpr* expr = x1.here;
			appendASTStmtRet(stmt, *expr);
			free(expr);
			x2.pos        = fusePosition(x1.tk.pos, x0.tk.pos);
			x2.here       = stmt;
			x2.kind       = AL_ASGN;
			ast.head     -= 2;
			astStackPush(&ast, &x2);
			continue;
		}
		
		// ASGN =	_ , ASGN
		// ASGN =   _ , NL ASGN
		if((astStackPeek(&ast, 0, &x0) && (x0.kind == AL_ASGN) &&
		    astStackPeek(&ast, 1, &x1) && (x1.kind == AL_TKN ) && (x1.tk.type == TKN_COMMA ) &&
		    astStackPeek(&ast, 2, &x2) && (x2.kind == AL_TKN ) && (x2.tk.type == TKN_WILD  )) ||
		     
		   (astStackPeek(&ast, 0, &x0) && (x0.kind == AL_ASGN) &&
		    astStackPeek(&ast, 1, &x3) && (x3.kind == AL_TKN ) &&((x3.tk.type == TKN_NEWLINE) || (x3.tk.type == TKN_SEMICOLON)) &&
		    astStackPeek(&ast, 2, &x1) && (x1.kind == AL_TKN ) && (x1.tk.type == TKN_COMMA  ) &&
		    astStackPeek(&ast, 3, &x2) && (x2.kind == AL_TKN ) && (x2.tk.type == TKN_WILD   ))){
		    
			ASTStmt* stmt = (ASTStmt*)x0.here;
			ASTExpr  expr = (ASTExpr){.pos=x2.tk.pos, .a=NULL, .b=NULL, .xp=NULL, .type=XT_WILD};
			appendASTStmtExp(stmt, expr);
			x4.pos        = fusePosition(x2.tk.pos, x0.tk.pos);
			x4.here       = stmt;
			x4.kind       = AL_ASGN;
			ast.head     -= 2;
			astStackPush(&ast, &x4);
			continue;
		}
		
		// ASGN =	EXPR , ASGN
		// ASGN =   EXPR , NL ASGN
		x3.kind = AL_NIL;
		if((astStackPeek(&ast, 0, &x0) && (x0.kind == AL_ASGN) &&
		    astStackPeek(&ast, 1, &x1) && (x1.kind == AL_TKN ) && (x1.tk.type == TKN_COMMA) &&
		    astStackPeek(&ast, 2, &x2) && (x2.kind == AL_EXPR)) ||
		    
		   (astStackPeek(&ast, 0, &x0) && (x0.kind == AL_ASGN) &&
		    astStackPeek(&ast, 3, &x3) && (x3.kind == AL_TKN ) &&((x3.tk.type == TKN_NEWLINE) || (x3.tk.type == TKN_SEMICOLON)) &&
		    astStackPeek(&ast, 1, &x1) && (x1.kind == AL_TKN ) && (x1.tk.type == TKN_COMMA  ) &&
		    astStackPeek(&ast, 2, &x2) && (x2.kind == AL_EXPR))){
		    
			ASTStmt* stmt =  (ASTStmt*)x0.here;
			ASTExpr  expr = *(ASTExpr*)x2.here;
			appendASTStmtExp(stmt, expr);
			x4.pos        = fusePosition(x2.tk.pos, x0.tk.pos);
			x4.here       = stmt;
			x4.kind       = AL_ASGN;
			ast.head     -= (3 + (x3.kind == AL_TKN));
			astStackPush(&ast, &x4);
			continue;
		}
		
		
		// STMT =	ASGN EXPR NL	| ASGN =	ASGN EXPR ,		if ASGN has no exprs
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN ) && ((x0.tk.type == TKN_NEWLINE) || (x0.tk.type == TKN_SEMICOLON) || (x0.tk.type == TKN_COMMA)) &&
		   astStackPeek(&ast, 1, &x1) && (x1.kind == AL_EXPR) &&
		   astStackPeek(&ast, 2, &x2) && (x2.kind == AL_ASGN)){
		    ASTStmt* stmt = x2.here;
			if(stmt->expct == 0){
				ASTExpr* expr = x1.here;
				appendASTStmtExp(stmt, *expr);
				free(expr);
				x3.pos        = fusePosition(x2.pos, x0.pos);
				x3.here       = stmt;
				x3.kind       = (x0.tk.type == TKN_COMMA)? AL_ASGN : AL_STMT;
				ast.head     -= 3;
				astStackPush(&ast, &x3);
				continue;
			}
		}
		
		// STMT =	ASGN NL EXPR NL		| ASGN =	ASGN NL EXPR ,		if ASGN has no exprs
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN ) && ((x0.tk.type == TKN_NEWLINE) || (x0.tk.type == TKN_SEMICOLON) || (x0.tk.type == TKN_COMMA)) &&
		   astStackPeek(&ast, 1, &x1) && (x1.kind == AL_EXPR) &&
		   astStackPeek(&ast, 2, &x2) && (x2.kind == AL_TKN ) && ((x2.tk.type == TKN_NEWLINE) || (x2.tk.type == TKN_NEWLINE)) &&
		   astStackPeek(&ast, 3, &x3) && (x3.kind == AL_ASGN)){
		   	ASTStmt* stmt = x3.here;
			if(stmt->expct == 0){
				ASTExpr* expr = x1.here;
				appendASTStmtExp(stmt, *expr);
				free(expr);
				x4.pos        = fusePosition(x3.pos, x0.pos);
				x4.here       = stmt;
				x4.kind       = (x0.tk.type == TKN_COMMA)? AL_ASGN : AL_STMT;
				ast.head     -= 4;
				astStackPush(&ast, &x4);
				continue;
			}
		}
		
		// STMT =	ASGN EXPR NL
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN ) &&((x0.tk.type == TKN_NEWLINE) || (x0.tk.type == TKN_SEMICOLON)) &&
		   astStackPeek(&ast, 1, &x1) && (x1.kind == AL_EXPR) &&
		   astStackPeek(&ast, 2, &x2) && (x2.kind == AL_ASGN)){
		    ASTStmt* stmt = x2.here;
		    ASTExpr* expr = x1.here;
		    appendASTStmtExp(stmt, *expr);
		    free(expr);
		    x3.pos        = fusePosition(x2.pos, x0.pos);
		    x3.here       = stmt;
		    x3.kind       = AL_STMT;
		    ast.head     -= 3;
		    astStackPush(&ast, &x3);
		    continue;
		}
		
		// STMT =	ASGN NL EXPR NL
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN ) && ((x0.tk.type == TKN_NEWLINE) || (x0.tk.type == TKN_SEMICOLON)) &&
		   astStackPeek(&ast, 1, &x1) && (x1.kind == AL_EXPR) &&
		   astStackPeek(&ast, 2, &x2) && (x2.kind == AL_TKN ) && ((x2.tk.type == TKN_NEWLINE) || (x2.tk.type == TKN_SEMICOLON)) &&
		   astStackPeek(&ast, 3, &x3) && (x3.kind == AL_ASGN)){
		    ASTStmt* stmt = x3.here;
		    ASTExpr* expr = x0.here;
		    appendASTStmtExp(stmt, *expr);
		    free(expr);
		    x4.pos        = fusePosition(x3.pos, x0.pos);
		    x4.here       = stmt;
		    x4.kind       = AL_STMT;
		    ast.head     -= 4;
		    astStackPush(&ast, &x4);
		    continue;
		}
		
		// STMT =	ASGN* NL		if ASGN has exprs
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN ) && ((x0.tk.type == TKN_NEWLINE) || (x0.tk.type == TKN_SEMICOLON)) &&
		   astStackPeek(&ast, 1, &x1) && (x1.kind == AL_ASGN)){
		    ASTStmt* stmt = x1.here;
			x2.pos        = fusePosition(x1.pos, x0.pos);
			x2.here       = stmt;
			x2.kind       = (stmt->expct)? AL_STMT : AL_ASGN;
			ast.head     -= 2;
			astStackPush(&ast, &x2);
			continue;
		}
		
		// STMT =	CNST
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_CNST)){
			ASTCnst* cnst = x0.here;
			ASTStmt* stmt = malloc(sizeof(ASTStmt));
			*stmt         = makeASTStmt(4, 2);
			stmt->pos     = x0.pos;
			appendASTStmtCnst(stmt, *cnst);
			free(cnst);
			x1.pos        = x0.pos;
			x1.kind       = AL_STMT;
			x1.here       = stmt;
			ast.head     -= 1;
			astStackPush(&ast, &x1);
			continue;
		}
		
		
		// STMS = STMT STMT
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_STMT) &&
		   astStackPeek(&ast, 1, &x1) && (x1.kind == AL_STMT)){
			ASTBlock* blk = malloc(sizeof(ASTBlock));
			*blk      = makeASTBlock(8);
			appendASTBlockStmt(blk, *(ASTStmt*)x0.here);
			appendASTBlockStmt(blk, *(ASTStmt*)x1.here);
			x2.pos    = fusePosition(x1.pos, x0.pos);
			x2.here   = blk;
			x2.kind   = AL_STMS;
			ast.head -= 2;
			astStackPush(&ast, &x2);
			continue;
		}
		
		
		// STMS = STMS STMT
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_STMT) &&
		   astStackPeek(&ast, 1, &x1) && (x1.kind == AL_STMS)){
			ASTBlock* blk = x1.here;
			appendASTBlockStmt(blk, *(ASTStmt*)x0.here);
			x2.pos    = fusePosition(x1.pos, x0.pos);
			x2.here   = blk;
			x2.kind   = AL_STMS;
			ast.head -= 2;
			astStackPush(&ast, &x2);
			continue;
		}
		
		// BLOK = STMS EXPR EOF
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_EXPR) &&
		   astStackPeek(&ast, 1, &x1) && (x1.kind == AL_STMS) && (ast.head == 2) && (tks.head == 0)){
			*ret       = *(ASTBlock*)x1.here;
			ret->pos   = fusePosition(x1.pos, x0.pos);
			ret->retx  = *(ASTExpr*)x0.here; free(x0.here);
			pass       = 1;
			cont       = 0;
			continue;
		}
		
		// BLOK = STMT EXPR EOF
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_EXPR) &&
		   astStackPeek(&ast, 1, &x1) && (x1.kind == AL_STMT) && (ast.head == 2) && (tks.head == 0)){
			*ret       = makeASTBlock(1);
			appendASTBlockStmt(ret, *(ASTStmt*)x1.here); free(x1.here);
			ret->pos   = fusePosition(x1.pos, x0.pos);
			ret->retx  = *(ASTExpr*)x0.here;             free(x0.here);
			pass       = 1;
			cont       = 0;
			continue;
		}
		
		// BLOK = SOF  EXPR EOF
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_EXPR) && (ast.head == 1) && (tks.head == 0)){
			ret->pos   = x0.pos;
			ret->stmts = NULL;
			ret->retx  = *(ASTExpr*)x0.here; free(x0.here);
			ret->stmct = 0;
			pass       = 1;
			cont       = 0;
			continue;
		}
		
		
		// SOF NL		|	 NL EOF
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN) &&
		 ((x0.tk.type == TKN_NEWLINE) || (x0.tk.type == TKN_SEMICOLON)) && ((ast.head == 1) || (tks.head == 0))){
			ast.head--;
			continue;
		}
		
		// Comment Removal
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_COMMENT  )){ast.head--; continue; }
		if(astStackPeek(&ast, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.type == TKN_COMMS    )){ast.head--; continue; }
		
		
		void* xval;
		int step = parseStep(&tks, &ast, 0, AL_BLOK, &xval);
		if(!step){
			*ret = *(ASTBlock*)xval;
			cont = 0;
		}else if(step < 0){
			pass = 0;
			cont = 0;
		}
	}
	
	free(ast.stk);
	free(tks.stk);
	return pass;
}
*/

/*==========================================

		EXPR PARSER

==========================================*/
int subparseBlock(ASTList* blk, ASTBlock* ret, ErrorList* errs){
	return 1;
}






/*==========================================

		TYPE PARSER

==========================================*/
int separatorRules(ASTStack* stk, ASTStack* tks){
	ASTList x0, x1;
	
	// NL / ;
	if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TKN ) && ((x0.tk.tk.type == TKN_NEWLINE) || (x0.tk.tk.type == TKN_SEMICOLON))){
		stk->head--;
		ASTList sp   = x0;
		sp.kind      = AL_SEPR;
		astStackPush(stk, &sp);
		return 1;
	}
		
	// EOF
	if(astStackPeek(stk, 0, &x0) && (x0.kind != AL_SEPR) && (tks->size < 1)){
		ASTList sp   = x0;
		sp.kind      = AL_SEPR;
		astStackPush(stk, &sp);
		return 1;
	}
		
	// SEPR		SEPR
	if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_SEPR) &&
	   astStackPeek(stk, 1, &x1) && (x1.kind == AL_SEPR)){
		stk->head--;
		return 1;
	}
	
	// SOF  SEPR
	if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_SEPR) && (stk->head == 1)){ stk->head--; return 1; }
	
	return 0;
}

int typeRule(ASTStack*, ASTStack*, int, ErrorList*);

int commentRule(ASTStack* stk, ASTStack* tks){
	// Comment and Newline Removal
	ASTList x0;
	if((astStackPeek(stk, 0, &x0)) && (x0.kind == AL_TKN ) && ((x0.tk.tk.type == TKN_COMMENT) || (x0.tk.tk.type == TKN_COMMS    ))){ stk->head--; return 1; }
	if((astStackPeek(stk, 0, &x0)) && (x0.kind == AL_TKN ) && ((x0.tk.tk.type == TKN_NEWLINE) || (x0.tk.tk.type == TKN_SEMICOLON))){ stk->head--; return 1; }
	return 0;
}

int constraintRule(ASTStack* stk, ASTStack* tks){

	ASTList x0, x1, x2, x3;
	
	// |:  EXPR  SEPR
	if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_SEPR) &&
	   astStackPeek(stk, 1, &x1) && (x1.kind == AL_BRK ) &&		// Make this EXPR at some point
	   astStackPeek(stk, 2, &x2) && (x2.kind == AL_TKN ) &&  (x2.tk.tk.type == TKN_CONSTRAIN)){
		// Constraint Line
		stk->head -= 3;
		return 1;
	}
	return 0;
}


int subparseStrc(ASTList* lst, ASTStruct* ret, ErrorList* errs){
	ASTLine  ln  = toLine(lst);
	ASTStack tks = lineToStack(&ln);
	ASTStack stk = makeEmptyStack(ln.size);
	int pass = 0;
	if(0){
		end:
		free(tks.stk);
		free(stk.stk);
		free(ln .lst);
		return pass;
	}
	int cont = 1;
	while(cont){
		#ifdef PARSER_DEBUG
			printf("ST %i %i | ", tks.head, stk.head);
			printASTStack(stk);
		#endif
		ASTList x0, x1, x2, x3, x4, x5;
		
		if(separatorRules(&stk, &tks)) continue;
		
		// TYLM :		[no header]		-- This is not a struct; it is a Type Build
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.tk.type == TKN_COLON) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TYPE)){
		   	if(stk.head == 2){
		    	pass = 0;
		    	goto end;
		    }
		}
		
		// ID  :  TYPE  SEPR
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_SEPR) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TYPE) &&
		   astStackPeek(&stk, 2, &x2) && (x2.kind == AL_TKN ) &&  (x2.tk.tk.type == TKN_COLON) &&
		   astStackPeek(&stk, 3, &x3) && (x3.kind == AL_TKN ) &&  (x3.tk.tk.type == TKN_S_ID)){
			// Struct line
			stk.head    -= 4;
			ASTList sf   = x0;
			sf.sf        = (AS_SF){.fld=x3.tk.tk.data.i64, .type=x1.type.ty};
			sf.kind      = AL_STLN;
			astStackPush(&stk, &sf);
			continue;
		}
		
		// Constraints
		if(constraintRule(&stk, &tks)) continue;
		
		
		// SOF  SF
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_STLN) && (stk.head == 1)){
			stk.head--;
			ASTList st   = x0;
			List nms=makeList(4, sizeof(int64_t)), tys=makeList(4, sizeof(ASTType)), cts=makeList(2, sizeof(ASTCnst));
			appendList(&nms, &x0.sf.fld );
			appendList(&tys, &x0.sf.type);
			st.strc.strc = (ASTStruct){.pos=x0.pos, .names=nms, .types=tys, .cnsts=cts};
			st.kind      = AL_STLS;
			astStackPush(&stk, &st);
			continue;
		}
		
		
		// SOF  CS
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_CNST) && (stk.head == 1)){
			stk.head--;
			ASTList st   = x0;
			List nms=makeList(4, sizeof(int64_t)), tys=makeList(4, sizeof(ASTType)), cts=makeList(2, sizeof(ASTCnst));
			appendList(&cts, &x0.cnst.cnst);
			st.strc.strc = (ASTStruct){.pos=x0.pos, .names=nms, .types=tys, .cnsts=cts};
			st.kind      = AL_STLS;
			astStackPush(&stk, &st);
			continue;
		}
		
		
		// SFS SF
		if(astStackPeek(&stk, 1, &x1) && (x1.kind == AL_STLS) &&
		   astStackPeek(&stk, 0, &x0) && (x0.kind == AL_STLN)){
			stk.head    -= 2;
			ASTList st   = x1;
			appendList(&st.strc.strc.names, &x0.sf.fld );
			appendList(&st.strc.strc.types, &x0.sf.type);
			st.kind      = AL_STLS;
			astStackPush(&stk, &st);
			continue;
		}
		
		
		// SFS CS
		if(astStackPeek(&stk, 1, &x1) && (x1.kind == AL_STLS) &&
		   astStackPeek(&stk, 0, &x0) && (x0.kind == AL_CNST)){
			stk.head    -= 2;
			ASTList st   = x1;
			appendList(&st.strc.strc.cnsts, &x0.cnst.cnst);
			st.kind      = AL_STLS;
			astStackPush(&stk, &st);
			continue;
		}
		
		// SFS SEPR
		if(astStackPeek(&stk, 1, &x1) && (x1.kind == AL_STLS) &&
		   astStackPeek(&stk, 0, &x0) && (x0.kind == AL_SEPR)){
			stk.head--;
			continue;
		}
		
		if(typeRule(&stk, &tks, 0, errs)) continue;
		
		if(commentRule(&stk, &tks)) continue;
	
		ASTList xval;
		int step = parseStep(&tks, &stk, 0, AL_STLS, &xval);
		if(!step){
			*ret = xval.strc.strc;
			cont = 0;
			pass = 1;
		}else if(step < 0){
			cont = 0;
			pass = 0;
		}
		
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_NIL)) return 0;
	}
	
	goto end;
}


int subparseUnon(ASTList* lst, ASTUnion*  ret, ErrorList* errs){
	ASTLine  ln  = toLine(lst);
	ASTStack tks = lineToStack(&ln);
	ASTStack stk = makeEmptyStack(ln.size);
	ret->tagid   = 0;
	ret->tagty   = 0;
	
	int pass = 0;
	if(0){
		end:
		free(tks.stk);
		free(stk.stk);
		free(ln .lst);
		return pass;
	}
	int cont = 1;
	while(cont){
		#ifdef PARSER_DEBUG
			printf("UN %i %i | ", tks.head, stk.head);
			printASTStack(stk);
		#endif
		ASTList x0, x1, x2, x3, x4, x5, x6, x7;
		
		if(separatorRules(&stk, &tks)) continue;
		
		// SOF  ID	TYID/BITY  :
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.tk.type == TKN_COLON) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TKN ) && (stk.head      ==         3) &&
		   astStackPeek(&stk, 2, &x2) && (x2.kind == AL_TKN ) && (x2.tk.tk.type == TKN_S_ID ) &&
		  ((x1.tk.tk.type == TKN_S_TYID) || ((x1.tk.tk.type == TKN_BID) && isTypeBID(x1.tk.tk.type)))){
		    printf("TAG %li %li\n", x1.tk.tk.data.i64, x2.tk.tk.data.i64);
			stk.head    -= 3;
			ASTList un   = x1;
			un.tag.tyid  = x1.tk.tk.data.i64;
			un.tag.tgid  = x2.tk.tk.data.i64;
			un.kind      = AL_TAG;
			astStackPush(&stk, &un);
			continue;
		}
		
		// SOF  :
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.tk.type == TKN_COLON) && (stk.head == 1)){
			stk.head--;
			ASTList un   = x1;
			un.tag.tyid  = 0;
			un.tag.tgid  = 0;
			un.kind      = AL_TAG;
			astStackPush(&stk, &un);
			continue;
		}
		
		// TAG SEPR
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_SEPR) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TAG)){
			stk.head--;
			continue;
		}
		
		// TYID  :  TYPE  SEPR
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_SEPR) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TYPE) &&
		   astStackPeek(&stk, 2, &x2) && (x2.kind == AL_TKN ) &&  (x2.tk.tk.type == TKN_COLON) &&
		   astStackPeek(&stk, 3, &x3) && (x3.kind == AL_TKN ) &&  (x3.tk.tk.type == TKN_S_TYID)){
			// Union line
			stk.head    -= 4;
			ASTList uf   = x0;
			uf.uf        = (AS_UF){.fld=x3.tk.tk.data.i64, .type=x1.type.ty, .tag=0};
			uf.kind      = AL_UNLN;
			astStackPush(&stk, &uf);
			continue;
		}
		
		
		// -   INT  =	TYID	:  TYPE  SEPR
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_SEPR) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TYPE) &&
		   astStackPeek(&stk, 2, &x2) && (x2.kind == AL_TKN ) &&  (x2.tk.tk.type == TKN_COLON)  &&
		   astStackPeek(&stk, 3, &x3) && (x3.kind == AL_TKN ) &&  (x3.tk.tk.type == TKN_S_TYID) &&
		   astStackPeek(&stk, 4, &x4) && (x4.kind == AL_TKN ) &&  (x4.tk.tk.type == TKN_EQL   ) &&
		   astStackPeek(&stk, 5, &x5) && (x5.kind == AL_TKN ) &&  (x5.tk.tk.type == TKN_INT   ) &&
		   astStackPeek(&stk, 6, &x6) && (x6.kind == AL_TKN ) &&  (x6.tk.tk.type == TKN_SUB   )){
			// Union line
			stk.head    -= 7;
			ASTList uf   = x0;
			uf.uf        = (AS_UF){.fld=x3.tk.tk.data.i64, .type=x1.type.ty, .tag=-x5.tk.tk.data.i64};
			uf.kind      = AL_UNLN;
			astStackPush(&stk, &uf);
			continue;
		}
		
		
		// INT   =	TYID	:  TYPE  SEPR
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_SEPR) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TYPE) &&
		   astStackPeek(&stk, 2, &x2) && (x2.kind == AL_TKN ) &&  (x2.tk.tk.type == TKN_COLON)  &&
		   astStackPeek(&stk, 3, &x3) && (x3.kind == AL_TKN ) &&  (x3.tk.tk.type == TKN_S_TYID) &&
		   astStackPeek(&stk, 4, &x4) && (x4.kind == AL_TKN ) &&  (x4.tk.tk.type == TKN_EQL   ) &&
		   astStackPeek(&stk, 5, &x5) && (x5.kind == AL_TKN ) &&  (x5.tk.tk.type == TKN_INT   )){
			// Union line
			stk.head    -= 6;
			ASTList uf   = x0;
			uf.uf        = (AS_UF){.fld=x3.tk.tk.data.i64, .type=x1.type.ty, .tag=x5.tk.tk.data.i64};
			uf.kind      = AL_UNLN;
			astStackPush(&stk, &uf);
			continue;
		}
		
		
		// Constraints
		if(constraintRule(&stk, &tks)) continue;
		
		
		// SOF UF
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_UNLN) && (stk.head == 1)){
			stk.head--;
			ASTList un   = x0;
			List nms=makeList(4, sizeof(int64_t)), tys=makeList(4, sizeof(ASTType)), vls=makeList(4, sizeof(int64_t)), cts=makeList(2, sizeof(ASTCnst));
			appendList(&nms, &x0.uf.fld );
			appendList(&tys, &x0.uf.type);
			appendList(&vls, &x0.uf.tag );
			un.unon.unon = (ASTUnion){.pos=x0.pos, .names=nms, .types=tys, .vals=vls, .cnsts=cts, .tagty=0, .tagid=0};
			un.kind      = AL_UNLS;
			astStackPush(&stk, &un);
			continue;
		}
		
		// SOF CS
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_CNST) && (stk.head == 1)){
			stk.head--;
			ASTList un   = x0;
			List nms=makeList(4, sizeof(int64_t)), tys=makeList(4, sizeof(ASTType)), vls=makeList(4, sizeof(int64_t)), cts=makeList(2, sizeof(ASTCnst));
			appendList(&cts, &x0.cnst.cnst);
			un.unon.unon = (ASTUnion){.pos=x0.pos, .names=nms, .types=tys, .vals=vls, .cnsts=cts, .tagty=0, .tagid=0};
			un.kind      = AL_UNLS;
			astStackPush(&stk, &un);
			continue;
		}
		
		
		// UH  UF
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_UNLN) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TAG)){
			stk.head    -= 2;
			ASTList un   = x1;
			List nms=makeList(4, sizeof(int64_t)), tys=makeList(4, sizeof(ASTType)), vls=makeList(4, sizeof(int64_t)), cts=makeList(2, sizeof(ASTCnst));
			appendList(&nms, &x0.uf.fld );
			appendList(&tys, &x0.uf.type);
			appendList(&vls, &x0.uf.tag );
			un.unon.unon = (ASTUnion){.pos=x1.pos, .names=nms, .types=tys, .vals=vls, .cnsts=cts, .tagty=x1.tag.tyid, .tagid=x1.tag.tgid};
			un.kind      = AL_UNLS;
			astStackPush(&stk, &un);
			continue;
		}
		
		
		// UH  CS
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_CNST) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TAG)){
			stk.head    -= 2;
			ASTList un   = x1;
			List nms=makeList(4, sizeof(int64_t)), tys=makeList(4, sizeof(ASTType)), vls=makeList(4, sizeof(int64_t)), cts=makeList(2, sizeof(ASTCnst));
			appendList(&cts, &x0.cnst.cnst);
			un.unon.unon = (ASTUnion){.pos=x1.pos, .names=nms, .types=tys, .vals=vls, .cnsts=cts, .tagty=x1.tag.tyid, .tagid=x1.tag.tgid};
			un.kind      = AL_UNLS;
			astStackPush(&stk, &un);
			continue;
		}
		
		
		// UFS UF
		if(astStackPeek(&stk, 1, &x1) && (x1.kind == AL_UNLS) &&
		   astStackPeek(&stk, 0, &x0) && (x0.kind == AL_UNLN)){
			stk.head    -= 2;
			ASTList un   = x1;
			appendList(&un.unon.unon.names, &x0.uf.fld );
			appendList(&un.unon.unon.types, &x0.uf.type);
			appendList(&un.unon.unon.vals , &x0.uf.tag );
			un.kind      = AL_UNLS;
			astStackPush(&stk, &un);
			continue;
		}
		
		
		// UFS CS
		if(astStackPeek(&stk, 1, &x1) && (x1.kind == AL_UNLS) &&
		   astStackPeek(&stk, 0, &x0) && (x0.kind == AL_CNST)){
			stk.head    -= 2;
			ASTList un   = x1;
			appendList(&un.unon.unon.cnsts, &x0.cnst.cnst);
			un.kind      = AL_UNLS;
			astStackPush(&stk, &un);
			continue;
		}
		
		// UFS SEPR
		if(astStackPeek(&stk, 1, &x1) && (x1.kind == AL_UNLS) &&
		   astStackPeek(&stk, 0, &x0) && (x0.kind == AL_SEPR)){
			stk.head--;
			continue;
		}
		
		int typePass = 1;
		if(astStackPeek(&tks, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.tk.type == TKN_COLON)){
			typePass = 0;
		}
		if(typePass && typeRule(&stk, &tks, 0, errs)) continue;
		
		if(commentRule(&stk, &tks)) continue;
		
		ASTList xval;
		int step = parseStep(&tks, &stk, 0, AL_UNLS, &xval);
		if(!step){
			*ret = xval.unon.unon;
			cont = 0;
			pass = 1;
		}else if(step < 0){
			cont = 0;
			pass = 0;
		}
	}
	
	goto end;
}


int subparseEnum(ASTList* lst, ASTEnum*   ret, ErrorList* errs){
	ASTLine  ln  = toLine(lst);
	ASTStack tks = lineToStack(&ln);
	ASTStack stk = makeEmptyStack(ln.size);
	int pass = 0;	
	if(0){
		end:
		free(tks.stk);
		free(stk.stk);
		free(ln .lst);
		return pass;
	}
	int cont = 1;
	while(cont){
		#ifdef PARSER_DEBUG
			printf("EN %i %i | ", tks.head, stk.head);
			printASTStack(stk);
		#endif
		ASTList x0, x1, x2, x3, x4, x5;
		
		if(separatorRules(&stk, &tks)) continue;
		
		// SOF  ID	TYID/BITY  :			(Union header, this should not be here!)
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.tk.type == TKN_COLON) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TKN ) && (stk.head      ==         3) &&
		   astStackPeek(&stk, 2, &x2) && (x2.kind == AL_TKN ) && (x2.tk.tk.type == TKN_S_ID ) &&
		  ((x1.tk.tk.type == TKN_S_TYID) || ((x1.tk.tk.type == TKN_BID) && isTypeBID(x1.tk.tk.type)))){
			pass = 0;
			goto end;
		}
		// SOF  :							(Union header, this should not be here!)
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.tk.type == TKN_COLON) && (stk.head == 1)){
			pass = 0;
			goto end;
		}
		
		// SOF  TYID/BITY  :
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.tk.type == TKN_COLON) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TKN ) && (stk.head      ==         2) &&
		  ((x1.tk.tk.type == TKN_S_TYID) || ((x1.tk.tk.type == TKN_BID) && isTypeBID(x1.tk.tk.type)))){
			stk.head    -= 2;
			ASTList en   = x1;
			en.tag.tyid  = x1.tk.tk.data.i64;
			en.kind      = AL_TAG;
			astStackPush(&stk, &en);
			continue;
		}
		
		// TAG SEPR
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_SEPR) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TAG)){
			stk.head--;
			continue;
		}
		
		// -   INT  =	TYID  SEPR
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_SEPR) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TKN ) &&  (x1.tk.tk.type == TKN_S_TYID) &&
		   astStackPeek(&stk, 2, &x2) && (x2.kind == AL_TKN ) &&  (x2.tk.tk.type == TKN_EQL   ) &&
		   astStackPeek(&stk, 3, &x3) && (x3.kind == AL_TKN ) &&  (x3.tk.tk.type == TKN_INT   ) &&
		   astStackPeek(&stk, 4, &x4) && (x4.kind == AL_TKN ) &&  (x4.tk.tk.type == TKN_SUB   )){
			// Struct line
			stk.head    -= 5;
			ASTList ef   = x0;
			ef.ef        = (AS_EF){.fld=x3.tk.tk.data.i64, .tag=-x5.tk.tk.data.i64};
			ef.kind      = AL_ENLN;
			astStackPush(&stk, &ef);
			continue;
		}
		
		// INT   =	TYID  SEPR
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_SEPR) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TKN ) &&  (x1.tk.tk.type == TKN_S_TYID) &&
		   astStackPeek(&stk, 2, &x2) && (x2.kind == AL_TKN ) &&  (x2.tk.tk.type == TKN_EQL   ) &&
		   astStackPeek(&stk, 3, &x3) && (x3.kind == AL_TKN ) &&  (x3.tk.tk.type == TKN_INT   )){
			// Struct line
			stk.head    -= 4;
			ASTList ef   = x0;
			ef.ef        = (AS_EF){.fld=x1.tk.tk.data.i64, .tag=x3.tk.tk.data.i64};
			ef.kind      = AL_ENLN;
			astStackPush(&stk, &ef);
			continue;
		}
		
		
		// Constraints
		if(constraintRule(&stk, &tks)) continue;
		
		
		// EH  EF
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_ENLN) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TAG)){
			stk.head    -= 2;
			ASTList en   = x1;
			List tgs=makeList(4, sizeof(int64_t)), vls=makeList(4, sizeof(int64_t)), cts=makeList(2, sizeof(ASTCnst));
			appendList(&tgs, &x0.uf.fld );
			appendList(&vls, &x0.uf.tag );
			en.enmt.enmt = (ASTEnum){.pos=x1.pos, .tags=tgs, .vals=vls, .cnsts=cts, .tagty=x1.tag.tyid};
			en.kind      = AL_ENLS;
			astStackPush(&stk, &en);
			continue;
		}
		
		
		// EH  CS
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_CNST) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TAG)){
			stk.head    -= 2;
			ASTList en   = x1;
			List tgs=makeList(4, sizeof(int64_t)), vls=makeList(4, sizeof(int64_t)), cts=makeList(2, sizeof(ASTCnst));
			appendList(&cts, &x0.cnst.cnst);
			en.enmt.enmt = (ASTEnum){.pos=x1.pos, .tags=tgs, .vals=vls, .cnsts=cts, .tagty=x1.tag.tyid};
			en.kind      = AL_ENLS;
			astStackPush(&stk, &en);
			continue;
		}
		
		
		// EFS EF
		if(astStackPeek(&stk, 1, &x1) && (x1.kind == AL_ENLS) &&
		   astStackPeek(&stk, 0, &x0) && (x0.kind == AL_ENLN)){
			stk.head    -= 2;
			ASTList en   = x1;
			appendList(&en.enmt.enmt.tags, &x0.ef.fld );
			appendList(&en.enmt.enmt.vals, &x0.ef.tag );
			en.kind      = AL_ENLS;
			astStackPush(&stk, &en);
			continue;
		}
		
		
		// EFS CS
		if(astStackPeek(&stk, 1, &x1) && (x1.kind == AL_ENLS) &&
		   astStackPeek(&stk, 0, &x0) && (x0.kind == AL_CNST)){
			stk.head    -= 2;
			ASTList en   = x1;
			appendList(&en.enmt.enmt.cnsts, &x0.cnst.cnst);
			en.kind      = AL_ENLS;
			astStackPush(&stk, &en);
			continue;
		}
		
		// EFS SEPR
		if(astStackPeek(&stk, 1, &x1) && (x1.kind == AL_ENLS) &&
		   astStackPeek(&stk, 0, &x0) && (x0.kind == AL_SEPR)){
			stk.head--;
			continue;
		}
		
		if(commentRule(&stk, &tks)) continue;
		
		ASTList xval;
		int step = parseStep(&tks, &stk, 0, AL_ENLS, &xval);
		if(!step){
			*ret = xval.enmt.enmt;
			cont = 0;
			pass = 1;
		}else if(step < 0){
			cont = 0;
			pass = 0;
		}
	}
	
	goto end;
}


int subparseBuild(ASTList* brk, ErrorList* errs, ASTBuild* ret){
	if(brk->kind == AL_AGEN) return 0;

	ASTList* lst = brk->wrap.here;
	ASTLine  ln  = toLine(lst);
	ASTStack tks = lineToStack(&ln);
	ASTStack stk = makeEmptyStack(ln.size);
	ret->recipe  = NULL;
	ret->fields  = makeList(4, sizeof(ASTTyElem));
	
	int pass   = 1;
	int header = 0;
	if(0){
		end:
		free(tks.stk);
		free(stk.stk);
		free(ln .lst);
		if(!pass){
			if(ret->recipe != NULL) free(ret->recipe);
			freeList(&ret->fields);
		}
		return pass;
	}
	
	int cont = 1;
	while(cont){
		#ifdef PARSER_DEBUG
			printf("BD %i %i | ", tks.head, stk.head);
			printASTStack(stk);
		#endif
		
		ASTList x0, x1, x2, x3, x4;
		
		// _
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.tk.type == TKN_WILD)){
			appendList(&errs->errs, &(Error){ERR_P_UNX_WILD, x1.pos});
			pass = 0;
			goto end;
		}
		
		// ID
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.tk.type == TKN_S_ID)){
			if(header) appendList(&errs->errs, &(Error){ERR_P_UNX_IDEN, x1.pos});
			pass = 0;
			goto end;
		}
		
		if(separatorRules(&stk, &tks         )) continue;
		if(commentRule   (&stk, &tks         )) continue;
		if(typeRule      (&stk, &tks, 1, errs)) continue;
		
		// SEPR
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_SEPR)){ stk.head--; continue; }
		
		// TYLM :		[no header]
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.tk.type == TKN_COLON) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TYLM) && !header){
		   	if(stk.head == 2){
		    	stk.head -= 2;
		    	header    = 1;
		    	ASTTyElem* lm = malloc(sizeof(ASTTyElem));
		    	*lm           = x1.tylm.lm;
		    	ret->recipe   = lm;
		    	continue;
		    }else{
		    	appendList(&errs->errs, &(Error){ERR_P_BAD_BDHD, x1.pos});
		    	pass = 0;
		    	goto end;
		    }
		}
		
		// TYLM ,		[header]
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.tk.type == TKN_COMMA) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TYLM)){
		   	if(header){
		    	stk.head -= 2;
		    	appendList(&ret->fields, &x1.tylm.lm);
				continue;
			}else{
				appendList(&errs->errs, &(Error){ERR_P_MIS_BDHD, x1.pos});
				pass = 0;
				goto end;
			}
		}
		
		// TYLM EOF		[header]
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TYLM) && (tks.head == 0)){
		   	if(header){
		    	stk.head--;
		    	appendList(&ret->fields, &x0.tylm.lm);
				continue;
			}else{
				appendList(&errs->errs, &(Error){ERR_P_MIS_BDHD, x1.pos});
				pass = 0;
				goto end;
			}
		}
		
		ASTList tk;
		if      ((tks.head == 0) && (stk.head == 0)){
			pass = 1; cont = 0;
		}else if((tks.head == 0) && (stk.head != 0)){
			pass = 0; cont = 0;
		}else if(astStackPop(&tks, &tk)){
			if(!astStackPush(&stk, &tk)){ printf("AST Stack overflow.\n"); exit(-1); }
		}
	}
	goto end;
}


int elemListParser(ASTList* brk, ErrorList* errs, List* ret){
	if(brk->kind == AL_AGEN){
		*ret = (List){0, 0, 0, 0};
		return 1;
	}

	*ret = makeList(4, sizeof(ASTTyElem));

	ASTList* lst = brk->wrap.here;
	ASTLine  ln  = toLine(lst);
	ASTStack tks = lineToStack(&ln);
	ASTStack stk = makeEmptyStack(ln.size);
	
	int pass = 1;
	if(0){
		end:
		free(tks.stk);
		free(stk.stk);
		free(ln .lst);
		if(!pass) freeList(ret);
		return pass;
	}
	
	int cont = 1;
	while(cont){
		#ifdef PARSER_DEBUG
			printf("LL %i %i | ", tks.head, stk.head);
			printASTStack(stk);
		#endif
		ASTList x0, x1;
		
		if(separatorRules(&stk, &tks         )) continue;
		if(commentRule   (&stk, &tks         )) continue;
		if(typeRule      (&stk, &tks, 1, errs)) continue;
		
		// SEPR
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_SEPR)){ stk.head--; continue; }
		
		// ID, MID, or WILD  - error
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TKN)){
			if(x0.tk.tk.type == TKN_S_ID ){ appendList(&errs->errs, &(Error){ERR_P_UNX_FTID, x0.pos}); pass = 0; goto end;}
			if(x0.tk.tk.type == TKN_S_MID){ appendList(&errs->errs, &(Error){ERR_P_UNX_FTMI, x0.pos}); pass = 0; goto end;}
			if(x0.tk.tk.type == TKN_WILD ){ appendList(&errs->errs, &(Error){ERR_P_UNX_FTWD, x0.pos}); pass = 0; goto end;}
		}
		
		// TYLM ,
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TKN ) &&  (x0.tk.tk.type == TKN_COMMA) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TYLM)){
			stk.head -= 2;
			appendList(ret, &x1.tylm.lm);
			continue;
		}
		
		// TYLM EOF
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TYLM) &&  (tks.head      ==         0)){
			stk.head--;
			appendList(ret, &x0.tylm.lm);
			continue;
		}
		
		ASTList tk;
		if      ((tks.head == 0) && (stk.head == 0)){
			pass = 1; cont = 0;
		}else if((tks.head == 0) && (stk.head != 0)){
			pass = 0; cont = 0;
		}else if(astStackPop(&tks, &tk)){
			if(!astStackPush(&stk, &tk)){ printf("AST Stack overflow.\n"); exit(-1); }
		}
	}
	goto end;
}



int typeAtomRule(ASTStack* stk, ASTStack* tks, ErrorList* errs){
	
	#ifdef PARSER_DEBUG
		printf("TA %i %i | ", tks->head, stk->head);
		printASTStack(*stk);
	#endif
	
	ASTList x0, x1, x2, x3, x4, x5, x6;
	
	// |> [] => [] -> []
	if( astStackPeek(stk, 5, &x5) &&  (x5.kind == AL_TKN ) && (x5.tk.tk.type == TKN_FNTY     ) &&
	    astStackPeek(stk, 4, &x4) && ((x4.kind == AL_BRK ) || (x4.kind       == AL_AGEN     )) &&
	    astStackPeek(stk, 3, &x3) &&  (x3.kind == AL_TKN ) && (x3.tk.tk.type == TKN_R_DARROW ) &&
	    astStackPeek(stk, 2, &x2) && ((x2.kind == AL_BRK ) || (x2.kind       == AL_AGEN     )) &&
	    astStackPeek(stk, 1, &x1) &&  (x1.kind == AL_TKN ) && (x1.tk.tk.type == TKN_R_ARROW  ) &&
	    astStackPeek(stk, 0, &x0) && ((x0.kind == AL_BRK ) || (x0.kind       == AL_AGEN      ))){
		// FnTy
		stk->head   -= 6;
		ASTList ta   = x0;
		List tps, prs, rts;
		// Parse tps, pars and rets, error when they fail
		int tpass = elemListParser(&x4, errs, &tps);
		int ppass = elemListParser(&x2, errs, &prs);
		int rpass = elemListParser(&x0, errs, &rts);
		if(tpass && ppass && rpass){
			ASTFnTy ft   = (ASTFnTy  ){.pos=x0.pos, tps, prs, rts};
			ta.tatm.tatm = (ASTTyAtom){.pos=x0.pos, .fty=ft, .kind=TA_FNTY};
			ta.kind      = AL_TATM;
			astStackPush(stk, &ta);
			return 1;
		}else{
			if(!tpass) appendList(&errs->errs, &(Error){ERR_P_BAD_FTTP, x4.pos});
			if(!ppass) appendList(&errs->errs, &(Error){ERR_P_BAD_FTPS, x2.pos});
			if(!rpass) appendList(&errs->errs, &(Error){ERR_P_BAD_FTRS, x0.pos});
			return 0;
		}
	}
	
	
	// |> [] -> []
	if( astStackPeek(stk, 3, &x3) &&  (x3.kind == AL_TKN ) && (x3.tk.tk.type == TKN_FNTY    ) &&
	    astStackPeek(stk, 2, &x2) && ((x2.kind == AL_BRK ) || (x2.kind       == AL_AGEN    )) &&
	    astStackPeek(stk, 1, &x1) &&  (x1.kind == AL_TKN ) && (x1.tk.tk.type == TKN_R_ARROW ) &&
	    astStackPeek(stk, 0, &x0) && ((x0.kind == AL_BRK ) || (x0.kind       == AL_AGEN     ))){
	    // FnTy
		stk->head   -= 4;   
		ASTList ta   = x0;
		List tps     = (List){0, 0, 0, 0}, prs, rts;
		// Parse pars and rets, error when they fail
		int ppass = elemListParser(&x2, errs, &prs);
		int rpass = elemListParser(&x0, errs, &rts);
		if(ppass && rpass){
			ASTFnTy ft   = (ASTFnTy  ){.pos=x0.pos, tps, prs, rts};
			ta.tatm.tatm = (ASTTyAtom){.pos=x0.pos, .fty=ft, .kind=TA_FNTY};
			ta.kind      = AL_TATM;
			astStackPush(stk, &ta);
			return 1;
		}else{
			if(!ppass) appendList(&errs->errs, &(Error){ERR_P_BAD_FTPS, x2.pos});
			if(!rpass) appendList(&errs->errs, &(Error){ERR_P_BAD_FTRS, x0.pos});
			return 0;
		}
	}
	
	// [BILD]
	if( astStackPeek(stk, 0, &x0) && (x0.kind == AL_BRK) ){
		ASTBuild bld;
		int errct = errs->errs.size;
		if(subparseBuild(&x0, errs, &bld)){
			stk->head--;   
			ASTList bd   = x0;
			bd.tatm.tatm = (ASTTyAtom){.pos=x0.pos, .bld=bld, .kind=TA_BILD};
			bd.kind      = AL_TATM;
			astStackPush(stk, &bd);
			return 1;
		}
		errs->errs.size = errct;
	}
	
	// BITY
	if( astStackPeek(stk, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.tk.type == TKN_S_BID) &&
	   isTypeBID(x0.tk.tk.data.i64)){
		stk->head--;   
		ASTList ta   = x0;
		ta.tatm.tatm = (ASTTyAtom){.pos=x0.pos, .id=x0.tk.tk.data.i64, .kind=TA_BITY};
		ta.kind      = AL_TATM;
		astStackPush(stk, &ta);
		return 1;
	}
	
	// TYID
	if( astStackPeek(stk, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.tk.type == TKN_S_TYID)){
		stk->head--;   
		ASTList ta   = x0;
		ta.tatm.tatm = (ASTTyAtom){.pos=x0.pos, .id=x0.tk.tk.data.i64, .kind=TA_TYID};
		ta.kind      = AL_TATM;
		astStackPush(stk, &ta);
		return 1;
	}
	
	// TVAR
	if( astStackPeek(stk, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.tk.type == TKN_S_TVAR)){
		stk->head--;   
		ASTList ta   = x0;
		ta.tatm.tatm = (ASTTyAtom){.pos=x0.pos, .id=x0.tk.tk.data.i64, .kind=TA_TVAR};
		ta.kind      = AL_TATM;
		astStackPush(stk, &ta);
		return 1;
	}
	
	// LOCTY
	if( astStackPeek(stk, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.tk.type == TKN_LOCTY)){
		stk->head--;   
		ASTList ta   = x0;
		ta.tatm.tatm = (ASTTyAtom){.pos=x0.pos, .loc=x0.tk.tk.data.loc, .kind=TA_TLOC};
		ta.kind      = AL_TATM;
		astStackPush(stk, &ta);
		return 1;
	}
	
	
	return 0;
}


int typeRule(ASTStack* stk, ASTStack* tks, int elemOnly, ErrorList* errs){
	
	#ifdef PARSER_DEBUG
		printf("TY %i %i | ", tks->head, stk->head);
		printASTStack(*stk);
	#endif
	ASTList x0, x1, x2, x3;
	
	// AGEN TATM
	if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TATM) &&
	   astStackPeek(stk, 1, &x1) && (x1.kind == AL_AGEN)){
		ASTList lm   = x0;
		lm.tylm.lm   = (ASTTyElem){.pos=x0.pos, .sizes=makeList(2, sizeof(int64_t)), .atom=x0.tatm.tatm};
		int64_t x    = 0;
		appendList(&lm.tylm.lm.sizes, &x);
		lm.kind      = AL_TYLM;
		stk->head   -= 2;
		astStackPush(stk, &lm);
		return 1;
	}
	
	// ASIZ TATM
	if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TATM) &&
	   astStackPeek(stk, 1, &x1) && (x1.kind == AL_ASIZ)){
		ASTList lm   = x0;
		lm.tylm.lm   = (ASTTyElem){.pos=x0.pos, .sizes=makeList(2, sizeof(int64_t)), .atom=x0.tatm.tatm};
		appendList(&lm.tylm.lm.sizes, &x1.tk.tk.data.i64);
		lm.kind      = AL_TYLM;
		stk->head   -= 2;
		astStackPush(stk, &lm);
		return 1;
	}
	
	// ^ TYLM
	if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TATM) &&
	   astStackPeek(stk, 1, &x1) && (x1.kind == AL_TKN ) && (x1.tk.tk.type == TKN_EXP)){
		// Ptr
		ASTList lm   = x0;
		lm.tylm.lm   = (ASTTyElem){.pos=x0.pos, .sizes=makeList(2, sizeof(int64_t)), .atom=x0.tatm.tatm};
		int64_t x    = -1;
		appendList(&lm.tylm.lm.sizes, &x);
		lm.kind      = AL_TYLM;
		stk->head   -= 2;
		astStackPush(stk, &lm);
		return 1;
	}
	
	// AGEN	TYLM
	if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TYLM) &&
	   astStackPeek(stk, 1, &x1) && (x1.kind == AL_AGEN)){
		ASTList lm   = x0;
		int64_t x    = 0;
		appendList(&lm.tylm.lm.sizes, &x);
		lm.kind      = AL_TYLM;
		stk->head   -= 2;
		astStackPush(stk, &lm);
		return 1;
	}
	
	// ASIZ	TYLM
	if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TYLM) &&
	   astStackPeek(stk, 1, &x1) && (x1.kind == AL_ASIZ)){
		ASTList lm   = x0;
		appendList(&lm.tylm.lm.sizes, &x1.tk.tk.data.i64);
		lm.kind      = AL_TYLM;
		stk->head   -= 2;
		astStackPush(stk, &lm);
		return 1;
	}
	
	// ^ TYLM
	if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TYLM) &&
	   astStackPeek(stk, 1, &x1) && (x1.kind == AL_TKN ) && (x1.tk.tk.type == TKN_EXP)){
		// Ptr
		ASTList lm   = x0;
		int64_t x	 = -1;
		appendList(&lm.tylm.lm.sizes, &x);
		lm.kind      = AL_TYLM;
		stk->head   -= 2;
		astStackPush(stk, &lm);
		return 1;
	}
	
	if(!elemOnly){
		// TATM
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TATM)){
			stk->head--;
			ASTList ty  = x0;
			ty.type.ty  = (ASTType){.pos=x0.pos, .atom=x0.tatm.tatm, .kind=TK_ATOM};
			ty.kind     = AL_TYPE;
			astStackPush(stk, &ty);
			return 1;
		}
	
		// TYLM
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TYLM)){
			stk->head--;
			ASTList ty  = x0;
			ty.type.ty  = (ASTType){.pos=x0.pos, .elem=x0.tylm.lm, .kind=TK_ELEM};
			ty.kind     = AL_TYPE;
			astStackPush(stk, &ty);
			return 1;
		}
	
		ASTStruct strc;
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_BRK ) && subparseStrc(x0.wrap.here, &strc, errs)){
			// Struct
			ASTList ty  = x0;
			ty.type.ty  = (ASTType){.pos=x0.pos, .strc=strc, .kind=TK_STRC};
			ty.kind     = AL_TYPE;
			stk->head--;
			astStackPush(stk, &ty);
			return 1;
		}
	
		ASTEnum   enmt;
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_PAR ) && subparseEnum(x0.wrap.here, &enmt, errs)){
			// Enum
			ASTList ty  = x0;
			ty.type.ty  = (ASTType){.pos=x0.pos, .enmt=enmt, .kind=TK_ENUM};
			ty.kind     = AL_TYPE;
			stk->head--;
			astStackPush(stk, &ty);
			return 1;
		}
	
		ASTUnion  unon;
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_PAR ) && subparseUnon(x0.wrap.here, &unon, errs)){
			// Union
			ASTList ty  = x0;
			ty.type.ty  = (ASTType){.pos=x0.pos, .unon=unon, .kind=TK_UNON};
			ty.kind     = AL_TYPE;
			stk->head--;
			astStackPush(stk, &ty);
			return 1;
		}
	}else{
		// TATM
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_TATM)){
			stk->head--;
			ASTList lm  = x0;
			lm.tylm.lm  = (ASTTyElem){.pos=x0.pos, .sizes=(List){0, 0, 0, 0}, .atom=x0.tatm.tatm};
			lm.kind     = AL_TYLM;
			astStackPush(stk, &lm);
			return 1;
		}
	}
	
	if(typeAtomRule(stk, tks, errs)) return 1;
	
	return 0;
}


// [ id : TyLM, ... ]
// [      TyLm, ... ]
// [ id,        ... ]
int parsParser(ASTList* brk, int requireLabels, int requireTypes, ErrorList* errs, ASTPars* ret){
	if(brk->kind == AL_AGEN){
		*ret = (ASTPars){.pos=brk->pos, .pars=(List){0, 0, 0, 0}, .lbls=(List){0, 0, 0, 0}};
		return 1;
	}

	*ret = makeASTPars(4);

	ASTList* lst = brk->wrap.here;
	ASTLine  ln  = toLine(lst);
	ASTStack tks = lineToStack(&ln);
	ASTStack stk = makeEmptyStack(ln.size);
	
	int pass = 1;
	if(0){
		end:
		free(tks.stk);
		free(stk.stk);
		free(ln .lst);
		return pass;
	}
	
	int cont = 1;
	while(cont){
		#ifdef PARSER_DEBUG
			printf("PS %i %i | ", tks.head, stk.head);
			printASTStack(stk);
		#endif
		ASTList x0, x1, x2, x3, x4;
		
		if(separatorRules(&stk, &tks         )) continue;
		if(commentRule   (&stk, &tks         )) continue;
		if(typeRule      (&stk, &tks, 1, errs)) continue;
		
		// SEPR
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_SEPR)){ stk.head--; continue; }
		
		// id ,
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TKN ) &&  (x0.tk.tk.type == TKN_COMMA ) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TKN ) && ((x1.tk.tk.type == TKN_S_ID  ) || (x1.tk.tk.type == TKN_S_MID))){
		    stk.head -= 2;
			if(requireTypes){
				appendList(&errs->errs, &(Error    ){ERR_P_EXP_TYPE, x0.pos});
			}else{
				appendList(&ret ->pars, &(ASTTyElem){.pos=x1.pos, .sizes=(List){0, 0, 0, 0}, .atom=(ASTTyAtom){.pos=x1.pos, .kind=TA_VOID}});
				appendList(&ret ->lbls, &x1.tk.tk.data.i64);
			}
			continue;
		}
		
		// id EOF
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.tk.type == TKN_S_ID) && (tks.head == 0)){
		    stk.head--;
			if(requireTypes){
				appendList(&errs->errs, &(Error    ){ERR_P_EXP_TYPE, x0.pos});
			}else{
				appendList(&ret ->pars, &(ASTTyElem){.pos=x0.pos, .sizes=(List){0, 0, 0, 0}, .atom=(ASTTyAtom){.pos=x0.pos, .kind=TA_VOID}});
				appendList(&ret ->lbls, &x0.tk.tk.data.i64);
			}
			continue;
		}
		
		// id : TYLM ,
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TKN ) &&  (x0.tk.tk.type == TKN_COMMA) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TYLM) &&
		   astStackPeek(&stk, 2, &x2) && (x2.kind == AL_TKN ) &&  (x2.tk.tk.type == TKN_COLON) &&
		   astStackPeek(&stk, 3, &x3) && (x3.kind == AL_TKN ) && ((x3.tk.tk.type == TKN_S_ID  ) || (x3.tk.tk.type == TKN_S_MID))){
			stk.head -= 4;
			appendList(&ret ->pars, &x1.tylm.lm);
			appendList(&ret ->lbls, &x3.tk.tk.data.i64);
			continue;
		}
		
		// id : TYLM EOF
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TYLM) &&  (tks.head      ==         0) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TKN ) &&  (x1.tk.tk.type == TKN_COLON) &&
		   astStackPeek(&stk, 2, &x2) && (x2.kind == AL_TKN ) && ((x2.tk.tk.type == TKN_S_ID  ) || (x2.tk.tk.type == TKN_S_MID))){
			stk.head -= 3;
			appendList(&ret ->pars, &x0.tylm.lm);
			appendList(&ret ->lbls, &x2.tk.tk.data.i64);
			continue;
		}
		
		// TYLM ,
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.tk.type == TKN_COMMA) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TYLM)){
		    stk.head -= 2;
			if(requireLabels){
				appendList(&errs->errs, &(Error){ERR_P_EXP_LABL, x0.pos});
			}else{
				int64_t x = 0;
				appendList(&ret ->pars, &x1.tylm.lm);
				appendList(&ret ->lbls, &x);
			}
			continue;
		}
		
		// TYLM EOF
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TYLM) && (tks.head == 0)){
		    stk.head--;
			if(requireLabels){
				appendList(&errs->errs, &(Error){ERR_P_EXP_LABL, x0.pos});
			}else{
				int64_t x = 0;
				appendList(&ret ->pars, &x0.tylm.lm);
				appendList(&ret ->lbls, &x);
			}
			continue;
		}
		
		ASTList tk;
		if      ((tks.head == 0) && (stk.head == 0)){
			pass = 1; cont = 0;
		}else if((tks.head == 0) && (stk.head != 0)){
			pass = 0; cont = 0;
		}else if(astStackPop(&tks, &tk)){
			if(!astStackPush(&stk, &tk)){ printf("AST Stack overflow.\n"); exit(-1); }
		}
	}
	goto end;
}


int tprsParser(ASTList* brk, ErrorList* errs, ASTPars* ret){
	if(brk->kind == AL_AGEN){
		*ret = (ASTPars){.pos=brk->pos, .pars=(List){0, 0, 0, 0}, .lbls=(List){0, 0, 0, 0}};
		return 1;
	}

	*ret = makeASTPars(4);

	ASTList* lst = brk->wrap.here;
	ASTLine  ln  = toLine(lst);
	ASTStack tks = lineToStack(&ln);
	ASTStack stk = makeEmptyStack(ln.size);
	
	int pass = 1;
	if(0){
		end:
		free(tks.stk);
		free(stk.stk);
		free(ln .lst);
		return pass;
	}
	
	int cont = 1;
	while(cont){
		#ifdef PARSER_DEBUG
			printf("TP %i %i | ", tks.head, stk.head);
			printASTStack(stk);
		#endif
		ASTList x0, x1, x2, x3, x4;
		
		if(separatorRules(&stk, &tks         )) continue;
		if(commentRule   (&stk, &tks         )) continue;
		if(typeRule      (&stk, &tks, 1, errs)) continue;
		
		// SEPR
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_SEPR)){ stk.head--; continue; }
		
		// TYLM(tvar) : TYLM ,
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.tk.type == TKN_COMMA) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TYLM) &&
		   astStackPeek(&stk, 2, &x2) && (x2.kind == AL_TKN ) && (x2.tk.tk.type == TKN_COLON) &&
		   astStackPeek(&stk, 3, &x3) && (x3.kind == AL_TYLM) && (x3.tylm.lm.sizes.size == 0) && (x3.tylm.lm.atom.kind == TA_TVAR)){
			stk.head -= 4;
			appendList(&ret ->pars, &x1.tylm.lm);
			appendList(&ret ->lbls, &x3.tylm.lm.atom.id);
			continue;
		}
		
		// TYLM(tvar) : TYLM EOF
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TYLM) && (tks.head      ==         0) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TKN ) && (x1.tk.tk.type == TKN_COLON) &&
		   astStackPeek(&stk, 2, &x2) && (x2.kind == AL_TYLM) && (x2.tylm.lm.sizes.size == 0) && (x2.tylm.lm.atom.kind == TA_TVAR)){
			stk.head -= 3;
			appendList(&ret ->pars, &x0.tylm.lm);
			appendList(&ret ->lbls, &x2.tylm.lm.atom.id);
			continue;
		}
		
		
		// TYLM(tvar) ,
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.tk.type == TKN_COMMA) &&
		   astStackPeek(&stk, 1, &x1) && (x1.kind == AL_TYLM) && (x1.tylm.lm.sizes.size == 0) && (x1.tylm.lm.atom.kind == TA_TVAR)){
			stk.head -= 2;
			int64_t x = 0;
			appendList(&ret ->pars, &x1.tylm.lm);
			appendList(&ret ->lbls, &x);
			continue;
		}
		
		// TYLM(tvar) EOF
		if(astStackPeek(&stk, 0, &x0) && (x0.kind == AL_TYLM) && (x0.tylm.lm.sizes.size == 0) && (x0.tylm.lm.atom.kind == TA_TVAR) && (tks.head == 0)){
			stk.head--;
			int64_t x = 0;
			appendList(&ret ->pars, &x0.tylm.lm);
			appendList(&ret ->lbls, &x);
			continue;
		}
		
		ASTList tk;
		if      ((tks.head == 0) && (stk.head == 0)){
			pass = 1; cont = 0;
		}else if((tks.head == 0) && (stk.head != 0)){
			pass = 0; cont = 0;
		}else if(astStackPop(&tks, &tk)){
			if(!astStackPush(&stk, &tk)){ printf("AST Stack overflow.\n"); exit(-1); }
		}
	}
	goto end;
}


/*==========================================

		HEADER PARSER

==========================================*/
int headerParser(ASTStack* stk, ASTStack* tks, ErrorList* errs, ASTProgram* ret){
	int cont = 1;
	while(cont){
		ASTList x0, x1, x2, x3, x4, x5, x6, x7, x8;
		#ifdef PARSER_DEBUG
			printf("HD %i %i | ", tks->head, stk->head);
			printASTStack(*stk);
		#endif
		
		// Separators and constraints
		if(separatorRules(stk, tks)) continue;
		if(constraintRule(stk, tks)) continue;
		
		
		// FNID		::		[]		=>		[]		->		[]		{}		SEPR
		if(astStackPeek(stk, 8, &x8) &&  (x8.kind == AL_TKN ) && (x8.tk.tk.type == TKN_S_ID    ) &&
		   astStackPeek(stk, 7, &x7) &&  (x7.kind == AL_TKN ) && (x7.tk.tk.type == TKN_DEFINE  ) &&
		   astStackPeek(stk, 6, &x6) && ((x6.kind == AL_BRK ) || (x6.kind == AL_AGEN)) &&
		   astStackPeek(stk, 5, &x5) &&  (x5.kind == AL_TKN ) && (x5.tk.tk.type == TKN_R_DARROW) &&
		   astStackPeek(stk, 4, &x4) && ((x4.kind == AL_BRK ) || (x4.kind == AL_AGEN)) &&
		   astStackPeek(stk, 3, &x3) &&  (x3.kind == AL_TKN ) && (x3.tk.tk.type == TKN_R_ARROW ) &&
		   astStackPeek(stk, 2, &x2) && ((x2.kind == AL_BRK ) || (x2.kind == AL_AGEN)) &&
		   astStackPeek(stk, 1, &x1) &&  (x1.kind == AL_BRC ) &&
		   astStackPeek(stk, 0, &x0) &&  (x0.kind == AL_SEPR)){
			ASTList fn   = x0;
			fn.fn.fn	 = (ASTFnDef){.pos=x8.pos, .fnid=x8.tk.tk.data.i64};
			// Check tprs, pars, rets, and blk (TODO: add blk)
			int tvsPass = tprsParser(&x6,       errs, &fn.fn.fn.tvrs);
			int prsPass = parsParser(&x4, 1, 1, errs, &fn.fn.fn.pars);
			int rtsPass = parsParser(&x2, 0, 1, errs, &fn.fn.fn.rets);
			if(tvsPass && prsPass && rtsPass){
				fn.kind      = AL_FNDF;
				stk->head   -= 9;
				astStackPush(stk, &fn);
				continue;
			}else{
				if(!tvsPass) appendList(&errs->errs, &(Error){ERR_P_BAD_TPRS, x6.pos});
				if(!prsPass) appendList(&errs->errs, &(Error){ERR_P_BAD_PARS, x4.pos});
				if(!rtsPass) appendList(&errs->errs, &(Error){ERR_P_BAD_RETS, x2.pos});
				stk->head -= 9;
				continue;
			}
		}
		
		// FNID		::		[]		->		[]		{}		SEPR
		if(astStackPeek(stk, 6, &x6) &&  (x6.kind == AL_TKN ) && (x6.tk.tk.type == TKN_S_ID    ) &&
		   astStackPeek(stk, 5, &x5) &&  (x5.kind == AL_TKN ) && (x5.tk.tk.type == TKN_DEFINE  ) &&
		   astStackPeek(stk, 4, &x4) && ((x4.kind == AL_BRK ) || (x4.kind == AL_AGEN)) &&
		   astStackPeek(stk, 3, &x3) &&  (x3.kind == AL_TKN ) && (x3.tk.tk.type == TKN_R_ARROW ) &&
		   astStackPeek(stk, 2, &x2) && ((x2.kind == AL_BRK ) || (x2.kind == AL_AGEN)) &&
		   astStackPeek(stk, 1, &x1) &&  (x1.kind == AL_BRC ) &&
		   astStackPeek(stk, 0, &x0) &&  (x0.kind == AL_SEPR)){
		    ASTList fn   = x0;
			fn.fn.fn	 = (ASTFnDef){.pos=x6.pos, .fnid=x6.tk.tk.data.i64,
			                          .tvrs=(ASTPars){.pos=x6.pos, .pars=(List){0, 0, 0, 0}, .lbls=(List){0, 0, 0, 0}}};
			// Check pars, rets, and blk (TODO: add blk)
			int prsPass = parsParser(&x4, 1, 1, errs, &fn.fn.fn.pars);
			int rtsPass = parsParser(&x2, 0, 1, errs, &fn.fn.fn.rets);
			if(prsPass && rtsPass){
				fn.kind      = AL_FNDF;
				stk->head   -= 7;
				astStackPush(stk, &fn);
				continue;
			}else{
				if(!prsPass) appendList(&errs->errs, &(Error){ERR_P_BAD_PARS, x4.pos});
				if(!rtsPass) appendList(&errs->errs, &(Error){ERR_P_BAD_RETS, x2.pos});
				stk->head -= 7;
				continue;
			}
		}
		
		// TYID     ::
		if(astStackPeek(stk, 1, &x1) && (x1.kind == AL_TKN ) && (x1.tk.tk.type == TKN_S_TYID  ) &&
		   astStackPeek(stk, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.tk.type == TKN_DEFINE  )){
			ASTList hd   = x1;
			hd.tyhd		 = (AS_TYHD){x1.tk.tk.data.i64, .tprs=(ASTPars){.pos=x1.pos, .pars=(List){0, 0, 0, 0}, .lbls=(List){0, 0, 0, 0}}};
			hd.kind      = AL_TYHD;
			stk->head   -= 2;
			astStackPush(stk, &hd);
			continue;
		}
		
		// TYHD		[]		=>
		if(astStackPeek(stk, 2, &x2) && (x2.kind == AL_TYHD) &&
		   astStackPeek(stk, 1, &x1) && (x1.kind == AL_BRK ) &&
		   astStackPeek(stk, 0, &x0) && (x0.kind == AL_TKN ) && (x0.tk.tk.type == TKN_R_DARROW)){		
			stk->head -= 3;
			ASTList hd   = x2;
			int pos = eqPos     ( x2.pos,    x2.tyhd.tprs.pos);
			int tps = tprsParser(&x1, errs, &hd.tyhd.tprs);
			if(pos && tps){
				hd.kind          = AL_TYHD;
				hd.tyhd.tprs.pos = x1.pos;
				astStackPush(stk, &hd);
			}else{
				if(!pos) appendList(&errs->errs, &(Error){ERR_P_MLT_TPRS, x1.pos});
				if(!tps) appendList(&errs->errs, &(Error){ERR_P_BAD_TPRS, x1.pos});
			}
			continue;
		}
		
		// TYHD		TYPE	SEPR
		if(astStackPeek(stk, 2, &x2) && (x2.kind == AL_TYHD) &&
		   astStackPeek(stk, 1, &x1) && (x1.kind == AL_TYPE) &&
		   astStackPeek(stk, 0, &x0) && (x0.kind == AL_SEPR)){
			ASTList td   = x0;
			td.kind      = AL_TYDF;
			td.ty.ty     = (ASTTyDef){.pos=x0.pos, .tyid=x2.tyhd.tyid, .tprs=x2.tyhd.tprs, .tdef=x1.type.ty};
			stk->head   -= 3;
			astStackPush(stk, &td);
			continue;
		}
		
		// TYHD		[]		SEPR
		if(astStackPeek(stk, 2, &x2) && (x2.kind == AL_TYHD) &&
		   astStackPeek(stk, 1, &x1) && (x1.kind == AL_BRK ) &&
		   astStackPeek(stk, 0, &x0) && (x0.kind == AL_SEPR)){
			appendList(&errs->errs, &(Error){ERR_P_BAD_STRC, x0.pos});
			stk->head -= 3;
			continue; 
		}
		
		// TYHD		()		SEPR
		if(astStackPeek(stk, 2, &x2) && (x2.kind == AL_TYHD) &&
		   astStackPeek(stk, 1, &x1) && (x1.kind == AL_PAR ) &&
		   astStackPeek(stk, 0, &x0) && (x0.kind == AL_SEPR)){
			appendList(&errs->errs, &(Error){ERR_P_BAD_TYPR, x0.pos});
			stk->head -= 3;
			continue; 
		}
		
		// TYHD		{}		SEPR
		if(astStackPeek(stk, 2, &x2) && (x2.kind == AL_TYHD) &&
		   astStackPeek(stk, 1, &x1) && (x1.kind == AL_BRC ) &&
		   astStackPeek(stk, 0, &x0) && (x0.kind == AL_SEPR)){
			appendList(&errs->errs, &(Error){ERR_P_UNX_BLOK, x0.pos});
			stk->head -= 3;
			continue; 
		}
		
		// Header/Definition Combinators, plus newline and comment removal
		if(astStackPeek(stk, 1, &x1) && (x1.kind == AL_PROG)){
			if(astStackPeek(stk, 0, &x0)){
				if((x0.kind == AL_TKN ) &&
				  ((x0.tk.tk.type == TKN_COMMENT) || (x0.tk.tk.type == TKN_COMMS))){stk->head--; continue;}
				
				if(x0.kind == AL_SEPR){ stk->head--; continue; }
			
				// HEADER
				if(x0.kind == AL_HEAD){	appendList(&x1.prog.prog.hds, &x0.hd.hd    ); stk->head -= 2; astStackPush(stk, &x1); continue; }
			
				// FNDEF
				if(x0.kind == AL_FNDF){ appendList(&x1.prog.prog.fns, &x0.fn.fn    ); stk->head -= 2; astStackPush(stk, &x1); continue; }
			
				// TYDEF
				if(x0.kind == AL_TYDF){ appendList(&x1.prog.prog.tys, &x0.ty.ty    ); stk->head -= 2; astStackPush(stk, &x1); continue; }
			
				// CNST
				if(x0.kind == AL_CNST){ appendList(&x1.prog.prog.cns, &x0.cnst.cnst); stk->head -= 2; astStackPush(stk, &x1); continue; }
			
			}
		}else{
			if(astStackPeek(stk, 0, &x0)){
				// Comment - if nothing exists in the file before this point, a comment here is okay. Otherwise, error
				if((x0.kind == AL_TKN ) && ((x0.tk.tk.type == TKN_COMMENT) || (x0.tk.tk.type == TKN_COMMS))){
					if(stk->head > 1) appendList(&errs->errs, &(Error){ERR_P_UNX_COMM, x0.pos});
					stk->head--;
				}
			
				// Newline - if nothing exists in the file before this point, a newline here is okay. Otherwise, proceed with caution; valididty depends on rules downstream.
				if(x0.kind == AL_SEPR){
					if(stk->head > 1) appendList(&errs->errs, &(Error){ERR_P_UNX_LINE, x0.pos});
					stk->head--;
					continue;
				}
			
				// HEADER - Make PROG
				if(x0.kind == AL_HEAD){
					ASTList pg   = x0;
					pg.prog.prog = makeASTProgram(tks->size / 4);
					pg.kind      = AL_PROG;
					appendList(&pg.prog.prog.hds, &x0.hd.hd);
					stk->head--;
					astStackPush(stk, &pg);
					continue;
				}
			
				// FNDEF  - error : Expected Header before fndef
				if(x0.kind == AL_FNDF){ appendList(&errs->errs, &(Error){ERR_P_UNX_FNDF, x0.pos}); stk->head--; continue; }
			
				// TYDEF  - error : Expected Header before typedef
				if(x0.kind == AL_TYDF){ appendList(&errs->errs, &(Error){ERR_P_UNX_TYDF, x0.pos}); stk->head--; continue; }
			
				// CNST	  - error : Expected Header before constraint
				if(x0.kind == AL_CNST){ appendList(&errs->errs, &(Error){ERR_P_UNX_CNST, x0.pos}); stk->head--; continue; }
			}
		}
		
		if(astStackPeek(stk, 0, &x0) && (x0.kind == AL_BRK)){
			ASTList* here = x0.wrap.here;
		 	if(here != NULL){
		 		ASTLine ln = toLine(here);
		 		if((ln.size == 3) &&
		 		   (ln.lst[0].kind == AL_TKN) &&  (ln.lst[0].tk.tk.type == TKN_S_BID) &&
		 		   (ln.lst[1].kind == AL_TKN) &&  (ln.lst[1].tk.tk.type == TKN_COLON) &&
		 		   (ln.lst[2].kind == AL_TKN) && ((ln.lst[2].tk.tk.type == TKN_S_ID ) || (ln.lst[2].tk.tk.type == TKN_S_TYID) || (ln.lst[2].tk.tk.type == TKN_S_MID))){
		 			ASTHeader head;
		 			head.pos = x1.pos;
		 			head.bid = ln.lst[0].tk.tk.data.i64;
		 			head.sym = ln.lst[2].tk.tk.data.i64;
		 			stk->head--;
		 	
		 			ASTList hd;
					hd.pos   = head.pos;
					hd.hd.hd = head;
					hd.kind  = AL_HEAD;
					astStackPush(stk, &hd);
		 			continue;
		 		}
		 	}
		}
		
		// Type Parser
		if(astStackPeek(tks, 0, &x0) && (x0.kind == AL_TKN) && (x0.tk.tk.type != TKN_R_ARROW) && (x0.tk.tk.type != TKN_R_DARROW)){
			int tymode = 0;
			for(int i = 0; i < stk->size; i++){
				if(astStackPeek(stk, i, &x0) && (x0.kind == AL_TYHD)){
					tymode = 1;
					break;
				}
			}
			if(tymode && typeRule(stk, tks, 0, errs)) continue;
		}
	
		// No rules applied. Let's grab another token
		ASTList xval;
		int step = parseStep(tks, stk, 1, AL_PROG, &xval);
		if(!step){
			*ret = xval.prog.prog;
			cont = 0;
		}else if(step < 0){
			return 0;
		}
	}
	return !errs->errs.size;
}




int parseCode(LexerState* tks, SymbolTable* tab, ASTProgram* prog, ErrorList* errs){
	//printf("\n=================\n");
	ASTList* lst;
	if(!unwrap(tks, errs, &lst)){
		printf("Unwrap failed\n");
		return -1;
	}
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
	free(ast.stk);
	free(stk.stk);

	return -1;
}




