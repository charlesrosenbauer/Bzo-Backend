#include "x86.h"




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
		
		/*
			The approach I'm going to try and take here is to accept the addressing info given,
			and just try to generate it in any way possible.
		*/
		
	}
	
	
	return ix;
}


