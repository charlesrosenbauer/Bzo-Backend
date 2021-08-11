#ifndef __PROGRAM_HEADER__
#define __PROGRAM_HEADER__


#include "stdint.h"

#include "common.h"
#include "ast.h"
#include "postprocess.h"
#include "bytecodegen.h"


typedef struct{
	int64_t		name;
	int64_t		fileId;
	int64_t*	defs;
	int64_t*	visible;
	int			defct, visct;
	
	char*		path;
	char*		text;
	int			fsize;
	
	//ASTProgram  prog;
}ProgramFile;

typedef struct{
	SymbolTable		syms;
	ProgramFile*	files;
	
	
	
	//TypeTable		ttab;
	//FuncTable		ftab;

	int 			filect, namect;
}Program;


int compile(Program*, char**, int);


#endif
