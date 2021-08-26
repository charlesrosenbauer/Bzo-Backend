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
	
	// Header Parser
	ERR_P_UNX_FNDF   = 0x300,
	ERR_P_UNX_TYDF   = 0x301,
	ERR_P_UNX_COMM   = 0x302,
	ERR_P_UNX_CNST   = 0x303,
	ERR_P_UNX_LINE   = 0x304,
	ERR_P_UNX_BLOK   = 0x305,
	
	// Function, Expr, and Parameter Parser
	ERR_P_BAD_FUNC   = 0x400,
	ERR_P_BAD_EXPR   = 0x401,
	ERR_P_EXP_LABL   = 0x402,
	ERR_P_EXP_TYPE   = 0x403,
	ERR_P_BAD_TPRS   = 0x404,
	ERR_P_BAD_PARS   = 0x405,
	ERR_P_BAD_RETS   = 0x406,
	
	// Type Parser
	ERR_P_BAD_TYPE   = 0x500,
	ERR_P_BAD_STRC   = 0x501,
	ERR_P_BAD_UNON   = 0x502,
	ERR_P_BAD_TGUN   = 0x503,
	ERR_P_BAD_TYPR   = 0x504,
	ERR_P_MLT_TPRS   = 0x505,
	ERR_P_MIS_BDHD   = 0x506,
	ERR_P_UNX_WILD   = 0x507,
	ERR_P_BAD_BDHD   = 0x508,
	ERR_P_UNX_IDEN   = 0x509,
	ERR_P_UNX_FTID   = 0x50A,
	ERR_P_UNX_FTMI   = 0x50B,
	ERR_P_UNX_FTWD   = 0x50C,
	ERR_P_BAD_FTTP   = 0x50D,
	ERR_P_BAD_FTPS   = 0x50E,
	ERR_P_BAD_FTRS   = 0x50F,
	
	ERR_P_MAX_P_ERR  = 0x5FF
}ErrorMsg;



typedef struct{
	ErrorMsg   msg;
	Position   pos;
}Error;


typedef struct{
	List	errs;	// Error
}ErrorList;


ErrorList makeErrorList(int);
void      printErrors  (char**, ErrorList*);




#endif
