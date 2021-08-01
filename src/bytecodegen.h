#ifndef __BYTECODEGEN_HEADER__
#define __BYTECODEGEN_HEADER__


#include "stdint.h"

#include "program.h"
#include "ast.h"


typedef enum{

	// Arithmetic
	BC_ADD		= 0x0001,
	BC_SUB		= 0x0002,
	BC_MUL		= 0x0003,
	BC_DIV		= 0x0004,
	BC_MOD		= 0x0005,
	
	// Bit Manipulation
	BC_AND		= 0x0101,
	BC_OR		= 0x0102,
	BC_XOR		= 0x0103,
	BC_NOT		= 0x0104,
	BC_SHL		= 0x0105,
	BC_SHR		= 0x0106,
	BC_ROL		= 0x0107,
	BC_ROR		= 0x0108,
	BC_PCT		= 0x0109,
	BC_CTZ		= 0x010A,
	BC_CLZ		= 0x010B,
	
	// Ordering
	BC_ILS		= 0x0201,
	BC_ILSE		= 0x0202,
	BC_IGT		= 0x0203,
	BC_IGTE		= 0x0204,
	BC_ULS		= 0x0205,
	BC_ULSE		= 0x0206,
	BC_UGT		= 0x0207,
	BC_UGTE		= 0x0208,
	BC_EQ		= 0x0209,
	BC_NEQ		= 0x020A,
	BC_NZ		= 0x020B,
	
	// Loop
	BC_LOOP		= 0x1001,
	BC_FOR		= 0x1002,
	BC_MAP		= 0x1003,
	BC_FOLD		= 0x1004,
	BC_SCAN		= 0x1005,
	BC_FILTER	= 0x1006,
	BC_ANY		= 0x1007,
	BC_ALL		= 0x1008,
	BC_NONE		= 0x1009,
	BC_ZIP		= 0x100A,
	BC_UNZIP	= 0x100B,
	
	// Flow
	BC_CALL		= 0x1101,
	BC_IF		= 0x1102,
	BC_IFE		= 0x1103,
	BC_RET		= 0x1104,
	
	// Memory
	BC_LD		= 0x1201,
	BC_ST		= 0x1202,
	BC_LDIX		= 0x1203,
	BC_STIX		= 0x1204,
	BC_PUSH		= 0x1205,
	BC_POP		= 0x1206,
	BC_ALLOC	= 0x1207,
	BC_FREE		= 0x1208
}Opcode;

typedef enum{
	BT_8,		BT_8x2,		BT_8x4,		BT_8x8,		BT_8x16,	BT_8x32,
	BT_16,		BT_16x2,	BT_16x4,	BT_16x8,	BT_16x16,
	BT_32,		BT_32x2,	BT_32x4,	BT_32x8,
	BT_64,		BT_64x2,	BT_64x4,
	BT_128,		BT_128x2,
	BT_256
}BytecodeType;

typedef struct{
	Opcode   		opc;
	uint32_t 		a, b, c;
	uint64_t 		imm;
	BytecodeType	size;
}Bytecode;

typedef struct{
	Bytecode* code;
	uint32_t  blockId;
	int       size, cap;
}BCBlock;

typedef struct{
	uint64_t	type;
	uint64_t*	refs;
	uint64_t    def;
	int			refct, refcap;
}BCVariable;

typedef struct{
	BCVariable*	vars;
	BCBlock*	blks;
	int varct, varcap, blkct, blkcap;
}BytecodeFunction;


BytecodeFunction buildFunction(SymbolTable*, ASTFnDef*);



#endif
