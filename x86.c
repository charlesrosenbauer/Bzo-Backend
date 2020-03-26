#include "codegen.h"
#include "x86.h"



uint8_t x86Opcodes(OPCODE op){
  switch(op){
    case ADDI : return 0;
    case SUBI : return 0;
    case MULI : return 0;
    case MULU : return 0;
    case DIVI : return 0;
    case DIVU : return 0;
    case MODI : return 0;
    case MODU : return 0;
    case AND  : return 0;
    case OR   : return 0;
    case XOR  : return 0;
    case NOT  : return 0;
    case SHL  : return 0;
    case SHR  : return 0;
    case RTL  : return 0;
    case RTR  : return 0;
    case PCT  : return 0;
    case CTZ  : return 0;
    case CLZ  : return 0;

  }
  return 0;
}








// Functions for writing immediates and displacements

int writeU8 (uint8_t* code, uint64_t imm, int ix){
  code[ix] = imm & 0xff;
  return ix + 1;
}

int writeU16(uint8_t* code, uint64_t imm, int ix){
  code[ix+1] =  imm        & 0xff;
  code[ix  ] = (imm >>  8) & 0xff;
  return ix + 2;
}

int writeU32(uint8_t* code, uint64_t imm, int ix){
  code[ix+3] =  imm        & 0xff;
  code[ix+2] = (imm >>  8) & 0xff;
  code[ix+1] = (imm >> 16) & 0xff;
  code[ix  ] = (imm >> 24) & 0xff;
  return ix + 3;
}

int writeU64(uint8_t* code, uint64_t imm, int ix){
  code[ix+7] =  imm        & 0xff;
  code[ix+6] = (imm >>  8) & 0xff;
  code[ix+5] = (imm >> 16) & 0xff;
  code[ix+4] = (imm >> 24) & 0xff;
  code[ix+3] = (imm >> 32) & 0xff;
  code[ix+2] = (imm >> 40) & 0xff;
  code[ix+1] = (imm >> 48) & 0xff;
  code[ix  ] = (imm >> 56) & 0xff;
  return ix + 7;
}



int instructionRegReg(CODEBUFFER* buffer, REGREGINST op){

  if(op.op & 0xff){
    op.b = op.op & 0x7;
  }
  uint8_t pfx = (op.op >> 32);
  uint8_t op0 = (op.op >> 24);
  uint8_t opct= (op.op >> 40) + 1;
  uint8_t op1 = (op.op >> 16);
  uint8_t op2 = (op.op >>  8);

  buffer->bytes[buffer->length] = pfx;
  buffer->length++;

  if(op.size != DWORD){
    // 8 / 16 / 32 bit
    uint8_t mod = 0xC0 | ((op.a & 0x7) << 3) | (op.b & 0x7);
    uint8_t opw = (op.size != BYTE)? 1 : 0;
    uint8_t opd = 0;
    if(op.size == HALF){
      buffer->bytes[buffer->length] = 0x66;
      buffer->length++;
    }
    uint8_t opc = op0 | opd | opw;
    buffer->bytes[buffer->length     ] = opc;
    buffer->bytes[buffer->length+opct] = mod;
    if(opct != 1){
      if(opct == 2){
        buffer->bytes[buffer->length+2 ] = op1;
      }else{
        buffer->bytes[buffer->length+2 ] = op1;
        buffer->bytes[buffer->length+3 ] = op2;
      }
    }
    buffer->length += 1 + opct;
  }else{
    // 64 bit
    uint8_t mod = 0xC0 | ((op.a & 0x7) << 3) | (op.b & 0x7);
    uint8_t opw = (op.size != BYTE)? 1 : 0;
    uint8_t opd = 0;
    uint8_t pfx = 0x48 | ((op.a >> 1) & 8) | ((op.b >> 3) & 1);
    uint8_t opc = op0 | opd | opw;
    buffer->bytes[buffer->length       ] = pfx;
    buffer->bytes[buffer->length+1     ] = opc;
    buffer->bytes[buffer->length+1+opct] = mod;
    if(opct != 1){
      if(opct == 2){
        buffer->bytes[buffer->length+2 ] = op1;
      }else{
        buffer->bytes[buffer->length+2 ] = op1;
        buffer->bytes[buffer->length+3 ] = op2;
      }
    }
    buffer->length += 2 + opct;
  }
  return 0;
}








void appendCodeBlock(CODEBLOCK* a, CODEBLOCK* b, int* ins, int* exs){
  if((a->opcount + b->opcount) > (a->capacity)){
    // resize
  }

  // insert b into a
}