#include "defs.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"





char** makeOpNameTab(){
	// Just going to make the table bigger than necessary for now
	char** ret = malloc(sizeof(char*) * 256);

	ret[OP_ADD  ] = "ADD";
	ret[OP_SUB  ] = "SUB";
	ret[OP_ADC  ] = "ADC";
	ret[OP_SBB  ] = "SBB";
	ret[OP_MUL  ] = "MUL";
	ret[OP_IMUL ] = "IMUL";
	ret[OP_DIV  ] = "DIV";
	ret[OP_IDIV ] = "IDIV";
	ret[OP_XOR  ] = "XOR";
	ret[OP_AND  ] = "AND";
	ret[OP_OR   ] = "OR";
	ret[OP_SHL  ] = "SHL";
	ret[OP_SHR  ] = "SHR";
	ret[OP_ROL  ] = "ROL";
	ret[OP_ROR  ] = "ROR";
	ret[OP_SAL  ] = "SAL";
	ret[OP_SAR  ] = "SAR";
	ret[OP_XCHG ] = "XCHG";
	ret[OP_CMOV ] = "CMOV";
	ret[OP_SET  ] = "SET";
	ret[OP_J    ] = "J";
	ret[OP_JMP  ] = "JMP";
	ret[OP_CALL ] = "CALL";
	ret[OP_RET  ] = "RET";
	ret[OP_MOV  ] = "MOV";
	ret[OP_TEST ] = "TEST";
	ret[OP_PCT  ] = "PCT";
	ret[OP_CTZ  ] = "CTZ";
	ret[OP_CLZ  ] = "CLZ";
	ret[OP_LD   ] = "LD";
	ret[OP_STR  ] = "STR";
	ret[OP_LEA  ] = "LEA";
	ret[OP_NOT  ] = "NOT";
	ret[OP_NEG  ] = "NEG";
	ret[OP_INC  ] = "INC";
	ret[OP_DEC  ] = "DEC";
	ret[OP_INT  ] = "INT";
	ret[OP_NOP  ] = "NOP";
	ret[OP_PUSH ] = "PUSH";
	ret[OP_POP  ] = "POP";
	ret[OP_PUSHF] = "PUSHF";
	ret[OP_POPF ] = "POPF";
	ret[OP_STC  ] = "STC";
	ret[OP_STD  ] = "STD";
	ret[OP_CMP  ] = "CMP";

	ret[OP_CNST ] = "CNST";
	ret[OP_ARGS ] = "ARGS";
	ret[OP_ZERO ] = "ZERO";
	ret[OP_CTRL ] = "CTRL";

	return ret;
}


int findOp(char** opnames, char* name){
	for(int i = 0; i < 256; i++){
		if(!strcmp(opnames[i], name)) return i;
	}
	return -1;
}


char* getCCText(Settings cc){
	if(cc & COND){
		switch(cc & COND){
			case C_LS  : return "L";
			case C_LSE : return "LE";
			case C_GT  : return "G";
			case C_GTE : return "GE";
			case C_EQ  : return "E";
			case C_NEQ : return "NE";
			case C_O   : return "O";
			case C_S   : return "S";
			case C_Z   : return "Z";
			case C_NZ  : return "NZ";
		}
	}
	return "";
}


Block newBlock(BlockType ty, int capacity){
	Block ret;
	ret.code     = malloc(sizeof(OP) * capacity);
	ret.capacity = capacity;
	ret.size     = 0;
	ret.type     = ty;
	return ret;
}


void addBlkOp(Block* blk, OP op){
	if(blk->size+1 >= blk->capacity){
		OP* code = malloc(sizeof(OP) * blk->capacity * 2);
		for(int i = 0; i < blk->size; i++){
			code[i] = blk->code[i];
		}
		free(blk->code);
		blk->code = code;
		blk->capacity *= 2;
	}
	blk->code[blk->size] = op;
	blk->size++;
}



void printOP(char** opnames, OP opcode){
	char q [32];
	char r [32];
	char a [32];
	char b [32];
	char m [32];

	if(opcode.q >= 0)
		sprintf(q, "#%i", opcode.q);
	else
		q[0] = '\0';


	if(opcode.r >= 0)
		sprintf(r, "#%i", opcode.r);
	else
		r[0] = '\0';


	if(opcode.a >= 0)
		sprintf(a, "#%i", opcode.a);
	else
		a[0] = '\0';


	if(opcode.b >= 0)
		sprintf(b, "#%i", opcode.b);
	else
		b[0] = '\0';


	if(opcode.imm != 0)
		sprintf(m, "<%#010x>", opcode.imm);
	else
		m[0] = '\0';


	printf("%s %s = %s%s %s %s %s\n", q, r, opnames[opcode.opc], getCCText(opcode.settings), a, b, m);
}


void printBlock(char** opnames, Block blk, int blkNum){
	printf("===================\n");
	if(blk.type == FUNCHEAD){
		printf("FUNCTION %#010x:\n", blkNum);
	}else if(blk.type == INNERLOOP){
		printf("LOOP     %#010x:\n", blkNum);
	}else{
		printf("BRANCH   %#010x:\n", blkNum);
	}
	for(int i = 0; i < blk.size; i++)
		printOP(opnames, blk.code[i]);
	printf("===================\n\n");
}


