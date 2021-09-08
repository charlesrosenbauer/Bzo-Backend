#include "stdio.h"
#include "stdlib.h"

#include "util.h"
#include "error.h"


void fmtPosition(char** fnames, Position p, char* buffer){
	sprintf(buffer, "%s@(%i:%i)", fnames[p.fileId], p.lineStart, p.colStart);
}


void fmtError(char** fnames, Error e, char* buffer){
	char posBuffer[256];
	fmtPosition(fnames, e.pos, posBuffer);
	if       (e.msg < ERR_X_MAX_X_ERR){
		sprintf(buffer, "%s Unknown Error\n", posBuffer);
	}else if (e.msg < ERR_P_MAX_P_ERR){
		switch(e.msg){
			case ERR_P_BAD_PAR  : sprintf(buffer, "%s Broken Parentheses.                                   \n", posBuffer); break;
			case ERR_P_BAD_BRK  : sprintf(buffer, "%s Broken Bracket.                                       \n", posBuffer); break;
			case ERR_P_BAD_BRC  : sprintf(buffer, "%s Broken Curly Brace.                                   \n", posBuffer); break;
			case ERR_P_DNGL_PAR : sprintf(buffer, "%s Unmatched Parentheses.                                \n", posBuffer); break;
			case ERR_P_DNGL_BRK : sprintf(buffer, "%s Unmatched Bracket.                                    \n", posBuffer); break;
			case ERR_P_DNGL_BRC : sprintf(buffer, "%s Unmatched Curly Brace.                                \n", posBuffer); break;
			
			case ERR_P_UNX_FNDF : sprintf(buffer, "%s Unexpected Function Definition.                       \n", posBuffer); break;
			case ERR_P_UNX_TYDF : sprintf(buffer, "%s Unexpected Type Definition.                           \n", posBuffer); break;
			case ERR_P_UNX_COMM : sprintf(buffer, "%s Unexpected Comment.                                   \n", posBuffer); break;
			case ERR_P_UNX_CNST : sprintf(buffer, "%s Unexpected Constraint.                                \n", posBuffer); break;
			case ERR_P_UNX_LINE : sprintf(buffer, "%s Unexpected Newline or Semicolon.                      \n", posBuffer); break;
			case ERR_P_UNX_BLOK : sprintf(buffer, "%s Unexpected code block.                                \n", posBuffer); break;
			
			case ERR_P_BAD_FUNC : sprintf(buffer, "%s Bad Func definition.                                  \n", posBuffer); break;
			case ERR_P_BAD_EXPR : sprintf(buffer, "%s Malformed expression.                                 \n", posBuffer); break;
			case ERR_P_EXP_LABL : sprintf(buffer, "%s Missing label on parameter.                           \n", posBuffer); break;
			case ERR_P_EXP_TYPE : sprintf(buffer, "%s Missing type on parameter.                            \n", posBuffer); break;
			case ERR_P_BAD_TPRS : sprintf(buffer, "%s Broken type parameters.                               \n", posBuffer); break;
			case ERR_P_BAD_PARS : sprintf(buffer, "%s Broken parameters.                                    \n", posBuffer); break;
			case ERR_P_BAD_RETS : sprintf(buffer, "%s Broken return parameters.                             \n", posBuffer); break;
			
			case ERR_P_BAD_TYPE : sprintf(buffer, "%s Bad Type definition.                                  \n", posBuffer); break;
			case ERR_P_BAD_STRC : sprintf(buffer, "%s Malformed struct definition.                          \n", posBuffer); break;
			case ERR_P_BAD_UNON : sprintf(buffer, "%s Malformed union definition.                           \n", posBuffer); break;
			case ERR_P_BAD_TGUN : sprintf(buffer, "%s Malformed tagged union definition.                    \n", posBuffer); break;
			case ERR_P_BAD_TYPR : sprintf(buffer, "%s Malformed type, contents of parentheses are malformed.\n", posBuffer); break;
			case ERR_P_MLT_TPRS : sprintf(buffer, "%s Multiple type parameter blocks.                       \n", posBuffer); break;
			case ERR_P_MIS_BDHD : sprintf(buffer, "%s Missing type build header.                            \n", posBuffer); break;
			case ERR_P_UNX_WILD : sprintf(buffer, "%s Wildcards are not permitted in type builds.           \n", posBuffer); break;
			case ERR_P_BAD_BDHD : sprintf(buffer, "%s Unexpected tokens before Build Type header.           \n", posBuffer); break;
			case ERR_P_UNX_IDEN : sprintf(buffer, "%s Unexpected identifier in Build Type.                  \n", posBuffer); break;
			case ERR_P_UNX_FTID : sprintf(buffer, "%s Unexpected identifier in Function Type.               \n", posBuffer); break;
			case ERR_P_UNX_FTMI : sprintf(buffer, "%s Unexpected mutable identifier in Function Type.       \n", posBuffer); break;
			case ERR_P_UNX_FTWD : sprintf(buffer, "%s Unexpected wildcard in Function Type.                 \n", posBuffer); break;
			case ERR_P_BAD_FTTP : sprintf(buffer, "%s Broken type parameters in Function Type.              \n", posBuffer); break;
			case ERR_P_BAD_FTPS : sprintf(buffer, "%s Broken parameters in Function Type.                   \n", posBuffer); break;
			case ERR_P_BAD_FTRS : sprintf(buffer, "%s Broken return parameters in Function Type.            \n", posBuffer); break;
			
			case ERR_T_BAD_BITY : sprintf(buffer, "%s Unrecognized builtin type.                            \n", posBuffer); break;
			
			case ERR_N_BAD_MODL : sprintf(buffer, "%s Expected module name.                                 \n", posBuffer); break;
			case ERR_N_RPT_MODL : sprintf(buffer, "%s Repeat module name.                                   \n", posBuffer); break;
			case ERR_N_RPT_IMPT : sprintf(buffer, "%s Repeat module import.                                 \n", posBuffer); break;
			
			default: sprintf(buffer, "%s Unknown Parser Error\n", posBuffer); break;
		}
	}else{
		sprintf(buffer, "%s Unknown Compiler Error\n", posBuffer);
	}
}

void printErrors(char** fnames, ErrorList* errs){
	// Ideally, we should sort errors first
	for(int i = 0; i < errs->errs.size; i++){
		char buffer[1024];
		Error* ers = errs->errs.array;
		fmtError(fnames, ers[i], buffer);
		printf("%s\n\n", buffer);
	}
}




ErrorList makeErrorList(int sz){
	ErrorList ret;
	ret.errs  = makeList(sz, sizeof(Error));
	return ret;
}
