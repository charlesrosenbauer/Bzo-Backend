#ifndef __HASHTABLE_HEADER__
#define __HASHTABLE_HEADER__


#include "stdint.h"
#include "stdlib.h"
#include "util.h"



typedef struct{
  uint64_t key;
  void*    val;
}HashEntry;

typedef struct{
  HashEntry*  entries;
  void*       stack;
  int         entrycount, stride, stacktop;
}HashTable;



uint64_t  strToHash      (String);
HashTable makeHashTable  (int,        size_t);
uint64_t  hash           (uint64_t);
int       insertHashTable(HashTable*, uint64_t, void*);
void*     lookupHashTable(HashTable*, uint64_t);


#endif
