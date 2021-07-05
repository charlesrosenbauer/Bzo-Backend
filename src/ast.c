#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#include "program.h"
#include "util.h"
#include "ast.h"


/*
	TODO:  build out some code for building ASTPars through append and concat
*/
ASTPars makeASTPars(int size){
	ASTPars ret;
	ret.pars = malloc(sizeof(ASTTyElem) * size);
	ret.lbls = malloc(sizeof(int)       * size);
	ret.prct = 0;
	ret.fill = size;
	return ret;
}

void appendASTPars(ASTPars* ps, ASTTyElem elem, int label){
	if(ps->prct + 1 >= ps->fill){
		ASTTyElem* ptmp = ps->pars;
		int*       ltmp = ps->lbls;
		ps->fill *= 2;
		ps->pars = malloc(sizeof(ASTTyElem) * ps->fill);
		ps->lbls = malloc(sizeof(int)       * ps->fill);
		for(int i = 0; i < ps->prct; i++){
			ps->pars[i] = ptmp[i];
			ps->lbls[i] = ltmp[i];
		}
		free(ptmp);
		free(ltmp);
	}
	ps->pars[ps->prct] = elem;
	ps->lbls[ps->prct] = label;
	ps->prct++;
}


void printASTExpr(ASTExpr* expr){
	if(expr == NULL){
		printf("()");
		return;
	}
	switch(expr->type){
		case XT_INT : printf(" I%li " , expr->tk.data.i64     ); break;
		case XT_FLT : printf(" F%f "  , expr->tk.data.f64     ); break;
		case XT_STR : printf(" S%s "  , expr->tk.data.str.text); break;
		case XT_TAG : printf(" T%s "  , expr->tk.data.str.text); break;
		case XT_ID  : printf(" ID%li ", expr->tk.data.i64     ); break;
		case XT_MID : printf(" MI%li ", expr->tk.data.i64     ); break;
		case XT_PAR :{printf(" ( "); printASTExpr(expr->xp); printf(" ) ");} break;
		
		case XT_BOP :{printf(" ( "); printASTExpr(expr->a);  printf(" %i ", expr->tk.type); printASTExpr(expr->b); printf(" ) ");}; break;
		case XT_UOP :{printf(" ( "); printf(" %i ", expr->tk.type); printASTExpr(expr->a); printf(" ) ");}; break;
		default: printf(" (?) "); break;
	}
}


void printASTPars (ASTPars prs){
	printf(" [");
	for(int i = 0; i < prs.prct; i++){
		if(prs.lbls != NULL) printf("%i : ", prs.lbls[i]);
		if(prs.pars != NULL) printf("TE");
		if((i+1) < prs.prct) printf(", ");
	}
	printf("] ");
}


void printASTBlock(ASTBlock blk){
	printf("{%i} ", blk.stmct);
}


void printASTLambda(ASTLmda lmda){
	printASTPars (lmda.pars);
	if(lmda.isProc) printf("!");
	printASTBlock(lmda.block);
}













ASTProgram makeASTProgram(int size){
	ASTProgram ret;
	ret.hds   = malloc(sizeof(ASTHeader) * size);
	ret.fns   = malloc(sizeof(ASTFnDef ) * size);
	ret.tys   = malloc(sizeof(ASTTyDef ) * size);
	ret.fnct  = 0;
	ret.tyct  = 0;
	ret.hdct  = 0;
	ret.fncap = size;
	ret.tycap = size;
	ret.hdcap = size;
	return ret;
}

void appendFnDef(ASTProgram* prog, ASTFnDef fn){
	if(prog->fnct+1 > prog->fncap){
		ASTFnDef* tmp = prog->fns;
		prog->fns     = malloc(sizeof(ASTFnDef) * prog->fncap * 2);
		for(int i = 0; i < prog->fnct; i++) prog->fns[i] = tmp[i];
		prog->fncap  *= 2;
		free(tmp);
	}
	prog->fns[prog->fnct] = fn;
	prog->fnct++;
}


void appendTyDef(ASTProgram* prog, ASTTyDef ty){
	if(prog->tyct+1 > prog->fncap){
		ASTTyDef* tmp = prog->tys;
		prog->tys     = malloc(sizeof(ASTTyDef) * prog->tycap * 2);
		for(int i = 0; i < prog->tyct; i++) prog->tys[i] = tmp[i];
		prog->tycap  *= 2;
		free(tmp);
	}
	prog->tys[prog->tyct] = ty;
	prog->tyct++;
}


void appendHeader(ASTProgram* prog, ASTHeader hd){
	if(prog->hdct+1 > prog->hdcap){
		ASTHeader* tmp = prog->hds;
		prog->hds     = malloc(sizeof(ASTHeader) * prog->hdcap * 2);
		for(int i = 0; i < prog->hdct; i++) prog->hds[i] = tmp[i];
		prog->hdcap  *= 2;
		free(tmp);
	}
	prog->hds[prog->hdct] = hd;
	prog->hdct++;
}



void printASTProgram(ASTProgram prog){
	printf("HEADS=\n");
	for(int i = 0; i < prog.hdct; i++){ printf("  HD%i | %i %i %s\n", i, prog.hds[i].bid, prog.hds[i].str.len, prog.hds[i].str.text); }
	printf("FUNCS=\n");
	for(int i = 0; i < prog.fnct; i++){
		printf("  FN%i | %i :: ", i, prog.fns[i].fnid);
		printASTPars (prog.fns[i].tvrs); printf(" => ");
		printASTPars (prog.fns[i].pars); printf(" -> ");
		printASTPars (prog.fns[i].rets); printf("\n    ");
		printASTBlock(prog.fns[i].def ); printf("\n");
	}
	printf("TYPES=\n");
	for(int i = 0; i < prog.tyct; i++)	printf("  TY%i | %i\n", i, prog.tys[i].tyid);
}
