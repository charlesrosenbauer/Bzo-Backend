#include "stdlib.h"
#include "stdio.h"

#include "type.h"
#include "util.h"



Array  makeArray(){
	Array ret;
	ret.val    = malloc(sizeof(TypeUnion));
	ret.size   = 0;
	ret.align  = 0;
	ret.stride = 0;
	return ret;
}


Struct makeStruct(int size){
	Struct ret;
	uint8_t* buf = malloc((sizeof(TypeUnion) + sizeof(TypeKind) + sizeof(int)) * size);
	ret.pars     = buf;
	ret.kinds    = (TypeKind*)(buf + (sizeof(TypeUnion) * size));
	ret.offsets  = (int*)     (buf + (sizeof(TypeUnion) + sizeof(TypeKind)) * size);
	ret.parct    = size;
	ret.size     = 0;
	ret.align    = 0;
	return ret;
}


Union makeUnion(int size){
	Union ret;
	uint8_t* buf = malloc((sizeof(TypeUnion) + sizeof(TypeKind)) * size);
	ret.pars     = buf;
	ret.kinds    = (TypeKind*)(buf + (sizeof(TypeUnion) * size));
	ret.parct    = size;
	ret.size     = 0;
	ret.align    = 0;
	return ret;
}


void leftpad(int pad){
	for(int i = 0; i < pad; i++) printf("  ");
}

void printPrimitive(Primitive p, int pad){
	leftpad(pad);
	switch(p){
		case P_I8  : printf("I8\n" ); break;
		case P_I16 : printf("I16\n"); break;
		case P_I32 : printf("I32\n"); break;
		case P_I64 : printf("I64\n"); break;
		case P_U8  : printf("I8\n" ); break;
		case P_U16 : printf("I16\n"); break;
		case P_U32 : printf("I32\n"); break;
		case P_U64 : printf("I64\n"); break;
		case P_F16 : printf("F16\n"); break;
		case P_F32 : printf("F32\n"); break;
		case P_F64 : printf("F64\n"); break;
		case P_Ptr : printf("Ptr\n"); break;
	}
}

void printStruct(Struct s, int pad){
	leftpad(pad);
	printf("struct{\n");
	TypeUnion* pars = s.pars;
	for(int i = 0; i < s.parct; i++){
		leftpad(pad+1);
		printf("@%iB:\n", s.offsets[i]);
		if(s.kinds[i] == TK_STRUCT){
			printStruct(pars[i].strc, pad+1);
		}else if(s.kinds[i] == TK_PRIMITIVE){
			printPrimitive(pars[i].prim, pad+1);
		}else if(s.kinds[i] == TK_UNION){
			printUnion(pars[i].unon, pad+1);
		}else if(s.kinds[i] == TK_ARRAY){
			printArray(pars[i].arry, pad+1);
		}else if(s.kinds[i] == TK_NAMED){
			leftpad(pad+1);
			printf("TYID=%lu\n", pars[i].name.tyid);
		}else if(s.kinds[i] == TK_VOID){
			printf("<VOID>\n");
		}
		printf("\n");
	}
	leftpad(pad);
	printf("}\n");
}

void printUnion(Union s, int pad){
	leftpad(pad);
	printf("union{\n");
	TypeUnion* pars = s.pars;
	for(int i = 0; i < s.parct; i++){
		leftpad(pad+1);
		if(s.kinds[i] == TK_STRUCT){
			printStruct(pars[i].strc, pad+1);
		}else if(s.kinds[i] == TK_PRIMITIVE){
			printPrimitive(pars[i].prim, pad+1);
		}else if(s.kinds[i] == TK_UNION){
			printUnion (pars[i].unon, pad+1);
		}else if(s.kinds[i] == TK_ARRAY){
			printArray (pars[i].arry, pad+1);
		}else if(s.kinds[i] == TK_NAMED){
			leftpad(pad+1);
			printf("TYID=%lu\n", pars[i].name.tyid);
		}else if(s.kinds[i] == TK_VOID){
			printf("<VOID>\n");
		}
	}
	leftpad(pad);
	printf("}\n");
}

void printArray(Array ar, int pad){
	leftpad(pad);
	printf("array[%i] stride=%i{\n", ar.count, ar.stride);
	TypeUnion* val = ar.val;
	if(ar.kind == TK_STRUCT){
		printStruct   (val->strc, pad+1);
	}else if(ar.kind == TK_PRIMITIVE){
		printPrimitive(val->prim, pad+1);
	}else if(ar.kind == TK_UNION){
		printUnion    (val->unon, pad+1);
	}else if(ar.kind == TK_NAMED){
		printf("  TYID=%lu\n", val->name.tyid);
	}else if(ar.kind == TK_ARRAY){
		printArray    (val->arry, pad+1);
	}else if(ar.kind == TK_VOID){
		printf("<VOID>\n");
	}
	leftpad(pad);
	printf("}\n");
}

