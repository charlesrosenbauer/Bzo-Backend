#ifndef __POSTPROCESS_HEADER__
#define __POSTPROCESS_HEADER__


#include "stdint.h"

#include "ast.h"
#include "program.h"



typedef struct{
	int64_t  id;
	uint64_t hash;
	int64_t* fieldIds;
	int64_t* fields;
	int*     offsets;
	int      size, align, fieldct;
}StructData;

typedef struct{
	int64_t  id;
	uint64_t hash;
	int64_t* fieldIds;
	int64_t* fields;
	int64_t* vals;
	int      size, tagOffset, align, fieldct;
}UnionData;

typedef struct{
	int64_t  id;
	uint64_t hash;
	int64_t* valIds;
	int64_t* vals;
	int      size, valct;
}EnumData;

typedef struct{
	int64_t  id;
	uint64_t hash;
	int64_t* sizes;
	int64_t  elem;
	int      dimension;
}ArrayData;

typedef struct{
	int64_t  id;
	uint64_t hash;
	int64_t* pars;
	int64_t  recipe;
	int      size, align, parct;
}BuildData;

typedef enum{
	TDK_VOID = 0,
	TDK_STRC = 1,
	TDK_UNON = 2,
	TDK_ENUM = 3,
	TDK_ARRY = 4,
	TDK_BILD = 5
}TypeDataKind;

typedef struct{
	union{
		StructData strc;
		UnionData  unon;
		EnumData   enmt;
		ArrayData  arry;
		BuildData  bild;
	};
	int64_t      name;
	ASTType*     type;
	TypeDataKind kind;
}TypeData;

typedef struct{
	TypeData*  types;
	int typect, typecap;
}TypeTable;

TypeTable makeTypeTable  (int);
int       insertTypeTable(TypeTable*, TypeData);
int       sizeType       (TypeTable*, ASTType*);
int       sizeTypes      (TypeTable*, ASTProgram*);

int       postprocess    (SymbolTable*, ASTProgram*);
void      printTypeTable (TypeTable*);



#endif
