#include "codegen.h"
#include "program.h"

#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"


void printX86Reg(X86Register r){
	switch(r){
		case RAX  :  printf("RAX   "); break;
		case RCX  :  printf("RCX   "); break;
		case RDX  :  printf("RDX   "); break;
		case RBX  :  printf("RBX   "); break;
		case RSP  :  printf("RSP   "); break;
		case RBP  :  printf("RBP   "); break;
		case RSI  :  printf("RSI   "); break;
		case RDI  :  printf("RDI   "); break;
		case R8   :  printf("R8    "); break;
		case R9   :  printf("R9    "); break;
		case R10  :  printf("R10   "); break;
		case R11  :  printf("R11   "); break;
		case R12  :  printf("R12   "); break;
		case R13  :  printf("R13   "); break;
		case R14  :  printf("R14   "); break;
		case R15  :  printf("R15   "); break;
		case XMM0 :  printf("XMM0  "); break;
		case XMM1 :  printf("XMM1  "); break;
		case XMM2 :  printf("XMM2  "); break;
		case XMM3 :  printf("XMM3  "); break;
		case XMM4 :  printf("XMM4  "); break;
		case XMM5 :  printf("XMM5  "); break;
		case XMM6 :  printf("XMM6  "); break;
		case XMM7 :  printf("XMM7  "); break;
		case XMM8 :  printf("XMM8  "); break;
		case XMM9 :  printf("XMM9  "); break;
		case XMM10:  printf("XMM10 "); break;
		case XMM11:  printf("XMM11 "); break;
		case XMM12:  printf("XMM12 "); break;
		case XMM13:  printf("XMM13 "); break;
		case XMM14:  printf("XMM14 "); break;
		case XMM15:  printf("XMM15 "); break;
		case STSRG:  printf("STSRG "); break;
		case NOREG:  printf("NOREG "); break;
		default:     printf("?REG? "); break;
	}
}

void printCondCode(char** ret, X86Cond c){
	switch(c){
		case CC_O  : *ret = "o "; break;
		case CC_NO : *ret = "no"; break;
		case CC_B  : *ret = "b "; break;
		case CC_AE : *ret = "ae"; break;
		case CC_E  : *ret = "e "; break;
		case CC_NE : *ret = "ne"; break;
		case CC_BE : *ret = "be"; break;
		case CC_A  : *ret = "a "; break;
		case CC_S  : *ret = "s "; break;
		case CC_NS : *ret = "ns"; break;
		case CC_PE : *ret = "pe"; break;
		case CC_PO : *ret = "po"; break;
		case CC_L  : *ret = "l "; break;
		case CC_GE : *ret = "ge"; break;
		case CC_LE : *ret = "le"; break;
		case CC_G  : *ret = "g "; break;
		default    : *ret = "? "; break;
	}
}


