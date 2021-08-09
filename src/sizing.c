#include "postprocess.h"

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "ast.h"
#include "common.h"




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


int dumpToTypeTable(TypeTable* tab, ASTProgram* prog){
	for(int i = 0; i < prog->tyct; i++)
		insertTypeTable(tab, (TypeData){.name=prog->tys[i].tyid, .type=&prog->tys[i].tdef, .kind=TDK_VOID});
	return prog->tyct;
}


int sizeTypes(TypeTable* tab){

	int done = 0, step = 0;
	do{
		step = 0;
		for(int i = 0; i < tab->typect; i++){
			TypeData td = tab->types[i];
			switch(td.kind){
				case TDK_VOID: {
					switch(td.type->type){
						case TT_ELEM : td.kind = TDK_ARRY; break;
						case TT_STRC : td.kind = TDK_STRC; break;
						case TT_UNON : td.kind = TDK_UNON; break;
						case TT_ENUM : td.kind = TDK_ENUM; break;
						default: break;
					}
					step++;
				}break;
				
				case TDK_STRC: {
					if(td.strc.size == -1){
						// Build Struct
						int pass = 1;
						if(td.strc.fieldct == -1){
							ASTStruct strc = td.type->strc;
							// Set fieldct
							td.strc.fieldct = strc.elct;
							
							// Allocate array
							td.strc.fieldIds = malloc(sizeof(int64_t) * strc.elct);
							td.strc.offsets  = malloc(sizeof(int64_t) * strc.elct);
							td.strc.fields   = malloc(sizeof(int64_t) * strc.elct);
						}
					}
				}break;
				
				case TDK_UNON: {
					if(td.unon.size == -1){
						// Build Union
						int pass = 1;
						if(td.unon.fieldct == -1){
							ASTUnion unon = td.type->unon;
							// Set fieldct
							td.unon.fieldct = unon.elct;
							
							// Allocate array
							td.unon.fieldIds = malloc(sizeof(int64_t) * unon.elct);
							td.unon.fields   = malloc(sizeof(int64_t) * unon.elct);
							td.unon.vals     = malloc(sizeof(int64_t) * unon.elct);
						}
					}
				}break;
				
				case TDK_ENUM: {
					if(td.enmt.size == -1){
						// Build Enum
						int pass = 1;
						if(td.enmt.valct == -1){
							ASTEnum enmt = td.type->enmt;
							// Set fieldct
							td.enmt.valct = enmt.tgct;
							
							// Allocate array
							td.enmt.valIds = malloc(sizeof(int64_t) * enmt.tgct);
							td.enmt.vals   = malloc(sizeof(int64_t) * enmt.tgct);
						}
					}
				}break;
				
				case TDK_ARRY: {
					if(td.arry.size == -1){
						// Build Array
					}
				}break;
				
				case TDK_BILD: {
					if(td.bild.size == -1){
						// Build Build
					}
				}break;
			}
			tab->types[i] = td;
		}
	}while(step != 0);
	
	return done == tab->typect;
}



void printTypeTable(TypeTable* t){
	for(int i = 0; i < t->typect; i++){
		TypeData td = t->types[i];
		switch(td.kind){
			case TDK_VOID : printf("%li | VOID\n"             , td.name); break;
			case TDK_STRC :{
					printf("%li | STRC %li | %i %i %i[", td.name, td.strc.id, td.strc.size, td.strc.align, td.strc.fieldct);
					for(int i = 0; i < td.strc.fieldct;   i++) printf("(%i | %li %li @%i);" , i, td.strc.fieldIds[i], td.strc.fields[i], td.strc.offsets[i]);
					printf("]\n");		}break;
			case TDK_UNON :{
					printf("%li | UNON %li | %i %i %i[", td.name, td.unon.id, td.unon.size, td.unon.align, td.unon.fieldct);
					for(int i = 0; i < td.unon.fieldct;   i++) printf("(%i | %li %li =%li);", i, td.unon.fieldIds[i], td.unon.fields[i], td.unon.vals[i]);
					printf("]\n");		}break;
			case TDK_ENUM :{
					printf("%li | ENUM %li | %i %i["   , td.name, td.enmt.id, td.enmt.size, td.enmt.valct);
					for(int i = 0; i < td.enmt.valct;     i++) printf("(%i | %li %li);"     , i, td.enmt.valIds[i], td.enmt.vals[i]);
					printf("]\n");		}break;
			case TDK_ARRY :{
					printf("%li | ARRY %li | %i %i %i[", td.name, td.arry.id, td.arry.size, td.arry.align, td.arry.dimension);
					for(int i = 0; i < td.arry.dimension; i++){
						if(td.arry.sizes[i] > 0){
							printf("[%li]", td.arry.sizes[i]);
						}else if(td.arry.sizes[i] == 0){
							printf("[]");
						}else{
							printf("^");
						}
					}
					printf("%li]\n", td.arry.elem);		}break;
			case TDK_BILD :{
					printf("%li | BILD %li | %i %i %i[%li : ", td.name, td.bild.id, td.bild.size, td.bild.align, td.bild.parct, td.bild.recipe);
					for(int i = 0; i < td.bild.parct;     i++) printf("%li, ", td.bild.pars[i]);
					printf("]\n");		}break;
		}
	}
}





