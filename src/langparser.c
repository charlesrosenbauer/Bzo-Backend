#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "program.h"
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
	return 1;
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
	if((l < 0) || (l >=     ls->lnct  )) return NULL;
	int end = (l+1 >= ls->lnct)? ls->tkct : ls->ixs[l+1];
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

TkLines takeLine(TkLines* ls, int l){
	TkLines ret = (TkLines){NULL, NULL, 0, 0};
	if((l < 0) || (l >= ls->lnct)) return ret;
	int endl = (l+1 >= ls->lnct)? ls->tkct : ls->ixs[l+1];
	ret.tks  =  ls->tks;
	ret.ixs  = &ls->ixs[l];
	ret.tkct =  endl;
	ret.lnct =  1;
	return ret;
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
						case TKN_NEWLINE   : printf("NL "); break;
						case TKN_INT       : printf("I# "); break;
						case TKN_FLT       : printf("F# "); break;
						case TKN_STR       : printf("ST "); break;
						case TKN_TAG       : printf("TG "); break;
						case TKN_S_ID      : printf("ID "); break;
						case TKN_S_MID     : printf("MI "); break;
						case TKN_S_BID     : printf("BI "); break;
						case TKN_S_TYID    : printf("TI "); break;
						case TKN_COMMENT   : printf("#: "); break;
						case TKN_COMMS     : printf("## "); break;
						case TKN_COMMA     : printf(",  "); break;
						case TKN_SEMICOLON : printf(";  "); break;
						case TKN_ASSIGN    : printf(":= "); break;
						default:             printf("TK "); break;
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
		printf("L%i| ", i);
		printTkLine(ls, ix, end);
		printf("\n");
	}
	printf("\n");
}

void printTkLinesRaw(TkLines* ls){
	for(int i = 0; i < ls->lnct; i++){
		printf("L%i | %i\n", i, ls->ixs[i]);
		//printf("\n");
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
		if((head->kind == TL_TKN) && ((head->tk.type == TKN_NEWLINE) || (head->tk.type == TKN_SEMICOLON)) || (head->kind == TL_NIL) || (head->next == NULL)) ret.lnct++;
		ret.tkct++;
		head = head->next;
	}
	if(ret.tkct < 1) return ret;
	ret.lnct++;
	
	head = lst;
	ret.tks = malloc(sizeof(TkList*) * ret.tkct);
	ret.ixs = malloc(sizeof(int)     * ret.lnct);
	for(int i = 0; i < ret.lnct; i++) ret.ixs[i] = ret.tkct;
	int lineIx = 0;
	ret.ixs[0] = 0;
	for(int i = 0; i < ret.tkct; i++){
		ret.tks[i] = head;
		if(((head->kind == TL_TKN) && ((head->tk.type == TKN_NEWLINE) || (head->tk.type == TKN_SEMICOLON) || (head->tk.type == TKN_COMMENT))) || (i+1 >= ret.tkct)){
			if(lineIx+1 < ret.lnct) ret.ixs[lineIx+1] = i+1;
			lineIx++;
		}
		if(ret.ixs[lineIx] == ret.tkct) ret.lnct = lineIx;	// This is kind of a hack
		head = head->next;
	}
	return ret;
}


