#ifndef __CODEGEN_HEADER__
#define __CODEGEN_HEADER__


#include "stdint.h"
#include "util.h"









typedef struct{
  OPCODE op;
  int    a, b, c;
  uint64_t  imm;
}INSTRUCTION;


typedef struct{
  INSTRUCTION*  instructions;
  int           opcount, ins, exs;
}CODEBLOCK;


typedef struct{
  uint8_t*  bytes;
  int capacity, length;
}CODEBUFFER;







int insertInstruction(CODEBUFFER*, INSTRUCTION);




#endif
