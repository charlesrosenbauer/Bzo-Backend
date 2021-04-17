#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "langparser.h"
#include "error.h"
#include "util.h"




/*
	Parser allocator
*/
AllocatorAST makeAlloc(int size){
	AllocatorAST ret;
	ret.size   = size;
	ret.fill   = 0;
	ret.buffer = malloc(sizeof(uint8_t) * size);
	ret.next   = NULL;
	return ret;
}

void* allocate(AllocatorAST* a, int size, int align){
	AllocatorAST*  here = a;
	AllocatorAST** last = NULL;
	while(here != NULL){
		int off =  here->fill & (align-1);
		int ix  =  here->fill;
		if(off){
			ix +=   align;
			ix &= ~(align-1);
		}
		if((ix + size) < here->size){
			here->fill = ix+size;
			return &here->buffer[ix];
		}else{
			last = (AllocatorAST**)&here->next;
			here =                  here->next;
		}
	}
	*last  = malloc(sizeof(AllocatorAST));
	**last = makeAlloc(16384);
	return allocate(*last, size, align);
}

void freeAllocator(AllocatorAST* a){
	if(a == NULL) return;
	free(a->buffer);
	freeAllocator(a->next);
}


typedef struct{
	void* here;
	int   name;
	void* next;
}TList;

void freeTList(TList* l){
	if(l == NULL) return;
	freeTList(l->next);
	if(l->here != NULL) free(l->here);
	free(l);
}




/*
	Parser
*/
typedef struct{
	LexerState tks;
	int        tix;
}ParserState;

typedef struct{
	int(* pptr)(ParserState*, void*, void*);
	void* pars;
	int   retsize;
}ParserFunc;



ASTProgram makeASTProgram(int defct){
	ASTProgram ret;
	ret.fns   = malloc(sizeof(ASTFnDef) * defct);
	ret.tys   = malloc(sizeof(ASTTyDef) * defct);	
	ret.fncap = defct;
	ret.tycap = defct;
	ret.fnct  = 0;
	ret.tyct  = 0;
	ret.alloc = makeAlloc(16384);
	return ret;
}

TkType peekToken(ParserState* ps){
	if((ps->tix < 0) || (ps->tix >= ps->tks.tkcap)) return TKN_VOID;
	return ps->tks.tks[ps->tix].type;
}

Token  eatToken(ParserState* ps){
	Token ret;
	ret.type = TKN_VOID;
	if((ps->tix < 0) || (ps->tix >= ps->tks.tkcap)) return ret;
	ps->tix++;
	return ps->tks.tks[ps->tix-1];
}


int skipBrak(LexerState* tks, int tix){
	int ix      = tix;
	int ct      = 0;
	int skipped = 0;
	while(ix < tks->tkct){
		if      (tks->tks[ix].type == TKN_BRK_OPN){
			ct++;
			skipped = 1;
		}else if(tks->tks[ix].type == TKN_BRK_END){
			ct--;
			skipped = 1;
		}
		if((ct == 0) && skipped) return ix;
		ix++;
	}
	return -1;
}

int skipPars(LexerState* tks, int tix){
	int ix      = tix;
	int ct      = 0;
	int skipped = 0;
	while(ix < tks->tkct){
		if      (tks->tks[ix].type == TKN_PAR_OPN){
			ct++;
			skipped = 1;
		}else if(tks->tks[ix].type == TKN_PAR_END){
			ct--;
			skipped = 1;
		}
		if((ct == 0) && skipped) return ix;
		ix++;
	}
	return -1;
}

int skipBrac(LexerState* tks, int tix){
	int ix      = tix;
	int ct      = 0;
	int skipped = 0;
	while(ix < tks->tkct){
		if      (tks->tks[ix].type == TKN_BRC_OPN){
			ct++;
			skipped = 1;
		}else if(tks->tks[ix].type == TKN_BRC_END){
			ct--;
			skipped = 1;
		}
		if((ct == 0) && skipped) return ix;
		ix++;
	}
	return -1;
}



/*
	Okay, going to try out something else for this parser.
	
	Focus on dividing the code first into trees based on pars/brcs/brks
*/
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


/*
	Type parsing stuff
*/

int parseTypeElem(TkList** lst, ASTTypeElem* ret){
	TkList* head = *lst;
	while(head != NULL){
		if(head->kind == TL_TKN){
		
		}else if(head->kind == TL_BRK){
			
		}
		head = head->next;
	}
}


