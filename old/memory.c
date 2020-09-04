#include "memory.h"
#include "util.h"
#include "stdlib.h"
#include "stdint.h"







POOL newPool(int poolsize){
  POOL ret;
  int fillsize = 1 + (poolsize / 64);
  ret.heap = malloc(sizeof(LISP)     * poolsize);
  ret.fill = malloc(sizeof(uint64_t) * fillsize);
  //ret.dump = malloc(sizeof(LISP*)    * dumpsize);

  for(int i = 0; i < fillsize; i++) ret.fill[i] = 0;
  //for(int i = 0; i < dumpsize; i++) ret.dump[i] = NULL;

  ret.size     = poolsize;
  //ret.dumpsize = dumpsize;
  ret.heaptop  = 0;
  //ret.dumptop  = 0;
  ret.cleanIx  = 0;

  return ret;
}


LISP* alloc(POOL* p){
  LISP* ret = NULL;
  if(p->heaptop < p->size){
    // Stack fill
    ret = &p->heap[p->heaptop];
    int ix = p->heaptop / 64;
    int jx = p->heaptop % 64;
    p->fill[ix] |= (1l << jx);
    p->heaptop++;
    return ret;
  }else{
    // Cleanup
    for(int i = 0; i < p->cleanIx; i++){
      if(p->fill[i] != 0xffffffffffffffff){
        int ix = __builtin_clzl(~p->fill[i]);
        p->fill[i] |= (1l << ix);
        return &(p->heap[(i * 64) + ix]);
      }
    }
  }
  return NULL;
}


void freeStr(STRING* str){
  str->refc--;
  if(str->refc == 0){
    free(str->text);
  }
}


void freeLisp(POOL* p, LISP* l){
  if(l->refc == 1){
    switch(l->here.typ){
      case PTRTYP : freeLisp(p, l->here.val.PVAL); break;
      case STRTYP : freeStr (&(l->here.val.SVAL)); break;
      case LSPTYP : freeLisp(p, l->here.val.PVAL); break;
      default     : break;
    }
    freeLisp(p, l->next);
    int ix = (p->heap - l) / sizeof(LISP);
    uint64_t mask = ~(1l << (ix % 64));
    p->fill[ix/64] &= mask;
  }else{
    l->refc--;
  }
}
