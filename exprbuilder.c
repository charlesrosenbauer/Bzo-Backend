#include "program.h"
#include "func.h"
#include "type.h"
#include "alloc.h"

#include "stdint.h"
#include "stdlib.h"


/*
	Current plan here is to treat each expression as a list of transforms, and then recursively break each
	transform into something that decomposes the type of its input, applies lower-level transforms, and then
	constructs the output type and passes it to the next transform.
*/

int buildExpr(Program*, FuncDef*, TypeUnion*, TypeKind, ExprUnion*, ExprKind);



int buildCmpd(FuncDef* fn, int in, CmpdExpr* cmpd){
	Type* t = &fn->vartypes[in];
	if(cmpd->parct == 1){
		// Special case
		// apply par[0] to input
	}else{
		if((t->kind == TK_STRUCT) && (t->type.strc.parct == cmpd->parct)){
			for(int i = 0; i < cmpd->parct; i++){
				// apply par[n] to inpar[n]
			}
		}else{
			return -1;	// Broken expression
		}
	}
	return 0;
}


int buildExpr(Program* p, FuncDef* fn, TypeUnion* tx, TypeKind tk, ExprUnion* expr, ExprKind ek){
	switch(ek){
	
	}
	return 0;
}


int buildTaillessStmt(Program* p, FuncDef* fn, ExprExpr* expr){
	ExprUnion* xps = expr->pars;
	if(expr->parct <= 0) return -1;
	
	// Head
	ExprUnion head = xps[0];
	ExprKind  hknd = expr->kinds[0];
	
	
	
	// Body
	for(int i = 1; i < expr->parct; i++){
		ExprUnion bdx = xps[i];
		ExprKind  bdk = expr->kinds[i];
	}
	
	return 0;
}

int buildStmt(Program* p, FuncDef* fn, ExprExpr* expr){
	expr->parct--;
	int ret = buildTaillessStmt(p, fn, expr);
	expr->parct++;
	if(ret < 0) return ret;

	// Tail
	ExprUnion* xps = expr->pars;
	ExprUnion tail = xps[expr->parct-1];
	ExprKind  tknd = expr->kinds[expr->parct-1];

	return 0;
}


