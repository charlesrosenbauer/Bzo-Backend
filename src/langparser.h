#ifndef __LANGPARSER_HEADER__
#define __LANGPARSER_HEADER__


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
	int*     arrs;
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

typedef enum{
	TT_ELEM,
	TT_STRC,
	TT_UNON,
	TT_BITY
}ASTTypeEnum;

typedef struct{
	union{
		ASTTypeElem		elem;
		ASTStruct		strc;
		ASTUnion		unon;
		int             bity;
	}type;
	ASTTypeEnum kind;
}ASTType;

typedef struct{
	Position pos;
	int      tyid;
	ASTType  type;
}ASTTyDef;


typedef enum{
	OPR_ADD,		// +
	OPR_SUB,		// -
	OPR_MUL,		// *
	OPR_DIV,		// /
	OPR_MOD,		// %
	OPR_EXP,		// ^
	OPR_AND,		// &
	OPR_OR,			// |
	OPR_XOR,		// ^^
	OPR_NOT,		// !
	OPR_LS,			// <
	OPR_GT,			// >
	OPR_LSE,		// =<
	OPR_GTE,		// >=
	OPR_EQ,			// =
	OPR_NEQ,		// !=
	OPR_IX 			// [i]
}Operator;

typedef struct{
	Position pos;
}ASTOp;

typedef struct{
	Position pos;
}ASTExpr;

typedef struct{
	Position pos;

	ASTExpr expr;
	int*    pars;
	int     prct;
}ASTStmt;

typedef struct{
	Position pos;
	
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
