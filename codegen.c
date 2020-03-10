#include "codegen.h"
#include "util.h"


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
