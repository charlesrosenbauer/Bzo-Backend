#include "x86.h"

#include "stdlib.h"
#include "stdio.h"


typedef enum{
	IK_ERROR,		// error
	IK_TINYOP,		// prefix | opcode
	IK_TIM1OP,		// prefix | opcode | imm8
	IK_TIM4OP,      // prefix | opcode | imm32
	IK_NORMOP,		// prefix | opcode | r/m | sib
	IK_RMEXOP 		// prefix | opcode | r/m + opc
}X86_InsKind;

X86_InsKind x86Kind(X86Opcode opc, uint32_t* opcode){
	switch(opc){
	
		/*
			OPCODE FORMAT
			
			XX AA BB CC
			
			XX - r/m opcode (for instructions that need it)
			AA - first  opcode byte (optional)
			BB - second opcode byte (optional)
			CC - third  opcode byte
		*/
	
		// Arithmetic
		case X86_ADD    : {*opcode = 0x00000000; return IK_NORMOP;}
		case X86_SUB    : {*opcode = 0x00000028; return IK_NORMOP;}
		case X86_MUL    : {*opcode = 0x00000000; return IK_RMEXOP;}
		case X86_DIV    : {*opcode = 0x00000000; return IK_RMEXOP;}
		case X86_IMUL   : {*opcode = 0x00000000; return IK_RMEXOP;}
		case X86_IDIV   : {*opcode = 0x00000000; return IK_RMEXOP;}
		case X86_INC    : {*opcode = 0x000000ff; return IK_RMEXOP;}
		case X86_DEC    : {*opcode = 0x010000ff; return IK_RMEXOP;}
		case X86_NEG    : {*opcode = 0x030000f7; return IK_RMEXOP;}
		
		// Bitwise
		case X86_AND    : {*opcode = 0x00000020; return IK_NORMOP;}
		case X86_OR     : {*opcode = 0x00000008; return IK_NORMOP;}
		case X86_XOR    : {*opcode = 0x00000030; return IK_NORMOP;}
		case X86_NOT    : {*opcode = 0x020000f7; return IK_RMEXOP;}
		
		
		// Derived Arithmetic and Bitwise
		case X86_ADDIMM : {*opcode = 0x00000005; return IK_TIM4OP;}
		case X86_XORIMM : {*opcode = 0x00000035; return IK_TIM4OP;}
		case X86_ORRIMM : {*opcode = 0x0000000d; return IK_TIM4OP;}
		case X86_ANDIMM : {*opcode = 0x00000025; return IK_TIM4OP;}
		case X86_SHLIMM : {*opcode = 0x0000c1e0; return IK_TIM1OP;}
		case X86_SHRIMM : {*opcode = 0x0000c1e8; return IK_TIM1OP;}
		
		// Other
		case X86_NOP    : {*opcode = 0x00000090; return IK_TINYOP;}
		
		default: return IK_ERROR;
	}
}

uint8_t regRegByte(X86Register a, X86Register b){
	if((a != -1) && (b != -1)){
		return 0xC | (a << 3) | b;
	}
	return 0;
}


int writeX86Op(X86Op op, uint8_t* buffer){
	uint32_t opcode = 0;
	X86_InsKind ik = x86Kind(op.opc, &opcode);
	if(ik == IK_ERROR) return 0;
	
	// Prefices
	int ix = 0;
	if(op.lock){ buffer[ix] = 0xf0; ix++; }
	if(op.bitsize == SC_64){
		buffer[ix] = 0x48; ix++;
	}else if(op.bitsize == SC_16){
		buffer[ix] = 0x66; ix++;
	}
	
	// Opcode
	if(opcode > 0x00ffff){ buffer[ix] = (opcode >> 16) & 0xff; ix++; }
	if(opcode > 0x0000ff){ buffer[ix] = (opcode >>  8) & 0xff; ix++; }
	buffer[ix] = opcode & 0xff; ix++;
	
	// Tiny ops are done at this point
	if(ik == IK_TINYOP) return ix;
	
	
	// Immediates
	if(ik == IK_TIM1OP){
		// write immediate8
		buffer[ix] = op.immediate & 0xff;
		return ix + 1;
	}else if(ik == IK_TIM4OP){
		// write immediate32
		buffer[ix  ] = (op.immediate      ) & 0xff;
		buffer[ix+1] = (op.immediate >>  8) & 0xff;
		buffer[ix+2] = (op.immediate >> 16) & 0xff;
		buffer[ix+3] = (op.immediate >> 24) & 0xff;
		return ix + 4;
	}
	
	// Addressing. Here's where things get complex.
	
	
	return ix;
}


