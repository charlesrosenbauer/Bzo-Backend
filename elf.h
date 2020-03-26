#ifndef __ELF_HEADER__
#define __ELF_HEADER__


#include "stdint.h"
#include "codegen.h"




typedef struct{
	uint32_t  magic;
	uint8_t   bitsize;
	uint8_t   endianness;
	uint8_t   elfversion0;
	uint8_t   abi;
	uint8_t   abiversion;
	uint8_t   unusedA[7];
	uint16_t  objfiletype;
	uint16_t  isa;
	uint32_t  elfversion1;
	uint64_t  entrypos;
	uint64_t  pheadoffset;
	uint64_t  sheadoffset;
	uint32_t  flags;
	uint16_t  headersize;
	uint16_t  pheadersize;
	uint16_t  pheadcount;
	uint16_t  sheadersize;
	uint16_t  sheadcount;
	uint16_t  sheadindex;
}ELFHEADER;


typedef struct{
	uint32_t  p_type;
	uint32_t  p_flags;
	uint64_t  p_offset;
	uint64_t  p_vaddr;
	uint64_t  p_paddr;
	uint64_t  p_filesz;
	uint64_t  p_memsz;
	uint64_t  p_align;
}PROGRAMHEADER;





int makeELF (uint8_t**, uint8_t*, int, uint8_t*, int);
int writeELF(char*,     uint8_t*, int);


#endif