void printProgram(Program p){
	for(int i = 0; i < p.size; i++)
		printBlock(p.opNames, p.blocks[i], i);
}



Program newProgram(int capacity){
	Program ret;
	ret.blocks   = malloc(sizeof(Block) * capacity);
	ret.capacity = capacity;
	ret.size     = 0;
	ret.opNames  = makeOpNameTab();
	ret.opProps  = loadOpProps(ret.opNames, "op.props");
	return ret;
}


void addPrgmBlk(Program* p, Block blk){
	if(p->size+1 >= p->capacity){
		Block* blocks = malloc(sizeof(Block) * p->capacity * 2);
		for(int i = 0; i < p->size; i++){
			blocks[i] = p->blocks[i];
		}
		free(p->blocks);
		p->blocks    = blocks;
		p->capacity *= 2;
	}
	p->blocks[p->size] = blk;
	p->size++;
}


void printRegisters(char* buffer, Registers r){
	sprintf(buffer, "[%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s]", 
		(r & R_RA  )? "RAX,"   : "",
		(r & R_RB  )? "RBX,"   : "",
		(r & R_RC  )? "RCX,"   : "",
		(r & R_RD  )? "RDX,"   : "",
		(r & R_BP  )? "RBP,"   : "",
		(r & R_SP  )? "RSP,"   : "",
		(r & R_SI  )? "RSI,"   : "",
		(r & R_DI  )? "RDI,"   : "",
		(r & R_R8  )? "R8,"    : "",
		(r & R_R9  )? "R9,"    : "",
		(r & R_R10 )? "R10,"   : "",
		(r & R_R11 )? "R11,"   : "",
		(r & R_R12 )? "R12,"   : "",
		(r & R_R13 )? "R13,"   : "",
		(r & R_R14 )? "R14,"   : "",
		(r & R_R15 )? "R15,"   : "",
		(r & R_X0  )? "XMM0,"  : "",
		(r & R_X1  )? "XMM1,"  : "",
		(r & R_X2  )? "XMM2,"  : "",
		(r & R_X3  )? "XMM3,"  : "",
		(r & R_X4  )? "XMM4,"  : "",
		(r & R_X5  )? "XMM5,"  : "",
		(r & R_X6  )? "XMM6,"  : "",
		(r & R_X7  )? "XMM7,"  : "",
		(r & R_X8  )? "XMM8,"  : "",
		(r & R_X9  )? "XMM9,"  : "",
		(r & R_X10 )? "XMM10," : "",
		(r & R_X11 )? "XMM11," : "",
		(r & R_X12 )? "XMM12," : "",
		(r & R_X13 )? "XMM13," : "",
		(r & R_X14 )? "XMM14," : "",
		(r & R_X15 )? "XMM15," : "",
		(r & R_FLGS)? "FLAGS," : ""
	);
}


void printPipes(char* buffer, Pipes p){
	if(p & FULL_P){
		sprintf(buffer, "[FULL]");
	}else{
		sprintf(buffer, "[%s%s%s%s|%s%s%s|%s%s%s%s]",
			(p &  INT0_P)? "INT0," : "",
			(p &  INT1_P)? "INT1," : "",
			(p &  INT2_P)? "INT2," : "",
			(p &  INT3_P)? "INT3," : "",
			(p &  DMA0_P)? "DMA0," : "",
			(p &  DMA1_P)? "DMA1," : "",
			(p &  DMA2_P)? "DMA2," : "",
			(p &  FPU0_P)? "FPU0," : "",
			(p &  FPU1_P)? "FPU1," : "",
			(p &  FPU2_P)? "FPU2," : "",
			(p &  FPU3_P)? "FPU3," : ""
		);
	}
}


void printOpProps(OpcodeProperties* props){
	for(int i = 0; i < 256; i++){
		char buffers[9][1024];
		printPipes(buffers[0], props[i].pipes[0]);
		printPipes(buffers[1], props[i].pipes[1]);
		printPipes(buffers[2], props[i].pipes[2]);
		printPipes(buffers[3], props[i].pipes[3]);

		printRegisters(buffers[4], props[i].a);
		printRegisters(buffers[5], props[i].b);
		printRegisters(buffers[6], props[i].q);
		printRegisters(buffers[7], props[i].r);
		printRegisters(buffers[8], props[i].s);

		printf("%i %s : %d cycles, %s %s %s %s | %s %s > %s %s | %s\n",
				i, props[i].name, props[i].latency,
				buffers[0], buffers[1], buffers[2], buffers[3],
				buffers[4], buffers[5], buffers[6], buffers[7], buffers[8]);
	}
}



void resizeTableau(Tableau* t){
	OP* tab  = t->table;
	t->table = malloc(sizeof(OP) * t->h * t->w * 2);
	for(int i = 0; i < (t->h * t->w); i++){
		t->table[i]               = tab[i];
		t->table[i+(t->h * t->w)] = (OP){0, 0, 0, 0, 0, 0, OP_NOP};
	}
	free(tab);
}



Tableau makeTableau(int depth, int pipes){
	Tableau ret;
	ret.table = malloc(sizeof(OP) * depth * pipes);
	for(int i = 0; i < (depth*pipes); i++) ret.table[i] = (OP){0, 0, 0, 0, 0, 0, OP_NOP};
	ret.h     = depth;
	ret.w     = pipes;
	return ret;
}