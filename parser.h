#ifndef __PARSER_HEADER__
#define __PARSER_HEADER__

#include "interpreter.h"
#include "stdlib.h"

typedef struct{
  char*  text;
  int    head;
  int    size;

  LISP** defs;
}PARSERSTATE;

LISP* parseLispAlt(PARSERSTATE*);
void  printVal (VALOBJ);
PROGRAM* parseProgram(PARSERSTATE*, int);


#endif
