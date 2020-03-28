#ifndef __X86_HEADER__
#define __X86_HEADER__



#include "stdint.h"
#include "util.h"
#include "codegen.h"



typedef enum{
  // Encoding:
  // XX AA BB CC DD EE
  // XX : Number of opcode bytes (+1)
  // AA : Required Prefix byte (00 if none. 66 and 48 are encoded by other means)
  // BB : Primary   Opcode
  // CC : Secondary Opcode (00 if none)
  // DD : Tertiary  Opcode (00 if none)
  // EE : 0X, encoding additional R/M bits

  // Last 4 bits encode:
  // XYYY :
  //   X  = does this opcode use R/M field?
  //   Ys = contents of R/M field
  X86_ADD       = 0x000000000000,
  X86_PUSH      = 0x000006000000,
  X86_POP       = 0x000007000000,
  X86_OR        = 0x000008000000,
  X86_LEA       = 0x00008D000000,
  X86_CALL      = 0x00009A000000,
  X86_PUSHF     = 0x00009C000000,
  X86_POPF      = 0x00009D000000,
  X86_SAHF      = 0x00009E000000,
  X86_LAHF      = 0x00009F000000,
  X86_ADC       = 0x000010000000,
  X86_SBB       = 0x000018000000,
  X86_AND       = 0x000020000000,
  X86_SUB       = 0x000028000000,
  X86_XOR       = 0x000030000000,
  X86_CMP       = 0x000038000000,
  X86_INC       = 0x000040000000,
  X86_DEC       = 0x000048000000,
  X86_IMUL      = 0x000069000000,
  X86_JCC       = 0x000070000000,
  X86_TEST      = 0x000084000000,
  X86_XCHG      = 0x000086000000,
  X86_NOP       = 0x000090000000,
  X86_CBW       = 0x000098000000,
  X86_CWD       = 0x000099000000,
  X86_MOV       = 0x0000A0000000,
  X86_MOVSB     = 0x0000A4000000,
  X86_MOVSW     = 0x0000A5000000,
  X86_CMPSB     = 0x0000A6000000,
  X86_CMPSW     = 0x0000A7000000,
  X86_STOSB     = 0x0000AA000000,
  X86_STOSW     = 0x0000AB000000,
  X86_LODSB     = 0x0000AC000000,
  X86_LODSW     = 0x0000AD000000,
  X86_SCASB     = 0x0000AE000000,
  X86_SCASW     = 0x0000AF000000,
  X86_ROL       = 0x0000C0000008,
  X86_ROR       = 0x0000C0000009,
  X86_RCL       = 0x0000C000000A,
  X86_RCR       = 0x0000C000000B,
  X86_SAL       = 0x0000C000000C,
  X86_SAR       = 0x0000C000000D,
  X86_SHL       = 0x0000C000000E,
  X86_SHR       = 0x0000C000000F,
  X86_RETN      = 0x0000C2000000,
  X86_LES       = 0x0000C4000000,
  X86_LDS       = 0x0000C5000000,
  X86_INT       = 0x0000CC000000,
  X86_INTO      = 0x0000CE000000,
  X86_IRET      = 0x0000CF000000,
  X86_XLAT      = 0x0000D7000000,
  X86_LOOP      = 0x0000E0000000,
  X86_JCXZ      = 0x0000E3000000,
  X86_JMP       = 0x0000E9000000,
  X86_HLT       = 0x0000F4000000,
  X86_CMC       = 0x0000F5000000,
  X86_NOT       = 0x0000F6080000,
  X86_NEG       = 0x0000F6000009,
  X86_MUL       = 0x0000F600000A,
  X86_DIV       = 0x0000F600000B,
  X86_IDIV      = 0x0000F600000C,
  X86_CLC       = 0x0000F8000000,
  X86_STC       = 0x0000F9000000,
  X86_CLI       = 0x0000FA000000,
  X86_STI       = 0x0000FB000000,
  X86_CLD       = 0x0000FC000000,
  X86_STD       = 0x0000FD000000,
  X86_LCKADD    = 0xF00000000000


}X86OPCODE;




typedef enum{
  BYTE, HALF, WORD, DWORD, V128, V256
}OPSIZE;

typedef enum{
  RR, RC, RS, RI
}INSTTYPE;

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
  X86OPCODE op;
  REGISTER  a, b;
  OPSIZE    size;
}REGREGINST;

typedef struct{
  X86OPCODE op;
  REGISTER  a;
  OPSIZE    size;
  uint64_t  imm;
}REGCONSTINST;

typedef struct{
  X86OPCODE op;
  REGISTER  x;
  OPSIZE    size;
  int       offset;
  uint64_t  imm;
  int8_t    hasImm;
}REGSTACKINST;


typedef enum{
  BRANCH,
  PHI,
  LOOP,
  LEXIT
}CTRLINST;


typedef struct{
  union{
    REGREGINST    rr;
    REGSTACKINST  rs;
    REGCONSTINST  rc;
    CTRLINST      ct;
  }instruction;
  INSTTYPE  type;
  uint16_t  a, b, c;
}MACHINEINSTRUCTION;


typedef struct{
  MACHINEINSTRUCTION*  instructions;
  int                  opcount, ins, exs, capacity;
}CODEBLOCK;



void appendCodeBlock(CODEBLOCK*, CODEBLOCK*, int*, int*);



#endif