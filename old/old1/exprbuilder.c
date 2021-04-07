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

int buildExprElem(Program*, FuncDef*, TypeUnion*, TypeKind, ExprUnion*, ExprKind);



int buildCmpd(Program* p, FuncDef* fn, TypeUnion* tx, TypeKind tk, CmpdExpr* cmpd){
	if(cmpd->parct == 1){
		// Special case
		// apply par[0] to input
	}else{
		if((tk == TK_STRUCT) && (tx->strc.parct == cmpd->parct)){
			for(int i = 0; i < cmpd->parct; i++){
				// apply par[n] to inpar[n]
			}
		}else{
			return -1;	// Broken expression
		}
	}
	return 0;
}

int buildPoly(Program* p, FuncDef* fn, TypeUnion* tx, TypeKind tk, PolyExpr* poly){
	// buildPoly is going to be a bit complex. Not sure yet how to properly handle this.
	// It's probably going to require some more serious infrastructure for building pattern matching code
	return 0;
}

int buildExpr(Program* p, FuncDef* fn, TypeUnion* tx, TypeKind tk, ExprExpr* expr){
	
	return 0;
}

int buildLmda(Program* p, FuncDef* fn, TypeUnion* tx, TypeKind tk, LetExpr * letx){
	return 0;
}


int buildExprElem(Program* p, FuncDef* fn, TypeUnion* tx, TypeKind tk, ExprUnion* expr, ExprKind ek){
	switch(ek){
		case XK_CMPD : {
			return buildCmpd(p, fn, tx, tk, &expr->cmpd);
		}break;
		
		case XK_POLY : {
			return buildPoly(p, fn, tx, tk, &expr->poly);
		}break;
		
		case XK_EXPR : {
			return buildExpr(p, fn, tx, tk, &expr->expr);
		}break;
		
		case XK_LMDA : {
			return buildLmda(p, fn, tx, tk, &expr->letx);
		}break;
	}
	return -1;
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

int buildStmtExpr(Program* p, FuncDef* fn, ExprExpr* expr){
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


