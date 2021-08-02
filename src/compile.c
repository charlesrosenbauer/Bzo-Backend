#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "postprocess.h"
#include "bytecodegen.h"
#include "common.h"
#include "ast.h"
#include "compile.h"


#define COMPILE_DEBUG


int compile(Program* prog, char** files, int filect){

	ErrorList errs  = makeErrorList(64);
	
	if(0){
		error:
		
		return 0;
	}

	// Test case
	char*  testpath  = "tests/main.bzo";
	char** testfiles = &testpath;
	if(filect == 0){
		files  = testfiles;
		filect = 1;
	}


	// Load files
	prog->files = malloc(sizeof(ProgramFile) * filect);
	for(int i = 0; i < filect; i++){
		uint8_t* file;
		int      fsize = 0;
		
		// TODO: perform safety checks on file
		loadFile(files[i], &file, &fsize);
		if(fsize < 1){
			printf("Source code failed to load.\n");
			// TODO: Implement this as an error
			printf("Bad file path: %s\n", files[i]);
			goto error;
		}
		
		prog->files[i] = (ProgramFile){
			.name    = -1,	// To be set later - the symbol in the "module" header
			.fileId  =  i,
			.defs    =  NULL,
			.visible =  NULL,
			.defct   =  0,
			.visct   =  0,
			
			.path    =  files[i],
			.text    =  (char*)file,
			.fsize   =  fsize
		};
	}
	
	prog->syms = makeSymbolTable(4096);
	
	// Lex and Parse Files
	for(int i = 0; i < filect; i++){
	
		// Lexer and Symbol Table
		LangReader lr = (LangReader){prog->files[i].text, prog->files[i].fsize, 0, 1, 1, prog->files[i].fileId};
		//printf("Lexing file %s\n", prog->files[i].path);
		LexerState ls = lexer(&lr);
		symbolizeTokens(&prog->syms, &ls);
		
		// Parser
		if(parseCode(&ls, &prog->syms, &prog->files[i].prog, &errs)){
			printf("Program failed to parse.\n");
			goto error;
		}
	}
	
	
	#ifdef COMPILE_DEBUG
	for(int i = 0; i < filect; i++){
		printf("\n\n===============================\n");
		printf("File: %s\n", prog->files[i].path);
		printf(    "===============================\n");
		printASTProgram(prog->files[i].prog);
	}
	#endif
	
	
	// Assemble Namespaces
	
	
	
	// Type Sizing
	prog->ttab = makeTypeTable(1024);
	for(int i = 0; i < filect; i++)
		dumpToTypeTable(&prog->ttab, &prog->files[i].prog);
	
	if(sizeTypes(&prog->ttab)){
		printf("Program failed to pass type size checks.\n");
		goto error;
	}
	
	#ifdef COMPILE_DEBUG
	printTypeTable(&prog->ttab);
	#endif
	
	
	// Bytecode generation
	
	
	
	// Type checking
	
	
	
	// Analysis
	
	
	
	// Machine code generation
	
	
	return 1;
}
