#ifndef __PARSER_HEADER__
#define __PARSER_HEADER__

#include "util.h"
#include "stdlib.h"

typedef struct{
	char*  text;
	int    head;
	int    size;

	Lisp** defs;
}ParserState;

Lisp*    parseLispAlt(ParserState*);
void     printVal (Valobj);
Program* parseProgram(ParserState*, int, int, int, int);

void     printFunction(FnDef*);
void     printType    (TyDef*);
void     printTyClass (TCDef*);
void     printImpl    (ImDef*);

#endif
