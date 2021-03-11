#include "stdio.h"

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
