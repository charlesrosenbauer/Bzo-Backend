#ifndef __FUNC_HEADER__
#define __FUNC_HEADER__

#include "type.h"

#include "stdint.h"


typedef enum{
	XK_CMPD,
	XK_POLY,
	XK_EXPR,
	XK_LMDA,
	XK_PRFX,
	XK_PRIMINT,
	XK_PRIMUNT,
	XK_PRIMSTR,
	XK_PRIMFLT,
	XK_PRIMFUN,
	XK_PRIMVAR
}ExprKind;

typedef struct{
	void*     pars;
	ExprKind* kinds;
	int       parct;
}CmpdExpr;

typedef struct{
	void*     pars;
	ExprKind* kinds;
	int       parct;
}ExprExpr;

typedef struct{
	void*     pars;
	ExprKind* kinds;
	int       parct;
}PolyExpr;

typedef union{
	int64_t   i64;
	uint64_t  u64;
	float     f32;
	double    f64;
	char*     str;
}PrimExpr;

typedef union{
	PrimExpr prim;
	CmpdExpr cmpd;
	PolyExpr poly;
	ExprExpr expr;
}ExprUnion;


typedef enum{
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV
}Opcode;


typedef struct{
	Type pars, rets;
	
	
}FuncDef;


ExprUnion makeExpr(int);
ExprUnion makeCmpd(int);
ExprUnion makePoly(int);




#endif
