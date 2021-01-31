#ifndef __TYPE_HEADER__
#define __TYPE_HEADER__


#include "program.h"

#include "stdint.h"





TypeTable makeTypeTable(int, int);
void      resizeTyTable(TypeTable*, int);

Array  makeArray   ();
Struct makeStruct  (int);
Union  makeUnion   (int);
void   printType   (Type);
int    calcTypeSize(TypeTable*, Type*);

void structIx(Struct*, int, TypeUnion, TypeKind);
void unionIx (Union *, int, TypeUnion, TypeKind);

int  calcArraySize    (TypeTable*, Array*,    int*, int*);
int  calcUnionSize    (TypeTable*, Union*,    int*, int*);
int  calcStructSize   (TypeTable*, Struct*,   int*, int*);
int  calcNamedTypeSize(TypeTable*, NamedType, int*, int*);
void printArray       (Array,     int);
void printStruct      (Struct,    int);
void printUnion       (Union,     int);
void printPrimitive   (Primitive, int);

int  sizeTypeTable    (TypeTable*);


int subtype        (TypeTable*, TypeUnion, TypeUnion, TypeKind, TypeKind);
int isTypePrimitive(TypeTable*, Type, Primitive*);


#endif
