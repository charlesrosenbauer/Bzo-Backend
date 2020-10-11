#include "x86.h"

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"




X86Block allocRegs(X86BCBlock blk){
	X86Block ret;
	
	uint16_t* as = malloc(sizeof(uint16_t) * blk.opct);
	uint16_t* bs = malloc(sizeof(uint16_t) * blk.opct);
	uint16_t* cs = malloc(sizeof(uint16_t) * blk.opct);
	uint16_t* qs = malloc(sizeof(uint16_t) * blk.opct);
	uint16_t* rs = malloc(sizeof(uint16_t) * blk.opct);
	for(int i = 0; i < blk.opct; i++){ as[i] = 0; bs[i] = 0; cs[i] = 0; qs[i] = 0; rs[i] = 0; }
	
	int maxvar = 0;
	for(int i = 0; i < blk.opct; i++){
		X86BCOp op = blk.ops[i];
		maxvar = (maxvar > op.a)? maxvar : op.a;
		maxvar = (maxvar > op.b)? maxvar : op.b;
		maxvar = (maxvar > op.c)? maxvar : op.c;
		maxvar = (maxvar > op.q)? maxvar : op.q;
		maxvar = (maxvar > op.r)? maxvar : op.r;
		printf("OP %i: max=%i\n", i, maxvar);
	}
	
	int* varInits = malloc(sizeof(int) * (maxvar+1));
	int* varFrees = malloc(sizeof(int) * (maxvar+1));
	for(int i = 0; i <=  maxvar; i++){ varInits[i] = 0; varFrees[i] = 0; }
	for(int i = 0; i < blk.opct; i++){
		
	}
	
	free(as);
	free(bs);
	free(cs);
	free(qs);
	free(rs);
	free(varInits);
	free(varFrees);
	return ret;
}

