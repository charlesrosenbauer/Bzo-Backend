#include "parser.h"
#include "x86-reader.h"
#include "stdint.h"
#include "stdlib.h"
#include "hashtable.h"







HASHTABLE parseOpcodeTable(PARSERSTATE* state){
	HASHTABLE ret = makeHashTable(1024, sizeof(X86_OPCODE));

	int error = 0;

	while((state->head != state->size) && (state->text[state->head] != '\0')){
    	LISP* l = parseLispAlt(state);
    	X86_OPCODE op;

    	if(l != NULL){
    		op.opname     =  lispIx(l, 0).val.SVAL;
      		op.opcodesize =  lispIx(l, 1).val.UVAL;
      		op.opcode     = (lispIx(l, 2).val.UVAL % 256) << 16;
      		op.opcode     = (lispIx(l, 3).val.UVAL % 256) <<  8;
      		op.opcode     = (lispIx(l, 4).val.UVAL % 256);
    	}
  	}

  	return ret;
}

