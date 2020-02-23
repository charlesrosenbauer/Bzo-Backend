#include "util.h"
#include "stdlib.h"
#include "stdio.h"



void printLisp(LISP* l){
  LISP* here = l;
  if(here == NULL){
    printf("nil ");
    return;
  }

  printf("(");
  while(here != NULL){
    switch(here->here.typ){
      case FNCTYP : printf("f%lu ", here->here.val.UVAL     ); break;
      case INTTYP : printf("i%li ", here->here.val.IVAL     ); break;
      case UNTTYP : printf("u%lu ", here->here.val.UVAL     ); break;
      case FLTTYP : printf("r%f ",  here->here.val.FVAL     ); break;
      case STRTYP : printf("s%i ",  here->here.val.SVAL.size); break;
      case LSPTYP : printLisp(here->here.val.PVAL           ); break;
      case VARTYP : printf("v%lu ", here->here.val.UVAL     ); break;
      case OPRTYP : printf("x%lu ", here->here.val.UVAL     ); break;
      case TYPTYP : printf("t%lu ", here->here.val.UVAL     ); break;
      case HOLTYP : printf("HOLE"                           ); break;
      default:      printf("%i "  , here->here.typ          ); break;
    }
    here = here->next;
  }
  printf(") ");
}



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




void printProgram(PROGRAM* prog){
  printf("PROGRAM:\n\n");
  for(int i = 0; i < prog->fnct; i++){
    printf("  Function %i, %i parameters, code:", i, prog->funcs[i].prct);
    printLisp(prog->funcs[i].code);
    printf("\n");
  }
  printf("DONE\n");
}