void printX86Opcode(X86Opcode opc, X86Cond c){
	char* buffer = "? ";
	printCondCode(&buffer, c);
	switch(opc){
		case XO_ADD    : printf("ADD    "        ); break;
		case XO_SUB    : printf("SUB    "        ); break;
		case XO_ADC    : printf("ADC    "        ); break;
		case XO_SBB    : printf("SBB    "        ); break;
		case XO_MUL    : printf("MUL    "        ); break;
		case XO_DIV    : printf("DIV    "        ); break;
		case XO_IMUL   : printf("IMUL   "        ); break;
		case XO_IDIV   : printf("IDIV   "        ); break;
		case XO_CMP    : printf("CMP    "        ); break;
		case XO_NEG    : printf("NEG    "        ); break;
		case XO_INC    : printf("INC    "        ); break;
		case XO_DEC    : printf("DEC    "        ); break;
		case XO_SETcc  : printf("SET%s  ", buffer); break;
		
		case XO_AND    : printf("AND    "        ); break;
		case XO_OR     : printf("OR     "        ); break;
		case XO_XOR    : printf("XOR    "        ); break;
		case XO_NOT    : printf("NOT    "        ); break;
		case XO_POPCNT : printf("POPCNT "        ); break;
		case XO_BSF    : printf("BSF    "        ); break;
		case XO_BSR    : printf("BSR    "        ); break;
		case XO_LZCNT  : printf("LZCNT  "        ); break;
		case XO_TZCNT  : printf("TZCNT  "        ); break;
		case XO_SAR    : printf("SAR    "        ); break;
		case XO_SHL    : printf("SHL    "        ); break;
		case XO_SHR    : printf("SHR    "        ); break;
		case XO_ROL    : printf("ROL    "        ); break;
		case XO_ROR    : printf("ROR    "        ); break;
		case XO_RCL    : printf("RCL    "        ); break;
		case XO_RCR    : printf("RCR    "        ); break;
		case XO_TEST   : printf("TEST   "        ); break;
		
		case XO_MOV    : printf("MOV    "        ); break;
		case XO_PUSH   : printf("PUSH   "        ); break;
		case XO_POP    : printf("POP    "        ); break;
		case XO_LDMOV  : printf("LDMOV  "        ); break;
		case XO_STMOV  : printf("STMOV  "        ); break;
		case XO_CMOVcc : printf("CMOV%s ", buffer); break;
		
		case XO_RET    : printf("RET    "        ); break;
		case XO_JMP    : printf("JMP    "        ); break;
		case XO_Jcc    : printf("J%s    ", buffer); break;
		case XO_CALL   : printf("CALL   "        ); break;
		case XO_INT    : printf("INT    "        ); break;
		
		default        : printf("?OP?   "        ); break;
	}
}


void printX86Size(X86Size s){
	switch(s){
		case SC_8  : printf("8b  "); break;
		case SC_16 : printf("16b "); break;
		case SC_32 : printf("32b "); break;
		case SC_64 : printf("64b "); break;
	}
}


void printX86Op(X86Op opc){
	printX86Opcode(opc.opc, opc.cond);
	printX86Size  (opc.size);
	printX86Reg   (opc.ra);
	printX86Reg   (opc.rb);
	printf(" #%lu %i %i > %i %i\n", opc.imm, opc.a, opc.b, opc.q, opc.r);
}


void printX86Func(X86Func* fn){
	printf("VARS =\n");
	for(int i = 0; i < fn->varct; i++){
		printf("%i : ", i);
		printX86Reg(fn->varregs[i]);
		printf("%i > %i\n", fn->vardefs[i], fn->varends[i]);
	}

	printf("BLOCKS =\n");
	for(int i = 0; i < fn->bct; i++){
		printf("BK %i\n", i);
		for(int j = 0; j < fn->blocks[i].opct; j++){
			X86Op opc = fn->blocks[i].ops[j];
			printX86Opcode(opc.opc, opc.cond);
			printX86Size  (opc.size);
			printX86Reg   (opc.ra);
			printX86Reg   (opc.rb);
			printf(" #%lu %i %i > %i %i\n", opc.imm, opc.a, opc.b, opc.q, opc.r);
		}
	}
}


int appendX86Var(X86Func* fn){
	if(fn->varct+1 >= fn->varcap){
		X86Register* rtmp = fn->varregs;
		int*         dtmp = fn->vardefs;
		int*         etmp = fn->varends;
		fn->varcap *= 2;
		fn->varregs = malloc(sizeof(X86Register) * fn->varcap);
		fn->vardefs = malloc(sizeof(int) * fn->varcap);
		fn->varends = malloc(sizeof(int) * fn->varcap);
		for(int i = 0; i < fn->varct; i++){
			fn->varregs[i] = rtmp[i];
			fn->vardefs[i] = dtmp[i];
			fn->varends[i] = etmp[i];
		}
		free(rtmp);
		free(dtmp);
		free(etmp);
	}
	fn->varct += 1;
	return fn->varct-1;
}


