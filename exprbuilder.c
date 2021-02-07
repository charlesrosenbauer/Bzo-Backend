#include "program.h"
#include "alloc.h"
#include "func.h"
#include "type.h"

#include "stdint.h"
#include "stdlib.h"


/*
	Current plan here is to convert statements into a graph where it will be
	easier to apply some simple reductions, then convert it into bytecode.
	
	This also means we can be a lot messier with data layout here, as this
	will be a short-lived data structure that will be converted into a longer-
	lived bytecode format with a more cache-friendly layout.
*/

typedef enum{
	NK_STRC,
	NK_POLY,
	NK_SELM,
	NK_GELM,
	NK_CALL,
	NK_BRNX
}NodeKind;

typedef struct{
	NodeKind kind;
	int      a, b, c;
}ExprNode;
