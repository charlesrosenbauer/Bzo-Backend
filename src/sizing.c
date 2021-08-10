#include "postprocess.h"

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "ast.h"
#include "common.h"



void getTypeSizeAlign(int64_t x, int* size, int* align){
	switch(x){
		case BID_I8		: {*size =  1; *align =  1;} break;
		case BID_I16 	: {*size =  2; *align =  2;} break;
		case BID_I32	: {*size =  4; *align =  4;} break;
		case BID_I64	: {*size =  8; *align =  8;} break;
		case BID_I128	: {*size = 16; *align = 16;} break;
		case BID_I256	: {*size = 32; *align = 32;} break;
		case BID_U8		: {*size =  1; *align =  1;} break;
		case BID_U16	: {*size =  2; *align =  2;} break;
		case BID_U32	: {*size =  4; *align =  4;} break;
		case BID_U64	: {*size =  8; *align =  8;} break;
		case BID_U128	: {*size = 16; *align = 16;} break;
		case BID_U256	: {*size = 32; *align = 32;} break;
		case BID_BOOL	: {*size =  1; *align =  1;} break;
		case BID_F16	: {*size =  2; *align =  2;} break;
		case BID_F32	: {*size =  4; *align =  4;} break;
		case BID_F64	: {*size =  8; *align =  8;} break;
		default 		: {*size =  0; *align =  0;} break;
	}
}




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


int dumpToTypeTable(TypeTable* tab, ASTProgram* prog){
	for(int i = 0; i < prog->tyct; i++)
		insertTypeTable(tab, (TypeData){.name=prog->tys[i].tyid, .type=&prog->tys[i].tdef, .kind=TDK_VOID});
	return prog->tyct;
}


int sizeTypes(TypeTable* tab){
	
	/*
		Redo sizeTypes:
		* Build basic types first
		* Build subtypes as needed in subsequent passes
		* Mark polymorphic types
		* Build polymorphic type instances as needed
	*/
	for(int i = 0; i < tab->typect; i++){
		TypeData td = tab->types[i];
		switch(td.type->type){
			case TT_ELEM : {
				td.kind = TDK_ARRY;
			} break;
			
			case TT_STRC : {
				td.kind = TDK_STRC;
				ASTStruct strc = td.type->strc;
				td.strc.fieldct = strc.elct;
				td.strc.fieldIds = malloc(sizeof(int64_t) * strc.elct);
				td.strc.offsets  = malloc(sizeof(int64_t) * strc.elct);
				td.strc.fields   = malloc(sizeof(int64_t) * strc.elct);
				for(int i = 0; i < strc.elct; i++){
					td.strc.fieldIds[i] = strc.vals[i];
					td.strc.fields  [i] = -1;
					td.strc.offsets [i] = 0;
				}
			} break;
			
			case TT_UNON : {
				td.kind = TDK_UNON;
				ASTUnion unon = td.type->unon;
				td.unon.fieldct = unon.elct;
				td.unon.fieldIds = malloc(sizeof(int64_t) * unon.elct);
				td.unon.fields   = malloc(sizeof(int64_t) * unon.elct);
				td.unon.vals     = malloc(sizeof(int64_t) * unon.elct);
				for(int i = 0; i < unon.elct; i++){
					td.unon.fieldIds[i] = unon.tags[i];
					td.unon.fields  [i] = -1;
					td.unon.vals    [i] = unon.vals[i];
				}
			} break;
			
			case TT_ENUM : {
				td.kind = TDK_ENUM;
				ASTEnum enmt = td.type->enmt;
				td.enmt.valct = enmt.tgct;
				td.enmt.valIds = malloc(sizeof(int64_t) * enmt.tgct);
				td.enmt.vals   = malloc(sizeof(int64_t) * enmt.tgct);
				for(int i = 0; i < enmt.tgct; i++){
					td.enmt.valIds[i] = enmt.tags[i];
					td.enmt.vals  [i] = enmt.vals[i];
				}
			} break;
		}
		tab->types[i] = td;
	}
	
	return 0;
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





