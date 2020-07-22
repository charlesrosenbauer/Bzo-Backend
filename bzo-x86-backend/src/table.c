#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"

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


int parseName(ParserState* state, char* buffer, int buflimit){
	int ix = state->head;
	for(int i = 0; i < buflimit; i++){
		char c = state->text[state->head];
		if(((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z'))){
			state->head++;
			buffer[i] = c;
		}else{
			buffer[i] = '\0';
			return 1;
		}
	}
	state->head = ix;
	return 0;
}


int parseInt(ParserState* state, uint64_t* ret){
	*ret = 0;
	int ix = state->head;
	int step = 0;
	while(1){
		char c = state->text[state->head];
		if((c >= '0') && (c <= '9')){
			state->head++;
			*ret = ((*ret) * 10) + (c - '0');
			step++;
		}else{
			return (step > 0);
		}
	}
	state->head = ix;
	return 0;
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
  state->head = ix;
  return 1;
}


int parsePipes(ParserState* p, Pipes* ret){
	char buffer[16];
	ParserState ps = *p;
	if(parseName(p, buffer, 16)){
		if(!strcmp(buffer, "NIL")){
			*ret = 0;
			return 1;
		}
		if(!strcmp(buffer, "INT")){
			*ret = INTS_P;
			return 1;
		}
		if(!strcmp(buffer, "DMA")){
			*ret = DMAS_P;
			return 1;
		}
		if(!strcmp(buffer, "FPU")){
			*ret = FPUS_P;
			return 1;
		}
		if(!strcmp(buffer, "FULL")){
			*ret = FULL_P;
			return 1;
		}
		if(!strcmp(buffer, "I")){
			skipWhitespace(p);
			uint64_t bitset = 0;
			if(!parseBitset(p, &bitset)){
				*p = ps;
				return 0;
			}
			*ret = (bitset & 0xf) * INT0_P;
			return 1;
		}
		if(!strcmp(buffer, "M")){
			skipWhitespace(p);
			uint64_t bitset = 0;
			if(!parseBitset(p, &bitset)){
				*p = ps;
				return 0;
			}
			*ret = (bitset & 0x7) * DMA0_P;
			return 1;
		}
		if(!strcmp(buffer, "F")){
			skipWhitespace(p);
			uint64_t bitset = 0;
			if(!parseBitset(p, &bitset)){
				*p = ps;
				return 0;
			}
			*ret = (bitset & 0xf) * FPU0_P;
			return 1;
		}
	}

	return 0;
}



int parseRegisters(ParserState* p, Registers* ret){
	char buffer[16];
	ParserState ps = *p;
	if(parseName(p, buffer, 16)){
		if(!strcmp(buffer, "NIL")){
			*ret = 0;
			return 1;
		}
		if(!strcmp(buffer, "GPR")){
			*ret = R_GPR;
			return 1;
		}
		if(!strcmp(buffer, "XMM")){
			*ret = R_XMM;
			return 1;
		}
		if(!strcmp(buffer, "GMN")){
			*ret = R_GMN;
			return 1;
		}
		if(!strcmp(buffer, "SPC")){
			*ret = R_SPC;
			return 1;
		}
	}

	return 0;
}




OpcodeProperties* loadOpProps(char** opnames, char* fname){
	uint8_t* buffer  = malloc(sizeof(uint8_t) * 131072);
	int      fsize;
	loadFile(fname, &buffer, &fsize);

	OpcodeProperties* props = malloc(sizeof(OpcodeProperties) * 256);
	ParserState p = {(char*)buffer, 0, fsize};

	int ix = 0;
	int pi = 0;
	while(p.head < (p.size-1)){
		skipWhitespace(&p);

		OpcodeProperties prop;
		uint64_t lat;
		Pipes a, b, c, d;
		char* name = malloc(sizeof(char) * 16);
		if(!parseName (&p, name, 16))
								{ printf("Name    parse failed.\n"); free(props); free(buffer); return NULL; };
		skipWhitespace(&p);

		if(!parseInt  (&p, &lat)){ printf("Latency parse failed.\n"); free(props); free(buffer); return NULL; }
		skipWhitespace(&p);

		if(!parsePipes(&p, &a)){ printf("Pipe 0  parse failed.\n"); free(props); free(buffer); return NULL; }
		skipWhitespace(&p);

		if(!parsePipes(&p, &b)){ printf("Pipe 1  parse failed.\n"); free(props); free(buffer); return NULL; }
		skipWhitespace(&p);

		if(!parsePipes(&p, &c)){ printf("Pipe 2  parse failed.\n"); free(props); free(buffer); return NULL; }
		skipWhitespace(&p);

		if(!parsePipes(&p, &d)){ printf("Pipe 3  parse failed.\n"); free(props); free(buffer); return NULL; }
		skipWhitespace(&p);

		prop.pipes[0] = a;
		prop.pipes[1] = b;
		prop.pipes[2] = c;
		prop.pipes[3] = d;
		prop.latency  = lat;
		prop.name     = name;


		//printf("%i %s\n", pi, name);

		int propIx = findOp(opnames, prop.name);
		if(propIx < 0){ printf("Name is not valid.\n"); free(props); free(buffer); return NULL; };

		props[propIx] = prop;
		pi++;
	}


	free(buffer);
	return props;
}