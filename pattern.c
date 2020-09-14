#include "ast.h"

#include "util.h"





ParseErr parseExpr(LISP* l, CodeBlock* blk, int* blkct){
	VALOBJ kind = lispIx(l, 0);
	if((kind.typ == OPRTYP) && (kind.val.UVAL == 0x300)){
		return PE_BAD_EXPR;
	}

	// Okay, this looks like an expression. Parse it as one.

	return PE_OKAY;
}