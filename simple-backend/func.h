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
	XK_PRIMVAR,
	XK_PRIMOPC
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


typedef enum{
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_CALL,
	OP_RET,
	OP_VAR,
	OP_CONST,
	OP_ALLOC,
	OP_CMPD_LD,
	OP_CMPD_ST
}Opcode;

typedef union{
	int64_t   i64;
	uint64_t  u64;
	float     f32;
	double    f64;
	char*     str;
	int       opc;
}PrimExpr;

typedef union{
	PrimExpr prim;
	CmpdExpr cmpd;
	PolyExpr poly;
	ExprExpr expr;
}ExprUnion;


typedef struct{
	Opcode     opc;
	Primitive  type;
	uint64_t   a, b, c;
}ThreeAddrCode;


typedef struct{
	int pars, rets;
	
	ThreeAddrCode* code;
	int            size, cap;
}CodeBlock;


typedef struct{
	Type pars, rets;
	
	ExprUnion  defn;
	ExprKind   defkind;
	
	Type*          vartypes;
	int            tyct, tycap;
	
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

void      appendToBlock(CodeBlock*, ThreeAddrCode);
int       makeVar      (FuncDef*, Type);

void      setIx   (ExprUnion*, ExprUnion, ExprKind, int);

void      printExpr     (ExprUnion, ExprKind);
void      print3AddrCode(ThreeAddrCode);
void      printCodeBlock(CodeBlock);
void      printFunc     (FuncDef);

void      buildFunc(FuncDef*, FuncTable*);




#endif
