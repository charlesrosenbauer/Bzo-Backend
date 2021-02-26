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
	ASTProgram* ast;
	char* text;
	int   size, head, line, column;
}LangParser;









#endif
