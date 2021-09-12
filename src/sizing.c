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
			case LK_FNTY : kind = "FNTY"; break;
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



int buildStructLayout(LayoutTable* ltab, int tyid, ASTType* type){
	return 1;
}


int buildUnionLayout(LayoutTable* ltab, int tyid, ASTType* type){
	return 1;
}

int buildEnumLayout(LayoutTable* ltab, int tyid, ASTType* type){
	return 1;
}

int buildAtomLayout(LayoutTable* ltab, int tyid, ASTType* type){
	return 1;
}

int buildElemLayout(LayoutTable* ltab, int tyid, ASTType* type){
	return 1;
}



int unfoldType  (LayoutTable*, ASTType*);

int unfoldStruct(LayoutTable* ltab, ASTStruct* strc){
	int  size = strc->names.size;
	Layout  l = (Layout    ){.size=0, .align=0, .ast=strc, .kind=LK_STRC};
	l.strc    = (StructData){.fieldTys=makeList(size+1, sizeof(int64_t)),
							 .fieldIds=makeList(size+1, sizeof(int64_t)),
							 .offsets =makeList(size+1, sizeof(int64_t))};
	if(0){
		fail:
		freeList(&l.strc.fieldTys);
		freeList(&l.strc.fieldIds);
		freeList(&l.strc.offsets );
		return -1;
	}
	for(int i = 0; i < strc->names.size; i++){
		int64_t* id = getList(&strc->names, i);
		ASTType* ty = getList(&strc->types, i);
		int64_t  ti = unfoldType(ltab, ty);
		int64_t   x = -1;
		if(ti == -1) goto fail;
		appendList(&l.strc.fieldIds,  id);
		appendList(&l.strc.fieldTys, &ti);
		appendList(&l.strc.offsets , &x );
	}
	return appendList(&ltab->layouts, &l);
}

int unfoldUnion (LayoutTable* ltab, ASTUnion*  unon){
	int  size = unon->names.size;
	Layout  l = (Layout   ){.size=0, .align=0, .ast=unon, .kind=LK_UNON};
	l.unon    = (UnionData){.fieldTys =makeList(size+1, sizeof(int64_t)),
							.fieldIds =makeList(size+1, sizeof(int64_t)),
							.fieldVls =makeList(size+1, sizeof(int64_t)),
							.tagty    =unon->tagty,
							.tagid    =unon->tagid,
							.tagoffset=-1};
	if(0){
		fail:
		freeList(&l.unon.fieldTys);
		freeList(&l.unon.fieldIds);
		freeList(&l.unon.fieldVls);
		return -1;
	}
	for(int i = 0; i < unon->names.size; i++){
		int64_t* id = getList(&unon->names, i);
		int64_t* vl = getList(&unon->vals , i);
		ASTType* ty = getList(&unon->types, i);
		int64_t  ti = unfoldType(ltab, ty);
		if(ti == -1) goto fail;
		appendList(&l.unon.fieldIds,  id);
		appendList(&l.unon.fieldVls,  vl);
		appendList(&l.unon.fieldTys, &ti);
	}
	return appendList(&ltab->layouts, &l);
}

int unfoldEnum  (LayoutTable* ltab, ASTEnum*   enmt){
	return 1;
}

int unfoldAtom  (LayoutTable* ltab, ASTTyAtom* atom){
	return 1;
}

int unfoldElem  (LayoutTable* ltab, ASTTyElem* elem){
	return 1;
}

int unfoldType  (LayoutTable* ltab, ASTType*  type){
	switch(type->kind){
		case TK_ATOM : return unfoldAtom  (ltab, &type->atom); break;
		case TK_STRC : return unfoldStruct(ltab, &type->strc); break;
		case TK_UNON : return unfoldUnion (ltab, &type->unon); break;
		case TK_ENUM : return unfoldEnum  (ltab, &type->enmt); break;
		case TK_ELEM : return unfoldElem  (ltab, &type->elem); break;
	}
	return -1;
}





int  sizeType(LayoutTable* ltab, ErrorList* errs, int tyid){
	ASTTyDef* tdef = NULL;	//getList(&prog. tys    , tyid);	// FIXME!!!
	Layout*      l = getList(&ltab->layouts, tyid);
	if(l->size == 0){
		switch(l->kind){
			case LK_STRC: break;
			case LK_UNON: break;
			case LK_ENUM: break;
			case LK_BITY: break;
			case LK_BILD: break;
			case LK_FNTY: break;
			case LK_TDEF: {
				ASTTyDef* type = l->ast;
				if(l->tdef == -1){
					l->tdef = unfoldType(ltab, &type->tdef);
				}
				
				if(l->tdef == -1){
					// Unfold error
					return 0;
				}
				
				switch(type->tdef.kind){
					case TK_ATOM : return buildAtomLayout  (ltab, tyid, &type->tdef);
					case TK_STRC : return buildStructLayout(ltab, tyid, &type->tdef);
					case TK_UNON : return buildUnionLayout (ltab, tyid, &type->tdef);
					case TK_ENUM : return buildEnumLayout  (ltab, tyid, &type->tdef);
					case TK_ELEM : return buildElemLayout  (ltab, tyid, &type->tdef);
				}
			}break;
		}
	}
	return 0;
}



void prepareTypeLayouts(LayoutTable* ltab, ASTProgram prog){
	for(int i = 0; i < prog.tys.size; i++){
		ASTTyDef* tdef = getList(&prog.tys, i);
		Layout l       = (Layout){.pos=tdef->pos, .tdef=-1, .hash=0, .size=0, .align=0, .ast=tdef, .kind=LK_TDEF};
		appendList(&ltab->layouts, &l);
		
		// TODO: Add TypeLayout object
	}

}


int makeTypeLayouts(LayoutTable* ltab, ErrorList* errs){
	if(0){
		error:
		return 0;
	}
	
	int step = 0;
	int cont = 3;
	int size = ltab->layouts.size;
	while(cont){
		int newStep = 0;
		
		for(int i = 0; i < ltab->layouts.size; i++){
			newStep += sizeType(ltab, errs, i);
			
		}
		
		// TODO: check if progress has been made. If not, fail
		cont--;
	}
	return 1;
}


