#ifndef __CODEGEN_HEADER__
#define __CODEGEN_HEADER__


#include "stdint.h"
#include "util.h"







typedef enum{
  BYTE, HALF, WORD, DWORD, V128, V256
}OPSIZE;

typedef enum{
  RAX =  0,
  RBX =  3,
  RCX =  1,
  RDX =  2,
  RSP =  4,
  RBP =  5,
  RSI =  6,
  RDI =  7,
  R8  =  8,
  R9  =  9,
  R10 = 10,
  R11 = 11,
  R12 = 12,
  R13 = 13,
  R14 = 14,
  R15 = 15
}REGISTER;


typedef struct{
  OPCODE op;
  REGISTER  a, b, c;
  uint8_t   prefixes[4];
  OPSIZE    size;
  uint32_t  disp;
  uint64_t  imm;
}MACHINEINSTRUCTION;


typedef struct{
  OPCODE    op;
  REGISTER  a, b;
  OPSIZE    size;
  uint64_t  imm;
  int8_t    hasImm;
}REGREGINST;

typedef struct{
  OPCODE    op;
  REGISTER  x;
  OPSIZE    size;
  int       offset;
  uint64_t  imm;
  int8_t    hasImm;
}REGSTACKINST;


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
