#ifndef __X86_HEADER__
#define __X86_HEADER__



#include "stdint.h"
#include "util.h"
#include "codegen.h"



typedef enum{
  // Last 4 bits encode:
  // XYYY :
  //   X  = does this opcode use R/M field?
  //   Ys = contents of R/M field
  X86_ADD       = 0x000,
  X86_PUSH      = 0x060,
  X86_POP       = 0x070,
  X86_OR        = 0x080,
  X86_LEA       = 0x8D0,
  X86_CALL      = 0x9A0,
  X86_PUSHF     = 0x9C0,
  X86_POPF      = 0x9D0,
  X86_SAHF      = 0x9E0,
  X86_LAHF      = 0x9F0,
  X86_ADC       = 0x100,
  X86_SBB       = 0x180,
  X86_AND       = 0x200,
  X86_SUB       = 0x280,
  X86_XOR       = 0x300,
  X86_CMP       = 0x380,
  X86_INC       = 0x400,
  X86_DEC       = 0x480,
  X86_IMUL      = 0x690,
  X86_JCC       = 0x700,
  X86_TEST      = 0x840,
  X86_XCHG      = 0x860,
  X86_NOP       = 0x900,
  X86_CBW       = 0x980,
  X86_CWD       = 0x990,
  X86_MOV       = 0xA00,
  X86_MOVSB     = 0xA40,
  X86_MOVSW     = 0xA50,
  X86_CMPSB     = 0xA60,
  X86_CMPSW     = 0xA70,
  X86_STOSB     = 0xAA0,
  X86_STOSW     = 0xAB0,
  X86_LODSB     = 0xAC0,
  X86_LODSW     = 0xAD0,
  X86_SCASB     = 0xAE0,
  X86_SCASW     = 0xAF0,
  X86_ROL       = 0xC08,
  X86_ROR       = 0xC09,
  X86_RCL       = 0xC0A,
  X86_RCR       = 0xC0B,
  X86_SAL       = 0xC0C,
  X86_SAR       = 0xC0D,
  X86_SHL       = 0xC0E,
  X86_SHR       = 0xC0F,
  X86_RETN      = 0xC20,
  X86_LES       = 0xC40,
  X86_LDS       = 0xC50,
  X86_INT       = 0xCC0,
  X86_INTO      = 0xCE0,
  X86_IRET      = 0xCF0,
  X86_XLAT      = 0xD70,
  X86_LOOP      = 0xE00,
  X86_JCXZ      = 0xE30,
  X86_JMP       = 0xE90,
  X86_HLT       = 0xF40,
  X86_CMC       = 0xF50,
  X86_NOT       = 0xF68,
  X86_NEG       = 0xF69,
  X86_MUL       = 0xF6A,
  X86_DIV       = 0xF6B,
  X86_IDIV      = 0xF6C,
  X86_CLC       = 0xF80,
  X86_STC       = 0xF90,
  X86_CLI       = 0xFA0,
  X86_STI       = 0xFB0,
  X86_CLD       = 0xFC0,
  X86_STD       = 0xFD0


}X86OPCODE;




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



int insertInstruction(CODEBUFFER*, MACHINEINSTRUCTION);



#endif
