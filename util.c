#include "util.h"
#include "stdlib.h"
#include "stdio.h"



void loadFile(char* fname, uint8_t** buffer, int* fsize){
  FILE*  pFile = fopen (fname,"r");
  size_t result;
  if (pFile == NULL){ printf("Cannot locate exec file."); exit(1); }
  fseek (pFile , 0 , SEEK_END);
  *fsize = ftell (pFile);
  rewind (pFile);

  // allocate memory to contain the whole file:
  *buffer = malloc (sizeof(uint8_t)*(*fsize));
  if (buffer == NULL) { printf("Memory error",stderr); exit(2); }

  // copy the file into the buffer:
  result = fread (*buffer,1,(*fsize),pFile);
  if (result != (*fsize)) { printf("Reading error",stderr); exit(3); }

  fclose(pFile);
}




void printLisp(Lisp* l){
  Lisp* here = l;
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
      case BSTTYP : {
        printf("b");
        for(int i = (64 - __builtin_clzl(here->here.val.BVAL))-1; i >= 0; i--)
          printf((here->here.val.BVAL & (1l << i))? "#" : "_");
        printf(" ");
      }break;
      default:      printf("%i "  , here->here.typ          ); break;
    }
    here = here->next;
  }
  printf(") ");
}



Valobj lispIx(Lisp* l, int ix){
  if(l == NULL){
    printf("Unexpected nil list.\n");
    exit(9);
  }
  for(int i = 1; i <= ix; i++){
    l = l->next;
    if(l == NULL){
      printf("Tried to access element %i of list, only got to element %i.\n", ix, i-1);
      exit(10);
    }
  }
  return l->here;
}




void printProgram(Program* prog){
  printf("PROGRAM:\n\n");
  for(int i = 0; i < prog->fnct; i++){
    //printf("  Function %i, %i parameters, code:", i, prog->funcs[i].parct);
    //printLisp(prog->funcs[i].code);
    printf("\n");
  }


  for(int i = 0; i < prog->tyct; i++){
    //printf("  Type %i, %i bytes, definition:", i, prog->types[i].fieldct);
    //printLisp(prog->types[i].data.tydef);
    printf("\n");
  }
  printf("DONE\n");
}




int lispSize(Lisp* l){
  int size = 0;
  Lisp* here = l;
  while(here != NULL){
    size++;
    here = here->next;
  }
  return size;
}



void printHexBuffer(uint8_t* buffer, int size){
	for(int i = 0; i < size; i++){
		uint8_t x = buffer[i];
		char a = (x % 16), b = (x / 16);
		a += (a > 9)? ('A'-10) : '0';
		b += (b > 9)? ('A'-10) : '0';
		printf("%c%c ", b, a);
	}
	printf("\n");
}
