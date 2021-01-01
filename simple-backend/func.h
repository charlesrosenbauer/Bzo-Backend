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
	OP_DIV,
	OP_CALL,
	OP_RET,
	OP_VAR,
	OP_CONST
}Opcode;

typedef struct{
	Opcode     opc;
	Primitive  type;
	uint16_t   a, b, c;
	uint16_t*  pars;
	uint8_t    parct, retct;
}ThreeAddrCode;


typedef struct{
	int pars, rets;
	Type*          vartypes;
	ThreeAddrCode* code;
	int            size, cap;
}CodeBlock;


typedef struct{
	Type pars, rets;
	
	CodeBlock* blocks;
	int        blockct, blockcap;
}FuncDef;


typedef struct{
	FuncDef* funcs;
	int      fnct;
}FuncTable;


FuncTable makeFuncTable(int);

FuncDef   makeFuncDef  (Type, Type, int);
CodeBlock makeCodeBlock(int, int, int);

ExprUnion makeExpr(int);
ExprUnion makeCmpd(int);
ExprUnion makePoly(int);

void      print3AddrCode(ThreeAddrCode);




#endif
