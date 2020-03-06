#ifndef __CODEGEN_HEADER__
#define __CODEGEN_HEADER__


#include "stdint.h"
#include "util.h"







typedef enum{
  BYTE, HALF, WORD, DWORD, V128, V256
}OPSIZE;


typedef struct{
  OPCODE op;
  int    a, b, c;
  OPSIZE    size;
  uint32_t  disp;
  uint64_t  imm;
}MACHINEINSTRUCTION;


typedef struct{
  MACHINEINSTRUCTION*  instructions;
  int                  opcount, ins, exs;
}CODEBLOCK;


typedef struct{
  uint8_t*  bytes;
  int capacity, length;
}CODEBUFFER;







int insertInstruction(CODEBUFFER*, MACHINEINSTRUCTION);




#endif
