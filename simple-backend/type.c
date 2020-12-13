#include "stdlib.h"
#include "stdio.h"

#include "type.h"
#include "util.h"

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
		}
		printf("\n");
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

int calcStructSize(Struct* st, int* retsize, int* retalign){
	int size = 0, align = 1;
	TypeUnion* pars = st->pars;
	for(int i = 0; i < st->parct; i++){
		int s, a;
		if(st->kinds[i] == TK_STRUCT){
			if(!calcStructSize   (&pars[i].strc, &s, &a)) return 0;
		}else if(st->kinds[i] == TK_PRIMITIVE){
			if(!calcPrimitiveSize( pars[i].prim, &s, &a)) return 0;
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

int calcTypeSize(Type* ty){
	if(ty->kind == TK_STRUCT){
		return calcStructSize   (&ty->type.strc, &ty->size, &ty->align);
	}else if(ty->kind == TK_PRIMITIVE){
		return calcPrimitiveSize( ty->type.prim, &ty->size, &ty->align);
	}
	
	return 0;
}

