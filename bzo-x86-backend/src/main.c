#include "defs.h"
#include "stdio.h"
#include "stdlib.h"
#include "structs.h"

int main(){

	/*
	Program p = newProgram(64);

	printOpProps(p.opProps);

	Block blk = newBlock(FUNCHEAD, 64);
	addBlkOp(&blk, (OP){ 1, -1,  0, -1, C_LSE|B64, 0xff, OP_CMOV});
	addBlkOp(&blk, (OP){ 2,  5,  1,  1,       B64,    0, OP_IMUL});
	addBlkOp(&blk, (OP){ 3, -1,  2,  1,       B64,    0, OP_XOR });
	addBlkOp(&blk, (OP){ 4, -1,  3,  0,       B64,    0, OP_PUSH});

	addPrgmBlk(&p, blk);

	blk = newBlock(INNERLOOP, 64);
	addBlkOp(&blk, (OP){ 1, -1,  0, -1, C_LSE|B64, 0xff, OP_CMOV});
	addBlkOp(&blk, (OP){ 2,  5,  1,  1,       B64,    0, OP_MUL });
	addBlkOp(&blk, (OP){ 3, -1,  2,  1,       B64,    0, OP_AND });
	addBlkOp(&blk, (OP){ 4, -1,  3,  0,       B64,    0, OP_POP });

	addPrgmBlk(&p, blk);

	printProgram(p);
	*/

	OpcodeLine* lines = malloc(sizeof(OpcodeLine) * 256);
	char** opnames    = makeOpNameTab();
	printf("%i\n", getOpcodeLines("ops.hex", opnames, lines));

	for(int i = 0; i < 4; i++){
		printOpLine(&lines[i]);
	}
}