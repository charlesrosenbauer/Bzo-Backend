#include "stdio.h"
#include "stdlib.h"

#include "util.h"
#include "error.h"


void fmtPosition(Position p, char* buffer){
	sprintf(buffer, "%i@(%i:%i)", p.fileId, p.lineStart, p.colStart);
}


void fmtError(Error e, char* buffer){
	char posBuffer[256];
	fmtPosition(e.pos, posBuffer);
	if       (e.msg < ERR_X_MAX_X_ERR){
		sprintf(buffer, "%s Unknown Error\n", posBuffer);
	}else if (e.msg < ERR_P_MAX_P_ERR){
		switch(e.msg){
			case ERR_P_BAD_PAR  : sprintf(buffer, "%s Broken Parentheses.   \n", posBuffer); break;
			case ERR_P_BAD_BRK  : sprintf(buffer, "%s Broken Bracket.       \n", posBuffer); break;
			case ERR_P_BAD_BRC  : sprintf(buffer, "%s Broken Curly Brace.   \n", posBuffer); break;
			case ERR_P_DNGL_PAR : sprintf(buffer, "%s Unmatched Parentheses.\n", posBuffer); break;
			case ERR_P_DNGL_BRK : sprintf(buffer, "%s Unmatched Bracket.    \n", posBuffer); break;
			case ERR_P_DNGL_BRC : sprintf(buffer, "%s Unmatched Curly Brace.\n", posBuffer); break;
			default: sprintf(buffer, "%s Unknown Parser Error\n", posBuffer); break;
		}
	}else{
		sprintf(buffer, "%s Unknown Compiler Error\n", posBuffer);
	}
}

void printErrors(ErrorList* errs){
	// Ideally, we should sort errors first
	for(int i = 0; i < errs->erct; i++){
		char buffer[1024];
		fmtError(errs->errs[i], buffer);
		printf("%s\n\n", buffer);
	}
}




ErrorList makeErrorList(int sz){
	ErrorList ret;
	ret.errs  = malloc(sizeof(Error) * sz);
	ret.erct  = 0;
	ret.ercap = sz;
	return ret;
}


void      appendError  (ErrorList* errs, Error e){
	if(errs->erct+1 >= errs->ercap){
		Error* tmp = errs->errs;
		errs->errs = malloc(sizeof(Error) * errs->ercap * 2);
		for(int i = 0; i < errs->ercap; i++) errs->errs[i] = tmp[i];
		errs->ercap *= 2;
		free(tmp);
	}
	errs->errs[errs->erct] = e;
	errs->erct++;
}
