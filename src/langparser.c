#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "langparser.h"
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





/*
	Parser
*/
typedef struct{
	int(* pptr)(LexerState*, void*, void*, int);
	void* pars;
}ParserFunc;

typedef struct{
	LexerState tks;
	int        tix;
}ParserState;


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
	Type parsing stuff
*/


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
int parseASTTypeElem(LexerState* tks, AllocatorAST* alloc, int tix, ASTTypeElem* ret){
	int ix = tix;
	if(tks->tks[ix].type == TKN_S_TYID){
		ret->tyid = tks->tks[ix].data.u64;
		ret->arrs = NULL;
		ret->arct = 0;
		ret->pos  = tks->tks[ix].pos;
		return 1;
	}
	
	// First pass: figure out how many decorators are on here
	while(1){
		if(tks->tkct < (ix+3)) return -1;
		if(tks->tks[ix].type == TKN_EXP){
			ret->arct++;
			ix  += 1;
		}else if((tks->tks[ix  ].type == TKN_BRK_OPN) &&
				 (tks->tks[ix+1].type == TKN_BRK_END)){
			ret->arct++;
			ix  += 2; 
		}else if((tks->tks[ix  ].type == TKN_BRK_OPN) &&
				 (tks->tks[ix+1].type == TKN_INT    ) &&
				 (tks->tks[ix+2].type == TKN_BRK_END)){
			ret->arct++;
			ix  += 3;		 
		}else if(tks->tks[ix].type == TKN_S_TYID){
			break;
		}else{
			return -17;
		}
	}
	
	// Second pass: allocate space to track allocators, then build them
	ix = tix;
	ret->arrs = (int*)allocate(alloc, ret->arct * sizeof(int), 8);
	ret->pos  = tks->tks[ix].pos;
	int arix  = 0;
	while(1){
		if(tks->tkct < (ix+3)) return -1;
		if(tks->tks[ix].type == TKN_EXP){
			ret->arrs[arix] = -1;
			arix += 1;
			ix   += 1;
		}else if((tks->tks[ix  ].type == TKN_BRK_OPN) &&
				 (tks->tks[ix+1].type == TKN_BRK_END)){
			ret->arrs[arix] = 0;
			arix += 1;
			ix   += 2;
		}else if((tks->tks[ix  ].type == TKN_BRK_OPN) &&
				 (tks->tks[ix+1].type == TKN_INT    ) &&
				 (tks->tks[ix+2].type == TKN_BRK_END)){
			ret->arrs[arix] = tks->tks[ix+1].data.u64;
			arix += 1;
			ix   += 3;		 
		}else if(tks->tks[ix].type == TKN_S_TYID){
			ret->tyid = tks->tks[ix].data.i64;
			return (ix+1)-tix;
		}else{
			return -18;
		}
	}
}





// Headers for mutual recursion
int parseASTStruct(LexerState*, AllocatorAST*, int, ASTStruct*);
int parseASTUnion (LexerState*, AllocatorAST*, int, ASTUnion *);
int parseASTType  (LexerState*, AllocatorAST*, int, ASTType  *);



int parseASTStruct(LexerState* tks, AllocatorAST* alloc, int tix, ASTStruct* ret){
	int ix   = tix;
	int prct = 0;
	if(peekToken(tks, ix) != TKN_BRK_OPN) return -1;
	ret->pos = tks->tks[ix].pos;
	ix++;
	
	int closed = 0;
	TList* head = malloc(sizeof(TList));
	TList* tail = head;
	head->here = NULL;
	head->next = NULL;
	head->name = 0;
	
	while(ix < tks->tkct){
		TkType tk = peekToken(tks, ix);
		
		if      (tk == TKN_NEWLINE){
			ix++;
		}else if(tk == TKN_S_ID){
			int id = tks->tks[ix].data.u64;
			ASTType* ty = (ASTType*)allocate(alloc, sizeof(ASTType), 8);
			int skip = parseASTType(tks, alloc, ix+1, ty);
			ix += skip + 1;
			if((skip < 1) || (peekToken(tks, ix) != TKN_NEWLINE)){ freeTList(head); return -2; }
			tail->next = malloc(sizeof(TList));
			tail = tail->next;
			tail->next = NULL;
			tail->here = ty;
			tail->name = id;
			prct++;
		}else if(tk != TKN_BRK_END){
			freeTList(head);
			return -3;
		}else{
			closed = 1;
			break;
		}
	}
	if(!closed){ freeTList(head); return -4; }
	
	ret->valct  = prct;
	ret->vals   = (ASTType*)allocate(alloc, sizeof(ASTType) * prct, 8);
	ret->labels = (int    *)allocate(alloc, sizeof(int    ) * prct, 4);
	tail = head->next;
	ASTType* pars = ret->vals;
	int vi = 0;
	while(tail != NULL){
		pars[vi]        = *(ASTType*)tail->here;
		ret->labels[vi] = tail->name;
		tail->here      = NULL;
		vi++;
		tail = tail->next;
	}
	
	freeTList(head);
	return (ix+1)-tix;
}


