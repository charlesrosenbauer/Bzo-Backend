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
	int    tyct, tycap;
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
	XK_PRIMOPC,
	XK_PRIMWLD,
	XK_VOID
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

typedef struct{
	void*     expr;
	void*     pars;
	ExprKind* kinds;
	ExprKind  xkind;
	int       parct;
}PrfxExpr;

typedef struct{
	void*      patn;
	ExprExpr*  exps;
	void*      retn;
	ExprKind   patk, retk;
	int        expct;
}LetExpr;


typedef enum{
	OP_NOP     = 0x00,
	
	// Arithmetic
	OP_ADD     = 0x01,
	OP_SUB     = 0x02,
	OP_MUL     = 0x03,
	OP_DIV     = 0x04,
	OP_ABS     = 0x05,
	OP_NEG     = 0x06,
	
	// Comparison
	OP_LS      = 0x20,
	OP_GT      = 0x21,
	OP_LSE     = 0x22,
	OP_GTE     = 0x23,
	OP_ULS     = 0x24,
	OP_UGT     = 0x25,
	OP_ULSE    = 0x26,
	OP_UGTE    = 0x27,
	OP_EQ      = 0x28,
	OP_NEQ     = 0x29,
	
	// Bitwise
	OP_AND     = 0x40,
	OP_OR      = 0x41,
	OP_XOR     = 0x42,
	OP_NOT     = 0x43,
	OP_SHL     = 0x44,
	OP_SHR     = 0x45,
	OP_ROL     = 0x46,
	OP_ROR     = 0x47,
	OP_PCT     = 0x48,
	OP_CTZ     = 0x49,
	OP_CLZ     = 0x4A,
	
	OP_MAP     = 0x60,
	OP_FOLD    = 0x61,
	OP_SCAN    = 0x62,
	OP_FILTER  = 0x63,
	OP_ITER    = 0x64,
	
	OP_ZIP     = 0xA0,
	OP_UNZIP   = 0xA1,
	OP_LEN     = 0xA2,
	OP_STRIDE  = 0xA3,
	OP_IX      = 0xA4,
	OP_TAKE    = 0xA5,
	OP_DROP    = 0xA6,
	OP_PROD    = 0xA7,
	
	OP_CALL    = 0x80,
	OP_RET     = 0x81,
	OP_VAR     = 0x82,
	OP_CONST   = 0x83,
	OP_ALLOC   = 0x84,
	OP_CMPD_LD = 0x85,
	OP_CMPD_ST = 0x86,
	OP_IFE     = 0x87,
	OP_ELSE    = 0x88,
	OP_SYSCALL = 0x89,
	OP_STCPY   = 0x8A
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
	PrfxExpr prfx;
	LetExpr  letx;
}ExprUnion;


typedef struct{
	Opcode     opc;
	Primitive  type;
	uint16_t   a, b, q, r;
	uint64_t   f;
}ProgramCode;


typedef struct{
	ProgramCode* code;
	int          size, cap;
}CodeBlock;

typedef struct{
	int blockix, opix;
}VarDef;


typedef struct{
	Type pars, rets;
	
	ExprUnion  defn;
	ExprKind   defkind;
	
	Type*      vartypes;
	VarDef*    vardefs;
	int        vrct, vrcap;
	
	CodeBlock* blocks;
	int        blockct, blockcap;
	
	int		   isAlias;
}FuncDef;


typedef struct{
	FuncDef* funcs;
	int      fnct, fncap;
}FuncTable;




typedef struct{
	TypeTable types;
	FuncTable funcs;
}Program;



#endif
