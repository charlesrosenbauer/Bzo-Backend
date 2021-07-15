#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"

#include "program.h"
#include "util.h"
#include "ast.h"


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



ASTTyElem makeASTTyElem(int size){
	ASTTyElem ret;
	ret.arrs  = malloc(sizeof(int) * size);
	ret.arct  = 0;
	ret.arcap = size;
	return ret;
}

void appendASTTyElem(ASTTyElem* es, int arr){
	if(es->arct + 1 >= es->arcap){
		int* tmp = es->arrs;
		es->arcap *= 2;
		es->arrs = malloc(sizeof(int)       * es->arcap);
		for(int i = 0; i < es->arct; i++) es->arrs[i] = tmp[i];
		free(tmp);
	}
	es->arrs[es->arct] = arr;
	es->arct++;
}


ASTStruct makeASTStruct(int size){
	ASTStruct ret;
	ret.elems = malloc(sizeof(ASTType ) * size);
	ret.vals  = malloc(sizeof(uint64_t) * size);
	ret.elct  = 0;
	ret.elcap = size;
	return ret;
}

void appendASTStruct(ASTStruct* strc, ASTType elem, int val){
	if(strc->elct + 1 >= strc->elcap){
		ASTType*  etmp = strc->elems;
		uint64_t* vtmp = strc->vals;
		strc->elems = malloc(sizeof(ASTType ) * strc->elcap * 2);
		strc->vals  = malloc(sizeof(uint64_t) * strc->elcap * 2);
		ASTType*  elems = strc->elems;
		for(int i = 0; i < strc->elcap; i++){
			elems     [i] = etmp[i];
			strc->vals[i] = vtmp[i];
		}
		strc->elcap *= 2;
		free(etmp);
		free(vtmp);
	}
	ASTType* elems = strc->elems;
	elems     [strc->elct] = elem;
	strc->vals[strc->elct] = val;
	strc->elct++;
}

ASTUnion makeASTUnion(int size){
	ASTUnion ret;
	ret.elems = malloc(sizeof(ASTType ) * size);
	ret.vals  = malloc(sizeof(uint64_t) * size);
	ret.tags  = malloc(sizeof(uint64_t) * size);
	ret.tagId = 0;
	ret.tagTy = 0;
	ret.elct  = 0;
	ret.elcap = size;
	return ret;
}

void appendASTUnion(ASTUnion* unon, ASTType elem, int val, uint64_t tag){
	if(unon->elct + 1 >= unon->elcap){
		ASTType * etmp = unon->elems;
		uint64_t* vtmp = unon->vals;
		uint64_t* ttmp = unon->tags;
		unon->elems = malloc(sizeof(ASTType ) * unon->elcap * 2);
		unon->vals  = malloc(sizeof(uint64_t) * unon->elcap * 2);
		unon->tags  = malloc(sizeof(uint64_t) * unon->elcap * 2);
		ASTType* elems = unon->elems;
		for(int i = 0; i < unon->elcap; i++){
			elems     [i] = etmp[i];
			unon->vals[i] = vtmp[i];
			unon->tags[i] = ttmp[i];
		}
		unon->elcap *= 2;
		free(etmp);
		free(vtmp);
		free(ttmp);
	}
	ASTType* elems = unon->elems;
	elems     [unon->elct] = elem;
	unon->vals[unon->elct] = val;
	unon->tags[unon->elct] = tag;
	unon->elct++;
}

ASTEnum makeASTEnum(int size){
	ASTEnum ret;
	ret.tags  = malloc(sizeof(int     ) * size);
	ret.vals  = malloc(sizeof(uint64_t) * size);
	ret.tgct  = 0;
	ret.tgcap = size;
	return ret;
}

void appendASTEnum(ASTEnum* enmt, int val, int tag){
	if(enmt->tgct + 1 >= enmt->tgcap){
		int     * ttmp = enmt->tags;
		uint64_t* vtmp = enmt->vals;
		enmt->tags = malloc(sizeof(int     ) * enmt->tgcap * 2);
		enmt->vals = malloc(sizeof(uint64_t) * enmt->tgcap * 2);
		for(int i = 0; i < enmt->tgcap; i++){
			enmt->tags[i] = ttmp[i];
			enmt->vals[i] = vtmp[i];
		}
		enmt->tgcap *= 2;
		free(ttmp);
		free(vtmp);
	}
	enmt->tags[enmt->tgct] = tag;
	enmt->vals[enmt->tgct] = val;
	enmt->tgct++;
}


