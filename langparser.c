#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

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
	for(int i = 0; i < size; i++){
		ret.syms[i].text = NULL;
		ret.syms[i].hash = 0;
	}
	return ret;
}




void growSymbolTable(SymbolTable* tab){
	Symbol* tmp = tab->syms;
	int oldsize = tab->size;
	*tab = makeSymbolTable(oldsize * 2);
	for(int i = 0; i < oldsize; i++){
		if(tmp[i].text != NULL){
			
		}
	}
}


void printSymbolTable(SymbolTable tab){
	for(int i = 0; i < tab.size; i++)
		if(tab.syms[i].hash != 0) printf("%i : #%lu => %s\n", i, tab.syms[i].hash, tab.syms[i].text);
}


