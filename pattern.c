#include "ast.h"

#include "util.h"




ParseError parseCmpd(LISP* l, CodeBlock* blk, int* blkct){
	VALOBJ kind = lispIx(l, 0);
	if((kind.typ == OPRTYP) && (kind.val.UVAL == 0x303)){
		return PE_BAD_EXPR;
	}

	// Okay, this looks like a CMPD. Parse it as one.

	return PE_OKAY;
}


ParseError parsePoly(LISP* l, CodeBlock* blk, int* blkct){
	VALOBJ kind = lispIx(l, 0);
	if((kind.typ == OPRTYP) && (kind.val.UVAL == 0x304)){
		return PE_BAD_EXPR;
	}

	// Okay, this looks like a POLY. Parse it as one.

	return PE_OKAY;
}



ParseError parseExpr(LISP* l, CodeBlock* blk, int* blkct){
	VALOBJ kind = lispIx(l, 0);
	if((kind.typ == OPRTYP) && (kind.val.UVAL == 0x300)){
		return PE_BAD_EXPR;
	}

	// Okay, this looks like an expression. Parse it as one.

	return PE_OKAY;
}
