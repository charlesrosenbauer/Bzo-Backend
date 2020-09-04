#include "parser.h"
#include "x86-reader.h"
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "hashtable.h"






int flattenLispBits(LISP* l, uint64_t* xs, int size){
	LISP* lhead = l;
	for(int i = 0; i < size; i++){
		if(lhead == NULL){
			return i;
		}
		xs[i] = lhead->here.val.BVAL;
		lhead = lhead->next;
	}
	if(lhead != NULL){
		return size+1;
	}
	return size;
}



HASHTABLE parseOpcodeTable(PARSERSTATE* state){
	HASHTABLE ret = makeHashTable(1024, sizeof(X86_OPCODE));

	int error = 0;

	while((state->head != state->size) && (state->text[state->head] != '\0')){

		// There's a deliberate memory leak here.
		// Yes, it leaks, but not much enough to really matter. This pass should only happen once.
		// If it becomes a problem, then I'll worry about it.
		// In that case, there's probably a lot more work that should go into memory arenas in the parser.
    	LISP* l = parseLispAlt(state);
    	X86_OPCODE op;

    	if(l != NULL){
    		op.opname          =  lispIx(l, 0).val.SVAL;
    		uint64_t opcodesize = lispIx(l, 1).val.UVAL;
    		if((opcodesize < 1) || (opcodesize > 3)){
    			printf("Invalid opcode size.\n");
    			exit(-1);
			}
      		op.opcodesize      =  lispIx(l, 1).val.UVAL;
      		op.opcode          =  0;
      		op.opcode         |= (lispIx(l, 2).val.UVAL % 256) << 16;
      		op.opcode         |= (lispIx(l, 3).val.UVAL % 256) <<  8;
      		op.opcode         |= (lispIx(l, 4).val.UVAL % 256);
      		op.requiredprefix  =  lispIx(l, 5).val.UVAL % 256;
      		op.rmm             =  lispIx(l, 6).val.UVAL %  16;
      		op.inct            =  lispIx(l, 7).val.UVAL;
      		if((op.inct < 0) || (op.inct > 4)){
      			printf("Too many inputs.\n");
      			exit(-2);
      		}
      		LISP* ins = lispIx(l, 8).val.PVAL;
      		// input masks
      		if(flattenLispBits(ins, op.inmasks, op.inct) != op.inct){
      			printf("Incorrect number of input masks.\n");
      			exit(-3);
      		}
      		
      		op.exct            =  lispIx(l, 9).val.UVAL;
      		if((op.exct < 0) || (op.exct > 4)){
      			printf("Too many outputs.\n");
      			exit(-4);
      		}
      		LISP* exs = lispIx(l, 10).val.PVAL;
      		// output masks
      		if(flattenLispBits(exs, op.exmasks, op.exct) != op.exct){
      			printf("Incorrect number of output masks.\n");
      			exit(-5);
      		}
      		
      		op.pipes            =  lispIx(l, 11).val.BVAL;
      		op.minlatency       =  lispIx(l, 12).val.UVAL;
      		op.maxlatency       =  lispIx(l, 13).val.UVAL;

      		insertHashTable(&ret, strToHash(op.opname), &op);
    	}
  	}

  	return ret;
}




HASHTABLE loadOpcodeTable(char* fname){
	uint8_t* buffer;
	int      fsize;
	loadFile(fname, &buffer, &fsize);

	PARSERSTATE p;
  	p.text = (char*)buffer;
  	p.head = 0;
  	p.size = fsize;

  	return parseOpcodeTable(&p);
}