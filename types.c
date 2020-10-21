#include "types.h"
#include "util.h"
#include "stdlib.h"
#include "stdio.h"

/*
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
}*/


int getPrimitiveSizeAlign(int t, int* size, int* align){
	switch(t){
		case PT_I8   :{ *size  =  1; *align = 1; return 1; } break;
		case PT_I16  :{ *size  =  2; *align = 2; return 1; } break;
		case PT_I32  :{ *size  =  4; *align = 4; return 1; } break;
		case PT_I64  :{ *size  =  8; *align = 8; return 1; } break;
		case PT_U8   :{ *size  =  1; *align = 1; return 1; } break;
		case PT_U16  :{ *size  =  2; *align = 2; return 1; } break;
		case PT_U32  :{ *size  =  4; *align = 4; return 1; } break;
		case PT_U64  :{ *size  =  8; *align = 8; return 1; } break;
		case PT_F16  :{ *size  =  2; *align = 2; return 1; } break;
		case PT_F32  :{ *size  =  4; *align = 4; return 1; } break;
		case PT_F64  :{ *size  =  8; *align = 8; return 1; } break;
		case PT_CHAR :{ *size  =  4; *align = 8; return 1; } break;
		case PT_BOOL :{ *size  =  1; *align = 1; return 1; } break;
		case PT_ARR  :{ *size  = 24; *align = 8; return 1; } break;
		case PT_STR  :{ *size  = 16; *align = 8; return 1; } break;
	}
	return 0;
}



int buildTypes(Program* prog){

	
}
















