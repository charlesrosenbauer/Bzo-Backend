#ifndef __LATTICE_HEADER__
#define __LATTICE_HEADER__


#include "util.h"
#include "functions.h"
#include "stdint.h"



typedef struct{
  uint64_t* bits;
  int size, varct;
}LATTICE;



#endif
