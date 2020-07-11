#include "defs.h"

int main(){

	char** optab = makeOpNameTab();

	Block blk = newBlock(FUNCHEAD, 64);
	addBlkOp(&blk, (OP){1, -1, 0, -1, C_LSE, 0xff, OP_CMOV});
	addBlkOp(&blk, (OP){2,  5, 1,  1,     0,    0, OP_IMUL});
	addBlkOp(&blk, (OP){3, -1, 2,  1,     0,    0, OP_XOR });
	addBlkOp(&blk, (OP){4, -1, 3,  0,     0,    0, OP_PUSH});


	printBlock(optab, blk);
}