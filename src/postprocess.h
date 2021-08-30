#ifndef __POSTPROCESS_HEADER__
#define __POSTPROCESS_HEADER__


#include "stdint.h"

#include "ast.h"
#include "common.h"


typedef struct{
	List	fieldTys;	// int64_t
	List	fieldIds;	// int64_t
	List	offsets;	// int
}StructData;

typedef struct{
	List	fieldTys;	// int64_t
	List	fieldIds;	// int64_t
	List	fieldVls;	// int64_t
	int64_t tagty, tagid;
	int     tagoffset;
}UnionData;

typedef struct{
	List	vals;		// int64_t
	List	tags;		// int64_t
	int64_t tagty;
}EnumData;

typedef struct{
	List	sizes;		// int64_t
	int64_t baseType;
}ArrayData;

typedef struct{
	List	paramTys;	// int64_t
	int64_t recipe;
}BuildData;

typedef enum{
	LK_STRC,
	LK_UNON,
	LK_ENUM,
	LK_BITY,
	LK_BILD,
	LK_TDEF
}LayoutKind;

typedef struct{
	Position pos;
	union{
		StructData  strc;
		UnionData   unon;
		EnumData    enmt;
		BuildData   bild;
		int64_t     bity;
		int64_t     tdef;
	};
	uint64_t  hash;
	int size, align;
	void*      ast;
	LayoutKind kind;
}Layout;




typedef enum{
	LF_IS_ALIAS		= 0x001,
	LF_IS_TEMPLATE  = 0x002
}LayoutFlags;

typedef struct{
	int64_t 	name, space, tydef;
	int			layoutIx;
	LayoutFlags flags; 
}TypeLayout;

typedef struct{
	List		layouts;	// Layout
	TypeLayout* types;
	int         tyct;
}LayoutTable;


LayoutTable makeLayoutTable (int);
int         makeTypeLayouts (LayoutTable*, ErrorList*, ASTProgram);
void        printLayoutTable(LayoutTable);

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
