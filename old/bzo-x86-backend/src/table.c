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



int parseRegister(ParserState* p, Registers* ret){
	char buffer[16];
	ParserState ps = *p;
	if(parseName(p, buffer, 16)){
		if(!strcmp(buffer, "RAX")){
			*ret = R_RA;
			return 1;
		}
		if(!strcmp(buffer, "RBX")){
			*ret = R_RB;
			return 1;
		}
		if(!strcmp(buffer, "RCX")){
			*ret = R_RC;
			return 1;
		}
		if(!strcmp(buffer, "RDX")){
			*ret = R_RD;
			return 1;
		}
		if(!strcmp(buffer, "RBP")){
			*ret = R_BP;
			return 1;
		}
		if(!strcmp(buffer, "RSP")){
			*ret = R_SP;
			return 1;
		}
		if(!strcmp(buffer, "RDI")){
			*ret = R_DI;
			return 1;
		}
		if(!strcmp(buffer, "RSI")){
			*ret = R_SI;
			return 1;
		}
		if(!strcmp(buffer, "R8")){
			*ret = R_R8;
			return 1;
		}
		if(!strcmp(buffer, "R9")){
			*ret = R_R9;
			return 1;
		}
		if(!strcmp(buffer, "R10")){
			*ret = R_R10;
			return 1;
		}
		if(!strcmp(buffer, "R11")){
			*ret = R_R11;
			return 1;
		}
		if(!strcmp(buffer, "R12")){
			*ret = R_R12;
			return 1;
		}
		if(!strcmp(buffer, "R13")){
			*ret = R_R13;
			return 1;
		}
		if(!strcmp(buffer, "R14")){
			*ret = R_R14;
			return 1;
		}
		if(!strcmp(buffer, "R15")){
			*ret = R_R15;
			return 1;
		}


		if(!strcmp(buffer, "XMM0")){
			*ret = R_X0;
			return 1;
		}
		if(!strcmp(buffer, "XMM1")){
			*ret = R_X1;
			return 1;
		}
		if(!strcmp(buffer, "XMM2")){
			*ret = R_X2;
			return 1;
		}
		if(!strcmp(buffer, "XMM3")){
			*ret = R_X3;
			return 1;
		}
		if(!strcmp(buffer, "XMM4")){
			*ret = R_X4;
			return 1;
		}
		if(!strcmp(buffer, "XMM5")){
			*ret = R_X5;
			return 1;
		}
		if(!strcmp(buffer, "XMM6")){
			*ret = R_X6;
			return 1;
		}
		if(!strcmp(buffer, "XMM7")){
			*ret = R_X7;
			return 1;
		}
		if(!strcmp(buffer, "XMM8")){
			*ret = R_X8;
			return 1;
		}
		if(!strcmp(buffer, "XMM9")){
			*ret = R_X9;
			return 1;
		}
		if(!strcmp(buffer, "XMM10")){
			*ret = R_X10;
			return 1;
		}
		if(!strcmp(buffer, "XMM11")){
			*ret = R_X11;
			return 1;
		}
		if(!strcmp(buffer, "XMM12")){
			*ret = R_X12;
			return 1;
		}
		if(!strcmp(buffer, "XMM13")){
			*ret = R_X13;
			return 1;
		}
		if(!strcmp(buffer, "XMM14")){
			*ret = R_X14;
			return 1;
		}
		if(!strcmp(buffer, "XMM15")){
			*ret = R_X15;
			return 1;
		}
		if(!strcmp(buffer, "FLAGS")){
			*ret = R_FLGS;
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
		// TODO: Allow multiple registers to be listed
		if(!parseRegister(p, ret)){
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
		Registers ra, rb, rq, rr, rs;
		char* name = malloc(sizeof(char) * 16);
		if(!parseName (&p, name, 16))
								{ printf("Name    parse failed.\n"); free(props); free(buffer); return NULL; }
		skipWhitespace(&p);

		if(!parseInt  (&p, &lat))
								{ printf("Latency parse failed.\n"); free(props); free(buffer); return NULL; }
		skipWhitespace(&p);

		if(!parsePipes(&p, &a)) { printf("Pipe 0  parse failed.\n"); free(props); free(buffer); return NULL; }
		skipWhitespace(&p);

		if(!parsePipes(&p, &b)) { printf("Pipe 1  parse failed.\n"); free(props); free(buffer); return NULL; }
		skipWhitespace(&p);

		if(!parsePipes(&p, &c)) { printf("Pipe 2  parse failed.\n"); free(props); free(buffer); return NULL; }
		skipWhitespace(&p);

		if(!parsePipes(&p, &d)) { printf("Pipe 3  parse failed.\n"); free(props); free(buffer); return NULL; }
		skipWhitespace(&p);
		

		if(!parseRegisters(&p, &ra))
								{ printf("Register A parse failed.\n"); free(props); free(buffer); return NULL; }
		skipWhitespace(&p);

		if(!parseRegisters(&p, &rb))
								{ printf("Register B parse failed.\n"); free(props); free(buffer); return NULL; }
		skipWhitespace(&p);

		if(!parseRegisters(&p, &rq))
								{ printf("Register Q parse failed.\n"); free(props); free(buffer); return NULL; }
		skipWhitespace(&p);

		if(!parseRegisters(&p, &rr))
								{ printf("Register R parse failed.\n"); free(props); free(buffer); return NULL; }
		skipWhitespace(&p);

		if(!parseRegisters(&p, &rs))
								{ printf("Register S parse failed.\n"); free(props); free(buffer); return NULL; }
		skipWhitespace(&p);


		prop.pipes[0] = a;
		prop.pipes[1] = b;
		prop.pipes[2] = c;
		prop.pipes[3] = d;
		prop.latency  = lat;
		prop.name     = name;

		prop.a = ra;
		prop.b = rb;
		prop.q = rq;
		prop.r = rr;
		prop.s = rs;

		//printf("%i %s\n", pi, name);

		int propIx = findOp(opnames, prop.name);
		if(propIx < 0){ printf("Name is not valid.\n"); free(props); free(buffer); return NULL; };

		props[propIx] = prop;
		pi++;
	}


	free(buffer);
	return props;
}



int getOpcodeLines(char* fname, char** opnames, OpcodeLine* lines){
	uint8_t* buffer  = malloc(sizeof(uint8_t) * 131072);
	int      fsize;
	loadFile(fname, &buffer, &fsize);

	int linect = fsize / 48;
	int ops  = 0;
	for(int i = 0; i < linect; i++){
		OpcodeLine* line = (OpcodeLine*)(&buffer[i * 48]);
		int opIx = findOp(opnames, line->name);
		if(opIx < 0){ printf("Name %s is not valid.\n", (char*)line->name); free(buffer); return 0; };

		lines[opIx] = *line;
		ops++;
	}

	return ops;
}


void printOpLine(OpcodeLine* line){
	printf("%s : %i cycles, %i priority, %u pipes, A:%u, B:%u, C:%u, D:%u",
		line->name, line->latency, line->priority, line->pipes, line->regA, line->regB, line->regQ, line->regR);

	if(line-> setsFlags) printf(" [sets flags]");
	if(line->readsFlags) printf(" [reads flags]");

	printf(" uOps: %i\n", line->uOps);
}