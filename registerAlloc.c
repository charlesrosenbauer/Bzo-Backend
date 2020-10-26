#include "x86.h"
#include "struct.h"

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"



/*
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
	
	for(int i = 1; i <= maxvar; i++){
		printf("%i : %i -> %i\n", i, varInits[i], varFrees[i]);
	}
	
	X86Register* varRegs = malloc(sizeof(X86Register) * (maxvar+1));
	int regSpans[32];
	for(int i = 0; i <  32; i++) regSpans[i] = -1;
	for(int i = 1; i <= maxvar; i++){
		for(int j = 0; j < 32; j++){
			if(regSpans[j] < varFrees[i]){
				varRegs [i] = j;
				regSpans[j] = varFrees[i];
				break;
			}
		} 
	}
	for(int i = 1; i <= maxvar; i++){
		printf("V:%i [%i / %i]\n", i, varRegs[i], regSpans[varRegs[i]]);
	}
	
	uint16_t* regTable = malloc(sizeof(uint16_t) * (maxvar+1));
	
	
	
	free(regTable);
	free(as);
	free(bs);
	free(cs);
	free(qs);
	free(rs);
	free(varInits);
	free(varFrees);
	return ret;
}


/*
typedef struct{
	uint8_t*	bytes;
	uint64_t	offset;
	int			codesize, codecap;
	
	uint64_t*	addresses;
	int*		addroffsets;
	
	int			addrsize, addrcap;
}MachineBlock;


typedef struct{
	X86Opcode   opc;
	ValSize     bitsize;
	int         a, b, c, q, r;
	uint64_t    imm;
	uint8_t     lock;
}X86BCOp;

typedef struct{
	X86BCOp*    ops;
	int opct, opcap;
}X86BCBlock;



typedef struct{
	X86Register reg;
	int         offset;
}X86Value;

typedef struct{
	X86Value*      pars;
	X86Value*      rets;
	X86Value*      vars;
	
	int parct, retct, varct, stackSize, blockSize;

	X86BCBlock*    bcblocks;
	MachineBlock*  mcblocks;
	
	int bct, mct;
}X86Function;
*/



//int functionRegAlloc(X86Function* fn){

	/*
		I'm going to try to use a backtracking algorithm for this.
	*/
	
	/*
	// First, create some useful tables
	Set32* varRefs  = malloc(sizeof(Set32) * fn->bct);
	for(int i = 0; i < fn->bct; i++) varRefs[i] = initSet32(fn->varct);
	
	for(int i = 0; i < fn->bct; i++){
		X86BCBlock* blk = &fn->bcblocks[i];
		for(int j = 0; j < blk->opct; j++){
			X86BCOp op = blk->ops[j];
			if(op.a) insertSet32(&varRefs[i], op.a);
			if(op.b) insertSet32(&varRefs[i], op.b);
			if(op.c) insertSet32(&varRefs[i], op.c);
			if(op.q) insertSet32(&varRefs[i], op.q);
			if(op.r) insertSet32(&varRefs[i], op.r);
		}
		printf("Block %i:\n", i);
		printSet32(varRefs[i]);
	}
	
	// Then, precolor pars and rets
	
	return 1;
}


*/