int parseTypeDef(LexerState* tks, ASTProgram* prog, int tix){

	return 0;
}

int parseFuncDef(LexerState* tks, SymbolTable* tab, ASTProgram* prog, int tix){
	
	return 0;
}



int checkWrap(LexerState* tks, ErrorList* errs, TkList** list){
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


/*
	Ideally we should take the TkList tree and split wrap contents based on lines, semicolons, commas, etc.
*/
typedef struct{
	TkList** tks;
	int*     ixs;
	int      tkct, lnct;
}TkLines;

typedef struct{
	TkLines* ls;
	int      line, ix;
}TkLinePos;

TkList* tklIx(TkLines* ls, int l, int i){
	int end = (l >= ls->lnct)? ls->tkct : ls->ixs[l+1];
	if((l < 0) || (l >=     ls->lnct  )) return NULL;
	if((i < 0) || (i >= end-ls->ixs[l])) return NULL;
	return ls->tks[ls->ixs[l] + i];
}

TkList* tkpIx(TkLinePos* p){
	return tklIx(p->ls, p->line, p->ix);
}

int tkpNextLine(TkLinePos* p){
	p->line++;
	p->ix = 0;
	if(p->line >= p->ls->lnct) return 0;
	return 1;
}

int tkpNextIx(TkLinePos* p){
	p->ix++;
	int lix = p->ls->ixs[p->line];
	if(lix + p->ix >= p->ls->tkct){
		p->ix = 0;
		return 0;
	}
	int next = p->ls->ixs[p->line+1];
	if(lix + p->ix >= next){
		p->ix = 0;
		p->line++;
		return 0;
	}
	return 1;
}


void printTkLine(TkLines* ls, int ix, int end){
	for(int j = ix; j < end; j++){
		TkList* x = ls->tks[j];
		if(x == NULL){
			printf("NA ");
			return;
		}else{
			switch(x->kind){
				case TL_PAR : printf("() "); break;
				case TL_BRK : printf("[] "); break;
				case TL_BRC : printf("{} "); break;
				case TL_NIL : printf("__ "); break;
				case TL_TKN : {
					switch(x->tk.type){
						case TKN_NEWLINE : printf("NL "); break;
						case TKN_INT     : printf("I# "); break;
						case TKN_FLT     : printf("F# "); break;
						case TKN_STR     : printf("ST "); break;
						case TKN_TAG     : printf("TG "); break;
						case TKN_S_ID    : printf("ID "); break;
						case TKN_S_MID   : printf("MI "); break;
						case TKN_S_BID   : printf("BI "); break;
						case TKN_S_TYID  : printf("TI "); break;
						case TKN_COMMENT : printf("#: "); break;
						case TKN_COMMS   : printf("## "); break;
						default:           printf("TK "); break;
					}
				} break;
			}
		}
	}
}

void printTkLines(TkLines* ls){
	for(int i = 0; i < ls->lnct; i++){
		int ix  = ls->ixs[i];
		int end = (i+1 >= ls->lnct)? ls->tkct : ls->ixs[i+1];
		printTkLine(ls, ix, end);
		printf("\n");
	}
	printf("\n");
}


void printTkLinePos(TkLinePos* p){
	printf("L=%i/%i\n", p->line, p->ls->lnct);
	for(int i = 0; i < p->ls->lnct; i++){
		int ix  = p->ls->ixs[i];
		int end = (i+1 >= p->ls->lnct)? p->ls->tkct : p->ls->ixs[i+1];
		printTkLine(p->ls, ix, end);
		printf("\n");
		if(i == p->line){
			for(int j = 0; j < p->ix; j++) printf("   ");
			printf(" ^\n"); 
		}
	}
	printf("\n");
}


TkLines splitLines(TkList* lst){
	// split on \n and ;
	TkLines ret  = (TkLines){NULL, NULL, 0, 1};
	TkList* head = lst;
	while(head  != NULL){
		if((head->kind == TL_TKN) && ((head->tk.type == TKN_NEWLINE) || (head->tk.type == TKN_SEMICOLON)) || (head->next == NULL)) ret.lnct++;
		ret.tkct++;
		head = head->next;
	}
	if(ret.tkct < 1) return ret;
	ret.lnct++;
	
	head = lst;
	ret.tks = malloc(sizeof(TkList*) * ret.tkct);
	ret.ixs = malloc(sizeof(int)     * ret.lnct);
	int lineIx = 0;
	ret.ixs[0] = 0;
	for(int i = 0; i < ret.tkct; i++){
		ret.tks[i] = head;
		if(((head->kind == TL_TKN) && ((head->tk.type == TKN_NEWLINE) || (head->tk.type == TKN_SEMICOLON) || (head->tk.type == TKN_COMMENT))) || (i+1 >= ret.tkct)){
			if(lineIx+1 < ret.lnct) ret.ixs[lineIx+1] = i+1;
			lineIx++;
		}
		head = head->next;
	}
	return ret;
}

TkLines splitCommas(TkList* lst){
	// split on ,
	TkLines ret  = (TkLines){NULL, NULL, 0, 1};
	TkList* head = lst;
	while(head  != NULL){
		if((head->kind == TL_TKN) && (head->tk.type == TKN_COMMA)) ret.lnct++;
		ret.tkct++;
		head = head->next;
	}
	if(ret.tkct < 1) return ret;
	ret.lnct++;
	
	head = lst;
	ret.tks = malloc(sizeof(TkList*) * ret.tkct);
	ret.ixs = malloc(sizeof(int)     * ret.lnct);
	int lineIx = 0;
	for(int i = 0; i < ret.tkct; i++){
		ret.tks[i] = head;
		head = head->next;
		if((head->kind == TL_TKN) && (head->tk.type == TKN_COMMA)){
			ret.ixs[lineIx] = i;
			lineIx++;
		}
	}
	return ret;
}

int lineSize(TkLines* ls, int l){
	if((l >= ls->lnct) || (l < 0)) return 0;
	int end = (l+1 >= ls->lnct)? ls->tkct : ls->ixs[l+1];
	return ls->ixs[l+1] - ls->ixs[l];
}

int skipLines(TkLinePos* ls){
	return 1;
	/*
	if(t->kind == TL_TKN){
		if((t->tk.type == TKN_NEWLINE) || (t->tk.type == TKN_COMMENT) || (t->tk.type == TKN_SEMICOLON)){
			pass = 1;
		}else{
			return 0;
		}
	}*/	
}

/*
	Type Parsing code
*/

int parseType(TkLinePos*, ASTType*);


int parseTyElem(TkLinePos* ls, ASTTypeElem* elem){
	TkLinePos undo = *ls;
	int  pass  =  0;
	TkList* x  =  tkpIx(ls);
	int  size  =  lineSize(ls->ls, ls->line);
	elem->arrs =  malloc(sizeof(int) * size);
	elem->arct = 0;
	while(  x !=  NULL){
		if((x->kind == TL_TKN) && (x->tk.type == TKN_S_TYID)){
			pass = 1;
			elem->tyid = x->tk.data.u64;
			break;
		}else if((x->kind == TL_TKN) && (x->tk.type == TKN_EXP)){
			elem->arrs[elem->arct] = -1;
			elem->arct++;
		}else if( x->kind == TL_BRK){
			TkList* here = x->here;
			if((here == NULL) || (here->kind == TL_NIL)){
				elem->arrs[elem->arct] = 0;
				elem->arct++;
			}else if((here->tk.type == TKN_INT) && (here->next == NULL)){
				uint64_t n = here->tk.data.u64;
				elem->arrs[elem->arct] = n;
				elem->arct++;
			}else{
				break;
			}
		}
		if(!tkpNextIx(ls)) break;
		x = tkpIx(ls);
	}
	int ret = pass && skipLines(ls);
	if(!ret){ free(elem->arrs); elem->arrs = NULL; *ls = undo; }
	return ret;
}



int parseStructField(TkLinePos* ls, int* label, ASTType* val, int* fieldIx){
	TkLinePos undo = *ls;
	TkList* lbl = tkpIx(ls);
	if((lbl == NULL) || (lbl->kind != TL_TKN) || (lbl->tk.type != TKN_S_ID)) { *ls = undo; return 0; }
	*label = lbl->tk.data.u64;
	
	if(!tkpNextIx(ls)){ *ls = undo; return 0; }
	TkList* cln = tkpIx(ls);
	if((cln == NULL) || (cln->kind != TL_TKN) || (cln->tk.type != TKN_COLON)){ *ls = undo; return 0; }
	
	if(!tkpNextIx(ls)){ *ls = undo; return 0; }
	val->kind = TT_ELEM;
	int ret = parseType(ls, val);
	if(!ret) *ls = undo;
	*fieldIx += ret;
	return ret;
}


int parseUnion(TkLinePos* ls, ASTUnion* unon){
	TkLinePos undo = *ls;
	TkList* par = tkpIx(ls);
	if(par->kind != TL_PAR) return 0;
	par = par->here;
	
	TkLines lines = splitLines(par);
	printTkLines(&lines);
	unon->vals    = malloc(sizeof(ASTType) * ls->ls->lnct);
	unon->labels  = malloc(sizeof(int)     * ls->ls->lnct);
	unon->valct   = 0;
	for(int i = 0; i < lines.lnct; i++){
		TkLinePos  ps = (TkLinePos){&lines, i, 0};
		ASTType* vals = unon->vals;
		if(!parseStructField(&ps, &unon->labels[unon->valct], &vals[unon->valct], &unon->valct)){
			if(!skipLines(ls)){
				free(unon->vals);
				free(unon->labels);
				*ls = undo;
				return 0;
			}
		}
	}
	return 1;
}


int parseStruct(TkLinePos* ls, ASTStruct* strc){
	TkLinePos undo = *ls;
	TkList* brk = tkpIx(ls);
	if(brk->kind != TL_BRK) return 0;
	brk = brk->here;
	
	TkLines lines = splitLines(brk);
	printTkLines(&lines);
	strc->vals    = malloc(sizeof(ASTType) * ls->ls->lnct);
	strc->labels  = malloc(sizeof(int)     * ls->ls->lnct);
	strc->valct   = 0;
	for(int i = 0; i < lines.lnct; i++){
		TkLinePos  ps = (TkLinePos){&lines, i, 0};
		ASTType* vals = strc->vals;
		if(!parseStructField(&ps, &strc->labels[strc->valct], &vals[strc->valct], &strc->valct)){
			if(!skipLines(ls)){
				free(strc->vals);
				free(strc->labels);
				*ls = undo;
				return 0;
			}
		}
	}
	return 1;
}


int parseType(TkLinePos* ls, ASTType* type){
	TkLinePos undo = *ls;
	
	TkList* tdef = tkpIx(ls);
	if (tdef == NULL){ *ls = undo; return 0; }
	if      (tdef->kind == TL_TKN){
		type->kind      = TT_ELEM;
		Token tk  = tdef->tk;
		type->type.elem.pos = fusePosition(type->type.elem.pos, tk.pos);
		if      (tk.type == TKN_S_TYID){
			type->type.elem = (ASTTypeElem){tk.pos, tk.data.u64, NULL, 0};
		}else if(tk.type == TKN_S_BID){
			type->kind      = TT_BITY;
			type->type.bity = (ASTBuiltin ){tk.pos, tdef->tk.data.u64};
		}else if(tk.type == TKN_EXP){
			int ret = parseTyElem(ls, &type->type.elem);
			if(!ret) *ls = undo;
			return ret;
		}else{
			*ls = undo;
			return 0;
		}
		if(!tkpNextIx(ls)) return 1;
		return skipLines(ls);
	}else if(tdef->kind == TL_BRK){
		// Either Elem or Strc
		type->kind = TT_ELEM;
		if(parseTyElem(ls, &type->type.elem)) return 1;
		type->kind = TT_STRC;
		int ret = parseStruct(ls, &type->type.strc);
		if(!ret) *ls = undo;
		return ret;
	}else if(tdef->kind == TL_PAR){
		/*
			For now, we're just handling simple unions
			Eventually we want to add support for tagged unions and enums
			Tagged unions start with ( ID TYID : NEWL, where ID is the tag id and TYID is the type, usually an integer
			Enums are similar, but without a tag
			Tagged unions and enums also both may have optional integer values with each value
		*/
		type->kind = TT_UNON;
		int ret = parseUnion(ls, &type->type.unon);
		if(!ret) *ls = undo;
		return ret;
	}else{
		*ls = undo;
		return 0;
	}
	*ls = undo;
	return 0;
}


int parseTyDef(TkLinePos* ls, ASTTyDef* tydf){
	TkLinePos undo = *ls;

	TkList* tyid = tkpIx(ls);
	if((tyid == NULL) || (tyid->kind != TL_TKN) || (tyid->tk.type != TKN_S_TYID)){ *ls = undo; return 0; }
	tydf->tyid = tyid->tk.data.u64;
	tydf->pos  = tyid->tk.pos;
	
	if(!tkpNextIx(ls)){ *ls = undo; return 0; }
	TkList* defn = tkpIx(ls);
	if((defn == NULL) || (defn->kind != TL_TKN) || (defn->tk.type != TKN_DEFINE)){ *ls = undo; return 0; }
	
	if(!tkpNextIx(ls)){ *ls = undo; return 0; }
	int ret = parseType(ls, &tydf->type);
	if(!ret) *ls = undo;
	return ret;
}






/*
	Function parsing code
*/

int parseStatement(TkLinePos* ls, ASTStmt* stmt){
	TkLinePos undo = *ls;
	TkList* rts = tkpIx(ls);
	if((rts == NULL) || (rts->kind != TL_TKN) || (rts->tk.type != TKN_S_ID)) { *ls = undo; return 0; }
	TkLines pars = splitCommas(rts);
	printTkLines(&pars);
	
	
	return 1;
}





int parseFnDefHeader(TkLinePos* ls, ASTFnDef* fndf){
	TkLinePos undo = *ls;

	TkList* fnid = tkpIx(ls);
	if((fnid == NULL) || (fnid->kind != TL_TKN) || (fnid->tk.type != TKN_S_ID  )) return 0;
	fndf->fnid = fnid->tk.data.u64;
	fndf->pos  = fnid->tk.pos;
	
	if(!tkpNextIx(ls)){ *ls = undo; return 0; }
	TkList* defn = tkpIx(ls);
	if((defn == NULL) || (defn->kind != TL_TKN) || (defn->tk.type != TKN_DEFINE)){ *ls = undo; return 0; }
	
	return 1;
}


int parseFnDef(TkLinePos* ls, ASTFnDef* fndf){
	TkLinePos undo = *ls;

	TkList* fnid = tkpIx(ls);
	if((fnid == NULL) || (fnid->kind != TL_TKN) || (fnid->tk.type != TKN_S_ID  )) return 0;
	fndf->fnid = fnid->tk.data.u64;
	fndf->pos  = fnid->tk.pos;
	
	return 1;
}


int parseTestExpr(TkLinePos* ls, ASTBlock* blk){
	TkLinePos undo = *ls;
	TkList* brc = tkpIx(ls);
	printf("A\n");
	if((brc == NULL) || (brc->kind != TL_BRC)) return 0;
	brc = brc->here;
	printf("B\n");
	
	// TODO : Parse basic expressions. Then build code out to compile them to bytecode
	
	TkLines lines = splitLines(brc);
	printTkLines(&lines);
	for(int i = 0; i < lines.lnct; i++){
		printf("C%i\n", i);
		TkLinePos  ps = (TkLinePos){&lines, i, 0};
		if(!parseStatement(&ps, NULL)){
			if(!skipLines(ls)){
				*ls = undo;
				return 0;
			}
		}
	}
	
	return 1;
}



int parseCode(LexerState* tks, SymbolTable* tab, ASTProgram* prog, ErrorList* errs){

	printf("\n=================\n");
	TkList* lst;
	if(!checkWrap(tks, errs, &lst)) return -1;
	printTkList(lst, 0);
	printf("\n=================\n");
	
	TkLines   lines = splitLines(lst);
	printTkLines(&lines);
	
	for(int i = 0; i < lines.lnct; i++){
		printf("LINE %i : %i\n", i, lines.ixs[i]);
		
		TkLinePos lnpos = (TkLinePos){&lines, i, 0};
		ASTTyDef tydf;
		if(parseTyDef(&lnpos, &tydf)){
			prog->tys[prog->tyct] = tydf;
			prog->tyct++;
			//printf("Type @ %i\n", i);
			continue;
		}
		ASTFnDef fndf;
		if(parseFnDef(&lnpos, &fndf)){
			prog->fns[prog->fnct] = fndf;
			prog->fnct++;
			continue;
		}
		
		ASTBlock blok;
		if(parseTestExpr(&lnpos, &blok)){
			printf("Block parsed\n");
			continue;
		}
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