int parseASTUnion(LexerState* tks, AllocatorAST* alloc, int tix, ASTUnion* ret){
	int ix   = tix;
	int prct = 0;
	if(peekToken(tks, ix) != TKN_PAR_OPN) return -1;
	ret->pos = tks->tks[ix].pos;
	ix++;
	
	int closed = 0;
	TList* head = malloc(sizeof(TList));
	TList* tail = head;
	head->here = NULL;
	head->next = NULL;
	head->name = 0;
	
	while(ix < tks->tkct){
		TkType tk = peekToken(tks, ix);
		
		if      (tk == TKN_NEWLINE){
			ix++;
		}else if(tk == TKN_S_ID){
			int id = tks->tks[ix].data.u64;
			ASTType* ty = (ASTType*)allocate(alloc, sizeof(ASTType), 8);
			int skip = parseASTType(tks, alloc, ix+1, ty);
			ix += skip + 1;
			if((skip < 1) || (peekToken(tks, ix) != TKN_NEWLINE)){ freeTList(head); return -2; }
			tail->next = malloc(sizeof(TList));
			tail = tail->next;
			tail->next = NULL;
			tail->here = ty;
			tail->name = id;
			prct++;
		}else if(tk != TKN_PAR_END){
			freeTList(head);
			return -3;
		}else{
			closed = 1;
			break;
		}
	}
	if(!closed){ freeTList(head); return -4; }
	
	ret->valct  = prct;
	ret->vals   = (ASTType*)allocate(alloc, sizeof(ASTType) * prct, 8);
	ret->labels = (int    *)allocate(alloc, sizeof(int    ) * prct, 4);
	tail = head->next;
	ASTType* pars = ret->vals;
	int vi = 0;
	while(tail != NULL){
		pars[vi]        = *(ASTType*)tail->here;
		ret->labels[vi] = tail->name;
		tail->here      = NULL;
		vi++;
		tail = tail->next;
	}
	
	freeTList(head);
	return (ix+1)-tix;
}



int parseASTType(LexerState* tks, AllocatorAST* alloc, int tix, ASTType* ret){
	int ix = tix;
	
	TkType tkind = peekToken(tks, ix);
	if(tkind == TKN_BRK_OPN){
		// Either ELEM or STRC
		int skip;
		ret->kind = TT_ELEM;
		skip = parseASTTypeElem(tks, alloc, ix, &ret->type.elem);
		if(skip > 0) return skip;
		ret->kind = TT_STRC;
		return parseASTStruct  (tks, alloc, ix, &ret->type.strc);
	}else if(tkind == TKN_PAR_OPN){
		// UNON
		ret->kind = TT_UNON;
		return parseASTUnion(tks, alloc, ix, &ret->type.unon);
	}else if(tkind == TKN_S_BID){
		// BITY
		ret->kind = TT_BITY;
		ret->type.bity = tks->tks[ix].data.u64;
		return 1;
	}else{
		int skip;
		ret->kind = TT_ELEM;
		skip = parseASTTypeElem(tks, alloc, ix, &ret->type.elem);
		if(skip > 0) return skip;
		return -9;
	}
	
	return 0;
}


int parseTypeDef(LexerState* tks, ASTProgram* prog, int tix){
	int ix = tix;
	if(tks->tkct < (ix+3)) return -1;
	
	Position p;
	int tyid;
	if ((tks->tks[ix  ].type == TKN_S_TYID) &&
		(tks->tks[ix+1].type == TKN_DEFINE)){
		tyid = tks->tks[ix].data.u64;
		p    = tks->tks[ix].pos;
		ix  += 2;
	}else{
		return -10;
	}
	
	ASTType type;
	if(tks->tks[ix+2].type == TKN_S_BID){
		if(isTypeBID(tks->tks[ix+2].data.u64)){
			type.type.bity = tks->tks[ix+2].data.u64;
			type.kind      = TT_BITY;
			prog->tys[prog->tyct] = (ASTTyDef){p, tyid, type};
			prog->tyct++;
			return 3;
		}else{
			return -11;
		}
	}
	
	prog->tys[prog->tyct] = (ASTTyDef){p, tyid, type};
	prog->tyct++;
	
	int skip = parseASTType(tks, &prog->alloc, ix, &prog->tys[prog->tyct-1].type);
	return (skip < 1)? skip : skip + (ix-tix);
}*/

