#ifndef __TYPES_HEADER__
#define __TYPES_HEADER__


#include "stdint.h"






typedef enum{
  FLAT, POINTER, REFERENCE, DYNARRAY, STCARRAY, CMPD, POLY
}TYPE_KIND;


typedef struct{
  int32_t   offset;
  int32_t   type;
  void*     subtype;
  TYPE_KIND kind;
}TYPE_FIELD;



#endif