int writeX86Function(X86Function fn, MachineBlock* mb){
	// Keep track of the offsets of each block so that jumps can be safely written
	int* blockIxs = malloc(sizeof(int) * fn.bct);
	for(int i = 0; i < fn.bct; i++) blockIxs[i] = -1;
	
	// TODO: Add a hashtable or something to keep track of jumps that can't be
	// resolved in a single pass. This will be mostly loops.
	
	
	for(int i = 0; i < fn.bct; i++){
		for(int j = 0; j < fn.blocks[i].opct; j++){
			if(mb->bytect + 20 > mb->bytecap){
				uint8_t* tmp = mb->bytes;
				mb->bytes    = malloc(mb->bytecap * 2);
				for(int k = 0; k < mb->bytect; k++) mb->bytes[k] = tmp[k];
				free(tmp);
				mb->bytecap *= 2;
			}
		
			mb->bytect += writeX86Op(fn.blocks[i].ops[j], mb->bytes);
		}
	}
	
	free(blockIxs);
	return 0;
}



X86Block makeBlock(int cap){
	X86Block ret;
	ret.ops       =  malloc(sizeof(X86Op) * cap);
	ret.opcap     =  cap;
	ret.opct      =  0;
	ret.nextBlock = -1;
	return ret;
}

X86Function makeFunction(int blockct, int parct, int retct, int varct, int opcap){
	X86Function ret;
	ret.blocks = malloc(sizeof(X86Block) * blockct);
	for(int i = 0; i < blockct; i++) ret.blocks[i] = makeBlock(opcap);
	
	ret.bct    = blockct;
	ret.parct  = parct;
	ret.retct  = retct;
	ret.varct  = varct;
	ret.pars   = malloc(sizeof(X86Value) * parct);
	for(int i = 0; i < parct; i++) ret.pars[i] = (X86Value){NOREG, -1};
	ret.rets   = malloc(sizeof(X86Value) * retct);
	for(int i = 0; i < retct; i++) ret.rets[i] = (X86Value){NOREG, -1};
	ret.vars   = malloc(sizeof(X86Value) * varct);
	for(int i = 0; i < varct; i++) ret.vars[i] = (X86Value){NOREG, -1};
	
	return ret;
}

int appendOp(X86Block* blk, X86Op op){
	if(blk->opct + 1 >= blk->opcap){
		X86Op* tmp = blk->ops;
		blk->ops   = malloc(sizeof(X86Op) * blk->opcap * 2);
		for(int i = 0; i < blk->opct; i++) blk->ops[i] = tmp[i];
		free(tmp);
		blk->opcap *= 2;
	}
	blk->ops[blk->opct] = op;
	blk->opct++;
	return blk->opct;
}

void printX86Block(X86Block* blk){
	printf("Block\n");
	for(int i = 0; i < blk->opct; i++){
		X86Op o = blk->ops[i];
		printf("  %i : %i %i %i > %i %i | %lu\n", o.opc, o.vv.a, o.vv.b, o.vv.c, o.vv.q, o.vv.r, o.immediate);
	}
	printf("GOTO : %i\n", blk->nextBlock);
}





