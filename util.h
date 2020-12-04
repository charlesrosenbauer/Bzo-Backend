#ifndef __UTIL_HEADER__
#define __UTIL_HEADER__

#include "stdint.h"


typedef enum{
	// Instructions
	
	// Arithmetic
	OP_ADD = 0x00000,
	OP_SUB = 0x00001,
	OP_MUL = 0x00002,
	OP_DIV = 0x00003,
	OP_MIN = 0x00004,
	OP_MAX = 0x00005,
	OP_ADC = 0x00006,
	OP_SBB = 0x00007,
	
	// Comparison
	OP_LS  = 0x00010,
	OP_LSE = 0x00011,
	OP_GT  = 0x00012,
	OP_GTE = 0x00013,
	OP_EQ  = 0x00014,
	OP_NEQ = 0x00015,
	
	// Bitwise
	OP_XOR = 0x00020,
	OP_AND = 0x00021,
	OP_OR  = 0x00022,
	OP_SHL = 0x00023,
	OP_SHR = 0x00024,
	OP_ROL = 0x00025,
	OP_ROR = 0x00026,
	OP_PCT = 0x00027,
	OP_CTZ = 0x00028,
	OP_CLZ = 0x00029,
	OP_CLM = 0x0002A,


	// IR Control
	DEFN   = 0x00300,
	DEFTY  = 0x00301,
	DEFTC  = 0x00302,
	IMPTC  = 0x00303,
	
	// Primitive Types
	IT_I8  = 0x00400,
	IT_I16 = 0x00401,
	IT_I32 = 0x00402,
	IT_I64 = 0x00403,
	IT_U8  = 0x00404,
	IT_U16 = 0x00405,
	IT_U32 = 0x00406,
	IT_U64 = 0x00407,
	
	// Opcode Sizes
	SZ_S8  = 0x00000,
	SZ_S16 = 0x01000,
	SZ_S32 = 0x02000,
	SZ_S64 = 0x03000

	
}Opcode;



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
	uint64_t UVAL;
	int64_t  IVAL;
	double   FVAL;
	String   SVAL;
	void*    PVAL;
	Opcode   OVAL;
	Bitset   BVAL;
}Val;

typedef struct{
	Val    val;
	int    typ;
}Valobj;

typedef struct{
	int    refc;
	Valobj here;
	void*  next;
}Lisp;


typedef enum{
	TS_UNT,
	TS_INT,
	TS_FLT,
	TS_STR,
	TS_TYP,
	TS_FUN,
	TS_VAR
}TypeSymbolKind;

typedef struct{
	union{
		uint64_t untLit;
		int64_t  intLit;
		float    fltLit;
		char*    strLit;
		uint64_t typLit;
		uint64_t funLit;
		uint64_t varLit;
	};
	TypeSymbolKind symKind;
}TypeSymbol;

typedef struct{
	void*  fields;
	int    fieldct, size, align;
}TypeStruct;

typedef struct{
	void*  fields;
	int    fieldct, size, align;
}TypeUnion;

typedef struct{
	void*  tprs;
	void*  pars;
	void*  rets;
	int    parct, retct, tprct;
}TypeFunction;

typedef struct{
	void*  types;
	int    tyct, size, align;
}TypeExpr;

typedef enum{
	T_SYMBOL,
	T_STRUCT,
	T_UNION,
	T_FUNCTION,
	T_EXPR,
	T_ALIAS,
	T_SOURCE
}TypeKind;

typedef struct{
	union{
		TypeSymbol   sym;
		TypeStruct   str;
		TypeUnion    uni;
		TypeFunction fnc;
		TypeExpr     exp;
		uint64_t     alias;
	};
	TypeKind kind;
	
	void*    pars;
	int      parct, size, align;
}Type;

typedef struct{
	uint64_t fnid, tcid;
	
	Lisp**   impls;
	int      implct;
}ClassImpl;


typedef struct{
	TypeFunction fntype;
	
	Lisp* tprsSource;
	Lisp* parsSource;
	Lisp* retsSource;
	Lisp* codeSource;
	
	int tpct, prct, rtct;
}FnDef;

typedef struct{
	Type  type;
	Lisp* parSource;
	Lisp* defSource;
	
	int parct, size, align;
}TyDef;

typedef struct{
	
}TCDef;

typedef struct{
	
}ImDef;


typedef struct{
	FnDef* funcs;
	TyDef* types;
	TCDef* classes;
	ImDef* impls;
	int    fnct, tyct, tcct, imct;
}Program;

typedef enum{
	PT_I8   =  -1,
	PT_I16  =  -2,
	PT_I32  =  -3,
	PT_I64  =  -4,
	PT_U8   =  -5,
	PT_U16  =  -6,
	PT_U32  =  -7,
	PT_U64  =  -8,
	PT_F16  =  -9,
	PT_F32  = -10,
	PT_F64  = -11,
	PT_CHAR = -12,
	PT_BOOL = -13,
	PT_ARR  = -14,
	PT_STR  = -15
}Primtype;


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
static const int TUPTYP = -14;
static const int BSTTYP = -15;








Valobj lispIx      (Lisp*, int);
void   printProgram(Program*);
void   printLisp   (Lisp*);
int    lispSize    (Lisp*);
void   loadFile    (char*, uint8_t**, int*);

void printHexBuffer(uint8_t*, int);


#endif