TkLines splitCommas(TkLines* ls){
	// split on ,
	TkLines ret = *ls;
	if(ls->lnct < 1) return ret;
	for(int i = ls->ixs[0]; i < ls->tkct; i++){
		TkList* head = ls->tks[i];
		if((head->kind == TL_TKN) && (head->tk.type == TKN_COMMA)) ret.lnct++;
	}
	if(ret.lnct <= ls->lnct) return ret;
	ret.ixs = malloc(sizeof(int) * ret.lnct);
	for(int i = 0; i < ret.lnct; i++) ret.ixs[i] = ls->tkct;
	int ix  = 0;
	int n   = ls->ixs[0];
	for(int i = ls->ixs[0]; i < ls->tkct; i++){
		TkList* head = ls->tks[i];
		if(((head->kind == TL_TKN) && (head->tk.type == TKN_COMMA)) || (i+1 >= ls->tkct)){
			ret.ixs[ix] = n;
			n = i+1;
			ix++;
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
	TkLinePos undo = *ls;
	while(1){
		TkList* t = tkpIx(ls);
		if(t == NULL) return 1;
		if(t->kind == TL_TKN){
			if((t->tk.type != TKN_NEWLINE) && (t->tk.type != TKN_COMMENT) && (t->tk.type != TKN_SEMICOLON)){
				*ls = undo;
				return 0;
			}
		}
		if(!tkpNextIx(ls)) return 1;
	}
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


// TODO: build out expression printing code
void printExpr(ASTExpr*);

void printLiteral(ASTLiteral* lt){
	switch(lt->kind){
		case LK_INT: printf("I%lu "  , lt->i64); break;
		case LK_FLT: printf("F%f "   , lt->f64); break;
		case LK_STR: printf("\"%s\" ", ((StrToken*)lt->ptr)->text); break;
		case LK_TAG: printf("\'%s\' ", ((StrToken*)lt->ptr)->text); break;
		case LK_ID : printf("ID%lu " , lt->i64); break;
		case LK_MID: printf("MI%lu " , lt->i64); break;
		case LK_TID: printf("TI%lu " , lt->i64); break;
		case LK_BID: printf("BI%lu " , lt->i64); break;
	}
}

void printOp(ASTOp* op){
	char* str = "?";
	if      ((op->opc <= TOP_BINOP) && (op->opc >= BTM_BINOP)){
		switch(op->opc){
			case OPR_ADD : str = "+ " ; break;
			case OPR_SUB : str = "- " ; break;
			case OPR_MUL : str = "* " ; break;
			case OPR_DIV : str = "/ " ; break;
			case OPR_MOD : str = "% " ; break;
			case OPR_XOR : str = "\\ "; break;
			case OPR_LS  : str = "< " ; break;
			case OPR_GT  : str = "> " ; break;
			case OPR_LSE : str = "=<" ; break;
			case OPR_GTE : str = ">=" ; break;
			case OPR_EQ  : str = "= " ; break;
			case OPR_NEQ : str = "\\="; break;
			case OPR_IX  : str = "[]" ; break;
			default      : str = "? " ; break;
		}
		printf("(%s ", str);
		printExpr(&((ASTExpr*)op->pars)[0]);
		printExpr(&((ASTExpr*)op->pars)[1]);
		printf(") ");
	}else if((op->opc <= TOP_UNOP)  && (op->opc >= BTM_UNOP )){
		switch(op->opc){
			case OPR_NEG : str = "+ " ; break;
			case OPR_NOT : str = "! " ; break;
			case OPR_DREF: str = "<-" ; break;
			case OPR_PTR : str = "* " ; break;
			default      : str = "? " ; break;
		}
		printf("(%s ", str);
		printExpr(op->pars);
		printf(") ");
	}else{
		printf("(?) ");
	}
}


void printFnCall(ASTFnCall* x){
	printf("[%i:", x->func);
	ASTExpr* xs = x->pars;
	for(int i = 0; i < x->prct; i++)
		printExpr(&xs[i]);
	printf("] ");
}


void printStmt(ASTStmt* stmt){
	for(int i = 0; i < stmt->prct; i++)
		if(stmt->pars[i] >= 0)
			printf("X%i, ", stmt->pars[i]);
		else
			printf("_ , ");
	printf(":=");
	printExpr(&stmt->expr);
	printf(";  ");
}


void printBlock(ASTBlock* blok){
	printf("{");
	for(int i = 0; i < blok->stmtct; i++) printStmt(&blok->stmts[i]);
	printf("} ");
}


void printPars(ASTPars* pars){
	printf("[");
	for(int i = 0; i < pars->prct; i++)
		if(pars->pars[i] >= 0)
			printf("X%i, ", pars->pars[i]);
		else
			printf("_ , ");
	printf("] ");
}


void printLmda(ASTLmda* lmda){
	printPars (&lmda->pars);
	if(lmda->isProc) printf("!");
	printBlock(&lmda->blok);
}


void printExpr(ASTExpr* expr){
	switch(expr->type){
		case XT_VOID : printf("(_) ");										break;
		case XT_LMDA : printLmda   (expr->data);							break;
		case XT_BLOK : printBlock  (expr->data);							break;
		case XT_PARS : printPars   (expr->data);							break;
		case XT_LTRL : printLiteral(expr->data); 							break;
		case XT_UNOP : printOp     (expr->data); 							break;
		case XT_BNOP : printOp     (expr->data); 							break;
		case XT_FNCL : printFnCall (expr->data);			    			break;
		case XT_PARN : printf("("); printExpr(expr->data); printf(") ");	break;
	}
}


typedef enum{
	MK_TKN,
	MK_PAR,
	MK_AIX,
	MK_FNC,
	MK_PRM,
	MK_BLK,
	MK_EXP,
	MK_NIL
}TmpExprKind;

typedef struct{
	void* 	    data;
	TmpExprKind kind;
}TmpExpr;

void printTmpExprSimple(TmpExpr* xs, int ct){
	for(int i = 0; i < ct; i++){
		switch(xs[i].kind){
			case MK_TKN : {
				TkList* t = xs[i].data;
				switch(t->tk.type){
					case TKN_ADD 	: printf("+   " ); break;
					case TKN_SUB 	: printf("-   " ); break;
					case TKN_MUL 	: printf("*   " ); break;
					case TKN_DIV	: printf("/   " ); break;
					case TKN_MOD	: printf("%%   "); break;
					case TKN_WHERE  : printf("@   " ); break;
					case TKN_INT 	: printf("INT " ); break;
					case TKN_FLT 	: printf("FLT " ); break;
					case TKN_STR 	: printf("STR " ); break;
					case TKN_TAG 	: printf("TAG " ); break;
					case TKN_WILD	: printf("_   " ); break;
					case TKN_S_ID   : printf("ID  " ); break;
					case TKN_S_BID  : printf("BID " ); break;
					case TKN_S_TYID : printf("TID " ); break;
					case TKN_S_MID  : printf("MID " ); break;
					case TKN_NEWLINE: printf("NL  " ); break;
					default			: printf("TKN " ); break;
				}
			}break;
			case MK_PAR : printf("(x) "); break;
			case MK_AIX : printf("[i] "); break;
			case MK_FNC : printf("[F] "); break;
			case MK_PRM : printf("[p] "); break;
			case MK_BLK : printf("{x} "); break;
			case MK_NIL : printf("|_| "); break;
			case MK_EXP : printf("EXP "); break;
			default     : printf("??? "); break;
		}
	}
	printf("\n");
}


int parseExpr(TkLinePos*, int, int, ASTExpr*);

// ( expr )
int parseParentheses(TkLinePos* p, int ix, ASTExpr* ret){
	TkLinePos undo = *p;
	if(p->ls->tks[ix]->kind == TL_PAR){
		ret->type     = XT_PARN;
		ret->data	  = malloc(sizeof(ASTExpr));
		TkLines lines = splitLines(p->ls->tks[ix]->here);
		TkLinePos sub = (TkLinePos){&lines, 0, 0};
		int rvl = parseExpr(&sub, 0, lines.tkct, ret->data);
		if(!rvl){
			*p = undo;
			free(ret->data);
			ret->data = NULL;
		}
		return rvl;
	}
	return 0;
}

// [ expr ]
int parseArrIndex(TkLinePos* p, int ix, ASTExpr* ret){
	TkLinePos undo = *p;
	if(p->ls->tks[ix]->kind == TL_BRK){
		ret->type     = XT_ARIX;
		ret->data	  = malloc(sizeof(ASTExpr));
		TkLines lines = splitLines(p->ls->tks[ix]->here);
		TkLinePos sub = (TkLinePos){&lines, 0, 0};
		int rvl = parseExpr(&sub, 0, lines.tkct, ret->data);
		if(!rvl){
			*p = undo;
			free(ret->data);
			ret->data = NULL;
		}
		return rvl;
	}
	return 0;
}

// [ f : expr, ... expr ]
int parseFnCall(TkLinePos* p, int ix, ASTExpr* ret){
	TkLinePos undo = *p;
	return 0;
}

// [ x, ... x ]
int parseParams(TkLinePos* p, int ix, ASTExpr* ret){
	TkLinePos undo = *p;
	return 0;
}

// { stmt ; ... stmt }
int parseBlock(TkLinePos* p, int ix, ASTExpr* ret){
	TkLinePos undo = *p;
	return 0;
}


ASTExpr makeLiteral(TkList* t){
	Position  p = t->tk.pos;
	ASTExpr ret = (ASTExpr){p, XT_LTRL, NULL};
	ASTLiteral lit;
	switch(t->tk.type){
		case TKN_S_ID  : lit = (ASTLiteral){p,         t->tk.data.i64, LK_ID }; break;
		case TKN_S_MID : lit = (ASTLiteral){p,         t->tk.data.i64, LK_MID}; break;
		case TKN_STR   : lit = (ASTLiteral){p, (void*)&t->tk.data.str, LK_STR}; break;
		case TKN_TAG   : lit = (ASTLiteral){p, (void*)&t->tk.data.str, LK_TAG}; break;
		case TKN_INT   : lit = (ASTLiteral){p,         t->tk.data.i64, LK_INT}; break;
		case TKN_FLT   : lit = (ASTLiteral){p,         t->tk.data.f64, LK_FLT}; break;
		default        : printf("makeLiteral() assertion failed.\n"); exit(-1); break;	// Technically this is an error, but it shouldn't happen.
	}
	ret.data    = malloc(sizeof(ASTLiteral));
	*((ASTLiteral*)ret.data) = lit;
	return ret;
}


int parseExpr(TkLinePos* p, int start, int end, ASTExpr* expr){
	if(start >= end) return 0;
	TkLinePos undo = *p;
	
	TmpExpr* exps = malloc(sizeof(TmpExpr) * (end-start));
	int      exct = 0;
	for(int i = start; i < end; i++){
		TkList* t  = p->ls->tks[i];
		switch(t->kind){
			case TL_PAR: {
				ASTExpr  exp;
				if(!parseParentheses(p, i, &exp)){ free(exps); return 0; }
				ASTExpr* x = malloc(sizeof(ASTExpr));
				*x = exp;
				exps[exct] = (TmpExpr){x, MK_PAR};
			}break;
			case TL_BRK: {
				ASTExpr  exp;
				if      (parseArrIndex(p, i, &exp)){
					ASTExpr* x = malloc(sizeof(ASTExpr)); *x = exp;
					exps[exct] = (TmpExpr){x, MK_AIX};
				}else if(parseFnCall  (p, i, &exp)){
					ASTExpr* x = malloc(sizeof(ASTExpr)); *x = exp;
					exps[exct] = (TmpExpr){x, MK_FNC};
				}else if(parseParams  (p, i, &exp)){
					ASTExpr* x = malloc(sizeof(ASTExpr)); *x = exp;
					exps[exct] = (TmpExpr){x, MK_PRM};
				}else{
					free(exps);
					return 0;
				}
			}break;
			case TL_BRC: {
				ASTExpr  exp;
				if(!parseBlock(p, i, &exp)){ free(exps); return 0; }
				ASTExpr* x = malloc(sizeof(ASTExpr));
				*x = exp;
				exps[exct] = (TmpExpr){x, MK_BLK};	// TODO: Parse as Block
			}break;
			case TL_TKN: exps[exct] = (TmpExpr){t, MK_TKN}; break;
			default: { free(exps); *p = undo; return 0; }
		}
		exct++;	
	}
	
	printTmpExprSimple(exps, exct);
	int size = exct;
	int last = exct+1;
	while(size < last){
		// X F		=> Fn Call
		for(int i = 0; i < size-1; i++){
			TmpExpr *a = &exps[i], *b = &exps[i+1];
			int aid = (a->kind == MK_TKN) && ((((TkList*)a->data)->tk.type == TKN_S_ID ) || (((TkList*)a->data)->tk.type == TKN_S_MID));
			int bid = (b->kind == MK_TKN) && ((((TkList*)b->data)->tk.type == TKN_S_ID ) || (((TkList*)b->data)->tk.type == TKN_S_MID));
			if      ( (a->kind == MK_EXP) && bid){
				TmpExpr head = *a;
				a->kind = MK_FNC;
				
				
				
			}else if(aid && bid){
			
			}
		}
		
		// O O X	=> Unop
		
		// X O X	=> Binop
		
		// Simplify
	}
	
	free(exps);
	return 1;
}


int parseStatement(TkLinePos* p, ASTStmt* stmt){
	TkLinePos undo = *p;
	
	// Split on := into RETS | EXPR
	int ix    = p->ls->ixs[0];
	int end   = p->ls->tkct;
	int split = -1;
	for(int i = ix; i < end; i++){
		// Split on :=
		TkList* t = p->ls->tks[i];
		if( t == NULL) return -1;
		if((t->kind == TL_TKN) && (t->tk.type == TKN_ASSIGN)){
			split = i;
			break;
		}
	}
	if(split == -1){	// Unable to split; not a statement
		*p = undo;
		return 0;
	}
	
	// Get retval list (including _)
	stmt->pars = malloc(sizeof(int) * (split-ix));
	stmt->prct = 0;
	for(int i = ix; i < split; i++){
		TkList* t = p->ls->tks[i];
		if(t == NULL){ *p = undo; return 0; }
		if      ((t->kind == TL_TKN) && ((t->tk.type == TKN_S_ID) || (t->tk.type == TKN_S_MID))){
			stmt->pars[stmt->prct] = t->tk.data.u64;
			stmt->prct++;
		}else if((t->kind == TL_TKN) && ( t->tk.type == TKN_WILD)){
			stmt->pars[stmt->prct] = -1;
			stmt->prct++;
		}else{
			*p = undo;
			return 0;
		}
		i++;
		t = p->ls->tks[i];
		if(t == NULL){ *p = undo; return 0; }
		if((t->kind == TL_TKN) && ((t->tk.type == TKN_COMMA) || (t->tk.type == TKN_ASSIGN))) continue;
	}
	printf("PARS(%i)=", stmt->prct);
	for(int i = 0; i < stmt->prct; i++) printf("%i ", stmt->pars[i]);
	printf("\n");
	
	// Store EXPR into a custom expression list data structure
	ASTExpr expr = (ASTExpr){(Position){0, 0, 0, 0, 0}, 0, 0};
	int pass = parseExpr(p, split+1, end, &expr);
	if(!pass){ *p = undo; return 0; }
	stmt->expr = expr;
	
	// Parse each wrapped value, label them accordingly
	
	// Recursively parse expression list
	
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
	printf("A %p\n", brc);
	if((brc == NULL) || (brc->kind != TL_BRC)) return 0;
	brc = brc->here;
	printf("B\n");
	
	// TODO : Parse basic expressions. Then build code out to compile them to bytecode
	
	TkLines lines = splitLines(brc);
	printTkLines(&lines);
	blk->stmtct = lines.lnct;
	blk->stmts  = malloc(sizeof(ASTStmt) * blk->stmtct);
	int smix    = 0;
	for(int i = 0; i < lines.lnct; i++){
		printf("C%i\n", i);
		TkLines  line = takeLine(&lines, i);
		TkLinePos  ps = (TkLinePos){&line, 0, 0};
		printTkLines(&line);
		blk->stmts[smix] = (ASTStmt){(Position){0, 0, 0, 0, 0}, (ASTExpr){(Position){0, 0, 0, 0, 0}, 0, NULL}, 0, 0};
		if(!parseStatement(&ps, &blk->stmts[smix])){
			if(!skipLines(ls)){
				*ls = undo;
				return 0;
			}
		}
		if((blk->stmts[smix].expr.type != XT_VOID) || (blk->stmts[smix].prct != 0))	smix++;
	}
	blk->stmtct = smix+1;
	
	printBlock(blk);
	printf("\n");
	
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
		
		ASTBlock blok = (ASTBlock){(Position){0, 0, 0, 0, 0}, NULL, 0};
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


