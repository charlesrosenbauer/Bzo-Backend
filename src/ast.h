#ifndef __AST_HEADER__
#define __AST_HEADER__


#include "program.h"




typedef struct{
	Position pos;
	int*     arrs;
	int      arct, arcap;
	uint64_t tyid;
}ASTTyElem;

typedef struct{
	Position  pos;
	void*     elems;
	uint64_t* vals;
	int       elct, elcap;
}ASTUnion;

typedef struct{
	Position  pos;
	int*      tags;
	uint64_t* vals;
	int       tgct, tgcap;
}ASTEnum;

typedef struct{
	Position  pos;
	void*     elems;
	int       elct, elcap;
}ASTStruct;

typedef enum{
	TT_ELEM,
	TT_UNON,
	TT_STRC
}ASTTypeType;

typedef struct{
	Position  pos;
	union{
		ASTTyElem elem;
		ASTUnion  unon;
		ASTStruct strc;
	};
	ASTTypeType type;
}ASTType;

typedef struct{
	Position  pos;
	int       tyid;
}ASTTyDef;



typedef struct{
	Position pos;
	int*     ids;
	int      size;
}ASTLoc;



typedef enum{
	XT_INT,
	XT_FLT,
	XT_STR,
	XT_TAG,
	XT_ID,
	XT_MID,
	XT_PAR,
	XT_LOC,
	XT_LMDA,
	XT_SWCH,
	XT_IFE,
	XT_FNCL,
	XT_MK,
	XT_BOP,
	XT_UOP,
	XT_IX
}ExprType;

typedef struct{
	Position pos;
	void*         a;
	void*         b;
	union{
		Token     tk;
		ASTLoc    fd;
		void*     xp;
	};
	ExprType type;
}ASTExpr;

typedef struct{
	Position  pos;
	ASTExpr** pars;
	int       prct;
}ASTPars;

typedef struct{
	Position pos;
	ASTPars  rets;
	ASTExpr  expr;
}ASTStmt;

typedef struct{
	Position pos;
	ASTStmt* stmts;
	ASTExpr  retx;
	int      stmct;
}ASTBlock;

typedef struct{
	Position pos;
	ASTPars  pars;
	ASTBlock block;
	int      isProc;
}ASTLmda;

typedef struct{
	Position pos;
	int      fnid;
	ASTPars  tvrs, pars, rets;
	ASTBlock def;
}ASTFnDef;


typedef struct{
	Position pos;
	int      bid;
	StrToken str;
}ASTHeader;


typedef struct{
	ASTHeader* hds;
	ASTFnDef*  fns;
	ASTTyDef*  tys;
	int fnct, tyct, hdct, fncap, tycap, hdcap;
	
	//AllocatorAST alloc;
}ASTProgram;


ASTProgram makeASTProgram(int);
void       appendFnDef   (ASTProgram*, ASTFnDef);
void       appendTyDef   (ASTProgram*, ASTTyDef);
void       appendHeader  (ASTProgram*, ASTHeader);





ASTProgram makeASTProgram (int);
int        parseCode      (LexerState*, SymbolTable*, ASTProgram*, ErrorList*);
void       printASTType   (ASTType, int);
void       printASTProgram(ASTProgram);



















#endif
