#include "bytecode.h"
#include "stdlib.h"




Program   makeProgram    (int blkCap, int fncCap){
	Program ret;
	ret.blocks   = malloc(sizeof(CodeBlock) * blkCap);
	ret.blockNum = 0;
	ret.blockCap = blkCap;

	ret.fnids    = malloc(sizeof(int) * fncCap);
	ret.fnNum    = 0;
	ret.fnCap    = fncCap;

	return ret;
}


void      resizeProgram  (Program* p, int blkCap, int fnCap){

}


void      addProgramBlock(Program* p, CodeBlock blk){
	
}