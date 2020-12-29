#include "stdio.h"

#include "type.h"
#include "func.h"
#include "util.h"

void typetest(){
	Struct str2  = makeStruct(2);
	((TypeUnion*)str2 .pars)[0].prim = P_I32;
	str2 .kinds[0]                   = TK_PRIMITIVE;
	((TypeUnion*)str2 .pars)[1].prim = P_I16;
	str2 .kinds[1]                   = TK_PRIMITIVE;
	
	Array  str3  = makeArray();
	((TypeUnion*)str3 .val)->prim = 0;
	str3 .kind                    = TK_NAMED;
	str3 .count                   = 3;
	
	Struct str4  = makeStruct(4);
	((TypeUnion*)str4 .pars)[0].prim = P_F64;
	str4 .kinds[0]                   = TK_PRIMITIVE;
	((TypeUnion*)str4 .pars)[1].prim = P_F64;
	str4 .kinds[1]                   = TK_PRIMITIVE;
	((TypeUnion*)str4 .pars)[2].prim = P_F64;
	str4 .kinds[2]                   = TK_PRIMITIVE;
	((TypeUnion*)str4 .pars)[3].prim = P_F64;
	str4 .kinds[3]                   = TK_PRIMITIVE;
	
	Struct str16 = makeStruct(4);
	((TypeUnion*)str16.pars)[0].name = (NamedType){2};
	str16.kinds[0]                   = TK_NAMED;
	((TypeUnion*)str16.pars)[1].name = (NamedType){2};
	str16.kinds[1]                   = TK_NAMED;
	((TypeUnion*)str16.pars)[2].name = (NamedType){2};
	str16.kinds[2]                   = TK_NAMED;
	((TypeUnion*)str16.pars)[3].name = (NamedType){2};
	str16.kinds[3]                   = TK_NAMED;
	
	Type   vec2  = (Type){str2 , TK_STRUCT, 0, 0};
	Type   vec3  = (Type){(TypeUnion)str3 , TK_ARRAY, 0, 0};
	Type   vec4  = (Type){str4 , TK_STRUCT, 0, 0};
	Type   mat4  = (Type){str16, TK_STRUCT, 0, 0};
	
	TypeTable tab = makeTypeTable(4);
	tab.types[3]  = vec2;
	tab.types[1]  = vec3;
	tab.types[2]  = vec4;
	tab.types[0]  = mat4;
	
	if(!sizeTypeTable(&tab)) printf("Size fail\n");
	
	for(int i = 0; i < 4; i++) printType(tab.types[i]);
}


void functest(){

}


int main(){
	typetest();
}
