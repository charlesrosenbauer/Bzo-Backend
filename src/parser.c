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
			case AL_PAR  : printf("() "); break;
			case AL_BRC  : printf("{} "); break;
			case AL_BRK  : printf("[] "); break;
			case AL_TKN  : printf("TK "); break;
			
			case AL_TYDF : printf("TD "); break;
			case AL_TYPE : printf("TY "); break;
			case AL_TYLM : printf("LM "); break;
			case AL_STRC : printf("ST "); break;
			case AL_STLN : printf("S_ "); break;
			case AL_FNTY : printf("FT "); break;
			case AL_FPRS : printf("FP "); break;
			case AL_TPRS : printf("TP "); break;
			case AL_UNON : printf("UN "); break;
			case AL_ENUM : printf("EN "); break;
			case AL_UNLN : printf("U_ "); break;
			case AL_TGUN : printf("TU "); break;
			case AL_TULN : printf("T_ "); break;
			
			case AL_FNDF : printf("FD "); break;
			case AL_FTPS : printf("FS "); break;
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



/*
	Actual Parser Rules
*/

/*
	Type Parsing Rules
*/

// Recursion Headers
int parseType(ASTLine*, ErrorList*, ASTType*);

// TyElem	= ^		TyElem
//			| []	TyElem
//			| [Int]	TyElem
//			| TyId
//			| Id
int parseTyElem(ASTLine* line, ErrorList* errs, ASTTypeElem* ret){
	// Parse TyElem
	ASTLine ln = copyNoComms(line);
	if(ln.size < 1){ free(ln.lst); return 0; }
	ret->arrs  = malloc(sizeof(int) * ln.size);
	int retval = 0;
	ret->pos   = ln.lst[0].pos;
	for(int i  = 0; i < ln.size; i++){
		ASTList* sub = ln.lst[i].here;
		if      ((ln.lst[i].kind == AL_TKN) &&  (ln.lst[i].tk.type == TKN_EXP)){
			ret->arrs[i] = -1;
		}else if((ln.lst[i].kind == AL_BRK) &&  (sub == NULL)){
			ret->arrs[i] =  0;
		}else if((ln.lst[i].kind == AL_BRK) &&  (sub != NULL) && (sub->kind == AL_TKN) && (sub->tk.type == TKN_INT) && (sub->next == NULL)){
			ret->arrs[i] =  sub->tk.data.i64;
		}else if((ln.lst[i].kind == AL_TKN) && ((ln.lst[i].tk.type == TKN_S_ID) || (ln.lst[i].tk.type == TKN_S_TYID))){
			ret->arct = i;
			ret->tyid = ln.lst[i].tk.data.i64;
			retval    = i;
			break;
		}else{
			appendError(errs, (Error){ERR_P_BAD_TYPE, ln.lst[i].pos});
			break;
		}
	}
	
	free(ln.lst);
	return retval;
}

// FTPars	= [TyElem, ... ]
//			| ()
int parseFTPars(ASTLine* ln, ErrorList* errs, ASTFTPars* ret){
	if(ln->size < 1) return 0;
	if((ln->lst[0].kind == AL_PAR) && (ln->lst[0].here == NULL)){	// ()
		ret->pos  = ln->lst[0].pos;
		ret->pars = NULL;
		ret->prct = 0;
		return 1;
	}
	if((ln->lst[0].kind == AL_BRK) && (ln->lst[0].here != NULL)){
		// TODO: get sub, remove newlines, split on commas, parseTyElems
		return 1;
	}
	return 0;
}

