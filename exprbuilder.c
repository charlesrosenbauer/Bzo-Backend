#include "program.h"
#include "func.h"
#include "type.h"

#include "stdint.h"
#include "stdlib.h"


/*
	Current plan here is to convert statements into a graph where it will be
	easier to apply some simple reductions, then convert it into bytecode.
	
	This also means we can be a lot messier with data layout here, as this
	will be a short-lived data structure that will be converted into a longer-
	lived bytecode format with a more cache-friendly layout.
*/

typedef enum{
	NK_STRC,
	NK_POLY,
	NK_SELM,
	NK_GELM,
	NK_CALL,
	NK_BRNX
}NodeKind;

typedef struct{
	NodeKind kind;
	int      a, b, c;
}ExprNode;


typedef struct{
	ExprNode* exps;
	int       size, cap;
}Graph;


int allocNode(Graph* g){
	if(g->size+1 >= g->cap){
		ExprNode* tmp = g->exps;
		g->exps = malloc(sizeof(ExprNode) * g->cap * 2);
		for(int i = 0; i < g->size; i++) g->exps[i] = tmp[i];
		g->cap *= 2;
		free(tmp);
	}
	g->size++;
	return g->size-1;
}


int reduceGraph(ExprNode* graph){

}


int buildTaillessExpr(Program* p, FuncDef* fn, ExprExpr* expr){

}

int buildExpr(Program* p, FuncDef* fn, ExprExpr* expr){
	
}