ASTStmt makeASTStmt(int exps, int rets){
	ASTStmt ret;
	ret.rets   = malloc(sizeof(ASTExpr) * exps);
	ret.exps   = malloc(sizeof(ASTExpr) * rets);
	ret.expcap = exps;
	ret.retcap = rets;
	ret.expct  = 0;
	ret.retct  = 0;
	return ret;
}


void appendASTStmtExp(ASTStmt* stmt, ASTExpr xp){
	if(stmt->expct + 1 >= stmt->expcap){
		ASTExpr* tmp  = stmt->exps;
		stmt->exps    = malloc(sizeof(ASTExpr) * stmt->expcap);
		for(int i = 0; i < stmt->expcap; i++) stmt->exps[i] = tmp[i];
		stmt->expcap *= 2;
		free(tmp);
	}
	stmt->exps[stmt->expct] = xp;
	stmt->expct++;
}


void appendASTStmtRet(ASTStmt* stmt, ASTExpr rt){
	if(stmt->retct + 1 >= stmt->retcap){
		ASTExpr* tmp  = stmt->rets;
		stmt->rets    = malloc(sizeof(ASTExpr) * stmt->retcap);
		for(int i = 0; i < stmt->retcap; i++) stmt->rets[i] = tmp[i];
		stmt->retcap *= 2;
		free(tmp);
	}
	stmt->rets[stmt->retct] = rt;
	stmt->retct++;
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



void printASTType(ASTType, int);

void printASTEnum(ASTEnum enm, int pad){
	leftpad(pad);
	printf(" [ENUM %lu: ", enm.tagTy);
	for(int i = 0; i <  enm.tgct; i++) printf("%i = %li, ", enm.tags[i], enm.vals[i]);
	printf("] ");
}

void printASTUnion(ASTUnion unon, int pad){
	leftpad(pad);
	printf(" [UNION %lu %lu:\n", unon.tagTy, unon.tagId);
	ASTType* types = unon.elems;
	for(int i = 0; i < unon.elct; i++){
		leftpad(pad);
		printf("  %li = %li ", unon.vals[i], unon.tags[i]);
		printASTType(types[i], pad+1);
		printf("\n");
	}
	leftpad(pad);
	printf("] ");
}

void printASTStruct(ASTStruct strc, int pad){
	leftpad(pad);
	printf(" [STRUCT: %i\n", strc.elct);
	ASTType* types = strc.elems;
	for(int i = 0; i < strc.elct; i++){
		leftpad(pad);
		printf("  %i| %li :\n", i, strc.vals[i]);
		printASTType(types[i], pad+1);
		printf("\n");
	}
	leftpad(pad);
	printf("] ");
}

void printASTTyElem(ASTTyElem elem, int pad){
	leftpad(pad);
	printf(" [ELEM: ");
	for(int i = 0; i < elem.arct; i++){
		if(elem.arrs[i] == 0){
			printf("[]");
		}else if(elem.arrs[i] == -1){
			printf("^");
		}else{
			printf("[%i]", elem.arrs[i]);
		}
	}
	printf(" T=%lu] ", elem.tyid);
}

void printASTType(ASTType ty, int pad){
	leftpad(pad);
	printf(" [TYPE:\n");
	switch(ty.type){
		case TT_VOID: printf(" VOIDTY ");             break;
		case TT_ELEM: printASTTyElem(ty.elem, pad+1); break;
		case TT_STRC: printASTStruct(ty.strc, pad+1); break;
		case TT_UNON: printASTUnion (ty.unon, pad+1); break;
		case TT_ENUM: printASTEnum  (ty.enmt, pad+1); break;
		default:      printf(" ??TY%i?? ", ty.type ); break;
	}
	leftpad(pad);
	printf("]\n");
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
	for(int i = 0; i < prog.tyct; i++){
		printf("  TY%i | %i\n", i, prog.tys[i].tyid);
		printASTType(prog.tys[i].tdef, 1);
		printf("\n");
	}
}
