#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "parser.h"
#include "memory.h"
#include "util.h"
#include "types.h"
#include "x86-reader.h"
#include "hashtable.h"
#include "elf.h"





int main(){

  FILE*  pFile;
  char*  buffer;
  int    size;
  size_t result;

  loadFile("exec", &buffer, &size);  

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


  HASHTABLE x86tab = loadOpcodeTable("x86ops");
  printf("%i\n", x86tab.stacktop);

  uint8_t* elfbuffer;
  int elfsize = makeELF(&elfbuffer, NULL, 0, NULL, 0);
  writeELF("program", elfbuffer, elfsize);
}
