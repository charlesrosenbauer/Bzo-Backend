#include "stdio.h"

#include "program.h"
#include "codegen.h"
#include "type.h"
#include "func.h"
#include "util.h"
#include "parser.h"
#include "langparser.h"




void x86test(){
	X86Func fnc = makeX86Func (3, 16);
	appendX86Var  (&fnc);
	appendX86Block(&fnc);
	appendX86Op(&fnc.blocks[0]);
	appendX86Op(&fnc.blocks[0]);
	appendX86Op(&fnc.blocks[0]);
	appendX86Op(&fnc.blocks[0]);
	
	fnc.blocks[0].ops[0] = (X86Op){XO_IMUL, SC_64, NOREG, NOREG, 0,    0,  1,  3, -1, CC_NOCODE, XF_NOFLAGS};
	fnc.blocks[0].ops[1] = (X86Op){XO_IMUL, SC_64, NOREG, NOREG, 0,    1,  2,  4, -1, CC_NOCODE, XF_NOFLAGS};
	fnc.blocks[0].ops[2] = (X86Op){XO_ADD , SC_64, NOREG, NOREG, 0,    3,  4,  5, -1, CC_NOCODE, XF_NOFLAGS};
	fnc.blocks[0].ops[3] = (X86Op){XO_RET , SC_64, NOREG, NOREG, 0,   -1, -1, -1, -1, CC_NOCODE, XF_NOFLAGS};
	
	for(int i =  0; i < 12; i++){ fnc.blocks[0].invars[i] = -1; fnc.blocks[0].exvars[i] = -1; }
	for(int i = 12; i < 16; i++){ fnc.blocks[0].invars[i] = -2; fnc.blocks[0].exvars[i] = -2; }
	for(int i = 16; i < 32; i++){ fnc.blocks[0].invars[i] = -3; fnc.blocks[0].exvars[i] = -3; }
	fnc.blocks[0].invars[RSP] = -3;
	fnc.blocks[0].invars[RBP] = -3;
	fnc.blocks[0].exvars[RSP] = -3;
	fnc.blocks[0].exvars[RBP] = -3;
	
	fnc.blocks[0].invars[RSI] = 0;
	fnc.blocks[0].invars[RDI] = 1;
	fnc.blocks[0].invars[R8 ] = 2;
	fnc.blocks[0].exvars[RSI] = 5;
	
	printX86Func(&fnc);
	
	x86AllocRegs(&fnc.blocks[0]);
	
	printf("==========\n");
	printX86Func(&fnc);
	
	uint8_t program[256];
	int size = compileBlock(&fnc.blocks[0], program);
	
	printf("Program is %i bytes.\n", size);
	for(int i = 0; i < size; i++){
		printf("%02x ", program[i]);
	}
	printf("\n");
}


void parsetest(){
	uint8_t* file;
	int      fsize = 0;
	loadFile    ("tests/exec", &file, &fsize);
	
	Program p = parseProgram(file, fsize);
	if(!sizeTypeTable(&p.types)) printf("Size fail\n");
	
	buildFuncTable(&p);
	
	printf("Fns=%i, Tys=%i\n", p.funcs.fnct, p.types.tyct);
	for(int i = 0; i < p.types.tyct; i++) printType(p.types.types[i], 1);
	for(int i = 0; i < p.funcs.fnct; i++) printFunc(p.funcs.funcs[i]);
}


void langtest(){
	uint8_t* file;
	int      fsize = 0;
	loadFile    ("tests/main.bzo", &file, &fsize);
	
	printf("%s", file);
	LangReader lr = (LangReader){(char*)file, fsize, 0, 0, 0, 0};
	
	SymbolTable tab = makeSymbolTable(64);
	
	LexerState ls   = lexer(&lr);
	symbolizeTokens(&tab, &ls);
	printLexerState(ls);
}


int main(){
	//parsetest();
	langtest();
}
