#ifndef __STRUCT_HEADER__
#define __STRUCT_HEADER__


#include "stdint.h"


typedef struct{
	int32_t* vals;
	uint64_t bloom[2];
	int size, cap;
}Set32;


Set32 initSet32  (int);
int   insertSet32(Set32*, int32_t);
int   checkSet32 (Set32*, int32_t);
void  copySet32  (Set32*, Set32*);
Set32 intersect32(Set32 , Set32);
Set32 union32    (Set32 , Set32);
void  printSet32 (Set32);





#endif
