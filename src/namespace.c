#include "postprocess.h"

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "ast.h"
#include "common.h"
#include "compile.h"



int buildProgramMap(Program* prog, ErrorList* errs){
	int pass = 1;
	for(int i = 0; i < prog->filect; i++){
		prog->files[i].names.modName = 0;
		prog->files[i].names.imports = makeList(sizeof(int64_t), 8);
		for(int j = 0; j < prog->files[i].prog.hds.size; j++){
			// Handle headers
			ASTHeader* hd = getList(&prog->files[i].prog.hds, j);
			switch(hd->bid){
				case BID_MODULE : {
					if(!prog->files[i].names.modName){
						int repeat = -1;
						for(int k = 0; k < i; k++){
							if(prog->files[k].names.modName == hd->sym) repeat = k;
							break;
						}
						if(repeat < 0){
							prog->files[i].names.modName = hd->sym;
						}else{
							// Error
							printf("Repeat module.\n");
							pass = 0;
						}
					}else{
						// Error
						printf("Overwriting module.\n");
						pass = 0;
					}
				}break;
				
				case BID_IMPORT : {
					int new = 1;
					for(int k = 0; k < prog->files[i].names.imports.size; k++){
						if(((int64_t*)prog->files[i].names.imports.array)[k] == hd->sym){
							new = 0;
							break;
						}
					}
					if(new){
						appendList(&prog->files[i].names.imports, &hd->sym);
					}else{
						// Error
						printf("Repeat import.\n");
						pass = 0;
					}
				}break;
				
				default: {
					// Error
					pass = 0;
				}break;
			}
		}
		if(pass == 0) continue;
	
		for(int j = 0; j < prog->files[i].prog.tys.size; j++){
			// Append Types
		}
		if(pass == 0) continue;
		
		for(int j = 0; j < prog->files[i].prog.fns.size; j++){
			// Append Functions
		}
		if(pass == 0) continue;
		
		for(int j = 0; j < prog->files[i].prog.cns.size; j++){
			// Handle constraints
		}
		if(pass == 0) continue;
	}
	
	return pass;
}



void printProgramMap(Program* prog){
	printSymbolTable(prog->syms);
	for(int i = 0; i < prog->filect; i++){
		printf("\n\n===============================\n");
		printf("FILE=%s, MOD=%s\nIMPORTS=", prog->files[i].path, searchSymbolId(&prog->syms, prog->files[i].names.modName));
		for(int j = 0; j < prog->files[i].names.imports.size; j++)
			printf("%s, ", searchSymbolId(&prog->syms, ((int64_t*)prog->files[i].names.imports.array)[j]));
		printf("\n");
		printASTProgram(prog->files[i].prog);
		printf("\n===============================\n");
	}
}


