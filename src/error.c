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
			case ERR_P_BAD_PAR  : sprintf(buffer, "%s Broken Parentheses.               \n", posBuffer); break;
			case ERR_P_BAD_BRK  : sprintf(buffer, "%s Broken Bracket.                   \n", posBuffer); break;
			case ERR_P_BAD_BRC  : sprintf(buffer, "%s Broken Curly Brace.               \n", posBuffer); break;
			case ERR_P_DNGL_PAR : sprintf(buffer, "%s Unmatched Parentheses.            \n", posBuffer); break;
			case ERR_P_DNGL_BRK : sprintf(buffer, "%s Unmatched Bracket.                \n", posBuffer); break;
			case ERR_P_DNGL_BRC : sprintf(buffer, "%s Unmatched Curly Brace.            \n", posBuffer); break;
			
			case ERR_P_UNX_FNDF : sprintf(buffer, "%s Unexpected Function Definition.   \n", posBuffer); break;
			case ERR_P_UNX_TYDF : sprintf(buffer, "%s Unexpected Type Definition.       \n", posBuffer); break;
			case ERR_P_UNX_COMM : sprintf(buffer, "%s Unexpected Comment.               \n", posBuffer); break;
			case ERR_P_UNX_CNST : sprintf(buffer, "%s Unexpected Constraint.            \n", posBuffer); break;
			case ERR_P_UNX_LINE : sprintf(buffer, "%s Unexpected Newline.               \n", posBuffer); break;
			
			case ERR_P_BAD_FUNC : sprintf(buffer, "%s Bad Func definition.              \n", posBuffer); break;
			case ERR_P_BAD_EXPR : sprintf(buffer, "%s Malformed expression.             \n", posBuffer); break;
			
			case ERR_P_BAD_TYPE : sprintf(buffer, "%s Bad Type definition.              \n", posBuffer); break;
			case ERR_P_BAD_STRC : sprintf(buffer, "%s Malformed struct definition.      \n", posBuffer); break;
			case ERR_P_BAD_UNON : sprintf(buffer, "%s Malformed union definition.       \n", posBuffer); break;
			case ERR_P_BAD_TGUN : sprintf(buffer, "%s Malformed tagged union definition.\n", posBuffer); break;
			
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
