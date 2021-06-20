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




typedef struct{
	char* text;
	int   len;
}StrToken;


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

	BID_IMPORT			= 0x4000,
	BID_MODULE          = 0x4001
}BuiltinId;

int isTypeBID(BuiltinId);


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

typedef struct{
	Position  pos;
	
}ASTType;

typedef struct{
	Position  pos;
	int       tyid;
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

void printOperation(Operation, int);


typedef struct{
	Position pos;
	int      fnid;
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
	TKN_WAT,		// ?
	TKN_NWAT,		// !?
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
void       printASTType   (ASTType, int);
void       printASTProgram(ASTProgram);






#endif
