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
#include "print.h"

#include "x86.h"

#include "bytecode.h"
#include "ir.h"


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
  
	
	FILE*  pFile;
	uint8_t*  buffer;
	int       size;
	size_t    result;

	if(argc < 2){
		printf("No input program.\n");
		return -1;
	}

	loadFile(argv[1], &buffer, &size);  

	ParserState p;
	p.text = (char*)buffer;
	p.head = 0;
	p.size = size;

	Program* prog;
	prog = parseProgram(&p, 64, 64, 64, 64);
	if(prog == NULL) return 1;
	printf("%i fs, %i ts, %i cs, %i is\n", prog->fnct, prog->tyct, prog->tcct, prog->imct);
	
	for(int i = 0; i < 64; i++){
		if(prog->funcs[i].fnid == i)
			printFunction(&prog->funcs[i]);
	}
	
	for(int i = 0; i < 64; i++){
		if(prog->types[i].tyid == i)
			printType(&prog->types[i]);
	}
	
	buildTypes(prog);
	
	//BCProgram bc;
	//buildBytecode(prog, &bc);

	/*
	int err = buildTypes(prog);
	if(err != 0){
		printf("Error building types.\n");
		return -1;
	}*/

	//printProgram(prog);

	/*
	X86Function fn = makeFunction(5, 1, 2, 4, 8);
	// Block 0
	appendOp(&fn.blocks[0], (X86Op){X86_PARVAL  , (X86_VV_Addr){1, 0, 0, 0, 0}, AM_VV, SC_64, 0, 0});
	appendOp(&fn.blocks[0], (X86Op){X86_DEFVAL  , (X86_VV_Addr){2, 0, 0, 0, 0}, AM_VV, SC_64, 0, 0});
	appendOp(&fn.blocks[0], (X86Op){X86_ZERO_REG, (X86_VV_Addr){2, 0, 0, 0, 0}, AM_VV, SC_64, 0, 0});
	// Block 1
	appendOp(&fn.blocks[1], (X86Op){X86_DEFVAL  , (X86_VV_Addr){3, 0, 0, 0, 0}, AM_VV, SC_64, 0, 0});
	appendOp(&fn.blocks[1], (X86Op){X86_DEFVAL  , (X86_VV_Addr){4, 0, 0, 0, 0}, AM_VV, SC_64, 0, 0});
	appendOp(&fn.blocks[1], (X86Op){X86_CONST   , (X86_VV_Addr){4, 0, 0, 0, 0}, AM_VV, SC_64, 2, 0});
	appendOp(&fn.blocks[1], (X86Op){X86_DIV     , (X86_VV_Addr){1, 4, 0, 3, 0}, AM_VV, SC_64, 0, 0});
	appendOp(&fn.blocks[1], (X86Op){X86_CMP_JMP , (X86_CC_Addr){C86_Z,   3, 0}, AM_CC, SC_64, 3, 0});
	// Block 2
	appendOp(&fn.blocks[2], (X86Op){X86_DEFVAL  , (X86_VV_Addr){5, 0, 0, 0, 0}, AM_VV, SC_64, 0, 0});
	appendOp(&fn.blocks[2], (X86Op){X86_DEFVAL  , (X86_VV_Addr){6, 0, 0, 0, 0}, AM_VV, SC_64, 0, 0});
	appendOp(&fn.blocks[2], (X86Op){X86_CONST   , (X86_VV_Addr){6, 0, 0, 0, 0}, AM_VV, SC_64, 3, 0});
	appendOp(&fn.blocks[2], (X86Op){X86_DEFVAL  , (X86_VV_Addr){7, 0, 0, 0, 0}, AM_VV, SC_64, 0, 0});
	appendOp(&fn.blocks[2], (X86Op){X86_CONST   , (X86_VV_Addr){7, 0, 0, 0, 0}, AM_VV, SC_64, 1, 0});
	appendOp(&fn.blocks[2], (X86Op){X86_DIV     , (X86_VV_Addr){1, 4, 0, 3, 0}, AM_VV, SC_64, 0, 0});
	appendOp(&fn.blocks[2], (X86Op){X86_CMP_JMP , (X86_CC_Addr){C86_Z,   3, 0}, AM_CC, SC_64, 3, 0});*/
}








