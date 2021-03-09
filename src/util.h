#ifndef __UTIL_HEADER__
#define __UTIL_HEADER__


#include "stdint.h"


typedef enum{
	LO_NOP	   = 0x00000,
	
	// Arithmetic
	LO_ADD	   = 0x00001,
	LO_SUB     = 0x00002,
	LO_MUL     = 0x00003,
	LO_DIV     = 0x00004,
	LO_ABS     = 0x00005,
	LO_NEG     = 0x00006,
	
	// Comparison
	LO_LS      = 0x00020,
	LO_GT	   = 0x00021,
	LO_LSE	   = 0x00022,
	LO_GTE     = 0x00023,
	LO_ULS	   = 0x00024,
	LO_UGT     = 0x00025,
	LO_ULSE    = 0x00026,
	LO_UGTE    = 0x00027,
	LO_EQ      = 0x00028,
	LO_NEQ     = 0x00029,
	
	// Bitwise
	LO_AND	   = 0x00040,
	LO_OR      = 0x00041,
	LO_XOR     = 0x00042,
	LO_NOT     = 0x00043,
	LO_SHL     = 0x00044,
	LO_SHR     = 0x00045,
	LO_ROL     = 0x00046,
	LO_ROR     = 0x00047,
	LO_PCT     = 0x00048,
	LO_CTZ     = 0x00049,
	LO_CLZ     = 0x0004A,
	
	// Control Flow
	LO_MAP     = 0x00060,
	LO_FOLD    = 0x00061,
	LO_SCAN    = 0x00062,
	LO_FILTER  = 0x00063,
	LO_ITER    = 0x00064,
	
	// Array
	LO_ZIP     = 0x000A0,
	LO_UNZIP   = 0x000A1,
	LO_LEN     = 0x000A2,
	LO_STRIDE  = 0x000A3,
	LO_IX      = 0x000A4,
	LO_TAKE    = 0x000A5,
	LO_DROP    = 0x000A6,
	LO_PROD    = 0x000A7,

	// Control opcode. Not a real opcode, just to make code cleaner
	LO_MAX_OPC = 0x002FF,

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
	LO_BK	   = 0x0030D,
	LO_MK	   = 0x0030E,
	
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

void leftpad       (int);






#endif
