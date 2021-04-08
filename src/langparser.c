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
	TkLines ret  = (TkLines){NULL, NULL, 0, 0};
	TkList* head = lst;
	while(head  != NULL){
		if((head->kind == TL_TKN) && ((head->tk.type == TKN_NEWLINE) || (head->tk.type == TKN_SEMICOLON))) ret.lnct++;
		ret.tkct++;
		head = head->next;
	}
	if(ret.tkct < 1){
		return ret;
	}
	ret.lnct++;
	
	head = lst;
	ret.tks = malloc(sizeof(TkList*) * ret.tkct);
	ret.ixs = malloc(sizeof(int)     * ret.lnct);
	int lineIx = 0;
	ret.ixs[0] = 0;
	for(int i = 0; i < ret.tkct; i++){
		ret.tks[i] = head;
		if((head->kind == TL_TKN) && ((head->tk.type == TKN_NEWLINE) || (head->tk.type == TKN_SEMICOLON) || (head->tk.type == TKN_COMMENT))){
			ret.ixs[lineIx+1] = i+1;
			lineIx++;
		}
		head = head->next;
	}
	return ret;
}

TkLines splitCommas(TkList* lst){
	// split on ,
	TkLines ret  = (TkLines){NULL, NULL, 0, 0};
	TkList* head = lst;
	while(head  != NULL){
		if((head->kind == TL_TKN) && (head->tk.type == TKN_COMMA)) ret.lnct++;
		ret.tkct++;
		head = head->next;
	}
	if(ret.tkct < 1){
		return ret;
	}
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




int parseTyElem(TkLinePos* ls, ASTTypeElem* elem){
	return 1;
}


int parseTyDef(TkLinePos* ls, ASTTyDef* tydf){
	TkList* tyid = tkpIx(ls);
	if((tyid == NULL) || (tyid->kind != TL_TKN) || (tyid->tk.type != TKN_S_TYID)) return 0;
	tydf->tyid = tyid->tk.data.u64;
	tydf->pos  = tyid->tk.pos;
	
	if(!tkpNextIx(ls)) return 0;
	TkList* defn = tkpIx(ls);
	if((defn == NULL) || (defn->kind != TL_TKN) || (defn->tk.type != TKN_DEFINE)) return 0;
	
	if(!tkpNextIx(ls)) return 0;
	TkList* tdef = tkpIx(ls);
	if (tdef == NULL)  return 0;
	if      (tdef->kind == TL_TKN){
		tydf->type.kind      = TT_ELEM;
		Token tk  = tdef->tk;
		tydf->pos = fusePosition(tydf->pos, tk.pos);
		if      (tdef->tk.type == TKN_S_TYID){
			tydf->type.type.elem = (ASTTypeElem){tk.pos, tk.data.u64, NULL, 0};
		}else if(tdef->tk.type == TKN_S_BID){
			tydf->type.kind      = TT_BITY;
			tydf->type.type.bity = tdef->tk.data.u64;
		}else if(tdef->tk.type == TKN_EXP){
			return parseTyElem(ls, &tydf->type.type.elem);
		}else{
			return 0;
		}
		if(!tkpNextIx(ls)) return 0;
		return skipLines(ls);
	}else if(tdef->kind == TL_BRK){
		// Either Elem or Strc
		return parseTyElem(ls, &tydf->type.type.elem);
	}else if(tdef->kind == TL_PAR){
		// Unon
	}else{
		return 0;
	}
	
	return 1;
}



int parseFnDef(TkLinePos* ls, ASTFnDef* fndf){
	TkList* fnid = tkpIx(ls);
	if((fnid == NULL) || (fnid->kind != TL_TKN) || (fnid->tk.type != TKN_S_ID  )) return 0;
	fndf->fnid = fnid->tk.data.u64;
	fndf->pos  = fnid->tk.pos;
	
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
		printf("BID%i", ty.type.bity);
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


