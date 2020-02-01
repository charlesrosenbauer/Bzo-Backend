#include "codegen.h"




int insertInstruction(CODEBUFFER* buffer, INSTRUCTION instruction){

  if((buffer->length + 3) >= buffer->capacity){
    return -1;
  }

  int pos = buffer->length;

  switch(instruction.op){
    case ADDI : {
      buffer[pos  ] = 0x48;
      buffer[pos+1] = 0x01;
      buffer[pos+2] = 0xC0 | (instruction.a << 3) | (instruction.b);
      buffer->length+3;
    }break;

    case SUBI : {
      buffer[pos  ] = 0x48;
      buffer[pos+1] = 0x01;
      buffer[pos+2] = 0xC0 | (instruction.a << 3) | (instruction.b);
      buffer->length+3;
    }break;

    case XOR  : {
      buffer[pos  ] = 0x48;
      buffer[pos+1] = 0x31;
      buffer[pos+2] = 0xC0 | (instruction.a << 3) | (instruction.b);
      buffer->length+3;
    }break;

    case OR   : {
      buffer[pos  ] = 0x48;
      buffer[pos+1] = 0x09;
      buffer[pos+2] = 0xC0 | (instruction.a << 3) | (instruction.b);
      buffer->length+3;
    }break;

    case AND  : {
      buffer[pos  ] = 0x48;
      buffer[pos+1] = 0x21;
      buffer[pos+2] = 0xC0 | (instruction.a << 3) | (instruction.b);
      buffer->length+3;
    }break;

    default: return -1;
  }

  return 0;
}
