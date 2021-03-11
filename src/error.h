#ifndef __ERROR_HEADER__
#define __ERROR_HEADER__


typedef enum{
	ERR_NO_ERR		= 0,
	ERR_LEXER_FAIL  = 1,
	ERR_PARSER_FAIL = 2
}ErrorMsg;



typedef struct{
	ErrorMsg   msg;
	Position   pos;
}Error;












#endif
