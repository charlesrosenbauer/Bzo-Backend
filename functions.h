#ifndef __FUNCTION_HEADER__
#define __FUNCTION_HEADER__


#include "stdint.h"
#include "util.h"





typedef struct{
  OPCODE  op;
  uint8_t a, b, c, d, e, f;
}INSTRUCTION;

/*
  This is supposed to be similar to an extended basic block (single entrance,
  multiple exits), however function calls and higher-order functions are not
  considered exits.
*/
typedef struct{
  INSTRUCTION*  code;
  uint64_t*     exits;
  int codesize, inct, exct, exitct;
}BLOCK;

typedef struct{
  BLOCK*  blocks;
  int     blockct;
}PROGRAMBLOCKS;




#endif
