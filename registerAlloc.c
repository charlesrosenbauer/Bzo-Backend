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
	}
	
	int* varInits = malloc(sizeof(int) * (maxvar+1));
	int* varFrees = malloc(sizeof(int) * (maxvar+1));
	for(int i = 0; i <=  maxvar; i++){ varInits[i] = -1; varFrees[i] = -1; }
	for(int i = 0; i < blk.opct; i++){
		X86BCOp op  = blk.ops[i];
		int a = op.a, b = op.b, c = op.c, q = op.q, r = op.r;
		varInits[a] = (varInits[a] < 0)? i : varInits[a];
		varFrees[a] = i;
		
		varInits[b] = (varInits[b] < 0)? i : varInits[b];
		varFrees[b] = i;
		
		varInits[c] = (varInits[c] < 0)? i : varInits[c];
		varFrees[c] = i;
		
		varInits[q] = (varInits[q] < 0)? i : varInits[q];
		varFrees[q] = i;
		
		varInits[r] = (varInits[r] < 0)? i : varInits[r];
		varFrees[r] = i;
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

