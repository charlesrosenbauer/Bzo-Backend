#ifndef __AST_HEADER__
#define __AST_HEADER__


#include "common.h"


/*
	TODO:
	Decor AST will be a separate AST specifically for position tracking, errors, and other
	debug-oriented features. This is to avoid clogging up the main AST with mostly irrelevant
	data. This should help with performance, while also making it easier to maintain a nice
	representation of debug symbols.
*/
typedef struct{
	void*	fields;
	int		fieldct;
}DAST_Struct;

typedef struct{
	void*   tagId;
	void*   tagTy;
	void*	fields;
	int		fieldct;
}DAST_Union;

typedef struct{
	void*   tag;
	void*   fields;
	int     fieldct;
}DAST_Enum;



typedef enum{
	DA_STRC,
	DA_UNON,
	DA_ENUM,
	DA_EROR
}DecorASTType;

typedef struct{
	Position        pos;
	union{
		DAST_Struct strc;
		DAST_Union  unon;
		DAST_Enum   enmt;
		Error       eror;
	};
	DecorASTType    type;
}DecorAST;







typedef struct{
	Position	pos;
	List		fields;	// ASTTyElem
	void*		recipe; // ASTTyElem
}ASTBuild;

typedef struct{
	Position	pos;
	List		tpars;	// ASTTyElem
	List		pars;	// ASTTyElem
	List		rets;	// ASTTyElem
}ASTFnTy;

typedef enum{
	TA_TYID,
	TA_TVAR,
	TA_TLOC,
	TA_BITY,
	TA_BILD,
	TA_FNTY,
	TA_VOID
}TyAtomKind;

typedef struct{
	Position		pos;
	union{
		int64_t		id;
		void*		ptr;
		LocToken	loc;
		ASTBuild	bld;
		ASTFnTy		fty;
	};
	TyAtomKind  	kind;
}ASTTyAtom;


typedef struct{
	Position	pos;
	List		sizes;	// int64_t
	ASTTyAtom	atom;
}ASTTyElem;


typedef struct{
	Position	pos;
	List		names;	// int64_t
	List		types;	// ASTType
	List		cnsts;	// ASTCnst
}ASTStruct;

typedef struct{
	Position	pos;
	List		names;	// int64_t
	List		vals;	// int64_t
	List		types;	// ASTType
	List		cnsts;	// ASTCnst
	int64_t		tagty, tagid;
}ASTUnion;

typedef struct{
	Position	pos;
	List		tags;	// int64_t
	List		vals;	// int64_t
	List		cnsts;	// ASTCnst
	int64_t		tagty;
}ASTEnum;


typedef enum{
	TK_ATOM,
	TK_STRC,
	TK_UNON,
	TK_ENUM,
	TK_ELEM
}TyKind;

typedef struct{
	Position pos;
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
	List	   pars;	// ASTTyElem
	List	   lbls;	// int64_t
}ASTPars;

ASTPars makeASTPars  (int);


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
	List			cases;	// ASTCase
}ASTCond;


typedef struct{
	Position		pos;
	ASTExpr			ty, expr;
}ASTCnst;

typedef struct{
	Position		pos;
	List			rets;	// ASTExpr
	List			exps;	// ASTExpr
}ASTStmt;

typedef struct{
	Position		pos;
	List			stmts;	// ASTStmt
	List			cnsts;	// ASTCnst
}ASTBlock;

typedef struct{
	Position		pos;
	ASTPars			pars;
	ASTBlock		blk;
	int				isProc;
}ASTLmda;


typedef struct{
	Position  pos;
	int64_t   tyid;
	ASTPars   tprs;
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
	int64_t  sym;
}ASTHeader;


typedef struct{
	List	hds;	// ASTHeader
	List	fns;	// ASTFnDef
	List	tys;	// ASTTyDef
	List    cns;	// ASTCnst
}ASTProgram;


ASTProgram makeASTProgram(int);





ASTProgram makeASTProgram (int);
int        parseCode      (LexerState*, SymbolTable*, ASTProgram*, ErrorList*);
void       printASTType   (ASTType);
void       printASTProgram(ASTProgram);











#endif
