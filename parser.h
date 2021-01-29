#ifndef __PARSER_HEADER__
#define __PARSER_HEADER__

#include "util.h"
#include "stdlib.h"
#include "program.h"

typedef struct{
	char*  text;
	int    head;
	int    size;

	Lisp** defs;
}ParserState;

Lisp*    parseLispAlt(ParserState*);
void     printVal (Valobj);

Program  parseProgram(uint8_t*, int);

#endif
