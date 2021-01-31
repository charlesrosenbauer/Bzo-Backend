#include "stdio.h"

#include "program.h"
#include "codegen.h"
#include "type.h"
#include "func.h"
#include "util.h"
#include "parser.h"


Program standardTestTable(int size){
	
	TypeTable tret = makeTypeTable(size + 4, size * 4);
	
	tret.types[0] = (Type){(TypeUnion)(Primitive)P_I8 , TK_PRIMITIVE, 1, 1, 1};
	tret.types[1] = (Type){(TypeUnion)(Primitive)P_I16, TK_PRIMITIVE, 2, 2, 1};
	tret.types[2] = (Type){(TypeUnion)(Primitive)P_I32, TK_PRIMITIVE, 4, 4, 1};
	tret.types[3] = (Type){(TypeUnion)(Primitive)P_I64, TK_PRIMITIVE, 8, 8, 1};

	// TODO: I need better code for building types. This gets really tedious, really fast

	FuncTable fret = makeFuncTable(size + 28, size * 4);
	
	// ADD, SUB
	for(int i = 0; i < 2; i++){
		for(int j = 0; j < 4; i++){
			int ix = (i * 4) + j;
			Type      ti;
			ti.type = (TypeUnion)makeStruct(2);   // [T, T]
			ti.kind = TK_STRUCT;
			Type      to;		//  T
			to.kind           = TK_NAMED;
			to.type.name.tyid = j;
			
			
			
			ExprUnion ex;
			ex.prim.opc    = (!i)? OP_ADD : OP_SUB;
			fret.funcs[ix] = (FuncDef){ti, to, ex, XK_PRIMOPC, NULL, 0, 0, NULL, 0, 0, 1};
		}
	}
	
	// MUL, DIV
	for(int i = 2; i < 4; i++){
		for(int j = 0; j < 4; j++){
			int ix = (i * 4) + j;
			Type      t;
			t .type = (TypeUnion)makeStruct(2);   // [T, T]
			t .kind = TK_STRUCT;
			
			
			
			ExprUnion ex;
			ex.prim.opc    = (i == 2)? OP_MUL : OP_DIV;
			fret.funcs[ix] = (FuncDef){t , t , ex, XK_PRIMOPC, NULL, 0, 0, NULL, 0, 0, 1};
		}
	}
	
	// NEG, ABS, NOT
	for(int i = 4; i < 7; i++){
		for(int j = 0; j < 4; j++){
			int ix = (i * 4) + j;
			Type t;				//  T
			t.kind           = TK_NAMED;
			t.type.name.tyid = j;
			ExprUnion ex;
			ex.prim.opc    = (i == 4)? OP_NEG : (i == 5)? OP_ABS : OP_NOT;
			fret.funcs[ix] = (FuncDef){t , t , ex, XK_PRIMOPC, NULL, 0, 0, NULL, 0, 0, 1};
		}
	}
	
	
	Program ret = (Program){tret, fret};
	return ret;
}


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
	
	TypeTable tab = makeTypeTable(4, 128);
	tab.types[3]  = vec2;
	tab.types[1]  = vec3;
	tab.types[2]  = vec4;
	tab.types[0]  = mat4;
	
	if(!sizeTypeTable(&tab)) printf("Size fail\n");
	
	for(int i = 0; i < 4; i++) printType(tab.types[i]);
}


