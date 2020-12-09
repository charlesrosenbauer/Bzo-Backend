#include "stdlib.h"

#include "ast.h"
#include "util.h"



/*
ParseError parseCmpdExpr(LISP* l, CodeBlock* blk, int* blkct){
	VALOBJ kind = lispIx(l, 0);
	if((kind.typ == OPRTYP) && (kind.val.UVAL == 0x303)){
		return PE_BAD_CMPD;
	}

	// Okay, this looks like a CMPD. Parse it as one.
	LISP* next = l;
	while(next != NULL){
		
		next = next->next;
	}

	return PE_OKAY;
}


ParseError parsePolyExpr(LISP* l, CodeBlock* blk, int* blkct){
	VALOBJ kind = lispIx(l, 0);
	if((kind.typ == OPRTYP) && (kind.val.UVAL == 0x304)){
		return PE_BAD_POLY;
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
}*/


ParseError parsePattern(Lisp* l){
	return 0;
}



ParseError parseFnDef(Lisp* l, CodeBlock* blk, int* blkct){
	int size = lispSize(l);
	if(size < 7) return PE_BAD_FUNC;

	Valobj kind = lispIx(l, 0);
	if((kind.typ != OPRTYP) || (kind.val.UVAL != DEFN)) return PE_BAD_FUNC;
	
	Valobj fnid = lispIx(l, 1);
	if((fnid.typ != FNCTYP)) return PE_BAD_FUNC;
	
	Valobj prct = lispIx(l, 2);
	if((prct.typ != INTTYP)) return PE_BAD_FUNC;
	
	Valobj rtct = lispIx(l, 3);
	if((rtct.typ != INTTYP)) return PE_BAD_FUNC;
	
	Valobj pars = lispIx(l, 4);
	if((pars.typ != LSPTYP)) return PE_BAD_FUNC;
	
	Valobj rets = lispIx(l, 5);
	if((rets.typ != LSPTYP)) return PE_BAD_FUNC;
	
	// TODO: parse pars and rets
	
	//VALOBJ* defs = malloc(sizeof(VALOBJ) * size - 6);
	for(int i = 6; i < size; i++){
		Valobj def  = lispIx(l, i);
		if((def.typ != LSPTYP)) return PE_BAD_FUNC;
		
		Lisp* ldef = def.val.PVAL;
		if(lispSize(ldef) != 2)  return PE_BAD_FUNC;
		
		Valobj patn = lispIx(ldef, 0);
		if((patn.typ != LSPTYP)) return PE_BAD_FUNC;
		
		Valobj expr = lispIx(ldef, 1);
		if((patn.typ != LSPTYP)) return PE_BAD_FUNC;
		
		// TODO: parse patn and expr
		
	}
	
	return PE_OKAY;
}
















