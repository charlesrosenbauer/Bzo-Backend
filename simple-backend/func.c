#include "func.h"

#include "stdlib.h"
#include "stdio.h"




FuncTable makeFuncTable(int size){
	FuncTable ret;
	ret.funcs = malloc(sizeof(FuncDef) * size);
	ret.fnct  = size;
	return ret;
}




FuncDef   makeFuncDef  (Type p, Type r, int bct){
	FuncDef ret;
	ret.pars = p;
	ret.rets = r;
	ret.blocks   = malloc(sizeof(CodeBlock) * bct);
	ret.blockct  = 0;
	ret.blockcap = bct;
	return ret;
}


CodeBlock makeCodeBlock(int parct, int retct, int opct){
	CodeBlock ret;
	ret.pars     = parct;
	ret.rets     = retct;
	ret.vartypes = NULL;
	ret.code     = malloc(sizeof(ThreeAddrCode) * opct);
	ret.size     = 0;
	ret.cap      = opct;
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




void print3AddrCode(ThreeAddrCode c){
	switch(c.opc){
		case OP_ADD   : printf("ADD"   ); break;
		case OP_SUB   : printf("SUB"   ); break;
		case OP_MUL   : printf("MUL"   ); break;
		case OP_DIV   : printf("DIV"   ); break;
		case OP_CALL  : printf("CALL"  ); break;
		case OP_RET   : printf("RET"   ); break;
		case OP_VAR   : printf("VAR"   ); break;
		case OP_CONST : printf("CONST" ); break;
	}
	
	printf("%i %i > %i | ", c.a, c.b, c.c);
	
	if(c.parct != 0) printf("Pars = ");
	for(int i = 0; i < c.parct; i++) printf("%i ", c.pars[i]);
	if(c.retct != 0) printf("Rets = ");
	for(int i = 0; i < c.retct; i++) printf("%i ", c.pars[i+c.parct]);
	
	printPrimitive(c.type, 0);
}

