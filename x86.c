#include "x86.h"
#include "stdlib.h"


typedef enum{
	IK_ERROR,		// error
	IK_TINYOP,		// prefix | opcode
	IK_TIMMOP,		// prefix | opcode | imm
	IK_NORMOP,		// prefix | opcode | r/m | sib
	IK_RMEXOP 		// prefix | opcode | r/m + opc
}X86_InsKind;

X86_InsKind x86Kind(X86Opcode opc, uint32_t* opcode){
	switch(opc){
	
		// Arithmetic
		case X86_ADD  : {*opcode = 0x000000; return IK_NORMOP;}
		case X86_SUB  : {*opcode = 0x000028; return IK_NORMOP;}
		case X86_MUL  : {*opcode = 0x000000; return IK_RMEXOP;}
		case X86_DIV  : {*opcode = 0x000000; return IK_RMEXOP;}
		case X86_IMUL : {*opcode = 0x000000; return IK_RMEXOP;}
		case X86_IDIV : {*opcode = 0x000000; return IK_RMEXOP;}
		case X86_INC  : {*opcode = 0x00ff00; return IK_RMEXOP;}
		case X86_DEC  : {*opcode = 0x00ff01; return IK_RMEXOP;}
		case X86_NEG  : {*opcode = 0x00f703; return IK_RMEXOP;}
		
		// Bitwise
		case X86_AND  : {*opcode = 0x000020; return IK_NORMOP;}
		case X86_OR   : {*opcode = 0x000008; return IK_NORMOP;}
		case X86_XOR  : {*opcode = 0x000030; return IK_NORMOP;}
		case X86_NOT  : {*opcode = 0x00f702; return IK_RMEXOP;}
		
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
				mb->bytecap * 2;
			}
		
			mb->bytect += writeX86Op(fn.blocks[i].ops[j], mb->bytes);
		}
	}
	
	free(blockIxs);
	return 0;
}

/*
int writeX86Op(X86Op op, uint8_t* buffer){

	int a = op.a, b = op.b;
	int ix = 0;
	
	if(op.lock){
		buffer[ix] = 0xf0;
		ix++;
	}
	
	uint64_t opc = (uint64_t)op.opc;
	uint8_t  pfx = (opc >> 32);
	uint8_t  op0 = (opc >> 24);
	uint8_t  opct=((opc >> 40) + 1) % 16;
	uint8_t  op1 = (opc >> 16);
	uint8_t  op2 = (opc >>  8);
	
	if(opc >> 44){
		buffer[0] =  op0;
		buffer[1] = (opct > 1)? op1 : buffer[1];
		buffer[2] = (opct > 2)? op2 : buffer[2];
		return opct;
	}
	
	a = (opc & 0xf)? opc & 7 : a;
	
	if(op.mode == AM_RR){
		// Encode with register-register mode.
		if(op.bitsize != SC_64){
		
			uint8_t mod = 0xC0 | ((a & 0x7) << 3) | (b & 0x7);
			uint8_t opw = op.bitsize != SC_8;
			uint8_t opd = 0;
			if(op.bitsize == SC_16){
				buffer[ix] = 0x66;
				ix++;
			}
			uint8_t opc = op0 | opd | opw;
			buffer[ix    ] = opc;
			buffer[ix+opc] = mod;
			if(opct != 1){
				if(opct == 2){
					buffer[ix+2] = op1;
				}else{
					buffer[ix+2] = op1;
					buffer[ix+3] = op2;
				}
			}
			ix += 1 + opct;
		}else{
			// 64 bit
			uint8_t mod = 0xC0 | ((a & 0x7) << 3) | (b & 0x7);
			uint8_t opw = 1;
			uint8_t opd = 0;
			uint8_t pfx = 0x48 | ((a >> 1) & 8) | ((b >> 3) & 1);
			uint8_t opc =  op0 | 1;
			buffer[ix       ] = pfx;
			buffer[ix+1     ] = opc;
			buffer[ix+1+opct] = mod;
			if(opct != 1){
				if(opct == 2){
					buffer[ix+2] = op1;
				}else{
					buffer[ix+2] = op1;
					buffer[ix+3] = op2;
				}
			} 
			ix += 2 + opct;
		}
	}else{
		// Encode with register-memory mode.
		// This is going to get complex.
		*/
		/*
			The approach I'm going to try and take here is to accept the addressing info given,
			and just try to generate it in any way possible.
		*//*
		X86Addr addr = op.addr;
		if(op.bitsize != SC_16){
			// 32/64 bit addressing mode
			if(addr.scale == 1){
				// Try to avoid using SIB
				if((addr.b == NOREG) && (addr.a != NOREG)){
					
				}
			}else{
				// Gonna have to use SIB
				
			}
		}
		
	}
	
	
	return ix;
}


*/
/*
	Going to try to refactor this the best I can. X86 is a true monster.
	
	
*//*
int writeX86Full(X86Op op, uint8_t* buffer){
	
	int ix = 0;
	if(op.lock){
		buffer[ix] = 0xf0;
		ix++;
	}
	
	
	
	return ix;
}
*/

