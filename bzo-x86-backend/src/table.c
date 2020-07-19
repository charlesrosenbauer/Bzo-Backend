#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "defs.h"


void loadFile(char* fname, uint8_t** buffer, int* fsize){
  FILE*  pFile = fopen (fname,"r");
  size_t result;
  if (pFile == NULL){ printf("Cannot locate file %s.", fname); exit(1); }
  fseek (pFile , 0 , SEEK_END);
  *fsize = ftell (pFile);
  rewind (pFile);

  // allocate memory to contain the whole file:
  *buffer = malloc (sizeof(uint8_t)*(*fsize));
  if (buffer == NULL) { printf("Memory error", stderr); exit(2); }

  // copy the file into the buffer:
  result = fread (*buffer,1,(*fsize),pFile);
  if (result != (*fsize)) { printf("Reading error", stderr); exit(3); }

  fclose(pFile);
}



typedef struct{
  char*  text;
  int    head;
  int    size;
}ParserState;



int parseComment(ParserState* state){
  char c = state->text[state->head];
  if(c != ';'){
    return 0;
  }
  int ix = state->head+1;
  while((state->text[ix] != '\n') && (ix < state->size)){
    ix++;
  }
  state->head = ix;
  return 1;
}



void skipWhitespace(ParserState* state){
  while(1){
    char c = state->text[state->head];
    if((c == ' ') || (c == '\t') || (c == '\v') || (c == '\n')){
      state->head++;
    }else if(c == ';'){
      parseComment(state);
    }else{
      return;
    }
    if(state->head == state->size){
      return;
    }
  }
}


int parseBitset(ParserState* state, uint64_t* ret){
  char c = state->text[state->head];
  *ret = 0;
  if(c != 'b'){
    return 0;
  }
  int ix = state->head+1;
  while((state->text[ix] == '#') || (state->text[ix] == '_')){
    *ret += *ret;
    if(state->text[ix] == '#'){
      *ret |= 1;
    }
    ix++;
  }
  state->head = ix-1;
  return 1;
}


OpcodeProperties* loadOpProps(char* fname){
	uint8_t* buffer  = malloc(sizeof(uint8_t) * 131072);
	int      fsize;
	loadFile(fname, &buffer, &fsize);

	OpcodeProperties* props = malloc(sizeof(OpcodeProperties) * 256);
	ParserState p = {(char*)buffer, 0, fsize};


	free(buffer);
	return props;
}