#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "langparser.h"




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

uint8_t* allocate(AllocatorAST* a, int size, int align){
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





/*
	Parser
*/
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

inline TkType peekToken(LexerState* ls, int ix){
	if((ix < 0) || (ix >= ls->tkcap)) return TKN_VOID;
	return ls->tks[ix].type;
}




/*
	Type parsing stuff
*/

int parseASTType(LexerState* tks, AllocatorAST* alloc, int tix, ASTType* ret){
	int ix = tix;
	if(tks->tks[ix].type == TKN_S_TYID){
		ret->tyid = tks->tks[ix].data.u64;
		ret->arrs = NULL;
		ret->arct = 0;
		ret->pos  = tks->tks[ix].pos;
		return ix+1;
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
			return -1;
		}
	}
	
	// Second pass: allocate space to track allocators, then build them
	ix = tix;
	ret->arrs = allocate(alloc, ret->arct * sizeof(int), 8);
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
			return ix-tix;
		}else{
			return -1;
		}
	}
}

int parseTypeDef(LexerState* tks, SymbolTable* tab, ASTProgram* prog, int tix, int pix){
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
		return -1;
	}
	
	prog->tys[prog->tyct] = (ASTTyDef){p, tyid};
	prog->tyct++;
	
	return parseASTType(tks, &prog->alloc, ix, &prog->tys[prog->tyct-1].type) + (ix-tix);
}

/*
	Function parsing stuff
*/
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
			return -1;
		}
	}
	int isExpr = parseExpr(tks, alloc, ix, &ret->expr);
	if(isExpr > 0){
		ret->pars = 0;	// allocate parct*int
		for(int i = 0; i < parct; i++)
			ret->pars[i] = tks->tks[tix+i+i].data.u64;
		ret->prct = parct;
	}
	
}


int parseFuncDef(LexerState* tks, SymbolTable* tab, ASTProgram* prog, int tix, int pix){
	
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
		
		skip = parseFuncDef(tks, tab, prog, tix, pix);
		if(skip > 1){
			tix += skip;
			continue;
		}
		
		skip = parseTypeDef(tks, tab, prog, tix, pix);
		if(skip > 1){
			tix += skip;
			continue;
		}
		
		printf("Could not parse file... stuck at %i\n", tix);
		return -1;
	}

	return 0;
}


void printASTType(ASTType ty){
	for(int i = 0; i < ty.arct; i++){
		if(ty.arrs[i] < 0){
			printf("^");
		}else if(ty.arrs[i] == 0){
			printf("[]");
		}else{
			printf("[%i]", ty.arrs[i]);
		}
	}
	printf("T%i", ty.tyid);
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
		printf("    ");
		printASTType(prog.tys[i].type);
		printf("\n");
	}
}


