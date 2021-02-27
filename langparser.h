#ifndef __LANGPARSER_HEADER__
#define __LANGPARSER_HEADER__


#include "stdint.h"


typedef struct{
	int fileId, lineStart, lineEnd, colStart, colEnd;
}Position;

typedef struct{
	char*    text;
	uint64_t hash;
}Symbol;

typedef struct{
	Symbol* syms;
	int     size, fill;
}SymbolTable;

typedef struct{
	Position pos;
	
}ASTFnDef;

typedef struct{
	Position pos;
}ASTTyDef;

typedef struct{
	
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
	TKN_DEFINE,		// ::
	TKN_L_ARROW,	// <-
	TKN_R_ARROW,	// ->
	TKN_ADD,		// +
	TKN_SUB,		// -
	TKN_MUL,		// *
	TKN_DIV,		// /
	TKN_MOD,		// %
	TKN_AND,		// &
	TKN_OR,			// |
	TKN_NOT,		// !
	TKN_ID,			// identifier
	TKN_INT,		// 42
	TKN_FLT,		// 3.14
	TKN_STR, 		// "string"
	TKN_NEWLINE		// \n
}TkType;

typedef struct{
	TkType   type;
	Position pos;
}Token;

typedef struct{
	Token* tks;
	int    tkct, tkcap;
}LexerState;




void       printLexerState(LexerState);
LexerState lexer          (LangReader*);

#endif
