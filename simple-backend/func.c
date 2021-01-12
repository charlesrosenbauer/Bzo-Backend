#include "func.h"
#include "alloc.h"

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
	ret.vartypes = malloc(sizeof(Type) * 16);
	ret.tyct     = 0;
	ret.tycap    = 16;
	return ret;
}


CodeBlock makeCodeBlock(int parct, int retct, int opct){
	CodeBlock ret;
	ret.pars     = parct;
	ret.rets     = retct;
	ret.code     = malloc(sizeof(ThreeAddrCode) * opct);
	ret.size     = 0;
	ret.cap      = opct;
	return ret;
}



int addCodeBlockVar(FuncDef* fn, Type t){
	if(fn->tyct+1 >= fn->tycap){
		Type* tmp    = fn->vartypes;
		fn->tycap   *= 2;
		fn->vartypes = malloc(sizeof(Type) * fn->tycap);
		for(int i = 0;        i < fn->tyct;  i++) fn->vartypes[i] = tmp[i];
		for(int i = fn->tyct; i < fn->tycap; i++) fn->vartypes[i] =
			(Type){
				.kind  = TK_VOID,
				.size  = 0,
				.align = 0,
			};
	}
	fn->vartypes[fn->tyct] = t;
	fn->tyct++;
	return fn->tyct-1;
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
		case OP_ADD     : printf("ADD"    ); break;
		case OP_SUB     : printf("SUB"    ); break;
		case OP_MUL     : printf("MUL"    ); break;
		case OP_DIV     : printf("DIV"    ); break;
		case OP_CALL    : printf("CALL"   ); break;
		case OP_RET     : printf("RET"    ); break;
		case OP_VAR     : printf("VAR"    ); break;
		case OP_CONST   : printf("CONST"  ); break;
		case OP_ALLOC   : printf("ALLOC"  ); break;
		case OP_CMPD_LD : printf("CMPD_LD"); break;
		case OP_CMPD_ST : printf("CMPD_ST"); break;
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
	printf("PARS: %i\n", blk.pars);
	printf("RETS: %i\n", blk.rets);
	
	for(int i = 0; i < blk.size; i++)
		print3AddrCode(blk.code[i]);
}


int  connectExpr(FuncDef* fn, ExprUnion a, ExprUnion b, ExprKind ak, ExprKind bk, FuncTable* tab){
	if((ak == XK_CMPD) && (bk == XK_CMPD) && (a.cmpd.parct == b.cmpd.parct)){
		ExprUnion* apars = a.cmpd.pars;
		ExprUnion* bpars = b.cmpd.pars;
		for(int i = 0; i < a.cmpd.parct; i++){
			int err = connectExpr(fn, apars[i], bpars[i], a.cmpd.kinds[i], b.cmpd.kinds[i], tab);
			if(err) return err;
		}
	}else if((ak == XK_PRIMVAR) && (bk == XK_PRIMFUN)){
		printf("f%i ( v%i )\n", b.prim.u64, a.prim.u64);
		// TODO: fill this out more
	}else{
		return -1;
	}
	return 0;
}


int  buildExpr(FuncDef* fn, ExprExpr expr, FuncTable* tab){
	ExprUnion* pars = expr.pars;
	ExprUnion  head = pars[0];
	ExprUnion  tail = pars[expr.parct-1];
	
	Allocator allc = makeAllocator(16384);
	
	ExprUnion* prev = &head;
	ExprKind   pknd = expr.kinds[0];
	for(int i = 1; i < expr.parct; i++){
		printf(">>: ");
		printExpr(*prev, pknd);
		printf(" ==> ");
		printExpr(pars[i], expr.kinds[i]);
		printf("\n");
		switch(expr.kinds[i]){
			case XK_CMPD:{
				if(pknd == XK_CMPD){
					// align cmpds
					if(prev->cmpd.parct == pars[i].cmpd.parct){
						if(connectExpr(fn, *prev, pars[i], pknd, expr.kinds[i], tab)){ freeAlloc(&allc); return -1; }
					}else{
						freeAlloc(&allc);
						return -1;
					}
				}else{
					// break down input
					
				}
			}break;
			
			case XK_PRIMOPC:{
			
			}break;
			
			default: {
				freeAlloc(&allc);
				return -1;
			}
		}
		prev = &pars[i];
		pknd = expr.kinds[i];
	}
	
	freeAlloc(&allc);
	return 0;
}

void buildFunc(FuncDef* fn, FuncTable* tab){
	buildExpr(fn, fn->defn.expr, tab);
}


void      printFunc     (FuncDef fn){
	printf("{\n");
	printf("\nPARS=\n");
	printType(fn.pars);
	printf("\nRETS=\n");
	printType(fn.rets);
	
	printf("\nDEF=\n");
	printExpr(fn.defn, fn.defkind);
	
	printf("\nVARS: %i/%i\n", fn.tyct, fn.tycap);
	for(int i = 0; i < fn.tyct; i++){
		printf("\nV%i type=\n", i);
		printType(fn.vartypes[i]);
	}
	
	printf("\nBLOCKS: %i/%i\n", fn.blockct, fn.blockcap);
	for(int i = 0; i < fn.blockct; i++){
		printf("BLK %i\n", i);
		printCodeBlock(fn.blocks[i]);
	}
	printf("\n}\n");
}
