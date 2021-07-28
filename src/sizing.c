#include "postprocess.h"

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "ast.h"
#include "program.h"




TypeTable makeTypeTable(int size){
	TypeTable ret;
	ret.types   = malloc(sizeof(TypeData) * size);
	ret.typect  = 0;
	ret.typecap = size;
	return ret;
}

int       insertTypeTable(TypeTable* tab, TypeData ty){
	if(tab->typect+1 >= tab->typecap){
		TypeData* tmp = tab->types;
		tab->types    = malloc(sizeof(TypeData) * tab->typecap * 2);
		for(int i = 0; i < tab->typect; i++) tab->types[i] = tmp[i];
		free(tmp);
		tab->typecap *= 2;
	}
	tab->types[tab->typect] = ty;
	tab->typect++;
	return tab->typect-1;
}


int sizeStruct(TypeTable* tab, ASTStruct* strc){
	int size    = 0;
	int align   = 1;
	ASTType* ty = malloc(sizeof(ASTType));
	ty->pos  = strc->pos;
	ty->strc = *strc;
	ty->type = TT_STRC;
	
	
	
	TypeData td;
	td.type = ty;
	
	
	return insertTypeTable(tab, td);
}


int sizeUnion(TypeTable* tab, ASTUnion* unon){
	int size    = 0;
	int align   = 1;
	ASTType* ty = malloc(sizeof(ASTType));
	ty->pos  = unon->pos;
	ty->unon = *unon;
	ty->type = TT_UNON;
	
	
	
	TypeData td;
	td.type = ty;
	
	
	return insertTypeTable(tab, td);
}


int sizeEnum(TypeTable* tab, ASTEnum* enmt){
	int size    = 0;
	int align   = 1;
	ASTType* ty = malloc(sizeof(ASTType));
	ty->pos  = enmt->pos;
	ty->enmt = *enmt;
	ty->type = TT_ENUM;
	
	
	
	TypeData td;
	td.type = ty;
	
	
	return insertTypeTable(tab, td);
}

int sizeElem(TypeTable* tab, ASTTyElem* elem){
	int size    = 0;
	int align   = 1;
	ASTType* ty = malloc(sizeof(ASTType));
	ty->pos  = elem->pos;
	ty->elem = *elem;
	ty->type = TT_ELEM;
	
	
	
	TypeData td;
	td.type = ty;
	
	
	return insertTypeTable(tab, td);
}



int sizeType(TypeTable* tab, ASTType* ty){
	switch(ty->type){
		case TT_STRC : {
			return sizeStruct(tab, &ty->strc);
		}break;
		
		case TT_UNON : {
			return sizeUnion (tab, &ty->unon);
		}break;
		
		case TT_ENUM : {
			return sizeEnum  (tab, &ty->enmt);
		}break;
		
		case TT_ELEM : {
			return sizeElem  (tab, &ty->elem);
		}break;
		
		case TT_VOID : break;
	}
	return -1;
}
