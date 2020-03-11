#include "codegen.h"
#include "util.h"


uint8_t x86Opcodes(OPCODE op){
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


// Generate x86_64 instruction (machine code)
int insertInstruction(CODEBUFFER* buffer, MACHINEINSTRUCTION instruction){

  uint8_t code[15];
  int insthead = 0;

  // Add prefix bytes
  for(int i = 0; i < 4; i++){
    if(instruction.prefixes[i] != 0){
      code[insthead] = instruction.prefixes[i];
      insthead++;
    }else{
      break;
    }
  }



  return 0;
}




int instructionRegReg(CODEBUFFER* buffer, REGREGINST op){
  if(op.size != DWORD){
    // 8 / 16 / 32 bit
    uint8_t mod = 0xC0 | ((op.a & 0x7) << 3) | (op.b & 0x7);
    uint8_t opw = (op.size != BYTE)? 1 : 0;
    uint8_t opd = 0;
    if(op.size == HALF){
      buffer->bytes[buffer->length] = 0x66;
      buffer->length++;
    }
    uint8_t opc = x86Opcodes(op.op) | opd | opw;
    buffer->bytes[buffer->length  ] = opc;
    buffer->bytes[buffer->length+1] = mod;
    buffer->length += 2;
  }else{
    // 64 bit
    uint8_t mod = 0xC0 | ((op.a & 0x7) << 3) | (op.b & 0x7);
    uint8_t opw = (op.size != BYTE)? 1 : 0;
    uint8_t opd = 0;
    uint8_t pfx = 0x48 | ((op.a >> 1) & 8) | ((op.b >> 3) & 1);
    uint8_t opc = x86Opcodes(op.op) | opd | opw;
    buffer->bytes[buffer->length  ] = pfx;
    buffer->bytes[buffer->length+1] = opc;
    buffer->bytes[buffer->length+2] = mod;
    buffer->length += 3;
  }
  return 0;
}
