#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "langparser.h"



uint64_t symbolHash(char* sym){
	uint64_t ret = 718397189153151;
	int ix = 0;
	while(sym[ix] != 0){
		ret    ^=  sym[ix];
		int rot =  ret % 64;
		ret     = (ret << rot) | (ret >> (64 - rot));
		ix++;
	}
	ret += ix;
	if(ret == 0) ret += ix;		// this should almost never fire of course
	return ret;
}




SymbolTable makeSymbolTable(int size){
	SymbolTable ret;
	ret.syms = malloc(sizeof(Symbol) * size);
	ret.size = size;
	ret.fill = 0;
	for(int i = 0; i < size; i++){
		ret.syms[i].text = NULL;
		ret.syms[i].hash = 0;
	}
	return ret;
}


void insertSymbol(SymbolTable*, Symbol);

void growSymbolTable(SymbolTable* tab){
	Symbol* tmp = tab->syms;
	int oldsize = tab->size;
	int oldfill = tab->fill;
	*tab = makeSymbolTable(oldsize * 2);
	tab->fill   = oldfill;
	for(int i = 0; i < oldsize; i++) if(tmp[i].text != NULL) insertSymbol(tab, tmp[i]);
	free(tab);
}

void insertSymbol(SymbolTable* tab, Symbol s){
	if((tab->fill + 1) >= tab->size) growSymbolTable(tab);
	int offset = s.hash % tab->size;
	for(int i = 0; i < tab->size; i++){
		int ix = i + offset;
		ix = (ix >= tab->size)? ix - tab->size : ix;
		if(tab->syms[i].hash == 0){
			tab->syms[i] = s;
			tab->fill++;
			return;
		}
	}
}

void insertSymbolText(SymbolTable* tab, char* text){
	Symbol s;
	s.text = text;
	s.hash = symbolHash(text);
	insertSymbol(tab, s);
}

Symbol searchSymbol(SymbolTable* tab, Symbol s){
	int offset = s.hash % tab->size;
	Symbol ret;
	ret.hash = 0;
	ret.text = NULL;
	for(int i = 0; i < tab->size; i++){
		int ix = i + offset;
		ix = (ix >= tab->size)? ix - tab->size : ix;
		if(tab->syms[i].hash == s.hash){
			if(!strcmp(tab->syms[i].text, s.text)) return tab->syms[i];
		}else if(tab->syms[i].hash == 0){
			return ret;
		}
	}
	return ret;
}


void printSymbolTable(SymbolTable tab){
	for(int i = 0; i < tab.size; i++)
		if(tab.syms[i].hash != 0) printf("%i : #%lu => %s\n", i, tab.syms[i].hash, tab.syms[i].text);
}


char* printToken(Token tk){
	switch(tk.type){
		case TKN_PERIOD    : return "<TK : . >";
		case TKN_COLON     : return "<TK : : >";
		case TKN_SEMICOLON : return "<TK : ; >";
		case TKN_NEWLINE   : return "<TK : \\n>";
	}
	
	return "<?>";
}


void printLexerState(LexerState ls){
	for(int i = 0; i < ls.tkct; i++){
		Position p = ls.tks[i].pos;
		printf("%i : %s @ (%i:%i - %i:%i | %i)\n", i, printToken(ls.tks[i]), p.lineStart, p.colStart, p.lineEnd, p.colEnd, p.fileId);
	}
}




LexerState lexer(LangReader* lr){
	LexerState ls;
	ls.tks   = malloc(sizeof(Token) * lr->size / 4);
	ls.tkct  = 0;
	ls.tkcap = lr->size / 4;
	
	for(int i = 0; i < lr->size; i++){
		char c = lr->text[i];
		
		if(c == ' '){
			lr->column++;
		}else if(c == '\t'){
			lr->column += 4;
		}else if(c == '\n'){
			ls.tks[ls.tkct] = (Token){TKN_NEWLINE, (Position){lr->fileId, lr->line, lr->line+1, lr->column, 0}};
			lr->column  = 0;
			lr->line++;
			ls.tkct++;
		}else if(c == '\v'){
			ls.tks[ls.tkct] = (Token){TKN_NEWLINE, (Position){lr->fileId, lr->line, lr->line+1, lr->column, lr->column+1}};
			lr->column++;
			lr->line++;
			ls.tkct++;
		}else if(c == '.'){
			ls.tks[ls.tkct] = (Token){TKN_PERIOD, (Position){lr->fileId, lr->line, lr->line, lr->column, lr->column+1}};
			lr->column++;
			ls.tkct++;
		}
	}
	
	return ls;
}


























