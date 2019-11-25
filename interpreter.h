#ifndef __INTERPRETER_HEADER__
#define __INTERPRETER_HEADER__

#include "stdint.h"


typedef enum{
	// Int ops
	// Range: 0x0000 - 0x00FF
	NOOP  = 0x0000,
	ADDI  = 0x0001,
	SUBI  = 0x0002,
	MULI  = 0x0003,
	MULU  = 0x0004,
	DIVI  = 0x0005,
	DIVU  = 0x0006,
	MODI  = 0x0007,
	MODU  = 0x0008,
	AND   = 0x0009,
	OR    = 0x000A,
	XOR   = 0x000B,
	NOT   = 0x000C,
	SHL   = 0x000D,
	SHR   = 0x000E,
	RTL   = 0x000F,
	RTR   = 0x0010,
	MAXI  = 0x0011,
	MINI  = 0x0012,
	MAXU  = 0x0013,
	MINU  = 0x0014,
	ILS   = 0x0015,
	ILSE  = 0x0016,
	IGT   = 0x0017,
	IGTE  = 0x0018,
	EQ    = 0x0019,
	NEQ   = 0x001A,
	ULS   = 0x001B,
	ULSE  = 0x001C,
	UGT   = 0x001D,
	UGTE  = 0x001E,
	NZ    = 0x001F,
	PCT   = 0x0020,
	CTZ   = 0x0021,
	CLZ   = 0x0022,
	MTCH  = 0x0023,
	SBST  = 0x0024,
	SPST  = 0x0025,
	DSJT  = 0x0026,
	NCMP  = 0x0027,
	SXOR  = 0x0028,
	SAND  = 0x0029,
	SOR   = 0x002A,
	SXNOR = 0x002B,
	SNAND = 0x002C,
	SNOR  = 0x002D,
	INC   = 0x002E,
	DEC   = 0x002F,

	// Float ops
	// Range: 0x0100 - 0x01FF


	// Control ops
	// Range: 0x0200 - 0x02FF
	MAP   = 0x0200,
	FOLD  = 0x0201,
	SCAN  = 0x0202,
	FILTER= 0x0203,
	ANY   = 0x0204,
	CONMAP= 0x0205,
	SIFT  = 0x0206,
	IFE   = 0x0207,
	COND  = 0x0208,
	TAKE  = 0x0209,
	DROP  = 0x020A,
	HEAD  = 0x020B,
	TAIL  = 0x020C,
	INIT  = 0x020D,
	LAST  = 0x020E,
	REV   = 0x020F,
	CONCAT= 0x0210,
	CONGR = 0x0211,
	ZIP   = 0x0212,
	UNZIP = 0x0213,
	ZIP3  = 0x0214,
	UNZIP3= 0x0215,
	ZIP4  = 0x0216,
	UNZIP4= 0x0217,
	ZIP5  = 0x0218,
	UNZIP5= 0x0219,
	ZIP6  = 0x021A,
	UNZIP6= 0x021B,

	// Utility ops
	// Range: 0x0300 - 0x03FF
	EXPR  = 0x0300,
	LET   = 0x0301,
	LISPX = 0x0302,
	CMPD  = 0x0303,
	POLY  = 0x0304,
	TUP   = 0x0305,
	UNTUP = 0x0306,
	DEFN  = 0x0307
} OPCODE;

static const uint64_t BINOP =
											 (1l << ADDI) | (1l << SUBI) | (1l << MULI) | (1l << MULU) |
											 (1l << DIVI) | (1l << DIVU) | (1l << MODI) | (1l << MODU) |
							 				 (1l << AND ) | (1l << OR  ) | (1l << XOR ) | (1l << SHL ) |
											 (1l << SHR ) | (1l << RTL ) | (1l << RTR ) | (1l << MAXI) |
											 (1l << MINI) | (1l << MAXU) | (1l << MINU) | (1l << ILS ) |
											 (1l << ILSE) | (1l << IGT ) | (1l << IGTE) | (1l << EQ  ) |
											 (1l << NEQ ) | (1l << ULS ) | (1l << ULSE) | (1l << UGT ) |
											 (1l << UGTE) | (1l << MTCH) | (1l << SBST) | (1l << SPST) |
											 (1l << DSJT) | (1l << NCMP);

static const uint64_t UNOP  =
											 (1l << NOT ) | (1l << NZ  ) | (1l << PCT ) | (1l << CTZ ) |
											 (1l << CLZ ) | (1l << SXOR) | (1l << SAND) | (1l <<SXNOR) |
											 (1l <<SNAND) | (1l << SNOR) | (1l << SOR ) | (1l << INC ) |
											 (1l << DEC);


typedef struct{
	int   refc;
	int   type;
	int   size;
	void* data;
}ARR;

typedef struct{
	int   refc;
	char* text;
	int   size;
}STRING;

typedef struct{
	int   refc;
	void* left;
	void* right;
	int   balance;
	int   size;
	void* key;
	void* val;
}DICT;


typedef union{
	uint64_t UVAL;
	int64_t  IVAL;
	double   FVAL;
	STRING   SVAL;
	void*    PVAL;
	OPCODE   OVAL;
}VAL;

typedef struct{
	VAL    val;
	int    typ;
}VALOBJ;

typedef struct{
	int    refc;
	VALOBJ here;
	void*  next;
}LISP;

typedef union{
	LISP   lisp;
	DICT   dict;
	STRING string;
	ARR    array;
}OBJ;

typedef struct{
	LISP*  code;
	int    prct;
}FUNCTION;

typedef struct{
	FUNCTION* funcs;
	int       fnct;
}PROGRAM;

typedef struct{
	PROGRAM* prog;
	VALOBJ*  vars;
	int      varct;
}LISPENV;


static const int FNCTYP =  -1;
static const int PTRTYP =  -2;
static const int INTTYP =  -3;
static const int FLTTYP =  -4;
static const int STRTYP =  -5;
static const int UNTTYP =  -6;
static const int ARRTYP =  -7;
static const int DCTTYP =  -8;
static const int OPRTYP =  -9;
static const int VARTYP = -10;
static const int LSPTYP = -11;
static const int TYPTYP = -12;
static const int HOLTYP = -13;


typedef struct{
	int   size;
	int   fill;
	OBJ*  data;
	LISP* free;
}POOL;


VALOBJ eval(LISP*, LISPENV);





#endif
