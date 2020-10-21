#include "bytecode.h"
#include "stdlib.h"




BCProgram   makeBCProgram    (int blkCap, int fncCap){
	BCProgram ret;
	ret.blocks   = malloc(sizeof(CodeBlock) * blkCap);
	ret.blockNum = 0;
	ret.blockCap = blkCap;

	ret.fnids    = malloc(sizeof(int) * fncCap);
	ret.fnNum    = 0;
	ret.fnCap    = fncCap;

	return ret;
}


void      resizeProgram  (BCProgram* p, int blkCap, int fniCap){

	int blCap = (blkCap > p->blockCap)? blkCap : p->blockCap;
	int fnCap = (fniCap > p->fnCap   )? fniCap : p->fnCap;

	if(blCap == blkCap){
		CodeBlock* blkTmp = p->blocks;
		p->blocks   = malloc(sizeof(CodeBlock) * blCap);
		for(int i   = 0; i < p->blockNum; i++) p->blocks[i] = blkTmp[i];
		free(blkTmp);
		p->blockCap = blCap;
	}
	
	if(fnCap == fniCap){
		int* fniTmp = p->fnids;
		p->fnids    = malloc(sizeof(int) * fnCap);
		for(int i   = 0; i < p->fnNum; i++) p->fnids[i] = fniTmp[i];
		free(fniTmp);
		p->fnCap = fnCap;
	}
}


void      addProgramBlock(BCProgram* p, CodeBlock blk){
	if(p->blockCap <= (p->blockNum + 1))
		resizeProgram(p, p->blockCap * 2, p->fnCap);
	
	p->blocks[p->blockNum] = blk;
	p->blockNum++;
}
