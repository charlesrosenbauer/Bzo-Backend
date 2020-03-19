#ifndef __HASHTABLE_HEADER__
#define __HASHTABLE_HEADER__


#include "stdint.h"
#include "stdlib.h"


typedef struct{
  uint64_t key;
  void*    val;
}HASHENTRY;

typedef struct{
  HASHENTRY*  entries;
  void*       stack;
  int         entrycount, stride, stacktop;
}HASHTABLE;



HASHTABLE makeHashTable  (int,        size_t);
uint64_t  hash           (uint64_t);
int       insertHashTable(HASHTABLE*, uint64_t, void*);
void*     lookupHashTable(HASHTABLE*, uint64_t);


#endif
