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
	ret.vardefs  = malloc(sizeof(VarDef) * 16);
	ret.vrct     = 0;
	ret.vrcap    = 16;
	return ret;
}


CodeBlock makeCodeBlock(int opct){
	CodeBlock ret;
	ret.code     = malloc(sizeof(ProgramCode) * opct);
	ret.size     = 0;
	ret.cap      = opct;
	return ret;
}


int appendCodeBlock(FuncDef* fn, CodeBlock blk){
	if(fn->blockct + 1 >= fn->blockcap){
		CodeBlock* tmp = fn->blocks;
		fn->blockcap  *= 2;
		fn->blocks     = malloc(sizeof(CodeBlock) * fn->blockcap);
		for(int i = 0; i < fn->blockct; i++) fn->blocks[i] = tmp[i];
	}
	fn->blocks[fn->blockct] = blk;
	fn->blockct++;
	return fn->blockct-1;
}



int addCodeBlockVar(FuncDef* fn, Type t){
	if(fn->vrct+1 >= fn->vrcap){
		Type*   tmp  = fn->vartypes;
		VarDef* dtmp = fn->vardefs;
		fn->vrcap   *= 2;
		fn->vartypes = malloc(sizeof(Type) * fn->vrcap);
		fn->vardefs  = malloc(sizeof(VarDef) * fn->vrcap);
		for(int i = 0;        i < fn->vrct;  i++) fn->vartypes[i] = tmp[i];
		for(int i = fn->vrct; i < fn->vrcap; i++) fn->vartypes[i] =
			(Type){
				.kind  = TK_VOID,
				.size  = 0,
				.align = 0,
			};
		for(int i = 0;        i < fn->vrct;  i++) fn->vardefs[i] = dtmp[i];
	}
	fn->vartypes[fn->vrct] = t;
	fn->vardefs [fn->vrct] = (VarDef){0, 0};
	fn->vrct++;
	return fn->vrct-1;
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

ExprUnion makePrfx(int parct){
	ExprUnion ret;
	ret.prfx.expr  = malloc(sizeof(ExprUnion) + ((sizeof(ExprUnion) + sizeof(ExprKind)) * parct));
	ret.prfx.pars  = ret.prfx.expr + (sizeof(ExprUnion));
	ret.prfx.kinds = ret.prfx.pars + (sizeof(ExprUnion) * parct);
	ret.prfx.parct = parct;
	ret.prfx.xkind = XK_VOID;
	return ret;
}

ExprUnion makeLetx(int expct){
	ExprUnion ret;
	ret.letx.patn  = malloc(sizeof(ExprUnion) + sizeof(ExprUnion) + (sizeof(ExprExpr) * expct));
	ret.letx.retn  = ret.letx.patn + (sizeof(ExprUnion));
	ret.letx.exps  = ret.letx.retn + (sizeof(ExprUnion));
	ret.letx.expct = expct;
	ret.letx.patk  = XK_VOID;
	ret.letx.retk  = XK_VOID;
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
	if(fn->vrct+1 >= fn->vrcap){
		Type* tmp = fn->vartypes;
		fn->vrcap *= 2;
		fn->vartypes = malloc(sizeof(Type) * fn->vrcap);
		for(int i = 0; i < fn->vrct; i++) fn->vartypes[i] = tmp[i];
		free(tmp);
	}
	fn->vartypes[fn->vrct] = t;
	fn->vrct++;
	return fn->vrct-1;
}






void printOpcode(Opcode opc){
	switch(opc){
		case OP_ADD     : printf("ADD"    ); break;
		case OP_SUB     : printf("SUB"    ); break;
		case OP_MUL     : printf("MUL"    ); break;
		case OP_DIV     : printf("DIV"    ); break;
		case OP_NEG     : printf("NEG"    ); break;
		case OP_ABS     : printf("ABS"    ); break;
		
		case OP_LS      : printf("LS"     ); break;
		case OP_GT      : printf("GT"     ); break;
		case OP_LSE     : printf("LSE"    ); break;
		case OP_GTE     : printf("GTE"    ); break;
		case OP_ULS     : printf("ULS"    ); break;
		case OP_UGT     : printf("UGT"    ); break;
		case OP_ULSE    : printf("ULSE"   ); break;
		case OP_UGTE    : printf("UGTE"   ); break;
		case OP_EQ      : printf("EQ"     ); break;
		case OP_NEQ     : printf("NEQ"    ); break;
		
		case OP_AND     : printf("AND"    ); break;
		case OP_OR      : printf("OR"     ); break;
		case OP_XOR     : printf("XOR"    ); break;
		case OP_NOT     : printf("NOT"    ); break;
		case OP_SHL     : printf("SHL"    ); break;
		case OP_SHR     : printf("SHR"    ); break;
		case OP_ROL     : printf("ROL"    ); break;
		case OP_ROR     : printf("ROR"    ); break;
		case OP_PCT     : printf("PCT"    ); break;
		case OP_CTZ     : printf("CTZ"    ); break;
		case OP_CLZ     : printf("CLZ"    ); break;
		
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
		case OP_ITER    : printf("ITER"   ); break;
		
		case OP_ZIP     : printf("ZIP"    ); break;
		case OP_UNZIP   : printf("UNZIP"  ); break;
		case OP_LEN     : printf("LEN"    ); break;
		case OP_STRIDE  : printf("STRIDE" ); break;
		case OP_IX      : printf("IX"     ); break;
		case OP_TAKE    : printf("TAKE"   ); break;
		case OP_DROP    : printf("DROP"   ); break;
		case OP_PROD    : printf("PROD"   ); break;
		
		
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
			printf("]");
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
		
		case XK_PRFX:{
			PrfxExpr   xp = x .prfx;
			ExprUnion* ps = xp.pars;
			printf("[PX | ");
			printExpr(*(ExprUnion*)xp.expr, xp.xkind);
			printf(" < ");
			for(int i = 0; i < xp.parct; i++){ printExpr(ps[i], xp.kinds[i]); printf(", "); }
			printf("]");
		}break;
		
		case XK_LMDA:{
			LetExpr    lx = x .letx;
			printf("{LET |\nPATN=");
			printExpr(*(ExprUnion*)lx.patn, lx.patk);
			printf("\n");
			for(int i = 0; i < lx.expct; i++){ printExpr((ExprUnion)lx.exps[i], XK_EXPR); printf("\n"); }
			printf("RETN=");
			printExpr(*(ExprUnion*)lx.retn, lx.retk);
			printf("\n}\n");
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
		
		case XK_PRIMOPC:{
			PrimExpr p = x.prim;
			printf("OP_");
			printOpcode(p.opc);
		}break;
		
		case XK_PRIMSTR:{
			PrimExpr p = x.prim;
			printf("\"%s\"", x.prim.str);
		}break;
		
		case XK_PRIMWLD:{
			printf("_");
		}break;
		
		default: printf("<%i?>", k); break;
	}
}



int getBiggestVar(ExprUnion x, ExprKind k){
	switch(k){
		case XK_PRIMVAR : return x.prim.i64;
		
		case XK_EXPR : {
			int v = 0;
			ExprUnion* xs = x.expr.pars;
			for(int i = 0; i < x.expr.parct; i++){
				int vx = getBiggestVar(xs[i], x.expr.kinds[i]);
				v = (vx > v)? vx : v;
			}
			return v;
		}break;
		
		case XK_CMPD : {
			int v = 0;
			ExprUnion* xs = x.cmpd.pars;
			for(int i = 0; i < x.cmpd.parct; i++){
				int vx = getBiggestVar(xs[i], x.cmpd.kinds[i]);
				v = (vx > v)? vx : v;
			}
			return v;
		}break;
		
		case XK_POLY : {
			int v = 0;
			ExprUnion* xs = x.poly.pars;
			for(int i = 0; i < x.poly.parct; i++){
				int vx = getBiggestVar(xs[i], x.poly.kinds[i]);
				v = (vx > v)? vx : v;
			}
			return v;
		}break;
		
		case XK_PRFX : {
			int v = getBiggestVar(*(ExprUnion*)x.prfx.expr, x.prfx.xkind);
			ExprUnion* xs = x.prfx.pars;
			for(int i = 0; i < x.prfx.parct; i++){
				int vx = getBiggestVar(xs[i], x.prfx.kinds[i]);
				v = (vx > v)? vx : v;
			}
			return v;
		}break;
		
		case XK_LMDA : {
			int pv = getBiggestVar(*(ExprUnion*)x.letx.patn, x.letx.patk);
			int rv = getBiggestVar(*(ExprUnion*)x.letx.retn, x.letx.retk);
			for(int i = 0; i < x.letx.expct; i++){
				int vx = getBiggestVar((ExprUnion)x.letx.exps[i], XK_EXPR);
				pv = (vx > pv)? vx : pv;
			}
			return (pv > rv)? pv : rv;
		}break;
		
		default: return -1;
	}
}


void printProgCode(ProgramCode c){
	printOpcode(c.opc);
	
	printf(" (%lu) %u %u > %u %u | ", c.f, c.a, c.b, c.q, c.r);
	
	printPrimitive(c.type, 0);
}


void printCodeBlock(CodeBlock blk){
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


int  buildExpr(Program* p, FuncDef* fn, ExprExpr expr, CodeBlock* block){
/*
	ExprUnion* pars = expr.pars;
	ExprUnion  head = pars[0];
	ExprUnion  tail = pars[expr.parct-1];
	
	ExprUnion* prev = &head;
	ExprKind   pknd = expr.kinds[0];
	
	Allocator allc = makeAllocator(16384);
	
	fn->blocks[0] = makeCodeBlock(16);		// Temporary hack
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
	
	freeAlloc(&allc);*/
	return 0;
}

int buildLetx(Program* tab, FuncDef* fn, LetExpr expr, Program* p){
	CodeBlock letbk = makeCodeBlock(32);
	int bid = appendCodeBlock(fn, letbk);

	// TODO: figure out how to handle parameters
	
	for(int i = 0; i < expr.expct; i++){
		// TODO: account for expkind
		int retexp = buildExpr(p, fn, expr.exps[i], &letbk);
		if(retexp < 0) return retexp;
	}
	
	/*
		TODO: figure out how to handle returns.
		My guess is we'll pass in another parameter to buildLetx() that takes a
		return pattern. buildFunc() can then figure that out from the program
		return type, etc. while also allowing us to handle nesting of let
		expressions.
		
		Another approach might be to have buildLetx return the let return
		expression, and have the builder of the parent scope glue it into
		place.
	*/

	return -1;
}



void buildFunc(FuncDef* fn, Program* p){
	int varsize = getBiggestVar(fn->defn, fn->defkind);
	if(fn->vrcap >= varsize){
		Type*   ttmp = fn->vartypes;
		VarDef* vtmp = fn->vardefs;
		fn->vrcap    = varsize + 32;
		fn->vartypes = malloc(sizeof(Type  ) * fn->vrcap);
		fn->vardefs  = malloc(sizeof(VarDef) * fn->vrcap);
		for(int i = 0; i < fn->vrct; i++){ fn->vartypes[i] = ttmp[i]; fn->vardefs[i] = vtmp[i]; }
	}
	buildLetx(p, fn, fn->defn.letx, p);
}


int  buildFuncTable(Program* p){
	for(int i = 0; i < p->funcs.fnct; i++){
		FuncDef* fn = &p->funcs.funcs[i];
		int ipass = calcTypeSize(&p->types, &fn->pars);
		int opass = calcTypeSize(&p->types, &fn->rets);
		if(!(ipass & opass)) return 0;
		buildFunc(fn, p);
	}
	return 1;
}



void      printFunc     (FuncDef fn){
	printf("{\n");
	printf("\nPARS=\n");
	printType(fn.pars, 1);
	printf("\nRETS=\n");
	printType(fn.rets, 1);
	
	printf("\nDEF=\n");
	printExpr(fn.defn, fn.defkind);
	
	printf("\nVARS: %i/%i\n", fn.vrct, fn.vrcap);
	for(int i = 0; i < fn.vrct; i++){
		printf("\nV%i type=\n", i);
		printType(fn.vartypes[i], 1);
	}
	
	printf("\nBLOCKS: %i/%i\n", fn.blockct, fn.blockcap);
	for(int i = 0; i < fn.blockct; i++){
		printf("BLK %i\n", i);
		printCodeBlock(fn.blocks[i]);
	}
	printf("\n}\n");
}