void printType(Type ty){
	printf("%i bytes, %i alignment :: {\n", ty.size, ty.align);
	if(ty.kind == TK_STRUCT){
		printStruct   (ty.type.strc, 1);
	}else if(ty.kind == TK_PRIMITIVE){
		printPrimitive(ty.type.prim, 1);
	}else if(ty.kind == TK_UNION){
		printUnion    (ty.type.unon, 1);
	}else if(ty.kind == TK_NAMED){
		printf("  TYID=%lu\n", ty.type.name.tyid);
	}else if(ty.kind == TK_ARRAY){
		printArray    (ty.type.arry, 1);
	}else if(ty.kind == TK_VOID){
		printf("<VOID>\n");
	}
	printf("}\n");
}

int calcPrimitiveSize(Primitive pm, int* size, int* align){
	switch(pm){
		case P_I8  : {*size = 1; *align = 1; } break;
		case P_I16 : {*size = 2; *align = 2; } break;
		case P_I32 : {*size = 4; *align = 4; } break;
		case P_I64 : {*size = 8; *align = 8; } break;
		case P_U8  : {*size = 1; *align = 1; } break;
		case P_U16 : {*size = 2; *align = 2; } break;
		case P_U32 : {*size = 4; *align = 4; } break;
		case P_U64 : {*size = 8; *align = 8; } break;
		case P_F16 : {*size = 2; *align = 2; } break;
		case P_F32 : {*size = 4; *align = 4; } break;
		case P_F64 : {*size = 8; *align = 8; } break;
		case P_Ptr : {*size = 8; *align = 8; } break;
		default : return 0;
	}
	
	return 1;
}

int calcStructSize(TypeTable* tab, Struct* st, int* retsize, int* retalign){
	int size = 0, align = 1;
	TypeUnion* pars = st->pars;
	for(int i = 0; i < st->parct; i++){
		int s, a;
		if(st->kinds[i] == TK_STRUCT){
			if(!calcStructSize   (tab, &pars[i].strc, &s, &a)) return 0;
		}else if(st->kinds[i] == TK_PRIMITIVE){
			if(!calcPrimitiveSize(      pars[i].prim, &s, &a)) return 0;
		}else if(st->kinds[i] == TK_NAMED){
			if(!calcNamedTypeSize(tab,  pars[i].name, &s, &a)) return 0;
		}else if(st->kinds[i] == TK_UNION){
			if(!calcUnionSize    (tab, &pars[i].unon, &s, &a)) return 0;
		}else if(st->kinds[i] == TK_ARRAY){
			if(!calcArraySize    (tab, &pars[i].arry, &s, &a)) return 0;
		}else{
			return 0;
		}
		
		// Adjust offset for aligment
		if((size % a))	size += (a - (size % a));
		
		st->offsets[i] = size;
		align = lcm(align, a);
		size += s;
	}
	st->size  = size;
	st->align = align;
	*retsize  = size;
	*retalign = align;
	
	return 1;
}

int calcUnionSize(TypeTable* tab, Union* un, int* retsize, int* retalign){
	int size = 0, align = 1;
	TypeUnion* pars = un->pars;
	for(int i = 0; i < un->parct; i++){
		int s, a;
		if(un->kinds[i] == TK_STRUCT){
			if(!calcStructSize   (tab, &pars[i].strc, &s, &a)) return 0;
		}else if(un->kinds[i] == TK_PRIMITIVE){
			if(!calcPrimitiveSize(      pars[i].prim, &s, &a)) return 0;
		}else if(un->kinds[i] == TK_NAMED){
			if(!calcNamedTypeSize(tab,  pars[i].name, &s, &a)) return 0;
		}else if(un->kinds[i] == TK_UNION){
			if(!calcUnionSize    (tab, &pars[i].unon, &s, &a)) return 0;
		}else if(un->kinds[i] == TK_ARRAY){
			if(!calcArraySize    (tab, &pars[i].arry, &s, &a)) return 0;
		}else{
			return 0;
		}

		align = lcm(align, a);
		size  = (size > s)? size : s;
	}
	un->size  = size;
	un->align = align;
	*retsize  = size;
	*retalign = align;
	
	return 1;
}

int calcNamedTypeSize(TypeTable* tab, NamedType ty, int* retsize, int* retalign){
	if((ty.tyid >= 0) && (ty.tyid < tab->tyct)){
		if(tab->types[ty.tyid].align == 0) return 0;
		
		*retsize  = tab->types[ty.tyid].size;
		*retalign = tab->types[ty.tyid].align;
		return 1;
	}
	
	return 0;
}

