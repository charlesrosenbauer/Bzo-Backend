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




#endif
