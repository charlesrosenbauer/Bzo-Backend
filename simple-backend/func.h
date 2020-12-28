#ifndef __FUNC_HEADER__
#define __FUNC_HEADER__

#include "type.h"

#include "stdint.h"


typedef enum{
	XK_CMPD,
	XK_POLY,
	XK_LMDA,
	XK_PRFX,
	XK_PRIM
}ExprKind;

typedef struct{
	void*     pars;
	ExprKind* kinds;
	int       parct;
}CmpdExpr;

typedef struct{
	void*     pars;
	ExprKind* kinds;
	int       parct;
}PolyExpr;

typedef union{
	int64_t   i64;
	uint64_t  u64;
	float     f32;
	double    f64;
	char*     str;
}PrimExpr;


typedef struct{
	Type pars, rets;
	
	
}FuncDef;




#endif