/*
	Function parsing stuff
*//*
int parseExpr(LexerState* tks, AllocatorAST* alloc, int tix, ASTExpr* ret){
	return 0;
}


int parseStmt(LexerState* tks, AllocatorAST* alloc, int tix, ASTStmt* ret){
	int ix    = tix;
	int parct = 0;
	while(1){
		if(tks->tkct < (ix+2)) return -1;
		
		if(((tks->tks[ix  ].type == TKN_S_ID )  ||
			(tks->tks[ix  ].type == TKN_S_MID)) &&
			(tks->tks[ix+1].type == TKN_COMMA)){
			parct++;
			ix += 2;
		}else if(tks->tks[ix].type == TKN_ASSIGN){
			ix++;
			break;
		}else{
			return -13;
		}
	}
	int isExpr = parseExpr(tks, alloc, ix, &ret->expr);
	if(isExpr > 0){
		ret->pars = 0;	// allocate parct*int
		for(int i = 0; i < parct; i++)
			ret->pars[i] = tks->tks[tix+i+i].data.u64;
		ret->prct = parct;
	}
	
	// TODO: finish me
	
	return -14;
}
*/

int parseTypeDef(LexerState* tks, ASTProgram* prog, int tix){

	return 0;
}

int parseFuncDef(LexerState* tks, SymbolTable* tab, ASTProgram* prog, int tix){
	
	return 0;
}



/*
	Parser rewrite - make it more composable!
	
	General idea:
	* Parsing functions return new index
	* Functions to provide regex-like abstractions
	* Helpful error messages
*/




// Run a parser until it no longer succeeds. If none pass, pass anyway
int parseMany(LexerState* tks, ParserFunc* f, void* retval, int tix){
	int ix = tix;
	int ct = 0;
	TList*   tail = retval;
	while(ix < tks->tkct){
		void* x = NULL;
		int nx  = f->pptr(tks, f->pars, &x, ix);
		if(nx < 0) break;
		
		ct++;
		ix = nx;
		tail->next = malloc(sizeof(TList));
		tail = tail->next;
		tail->next = NULL;
		tail->here = x;
	}
	return ix;
}


// Run a parser until it no longer succeeds. If none pass, fail
int parseSome(LexerState* tks, ParserFunc* f, void* retval, int tix){
	int ix = parseMany(tks, f, retval, tix);
	return (ix > tix)? ix : -1;
}

int parseList(LexerState* tks, int(*pptr)(void*), void* pars, int tix){
	int ix = tix;
	//TkType t0 = 
	return 0;
}





int parseCode(LexerState* tks, SymbolTable* tab, ASTProgram* prog){
	int tix = 0;
	int pix = 0;
	while(tix < tks->tkct){
		int skip;
		if(tks->tks[tix].type == TKN_NEWLINE){
			tix ++;
			continue;
		}
		if(tks->tks[tix].type == TKN_COMMENT){
			tix ++;
			continue;
		}
		if(tks->tks[tix].type == TKN_COMMS){
			tix ++;
			continue;
		}
		
		skip = parseFuncDef(tks, tab, prog, tix);
		if(skip > 1){
			tix += skip;
			continue;
		}
		
		skip = parseTypeDef(tks, prog, tix);
		if(skip > 1){
			tix += skip;
			continue;
		}
		
		printf("Could not parse file... stuck at %i. Error=%i\n", tix, skip);
		return -15;
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
	printf("Functions:\n");
	for(int i = 0; i < prog.fnct; i++){
		Position p = prog.fns[i].pos;
		printf("  FN%i %i@(%i:%i - %i:%i)\n", i, p.fileId, p.lineStart, p.colStart, p.lineStart, p.lineEnd);
	}
	
	printf("Types:\n");
	for(int i = 0; i < prog.tyct; i++){
		Position p = prog.tys[i].pos;
		printf("  TY%i %i@(%i:%i - %i:%i)\n", i, p.fileId, p.lineStart, p.colStart, p.lineStart, p.lineEnd);
		printASTType(prog.tys[i].type, 2);
		printf("\n");
	}
}


