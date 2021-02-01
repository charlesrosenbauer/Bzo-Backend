#ifndef __UTIL_HEADER__
#define __UTIL_HEADER__


#include "stdint.h"


typedef enum{

	// Definitions
	LO_DEFN    = 0x00300,
	LO_DEFTY   = 0x00301,
	LO_DEFTC   = 0x00302,
	LO_IMPTC   = 0x00303,
	LO_EXPR    = 0x00304,
	LO_STMT    = 0x00305,
	LO_LET     = 0x00306,
	LO_LAMBDA  = 0x00307,
	LO_LSP     = 0x00308,
	LO_CMPD    = 0x00309,
	LO_POLY    = 0x0030A,
	LO_ARRAY   = 0x0030B,
	LO_FNTY    = 0x0030C,
	
	// Primitives
	LO_I8	   = 0x00400,
	LO_I16     = 0x00401,
	LO_I32     = 0x00402,
	LO_I64     = 0x00403,
	LO_U8	   = 0x00404,
	LO_U16     = 0x00405,
	LO_U32     = 0x00406,
	LO_U64     = 0x00407,
	LO_F16     = 0x00408,
	LO_F32     = 0x00409,
	LO_F64     = 0x0040A,
	LO_Ptr     = 0x0040B
	
}LispOpcode;

typedef struct{
	int   refc;
	int   type;
	int   size;
	void* data;
}Arr;

typedef struct{
	int   refc;
	char* text;
	int   size;
}String;

typedef uint64_t Bitset;


typedef union{
	uint64_t 	UVAL;
	int64_t  	IVAL;
	double   	FVAL;
	String   	SVAL;
	void*    	PVAL;
	LispOpcode  OVAL;
	Bitset	    BVAL;
}Val;

typedef enum{
	FNCTYP=0,
	PTRTYP=1,
	INTTYP=2,
	FLTTYP=3,
	STRTYP=4,
	UNTTYP=5,
	ARRTYP=6,
	DCTTYP=7,
	OPRTYP=8,
	VARTYP=9,
	LSPTYP=10,
	TYPTYP=11,
	HOLTYP=12,
	TUPTYP=13,
	BSTTYP=14
}LispType;

typedef struct{
	Val      val;
	LispType typ;
}Valobj;

typedef struct{
	int    refc;
	Valobj here;
	void*  next;
}Lisp;



int gcd(int, int);
int lcm(int, int);

Valobj lispIx      (Lisp*, int);
void   printLisp   (Lisp*);
int    lispSize    (Lisp*);
void   loadFile    (char*, uint8_t**, int*);

void printHexBuffer(uint8_t*, int);








#endif
