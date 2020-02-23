#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "parser.h"
#include "memory.h"
#include "util.h"





int main(){

  FILE*  pFile;
  char*  buffer;
  int    size;
  size_t result;

  pFile = fopen ("exec","r");
  if (pFile == NULL){ printf("Cannot locate exec file."); return 1; }
  fseek (pFile , 0 , SEEK_END);
  size = ftell (pFile);
  rewind (pFile);

  // allocate memory to contain the whole file:
  buffer = malloc (sizeof(uint8_t)*size);
  if (buffer == NULL) { printf("Memory error",stderr); return 2; }

  // copy the file into the buffer:
  result = fread (buffer,1,size,pFile);
  if (result != size) { printf("Reading error",stderr); return 3; }

  /*
  LISP   list[4];
  VALOBJ vars[4];
  vars[0] = makeInt(1);
  vars[1] = makeInt(2);
  vars[2] = makeInt(3);
  vars[3] = makeInt(4);*

  //unflatten(list, vars, 4);

  LISP  pars;
  pars.refc = 1;
  VALOBJ listhead;
  ARR    array;
  array.size = 4;
  array.data = vars;
  array.type = INTTYP;
  listhead.val.PVAL = &array;
  listhead.typ      = ARRTYP;
  pars.here = listhead;
  pars.next = NULL;*/


  PARSERSTATE p;
  p.text = buffer;
  p.head = 0;
  p.size = size;

  LISPENV env;
  env = newEnv();
  env.prog = parseProgram(&p, 64);
  env.heap = newPool(16777216);

  printProgram(env.prog);

  /*
  VALOBJ v = call(0, &pars, env, 0);
  printf("RESULT:\n");
  printVal(v);
  printf("\n");
  */

  fclose (pFile);
}
