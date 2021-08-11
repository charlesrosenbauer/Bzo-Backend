#ifndef __POSTPROCESS_HEADER__
#define __POSTPROCESS_HEADER__


#include "stdint.h"

#include "ast.h"
#include "common.h"

/*
typedef struct{
	Position pos;
	int64_t* fieldIds;
	int64_t* fields;
	int*     offsets;
	int      size, align, fieldct;
}StructData;

typedef struct{
	Position pos;
	int64_t* fieldIds;
	int64_t* fields;
	int64_t* vals;
	int      size, tagOffset, align, fieldct;
}UnionData;

typedef struct{
	Position pos;
	int64_t* valIds;
	int64_t* vals;
	int      size, align, valct;
}EnumData;

typedef struct{
	Position pos;
	int64_t* sizes;
	int64_t  elem;
	int      size, align, dimension;
}ArrayData;

typedef struct{
	Position pos;
	int64_t* pars;
	int64_t  recipe;
	int      size, align, parct;
}BuildData;

typedef struct{
	Position pos;
	int64_t  tyid;
	int		 size, align;
}AliasData;

typedef enum{
	TDK_VOID = 0,
	TDK_STRC = 1,
	TDK_UNON = 2,
	TDK_ENUM = 3,
	TDK_ARRY = 4,
	TDK_BILD = 5,
	TDK_ALIS = 6
}TypeDataKind;

typedef struct{
	union{
		StructData strc;
		UnionData  unon;
		EnumData   enmt;
		ArrayData  arry;
		BuildData  bild;
		AliasData  alis;
	};
	int64_t      name;
	ASTType*     type;
	TypeDataKind kind;
}TypeData;


typedef struct{
	int64_t*	files;
	int64_t*	tyixs;
	int			ct, cap;
}TypeNameEntry;

typedef struct{
	TypeNameEntry*	entries;
	int				size;
}TypeNameTable;


typedef struct{
	TypeNameTable ntab;
	TypeData*     types;
	int           typect, typecap;
}TypeTable;


typedef struct{
	int64_t		name;
	int64_t*	visible;
	int         visct;
}Namespace;

typedef struct{
	Namespace*  names;
	int nmct, nmcap;
}NamespaceTable;



TypeTable makeTypeTable     (int);
int       insertTypeTable   (TypeTable*, TypeData);
int       sizeType          (TypeTable*, ASTType*);
int       sizeTypes         (TypeTable*);
int       dumpToTypeTable   (TypeTable*, ASTProgram*, int);
void      printTypeTable    (TypeTable*);
void      printTypeNameTable(TypeNameTable*);
*/


#endif
