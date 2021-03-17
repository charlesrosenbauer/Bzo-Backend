#include "stdio.h"
#include "stdlib.h"

#include "util.h"
#include "error.h"



void fmtError(Error e, char* buffer){
	switch(e.msg){
		case ERR_NO_ERR       : return;
		case ERR_LEXER_FAIL   : {
			sprintf(buffer, "At %i:%i:%i, lexer failed.\n" , e.pos.fileId, e.pos.lineStart, e.pos.colStart);
		}break;
		case ERR_PARSER_FAIL  : {
			sprintf(buffer, "At %i:%i:%i, parser failed.\n", e.pos.fileId, e.pos.lineStart, e.pos.colStart);
		}break;
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
