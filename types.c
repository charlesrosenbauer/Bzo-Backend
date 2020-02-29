#include "types.h"
#include "util.h"
#include "stdlib.h"
#include "stdio.h"



int builtPoly(PROGRAM* prog, LISP* fields){

  if(fields == NULL) return 1;

  if((fields->here.typ != LSPTYP) || (fields->here.val.UVAL != POLY)) return 2;

  int size = lispSize(fields) - 1;
  TYPE_FIELD* fieldlist = malloc(sizeof(TYPE_FIELD) * size);

  // more stuff

  return 0;
}




int buildType(PROGRAM* prog, TYPEDEF* t, int* err){

  return 0;

  LISP* lisp = t->type;
  int size = lispSize(lisp);

  TYPE_FIELD* fields = malloc(sizeof(TYPE_FIELD) * size);
  LISP* here = lisp;
  for(int i = 0; i < size; i++){
    TYPE_FIELD f;
    /*
      This part will be a little complex.

      FLAT, POINTER, REFERENCE, and ARRAY should be pretty simple.

      CMPD and POLY require recursion, though poly is going to be a bit tricky.
      POLY produces a tagged union rather than a struct, which means all fields
      have the same offset, but there is an additional byte or two to store the
      tag. It also means that the total size of the union is based on the
      maximum field size, not the sum of field sizes.

      I have no idea how type variables are going to be handled yet.
    */


    fields[i] = f;
    here = lisp->next;
  }

  t->type = fields;
  return 0;
}









int buildTypes(PROGRAM* prog){

  int cont  = 1;
  while(cont){
    cont = 0;
    int built = 0;
    int tried = 0;
    for(int i = 0; i < prog->tyct; i++){
      int err = 0;

      // If type is not built yet, try to build it.
      if(prog->types[i].alignment == 0){
        tried++;
        if(!buildType(prog, &prog->types[i], &err)){
          cont = 1;
          built++;
        }
        if(err != 0){
          return err;
        }
      }
    }
    printf("%i %i\n", built, tried);
    if(tried == 0){
      // All done!
      return 0;
    }
    if(built == 0){
      // Recursion!
      return 1;
    }
  }

  return 0;
}






int getSizeAlign(LISP* ty, int* size, int* align){

  int newSize = 0;
  int newAlign= 1;

  LISP* head = ty;
  while(head != NULL){
    switch(head->here.typ){
      case TYPTYP : {
        switch(head->here.val.UVAL){

          // TODO: Adjust size calculations to account for alignment
          case T_I8 : {
            newSize  += 1;
          }break;

          case T_I16 : {
            newSize  += 2;
            newAlign  = (newAlign >  2)? newAlign :  2;
          }break;

          case T_I32 : {
            newSize  += 4;
            newAlign  = (newAlign >  4)? newAlign :  4;
          }break;

          case T_I64 : {
            newSize  += 8;
            newAlign  = (newAlign >  8)? newAlign :  8;
          }break;

          case T_I128 : {
            newSize  += 16;
            newAlign  = (newAlign > 16)? newAlign : 16;
          }break;

          case T_U8 : {
            newSize  += 1;
          }break;

          case T_U16 : {
            newSize  += 2;
            newAlign  = (newAlign >  2)? newAlign :  2;
          }break;

          case T_U32 : {
            newSize  += 4;
            newAlign  = (newAlign >  4)? newAlign :  4;
          }break;

          case T_U64 : {
            newSize  += 8;
            newAlign  = (newAlign >  8)? newAlign :  8;
          }break;

          case T_U128 : {
            newSize  += 16;
            newAlign  = (newAlign > 16)? newAlign : 16;
          }break;

          case T_F16 : {
            newSize  += 2;
            newAlign  = (newAlign >  2)? newAlign :  2;
          }break;

          case T_F32 : {
            newSize  += 4;
            newAlign  = (newAlign >  4)? newAlign :  4;
          }break;

          case T_F64 : {
            newSize  += 8;
            newAlign  = (newAlign >  8)? newAlign :  8;
          }break;

          case T_STR : {
            newSize  += 16;
            newAlign  = (newAlign >  8)? newAlign :  8;
          }break;

          case T_ARR : {
            newSize  += 24;
            newAlign  = (newAlign >  8)? newAlign :  8;
          }break;

          case T_PTR : {
            newSize  += 8;
            newAlign  = (newAlign >  8)? newAlign :  8;
          }break;
        }
      }break;

      case LSPTYP : {
        // TODO
      }break;

      default: return 0;
    }
    head = head->next;
  }

  *size  = newSize;
  *align = newAlign;
  return 1;
}
