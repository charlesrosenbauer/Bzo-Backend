#include "stdio.h"
#include "stdint.h"
#include "elf.h"
//#include "codegen.h"
#include "stdlib.h"





int makeELF(uint8_t** program, uint8_t* code, int codesize, uint8_t* data, int datasize){
	int  buffersize = sizeof(ELFHEADER) + sizeof(PROGRAMHEADER) + codesize + datasize;
	uint8_t* buffer = malloc(buffersize);

	int bufferhead = 0;
	ELFHEADER* eheader     = (ELFHEADER*)&buffer[bufferhead];
	eheader->magic         = 0x464c457f;
	eheader->bitsize       = 2;
	eheader->endianness    = 1;
	eheader->elfversion0   = 1;
	eheader->abi           = 0;
	eheader->abiversion    = 0;
	eheader->objfiletype   = 2;
	eheader->isa           = 0x3e;
	eheader->elfversion1   = 1;
	eheader->entrypos      = 0;		// entry
	eheader->pheadoffset   = 0x40;
	eheader->sheadoffset   = 0xb0;
	eheader->flags         = 0;
	eheader->headersize    = 64;
	eheader->pheadersize   = 0x20;
	eheader->pheadcount    = 1;
	eheader->sheadersize   = 0x28;
	eheader->sheadcount    = 4;
	eheader->sheadindex    = 3;

	bufferhead = 0x40;
	PROGRAMHEADER* pheader = (PROGRAMHEADER*)&buffer[bufferhead];
	pheader->p_type        = 1;
	pheader->p_flags       = 5;
	pheader->p_offset      = 0;
	pheader->p_vaddr       = 0x8000000;
	pheader->p_paddr       = 0x8000000;
	pheader->p_filesz      = 0x90;
	pheader->p_memsz       = 0x90;
	pheader->p_align       = 1;



	return buffersize;
}



int writeELF(char* fname, uint8_t* program, int programsize){
	FILE* f = fopen(fname, "wb");

	fwrite(program, programsize, 1, f);

	fclose(f);

	return 0;
}