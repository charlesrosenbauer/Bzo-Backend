#include "stdio.h"

#include "type.h"
#include "func.h"
#include "util.h"


int main(){
	Struct str2  = makeStruct(2);
	((TypeUnion*)str2 .pars)[0].prim = P_I32;
	str2 .kinds[0]                   = TK_PRIMITIVE;
	((TypeUnion*)str2 .pars)[1].prim = P_I32;
	str2 .kinds[1]                   = TK_PRIMITIVE;
	
	Struct str3  = makeStruct(3);
	((TypeUnion*)str3 .pars)[0].prim = P_F32;
	str3 .kinds[0]                   = TK_PRIMITIVE;
	((TypeUnion*)str3 .pars)[1].prim = P_F32;
	str3 .kinds[1]                   = TK_PRIMITIVE;
	((TypeUnion*)str3 .pars)[2].prim = P_F32;
	str3 .kinds[2]                   = TK_PRIMITIVE;
	
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
	Type   vec3  = (Type){str3 , TK_STRUCT, 0, 0};
	Type   vec4  = (Type){str4 , TK_STRUCT, 0, 0};
	Type   mat4  = (Type){str16, TK_STRUCT, 0, 0};
	
	TypeTable tab = makeTypeTable(4);
	tab.types[0]  = vec2;
	tab.types[1]  = vec3;
	tab.types[2]  = vec4;
	tab.types[3]  = mat4;
	
	if(!calcTypeSize(&tab, &vec2)) printf("Size fail\n");
	printType(vec2);
}
