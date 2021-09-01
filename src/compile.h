#ifndef __PROGRAM_HEADER__
#define __PROGRAM_HEADER__


#include "stdint.h"

#include "common.h"
#include "ast.h"
#include "postprocess.h"
#include "bytecodegen.h"


typedef struct{
	ASTFnDef* astdef;
	List      bcodes;	// Bytecode index (int64_t)
	int64_t	  id;
}FuncDef;

typedef struct{
	ASTTyDef* astdef;
	List	  layouts;	// Layout index (int64_t)
	int64_t   id;
}TypeDef;

typedef struct{
	List	fndefs;		// FuncDef
	List	tydefs;		// TypeDef
}DefinitionTable;

typedef struct{
	List	names;
	int64_t id;
}NameList;

typedef struct{
	int64_t modName;
	List	names;		// NameList
}Namespace;




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
	
	Namespace   names;
}ProgramFile;

typedef struct{
	SymbolTable		syms;
	ProgramFile*	files;
	
	
	
	List			fndefs;		// FuncDef
	List			tydefs;		// TypeDef

	int 			filect, namect;
}Program;

int buildProgramMap(Program*, ErrorList*);
int compile        (Program*, char**, int);


#endif
