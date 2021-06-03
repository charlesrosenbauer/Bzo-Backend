#ifndef __AST_HEADER__
#define __AST_HEADER__


#include "program.h"


/*
// This won't be sufficient to cover templated types, but we can worry about that later
typedef struct{
	Position pos;
	int      tyid;
	int*     arrs;	// -1 -> ptr, 0 -> gen, n -> size=n
	int      arct;
}ASTTypeElem;

typedef struct{
	Position pos;
	void*    vals;
	int*     labels;
	int      valct;
}ASTStruct;

typedef struct{
	Position pos;
	void*    vals;
	int*     labels;
	int      valct;
}ASTUnion;

typedef struct{
	Position pos;
	void*    pars;
	void*    rets;
	int      parct, retct;
}ASTFuncType;

typedef struct{
	Position pos;
	int      type;
	int      valct;
	int*     vals;
	int64_t* tags;
}ASTEnum;

typedef struct{
	Position  pos;
	void*     vals;
	int*      labels;
	int64_t*  tags;
	int       valct;
	int       tagtype;
	int       tagname;
}ASTTagUnion;

typedef struct{
	Position pos;
	int      bid;
}ASTBuiltin;

typedef enum{
	TT_ELEM,
	TT_STRC,
	TT_UNON,
	TT_FUNC,
	TT_ENUM,
	TT_TGUN,
	TT_BITY
}ASTTypeEnum;

typedef struct{
	union{
		ASTTypeElem		elem;
		ASTStruct		strc;
		ASTUnion		unon;
		ASTFuncType     func;
		ASTEnum         enmt;
		ASTTagUnion     tgun;
		ASTBuiltin      bity;
	}type;
	ASTTypeEnum kind;
}ASTType;

typedef struct{
	Position     pos;
	int*         pars;
	ASTTypeElem* elems;
	int          prct;
}ASTTPars;

typedef struct{
	Position     pos;
	ASTTypeElem* pars;
	int          prct;
}ASTFTPars;

typedef struct{
	Position pos;
	int*     pars;
	ASTType* types;
	int      prct;
}ASTFnPars;

typedef struct{
	Position pos;
	int      flid;
	ASTType  type;
}ASTStructLine;

typedef struct{
	Position pos;
	int      tyid;
	ASTType  type;
}ASTTyDef;










typedef struct{
	Position  pos;
	Operation opc;
	void*     pars;
}ASTOp;

typedef struct{
	int		  func;
	void*     pars;
	int       prct;
}ASTFnCall;

typedef enum{
	LK_INT,
	LK_FLT,
	LK_STR,
	LK_TAG,
	LK_ID,
	LK_MID,
	LK_TID,
	LK_BID
}LitKind;

typedef struct{
	Position pos;
	union{
		uint64_t	i64;
		double		f64;
		void*       ptr;
	};
	LitKind  kind;
}ASTLiteral;

typedef enum{
	XT_VOID,
	XT_LMDA,
	XT_BLOK,
	XT_LTRL,
	XT_PARS,
	XT_PARN,
	XT_BNOP,
	XT_UNOP,
	XT_FNCL,
	XT_ARIX
}ExprType;

typedef struct{
	Position pos;
	ExprType type;
	void*    data;
}ASTExpr;

typedef struct{
	Position pos;

	ASTExpr expr;
	int*    pars;
	int     prct;
}ASTStmt;

typedef struct{
	Position pos;
	ASTStmt* stmts;
	int      stmtct;
}ASTBlock;

typedef struct{
	Position pos;
	int*     pars;
	int      prct;
}ASTPars;

typedef struct{
	Position pos;
	ASTPars  pars;
	ASTBlock blok;
	int      isProc;
}ASTLmda;



typedef struct{
	Position pos;
	int      fnid;
	
	// TODO: Add types
	
	int* pars;
	int  prct;
	
	ASTStmt* stmts;
	int      stct;
	
	ASTExpr  retx;
}ASTFnDef;


typedef struct{
	uint8_t* buffer;
	int      size, fill;
	void*    next;
}AllocatorAST;








*/



















#endif