X86Block makeX86Block(int opcap){
	X86Block ret;
	ret.ops   = malloc(sizeof(X86Op) * opcap);
	ret.opct  = 0;
	ret.opcap = opcap;
	for(int i = 0; i < opcap; i++){ ret.ops[i].ra = NOREG; ret.ops[i].rb = NOREG; }
	return ret;
}


int appendX86Block(X86Func* fn){
	if(fn->bct+1 >= fn->bcap){
		X86Block* tmp = fn->blocks;
		fn->bcap  *= 2;
		fn->blocks = malloc(sizeof(X86Block) * fn->bcap);
		for(int i  = 0; i < fn->bct; i++) fn->blocks[i] = tmp[i];
		free(tmp);
	}
	fn->bct += 1;
	fn->blocks[fn->bct-1] = makeX86Block(16);
	return fn->bct-1;
}


int appendX86Op(X86Block* blk){
	if(blk->opct+1 >= blk->opcap){
		X86Op* tmp  = blk->ops;
		blk->opcap *= 2;
		blk->ops    = malloc(sizeof(X86Op) * blk->opcap);
		for(int i = 0; i < blk->opct; i++) blk->ops[i] = tmp[i];
		free(tmp);
	}
	blk->opct += 1;
	blk->ops[blk->opct-1].ra = NOREG;
	blk->ops[blk->opct-1].rb = NOREG;
	return blk->opct-1;
}


X86Func makeX86Func (int bcap, int varcap){
	X86Func ret;
	ret.blocks  = malloc(sizeof(X86Block) * bcap);
	ret.bct     = 0;
	ret.bcap    = bcap;
	ret.varregs = malloc(sizeof(X86Register) * varcap);
	ret.vardefs = malloc(sizeof(int) * varcap);
	ret.varends = malloc(sizeof(int) * varcap);
	ret.varct   = 0;
	ret.varcap  = varcap;
	return ret;
}



int isCommutative(X86Opcode opc){
	switch(opc){
		case XO_ADD    : return 1;
		case XO_ADC    : return 1;
		case XO_MUL    : return 1;
		case XO_IMUL   : return 1;
		case XO_AND    : return 1;
		case XO_OR     : return 1;
		case XO_XOR    : return 1;
		default        : return 0;
	}
	return 0;
}



int implicitRegisters(X86Op opc, X86Register* varregs, int* regvars){
	switch(opc.opc){
		case XO_MUL: {
			varregs[opc.r] = RAX;
			regvars[RAX  ] = opc.r;
			varregs[opc.r] = RDX;
			regvars[RDX  ] = opc.r;
		}break;
		
		case XO_DIV: {
			varregs[opc.r] = RAX;
			regvars[RAX  ] = opc.r;
			varregs[opc.r] = RDX;
			regvars[RDX  ] = opc.r;
		}break;
		
		default: return 0;
	}
	
	return 1;
}


typedef struct{
	int* vallocs;
	int* regvals;
	int* refcts;
	int  varct, stacksize;
}RegisterTable;


void resizeRegisterTable(RegisterTable* tab, int varct, int stacksize){
	if(tab->vallocs == NULL){
		tab->vallocs = malloc(sizeof(int) * varct);
		for(int i = 0; i < tab->varct; i++) tab->vallocs[i] = 0;
	}else{
		int* tmp = tab->vallocs;
		tab->vallocs = malloc(sizeof(int) * varct);
		for(int i = 0; i < tab->varct; i++) tab->vallocs[i] = tmp[i];
	}
	
	if(tab->regvals == NULL){
		tab->regvals = malloc(sizeof(int) * (stacksize + 32));
		for(int i = 0; i < (tab->stacksize+32); i++) tab->regvals[i] = 0;
	}else{
		int* tmp = tab->regvals;
		tab->regvals = malloc(sizeof(int) * varct);
		for(int i = 0; i < (tab->stacksize+32); i++) tab->regvals[i] = tmp[i];
	}
	
	if(tab->refcts == NULL){
		tab->refcts = malloc(sizeof(int) * varct);
		for(int i = 0; i < tab->varct; i++) tab->refcts[i] = 0;
	}else{
		int* tmp = tab->refcts;
		tab->refcts = malloc(sizeof(int) * varct);
		for(int i = 0; i < tab->varct; i++) tab->refcts[i] = tmp[i];
	}
	
	tab->varct     = varct;
	tab->stacksize = stacksize;
}


