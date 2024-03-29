#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "common.h"









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
	ret.idct = 1;
	for(int i = 0; i < size; i++){
		ret.syms[i].text = NULL;
		ret.syms[i].hash = 0;
		ret.syms[i].flag = SF_AMBI;
	}
	return ret;
}


int  insertSymbol(SymbolTable*, Symbol);

void growSymbolTable(SymbolTable* tab){
	Symbol* tmp = tab->syms;
	int oldsize = tab->size;
	int oldfill = tab->fill;
	*tab = makeSymbolTable(oldsize * 2);
	tab->fill   = oldfill;
	for(int i = 0; i < oldsize; i++) if(tmp[i].text != NULL) insertSymbol(tab, tmp[i]);
	free(tab);
}

int insertSymbol(SymbolTable* tab, Symbol s){
	if((tab->fill + 1) >= tab->size) growSymbolTable(tab);
	int offset = s.hash % tab->size;
	for(int i = 0; i < tab->size; i++){
		int ix = i + offset;
		ix = (ix >= tab->size)? ix - tab->size : ix;
		if(tab->syms[i].hash == 0){
			s.id = tab->idct;
			tab->syms[i] = s;
			tab->fill++;
			tab->idct++;
			return tab->idct-1;
		}else if(tab->syms[i].hash == s.hash){
			if(!strcmp(tab->syms[i].text, s.text)) return tab->syms[i].id;
		}
	}
	return -1;
}

int insertSymbolText(SymbolTable* tab, char* text, SymbolFlags flag){
	Symbol s;
	s.text = text;
	s.hash = symbolHash(text);
	s.flag = flag;
	return insertSymbol(tab, s);
}

Symbol searchSymbol(SymbolTable* tab, Symbol s){
	int offset = s.hash % tab->size;
	Symbol ret;
	ret.hash = 0;
	ret.text = NULL;
	ret.id   = -1;
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

char* searchSymbolId(SymbolTable* tab, int id){
	if(tab->syms[id-1].hash != 0) return tab->syms[id-1].text;
	return "";
}


char* printSymbolFlag(SymbolFlags flag){
	switch(flag){
		case SF_AMBI : return " __ ";
		case SF_ID   : return " ID ";
		case SF_MID  : return " MI ";
		case SF_TYID : return " TI ";
		case SF_TVAR : return " TV ";
		default      : return " ?? ";
	}
}


void printSymbolTable(SymbolTable tab){
	for(int i = 0; i < tab.size; i++)
		if(tab.syms[i].hash != 0) printf("%i : #%lu => %s (%s) \n", tab.syms[i].id, tab.syms[i].hash, tab.syms[i].text, printSymbolFlag(tab.syms[i].flag));
}
