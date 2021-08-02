#ifndef __PROGRAM_HEADER__
#define __PROGRAM_HEADER__


#include "stdint.h"

#include "common.h"
#include "ast.h"


typedef struct{
	int64_t		name;
	int64_t		fileId;
	int64_t*	defs;
	int64_t*	visible;
	int			defct, visct;
	
	char*		path;
	char*		text;
	int			fsize;
	
	ASTProgram  prog;
}ProgramFile;







#endif
