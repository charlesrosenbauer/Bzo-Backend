#include "types.h"
#include "util.h"
#include "stdlib.h"
#include "stdio.h"


int buildStruct (PROGRAM* prog, LISP* l, TYPE_FIELD* fields){
  return 0;
}

int buildUnion (PROGRAM* prog, LISP* l, TYPE_FIELD* fields){
  return 0;
}

int buildTagged(PROGRAM* prog, LISP* l, TYPE_FIELD* fields){
  return 0;
}




int buildType(PROGRAM* prog, TYPE* t){
  LISP* def = t->data.tydef;
  if(def == NULL) return 1;

  VALOBJ tag = lispIx(def, 0);
  if(tag.typ != OPRTYP) return 2;

  switch(tag.val.OVAL){
    case STRCT : {

    }break;

    case UNION : {

    }break;

    case TAGGD : {

    }break;

    default: return 3;
  }

  // TODO: Clean up definition

  return 0;
}



int buildTypes(PROGRAM* prog){

  for(int i = 0; i < prog->tyct; i++){
    int err = buildType(prog, &prog->types[i]);
    if(err != 0){
      return err;
    }
  }

  return 0;
}