// FnType	= FTPars -> FTPars
//			| FTPars -> TyElem
//			| FTPars => FTPars -> FTPars
//			| FTPars => FTPars -> TyElem
int parseFnType(ASTLine* ln, ErrorList* errs, ASTFuncType* ret){
	int  skip = 0;
	ASTLine l = copyLine(ln);
	printf("parseFnType   | ");
	printASTLine(l);
	
	if(0){
		fail:
		free(l.lst);
		printf("parseFnType   | fail\n");
		return 0;
	}
	
	if(0){
		pass:
		free(l.lst);
		printf("parseFnType   | pass\n");
		return skip;
	}

	if(ln->size >= 5){
		// Polymorphism
		ASTFTPars a, b, c;
		ASTLine line;
		viewAt(&l, &line, 0);
		int passA = parseFTPars(&line, errs, &a);
		if(!passA) goto fail;
		viewAt(&l, &line, 2);
		int passB = parseFTPars(&line, errs, &b);
		if(!passB) goto fail;
		viewAt(&l, &line, 4);
		int passC = parseFTPars(&line, errs, &c);
		if(!passC) goto fail;
		if((l.lst[1].kind != AL_TKN) || (l.lst[1].tk.type != TKN_R_DARROW) ||
		   (l.lst[3].kind != AL_TKN) || (l.lst[3].tk.type != TKN_R_ARROW)) goto fail;
		   
		// TODO: link a, b, and c to ret
		skip = 5;
		goto pass;
	}
	if(ln->size >= 3){
		// No polymorphism
		ASTFTPars a, b, c;
		ASTLine line;
		viewAt(&l, &line, 0);
		int passA = parseFTPars(&line, errs, &a);
		if(!passA) goto fail;
		viewAt(&l, &line, 2);
		int passB = parseFTPars(&line, errs, &b);
		if(!passB) goto fail;
		if((l.lst[1].kind != AL_TKN) || (l.lst[1].tk.type != TKN_R_ARROW)) goto fail;
		
		// TODO: link a and b to ret
		skip = 3;
		goto pass;
	}
	goto fail;
}

// Struct	= [ StLn ; ... ]
int parseStruct(ASTLine* ln, ErrorList* errs, ASTStruct* ret){
	return 0;
}

// Union	= ( StLn ; ... )
int parseUnion(ASTLine* ln, ErrorList* errs, ASTUnion* ret){
	return 0;
}

// StLn		= Id : Type

// Enum		= ( TId : EnLn ; ... )
int parseEnum(ASTLine* ln, ErrorList* errs, ASTEnum* ret){
	return 0;
}

// EnLn		= Id =  Int
//			| Id = -Int

// TagUnion = (Id TyId : UnLn ; ... )
int parseTagUnion(ASTLine* ln, ErrorList* errs, ASTTagUnion* ret){
	return 0;
}


// BId		= BId
int parseBuiltin (ASTLine* ln, ErrorList* errs, ASTBuiltin* ret){
	ASTLine l = *ln;
	printf("parseBuiltin  | ");
	printASTLine(l);
	
	if((ln->size > 0) && (ln->lst[0].kind == AL_TKN) && (ln->lst[0].tk.type == TKN_S_BID)){
		ret->bid = ln->lst[0].tk.data.i64;
		ret->pos = ln->lst[0].tk.pos;
		printf("parseBuiltin  | pass\n");
		return 1;
	}
	printf("parseBuiltin  | fail\n");
	return 0;
}

// UnLn		=  Int = TId : Type
//			| -Int = TId : Type

// TySet	= TId  = TId | ...


// Pars		= [ Id : TyElem , ... ]

