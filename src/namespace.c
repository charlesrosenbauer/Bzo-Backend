#include "postprocess.h"

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#include "ast.h"
#include "common.h"
#include "compile.h"



IdTable makeIdTable(int size){
	if(size < 256) size = 256;
	if(__builtin_popcountl(size) != 1)
		size = 1l << (64 - __builtin_clzl(size));
	
	IdTable ret;
	ret.fnfill	= 0;
	ret.tyfill	= 0;
	ret.fnsize	= size;
	ret.tysize	= size;
	ret.fns   	= malloc(sizeof(Label) * size);
	ret.tys		= malloc(sizeof(Label) * size);
	for(int i	= 0; i < size; i++){
		ret.fns[i].id = 0;
		ret.tys[i].id = 0;
	}
	return ret;
}


int growLabelTable(Label** tab, int* tabsize){
	int size   = *tabsize;
	Label* tmp = *tab;
	*tab       = malloc(sizeof(Label) * size * 2);
	for(int i  = 0; i < size*2; i++) (*tab)[i].id = 0;
	for(int i  = 0; i < size;   i++){
		if(tmp[i].id != 0){
			uint64_t n = tmp[i].id * 11400714819323198485ul;
			n >>= __builtin_ctzl(size * 2);
			for(int j    = 0; j < size*2; j++){
				int ix   = (j + n) & ((size*2) - 1);
				if((*tab)[ix].id == 0){
					(*tab)[ix] = tmp[i];
					j          = (size*2);
				}
			}
		}
	}
	free(tmp);
	*tabsize  *= 2;
	return *tabsize;
}



int insertIdTableFn(IdTable* itab, int64_t id, int64_t mod, int64_t def){
	uint64_t n = id * 11400714819323198485ul;
	n >>= __builtin_ctzl(itab->fnsize);
	
	if(itab->fnfill+1 >= (itab->fnsize / 2))
		growLabelTable(&itab->fns, &itab->fnsize);
	
	for(int i  = 0; i < itab->fnsize; i++){
		int ix = (i + n) & (itab->fnsize - 1);
		if(itab->fns[ix].id == 0){
			itab->fns[ix].id   = id;
			itab->fns[ix].mods = makeList(4, sizeof(int64_t));
			itab->fns[ix].defs = makeList(4, sizeof(int64_t));
			appendList(&itab->fns[ix].mods, &mod);
			appendList(&itab->fns[ix].defs, &def);
			itab->fnfill++;
			return ix;
		}else if(itab->fns[ix].id == id){
			appendList(&itab->fns[ix].mods, &mod);
			appendList(&itab->fns[ix].defs, &def);
			return ix;
		}
	}
	
	return 0;
}

int insertIdTableTy(IdTable* itab, int64_t id, int64_t mod, int64_t def){
	uint64_t n = id * 11400714819323198485ul;
	n >>= __builtin_ctzl(itab->tysize);
	
	if(itab->tyfill+1 >= (itab->tysize / 2))
		growLabelTable(&itab->tys, &itab->tysize);
	
	for(int i  = 0; i < itab->tysize; i++){
		int ix = (i + ix) & (itab->tysize - 1);
		if(itab->tys[ix].id == 0){
			itab->tys[ix].id   = id;
			itab->tys[ix].mods = makeList(4, sizeof(int64_t));
			itab->tys[ix].defs = makeList(4, sizeof(int64_t));
			appendList(&itab->tys[ix].mods, &mod);
			appendList(&itab->tys[ix].defs, &def);
			itab->tyfill++;
			return ix;
		}else if(itab->tys[ix].id == id){
			appendList(&itab->tys[ix].mods, &mod);
			appendList(&itab->tys[ix].defs, &def);
			return ix;
		}
	}
	
	return 0;
}



int buildProgramMap(Program* prog, ErrorList* errs){
	int pass     = 1;
	prog->tydefs = makeList(sizeof(TypeDef), 256);
	prog->fndefs = makeList(sizeof(FuncDef), 256);
	prog->idtab  = makeIdTable(256);
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
							appendList(&errs->errs, &(Error){ERR_N_BAD_MODL, hd->pos});
							pass = 0;
						}
					}else{
						// Error
						appendList(&errs->errs, &(Error){ERR_N_RPT_MODL, hd->pos});
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
						appendList(&errs->errs, &(Error){ERR_N_RPT_IMPT, hd->pos});
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
		int64_t mod = prog->files[i].names.modName;
	
		for(int j = 0; j < prog->files[i].prog.tys.size; j++){
			// Append Types
			ASTTyDef* tast = getList(&prog->files[i].prog.tys, j);
			TypeDef   tdef = (TypeDef){tast, makeList(sizeof(int64_t), 4), tast->tyid, mod};
			int64_t    def = appendList(&prog->tydefs, &tdef);
			insertIdTableTy(&prog->idtab, tast->tyid, mod, def);
			
			// FIXME: something is broken here I think. Vec3 shows up in both test files, but only shows up in one namespace
		}
		
		for(int j = 0; j < prog->files[i].prog.fns.size; j++){
			// Append Functions
			ASTFnDef* fast = getList(&prog->files[i].prog.fns, j);
			FuncDef   fdef = (FuncDef){fast, makeList(sizeof(int64_t), 4), fast->fnid, mod};
			int64_t    def = appendList(&prog->fndefs, &fdef);
			insertIdTableFn(&prog->idtab, fast->fnid, mod, def);
		}
		
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
	printf("TYPES=\n");
	for(int i = 0; i < prog->tydefs.size; i++){
		TypeDef* t = getList(&prog->tydefs, i);
		printf("%i : %p %li %li\n", i, t->astdef, t->id, t->mod);
	}
	printf("\n");
	for(int i = 0; i < prog->idtab.tysize; i++){
		Label l   = prog->idtab.tys[i];
		if(l.id  != 0){
			printf("%i| T%li [", i, l.id);
			int64_t* ms = l.mods.array;
			int64_t* ds = l.defs.array;
			for(int   j = 0; j < l.mods.size; j++) printf("%li:%li, ", ms[j], ds[j]);
			printf("]\n");
		}
	}
	printf("\n");
		
	printf("FUNCS=\n");
	for(int i = 0; i < prog->fndefs.size; i++){
		FuncDef* f = getList(&prog->fndefs, i);
		printf("%i : %p %li %li\n", i, f->astdef, f->id, f->mod);
	}
	printf("\n");
	for(int i = 0; i < prog->idtab.fnsize; i++){
		Label l  = prog->idtab.fns[i];
		if(l.id  != 0){
			printf("%i| F%li [", i, l.id);
			int64_t* ms = l.mods.array;
			int64_t* ds = l.defs.array;
			for(int   j = 0; j < l.mods.size; j++) printf("%li:%li, ", ms[j], ds[j]);
			printf("]\n");
		}
	}
	printf("\n");
}


