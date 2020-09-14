#ifndef __AST_HEADER__
#define __AST_HEADER__


#include "stdint.h"

#include "bytecode.h"
#include "util.h"



typedef enum{
	PE_OKAY,
	PE_BAD_FN_TYPE,
	PE_BAD_PATTERN,
	PE_INCOMPLETE_PATTERN
}ParseError;


ParseError parseFunction(LISP*, CodeBlock*, int*);
ParseError parsePattern (LISP*, CodeBlock*, int*);











#endif