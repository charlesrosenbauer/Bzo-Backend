#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "parser.h"
#include "memory.h"
#include "util.h"
#include "types.h"
//#include "x86-reader.h"
#include "hashtable.h"
//#include "elf.h"
#include "struct.h"

#include "x86.h"

#include "bytecode.h"


#include <sys/mman.h>




int main(int argc, char** argv){

/*
	{
		uint8_t* buffer = mmap(NULL, 1024, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
		printf("Buffer address: %lu\n", buffer);
		
		X86Op opc;
		opc.opc     = X86_ADD;
		opc.bitsize = SC_64;
		opc.a       = RDI;
		opc.b       = RDI;
		opc.lock    = 0;
		opc.mode    = AM_RR;
	
		
		int      size = writeX86Op(opc, buffer);
		
		opc.opc     = X86_MOV;
		opc.a       = RDI;
		opc.b       = RAX;
		
		size += writeX86Op(opc, &buffer[size]);
		
		opc.opc     = X86_RETN;
		
		size += writeX86Op(opc, &buffer[size]);
	
		printHexBuffer(buffer, size);
		
		int(*f)(int) = buffer;
		printf("%i * 2 = %i\n", 7, f(7));
	}

*/
  
	/*
	FILE*  pFile;
	uint8_t*  buffer;
	int       size;
	size_t    result;

	if(argc < 2){
		printf("No input program.\n");
		return -1;
	}

	loadFile(argv[1], &buffer, &size);  

	PARSERSTATE p;
	p.text = (char*)buffer;
	p.head = 0;
	p.size = size;

	PROGRAM* prog;
	prog = parseProgram(&p, 64, 64);

	int err = buildTypes(prog);
	if(err != 0){
		printf("Error building types.\n");
		return -1;
	}

	printProgram(prog);*/

	/*
	CodeBlock blk = makeCodeBlock(BKT_FUNCTION_HEAD, 64, 64, 3, 1);
	appendOpcode(&blk, (IR_Instruction){IR_ADD, IRP_I64, (IR_Pars){1, 2, 0, 6, 0}});
	appendOpcode(&blk, (IR_Instruction){IR_NOP, IRP_I64, (IR_Pars){3, 6, 0, 5, 0}});
	appendOpcode(&blk, (IR_Instruction){IR_ADD, IRP_I64, (IR_Pars){3, 6, 0, 4, 0}});
	printCodeBlock(blk);
	printf("#: %lu\n", hashBlock(&blk));

	blk = denopCodeBlock(blk);
	printCodeBlock(blk);
	printf("#: %lu\n", hashBlock(&blk));*/
  
  
  	/*
	X86BCBlock blk;
	blk.ops    = malloc(sizeof(X86BCOp) * 16);
	blk.opct   = 4;
	blk.opcap  = 16;
	blk.ops[0] = (X86BCOp){X86_ADD , SC_64, 1, 2, 0, 4, 0};
	blk.ops[1] = (X86BCOp){X86_ADD , SC_64, 4, 3, 0, 5, 0};
	blk.ops[2] = (X86BCOp){X86_MUL , SC_64, 5, 2, 0, 6, 0};
	blk.ops[3] = (X86BCOp){X86_RETN, SC_64, 6, 0, 0, 0, 0};
  
	X86Block x86 = allocRegs(blk);*/
}
