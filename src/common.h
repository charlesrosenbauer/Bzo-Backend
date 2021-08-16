#ifndef __COMMON_HEADER__
#define __COMMON_HEADER__


#include "stdint.h"


#include "util.h"
#include "error.h"


/*
	Symbol Table
*/
typedef enum{
	SF_AMBI		= 0x00,
	SF_ID		= 0x01,
	SF_MID		= 0x02,
	SF_TYID		= 0x04,
	SF_TVAR		= 0x08
}SymbolFlags;

typedef struct{
	char*       text;
	uint64_t    hash;
	int         id;
	SymbolFlags flag;
}Symbol;

typedef struct{
	Symbol* syms;
	int     size, fill, idct;
}SymbolTable;

void        printSymbolTable(SymbolTable);
Symbol      searchSymbol    (SymbolTable*, Symbol);
int         insertSymbolText(SymbolTable*, char*, SymbolFlags);
int         insertSymbol    (SymbolTable*, Symbol);
SymbolTable makeSymbolTable (int);


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
	TKN_PIPE,		// \    (backslash)
	TKN_CONSTRAIN,	// |:
	TKN_FNTY,		// |>
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
	TKN_XOR,		// ^^
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
	TKN_TVAR,		// type variable
	TKN_MID,        // mutable variable
	TKN_BID,		// builtin variable
	TKN_S_ID,		// identifier
	TKN_S_TYID,     // type identifier
	TKN_S_TVAR,		// type variable
	TKN_S_MID,      // mutable variable
	TKN_S_BID,		// builtin variable
	TKN_INT,		// 42
	TKN_FLT,		// 3.14
	TKN_STR, 		// "string"
	TKN_TAG,		// 'tag'
	TKN_NEWLINE,	// \n
	TKN_LOCID,		// id @ place
	TKN_LOCTY,		// Ty @ place
	
	TKN_VOID
}TkType;


typedef struct{
	char* text;
	int   len;
}StrToken;

typedef struct{
	uint64_t* path;
	int       len;
}LocToken;


typedef union{
	StrToken str;
	LocToken loc;
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


/*
	Builtin Types
*/
typedef enum{
	BID_POPCOUNT32		= -0x1001,
	BID_POPCOUNT64		= -0x1002,
	
	BID_I8              = -0x2000,
	BID_I16             = -0x2001,
	BID_I32				= -0x2002,
	BID_I64				= -0x2003,
	BID_I128			= -0x2004,
	BID_I256			= -0x2005,
	BID_U8				= -0x2006,
	BID_U16				= -0x2007,
	BID_U32				= -0x2008,
	BID_U64				= -0x2009,
	BID_U128			= -0x200A,
	BID_U256			= -0x200B,
	BID_BOOL			= -0x200C,
	BID_F16				= -0x200D,
	BID_F32				= -0x200E,
	BID_F64				= -0x200F,
	BID_MAX_TYPE		= -0x2FFF,

	BID_IMPORT			= -0x4000,
	BID_MODULE          = -0x4001
}BuiltinId;

int isTypeBID(BuiltinId);





#endif
