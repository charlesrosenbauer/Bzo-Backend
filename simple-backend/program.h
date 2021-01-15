#ifndef __PROGRAM_HEADER__
#define __PROGRAM_HEADER__


#include "stdint.h"



typedef enum{
	TK_STRUCT,
	TK_UNION,
	TK_NAMED,
	TK_ARRAY,
	TK_PRIMITIVE,
	TK_FUNCTION,
	TK_VOID
}TypeKind;

typedef struct{
	void*     pars;
	TypeKind* kinds;
	int*      offsets;
	int       parct, size, align;
}Struct;

typedef struct{
	void*     pars;
	TypeKind* kinds;
	int       parct, size, align;
}Union;

typedef struct{
	uint64_t  tyid;
}NamedType;

typedef struct{
	void*     val;
	TypeKind  kind;
	int       count, stride, size, align;
}Array;

typedef struct{
	void*     ios;
	TypeKind  ikind, okind;
}FuncType;

typedef enum{
	P_I8,
	P_I16,
	P_I32,
	P_I64,
	P_U8,
	P_U16,
	P_U32,
	P_U64,
	P_F16,
	P_F32,
	P_F64,
	P_Ptr
}Primitive;

typedef union{
	Struct    strc;
	Primitive prim;
	Union     unon;
	NamedType name;
	Array     arry;
	FuncType  func;
}TypeUnion;

typedef struct{
	TypeUnion type;
	TypeKind  kind;
	int size, align, isAlias;
}Type;


typedef struct{
	Type*  types;
	int    tyct;
}TypeTable;






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
	OP_ADD     = 0x00,
	OP_SUB     = 0x01,
	OP_MUL     = 0x02,
	OP_DIV     = 0x03,
	OP_AND     = 0x04,
	OP_OR      = 0x05,
	OP_XOR     = 0x06,
	OP_NOT     = 0x07,
	OP_SHL     = 0x08,
	OP_SHR     = 0x09,
	OP_NEG     = 0x0A,
	OP_ABS     = 0x0B,
	OP_CALL    = 0x20,
	OP_RET     = 0x21,
	OP_VAR     = 0x22,
	OP_CONST   = 0x23,
	OP_ALLOC   = 0x24,
	OP_CMPD_LD = 0x25,
	OP_CMPD_ST = 0x26
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
	
	Type*      vartypes;
	int        tyct, tycap;
	
	CodeBlock* blocks;
	int        blockct, blockcap;
	
	int		   isAlias;
}FuncDef;


typedef struct{
	FuncDef* funcs;
	int      fnct;
}FuncTable;




typedef struct{
	TypeTable types;
	FuncTable funcs;
}Program;



#endif
