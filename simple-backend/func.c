#include "func.h"

#include "stdlib.h"




FuncTable makeFuncTable(int size){
	FuncTable ret;
	ret.funcs = malloc(sizeof(FuncDef) * size);
	ret.fnct  = size;
	return ret;
}




ExprUnion makeExpr(int parct){
	ExprUnion ret;
	ret.expr.pars  = malloc((sizeof(ExprUnion) + sizeof(ExprKind)) * parct);
	ret.expr.kinds = ret.expr.pars + (sizeof(ExprUnion) * parct);
	ret.expr.parct = parct;
	return ret;
}


ExprUnion makeCmpd(int parct){
	ExprUnion ret;
	ret.cmpd.pars  = malloc((sizeof(ExprUnion) + sizeof(ExprKind)) * parct);
	ret.cmpd.kinds = ret.cmpd.pars + (sizeof(ExprUnion) * parct);
	ret.cmpd.parct = parct;
	return ret;
}


ExprUnion makePoly(int parct){
	ExprUnion ret;
	ret.poly.pars  = malloc((sizeof(ExprUnion) + sizeof(ExprKind)) * parct);
	ret.poly.kinds = ret.poly.pars + (sizeof(ExprUnion) * parct);
	ret.poly.parct = parct;
	return ret;
}