void functest(){
	ExprUnion expr  = makeExpr(4);
	PrimExpr  var0; var0.u64 = 0;
	PrimExpr  var1; var1.u64 = 1;
	PrimExpr  var2; var2.u64 = 2;
	PrimExpr  fun0; fun0.u64 = 0;
	PrimExpr  fun1; fun1.u64 = 1;
	
	ExprUnion cmpd0 = makeCmpd(2);
	setIx(&cmpd0, (ExprUnion)var0, XK_PRIMVAR, 0);
	setIx(&cmpd0, (ExprUnion)var1, XK_PRIMVAR, 1);
	
	ExprUnion cmpd1 = makeCmpd(2);
	setIx(&cmpd1, (ExprUnion)fun0, XK_PRIMFUN, 0);
	setIx(&cmpd1, (ExprUnion)fun1, XK_PRIMFUN, 1);
	
	setIx(&expr,                      cmpd0 , XK_CMPD   , 0);
	setIx(&expr,                      cmpd1 , XK_CMPD   , 1);
	setIx(&expr, (ExprUnion)(PrimExpr)OP_ADD, XK_PRIMOPC, 2);
	setIx(&expr, (ExprUnion)          var2  , XK_PRIMVAR, 3);
	
	printExpr(expr, XK_EXPR);
	
	Type voidtype   = (Type){
		.kind  = TK_VOID,
		.size  = 0,
		.align = 0,
	};
	
	FuncTable fntab = makeFuncTable(64, 128);
	fntab.funcs[0]  = makeFuncDef  (voidtype, voidtype, 2);
	fntab.funcs[0].defn    = expr;
	fntab.funcs[0].defkind = XK_EXPR;
	
	fntab.funcs[1]  = makeFuncDef  (voidtype, voidtype, 1);
	
	fntab.funcs[2]  = makeFuncDef  (voidtype, voidtype, 1);
	
	buildFunc(&fntab.funcs[0], &fntab);
	//buildFunc(&fntab.funcs[1], &fntab);
	//buildFunc(&fntab.funcs[2], &fntab);
	
	//printFunc(fntab.funcs[0]);
}


void x86test(){
	X86Func fnc = makeX86Func (3, 16);
	appendX86Var  (&fnc);
	appendX86Block(&fnc);
	appendX86Op(&fnc.blocks[0]);
	appendX86Op(&fnc.blocks[0]);
	appendX86Op(&fnc.blocks[0]);
	appendX86Op(&fnc.blocks[0]);
	
	fnc.blocks[0].ops[0] = (X86Op){XO_IMUL, SC_64, NOREG, NOREG, 0,    0,  1,  3, -1, CC_NOCODE, XF_NOFLAGS};
	fnc.blocks[0].ops[1] = (X86Op){XO_IMUL, SC_64, NOREG, NOREG, 0,    1,  2,  4, -1, CC_NOCODE, XF_NOFLAGS};
	fnc.blocks[0].ops[2] = (X86Op){XO_ADD , SC_64, NOREG, NOREG, 0,    3,  4,  5, -1, CC_NOCODE, XF_NOFLAGS};
	fnc.blocks[0].ops[3] = (X86Op){XO_RET , SC_64, NOREG, NOREG, 0,   -1, -1, -1, -1, CC_NOCODE, XF_NOFLAGS};
	
	for(int i =  0; i < 12; i++){ fnc.blocks[0].invars[i] = -1; fnc.blocks[0].exvars[i] = -1; }
	for(int i = 12; i < 16; i++){ fnc.blocks[0].invars[i] = -2; fnc.blocks[0].exvars[i] = -2; }
	for(int i = 16; i < 32; i++){ fnc.blocks[0].invars[i] = -3; fnc.blocks[0].exvars[i] = -3; }
	fnc.blocks[0].invars[RSP] = -3;
	fnc.blocks[0].invars[RBP] = -3;
	fnc.blocks[0].exvars[RSP] = -3;
	fnc.blocks[0].exvars[RBP] = -3;
	
	fnc.blocks[0].invars[RSI] = 0;
	fnc.blocks[0].invars[RDI] = 1;
	fnc.blocks[0].invars[R8 ] = 2;
	fnc.blocks[0].exvars[RSI] = 5;
	
	printX86Func(&fnc);
	
	x86AllocRegs(&fnc.blocks[0]);
	
	printf("==========\n");
	printX86Func(&fnc);
	
	uint8_t program[256];
	int size = compileBlock(&fnc.blocks[0], program);
	
	printf("Program is %i bytes.\n", size);
	for(int i = 0; i < size; i++){
		printf("%02x ", program[i]);
	}
	printf("\n");
}


void parsetest(){
	uint8_t* file;
	int      fsize = 0;
	loadFile    ("tests/exec", &file, &fsize);
	
	Program p = parseProgram(file, fsize);
	if(!sizeTypeTable(&p.types)) printf("Size fail\n");
	for(int i = 0; i < p.types.tyct; i++) printType(p.types.types[i]);
}


int main(){
	parsetest();
}
