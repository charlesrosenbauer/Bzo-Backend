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


void setIx(ExprUnion* c, ExprUnion x, ExprKind k, int i){
	ExprUnion* pars = c->cmpd.pars;
	pars         [i] = x;
	c->cmpd.kinds[i] = k;
}


void printOpcode(Opcode opc){
	switch(opc){
		case OP_ADD   : printf("ADD"   ); break;
		case OP_SUB   : printf("SUB"   ); break;
		case OP_MUL   : printf("MUL"   ); break;
		case OP_DIV   : printf("DIV"   ); break;
		case OP_CALL  : printf("CALL"  ); break;
		case OP_RET   : printf("RET"   ); break;
		case OP_VAR   : printf("VAR"   ); break;
		case OP_CONST : printf("CONST" ); break;
	}
}


void printExpr(ExprUnion x, ExprKind k){
	switch(k){
		case XK_EXPR:{
			ExprExpr   xp = x .expr;
			ExprUnion* ps = xp.pars;
			printf("[XP | ");
			for(int i = 0; i < xp.parct; i++){ printExpr(ps[i], xp.kinds[i]); printf(", "); }
			printf("]\n");
		}break;
		
		case XK_CMPD:{
			ExprExpr   xp = x .expr;
			ExprUnion* ps = xp.pars;
			printf("[CP | ");
			for(int i = 0; i < xp.parct; i++){ printExpr(ps[i], xp.kinds[i]); printf(", "); }
			printf("]");
		}break;
		
		case XK_POLY:{
			ExprExpr   xp = x .expr;
			ExprUnion* ps = xp.pars;
			printf("(PL | ");
			for(int i = 0; i < xp.parct; i++){ printExpr(ps[i], xp.kinds[i]); printf(", "); }
			printf(")");
		}break;
		
		case XK_PRIMFUN:{
			PrimExpr p = x.prim;
			printf("FN%lu", p.u64);
		}break;
		
		case XK_PRIMVAR:{
			PrimExpr p = x.prim;
			printf("V%lu", p.u64);
		}break;
		
		case XK_PRIMINT:{
			PrimExpr p = x.prim;
			printf("I%lu", p.u64);
		}break;
		
		case XK_PRIMUNT:{
			PrimExpr p = x.prim;
			printf("U%lu", p.u64);
		}break;
		
		case XK_PRIMFLT:{
			PrimExpr p = x.prim;
			printf("F%f", p.f64);
		}break;
		
		case XK_PRIMSTR:{
			PrimExpr p = x.prim;
			printf("\"%s\"", p.str);
		}break;
		
		case XK_PRIMOPC:{
			PrimExpr p = x.prim;
			printf("OP_");
			printOpcode(p.opc);
		}break;
		
		default: printf("<?>"); break;
	}
}




void print3AddrCode(ThreeAddrCode c){
	printOpcode(c.opc);
	
	printf("%i %i > %i | ", c.a, c.b, c.c);
	
	if(c.parct != 0) printf("Pars = ");
	for(int i = 0; i < c.parct; i++) printf("%i ", c.pars[i]);
	if(c.retct != 0) printf("Rets = ");
	for(int i = 0; i < c.retct; i++) printf("%i ", c.pars[i+c.parct]);
	
	printPrimitive(c.type, 0);
}


void printCodeBlock(CodeBlock blk){

}


void buildExpr(FuncDef* fn, ExprExpr expr){
	ExprUnion* pars = expr.pars;
	ExprUnion  head = pars[0];
	ExprUnion  tail = pars[expr.parct-1];
	for(int i = 1; i < expr.parct-1; i++){
		
	}
}

void buildFunc(FuncDef* fn){
	
}
