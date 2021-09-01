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
		for(int j = 0; j < prog->files[i].prog.hds.size; j++){
			// Handle headers
			ASTHeader* hd = getList(&prog->files[i].prog.hds, j);
			switch(hd->bid){
				case BID_MODULE : {
					if(!prog->files[i].names.modName){
						// Set modname
					}else{
						// Error
						pass = 0;
					}
				}break;
				
				case BID_IMPORT : {
					// Add import
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
