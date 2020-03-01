#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "parser.h"
#include "memory.h"
#include "util.h"
#include "types.h"





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

  PROGRAM* prog;
  prog = parseProgram(&p, 64, 64);

  int err = buildTypes(prog);
  if(err != 0){
    printf("Error building types.\n");
  }

  printProgram(prog);


  fclose (pFile);
}
