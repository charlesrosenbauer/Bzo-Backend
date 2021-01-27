#ifndef __UTIL_HEADER__
#define __UTIL_HEADER__


#include "stdint.h"


typedef enum{
	LO_ADD,
	LO_SUB
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
