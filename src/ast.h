#ifndef __AST_HEADER__
#define __AST_HEADER__


#include "program.h"




typedef struct{
	Position pos;
	int*     arrs;
	int      arct, arcap;
	uint64_t tyid;
}ASTTyElem;

ASTTyElem makeASTTyElem  (int);
void      appendASTTyElem(ASTTyElem*, int);

typedef struct{
	Position  pos;
	void*     elems;
	void*     cnsts;
	uint64_t* vals;
	uint64_t* tags;
	uint64_t  tagId, tagTy;
	int       elct, elcap, cnct, cncap;
}ASTUnion;

typedef struct{
	Position  pos;
	int*      tags;
	void*     cnsts;
	uint64_t* vals;
	uint64_t  tagTy;
	int       tgct, tgcap, cnct, cncap;
}ASTEnum;

typedef struct{
	Position  pos;
	void*     elems;
	void*     cnsts;
	uint64_t* vals;
	int       elct, elcap, cnct, cncap;
}ASTStruct;

typedef enum{
	TT_VOID,
	TT_ELEM,
	TT_ENUM,
	TT_UNON,
	TT_STRC
}ASTTypeType;

typedef struct{
	Position  pos;
	union{
		ASTTyElem elem;
		ASTEnum   enmt;
		ASTUnion  unon;
		ASTStruct strc;
	};
	ASTTypeType type;
}ASTType;

typedef struct{
	Position  pos;
	int       tyid;
	ASTType   tdef;
}ASTTyDef;

typedef struct{
	Position pos;
	int*     ids;
	int      size;
}ASTLoc;


ASTUnion  makeASTUnion   (int);
void      appendASTUnion (ASTUnion*, ASTType, int, uint64_t);
ASTEnum   makeASTEnum    (int);
void      appendASTEnum  (ASTEnum*, int, int);
ASTStruct makeASTStruct  (int);
void      appendASTStruct(ASTStruct*, ASTType, int);



typedef enum{
	XT_VOID,
	XT_WILD,
	XT_INT,
	XT_FLT,
	XT_STR,
	XT_TAG,
	XT_ID,
	XT_MID,
	XT_PAR,
	XT_LOCI,
	XT_LOCT,
	XT_LMDA,
	XT_SWCH,
	XT_IFE,
	XT_FNCL,
	XT_CNST,
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
	Token     name;
	union{
		ASTExpr   expr;
		ASTTyElem type;
	};
}ASTCnst;

void appendASTUnionCnst (ASTUnion*, ASTCnst);
void appendASTEnumCnst  (ASTEnum*, ASTCnst);
void appendASTStructCnst(ASTStruct*, ASTCnst);



typedef struct{
	Position  pos;
	ASTTyElem elem;
	int       label;
}ASTParam;

typedef struct{
	Position   pos;
	ASTTyElem* pars;
	int*       lbls;
	int        prct, fill;
}ASTPars;

ASTPars makeASTPars  (int);
void    appendASTPars(ASTPars*, ASTTyElem, int);

typedef struct{
	Position   pos;
	Token      name;
	ASTExpr*   pars;
	int        prct, prcap;
}ASTCall;

ASTCall makeASTCall  (int);
void    appendASTCall(ASTCall*, ASTExpr);

typedef struct{
	Position   pos;
	uint64_t*  pars;
	int        prct, prcap;
}ASTLmdaPars;

typedef struct{
	Position   pos;
	ASTExpr*   pars;
	int        prct, prcap;
}ASTExprPars;

typedef struct{
	Position pos;
	ASTExpr* rets;
	ASTExpr* exps;
	ASTCnst* cnsts;
	int retct, expct, cnct, retcap, expcap, cncap;
}ASTStmt;


ASTStmt makeASTStmt      (int, int);
void    appendASTStmtExp (ASTStmt*, ASTExpr);
void    appendASTStmtRet (ASTStmt*, ASTExpr);
void    appendASTStmtCnst(ASTStmt*, ASTCnst);

typedef struct{
	Position pos;
	ASTStmt* stmts;
	ASTExpr  retx;
	int      stmct, stcap;
}ASTBlock;

ASTBlock makeASTBlock      (int);
void     appendASTBlockStmt(ASTBlock*, ASTStmt);

typedef struct{
	Position pos;
	ASTPars  pars;
	ASTBlock block;
	int      isProc;
}ASTLambda;

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
