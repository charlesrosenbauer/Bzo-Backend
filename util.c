#include "interpreter.h"
#include "util.h"
#include "stdlib.h"
#include "stdio.h"



VALOBJ lispIx(LISP* l, int ix){
  if(l == NULL){
    printf("Unexpected nil list.\n");
    exit(9);
  }
  for(int i = 1; i <= ix; i++){
    l = l->next;
    if(l == NULL){
      printf("Tried to access element %ix of list, only got to element %i.\n", ix, i);
      exit(10);
    }
  }
  return l->here;
}