// Type		= TyElem
//			| Struct
//			| Union
//			| TagUnion
//			| FnType
//			| Enum
//			| BId
int parseType(ASTLine* ln, ErrorList* errs, ASTType* ret){
	printf("parseType     | ");
	ASTLine line = *ln;
	printASTLine(line);
	
	int skip = 0;
	if(0){
		pass:
		printf("parseType     | pass\n");
		return skip;
	}
	
	ASTTypeElem    elem;
	skip = parseTyElem  (ln, errs, &elem);	// Parse TyElem
	if(skip){ ret->kind = TT_ELEM; ret->type.elem = elem; goto pass; }

	ASTStruct      strc;
	skip = parseStruct  (ln, errs, &strc);	// Parse Struct
	if(skip){ ret->kind = TT_STRC; ret->type.strc = strc; goto pass; }
	
	ASTUnion       unon;
	skip = parseUnion   (ln, errs, &unon);	// Parse Union
	if(skip){ ret->kind = TT_UNON; ret->type.unon = unon; goto pass; }
	
	ASTTagUnion    tgun;
	skip = parseTagUnion(ln, errs, &tgun);  // Parse Tagged Union
	if(skip){ ret->kind = TT_TGUN; ret->type.tgun = tgun; goto pass; }
	
	ASTFuncType    fnty;
	skip = parseFnType  (ln, errs, &fnty);  // Parse Function Type
	if(skip){ ret->kind = TT_FUNC; ret->type.func = fnty; goto pass; }
	
	ASTEnum        enmt;
	skip = parseEnum    (ln, errs, &enmt);  // Parse Enum
	if(skip){ ret->kind = TT_ENUM; ret->type.enmt = enmt; goto pass; }
	
	ASTBuiltin     bity;
	skip = parseBuiltin (ln, errs, &bity);  // Parse Builtin
	if(skip){ ret->kind = TT_BITY; ret->type.bity = bity; goto pass; }
	
	// If all else fails, fail completely
	printf("parseType     | fail\n");
	return 0;
}

// TPars	= [ Id : TyElem ,	...	]
int parseTPars(ASTLine* ln, ErrorList* errs, ASTTPars* ret){
	if(ln->lst[0].kind == AL_BRK){
		// Split on comma, parse Id : TyElem lines
		return 1;
	}
	return 0;
}


// TyDef	= TId :: Type
//			| TId :: TPars => Type
int parseTyDef(ASTLine* ln, ErrorList* errs, ASTTyDef* ret){
	printf("parseTyDef    | ");
	ASTLine line = *ln;
	printASTLine(line);
	
	int skip = 0;
	if(0){
		pass:
		printf("parseTyDef    | pass\n\n");
		return skip;
	}

	TkType pattern[] = {TKN_S_TYID, TKN_DEFINE};
	if((ln->size >= 3) && tokenMatch(ln, pattern, 2)){
		if((ln->size >= 5) && (ln->lst[3].kind == AL_TKN) && (ln->lst[3].tk.type == TKN_R_DARROW)){
			// Parse TPars and Type
		}
		ASTLine line;
		viewAt(ln, &line, 2);
		// Parse Type
		skip = parseType(&line, errs, &ret->type);
		if(skip) goto pass;
	}
	printf("parseTyDef    | fail\n\n");
	return 0;
}


/*
	Function Parsing Rules
*/
// FnDef 	= Id :: Pars -> FTPars Block
//			| Id :: Pars -> TyElem Block
//			| Id :: Pars => Pars -> FTPars Block
//			| Id :: Pars => Pars -> TyElem Block
int parseFnDef(ASTLine* ln, ErrorList* errs, ASTFnDef* ret){
	return 0;
}

// Block	= { Stmt ; ... Expr }
//			| { Expr }

// Stmt		= XPrs := Expr

// XPrs		= Id, ...

// Loc		= Id @ Id
//			| Id @ Loc

// Expr		= Id
//			| MId
//			| Loc
//			| Int
//			| Flt
//			| Str
//			| Tag
//			| FnCall
//			| TyCall
//			| Match
//			| MatchVal
//			| If
//			| IfVal
//			| Expr Binop Expr
//			| Id[Expr]
//			| MId[Expr]
//			| Unop Expr
//			| Expr Id
//			| Expr MId
//			| Expr Loc
//			| ( Expr )
//			| Lmda
//			| Wild
//			| Expr, Expr

// Lmda		= [Id, ... ]   Block
//			| [Id, ... ] ! Block

// Binop	= + - * / % ^ < > >= =< & | = !=

// Unop		= - ^ ! <-

// FnCall	= [ Id  : Expr, ... ]
//			= [ MId : Expr, ... ]
//			| [ Loc : Expr, ... ]

// TyCall	= [ TId : Expr, ... ]

// If		= [ ? : Expr ? Block ; ... ]

// IfVal	= [ ? : Expr ? := Expr ; ... ]

