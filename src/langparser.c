#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "langparser.h"




int parseTypeDef(LexerState* tks, SymbolTable* tab, ASTProgram* prog, int tix, int pix){
	
	return 0;
}


int parseFuncDef(LexerState* tks, SymbolTable* tab, ASTProgram* prog, int tix, int pix){
	
	return 0;
}


int parseCode(LexerState* tks, SymbolTable* tab, ASTProgram* prog){
	int tix = 0;
	int pix = 0;
	while(tix < tks->tkct){
		int skip;
		if(tks->tks[tix].type == TKN_NEWLINE){
			tix ++;
			continue;
		}
		
		skip = parseFuncDef(tks, tab, prog, tix, pix);
		if(skip > 1){
			tix += skip;
			continue;
		}
		
		skip = parseTypeDef(tks, tab, prog, tix, pix);
		if(skip > 1){
			tix += skip;
			continue;
		}
		
		printf("Could not parse file...\n");
		return -1;
	}

	return 0;
}
