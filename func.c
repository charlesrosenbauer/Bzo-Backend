#include "func.h"
#include "alloc.h"

#include "stdlib.h"
#include "stdio.h"




FuncTable makeFuncTable(int size, int cap){
	size = (size > cap)? cap : size;
	FuncTable ret;
	ret.funcs = malloc(sizeof(FuncDef) * cap);
	ret.fnct  = size;
	ret.fncap = cap;
	return ret;
}

void resizeFnTable(FuncTable* tab, int size){
	if(tab->fncap < size){
		FuncDef* tmp  = tab->funcs;
		tab->funcs    = malloc(sizeof(FuncDef) * size * 2);
		for(int i     = 0; i < tab->fnct; i++) tab->funcs[i] = tmp[i];
		free(tmp);
		tab->fncap = size * 2;
	}
	tab->fnct = (tab->fnct < size)? size : tab->fnct;
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
	ret.code     = malloc(sizeof(ProgramCode) * opct);
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



void appendToBlock(CodeBlock* blk, ProgramCode opc){
	if(blk->size+1 >= blk->cap){
		ProgramCode* tmp = blk->code;
		blk->cap *= 2;
		blk->code = malloc(sizeof(ProgramCode) * blk->cap);
		for(int i = 0; i < blk->size; i++) blk->code[i] = tmp[i];
		free(tmp);
	}
	blk->code[blk->size] = opc;
	blk->size++;
}



int  makeVar(FuncDef* fn, Type t){
	if(fn->tyct+1 >= fn->tycap){
		Type* tmp = fn->vartypes;
		fn->tycap *= 2;
		fn->vartypes = malloc(sizeof(Type) * fn->tycap);
		for(int i = 0; i < fn->tyct; i++) fn->vartypes[i] = tmp[i];
		free(tmp);
	}
	fn->vartypes[fn->tyct] = t;
	fn->tyct++;
	return fn->tyct-1;
}






void printOpcode(Opcode opc){
	switch(opc){
		case OP_ADD     : printf("ADD"    ); break;
		case OP_SUB     : printf("SUB"    ); break;
		case OP_MUL     : printf("MUL"    ); break;
		case OP_DIV     : printf("DIV"    ); break;
		case OP_AND     : printf("AND"    ); break;
		case OP_OR      : printf("OR"     ); break;
		case OP_XOR     : printf("XOR"    ); break;
		case OP_NOT     : printf("NOT"    ); break;
		case OP_SHL     : printf("SHL"    ); break;
		case OP_SHR     : printf("SHR"    ); break;
		case OP_NEG     : printf("NEG"    ); break;
		case OP_ABS     : printf("ABS"    ); break;
		
		case OP_CALL    : printf("CALL"   ); break;
		case OP_RET     : printf("RET"    ); break;
		case OP_VAR     : printf("VAR"    ); break;
		case OP_CONST   : printf("CONST"  ); break;
		case OP_ALLOC   : printf("ALLOC"  ); break;
		case OP_CMPD_LD : printf("CMPD_LD"); break;
		case OP_CMPD_ST : printf("CMPD_ST"); break;
		case OP_IFE     : printf("IFE"    ); break;
		case OP_ELSE    : printf("ELSE"   ); break;
		case OP_SYSCALL : printf("SYSCALL"); break;
		case OP_STCPY   : printf("STCPY"  ); break;
		
		case OP_MAP     : printf("MAP"    ); break;
		case OP_FOLD    : printf("FOLD"   ); break;
		case OP_SCAN    : printf("SCAN"   ); break;
		case OP_FILTER  : printf("FILTER" ); break;
		case OP_ZIP     : printf("ZIP"    ); break;
		case OP_UNZIP   : printf("UNZIP"  ); break;
		case OP_ITER    : printf("ITER"   ); break;
		case OP_ITERN   : printf("ITERN"  ); break;
		default: printf("Unknown opcode %i", opc); break;
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




void printProgCode(ProgramCode c){
	printOpcode(c.opc);
	
	printf(" (%lu) %u %u > %u %u | ", c.f, c.a, c.b, c.q, c.r);
	
	printPrimitive(c.type, 0);
}


void printCodeBlock(CodeBlock blk){
	printf("PARS: %i\n", blk.pars);
	printf("RETS: %i\n", blk.rets);
	
	for(int i = 0; i < blk.size; i++)
		printProgCode(blk.code[i]);
}


int  connectExpr(FuncDef* fn, CodeBlock* blk, ExprUnion a, ExprUnion b, ExprKind ak, ExprKind bk, FuncTable* tab){
	if((ak == XK_CMPD) && (bk == XK_CMPD) && (a.cmpd.parct == b.cmpd.parct)){
		ExprUnion* apars = a.cmpd.pars;
		ExprUnion* bpars = b.cmpd.pars;
		for(int i = 0; i < a.cmpd.parct; i++){
			int err = connectExpr(fn, blk, apars[i], bpars[i], a.cmpd.kinds[i], b.cmpd.kinds[i], tab);
			if(err) return err;
		}
	}else if((ak == XK_PRIMVAR) && (bk == XK_PRIMFUN)){
		printf("f%lu ( v%lu )\n", b.prim.u64, a.prim.u64);
		ProgramCode fncall = (ProgramCode){OP_CALL, P_Ptr, 0, a.prim.u64, 0, makeVar(fn, tab->funcs[b.prim.u64].rets), b.prim.u64};
		appendToBlock(blk, fncall);
	}else if((ak == XK_CMPD) && (bk == XK_PRIMOPC)){
		printf("cmpd %i -> ", a.cmpd.parct);
		printOpcode(b.prim.opc);
		printf("\n");
	}else{
		return -1;
	}
	return 0;
}


int  buildExpr(FuncDef* fn, ExprExpr expr, FuncTable* tab){
	ExprUnion* pars = expr.pars;
	ExprUnion  head = pars[0];
	ExprUnion  tail = pars[expr.parct-1];
	
	ExprUnion* prev = &head;
	ExprKind   pknd = expr.kinds[0];
	
	Allocator allc = makeAllocator(16384);
	
	fn->blocks[0] = makeCodeBlock(2, 1, 16);		// Temporary hack
	fn->blockct   = 1;
	
	for(int i = 1; i < expr.parct; i++){
		printf(">>: ");
		printExpr(*prev, pknd);
		printf(" ==> ");
		printExpr(pars[i], expr.kinds[i]);
		printf("\n");
		if(connectExpr(fn, &fn->blocks[0], *prev, pars[i], pknd, expr.kinds[i], tab)){
			freeAlloc(&allc);
			return -1;
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