// Match	= [Expr ? : Expr ? Block ; ... ]

// MatchVal = [Expr ? : Expr ? := Expr ; ... ]



/*
int parseType(ASTLine*, ErrorList*);


int parseTypeElem(ASTLine* ln, ErrorList* errs){
	ASTTypeElem elem;
	if(ln->size < 1) return 0;
	elem.arrs = malloc(sizeof(int) * ln->size);

	int      pars = 0;
	for(int i = 0; i < ln->size; i++){
		if      ((ln->lst[i].kind == AL_TKN) && (ln->lst[i].tk.type == TKN_EXP)){		// ^
			elem.arrs[pars] = -1;
			pars++;
		}else if((ln->lst[i].kind == AL_BRK) && (ln->lst[i].here == NULL)){	// []
			elem.arrs[pars] =  0;
			pars++;
		}else if((ln->lst[i].kind == AL_BRK) && (ln->lst[i].here != NULL)){ // [INT]
			ASTList* here = ln->lst[i].here;
			if(here->next == NULL){
				if((here->tk.type == TKN_INT) && (here->next == NULL) && (here->tk.data.i64 > 0)){
					elem.arrs[pars] = here->tk.data.i64;
					pars++;
				}
			}
			appendError(errs, (Error){ERR_P_BAD_TYPE, here->pos});
			free(elem.arrs);
			return 0;
		}else if((ln->lst[i].kind == AL_TKN) && (ln->lst[i].tk.type == TKN_S_TYID)){
			// TyId. Eventually we need to handle [TyId: ...], tvar, and piped types as well.
			elem.arct = pars;
			elem.tyid = ln->lst[i].tk.data.i64;
			elem.pos  = fusePosition(ln->lst[0].pos, ln->lst[i].pos);
			// TODO: Need to add some more stuff here to make sure there isn't anything meaningful past here
			
			ASTTypeElem* lm  = malloc(sizeof(ASTTypeElem));
			*lm = elem;
			ln->lst[0].here  = lm;
			ln->lst[0].kind  = AL_TYLM;
			ln->size         = 1;
			return 1;
		}else{	// This shouldn't be here...
			appendError(errs, (Error){ERR_P_BAD_TYPE, ln->lst[i].pos});
			free(elem.arrs);
			return 0;
		}
	}
	return 0;
}



int parseStructLine(ASTLine* ln, ErrorList* errs){
	ASTLine a, b;
	if(!splitOnToken(ln, &a, &b, TKN_COLON)) return 0;
	int flid = 0;
	if((a.lst[0].kind == AL_TKN) && (a.lst[0].tk.type == TKN_S_TYID) && (a.size == 1))
		flid = a.lst[0].tk.data.i64;
	
	if(!parseType(&b, errs)) return 0;
	
	ASTStructLine* sl = malloc(sizeof(ASTStructLine));
	sl->pos  = fusePosition(a.lst[0].pos, b.lst[0].pos);
	sl->flid = flid;
	sl->type = *(ASTType*)b.lst[0].here;
	ln->lst[0].kind = AL_STLN;
	ln->lst[0].here = sl;
	ln->lst[0].pos  = sl->pos;
	return 1;
}


int parseStruct(ASTLine* ln, ErrorList* errs){
	if(ln->lst[0].kind == AL_BRK){
		// Split on newline and semicolon
		// Parse struct lines
	}
	return 0;
}


int parseUnion(ASTLine* ln, ErrorList* errs){
	if(ln->lst[0].kind == AL_PAR){
		// Find union header if one exists
		// Split on newline and semicolon
		// Parse union lines
	}
	return 0;
}

*/
/*
int parseType(ASTLine* ln, ErrorList* errs){
	ASTLine a, b;
	if(splitOnToken(ln, &a, &b, TKN_R_ARROW)){*/
		// Function Type
		/*
			parsePars needs to be working first
			a and b will either be a single type elem or a pars
			eventually we also will need to handle type parameters
		*/
		/*
		if(parseType(&a, errs) && parseType(&b, errs){
			ASTType* t      = malloc(sizeof(ASTType));
			t->type.unon    = *(ASTUnion*)ln->lst[0].here;
			t->kind         = TT_UNON;
			ln->lst[0].kind = AL_TYPE;
			ln->lst[0].here = t;
			return 1;
		}else{
			return 0;
		}*/
		/*
		return 0;
	}

	if(ln->lst[0].kind == AL_PAR){
		int pass = parseUnion(ln, errs);
		if(!pass) return 0;
		ASTType* t      = malloc(sizeof(ASTType));
		t->type.unon    = *(ASTUnion*)ln->lst[0].here;
		t->kind         = TT_UNON;
		ln->lst[0].kind = AL_TYPE;
		ln->lst[0].here = t;
		//ln->size = 1;
		return 1;
	}else if(ln->lst[0].kind == AL_BRK){
		int erct = errs->erct;
		if(!parseStruct(ln, errs)){
			int pass = parseTypeElem(ln, errs);
			if(!pass) return 0;
			errs->erct = erct;
			ASTType* t      = malloc(sizeof(ASTType));
			t->type.elem    = *(ASTTypeElem*)ln->lst[0].here;
			t->kind         = TT_ELEM;
			ln->lst[0].kind = AL_TYPE;
			ln->lst[0].here = t;
			//ln->size = 1;
			return 1;
		}
		ASTType* t      = malloc(sizeof(ASTType));
		t->type.strc    = *(ASTStruct*)ln->lst[0].here;
		t->kind         = TT_STRC;
		ln->lst[0].kind = AL_TYPE;
		ln->lst[0].here = t;
		//ln->size = 1;
		return 1;
	}else if((ln->lst[0].kind == AL_TKN) && (ln->lst[0].tk.type == TKN_S_BID)){
		ASTType* t      = malloc(sizeof(ASTType));
		t->type.bity    = (ASTBuiltin){ln->lst[0].tk.pos, ln->lst[0].tk.data.i64};
		t->kind         = TT_BITY;
		ln->lst[0].kind = AL_TYPE;
		ln->lst[0].here = t;
		//ln->size        = 1;
		return 1;
	}else{
		int pass = parseTypeElem(ln, errs);
		if(!pass) return 0;
		ASTType* t      = malloc(sizeof(ASTType));
		t->type.elem    = *(ASTTypeElem*)ln->lst[0].here;
		t->kind         = TT_ELEM;
		ln->lst[0].kind = AL_TYPE;
		ln->lst[0].here = t;
		//ln->size = 1;
		return 1;
	}
	return 0;
}


int parseTyDef(ASTLine* ln, ErrorList* errs){
	TkType pattern[] = {TKN_S_TYID, TKN_DEFINE};
	if((ln->size >= 3) && tokenMatch(ln, pattern, 2)){
		ASTLine type;
		viewAt(ln, &type, 2);
		int pass = parseType(&type, errs);
		if(pass){
			// TODO: Build a typedef
			printf("Typedef @ %i\n", ln->lst[0].pos.lineStart);
			ASTType t = *(ASTType*)type .lst[0].here;
			printASTType(t, 0); printf("\n");
			return 1;
		}else{
			printf("Umm... wat?\n");
		}
	}
	return 0;
}

int parseFnDef(ASTLine* ln, ErrorList* errs){
	TkType pattern[] = {TKN_S_ID, TKN_DEFINE};
	if((ln->size >= 3) && tokenMatch(ln, pattern, 2)){
		printf("Funcdef @ %i\n", ln->lst[0].pos.lineStart);
		return 1;
	}
	return 0;
}

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
		ASTLine x = a;
		cont = viewSplitOnToken(&x, &a, &b, TKN_NEWLINE);
		ASTTyDef tydf;
		if(parseTyDef(&a, errs, &tydf)){a = b; continue;}
		ASTFnDef fndf;
		if(parseFnDef(&a, errs, &fndf)){a = b; continue;}
		printf("WTF @ %i\n", a.lst[0].pos.lineStart);
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