void x86AllocRegs(X86Block* blk){
	
	int varct = 0;
	for(int i = 0; i < blk->opct; i++){
		X86Op op = blk->ops[i];
		varct = (varct > op.a)? varct : op.a;
		varct = (varct > op.b)? varct : op.b;
		varct = (varct > op.q)? varct : op.q;
		varct = (varct > op.r)? varct : op.r;
	}
	varct++;
	
	printf("VARCT = %i\n", varct);
	int*        rdcts   = malloc(sizeof(int) * varct);
	for(int i = 0; i < varct; i++){ rdcts[i] = 0; }
	for(int i = 0; i < blk->opct; i++){
		X86Op op = blk->ops[i];
		if(op.a > -1) rdcts[op.a]++;
		if(op.b > -1) rdcts[op.b]++;
	}
	
	for(int i = 0; i < varct; i++){
		if(rdcts[i] != 0){
			printf("%i : %i\n", i, rdcts[i]);
		}
	}
	
	for(int i = 0; i < 32; i++){
		if(blk->invars[i] == -1){
			printf("[X] ");
		}else if(blk->invars[i] == -2){
			printf("[P] ");
		}else if(blk->invars[i] == -3){
			printf("[_] ");
		}else{
			printf("[%i] ", blk->invars[i]);
		}
	}printf("\n");
	
	for(int i = 0; i < 32; i++){
		if(blk->exvars[i] == -1){
			printf("[X] ");
		}else if(blk->exvars[i] == -2){
			printf("[P] ");
		}else if(blk->exvars[i] == -3){
			printf("[_] ");
		}else{
			printf("[%i] ", blk->exvars[i]);
		}
	}printf("\n");
	
	int regvars[32];
	for(int i = 0; i < 32; i++) regvars[i] = blk->invars[i];
	
	int* rds = malloc(sizeof(int) * varct);
	int* rgs = malloc(sizeof(int) * varct);
	
	int tries    = 0;
	X86Block ret = makeX86Block(blk->opct * 2);
	while(1){
		for(int i = 0; i < varct; i++) rds[i] = rdcts[i];
		
		for(int i = 0; i < blk->opct; i++){
			int a  = blk->ops[i].a;
			int b  = blk->ops[i].b;
			int q  = blk->ops[i].q;
			int r  = blk->ops[i].r;
			
			// TODO: handle cases where a, b, q, or r are negative
			if((rds[a] == 1) || (rds[b] == 1)){
				if(rds[a] == 1){
					// Store q in a register
					int ix = appendX86Op(&ret);
					ret.ops[ix] = blk->ops[i];
				}else if(isCommutative(blk->ops[i].opc)){
					// Add opc b a -> q r
					// Store q in b register
					int ix = appendX86Op(&ret);
					ret.ops[ix] = blk->ops[i];
				}else{
					// Make new spot for c
					// Add mov a -> c
					// Add opc c b -> q r
					// Store q in c register
					int ix = appendX86Op(&ret);
					ret.ops[ix] = (X86Op){XO_MOV, SC_64, NOREG, NOREG, 0, -1, -1, -1, -1, CC_NOCODE, XF_NOFLAGS};
					ix     = appendX86Op(&ret);
					ret.ops[ix] = blk->ops[i];
				}
				rds[a]--;
				rds[b]--;
			}
		}
		
		if(1) break;
		
		ret.opct = 0;
		tries++;
		if(tries > 10){
			printf("Too many tries in register allocation.\n");
			break;
		}
	}
	
	free(blk->ops);
	*blk = ret;
	
	free(rgs);
	free(rds);
	free(rdcts);
}
