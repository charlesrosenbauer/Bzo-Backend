#ifndef __AST_HEADER__
#define __AST_HEADER__


#include "stdint.h"

#include "bytecode.h"
#include "util.h"



typedef enum{
	PE_OKAY,
	PE_BAD_FN_TYPE,
	PE_BAD_PATTERN,
	PE_INCOMPLETE_PATTERN,
	PE_BAD_CMPD,
	PE_BAD_POLY,
	PE_BAD_EXPR,
	PE_BAD_PRFX,
	PE_BAD_FUNC
}ParseError;


ParseError parseFunction(LISP*, CodeBlock*, int*);
ParseError parsePattern (LISP*, CodeBlock*, int*);
ParseError parseExpr    (LISP*, CodeBlock*, int*);








#endif
