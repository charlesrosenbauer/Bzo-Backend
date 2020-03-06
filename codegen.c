#include "codegen.h"
#include "util.h"




int insertInstruction(CODEBUFFER* buffer, MACHINEINSTRUCTION instruction){

  if((buffer->length + 3) >= buffer->capacity){
    return -1;
  }

  int pos = buffer->length;

  switch(instruction.op){
    case ADDI : {
      buffer->bytes[pos  ] = 0x48;
      buffer->bytes[pos+1] = 0x01;
      buffer->bytes[pos+2] = 0xC0 | (instruction.a << 3) | (instruction.b);
      buffer->length += 3;
    }break;

    case SUBI : {
      buffer->bytes[pos  ] = 0x48;
      buffer->bytes[pos+1] = 0x01;
      buffer->bytes[pos+2] = 0xC0 | (instruction.a << 3) | (instruction.b);
      buffer->length += 3;
    }break;

    case XOR  : {
      buffer->bytes[pos  ] = 0x48;
      buffer->bytes[pos+1] = 0x31;
      buffer->bytes[pos+2] = 0xC0 | (instruction.a << 3) | (instruction.b);
      buffer->length += 3;
    }break;

    case OR   : {
      buffer->bytes[pos  ] = 0x48;
      buffer->bytes[pos+1] = 0x09;
      buffer->bytes[pos+2] = 0xC0 | (instruction.a << 3) | (instruction.b);
      buffer->length += 3;
    }break;

    case AND  : {
      buffer->bytes[pos  ] = 0x48;
      buffer->bytes[pos+1] = 0x21;
      buffer->bytes[pos+2] = 0xC0 | (instruction.a << 3) | (instruction.b);
      buffer->length += 3;
    }break;

    default: return -1;
  }

  return 0;
}
