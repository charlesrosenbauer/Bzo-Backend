#ifndef __TYPE_HEADER__
#define __TYPE_HEADER__


#include "stdint.h"


typedef enum{
	TK_STRUCT,
	TK_PRIMITIVE
}TypeKind;

typedef struct{
	void*     pars;
	TypeKind* kinds;
	int*      offsets;
	int       parct, size, align;
}Struct;

typedef enum{
	P_I8,
	P_I16,
	P_I32,
	P_I64
}Primitive;

typedef union{
	Struct    strc;
	Primitive prim;
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



Struct makeStruct  (int);
void   printType   (Type);
int    calcTypeSize(Type*);


#endif
