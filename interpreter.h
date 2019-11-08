#ifndef __INTERPRETER_HEADER__
#define __INTERPRETER_HEADER__

#include "stdint.h"


typedef enum{
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
	NCMP  = 0x0027
} OPCODE;


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
	int   refc;
	int   type;
	VAL   here;
	void* next;
}LISP;

typedef union{
	LISP   lisp;
	DICT   dict;
	STRING string;
	ARR    array;
}OBJ;


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


typedef struct{
	int   size;
	int   fill;
	OBJ*  data;
	LISP* free;
}POOL;





#endif
