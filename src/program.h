#ifndef __PROGRAM_HEADER__
#define __PROGRAM_HEADER__


#include "stdint.h"



#include "stdint.h"

#include "util.h"
#include "error.h"


/*
	Symbol Table
*/
typedef struct{
	char*    text;
	uint64_t hash;
	int      id;
}Symbol;

typedef struct{
	Symbol* syms;
	int     size, fill, idct;
}SymbolTable;

void        printSymbolTable(SymbolTable);
Symbol      searchSymbol    (SymbolTable*, Symbol);
int         insertSymbolText(SymbolTable*, char*);
int         insertSymbol    (SymbolTable*, Symbol);
SymbolTable makeSymbolTable (int);




/*
	Builtin Types
*/
typedef enum{
	BID_POPCOUNT32		= 0x1001,
	BID_POPCOUNT64		= 0x1002,
	
	BID_I8              = 0x2000,
	BID_I16             = 0x2001,
	BID_I32				= 0x2002,
	BID_I64				= 0x2003,
	BID_U8				= 0x2004,
	BID_U16				= 0x2005,
	BID_U32				= 0x2006,
	BID_U64				= 0x2007,
	BID_BOOL			= 0x2008,
	BID_F16				= 0x2009,
	BID_F32				= 0x200A,
	BID_F64				= 0x200B,
	BID_MAX_TYPE		= 0x2FFF,

	BID_STRUCT          = 0x3000,
	BID_UNION			= 0x3001,

	BID_IMPORT			= 0x4000
}BuiltinId;

int isTypeBID(BuiltinId);



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
}ASTEnum;

typedef struct{
	Position  pos;
	void*     vals;
	int*      labels;
	uint64_t* states;
	int       valct;
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
	Position pos;
	int      tyid;
	ASTType  type;
}ASTTyDef;








typedef enum{
	// Binops
	BTM_BINOP	= 0x00,
	
	OPR_ADD		= 0x00,		// +
	OPR_SUB		= 0x01,		// -
	OPR_MUL		= 0x02,		// *
	OPR_DIV		= 0x03,		// /
	OPR_MOD 	= 0x04,		// %
	OPR_EXP 	= 0x05,		// ^
	OPR_AND 	= 0x06,		// &
	OPR_OR		= 0x07,		// |
	OPR_XOR 	= 0x08,		// ^^
	OPR_LS 		= 0x09,		// <
	OPR_GT		= 0x0A,		// >
	OPR_LSE		= 0x0B,		// =<
	OPR_GTE		= 0x0C,		// >=
	OPR_EQ		= 0x0D,		// =
	OPR_NEQ		= 0x0E,		// !=
	OPR_IX		= 0x0F,		// [i]
	
	TOP_BINOP   = 0x0F,
	
	// Unops
	BTM_UNOP	= 0x20,
	
	OPR_NEG		= 0x20,		// -
	OPR_NOT		= 0x21,		// !
	OPR_DREF	= 0x22,		// <-
	OPR_PTR		= 0x23,		// ^
	
	TOP_UNOP	= 0x23
}Operation;

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
	XT_LMDA,
	XT_BLOK,
	XT_LTRL,
	XT_PARS,
	XT_PARN,
	XT_BNOP,
	XT_UNOP,
	XT_FNCL
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

typedef struct{
	ASTFnDef* fns;
	ASTTyDef* tys;
	int fnct, tyct, fncap, tycap;
	
	AllocatorAST alloc;
}ASTProgram;







typedef struct{
	char* text;
	int   size, head, line, column, fileId;
}LangReader;



typedef enum{
	TKN_PERIOD,		// .
	TKN_COLON,		// :
	TKN_SEMICOLON,	// ;
	TKN_COMMA,		// ,
	TKN_BRK_OPN,	// [
	TKN_BRK_END,	// ]
	TKN_PAR_OPN,	// (
	TKN_PAR_END,	// )
	TKN_BRC_OPN,	// {
	TKN_BRC_END,	// }
	TKN_COMMS,		// #{ .. #}
	TKN_COMMENT,	// #:
	TKN_ASSIGN,		// :=
	TKN_DEFINE,		// ::
	TKN_L_ARROW,	// <-
	TKN_R_ARROW,	// ->
	TKN_L_DARROW,   // <=
	TKN_R_DARROW,   // =>
	TKN_EQL,		// =
	TKN_NEQ,		// !=
	TKN_ADD,		// +
	TKN_SUB,		// -
	TKN_MUL,		// *
	TKN_DIV,		// /
	TKN_MOD,		// %
	TKN_EXP,		// ^
	TKN_AND,		// &
	TKN_OR,			// |
	TKN_NOT,		// !
	TKN_GT,			// >
	TKN_LS,			// <
	TKN_GTE,		// >=
	TKN_LSE,		// =<
	TKN_SHL,		// <<
	TKN_SHR,		// >>
	TKN_WILD,		// _
	TKN_WHERE,		// @
	TKN_ID,			// identifier
	TKN_TYID,       // type identifier
	TKN_MID,        // mutable variable
	TKN_BID,		// builtin variable
	TKN_S_ID,		// identifier
	TKN_S_TYID,     // type identifier
	TKN_S_MID,      // mutable variable
	TKN_S_BID,		// builtin variable
	TKN_INT,		// 42
	TKN_FLT,		// 3.14
	TKN_STR, 		// "string"
	TKN_TAG,		// 'tag'
	TKN_NEWLINE,	// \n
	
	TKN_VOID
}TkType;

typedef struct{
	char* text;
	int   len;
}StrToken;

typedef union{
	StrToken str;
	uint64_t u64;
	int64_t  i64;
	double   f64;
}TokenData;

typedef struct{
	TkType    type;
	Position  pos;
	TokenData data;
}Token;

typedef struct{
	Token* tks;
	int    tkct, tkcap;
}LexerState;




void       printLexerState(LexerState);
LexerState lexer          (LangReader*);
void       symbolizeTokens(SymbolTable*, LexerState*);
char*      printToken     (Token, char*);


ASTProgram makeASTProgram (int);
int        parseCode      (LexerState*, SymbolTable*, ASTProgram*, ErrorList*);
void       printASTProgram(ASTProgram);






#endif