int calcArraySize(TypeTable* tab, Array* ar, int* retsize, int* retalign){
	int size = 0, align = 1;
	TypeUnion* pars = ar->val;
	if(ar->kind == TK_STRUCT){
		if(!calcStructSize   (tab, &pars->strc, &size, &align)) return 0;
	}else if(ar->kind == TK_PRIMITIVE){
		if(!calcPrimitiveSize(      pars->prim, &size, &align)) return 0;
	}else if(ar->kind == TK_NAMED){
		if(!calcNamedTypeSize(tab,  pars->name, &size, &align)) return 0;
	}else if(ar->kind == TK_ARRAY){
		if(!calcArraySize    (tab, &pars->arry, &size, &align)) return 0;
	}else if(ar->kind == TK_UNION){
		if(!calcUnionSize    (tab, &pars->unon, &size, &align)) return 0;
	}else{
		return 0;
	}
	
	int stride = (size % align)? (size + align - (size % align)) : size;
	
	ar->size   = stride * ar->count;
	ar->align  = align;
	ar->stride = stride;
	*retsize   = ar->size;
	*retalign  = ar->align;
	
	return 1;
}



int calcTypeSize(TypeTable* tab, Type* ty){
	if(ty->kind == TK_STRUCT){
		return calcStructSize   (tab, &ty->type.strc, &ty->size, &ty->align);
	}else if(ty->kind == TK_PRIMITIVE){
		return calcPrimitiveSize(      ty->type.prim, &ty->size, &ty->align);
	}else if(ty->kind == TK_UNION){
		return calcUnionSize    (tab, &ty->type.unon, &ty->size, &ty->align);
	}else if(ty->kind == TK_NAMED){
		return calcNamedTypeSize(tab,  ty->type.name, &ty->size, &ty->align);
	}else if(ty->kind == TK_ARRAY){
		return calcArraySize    (tab, &ty->type.arry, &ty->size, &ty->align);
	}
	
	return 0;
}




TypeTable makeTypeTable(int tyct){
	TypeTable ret;
	ret.types = malloc(sizeof(Type) * tyct);
	ret.tyct  = tyct;
	return ret;
}



int sizeTypeTable(TypeTable* tab){
	
	int fillsize = (tab->tyct / 64) + ((tab->tyct % 64) != 0);
	uint64_t* fillbits = malloc(sizeof(uint64_t) * fillsize);
	for(int i = 0; i < fillsize; i++) fillbits[i] = 0;

	
	int last = 0;
	int fill = 0;
	do{
		last = fill;
		for(int i = 0; i < tab->tyct; i++){
			if(!(fillbits[i/64] & (1l << (i%64)))){
				if(calcTypeSize(tab, &tab->types[i])){
					fill++;
					fillbits[i/64] |= (1l << (i%64));
				}
			}
		}
	}while((last != fill) && (fill != tab->tyct));
	
	return fill == tab->tyct;
}


// Assert that a is a subtype of b
int subtype(TypeTable* tab, TypeUnion a, TypeUnion b, TypeKind ak, TypeKind bk){
	if((ak == TK_STRUCT) && (bk == TK_STRUCT) && (a.strc.parct == b.strc.parct)){
		TypeUnion* as = a.strc.pars;
		TypeUnion* bs = b.strc.pars;
		for(int i = 0; i < a.strc.parct; i++)
			if(!subtype(tab, as[i], bs[i], a.strc.kinds[i], b.strc.kinds[i])) return 0;
			return 1;
	}else if((ak == TK_UNION) && (bk == TK_UNION) && (a.unon.parct <= b.unon.parct)){
		TypeUnion* as = b.unon.pars;
		for(int i = 0; i < a.unon.parct; i++)
			if(!subtype(tab, as[i], b, a.unon.kinds[i], bk)) return 0;
		return 1;
	}else if((ak == TK_PRIMITIVE) && (bk == TK_PRIMITIVE) && (a.prim == b.prim)){
		return 1;
	}else if((ak == TK_NAMED) && (bk == TK_NAMED) && (a.name.tyid == b.name.tyid)){
		return 1;
	}else if((ak == TK_ARRAY) && (bk == TK_ARRAY) && (a.arry.count == b.arry.count)){
		TypeUnion* as = a.arry.val;
		TypeUnion* bs = b.arry.val;
		return subtype(tab, *as, *bs, a.arry.kind, b.arry.kind);
	}else if((ak != TK_UNION) && (bk == TK_UNION)){
		TypeUnion* bs = b.unon.pars;
		for(int i = 0; i < b.unon.parct; i++)
			if(subtype(tab, a, bs[i], ak, b.unon.kinds[i])) return 1;
		return 0;
	}
	
	return 0;
}






