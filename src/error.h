#ifndef __ERROR_HEADER__
#define __ERROR_HEADER__


typedef enum{
	// Control Errors
	ERR_X_NO_ERR	 = 0x000,
	
	ERR_X_MAX_X_ERR  = 0x0FF,
	
	// Lexer Errors
	ERR_L_MAX_L_ERR  = 0x1FF,
	
	// Parser Errors
	ERR_P_BAD_PAR    = 0x200,
	ERR_P_BAD_BRK    = 0x201,
	ERR_P_BAD_BRC    = 0x202,
	ERR_P_DNGL_PAR   = 0x203,
	ERR_P_DNGL_BRK   = 0x204,
	ERR_P_DNGL_BRC   = 0x205,
	
	ERR_P_BAD_FUNC   = 0x300,
	ERR_P_BAD_EXPR   = 0x301,
	
	ERR_P_BAD_TYPE   = 0x400,
	ERR_P_BAD_STRC   = 0x401,
	ERR_P_BAD_UNON   = 0x402,
	ERR_P_BAD_TGUN   = 0x403,
	
	ERR_P_MAX_P_ERR  = 0x4FF
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
void      printErrors  (ErrorList*);





#endif
