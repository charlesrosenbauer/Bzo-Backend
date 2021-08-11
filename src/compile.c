#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "time.h"

#include "postprocess.h"
#include "bytecodegen.h"
#include "common.h"
#include "ast.h"
#include "compile.h"


#define COMPILE_DEBUG
//#define COMPILE_PROFILE


int compile(Program* prog, char** files, int filect){

	clock_t start = clock(), diff;

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
		
		// Load file safety check
		int plen = strlen(files[i]);
		if((plen < 5) || strcmp(&files[i][plen-4], ".bzo")){
			printf("Source code failed to load.\n");
			printf("%s is not a .bzo file.\n", files[i]);
			goto error;
		}
		
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
	
	#ifdef COMPILE_PROFILE
	diff = clock() - start;
	printf("Loading took %f milliseconds.\n", 1000.0 * diff / CLOCKS_PER_SEC);
	#endif
	
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
	printSymbolTable(prog->syms);
	#endif
	
	#ifdef COMPILE_PROFILE
	diff = clock() - diff;
	printf("Parsing took %f milliseconds.\n", 1000.0 * diff / CLOCKS_PER_SEC);
	#endif
	
	
	#ifdef COMPILE_DEBUG
	for(int i = 0; i < filect; i++){
		printf("\n\n===============================\n");
		printf("File: %s\n", prog->files[i].path);
		printf(    "===============================\n");
		printASTProgram(prog->files[i].prog);
	}
	#endif
	
	
	
	
	// Assemble Namespaces
	
	
	#ifdef COMPILE_PROFILE
	diff = clock() - diff;
	printf("Namespace construction took %f milliseconds.\n", 1000.0 * diff / CLOCKS_PER_SEC);
	#endif
	
	
	
	
	// Type Sizing
	prog->ttab = makeTypeTable(1024);
	for(int i = 0; i < filect; i++)
		dumpToTypeTable(&prog->ttab, &prog->files[i].prog, prog->files[i].fileId);
	
	if(sizeTypes(&prog->ttab)){
		printf("Program failed to pass type size checks.\n");
		goto error;
	}
	
	#ifdef COMPILE_PROFILE
	diff = clock() - diff;
	printf("Sizing took %f milliseconds.\n", 1000.0 * diff / CLOCKS_PER_SEC);
	#endif
	
	#ifdef COMPILE_DEBUG
	printTypeTable(&prog->ttab);
	#endif
	
	
	
	
	// Bytecode generation
	
	
	
	#ifdef COMPILE_PROFILE
	diff = clock() - diff;
	printf("Bytecode generation took %f milliseconds.\n", 1000.0 * diff / CLOCKS_PER_SEC);
	#endif
	
	
	
	
	// Type checking
	
	
	
	#ifdef COMPILE_PROFILE
	diff = clock() - diff;
	printf("Type checking took %f milliseconds.\n", 1000.0 * diff / CLOCKS_PER_SEC);
	#endif
	
	
	
	
	// Analysis
	
	
	
	#ifdef COMPILE_PROFILE
	diff = clock() - diff;
	printf("Analysis took %f milliseconds.\n", 1000.0 * diff / CLOCKS_PER_SEC);
	#endif
	
	
	
	
	// Machine code generation
	
	
	
	#ifdef COMPILE_PROFILE
	diff = clock() - diff;
	printf("Machine code generation took %f milliseconds.\n", 1000.0 * diff / CLOCKS_PER_SEC);
	#endif
	
	diff = clock() - start;
	printf("Compilation took %f milliseconds.\n", 1000.0 * diff / CLOCKS_PER_SEC);
	
	return 1;
}
