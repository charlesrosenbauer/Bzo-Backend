#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "interpreter.h"
#include "parser.h"





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


  PARSERSTATE p;
  p.text = buffer;
  p.head = 0;
  p.size = size;

  LISPENV env;
  env.vars  = NULL;
  env.varct = 0;

  env.prog = parseProgram(&p, 64);

  VALOBJ v = eval(env.prog->funcs[0].code, env);
  printVal(v);
  printf("\n");

  fclose (pFile);
}
