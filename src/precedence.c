#include "postprocess.h"

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "ast.h"
#include "common.h"



/*
// If it is a binop, it returns precedence
int precedenceBinop(TkType t){
	switch(t){
		case TKN_ADD   : return 6;		//	+
		case TKN_SUB   : return 6;		//	-
		case TKN_MUL   : return 5;		//	*
		case TKN_DIV   : return 4;		//	/
		case TKN_MOD   : return 2;		//	%
		case TKN_EXP   : return 3;		//	^
		case TKN_AND   : return 9;		//	&
		case TKN_OR    : return 9;		//	|
		case TKN_XOR   : return 9;		//	^^
		case TKN_SHL   : return 7;		//	<<
		case TKN_SHR   : return 7;		//	>>
		case TKN_GT    : return 8;		//	>
		case TKN_GTE   : return 8;		//	>=
		case TKN_LS    : return 8;		//	<
		case TKN_LSE   : return 8;		//	=<
		case TKN_EQL   : return 8;		//	=
		case TKN_NEQ   : return 8;		//	!=
		case TKN_PIPE  : return 1;		//	\.
		case TKN_PERIOD: return 1;		//	.
		// precedence 2 is index
		default :        return 0;
	}
}

int precedenceUnop(TkType t){
	switch(t){
		case TKN_SUB     : return 1;
		case TKN_EXP     : return 1;
		case TKN_L_ARROW : return 1;
		case TKN_NOT     : return 1;
		default          : return 0;
	}
}


int precedenceShuffle(ASTExpr* xp){
	if(xp == NULL) return 0;	
	switch(xp->type){
		case XT_BOP : {
			// Here's the tricky part
			int pa = precedenceShuffle(xp->a);
			int pb = precedenceShuffle(xp->b);
			int px = precedenceBinop  (xp->tk.type);
			if(px < pa){
				// I'm not sure if we actually need to shuffle both sides, since it's parsed from the left
				ASTExpr* l  = xp->a;
				ASTExpr* lr = l ->b;
				ASTExpr* p  = xp;
				l->b = p;
				p->a = lr;
				*xp  = *l;
			}
			return px;
		}break;
		
		case XT_UOP : {
			precedenceShuffle(xp->a);
			return 1;
		}break;
		
		case XT_LMDA: {
		
			return 1;
		}break;
		
		case XT_SWCH: {
		
			return 1;
		}break;
		
		case XT_IFE : {
		
			return 1;
		}break;
		
		case XT_MK  :
		case XT_FNCL: {
		
			return 1;
		}break;
		
		case XT_IX  : {
		
			return 1;
		}break;
		
		
		default: return 0;
	}
}*/

