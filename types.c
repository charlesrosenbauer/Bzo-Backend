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


int buildType(Type* t, Program* p, int tyIx, uint64_t* builts){
	/*
	if(t->parct != 0){
		// I don't know how to handle this case yet.
		// Just mark it as done and make it recognizeable [poisoned].
		t->size = -1;
		t->align= -1;
		builts[tyIx/64] |= (1l << (tyIx % 64));
		return 1;
	}
	
	switch(t->kind){
		case TK_STRUCT:{
			
		}break;
		
		case TK_UNION:{
		
		}break;
		
		case TK_PTR:{
			t->size  =  8;
			t->align =  8;
		}break;
		
		case TK_VAL:{
		
		}break;
		
		case TK_ARR:{
			t->size  = 24;
			t->align =  8;
		}break;
		
		case TK_FUNC:{
			t->size  = 16;
			t->align =  8;
		}break;
		
		case TK_TVAR:{
		
		}break;
	}*/
	
	return 1;
}



int buildTypes(Program* prog){

	int tyWords = (prog->tyct / 64) + ((prog->tyct % 64) != 0);
	uint64_t* builtTys = malloc(sizeof(uint64_t) * tyWords);
	for(int i = 0; i < tyWords; i++) builtTys[i] = 0;
	
	int built = 0;
	int last  = 0;
	while(built < prog->tyct){
		for(int i = 0; i < prog->tyct; i++){
			uint64_t mask = (1l << (i % 64));
			if(!(builtTys[i/64] & mask)){
				// Type has yet to be built. Try to build it!
				Type* t = &prog->types[i];
				int tProgress = buildType(t, prog, i, builtTys);
				if(tProgress == -1) return 0;
				built += tProgress;
			}
		}
		
		if(built == last){
			// !!Circular Dependency Found!!
			printf("Circular Dependency Found Among Types!\n");
			return 0;
		}
		last = built;
	}
	return 1;
}
















