#ifndef __AST_HEADER__
#define __AST_HEADER__


#include "common.h"


typedef struct{
	Position	pos;
	void*		fields;
	void*		recipe;
	int			fieldct;
}ASTBuild;

typedef enum{
	TA_TYID,
	TA_TVAR,
	TA_TLOC,
	TA_BITY,
	TA_BILD
}TyAtomKind;

typedef struct{
	Position		pos;
	union{
		int64_t		id;
		void*		ptr;
		LocToken	loc;
		ASTBuild*	bld;
	};
	TyAtomKind  	kind;
}ASTTyAtom;


typedef struct{
	Position	pos;
	int64_t*	sizes;
	ASTTyAtom	atom;
	int			dimension;
}ASTTyElem;


typedef struct{
	Position	pos;
	int64_t*	names;
	void*		types;
	int			fieldct;
}ASTStruct;

typedef struct{
	Position	pos;
	int64_t*	names;
	int64_t*	vals;
	void*		types;
	int			fieldct;
}ASTUnion;

typedef struct{
	Position	pos;
	int64_t*	tags;
	int64_t*	vals;
	int			fieldct;
}ASTEnum;


typedef enum{
	TK_ATOM,
	TK_STRC,
	TK_UNON,
	TK_ENUM,
	TK_ELEM
}TyKind;

typedef struct{
	union{
		ASTStruct	strc;
		ASTUnion	unon;
		ASTEnum		enmt;
		ASTTyAtom	atom;
		ASTTyElem	elem;
	};
	TyKind	kind;
}ASTType;



typedef enum{
	XA_ID,
	XA_MID,
	XA_LOC,
	XA_INT,
	XA_FLT,
	XA_STR,
	XA_BID,
	XA_NIL,
	XA_NXP,
	XA_WLD,
	XA_VOID
}ExprAtomKind;

typedef struct{
	Position		pos;
	union{
		int64_t		id;
		void*		ptr;
		uint64_t	u64;
		double		flt;
		LocToken	loc;
		StrToken	str;
	};
	ExprAtomKind	kind;
}ASTExprAtom;


typedef struct{
	Position   pos;
	ASTTyElem* pars;
	int64_t*   lbls;
	int        prct, fill;
}ASTPars;

ASTPars makeASTPars  (int);
void    appendASTPars(ASTPars*, ASTTyElem, int);


typedef enum{
	XK_UNOP,
	XK_BNOP,
	XK_PARN,
	XK_INDX,
	XK_CALL,
	XK_MAKE,
	XK_ATOM,
	XK_COND,
	XK_LMDA
}ExprKind;

typedef struct{
	void* 			a;
	void*			b;
	Token			tk;
}ASTOp;

typedef struct{
	Position		pos;
	union{
		ASTOp		op;
		ASTPars		pars;
		void*		obj;
	};
	ExprKind		kind;
}ASTExpr;

typedef struct{
	Position		pos;
	ASTExpr			cond, expr;
}ASTCase;

typedef struct{
	Position		pos;
	ASTExpr			expr;
	ASTCase*		cases;
	int				csct;
}ASTCond;


typedef struct{
	Position		pos;
	ASTExpr			ty, expr;
}ASTCnst;

typedef struct{
	Position		pos;
	ASTExpr*		rets;
	ASTExpr*		exps;
	int				rtct, xpct;
}ASTStmt;

typedef struct{
	Position		pos;
	ASTStmt*		stmts;
	ASTCnst*		cnsts;
	int				stct, csct;
}ASTBlock;

typedef struct{
	Position		pos;
	ASTPars			pars;
	ASTBlock		blk;
	int				isProc;
}ASTLmda;



/*
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
*/

typedef struct{
	Position  pos;
	int       tyid;
	ASTType   tdef;
}ASTTyDef;

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
