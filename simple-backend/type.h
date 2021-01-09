#ifndef __TYPE_HEADER__
#define __TYPE_HEADER__


#include "stdint.h"


typedef enum{
	TK_STRUCT,
	TK_UNION,
	TK_NAMED,
	TK_ARRAY,
	TK_PRIMITIVE,
	TK_VOID
}TypeKind;

typedef struct{
	void*     pars;
	TypeKind* kinds;
	int*      offsets;
	int       parct, size, align;
}Struct;

typedef struct{
	void*     pars;
	TypeKind* kinds;
	int       parct, size, align;
}Union;

typedef struct{
	uint64_t  tyid;
}NamedType;

typedef struct{
	void*     val;
	TypeKind  kind;
	int       count, stride, size, align;
}Array;

typedef enum{
	P_I8,
	P_I16,
	P_I32,
	P_I64,
	P_U8,
	P_U16,
	P_U32,
	P_U64,
	P_F16,
	P_F32,
	P_F64,
	P_Ptr
}Primitive;

typedef union{
	Struct    strc;
	Primitive prim;
	Union     unon;
	NamedType name;
	Array     arry;
}TypeUnion;

typedef struct{
	TypeUnion type;
	TypeKind  kind;
	int size, align;
}Type;


typedef struct{
	Type*  types;
	int    tyct;
}TypeTable;



TypeTable makeTypeTable(int);

Array  makeArray   ();
Struct makeStruct  (int);
Union  makeUnion   (int);
void   printType   (Type);
int    calcTypeSize(TypeTable*, Type*);


int  calcArraySize    (TypeTable*, Array*,    int*, int*);
int  calcUnionSize    (TypeTable*, Union*,    int*, int*);
int  calcStructSize   (TypeTable*, Struct*,   int*, int*);
int  calcNamedTypeSize(TypeTable*, NamedType, int*, int*);
void printArray       (Array,     int);
void printStruct      (Struct,    int);
void printUnion       (Union,     int);
void printPrimitive   (Primitive, int);

int  sizeTypeTable    (TypeTable*);


#endif
