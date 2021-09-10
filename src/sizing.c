#include "postprocess.h"

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "ast.h"
#include "common.h"



void getTypeSizeAlign(int64_t x, int* size, int* align){
	*size  = -1;
	*align = -1;
	switch(x){
		case BID_I8		: {*size =   1; *align =  1;} break;
		case BID_I16 	: {*size =   2; *align =  2;} break;
		case BID_I32	: {*size =   4; *align =  4;} break;
		case BID_I64	: {*size =   8; *align =  8;} break;
		case BID_I128	: {*size =  16; *align = 16;} break;
		case BID_I256	: {*size =  32; *align = 32;} break;
		case BID_U8		: {*size =   1; *align =  1;} break;
		case BID_U16	: {*size =   2; *align =  2;} break;
		case BID_U32	: {*size =   4; *align =  4;} break;
		case BID_U64	: {*size =   8; *align =  8;} break;
		case BID_U128	: {*size =  16; *align = 16;} break;
		case BID_U256	: {*size =  32; *align = 32;} break;
		case BID_BOOL	: {*size =   1; *align =  1;} break;
		case BID_F16	: {*size =   2; *align =  2;} break;
		case BID_F32	: {*size =   4; *align =  4;} break;
		case BID_F64	: {*size =   8; *align =  8;} break;
		case BID_P8		: {*size =   1; *align =  1;} break;
		case BID_P16	: {*size =   2; *align =  2;} break;
		case BID_P32	: {*size =	 4; *align =  4;} break;
		case BID_P64	: {*size =   8; *align =  8;} break;
		case BID_Q8		: {*size =   8; *align =  8;} break;		// Quire for P8  is   64 bits
		case BID_Q16	: {*size =  32; *align = 32;} break;		// Quire for P16 is  256 bits
		case BID_Q32	: {*size =  64; *align = 32;} break;		// Quire for P32 is  512 bits
		case BID_Q64	: {*size = 256; *align = 32;} break;		// Quire for P64 is 2048 bits
		default 		: {*size =   0; *align =  0;} break;
	}
}




LayoutTable makeLayoutTable(int tyct){
	LayoutTable ret;
	ret.layouts = makeList(tyct * 4, sizeof(Layout));
	ret.types   = malloc(sizeof(TypeLayout) * tyct);
	ret.tyct    = tyct;
	for(int i   = 0; i < tyct; i++) ret.types[i] = (TypeLayout){0, 0, 0, 0, 0};
	return ret;
}

void printLayoutTable(LayoutTable tab){
	printf("\n====LTAB====\n");
	for(int i = 0; i < tab.layouts.size; i++){
		Layout* l  = getList(&tab.layouts, i);
		char* kind = "????";
		switch(l->kind){
			case LK_STRC : kind = "STRC"; break;
			case LK_UNON : kind = "UNON"; break;
			case LK_ENUM : kind = "ENUM"; break;
			case LK_BITY : kind = "BITY"; break;
			case LK_BILD : kind = "BILD"; break;
			case LK_TDEF : kind = "TDEF"; break;
		}
		printf("L%i : %p %s (%i|%i) \n", i, l->ast, kind, l->size, l->align);
	}
	for(int i = 0; i < tab.tyct; i++){
		if(tab.types[i].tydef != 0){
			TypeLayout l = tab.types[i];
			char*     as = (l.flags & LF_IS_ALIAS   )? "ALIAS"    : "";
			char*     tm = (l.flags & LF_IS_TEMPLATE)? "TEMPLATE" : "";
			printf("%i | %li:%li [%li] %i %s%s\n", i, l.name, l.space, l.tydef, l.layoutIx, as, tm);
		}
	}
	printf("\n====LTAB====\n");
}



int buildStructLayout(LayoutTable* ltab, ASTType* type){
	return 1;
}


int buildUnionLayout(LayoutTable* ltab, ASTType* type){
	return 1;
}

int buildEnumLayout(LayoutTable* ltab, ASTType* type){
	return 1;
}

int buildAtomLayout(LayoutTable* ltab, ASTType* type){
	return 1;
}

int buildElemLayout(LayoutTable* ltab, ASTType* type){
	return 1;
}



int  sizeType(LayoutTable* ltab, ASTProgram prog, ErrorList* errs, int tyid){
	ASTTyDef* tdef = getList(&prog. tys    , tyid);
	Layout*      l = getList(&ltab->layouts, tyid);
	if(l->size == 0){
		switch(l->kind){
			case LK_STRC: break;
			case LK_UNON: break;
			case LK_ENUM: break;
			case LK_BITY:{
				getTypeSizeAlign(l->bity, &l->size, &l->align);
				if(l->size < 0){
					appendList(&errs->errs, &(Error){ERR_T_BAD_BITY, tdef->tdef.pos});
					return 0;
				}
				return 1;
			} break;
			case LK_BILD: break;
			case LK_TDEF:{
				if(l->tdef < 0){
					switch(tdef->tdef.kind){
						case TK_ELEM: {
							buildElemLayout(ltab, &tdef->tdef);
						}break;
						
						case TK_STRC: {
							buildStructLayout(ltab, &tdef->tdef);
						}break;
						
						case TK_UNON: {
							buildUnionLayout(ltab, &tdef->tdef);
						}break;
						
						case TK_ENUM: {
							buildEnumLayout(ltab, &tdef->tdef);
						}break;
						
						case TK_ATOM: {
							buildAtomLayout(ltab, &tdef->tdef);
						}break;
					}
				}else{
					Layout* ldef = getList(&ltab->layouts, l->tdef);
					if(ldef->size > 0){
						l->size  = ldef->size;
						l->align = ldef->align;
						return 1;
					}
				}
			}break;
		}
	}
	return 0;
}


int makeTypeLayouts(LayoutTable* ltab, ErrorList* errs, ASTProgram prog){
	if(0){
		error:
		return 0;
	}

	for(int i = 0; i < prog.tys.size; i++){
		ASTTyDef* tdef = getList(&prog.tys, i);
		Layout l       = (Layout){.pos=tdef->pos, .tdef=-1, .hash=0, .size=0, .align=0, .ast=tdef, .kind=LK_TDEF};
		appendList(&ltab->layouts, &l);
	}

	
	int step = 0;
	int cont = 1;
	int size = ltab->layouts.size;
	while(cont){
		int newStep = 0;
		
		for(int i = 0; i < prog.tys.size; i++){
			newStep += sizeType(ltab, prog, errs, i);
			
		}
		
		// TODO: check if progress has been made. If not, fail
		break;
	}
	return 1;
}


