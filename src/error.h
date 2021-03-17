#ifndef __ERROR_HEADER__
#define __ERROR_HEADER__


typedef enum{
	ERR_NO_ERR		= 0,
	ERR_LEXER_FAIL  = 1,
	ERR_PARSER_FAIL = 2,
	
	// Parser Errors
	ERR_P_BAD_PAR   = 0x100,
	ERR_P_BAD_BRK   = 0x101,
	ERR_P_BAD_BRC   = 0x102
}ErrorMsg;



typedef struct{
	ErrorMsg   msg;
	Position   pos;
}Error;


typedef struct{
	Error*     errs;
	int        erct, ercap;
}ErrorList;


ErrorList makeErrorList(int);
void      appendError  (ErrorList*, Error);






#endif
