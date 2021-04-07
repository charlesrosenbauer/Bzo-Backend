#ifndef __AST_HEADER__
#define __AST_HEADER__


#include "stdint.h"

#include "bytecode.h"
#include "util.h"



typedef enum{
	PE_OKAY,
	PE_BAD_FN_TYPE,
	PE_BAD_PATTERN,
	PE_INCOMPLETE_PATTERN,
	PE_BAD_CMPD,
	PE_BAD_POLY,
	PE_BAD_EXPR,
	PE_BAD_PRFX,
	PE_BAD_FUNC
}ParseError;


typedef enum{
	PTN_VAR,
	PTN_WILD,
	PTN_FILT,
	PTN_INT,
	PTN_FLT,
	PTN_STR,
	PTN_NIL,
	PTN_FUN,
	PTN_CMPD
}PatternType;

typedef struct{
	union{
		uint64_t UVAL;
		double   FVAL;
		char*    SVAL;
		void*    PVAL;
	};
	uint64_t     variable;
	PatternType  type;
}PatternObj;


typedef struct{
	PatternObj* objs;
	int objct;
}Pattern;


typedef enum{
	EXP_CMPD,
	EXP_POLY,
	EXP_EXPR,
	EXP_LMDA,
	EXP_PRFX,
	EXP_LET,
	EXP_INT,
	EXP_FLT,
	EXP_STR,
	EXP_FUN,
	EXP_NIL,
	EXP_TYP,
	EXP_VAR
}ExprType;

typedef struct{
	void* 		obj;
	ExprType 	type;
}ExprObj;

typedef struct{
	ExprObj*	objs;
	int			ct;
}Expr;

typedef struct{
	Expr		expr;
	int*		rets;
	int 		rct;
}Statement;

typedef struct{
	Expr*	exprs;
	int     ct;
}CmpdExpr;

typedef struct{
	Expr*	exprs;
	int		ct;
}PolyExpr;


typedef struct{
	Pattern		pars;
	Statement*  stmts;
	CmpdExpr	retstmt;
	int			stct;
}Lambda;

typedef struct{
	Statement*	stmts;
	CmpdExpr	retstmt;
	int 		stct;
}LetExpr;


ParseError parseFunction(Lisp*, CodeBlock*, int*);
//ParseError parsePattern (LISP*, CodeBlock*, int*);
ParseError parseExpr    (Lisp*, CodeBlock*, int*);








#endif
